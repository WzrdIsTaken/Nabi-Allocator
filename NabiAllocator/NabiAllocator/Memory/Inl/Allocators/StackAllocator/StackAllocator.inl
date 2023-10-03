// inl's Header
#include "Allocators\StackAllocator\StackAllocator.h"

// Nabi Headers
#include "HeapZone\HeapZoneInfo.h"

namespace nabi_allocator::stack_allocator
{
	template<StackAllocatorSettings Settings>
	StackAllocator<Settings>::StackAllocator(HeapZoneInfo const& heapZoneInfo)
	{
	}

	template<StackAllocatorSettings Settings>
	StackAllocator<Settings>::~StackAllocator()
	{
	}

	template<StackAllocatorSettings Settings>
	void* StackAllocator<Settings>::Allocate(uInt const numBytes, HeapZoneInfo const& heapZoneInfo)
	{
		return nullptr;
	}

	template<StackAllocatorSettings Settings>
	void StackAllocator<Settings>::Free(void* memory, HeapZoneInfo const& heapZoneInfo)
	{
	}

	template<StackAllocatorSettings Settings>
	std::deque<AllocatorBlockInfo> StackAllocator<Settings>::IterateThroughHeapZone(
		std::optional<std::function<bool(AllocatorBlockInfo const&)>> action, HeapZoneInfo const& heapZoneInfo) const
	{
		return std::deque<AllocatorBlockInfo>();
	}
} // namespace nabi_allocator::stack_allocator
