// inl's Header
#include "Allocators\SegregatedListAllocator\SegregatedListAllocator.h"

// STD Headers
#include <cmath>

// Nabi Headers
#include "DebugUtils.h"
#include "Operations\MemoryOperations.h"

namespace nabi_allocator::segregated_list_allocator
{
	template<SegregatedListAllocatorSettings Settings>
	SegregatedListAllocator<Settings>::SegregatedListAllocator(HeapZoneInfo const& heapZoneInfo)
		: AllocatorBase{}
	{
		// TODO - write this :p

		// Overall, this allocator is going to be very similar to the free list allocator.
		// But we avoid block coalescing and find block - the slowest things in the FreeListAllocator.

		// Right now I'm thinking we need to refactor the free list logic away from the free list allocator.
		// Then the segregated list and the free list allocator can use it.
		// Therefore the minimum bucket size in this allocator will be 16. 

		// I think the headers can just be 1 byte,.. but then the min bucket would have to be 32..
		// Or we could use numbers. Will have to research if ptrs or adding is faster... 
		// probs ptrs idk
	}

	template<SegregatedListAllocatorSettings Settings>
	SegregatedListAllocator<Settings>::~SegregatedListAllocator()
	{
	}

	template<SegregatedListAllocatorSettings Settings>
	void* SegregatedListAllocator<Settings>::Allocate(uInt const numBytes, HeapZoneInfo const& heapZoneInfo)
	{
		NA_ASSERT(numBytes > 0u, "Allocating 0 bytes");

		// Find the correct list(/bucket) for the payload
		uInt const listIndex = GetListIndex(numBytes);

		// 1) Get the free list for that list index
		// 2) Get the first node
		// 3) Allocate into that
		// 4) Rearrange the free list

		return nullptr;
	}

	template<SegregatedListAllocatorSettings Settings>
	void SegregatedListAllocator<Settings>::Free(void* memory, HeapZoneInfo const& heapZoneInfo)
	{
		NA_ASSERT(memory, "Cannot free nullptr");
		NA_ASSERT(memory_operations::IsPtrInRange(heapZoneInfo.m_Start, heapZoneInfo.m_End,
			NA_TO_UPTR(memory)), "Trying to release memory not managed by this allocator");

		// 1) Can just go directly to the memory
		// 2) Reintepret cast stuff
	}

	template<SegregatedListAllocatorSettings Settings>
	std::deque<AllocatorBlockInfo> SegregatedListAllocator<Settings>::IterateThroughHeapZone(
		std::optional<std::function<bool(AllocatorBlockInfo const&)>> action, HeapZoneInfo const& heapZoneInfo) const
	{
		// TODO - ctrl f todo
		return std::deque<AllocatorBlockInfo>();
	}

	template<SegregatedListAllocatorSettings Settings>
	inline constexpr uInt SegregatedListAllocator<Settings>::GetListIndex(uInt const numBytes) noexcept
	{
		// TODO - profile this solutions speed - https://stackoverflow.com/questions/15967240/fastest-implementation-of-log2int-and-log2float
		//	- If we remove the log2, can remove the cmath include in this file
		// TODO - Test for this func
		return static_cast<uInt>(std::log2(numBytes));
	}
} // namespace nabi_allocator::segregated_list_allocator
