#pragma once

// Nabi Headers
#include "IntegerTypes.h"
#include "SearchAlgorithms.h"

/**
 * The FreeListAllocator is templated with this settings struct so that it can be customised at compile time.
 * This means that any branches that are required to support customisation can be constexpr.
 * 
 * The syntax would be something like:
 * FreeListAllocatorSettings constexpr c_FreeListAllocatorSettings = 
 * {
 *    [values]
 * };
 * HeapZone<FreeListAllocator<c_FreeListAllocatorSettings>> m_HeapZone;
*/

namespace nabi_allocator
{
	struct FreeListAllocatorSettings final
	{
		SearchAlgorithm m_SearchAlgorithm; // Which search algorithm to use
		uInt m_BestFitLeniency; // If the search algorithm is best fit, how close a free block's size has to be to the required size to be accepted
	};

	FreeListAllocatorSettings constexpr c_FreeListAllocatorDefaultSettings =
	{
		.m_SearchAlgorithm = SearchAlgorithm::BestFit,
		.m_BestFitLeniency = 0u
	};
} // namespace nabi_allocator
