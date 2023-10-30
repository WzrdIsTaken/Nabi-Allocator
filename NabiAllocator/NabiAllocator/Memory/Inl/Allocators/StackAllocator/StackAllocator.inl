// inl's Header
#include "Allocators\StackAllocator\StackAllocator.h"

// STD Headers
#include <algorithm>

// Nabi Headers
#include "AllocationInfo.h"
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
		, m_CurrentPosition(c_NulluPtr)
#ifdef NA_DEBUG
		, m_PreviousPosition(c_NulluPtr)
#endif // ifdef NA_DEBUG
	{
		InitMemory(heapZoneInfo);
	}

	template<StackAllocatorSettings Settings>
	StackAllocator<Settings>::~StackAllocator()
	{
#ifdef NA_TRACK_ALLOCATIONS
		NA_ASSERT(m_AllocatorStats.m_ActiveAllocationCount == 0u && m_AllocatorStats.m_ActiveBytesAllocated == 0u,
			NA_NAMEOF_LITERAL(StackAllocator) << "'s destructor was called, but objects it allocated are still active");
#endif // ifdef NA_TRACK_ALLOCATIONS
	}

	template<StackAllocatorSettings Settings>
	void* StackAllocator<Settings>::Allocate(AllocationInfo const& allocationInfo, HeapZoneInfo const& heapZoneInfo)
	{
		NA_ASSERT(allocationInfo.m_NumBytes > 0u, "Allocating 0 bytes");

		// Check if the remaining space in the allocator is sufficient
		uInt requiredBlockSize = allocationInfo.m_NumBytes + c_BlockHeaderSize;
		uInt const padding = requiredBlockSize % c_BlockAllignment;
		requiredBlockSize += padding;
		bool const requiresPadding = padding != 0u;

		NA_ASSERT((m_CurrentPosition + requiredBlockSize) <= heapZoneInfo.m_End, 
			NA_NAMEOF_LITERAL(StackAllocator) " is out of memory");

		uInt allocatedBytes = 0u;

		// In the stack allocator, the payload comes first
		void* const payloadPtr = NA_TO_VPTR(m_CurrentPosition);
		allocatedBytes += allocationInfo.m_NumBytes;

		// Allocate padding if needed
		if (requiresPadding)
		{
			AllocateBlockPadding((m_CurrentPosition + allocatedBytes), padding);
			allocatedBytes += padding;
		}

		// Allocate the block header at the end of the block so it can be found with (m_CurrentPosition - c_BlockHeaderSize)
		BlockHeader* const blockHeader = NA_REINTERPRET_MEMORY(BlockHeader, m_CurrentPosition, +, allocatedBytes);

		BlockInfoContent blockInfoContent = {};
		blockInfoContent.m_Allocated = true;
		blockInfoContent.m_Padded = requiresPadding;
		blockInfoContent.m_NumBytes = requiredBlockSize;
#ifdef NA_MEMORY_TAGGING
		blockInfoContent.m_MemoryTag = allocationInfo.m_MemoryTag;
#endif // ifdef NA_MEMORY_TAGGING
		LoadBlockInfo(blockInfoContent, *blockHeader);
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
	void StackAllocator<Settings>::Reset(HeapZoneInfo const& heapZoneInfo)
	{
#ifdef NA_TRACK_ALLOCATIONS
		ResetAllocatorStats(m_AllocatorStats, AllocatorStatsResetType::Active);
#endif // #ifdef NA_TRACK_ALLOCATIONS

		InitMemory(heapZoneInfo);
	}

	template<StackAllocatorSettings Settings>
	std::deque<AllocatorBlockInfo> StackAllocator<Settings>::IterateThroughHeapZone(
		std::optional<std::function<bool(AllocatorBlockInfo const&)>> action, HeapZoneInfo const& heapZoneInfo) const
	{
		// Because we can't iterate forwards, as block headers are stored at the end of the payload,
		// iterating through a stack allocators heapzone is a little clumsy. 

		std::deque<AllocatorBlockInfo> allocatorBlocks = {};
		uInt progressThroughHeapZone = static_cast<uInt>(m_CurrentPosition);

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
			AllocatorBlockInfo const allocatorBlockInfo =
				IterateThroughHeapZoneHelper((progressThroughHeapZone - c_BlockHeaderSize),
					[](uInt blockNumBytes) -> s32
					{
						return -static_cast<s32>(c_BlockPaddingSize);
					});
			progressThroughHeapZone -= allocatorBlockInfo.m_NumBytes;
			allocatorBlocks.emplace_back(std::move(allocatorBlockInfo));
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

	template<StackAllocatorSettings Settings>
	void StackAllocator<Settings>::InitMemory(HeapZoneInfo const& heapZoneInfo)
	{
#ifdef NA_DEBUG
		uInt const heapZoneSize = memory_operations::GetMemorySize(heapZoneInfo.m_Start, heapZoneInfo.m_End);
		NA_ASSERT_DEFAULT(memory_operations::IsAlligned(heapZoneSize, c_BlockAllignment));
		NA_ASSERT_DEFAULT(heapZoneSize > c_MinBlockSize);

		m_PreviousPosition = c_NulluPtr;
#endif // ifdef NA_DEBUG

		m_CurrentPosition = heapZoneInfo.m_Start;
	}
} // namespace nabi_allocator::stack_allocator
