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

		u32 constexpr c_MemoryAllignment = 8u;
	} // inline namespace memory_constants
} // namespace nabi_allocator
