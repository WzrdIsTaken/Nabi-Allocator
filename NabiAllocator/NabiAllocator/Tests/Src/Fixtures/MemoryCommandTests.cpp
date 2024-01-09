// STD Headers
#include <thread>

// Library Headers
#include "gtest\gtest.h"

// Config Headers
#include "Config.h"

// Nabi Headers
#include "Allocators\FreeListAllocator\FreeListAllocator.h"
#include "HeapZone\HeapZone.h"
#include "HeapZone\HeapZoneScope.h"
#include "IntegerTypes.h"
#include "MemoryCommand.h"
#include "TestConstants.h"

/**
 * Tests for MemoryCommand.
 * 
 * Note:
 *	- Some stuff is tested in FullWorldflow.cpp -> BasicWorkflow & FullWorkFlow (it just makes more sense to do it there).
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
		{
			HeapZoneScope scope = { /*heapZone*/ nullptr, /*memoryTag*/ std::nullopt, /*customMemoryCommand*/ &memoryCommand };
			EXPECT_EQ(memoryCommand.GetTopHeapZoneScope(), &scope);
		}
		
		EXPECT_FALSE(memoryCommand.GetTopHeapZoneScope());
	}

	TEST(NA_FIXTURE_NAME, PushAndPopTwoScopes)
	{
		MemoryCommand memoryCommand = {};
		{
			HeapZoneScope scope1 = { nullptr, std::nullopt, &memoryCommand };
			{
				HeapZoneScope scope2 = { nullptr, std::nullopt, &memoryCommand };
				EXPECT_EQ(memoryCommand.GetTopHeapZoneScope(), &scope2);
			}
			EXPECT_EQ(memoryCommand.GetTopHeapZoneScope(), &scope1);
		}
		
		EXPECT_FALSE(memoryCommand.GetTopHeapZoneScope());
	}

	TEST(NA_FIXTURE_NAME, ThreadLocalHeapZoneScopes)
	{
		if (std::thread::hardware_concurrency() >= 2u)
		{
			// I know this looks a little bot, and its probs is, but this test does fail if you 
			// change MemoryCommand->g_HeapZoneScopes to 'static' instead of 'threadlocal'

			MemoryCommand memoryCommand = {};
			auto const pushHeapZoneScope =
				[&memoryCommand]() -> void
				{
					HeapZoneScope scope = { &c_UnmanagedHeap, std::nullopt, &memoryCommand };
					EXPECT_EQ(memoryCommand.GetHeapZoneScopeCount(), 1u);
				};

			HeapZoneScope scope = { &c_UnmanagedHeap, std::nullopt, &memoryCommand };
			std::thread thread(pushHeapZoneScope);
			EXPECT_EQ(memoryCommand.GetHeapZoneScopeCount(), 1u);
			thread.join();
		}
	}

	TEST(NA_FIXTURE_NAME, TooLargeAllocation)
	{
#	ifdef NA_DEBUG
		// Kinda the same deal as the StackAllocatorTests::EnsureTopOfStackFree. Idk why, but gTest 
		// does not like asserts and I can't find an EXPECT_ASSERT solution or something like at PG.
		return;
#	endif

		uInt constexpr heapZoneSize = 64u;

		MemoryCommand memoryCommand = {};
		HeapZone<DefaultFreeListAllocator> heapZone = { HeapZoneBase::c_NoParent, heapZoneSize, "TestHeapZone" };
		HeapZoneScope scope = { &heapZone, std::nullopt, &memoryCommand };

		void* ptr = memoryCommand.Allocate(heapZoneSize + 4u);
		bool const ptrIsNull = (ptr == nullptr); // gTest doesn't like checking pointers in EXPECT_[TRUE/FALSE]?
#	ifdef NA_MALLOC_IF_OUT_OF_MEMORY
			EXPECT_FALSE(ptrIsNull);
			memoryCommand.Free(ptr);
#	else
			EXPECT_TRUE(ptrIsNull);
			if (!ptrIsNull) // Just in case!
			{
				memoryCommand.Free(ptr);
			}
#	endif // ifdef NA_MALLOC_IF_OUT_OF_MEMORY
	}

#	undef NA_FIXTURE_NAME
#endif // ifdef NA_TESTS
} // namespace nabi_allocator::tests
