// inl's Header
#include "Allocators\FreeListAllocator\FreeListAllocator.h"

// STD Headers
#include <limits>

// Config Headers
#include "Config.h"

// Nabi Headers
#include "Allocators\AllocatorBlockInfo.h"
#include "Allocators\FreeListAllocator\BlockInfo.h"
#include "Allocators\FreeListAllocator\BlockInfoIndex.h"
#include "Allocators\FreeListAllocator\BlockPadding.h"
#include "Allocators\FreeListAllocator\FreeListNode.h"
#include "Allocators\FreeListAllocator\SearchAlgorithm.h"
#include "DebugUtils.h"
#include "HeapZone\HeapZoneInfo.h"
#include "Operations\BitOperations.h"
#include "Operations\MemoryOperations.h"

namespace nabi_allocator::free_list_allocator
{
	// The size of blocks of memory have to be multiples of 8. This is because then we can always
	// mask of 3 bits of the size to store other infomation and still retain the size of the block.
	// Allignments of 8 also always work for x64 and x64 architecture. 
	static uInt constexpr c_BlockAllignment = 8u;
	static uInt constexpr c_MinBlockSize = c_BlockHeaderSize + c_FreeListNodeSize + c_BlockFooterSize;

	template<FreeListAllocatorSettings Settings>
	FreeListAllocator<Settings>::FreeListAllocator(HeapZoneInfo const& heapZoneInfo)
		: m_FreeList(nullptr)
	{
		uInt const heapZoneSize = memory_operations::GetMemorySize(heapZoneInfo.m_Start, heapZoneInfo.m_End);
		NABI_ALLOCATOR_ASSERT_DEFAULT(memory_operations::IsAlligned(heapZoneSize, c_BlockAllignment));
		NABI_ALLOCATOR_ASSERT_DEFAULT(heapZoneSize >= c_MinBlockSize);

		AddFreeBlock(NABI_ALLOCATOR_TO_VPTR(heapZoneInfo.m_Start), heapZoneSize);
	}

	template<FreeListAllocatorSettings Settings>
	FreeListAllocator<Settings>::~FreeListAllocator()
	{
#ifdef NABI_ALLOCATOR_TRACK_ALLOCATIONS
		NABI_ALLOCATOR_ASSERT(m_AllocatorStats.m_ActiveAllocationCount == 0u && m_AllocatorStats.m_ActiveBytesAllocated == 0u,
			NABI_ALLOCATOR_NAMEOF_LITERAL(FreeListAllocator) << "'s destructor was called, but objects it allocated are still active");
#endif // ifdef NABI_ALLOCATOR_TRACK_ALLOCATIONS
	}

	template<FreeListAllocatorSettings Settings>
	void* FreeListAllocator<Settings>::Allocate(uInt const numBytes, HeapZoneInfo const& heapZoneInfo)
	{
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
		NABI_ALLOCATOR_ASSERT(blockHeader, NABI_ALLOCATOR_NAMEOF_LITERAL(FreeListAllocator) " is out of memory");

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

#ifdef NABI_ALLOCATOR_MEMORY_TAGGING
		// TODO Memory Tagging here (MemoryTagScope, get from the MemoryCommand singleton)
#endif // ifdef NABI_ALLOCATOR_MEMORY_TAGGING
		allocatedBytes += c_BlockHeaderSize;

		// Store the pointer to where the payload will be allocated
		void* const payloadPtr = NABI_ALLOCATOR_REINTERPRET_MEMORY(void, blockHeader, +, allocatedBytes);
		allocatedBytes += numBytes;

		// Allocate the padding if needed
		if (requiresPadding)
		{
			// We want to allocate the padding one byte before the footer. 
			// Its ok to do padding - c_BlockPaddingSize, because if "requiresPadding" is true, "padding" will always be > 1 and c_BlockPaddingSize is 1.
			uInt const blockPaddingStructPosition = padding - c_BlockPaddingSize;
			BlockPadding* const blockPadding = NABI_ALLOCATOR_REINTERPRET_MEMORY(BlockPadding, blockHeader, +, (allocatedBytes + blockPaddingStructPosition));

			blockPadding->m_Padding = static_cast<u8>(padding);
			allocatedBytes += padding;
		}

		// Allocate the footer
		BlockFooter* const blockFooter = NABI_ALLOCATOR_REINTERPRET_MEMORY(BlockFooter, blockHeader, +, allocatedBytes);
		blockFooter->m_BlockInfo = blockHeader->m_BlockInfo;
		allocatedBytes += c_BlockFooterSize;

		// Split the original memory block if it was of sufficient size
		if (remainingBlockSpaceSufficient)
		{
			void* const newFreeBlockStart = NABI_ALLOCATOR_REINTERPRET_MEMORY(void, blockFooter, +, c_BlockFooterSize);
			AddFreeBlock(newFreeBlockStart, unusedSpace);
		}

		// Update stats
#ifdef NABI_ALLOCATOR_TRACK_ALLOCATIONS
		u64 constexpr allocationCountAdjustment = 1u;
		UpdateAllocatorStats(m_AllocatorStats, AllocatorStatsUpdateType::Allocate, allocationCountAdjustment, static_cast<u64>(requiredBlockSize));
#endif // #ifdef NABI_ALLOCATOR_TRACK_ALLOCATIONS

		return payloadPtr;
	}

	template<FreeListAllocatorSettings Settings>
	void FreeListAllocator<Settings>::Free(void* memory, HeapZoneInfo const& heapZoneInfo)
	{
		NABI_ALLOCATOR_ASSERT(memory, "Cannot free nullptr");
		NABI_ALLOCATOR_ASSERT(memory_operations::IsPtrInRange(heapZoneInfo.m_Start, heapZoneInfo.m_End,
			NABI_ALLOCATOR_TO_UPTR(memory)), "Trying to release memory not managed by this allocator");

		// Get the payload's block's header
		BlockHeader* const blockHeader = NABI_ALLOCATOR_REINTERPRET_MEMORY(BlockHeader, memory, -, c_BlockHeaderSize);

		// Update stats
#ifdef NABI_ALLOCATOR_TRACK_ALLOCATIONS
		u64 constexpr allocationCountAdjustment = 1u;
		uInt const blockSize = bit_operations::RightShiftBit(blockHeader->m_BlockInfo, type_utils::ToUnderlying(BlockInfoIndex::SizeStart));
		UpdateAllocatorStats(m_AllocatorStats, AllocatorStatsUpdateType::Allocate, allocationCountAdjustment, static_cast<u64>(blockSize));
#endif // #ifdef NABI_ALLOCATOR_TRACK_ALLOCATIONS

		// Begin the free process
		TryCoalesceBlock(blockHeader, heapZoneInfo);
	}

	template<FreeListAllocatorSettings Settings>
	inline std::deque<AllocatorBlockInfo> FreeListAllocator<Settings>::IterateThroughMemoryPool(
		std::optional<std::function<bool(AllocatorBlockInfo const&)>> action, HeapZoneInfo const& heapZoneInfo)
	{
		std::deque<AllocatorBlockInfo> allocatorBlocks = {};
		BlockHeader const* blockHeader = nullptr;
		BlockInfoContent blockInfoContent = {};

		// Loop through all the blocks in the heap zone until the end or "action" returns false
		do
		{
			blockHeader = NABI_ALLOCATOR_REINTERPRET_MEMORY(BlockHeader, blockHeader, +, blockInfoContent.m_NumBytes);
			UnloadBlockInfo(&blockInfoContent, blockHeader);

			// Store the block's infomation in allocatorBlockInfo
			AllocatorBlockInfo& allocatorBlockInfo = allocatorBlocks.emplace_back();
			allocatorBlockInfo.m_MemoryAddress = NABI_ALLOCATOR_TO_UPTR(blockHeader);
#ifdef NABI_ALLOCATOR_MEMORY_TAGGING
			allocatorBlockInfo.m_MemoryTag = blockHeader->m_MemoryTag;
#endif // ifdef NABI_ALLOCATOR_MEMORY_TAGGING

			allocatorBlockInfo.m_Allocated = blockInfoContent.m_Allocated;
			allocatorBlockInfo.m_Padded = blockInfoContent.m_Padded;
			
			u32 padding = 0u;
			if (allocatorBlockInfo.m_Padded)
			{
				BlockPadding const* const blockPadding = 
					NABI_ALLOCATOR_REINTERPRET_MEMORY(BlockPadding, blockHeader, +, (blockInfoContent.m_NumBytes - c_BlockFooterSize - c_BlockPaddingSize));
				padding = static_cast<u32>(blockPadding.m_Padding);
			}

			allocatorBlockInfo.m_NumBytes = blockInfoContent.m_NumBytes;
			allocatorBlockInfo.m_Padding = padding;

			// Check if the loop should continue
			bool const continuecontinueLooping = action ? (*action)(allocatorBlockInfo) : true;
			if (!continueLooping)
			{
				break;
			}
		}
		while (allocatorBlocks.back().m_MemoryAddress + blockInfoContent.m_NumBytes) <= heapZoneInfo.m_End);

		return allocatorBlocks;
	}

	template<FreeListAllocatorSettings Settings>
	BlockHeader* const FreeListAllocator<Settings>::TryFindFreeBlock(uInt const numBytes) const
	{
		if constexpr (Settings.m_SearchAlgorithm == SearchAlgorithm::BestFit)
		{
			// Iterate through the free list to try and find a block with a size as close as possible to numBytes.
			FreeListNode* freeListNode = m_FreeList;
			BlockHeader* bestBlock = nullptr;
			uInt bestBlockSize = std::numeric_limits<uInt>::max();

			while (freeListNode)
			{
				BlockHeader* const freeBlockHeader = NABI_ALLOCATOR_REINTERPRET_MEMORY(BlockHeader, freeListNode, -, c_BlockHeaderSize);
				uInt const freeBlockNumBytes = bit_operations::RightShiftBit(freeBlockHeader->m_BlockInfo, type_utils::ToUnderlying(BlockInfoIndex::SizeStart));

				if (freeBlockNumBytes >= numBytes && numBytes <= (freeBlockNumBytes + Settings.m_BestFitLeniency))
				{
					return freeBlockHeader;
				}
				else
				{
					if (freeBlockNumBytes >= numBytes && freeBlockNumBytes < bestBlockSize)
					{
						bestBlock = freeBlockHeader;
						bestBlockSize = freeBlockNumBytes;
					}
				}
			}

			return bestBlock;
		}
		else if constexpr (Settings.m_SearchAlgorithm == SearchAlgorithm::FirstFit)
		{
			// Iterate through the free list until we find a block with size greater than or equal to numBytes. 
			FreeListNode* freeListNode = m_FreeList;

			while (freeListNode)
			{
				BlockHeader* const freeBlockHeader = NABI_ALLOCATOR_REINTERPRET_MEMORY(BlockHeader, freeListNode, -, c_BlockHeaderSize);
				uInt const freeBlockNumBytes = bit_operations::RightShiftBit(freeBlockHeader->m_BlockInfo, type_utils::ToUnderlying(BlockInfoIndex::SizeStart));

				if (numBytes <= freeBlockNumBytes)
				{
					return freeBlockHeader;
				}
				else
				{
					freeListNode = freeListNode->m_Next;
				}
			}

			return nullptr;
		}
		else if constexpr (Settings.m_SearchAlgorithm == SearchAlgorithm::NextFit)
		{
			NABI_ALLOCATOR_ASSERT_FAIL(NABI_ALLOCATOR_NAMEOF_LITERAL(SearchAlgorithm::NextFit) << "is not currently supported");
		}
		else
		{
			NABI_ALLOCATOR_ASSERT_FAIL("Using an unexpected " << NABI_ALLOCATOR_NAMEOF_LITERAL(SearchAlgorithm));
		}

		// If no suitable block was found, the allocator is out of memory
		return nullptr;
	}

	template<FreeListAllocatorSettings Settings>
	void FreeListAllocator<Settings>::TryCoalesceBlock(BlockHeader* const blockHeader, HeapZoneInfo const& heapZoneInfo)
	{
		NABI_ALLOCATOR_ASSERT(blockHeader, NABI_ALLOCATOR_NAMEOF_LITERAL(blockHeader) " is null");

		uInt const blockSize = bit_operations::RightShiftBit(blockHeader->m_BlockInfo, type_utils::ToUnderlying(BlockInfoIndex::SizeStart));
		uPtr const blockAddress = NABI_ALLOCATOR_TO_UPTR(blockHeader);
		uInt newBlockSize = blockSize;
		uInt leftExtent = 0u;

		// Attempt to coalesce with right neighbor
		if ((blockAddress + blockSize) <= heapZoneInfo.m_End)
		{
			BlockHeader* const rightBlockHeader = NABI_ALLOCATOR_REINTERPRET_MEMORY(BlockHeader, blockHeader, +, blockSize);
			bool const rightBlockFree = !bit_operations::GetBit(rightBlockHeader->m_BlockInfo, type_utils::ToUnderlying(BlockInfoIndex::Allocated));

			if (rightBlockFree)
			{
				uInt const rightBlockSize = bit_operations::RightShiftBit(rightBlockHeader->m_BlockInfo, type_utils::ToUnderlying(BlockInfoIndex::SizeStart));
				newBlockSize += rightBlockSize;
				RemoveFreeBlock(rightBlockHeader);
			}
		}

		// Attempt to coalesce with left neighbor
		if ((blockAddress - blockSize) >= heapZoneInfo.m_Start)
		{
			BlockFooter* const leftBlockFooter = NABI_ALLOCATOR_REINTERPRET_MEMORY(BlockFooter, blockHeader, -, c_BlockFooterSize);
			bool const leftBlockFree = !bit_operations::GetBit(leftBlockFooter->m_BlockInfo, type_utils::ToUnderlying(BlockInfoIndex::Allocated));

			if (leftBlockFree)
			{
				uInt const leftBlockSize = bit_operations::RightShiftBit(leftBlockFooter->m_BlockInfo, type_utils::ToUnderlying(BlockInfoIndex::SizeStart));
				leftExtent = leftBlockSize;
				newBlockSize += leftBlockSize;

				void* const leftblockStartPtr = NABI_ALLOCATOR_REINTERPRET_MEMORY(void, leftBlockFooter, -, (blockSize - c_BlockFooterSize));
				RemoveFreeBlock(leftblockStartPtr);
			}
		}

		// Mark the block as memory as a free block
		void* const leftExtentPtr = NABI_ALLOCATOR_REINTERPRET_MEMORY(void, blockHeader, -, leftExtent);
		memory_operations::ResetMemory(leftExtentPtr, newBlockSize);
		AddFreeBlock(leftExtentPtr, newBlockSize);
	}

	template<FreeListAllocatorSettings Settings>
	void FreeListAllocator<Settings>::AddFreeListNode(FreeListNode* const node)
	{
		NABI_ALLOCATOR_ASSERT(node, NABI_ALLOCATOR_NAMEOF_LITERAL(node) " is null");

		// If there are no free blocks, this block becomes the head of the list
		if (!m_FreeList)
		{
			m_FreeList = node;
			return;
		}

		// If the memory of block is before the memory of the list's head, then make this block the new head to ensure cache performance
		uPtr const nodeAddress = NABI_ALLOCATOR_TO_UPTR(node);
		if (nodeAddress < NABI_ALLOCATOR_TO_UPTR(m_FreeList))
		{
			node->m_Next = m_FreeList;
			m_FreeList->m_Previous = node;
			m_FreeList = node;
			return;
		}

		// Otherwise iterate through the free blocks until we find a place where the previous node's memory address is smaller and the next node's is bigger
		FreeListNode* currentNode = m_FreeList;

		while (currentNode->m_Next)
		{
			if (nodeAddress > NABI_ALLOCATOR_TO_UPTR(currentNode) &&
				nodeAddress < NABI_ALLOCATOR_TO_UPTR(currentNode->m_Next))
			{
				node->m_Next = currentNode->m_Next;
				currentNode->m_Next->m_Previous = node;
				node->m_Previous = currentNode;
				currentNode->m_Next = node;
				return;
			}
			else
			{
				currentNode = currentNode->m_Next;
			}
		}

		// If no place between two blocks was found, the memory location of this block must be greater than those in the list. Add it to the end
		currentNode->m_Next = node;
		node->m_Previous = currentNode;
	}

	template<FreeListAllocatorSettings Settings>
	void FreeListAllocator<Settings>::AddFreeBlock(void* const blockStartPtr, uInt const numBytes)
	{
		NABI_ALLOCATOR_ASSERT(blockStartPtr, NABI_ALLOCATOR_NAMEOF_LITERAL(blockStartPtr) " is null");
		NABI_ALLOCATOR_ASSERT(numBytes >= c_MinBlockSize, "Trying to add a free block with size " << numBytes << ". The minimum size is " << c_MinBlockSize);

		// Add the memory structures (header/footer)
		BlockHeader* const freeBlockHeader = NABI_ALLOCATOR_REINTERPRET_MEMORY_DEFAULT(BlockHeader, blockStartPtr);
		BlockFooter* const freeBlockFooter = NABI_ALLOCATOR_REINTERPRET_MEMORY(BlockFooter, blockStartPtr, +, (numBytes - c_BlockFooterSize));

		BlockInfoContent blockInfoContent = {};
		blockInfoContent.m_Allocated = false;
		blockInfoContent.m_Padded = false;
		blockInfoContent.m_NumBytes = numBytes;
		LoadBlockInfo(blockInfoContent, *freeBlockHeader, freeBlockFooter);

		// Add the free list node
		FreeListNode* const freeListNode = NABI_ALLOCATOR_REINTERPRET_MEMORY(FreeListNode, freeBlockHeader, +, c_BlockHeaderSize);
		memory_operations::ResetMemory(freeListNode);
		AddFreeListNode(freeListNode);
	}

	template<FreeListAllocatorSettings Settings>
	void FreeListAllocator<Settings>::RemoveFreeBlock(void* const blockStartPtr)
	{
		NABI_ALLOCATOR_ASSERT(blockStartPtr, NABI_ALLOCATOR_NAMEOF_LITERAL(blockStartPtr) " is null");

		// Find the free list node
		FreeListNode* const freeListNode = NABI_ALLOCATOR_REINTERPRET_MEMORY(FreeListNode, blockStartPtr, +, c_BlockHeaderSize);

		// If the free list node is the free list, then there are no more free blocks
		if (freeListNode == m_FreeList)
		{
			m_FreeList = nullptr;
		}
		else
		{
			// Otherwise, rewire the other nodes in the linked list
			if (freeListNode->m_Previous)
			{
				freeListNode->m_Previous->m_Next = freeListNode->m_Next;
			}
			if (freeListNode->m_Next)
			{
				freeListNode->m_Next->m_Previous = freeListNode->m_Previous;
			}
		}

		// Reset the free list node
		memory_operations::ResetMemory(freeListNode);
	}
} // namespace nabi_allocator::free_list_allocator
