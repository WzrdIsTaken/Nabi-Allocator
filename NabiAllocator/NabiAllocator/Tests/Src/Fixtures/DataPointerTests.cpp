// STD Headers
#include <string>

// Library Headers
#include "gtest\gtest.h"

// Config Headers
#include "Config.h"

// Nabi Headers
#include "IntegerTypes.h"
#include "Pointers\DataPointer.h"
#include "TestConstants.h"

/**
 * Tests for the DataPointer
*/

namespace nabi_allocator::tests
{
#ifdef NA_TESTS
#	define NA_FIXTURE_NAME NA_TEST_FIXTURE_NAME(DataPointerTests)

	TEST(NA_FIXTURE_NAME, CreateAndDereferenceDataPointer)
	{
		// * dereference
		{
			s32 constexpr value = 20;
			DataPointer<s32 const> dataPointer = &value;
			EXPECT_EQ(*dataPointer, value);
			EXPECT_EQ(dataPointer.m_Data, static_cast<uInt>(0)); // Data should be set to 0 when a DataPointer is constructed with only a ptr (would be the same deal in the next case)
		}
		
		// -> dereference
		{
			struct Container final
			{
				s32 m_Value;
			};
			Container constexpr container = { 24 };
			DataPointer<Container const> dataPointer = &container;
			EXPECT_EQ(dataPointer->m_Value, container.m_Value);
		}

		// Create with data
		{
			std::string const value = "Hello, DataPointer!";
			uInt constexpr data = 1;
			DataPointer<std::string const> dataPointer = { &value, data };
			EXPECT_EQ(*dataPointer, value);
			EXPECT_EQ(dataPointer.m_Data, data);
		}
	}

	TEST(NA_FIXTURE_NAME, UpdateDataPointer)
	{
		DataPointer<s32> dataPointer = nullptr;

		// Update ptr
		{
			s32 constexpr valueOne = 1;
			dataPointer = &valueOne;
			EXPECT_EQ(*dataPointer, valueOne);

			s32 constexpr valueTwo = 2;
			*dataPointer = valueTwo;
			EXPECT_EQ(*dataPointer, valueTwo);
		}
		
		// Update data
		{
			uInt constexpr data = 4;
			dataPointer.m_Data = data;
			EXPECT_EQ(dataPointer.m_Data, data);
		}
	}

	// There is a static assert to test the size of the pointer in DataPointer.h

#	undef NA_FIXTURE_NAME
#endif 
} // namespace nabi_allocator::tests
