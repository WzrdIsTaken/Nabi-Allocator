#pragma once

// Nabi Headers
#include "IntegerTypes.h"
#include "MemoryConstants.h"

/**
 * Some overloaded operators to make life easier.
*/

namespace nabi_allocator
{
	inline namespace memory_operators
	{
		inline constexpr u32 operator""_KB(u64 const value) { return static_cast<u32>(value) * c_KByte; } // This operator overload needs a character type or ull
		inline constexpr u32 operator""_MB(u64 const value) { return static_cast<u32>(value) * c_MByte; }
		inline constexpr u32 operator""_GB(u64 const value) { return static_cast<u32>(value) * c_GByte; }

		static_assert(1_KB == 1024u); // I'm on the fence whether these should be static asserts or unit tests..?
		static_assert(1_MB == 1'048'576u);
		static_assert(1_GB == 1'073'741'824u);
	} // inline namespace memory_operators
} // namespace nabi_allocator
