#pragma once

// Nabi Headers
#include "IntegerTypes.h"

/**
 * Global memory constants.
*/

namespace nabi_allocator
{
	inline namespace memory_constants
	{
		u32 constexpr c_KByte = 1024u;
		u32 constexpr c_MByte = c_KByte * 1024u;
		u32 constexpr c_GByte = c_MByte * 1024u;

		// The size of blocks of memory have to be multiples of 8. This is because then we can always
		// mask of 3 bits of the size to store other infomation and still retain the size of the block.
		// Allignments of 8 also always work for x64 and x64 architecture. 
		u32 constexpr c_BlockAllignment = 8u;
	} // inline namespace memory_constants
} // namespace nabi_allocator
