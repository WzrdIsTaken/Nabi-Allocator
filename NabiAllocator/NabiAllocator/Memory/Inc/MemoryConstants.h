#pragma once

// Nabi Headers
#include "IntegerTypes.h"

/**
 * Global memory constants & some overloaded operators to make life easier.
*/

namespace nabi_allocator
{
	inline namespace memory_typedefs
	{
		using MemoryTag = u32; // I hope a u32 has enough range for all the memory tags someone could ever want... (it can easily be changed here though)
	} // inline namespace memory_typedefs

	inline namespace memory_constants
	{
		uInt constexpr c_KByte = 1024u;
		uInt constexpr c_MByte = c_KByte * 1024u;
		uInt constexpr c_GByte = c_MByte * 1024u;
	} // inline namespace memory_constants

	inline namespace memory_operators
	{
		inline constexpr uInt operator""_KB(u64 const value) { return static_cast<uInt>(value) * c_KByte; } // This operator overload needs a character type or ull
		inline constexpr uInt operator""_MB(u64 const value) { return static_cast<uInt>(value) * c_MByte; }
		inline constexpr uInt operator""_GB(u64 const value) { return static_cast<uInt>(value) * c_GByte; }

		static_assert(1_KB == 1024u);
		static_assert(1_MB == 1'048'576u);
		static_assert(1_GB == 1'073'741'824u);
	} // inline namespace memory_operators
} // namespace nabi_allocator
