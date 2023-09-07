// Library Headers
#include "gtest\gtest.h"

// Config Headers
#include "Config.h"

// Nabi Headers
#include "IntegerTypes.h"
#include "MemoryOperations.h"

/**
 * Tests for MemoryOperations
*/

namespace nabi_allocator::tests
{
#ifdef NABI_ALLOCATOR_TESTS
#	define TEST_FIXTURE_NAME MemoryOperationsTests

	TEST(TEST_FIXTURE_NAME, RequestAndReleaseMemory)
	{
		s32* memory = memory_operations::RequestMemoryFromOS<s32*>(sizeof(s32));
		EXPECT_TRUE(memory);

		memory_operations::ReleaseMemoryToOS(memory);
		EXPECT_FALSE(memory);
	}

	TEST(TEST_FIXTURE_NAME, ResetMemory)
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

	TEST(TEST_FIXTURE_NAME, GetMemorySize)
	{
		uPtr const start = 0u;
		uPtr const end = 8u;

		void const* const startVoid = TO_VPTR(start);
		void const* const endVoid = TO_VPTR(end);

		EXPECT_EQ(8u, memory_operations::GetMemorySize(start, end));
		EXPECT_EQ(8u, memory_operations::GetMemorySize(startVoid, endVoid));
	}

#	undef TEST_FIXTURE_NAME
#endif // ifdef NABI_ALLOCATOR_TESTS
} // namespace nabi_allocator::tests
