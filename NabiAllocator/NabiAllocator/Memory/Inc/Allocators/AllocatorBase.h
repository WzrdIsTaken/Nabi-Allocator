#pragma once

// STD Headers
#include <deque>
#include <functional>
#include <optional>
#include <type_traits>

// Config Headers
#include "Config.h"

// Nabi Headers
#include "AllocatorBlockInfo.h"
#include "AllocatorStats.h"
#include "IntegerTypes.h"
#include "TypeUtils.h"

/**
 * The base class for all allocators. HeapZone's allocator template must comply with is_allocator.
*/

namespace nabi_allocator
{
	struct AllocationInfo;
	struct HeapZoneInfo;
} // namespace nabi_allocator

namespace nabi_allocator
{
	class AllocatorBase;
	template<typename T>
	concept is_allocator = std::is_base_of_v<AllocatorBase, T>;

	class AllocatorBase abstract
	{
	public:
		inline AllocatorBase();
		virtual ~AllocatorBase() = default;

		[[nodiscard]] virtual void* Allocate(AllocationInfo const& allocationInfo, HeapZoneInfo const& heapZoneInfo) = 0;
		virtual void Free(void* memory, HeapZoneInfo const& heapZoneInfo) = 0;
		virtual void Reset(HeapZoneInfo const& heapZoneInfo) = 0;

		virtual std::deque<AllocatorBlockInfo> IterateThroughHeapZone(
			std::optional<std::function<bool(AllocatorBlockInfo const&)>> const action, HeapZoneInfo const& heapZoneInfo) const = 0;

#ifdef NA_TRACK_ALLOCATIONS
		inline AllocatorStats const& GetStats() const noexcept;
#endif // ifdef NA_TRACK_ALLOCATIONS

	protected:
		[[nodiscard]] AllocatorBlockInfo IterateThroughHeapZoneHelper(
			uInt const blockHeaderPosition, std::function<s64(uInt const)> const calculateBlockPaddingAdjustment) const;

#ifdef NA_TRACK_ALLOCATIONS
		AllocatorStats m_AllocatorStats;
#endif // ifdef NA_TRACK_ALLOCATIONS

	private:
		NA_SET_COPY_MOVE_CONSTRUCTORS(AllocatorBase, delete);
	};
} // namespace nabi_allocator

// Include Inline
#include "Memory\Inl\Allocators\AllocatorBase.inl"
