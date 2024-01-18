// inl's Header
#include "Operations/BitOperations.h"

// STD Headers
#include <bitset>
#include <sstream>

namespace nabi_allocator::bit_operations
{
	template<is_integral T>
	NA_FORCE_INLINE constexpr T FlipBit(T const value, u32 const bitPosition) noexcept
	{
		return value ^ (static_cast<T>(1) << bitPosition);
	}

	template<is_integral T>
	NA_FORCE_INLINE constexpr bool GetBit(T const value, u32 const bitPosition) noexcept
	{
		return static_cast<bool>(value & (static_cast<T>(1) << bitPosition));
	}

	template<is_integral T>
	NA_FORCE_INLINE constexpr T SetBit(T const value, u32 const bitPosition, bool const bitState) noexcept
	{
		bool const bitValue = GetBit(value, bitPosition);
		return bitValue != bitState ? FlipBit(value, bitPosition) : value;
	}

	template<is_integral T>
	NA_FORCE_INLINE constexpr T LeftShiftBit(T const value, u32 const shiftAmount) noexcept
	{
		return value << shiftAmount;
	}

	template<is_integral T>
	NA_FORCE_INLINE constexpr T RightShiftBit(T const value, u32 const shiftAmount) noexcept
	{
		return value >> shiftAmount;
	}

	template<is_integral T>
	std::string ToBinaryString(T const value, u32 const split) noexcept
	{
		std::ostringstream result;
		std::string const valueAsStr = std::bitset<std::numeric_limits<T>::digits>(value).to_string();

		for (u32 i = 0u; i < valueAsStr.size(); ++i)
		{
			if (i % split == 0u && i != 0u)
			{
				result << " ";
			}
			result << valueAsStr[i];
		}

		return result.str();
	}
} // namespace nabi_allocator::bit_operations
