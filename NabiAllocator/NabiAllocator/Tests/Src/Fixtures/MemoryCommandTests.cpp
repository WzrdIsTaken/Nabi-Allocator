// Library Headers
#include "gtest\gtest.h"

// Config Headers
#include "Config.h"

// Nabi Headers
#include "HeapZone\HeapZoneScope.h"
#include "IntegerTypes.h"
#include "MemoryCommand.h"
#include "TestConstants.h"

/**
 * Tests for MemoryCommand
*/

namespace nabi_allocator::tests
{
#ifdef NA_TESTS
#	define NA_FIXTURE_NAME NA_TEST_FIXTURE_NAME(MemoryCommandTests)

	TEST(NA_FIXTURE_NAME, CreateAndDestroy)
	{
		MemoryCommand memoryCommand = {};
		EXPECT_FALSE(memoryCommand.GetTopHeapZoneScope());
	}

	TEST(NA_FIXTURE_NAME, PushAndPopScope)
	{
		MemoryCommand memoryCommand = {};
		HeapZoneScope scope = { /*heapZone*/ nullptr, /*memoryTag*/ nullptr, /*registerWithMemoryCommand*/ false};

		memoryCommand.PushHeapZoneScope(scope);
		EXPECT_EQ(memoryCommand.GetTopHeapZoneScope(), &scope);

		memoryCommand.PopHeapZoneScope(scope);
		EXPECT_FALSE(memoryCommand.GetTopHeapZoneScope());
	}

	TEST(NA_FIXTURE_NAME, PushTwoScopesThenReset)
	{
		MemoryCommand memoryCommand = {};
		HeapZoneScope scope1 = { nullptr, nullptr, false };
		HeapZoneScope scope2 = { nullptr, nullptr, false };

		memoryCommand.PushHeapZoneScope(scope1);
		memoryCommand.PushHeapZoneScope(scope2);
		EXPECT_EQ(memoryCommand.GetTopHeapZoneScope(), &scope2);

		memoryCommand.Reset();
		EXPECT_FALSE(memoryCommand.GetTopHeapZoneScope());
	}

#	undef NA_FIXTURE_NAME
#endif // ifdef NA_TESTS
} // namespace nabi_allocator::tests
