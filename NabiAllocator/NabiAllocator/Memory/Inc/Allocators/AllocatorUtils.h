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
#ifdef NABI_ALLOCATOR_DEBUG
	std::string GetMemoryLayout(AllocatorBase const& allocator, HeapZoneInfo const& heapZoneInfo);
#endif // #ifdef NABI_ALLOCATOR_DEBUG

#ifdef NABI_ALLOCATOR_MEMORY_TAGGING
	// TODO - Write a function to output the memory usage by memory tag
	//      - Eg: 50% Game, 10% Rendering 
#endif // #ifdef NABI_ALLOCATOR_MEMORY_TAGGING 
} // namespace nabi_allocator
