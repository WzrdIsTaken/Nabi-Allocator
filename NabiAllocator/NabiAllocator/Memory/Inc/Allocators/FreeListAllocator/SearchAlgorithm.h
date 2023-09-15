#pragma once

// Nabi Headers
#include "IntegerTypes.h"

/**
 * There are 3 common search algorithm for a free list allocator.
 * - Best Fit: Finds a memory block with a size as close as possible to what is required. Pros: Space | Cons: Speed.
 * - First Fit: Finds the first memory block of suitable size or greater. Pros: Speed | Cons: Space.
 * - Next Fit: Similar to FirstFit, but returns the next suitable free block after the first one found to try and
 *   use more of the available memory pool. However, in reality this ends up resulting in an even more 
 *   chockablocked/fragmented memory layout.
*/

namespace nabi_allocator::free_list_allocator
{
	enum class SearchAlgorithm : u32
	{
		BestFit,
		FirstFit,
		NextFit,

		ENUM_COUNT
	};
} // namespace nabi_allocator::free_list_allocator
