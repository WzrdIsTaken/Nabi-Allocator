// inl's Header
#include "Allocators\FreeListAllocator\FreeListAllocator.h"

// Config Headers
#include "Config.h"

// Nabi Headers
#include "Allocators\AllocatorBlockInfo.h"
#include "Allocators\BlockInfo.h"
#include "Allocators\BlockInfoIndex.h"
#include "Allocators\BlockPadding.h"
#include "Allocators\FreeListAllocator\FreeListNode.h"
#include "Allocators\FreeListAllocator\SearchAlgorithms.h"
#include "DebugUtils.h"
#include "HeapZone\HeapZoneInfo.h"
#include "Operations\BitOperations.h"
#include "Operations\MemoryOperations.h"

namespace nabi_allocator::free_list_allocator
{
	// The size of blocks of memory have to be multiples of 8. This is because then we can always
	// mask of 3 bits of the size to store other infomation and still retain the size of the block.
	// Allignments of 8 also always work for x64 and x86 architecture. 
	static uInt constexpr c_BlockAllignment = 8u;
	static uInt constexpr c_MinBlockSize = c_BlockHeaderSize + c_FreeListNodeSize + c_BlockFooterSize + 
		((c_BlockHeaderSize + c_FreeListNodeSize + c_BlockFooterSize) % c_BlockAllignment);

	template<FreeListAllocatorSettings Settings>
	FreeListAllocator<Settings>::FreeListAllocator(HeapZoneInfo const& heapZoneInfo)
		: AllocatorBase{}
		, m_FreeList(nullptr)
	{
		uInt const heapZoneSize = memory_operations::GetMemorySize(heapZoneInfo.m_Start, heapZoneInfo.m_End);
		NA_ASSERT_DEFAULT(memory_operations::IsAlligned(heapZoneSize, c_BlockAllignment));
		NA_ASSERT_DEFAULT(heapZoneSize > c_MinBlockSize);

		AddFreeBlock(NA_TO_VPTR(heapZoneInfo.m_Start), heapZoneSize);
	}

	template<FreeListAllocatorSettings Settings>
	FreeListAllocator<Settings>::~FreeListAllocator()
	{
#ifdef NA_TRACK_ALLOCATIONS
		NA_ASSERT(m_AllocatorStats.m_ActiveAllocationCount == 0u && m_AllocatorStats.m_ActiveBytesAllocated == 0u,
			NA_NAMEOF_LITERAL(FreeListAllocator) << "'s destructor was called, but objects it allocated are still active");
#endif // ifdef NA_TRACK_ALLOCATIONS
	}

	template<FreeListAllocatorSettings Settings>
	void* FreeListAllocator<Settings>::Allocate(uInt const numBytes, HeapZoneInfo const& heapZoneInfo)
	{
		NA_ASSERT(numBytes > 0u, "Allocating 0 bytes");

		// Search for a free block. If one can't be found, the allocator is out of memory
		BlockHeader* blockHeader = nullptr;
		uInt allocatedBytes = 0u;

		uInt requiredBlockSize = c_BlockHeaderSize + c_BlockFooterSize + numBytes;
		uInt padding = 0u;

		if (numBytes < c_FreeListNodeSize)
		{
			padding += (c_FreeListNodeSize - numBytes);
		}
		padding += (requiredBlockSize + padding) % c_BlockAllignment;

		requiredBlockSize += padding;
		bool requiresPadding = padding != 0u;

		blockHeader = TryFindFreeBlock(requiredBlockSize);
		NA_ASSERT(blockHeader, NA_NAMEOF_LITERAL(FreeListAllocator) " is out of memory");

		uInt const originalBlockSize = bit_operations::RightShiftBit(blockHeader->m_BlockInfo, type_utils::ToUnderlying(BlockInfoIndex::SizeStart));
		RemoveFreeBlock(blockHeader);

		// If the block is too large for the requested bytes, check that the remaining space after the block has been split is sufficient for another allocation
		uInt const unusedSpace = originalBlockSize - requiredBlockSize;
		bool const remainingBlockSpaceSufficient = unusedSpace >= c_MinBlockSize;

		if (!remainingBlockSpaceSufficient && unusedSpace != 0u)
		{
			requiredBlockSize = originalBlockSize; // We are now filling all the space in the block as it can't be split
			padding += unusedSpace;
			requiresPadding = true;
		}

		// Set the block header (the memory for it has already been reinterpret casted)
		BlockInfoContent blockInfoContent = {};
		blockInfoContent.m_Allocated = true;
		blockInfoContent.m_Padded = requiresPadding;
		blockInfoContent.m_NumBytes = requiredBlockSize;
		LoadBlockInfo(blockInfoContent, *blockHeader);

#ifdef NA_MEMORY_TAGGING
		// TODO Memory Tagging here (MemoryTagScope, get from the MemoryCommand singleton)
#endif // ifdef NA_MEMORY_TAGGING
		allocatedBytes += c_BlockHeaderSize;

		// Store the pointer to where the payload will be allocated
		void* const payloadPtr = NA_REINTERPRET_MEMORY(void, blockHeader, +, allocatedBytes);
		allocatedBytes += numBytes;

		// Allocate the padding if needed
		if (requiresPadding)
		{
			// We want to allocate the padding one byte before the footer. 
			// Its ok to do padding - c_BlockPaddingSize, because if "requiresPadding" is true, "padding" will always be > 1 and c_BlockPaddingSize is 1.
			uInt const blockPaddingStructPosition = padding - c_BlockPaddingSize;
			BlockPadding* const blockPadding = NA_REINTERPRET_MEMORY(BlockPadding, blockHeader, +, (allocatedBytes + blockPaddingStructPosition));

			blockPadding->m_Padding = static_cast<u8>(padding);
			allocatedBytes += padding;
		}

		// Allocate the footer
		BlockFooter* const blockFooter = NA_REINTERPRET_MEMORY(BlockFooter, blockHeader, +, allocatedBytes);
		blockFooter->m_BlockInfo = blockHeader->m_BlockInfo;
		allocatedBytes += c_BlockFooterSize;

		// Split the original memory block if it was of sufficient size
		if (remainingBlockSpaceSufficient)
		{
			void* const newFreeBlockStart = NA_REINTERPRET_MEMORY(void, blockFooter, +, c_BlockFooterSize);
			AddFreeBlock(newFreeBlockStart, unusedSpace);
		}

		// Update stats
#ifdef NA_TRACK_ALLOCATIONS
		u64 constexpr allocationCountAdjustment = 1u;
		UpdateAllocatorStats(m_AllocatorStats, AllocatorStatsUpdateType::Allocate, allocationCountAdjustment, static_cast<u64>(requiredBlockSize));
#endif // #ifdef NA_TRACK_ALLOCATIONS

		return payloadPtr;
	}

	template<FreeListAllocatorSettings Settings>
	void FreeListAllocator<Settings>::Free(void* memory, HeapZoneInfo const& heapZoneInfo)
	{
		NA_ASSERT(memory, "Cannot free nullptr");
		NA_ASSERT(memory_operations::IsPtrInRange(heapZoneInfo.m_Start, heapZoneInfo.m_End,
			NA_TO_UPTR(memory)), "Trying to release memory not managed by this allocator");

		// Get the payload's block's header
		BlockHeader* const blockHeader = NA_REINTERPRET_MEMORY(BlockHeader, memory, -, c_BlockHeaderSize);

		// Update stats
#ifdef NA_TRACK_ALLOCATIONS
		u64 constexpr allocationCountAdjustment = 1u;
		uInt const blockSize = bit_operations::RightShiftBit(blockHeader->m_BlockInfo, type_utils::ToUnderlying(BlockInfoIndex::SizeStart));
		UpdateAllocatorStats(m_AllocatorStats, AllocatorStatsUpdateType::Free, allocationCountAdjustment, static_cast<u64>(blockSize));
#endif // #ifdef NA_TRACK_ALLOCATIONS

		// Begin the free process
		TryCoalesceBlock(blockHeader, heapZoneInfo);
	}

	template<FreeListAllocatorSettings Settings>
	std::deque<AllocatorBlockInfo> FreeListAllocator<Settings>::IterateThroughHeapZone(
		std::optional<std::function<bool(AllocatorBlockInfo const&)>> action, HeapZoneInfo const& heapZoneInfo) const
	{
		std::deque<AllocatorBlockInfo> allocatorBlocks = {};
		uInt progressThroughHeapZone = static_cast<uInt>(heapZoneInfo.m_Start);
		BlockHeader const* blockHeader = nullptr;
		BlockInfoContent blockInfoContent = {};

		// Loop through all the blocks in the heap zone until the end or "action" returns false
		do
		{
			blockHeader = NA_REINTERPRET_MEMORY_DEFAULT(BlockHeader, static_cast<uPtr>(progressThroughHeapZone));
			UnloadBlockInfo(blockInfoContent, *blockHeader);
			progressThroughHeapZone += blockInfoContent.m_NumBytes;

			// Store the block's infomation in allocatorBlockInfo
			AllocatorBlockInfo& allocatorBlockInfo = allocatorBlocks.emplace_back();
			allocatorBlockInfo.m_MemoryAddress = NA_TO_UPTR(blockHeader);
#ifdef NA_MEMORY_TAGGING
			allocatorBlockInfo.m_MemoryTag = blockHeader->m_MemoryTag;
#endif // ifdef NA_MEMORY_TAGGING

			allocatorBlockInfo.m_PayloadPtr = NA_REINTERPRET_MEMORY(void, blockHeader, +, c_BlockHeaderSize);
			allocatorBlockInfo.m_Allocated = blockInfoContent.m_Allocated;
			allocatorBlockInfo.m_Padded = blockInfoContent.m_Padded;
			
			u32 padding = 0u;
			if (allocatorBlockInfo.m_Padded)
			{
				BlockPadding const* const blockPadding = 
					NA_REINTERPRET_MEMORY(BlockPadding, blockHeader, +, (blockInfoContent.m_NumBytes - c_BlockFooterSize - c_BlockPaddingSize));
				padding = static_cast<u32>(blockPadding->m_Padding);
			}

			allocatorBlockInfo.m_NumBytes = blockInfoContent.m_NumBytes;
			allocatorBlockInfo.m_Padding = padding;

			// Check if the loop should continue
			bool const continueLooping = action ? (*action)(allocatorBlockInfo) : true;
			if (!continueLooping)
			{
				break;
			}
		}
		while (progressThroughHeapZone < heapZoneInfo.m_End);

		return allocatorBlocks;
	}

	template<FreeListAllocatorSettings Settings>
	BlockHeader* const FreeListAllocator<Settings>::TryFindFreeBlock(uInt const numBytes) const
	{
		BlockHeader* freeBlock = nullptr;

		if constexpr (Settings.m_SearchAlgorithm == SearchAlgorithm::BestFit)
		{
			freeBlock = FindViaBestFit(m_FreeList, numBytes, Settings.m_BestFitLeniency);
		}
		else if constexpr (Settings.m_SearchAlgorithm == SearchAlgorithm::FirstFit)
		{
			freeBlock = FindViaFirstFit(m_FreeList, numBytes);
		}
		else if constexpr (Settings.m_SearchAlgorithm == SearchAlgorithm::NextFit)
		{
			freeBlock = FindViaNextFit(m_FreeList, numBytes);
		}
		else
		{
			NA_ASSERT_FAIL("Using an unexpected " << NA_NAMEOF_LITERAL(SearchAlgorithm));
		}

		// If no suitable block was found, the allocator is out of memory
		return freeBlock;
	}

	template<FreeListAllocatorSettings Settings>
	void FreeListAllocator<Settings>::TryCoalesceBlock(BlockHeader* const blockHeader, HeapZoneInfo const& heapZoneInfo)
	{
		NA_ASSERT(blockHeader, NA_NAMEOF_LITERAL(blockHeader) " is null");

		uInt const blockSize = bit_operations::RightShiftBit(blockHeader->m_BlockInfo, type_utils::ToUnderlying(BlockInfoIndex::SizeStart));
		uPtr const blockAddress = NA_TO_UPTR(blockHeader);
		uInt newBlockSize = blockSize;
		uInt leftExtent = 0u;

		// Attempt to coalesce with right neighbor
		if ((blockAddress + blockSize) != heapZoneInfo.m_End)
		{
			BlockHeader* const rightBlockHeader = NA_REINTERPRET_MEMORY(BlockHeader, blockHeader, +, blockSize);
			bool const rightBlockFree = !bit_operations::GetBit(rightBlockHeader->m_BlockInfo, type_utils::ToUnderlying(BlockInfoIndex::Allocated));

			if (rightBlockFree)
			{
				uInt const rightBlockSize = bit_operations::RightShiftBit(rightBlockHeader->m_BlockInfo, type_utils::ToUnderlying(BlockInfoIndex::SizeStart));
				newBlockSize += rightBlockSize;
				RemoveFreeBlock(rightBlockHeader);
			}
		}

		// Attempt to coalesce with left neighbor
		if (blockAddress != heapZoneInfo.m_Start)
		{
			BlockFooter* const leftBlockFooter = NA_REINTERPRET_MEMORY(BlockFooter, blockHeader, -, c_BlockFooterSize);
			bool const leftBlockFree = !bit_operations::GetBit(leftBlockFooter->m_BlockInfo, type_utils::ToUnderlying(BlockInfoIndex::Allocated));

			if (leftBlockFree)
			{
				uInt const leftBlockSize = bit_operations::RightShiftBit(leftBlockFooter->m_BlockInfo, type_utils::ToUnderlying(BlockInfoIndex::SizeStart));
				leftExtent = leftBlockSize;
				newBlockSize += leftBlockSize;

				void* const leftblockStartPtr = NA_REINTERPRET_MEMORY(void, leftBlockFooter, -, (leftBlockSize - c_BlockFooterSize));
				RemoveFreeBlock(leftblockStartPtr);
			}
		}

		// Mark the block as memory as a free block
		void* const leftExtentPtr = NA_REINTERPRET_MEMORY(void, blockHeader, -, leftExtent);
		AddFreeBlock(leftExtentPtr, newBlockSize);
	}

	template<FreeListAllocatorSettings Settings>
	void FreeListAllocator<Settings>::AddFreeBlock(void* const blockStartPtr, uInt const numBytes)
	{
		NA_ASSERT(blockStartPtr, NA_NAMEOF_LITERAL(blockStartPtr) " is null");
		NA_ASSERT(numBytes >= c_MinBlockSize, "Trying to add a free block with size " << numBytes << ". The minimum size is " << c_MinBlockSize);

		// Add the memory structures (header/footer)
		BlockHeader* const freeBlockHeader = NA_REINTERPRET_MEMORY_DEFAULT(BlockHeader, blockStartPtr);
		BlockFooter* const freeBlockFooter = NA_REINTERPRET_MEMORY(BlockFooter, blockStartPtr, +, (numBytes - c_BlockFooterSize));

		BlockInfoContent blockInfoContent = {};
		blockInfoContent.m_Allocated = false;
		blockInfoContent.m_Padded = false;
		blockInfoContent.m_NumBytes = numBytes;
		LoadBlockInfo(blockInfoContent, *freeBlockHeader, freeBlockFooter);

		// Add the free list node
		FreeListNode* const freeListNode = NA_REINTERPRET_MEMORY(FreeListNode, freeBlockHeader, +, c_BlockHeaderSize);
		ResetFreeListNode(freeListNode); // Its important to set this to nullptr, as AddFreeListNode spins through all the nodes until !currentNode->Next
		                                 // This would invalidate the free list and cause a crash in SearchAlgorithm::FindVia[algorithm name]
		AddFreeListNode(m_FreeList, freeListNode);
	}

	template<FreeListAllocatorSettings Settings>
	void FreeListAllocator<Settings>::RemoveFreeBlock(void* const blockStartPtr)
	{
		NA_ASSERT(blockStartPtr, NA_NAMEOF_LITERAL(blockStartPtr) " is null");

		// Find and remove the free list node
		FreeListNode* const freeListNode = NA_REINTERPRET_MEMORY(FreeListNode, blockStartPtr, +, c_BlockHeaderSize);
		RemoveFreeListNode(m_FreeList, freeListNode);
	}
} // namespace nabi_allocator::free_list_allocator
