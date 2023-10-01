#pragma once

// STD Headers
#include <array>
#include <deque>
#include <functional>
#include <optional>

// Nabi Headers
#include "Allocators\AllocatorBase.h"
#include "IntegerTypes.h"
#include "SegregatedListAllocatorSettings.h"

/**
 * A segregated list allocator is an optimisation of the free list allocator. Allocations are are categorized into
 * buckets based off there size (eg 16, 32, 64), and a free list is maintained for each bucket. This approach 
 * removes the need for searching for and coalescing free blocks (the slowest parts of the free list allocator)
 * resulting in faster allocations and frees. However, it is less flexible because each list can only store
 * payloads of a certain size. 
*/

namespace nabi_allocator
{
	struct AllocatorBlockInfo;
	struct HeapZoneInfo;
} // namespace nabi_allocator

namespace nabi_allocator::segregated_list_allocator
{
	template<SegregatedListAllocatorSettings Settings>
	class SegregatedListAllocator final : public AllocatorBase
	{
	public:
		explicit SegregatedListAllocator(HeapZoneInfo const& heapZoneInfo);
		~SegregatedListAllocator() override;

		[[nodiscard]] void* Allocate(uInt const numBytes, HeapZoneInfo const& heapZoneInfo) override;
		void Free(void* memory, HeapZoneInfo const& heapZoneInfo) override;

		std::deque<AllocatorBlockInfo> IterateThroughHeapZone(
			std::optional<std::function<bool(AllocatorBlockInfo const&)>> action, HeapZoneInfo const& heapZoneInfo) const override;

	private:
		NA_SET_COPY_MOVE_CONSTRUCTORS(SegregatedListAllocator, delete);

		[[nodiscard]] inline constexpr uInt GetListIndex(uInt const numBytes) noexcept;

		//std::array<size, free list node> m_FreeLists
	};
} // namespace nabi_allocator::segregated_list_allocator

// Include Inline
#include "Memory\Inl\Allocators\SegregatedListAllocator\SegregatedListAllocator.inl"
