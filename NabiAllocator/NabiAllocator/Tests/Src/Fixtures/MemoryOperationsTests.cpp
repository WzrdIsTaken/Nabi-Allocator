// Library Headers
#include "gtest\gtest.h"

// Config Headers
#include "Config.h"

// Nabi Headers
#include "IntegerTypes.h"
#include "Operations/MemoryOperations.h"
#include "TestConstants.h"

/**
 * Tests for MemoryOperations
*/

namespace nabi_allocator::tests
{
#ifdef NA_TESTS
#	define NA_FIXTURE_NAME NA_TEST_FIXTURE_NAME(MemoryOperationsTests)

	TEST(NA_FIXTURE_NAME, RequestAndReleaseMemory)
	{
		s32* memory = memory_operations::RequestMemoryFromOS<s32*>(sizeof(s32));
		EXPECT_TRUE(memory);

		memory_operations::ReleaseMemoryToOS(memory);
		EXPECT_FALSE(memory);
	}

	TEST(NA_FIXTURE_NAME, ResetMemory)
	{
		struct MockStruct final
		{
			u32 m_Value;
		};

		{
			MockStruct mockStruct = { 1u };

			memory_operations::ResetMemory(&mockStruct);
			EXPECT_EQ(mockStruct.m_Value, 0u);
		}
		
		{
			MockStruct mockStruct = { 2u };
			MockStruct* const ptrToMockStruct = &mockStruct;

			memory_operations::ResetMemory(ptrToMockStruct);
			EXPECT_EQ(ptrToMockStruct->m_Value, 0u);
		}

		{
			MockStruct mockStruct = { 3u };
			MockStruct* const ptrToMockStruct = &mockStruct;

			memory_operations::ResetMemory(ptrToMockStruct, sizeof(MockStruct));
			EXPECT_EQ(ptrToMockStruct->m_Value, 0u);
		}
	}

	TEST(NA_FIXTURE_NAME, GetMemorySize)
	{
		uPtr constexpr start = 0u;
		uPtr constexpr end = 8u;

		void const* const startVoid = NA_TO_VPTR(start);
		void const* const endVoid = NA_TO_VPTR(end);

		EXPECT_EQ(8u, memory_operations::GetMemorySize(start, end));
		EXPECT_EQ(8u, memory_operations::GetMemorySize(startVoid, endVoid));
	}

	TEST(NA_FIXTURE_NAME, IsAlligned)
	{
		uInt constexpr value = 8u;
		EXPECT_TRUE(memory_operations::IsAlligned(value, 4u));
		EXPECT_FALSE(memory_operations::IsAlligned(value, 3u));

		void const* const ptr = NA_TO_VPTR(value);
		EXPECT_TRUE(memory_operations::IsAlligned(ptr, 4u));
	}

#	undef NA_FIXTURE_NAME
#endif // ifdef NA_TESTS
} // namespace nabi_allocator::tests
