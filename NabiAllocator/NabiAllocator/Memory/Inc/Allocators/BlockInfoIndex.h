#pragma once

// Nabi Headers
#include "IntegerTypes.h"

/**
 * This enum's values store indexes for the data stored in BlockInfo.
 * As the block size is guaranteed to always be a multiple of 8, we can always safely mask of the first 3 bits and still hold the size.
*/

namespace nabi_allocator
{
	enum class BlockInfoIndex : u32
	{
		Allocated = 0u,
		Padded = 1u,
		SizeStart = 3u,

		ENUM_COUNT = 4u
	};
} // namespace nabi_allocator
