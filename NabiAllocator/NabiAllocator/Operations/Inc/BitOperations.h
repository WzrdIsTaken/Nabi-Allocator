#pragma once

// STD Headers
#include <string>
#include <type_traits>

// Nabi Headers
#include "IntegerTypes.h"

/**
 * Functions to perform bit manipulation.
*/

namespace nabi_allocator::bit_operations
{
	template<typename T>
	concept is_integral = std::is_integral<T>::value; // We could use the "requires" keword with each function, but I feel like that makes things cluttered

	template<is_integral T>
	[[nodiscard]] inline constexpr T FlipBit(T const value, u32 const bitPosition) noexcept;
	template<is_integral T>
	[[nodiscard]] inline constexpr bool GetBit(T const value, u32 const bitPosition) noexcept;
	template<is_integral T>
	[[nodiscard]] inline constexpr T SetBit(T const value, u32 const bitPosition, bool const bitState) noexcept;
	template<is_integral T>
	[[nodiscard]] inline constexpr T LeftShiftBit(T const value, u32 const shiftAmount) noexcept;
	template<is_integral T>
	[[nodiscard]] inline constexpr T RightShiftBit(T const value, u32 const shiftAmount) noexcept;

	template<is_integral T>
	[[nodiscard]] std::string ToBinaryString(T const value, u32 const split = 8u) noexcept;
} // namespace nabi_allocator::bit_operations

// Include Inline
#include "BitOperations.inl"
