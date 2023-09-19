#pragma once

// STD Headers
#include <deque>
#include <functional>
#include <optional>
#include <type_traits>

// Config Headers
#include "Config.h"

// Nabi Headers
#include "AllocatorStats.h"
#include "IntegerTypes.h"

/**
 * The base class for all allocators. HeapZone's allocator template must comply with is_allocator.
*/

namespace nabi_allocator
{
	struct AllocatorBlockInfo;
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
		[[nodiscard]] virtual void* Allocate(uInt const numBytes, HeapZoneInfo const& heapZoneInfo) = 0;
		virtual void Free(void* memory, HeapZoneInfo const& heapZoneInfo) = 0;

		[[nodiscard]] virtual std::deque<AllocatorBlockInfo> IterateThroughMemoryPool(
			std::optional<std::function<bool(AllocatorBlockInfo const&)>> action, HeapZoneInfo const& heapZoneInfo) = 0;

	protected:
#ifdef NABI_ALLOCATOR_TRACK_ALLOCATIONS
		AllocatorStats m_AllocatorStats = {};
#endif // ifdef NABI_ALLOCATOR_TRACK_ALLOCATIONS
	};
} // namespace nabi_allocator
