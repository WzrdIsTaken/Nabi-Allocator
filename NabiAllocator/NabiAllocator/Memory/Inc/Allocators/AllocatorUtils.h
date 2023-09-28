#pragma once

// STD Headers
#include <string>

// Config Headers
#include "Config.h"

/**
 * Utility functions which can be used with all allocators.
*/

namespace nabi_allocator
{
	class AllocatorBase;
	struct HeapZoneInfo;
} // namespace nabi_allocator

namespace nabi_allocator
{
	std::string GetMemoryLayout(AllocatorBase const& allocator, HeapZoneInfo const& heapZoneInfo);

#ifdef NA_MEMORY_TAGGING
	// TODO - Write a function to output the memory usage by memory tag
	//      - Eg: 50% Game, 10% Rendering 
#endif // #ifdef NA_MEMORY_TAGGING 
} // namespace nabi_allocator
