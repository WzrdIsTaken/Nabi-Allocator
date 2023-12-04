#pragma once

// STD Headers
#include <string>

// Nabi Headers
#include "IntegerTypes.h"
#include "TemplateConcepts.h"

/**
 * Functions to perform bit manipulation.
*/

namespace nabi_allocator::bit_operations
{
	template<is_integral T>
	[[nodiscard]] __forceinline constexpr T FlipBit(T const value, u32 const bitPosition) noexcept;
	template<is_integral T>
	[[nodiscard]] __forceinline constexpr bool GetBit(T const value, u32 const bitPosition) noexcept;
	template<is_integral T>
	[[nodiscard]] __forceinline constexpr T SetBit(T const value, u32 const bitPosition, bool const bitState) noexcept;
	template<is_integral T>
	[[nodiscard]] __forceinline constexpr T LeftShiftBit(T const value, u32 const shiftAmount) noexcept;
	template<is_integral T>
	[[nodiscard]] __forceinline constexpr T RightShiftBit(T const value, u32 const shiftAmount) noexcept;

	template<is_integral T>
	[[nodiscard]] std::string ToBinaryString(T const value, u32 const split = 8u) noexcept;
} // namespace nabi_allocator::bit_operations

// Include Inline
#include "Memory\Inl\Operations\BitOperations.inl"
