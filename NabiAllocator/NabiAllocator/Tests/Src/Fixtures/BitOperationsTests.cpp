// STD Headers
#include <string>

// Library Headers
#include "gtest\gtest.h"

// Config Headers
#include "Config.h"

// Nabi Headers
#include "IntegerTypes.h"
#include "Operations/BitOperations.h"
#include "TestConstants.h"

/**
 * Tests for BitOperations
*/

namespace nabi_allocator::tests
{
#ifdef NA_TESTS
#	define NA_FIXTURE_NAME NA_TEST_FIXTURE_NAME(BitOperationsTests)

	TEST(NA_FIXTURE_NAME, FlipBit)
	{
		u32 constexpr value = 0b0011u;

		{
			u32 constexpr expected = 0b0001u;
			u32 constexpr actual = bit_operations::FlipBit(value, 1u);
			EXPECT_EQ(actual, expected);
		}
		{
			u32 constexpr expected = 0b1011u;
			u32 constexpr actual = bit_operations::FlipBit(value, 3u);
			EXPECT_EQ(actual, expected);
		}
	}

	TEST(NA_FIXTURE_NAME, GetBit)
	{
		u32 constexpr value = 0b0001u;

		{
			bool constexpr expected = true;
			bool constexpr actual = bit_operations::GetBit(value, 0u);
			EXPECT_EQ(actual, expected);
		}
		{
			bool constexpr expected = false;
			bool constexpr actual = bit_operations::GetBit(value, 1u);
			EXPECT_EQ(actual, expected);
		}
	}

	TEST(NA_FIXTURE_NAME, SetBit)
	{
		u32 constexpr value = 0b0001u;

		{
			u32 constexpr expected = 0b0011u;
			u32 constexpr actual = bit_operations::SetBit(value, 1u, true);
			EXPECT_EQ(actual, expected);
		}
		{
			u32 constexpr expected = 0b0001u;
			u32 constexpr actual = bit_operations::SetBit(value, 0u, true);
			EXPECT_EQ(actual, expected);
		}
		{
			u32 constexpr expected = 0b0000u;
			u32 constexpr actual = bit_operations::SetBit(value, 0u, false);
			EXPECT_EQ(actual, expected);
		}
	}

	TEST(NA_FIXTURE_NAME, BitShift)
	{
		u32 constexpr value = 0b0010u;

		{
			u32 constexpr expected = 0b001u;
			u32 constexpr actual = bit_operations::RightShiftBit(value, 1u);
			EXPECT_EQ(actual, expected);
		}
		{
			u32 constexpr expected = 0b0100u;
			u32 constexpr actual = bit_operations::LeftShiftBit(value, 1u);
			EXPECT_EQ(actual, expected);
		}
	}

	TEST(NA_FIXTURE_NAME, ToBinaryString)
	{
		u32 constexpr value = 0b0101u;
		std::string const/*expr :(*/ expected = "00000000 00000000 00000000 00000101";

		std::string const actual = bit_operations::ToBinaryString(value, 8u);
		EXPECT_EQ(actual, expected);
	}

#	undef NA_FIXTURE_NAME
#endif // ifdef NA_TESTS
} // namespace nabi_allocator::tests
