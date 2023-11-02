#pragma once

// STD Headers
#include <functional>
#include <optional>
#include <string>

// Config Headers
#include "Config.h"

// Nabi Headers
#include "IntegerTypes.h"
#include "MemoryConstants.h"

/**
 * Utility functions which can be used with all allocators.
 * 
 * Note:
 * - The reason GetMemoryUsage takes in a std::function rather than a std::unordered_map of memory tag
 *   aliases is so that the MemoryTag->string conversion can be implementation specific. 
 *   Eg, it might use reflection or something like magic enum.
*/

namespace nabi_allocator
{
	class AllocatorBase;
	struct HeapZoneInfo;
} // namespace nabi_allocator

namespace nabi_allocator
{
	[[nodiscard]] std::string GetMemoryLayout(AllocatorBase const& allocator, HeapZoneInfo const& heapZoneInfo);

#ifdef NA_MEMORY_TAGGING
	[[nodiscard]] std::string GetMemoryUsage(AllocatorBase const& allocator, HeapZoneInfo const& heapZoneInfo, 
		 std::optional<std::function<std::string(u32 const)>> tagConverter = std::nullopt);
#endif // #ifdef NA_MEMORY_TAGGING 
} // namespace nabi_allocator
