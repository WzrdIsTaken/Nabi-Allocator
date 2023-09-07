#pragma once

// STD Headers
#include <cstdint>

/**
 * Brings std integer typedefs into scope and shortens them for brevity.
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

	inline namespace integer_constants
	{
		sPtr constexpr c_NullsPtr = static_cast<sPtr>(0);
		uPtr constexpr c_NulluPtr = static_cast<uPtr>(0);
	} // inline namespace integer_constants
} // namespace nabi_allocator
