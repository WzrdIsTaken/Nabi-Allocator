#pragma once

// STD Headers
#include <string>

// Config Headers
#include "Config.h"

// Nabi Headers
#include "IntegerTypes.h"
#include "TemplateConcepts.h"

/**
 * Functions to perform bit manipulation.
*/

namespace nabi_allocator::bit_operations
{
	template<is_integral T>
	[[nodiscard]] NA_FORCE_INLINE constexpr T FlipBit(T const value, u32 const bitPosition) noexcept;
	template<is_integral T>
	[[nodiscard]] NA_FORCE_INLINE constexpr bool GetBit(T const value, u32 const bitPosition) noexcept;
	template<is_integral T>
	[[nodiscard]] NA_FORCE_INLINE constexpr T SetBit(T const value, u32 const bitPosition, bool const bitState) noexcept;
	template<is_integral T>
	[[nodiscard]] NA_FORCE_INLINE constexpr T LeftShiftBit(T const value, u32 const shiftAmount) noexcept;
	template<is_integral T>
	[[nodiscard]] NA_FORCE_INLINE constexpr T RightShiftBit(T const value, u32 const shiftAmount) noexcept;

	template<is_integral T>
	[[nodiscard]] std::string ToBinaryString(T const value, u32 const split = 8u) noexcept;
} // namespace nabi_allocator::bit_operations

// Include Inline
#include "Memory\Inl\Operations\BitOperations.inl"
