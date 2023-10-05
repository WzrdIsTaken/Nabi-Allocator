// inl's Header
#include "Allocators\StackAllocator\StackAllocator.h"

// STD Headers
#include <algorithm>

// Nabi Headers
#include "Allocators\AllocatorBlockInfo.h"
#include "Allocators\BlockInfo.h"
#include "Allocators\BlockInfoIndex.h"
#include "Allocators\BlockPadding.h"
#include "DebugUtils.h"
#include "HeapZone\HeapZoneInfo.h"
#include "Operations\BitOperations.h"
#include "Operations\MemoryOperations.h"

namespace nabi_allocator::stack_allocator
{
	static uInt constexpr c_BlockAllignment = 8u;
	static uInt constexpr c_MinBlockSize = c_BlockAllignment + c_BlockHeaderSize +
		((c_BlockAllignment + c_BlockHeaderSize) % c_BlockAllignment);

	template<StackAllocatorSettings Settings>
	StackAllocator<Settings>::StackAllocator(HeapZoneInfo const& heapZoneInfo)
		: AllocatorBase{}
		, m_CurrentPosition(heapZoneInfo.m_Start)
#ifdef NA_DEBUG
		, m_PreviousPosition(c_NulluPtr)
#endif // ifdef NA_DEBUG
	{
#ifdef NA_DEBUG
		uInt const heapZoneSize = memory_operations::GetMemorySize(heapZoneInfo.m_Start, heapZoneInfo.m_End);
		NA_ASSERT_DEFAULT(memory_operations::IsAlligned(heapZoneSize, c_BlockAllignment));
		NA_ASSERT_DEFAULT(heapZoneSize > c_MinBlockSize);
#endif // ifdef NA_DEBUG
	}

	template<StackAllocatorSettings Settings>
	StackAllocator<Settings>::~StackAllocator()
	{
#ifdef NA_TRACK_ALLOCATIONS
		NA_ASSERT(m_AllocatorStats.m_ActiveAllocationCount == 0u && m_AllocatorStats.m_ActiveBytesAllocated == 0u,
			NA_NAMEOF_LITERAL(StackAllocator) << "'s destructor was called, but objects it allocated are still active");
#endif // ifdef NA_TRACK_ALLOCATIONS

		// ctrl f todo
		// todo comment up this class

		// TODO can we pull out the padding calculation logic into its own function?
		// And the allocate padding stuff?

		// TODO block info content should really contain the memory tag infomation. Rather than having a seperate bit after which does it
		// if we make this change, then also in IterateThroughHeapZone it should be updated

		// can iteratethroughheapzone func be funcationaltised?
	}

	template<StackAllocatorSettings Settings>
	void* StackAllocator<Settings>::Allocate(uInt const numBytes, HeapZoneInfo const& heapZoneInfo)
	{
		// TODO go through this and clean up etc

		NA_ASSERT(numBytes > 0u, "Allocating 0 bytes");

		uInt requiredBlockSize = numBytes + c_BlockHeaderSize;
		uInt const padding = requiredBlockSize % c_BlockAllignment;
		requiredBlockSize += padding;
		bool const requiresPadding = padding != 0u;

		NA_ASSERT((m_CurrentPosition + requiredBlockSize) <= heapZoneInfo.m_End, 
			NA_NAMEOF_LITERAL(StackAllocator) " is out of memory");

		uInt allocatedBytes = 0u;

		void* const payloadPtr = NA_TO_VPTR(m_CurrentPosition);
		allocatedBytes += numBytes;

		// Allocate the padding if needed
		if (requiresPadding)
		{
			uInt const blockPaddingStructPosition = padding - c_BlockPaddingSize;
			BlockPadding* const blockPadding = NA_REINTERPRET_MEMORY(BlockPadding, m_CurrentPosition, +, (allocatedBytes + blockPaddingStructPosition));

			blockPadding->m_Padding = static_cast<u8>(padding);
			allocatedBytes += padding;
		}

		BlockHeader* const blockHeader = NA_REINTERPRET_MEMORY(BlockHeader, m_CurrentPosition, +, allocatedBytes);

		// Allocate the block header
		BlockInfoContent blockInfoContent = {};
		blockInfoContent.m_Allocated = true;
		blockInfoContent.m_Padded = requiresPadding;
		blockInfoContent.m_NumBytes = requiredBlockSize;
		LoadBlockInfo(blockInfoContent, *blockHeader);

#ifdef NA_MEMORY_TAGGING
		// TODO Memory Tagging here (MemoryTagScope, get from the MemoryCommand singleton) (see above, this should really be part of blockinfocontent)
#endif // ifdef NA_MEMORY_TAGGING
		allocatedBytes += c_BlockHeaderSize;

		// Update stats
#ifdef NA_TRACK_ALLOCATIONS
		u64 constexpr allocationCountAdjustment = 1u;
		UpdateAllocatorStats(m_AllocatorStats, AllocatorStatsUpdateType::Allocate, allocationCountAdjustment, static_cast<u64>(requiredBlockSize));
#endif // #ifdef NA_TRACK_ALLOCATIONS

		// Update the stack pointers
#ifdef NA_DEBUG
		m_PreviousPosition = m_CurrentPosition;
#endif // ifdef NA_DEBUG
		m_CurrentPosition += allocatedBytes;

		return payloadPtr;
	}

	template<StackAllocatorSettings Settings>
	void StackAllocator<Settings>::Free(void* memory, HeapZoneInfo const& heapZoneInfo)
	{
		NA_ASSERT(memory, "Cannot free nullptr");
		NA_ASSERT(memory_operations::IsPtrInRange(heapZoneInfo.m_Start, heapZoneInfo.m_End,
			NA_TO_UPTR(memory)), "Trying to release memory not managed by this allocator");

		uPtr const memoryAddress = NA_TO_UPTR(memory);
		NA_ASSERT(memoryAddress == m_PreviousPosition, "Freeing memory which isn't at the top of the stack");

#if defined NA_DEBUG || defined NA_TRACK_ALLOCATIONS 
		BlockHeader* const blockHeader = NA_REINTERPRET_MEMORY(BlockHeader, m_CurrentPosition, -, c_BlockHeaderSize);
		uInt const blockSize = bit_operations::RightShiftBit(blockHeader->m_BlockInfo, type_utils::ToUnderlying(BlockInfoIndex::SizeStart));
#endif // ifdef NA_DEBUG || NA_TRACK_ALLOCATIONS

		// Update stats
#ifdef NA_TRACK_ALLOCATIONS
		u64 constexpr allocationCountAdjustment = 1u;
		UpdateAllocatorStats(m_AllocatorStats, AllocatorStatsUpdateType::Free, allocationCountAdjustment, static_cast<u64>(blockSize));
#endif // #ifdef NA_TRACK_ALLOCATIONS

		// Update the stack pointers
#ifdef NA_DEBUG
		m_PreviousPosition = memoryAddress - blockSize;
#endif // ifdef NA_DEBUG
		m_CurrentPosition = memoryAddress;
	}

	template<StackAllocatorSettings Settings>
	std::deque<AllocatorBlockInfo> StackAllocator<Settings>::IterateThroughHeapZone(
		std::optional<std::function<bool(AllocatorBlockInfo const&)>> action, HeapZoneInfo const& heapZoneInfo) const
	{
		// Because we can't iterate forwards, as block headers are stored at the end of the payload,
		// iterating through a stack allocators heapzone is a little clumsy. 

		std::deque<AllocatorBlockInfo> allocatorBlocks = {};
		uInt progressThroughHeapZone = static_cast<uInt>(m_CurrentPosition);
		BlockHeader const* blockHeader = nullptr;
		BlockInfoContent blockInfoContent = {};

		// There is no concept of 'free blocks' in a stack allocator, so check if there is any unused space
		uInt const freeMemoryNumBytes = static_cast<uInt>(heapZoneInfo.m_End - m_CurrentPosition);
		if (freeMemoryNumBytes > 0u)
		{
			AllocatorBlockInfo& allocatorBlockInfo = allocatorBlocks.emplace_back();
			allocatorBlockInfo.m_MemoryAddress = m_CurrentPosition;
			allocatorBlockInfo.m_Allocated = false;
			allocatorBlockInfo.m_NumBytes = freeMemoryNumBytes;
		}

		// Iterate though each block, starting from the back
		while (progressThroughHeapZone > heapZoneInfo.m_Start)
		{
			blockHeader = NA_REINTERPRET_MEMORY(BlockHeader, progressThroughHeapZone, -, c_BlockHeaderSize);
			UnloadBlockInfo(blockInfoContent, *blockHeader);
			progressThroughHeapZone -= blockInfoContent.m_NumBytes;

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
					NA_REINTERPRET_MEMORY(BlockPadding, blockHeader, -, c_BlockPaddingSize);
				padding = static_cast<u32>(blockPadding->m_Padding);
			}

			allocatorBlockInfo.m_NumBytes = blockInfoContent.m_NumBytes;
			allocatorBlockInfo.m_Padding = padding;
		}
		
		// Reverse because we were iterating backwards through the heapzone and call the action
		std::reverse(allocatorBlocks.begin(), allocatorBlocks.end()); 
		for (auto const& block : allocatorBlocks)
		{
			bool const continueLooping = action ? (*action)(block) : true;
			if (!continueLooping)
			{
				break;
			}
		}

		return allocatorBlocks;
	}
} // namespace nabi_allocator::stack_allocator
