#pragma once

// Nabi Headers
#include "IntegerTypes.h"

/**
 * TODO - Desc
*/

namespace nabi_allocator::segregated_list_allocator
{
	struct SegregatedListAllocatorSettings final
	{
		// need to think:
		// should i have a constexpr array or something for the types
		// or should i have just the fixed sizes. if i had an array, would it cause slow down i wonder..?
		// eg when we access the values in it
	};

	SegregatedListAllocatorSettings constexpr c_SegregatedListAllocatorDefaultSettings =
	{

	};
} // namespace nabi_allocator::segregated_list_allocator
