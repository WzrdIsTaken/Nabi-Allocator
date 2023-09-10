#pragma once

// STD Headers
#include <type_traits>

// Nabi Headers
#include "IntegerTypes.h"

/**
 * The base class for all allocators. HeapZone's allocator template must comply with is_allocator.
*/

namespace nabi_allocator
{
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
	};
} // namespace nabi_allocator
