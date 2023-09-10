#pragma once

// STD Headers
#include <cstdint>

/**
 * Brings std integer typedefs into scope and shortens them for brevity.
 * 
 * sPtr, uPtr and uInt should be used for memory related operations where *range* matters.
 * The other types should be used where *size* matters. (And range as well I guess.. :p,
 * as well as clarity something can/cannot be negative (obv))
*/

namespace nabi_allocator
{
	using s8  = std::int8_t;
	using s16 = std::int16_t;
	using s32 = std::int32_t;
	using s64 = std::int64_t;

	using u8  = std::uint8_t;
	using u16 = std::uint16_t;
	using u32 = std::uint32_t;
	using u64 = std::uint64_t;

	using sPtr = std::intptr_t;
	using uPtr = std::uintptr_t;
	using uInt = std::size_t;

	inline namespace integer_constants
	{
		sPtr constexpr c_NullsPtr = static_cast<sPtr>(NULL);
		uPtr constexpr c_NulluPtr = static_cast<uPtr>(NULL);
	} // inline namespace integer_constants
} // namespace nabi_allocator
