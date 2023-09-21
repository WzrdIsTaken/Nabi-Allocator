// STD Headers
#include <string>

// Library Headers
#include "gtest\gtest.h"

// Config Headers
#include "Config.h"

// Nabi Headers
#include "Allocators\AllocatorUtils.h"
#include "Allocators\FreeListAllocator\FreeListAllocator.h"
#include "Allocators\FreeListAllocator\FreeListAllocatorSettings.h"
#include "HeapZone\HeapZone.h"
#include "HeapZone\HeapZoneInfo.h"

#include "MemoryConstants.h"

/**
 * Tests for FreeListAllocator
*/

namespace nabi_allocator::tests
{
#ifdef NABI_ALLOCATOR_TESTS
#	define TEST_FIXTURE_NAME FreeListAllocatorTests

	// Six things
	// - Tests for the free list allocator (make sure there is a test for AddFreeListNode's logic and that all brances are hit)
	// - Do we need the reset memory calls in the cpp? look at them. [I think we need to write the tests first and see]
	// - The basic interface tests for the free list allocator (eg, allocating/freeing/usage with heapzone) will be the same for all allocators.
	//   Therefore, once we have written / tested the tests perhaps package them up into a macro or something. We could do like
	//   NABI_ALLOCATOR_IMPLEMENT_ALLOCATOR_BASIC_TESTS(testFixtureName, allocType) and this would create the gTest TEST macros
	//   for the basic tests which we could use for all allocators.
	// - Can friend classes change privacy levels of virtual memebers? if so, make a TEXT_FIXTURE_FRIEND thing or whatever googletest @ pg.
	//   We can then more easily test the inner working functions of free list allocator and other classes
	// - Is it worth moving the search code in FreeListAllocator::TryFindFreeBlock to a different place? Eg in SearthAlogirthm can have
	//   [[nodiscard]] [FindViaBestFit, FindViaFirstFit] etc functions. This could make the code more readable, modular and easy to test.
	//       - If we do this (^) remove the "The code in TryFindFreeBlock is repeated..." comment line in FreeListAllocator.h
	//		 - Also remember to remove the <limits> include from FreeListAllocator.inl
	// - Make sure you test with all the defines in Config.h enabled/disabled.
	// - And make sure that the includes in this file are only what we need
	// 
	// - After we have written these tests, do the todos
	// - An interesting thing we could do as well is a random alloc / free loop just to see whtats happening

	TEST(TEST_FIXTURE_NAME, notatestrnjustme)
	{
		// TODO - Today we actually need to write the tests! First go through the change list and make sure everything is ok/no todos.
		// Read through all the comments in this file and see if there is anything we need to action first.
		// Once this is done, commit
		// Then write the tests.
		// 
		// TODO Need to work out what the correct values of these allocations should be so can double check them
		// TODO ctrl f TODO <--!


		using namespace free_list_allocator;

		FreeListAllocatorSettings constexpr settings = {
			.m_SearchAlgorithm = SearchAlgorithm::BestFit,
			.m_BestFitLeniency = 0u
		};

		// Also could be interesting to write a test like this
		HeapZone<FreeListAllocator<settings>> heapZone = { 4_GB, "DebugName" };
		std::vector<void*> allocatedStuff;
		uInt constexpr minSize = 1u;
		uInt constexpr maxSize = 100u;

		for (int i = 0; i < 100; i++)
		{
			bool const shouldAlloc = static_cast<bool>(rand() % 2);
			if (shouldAlloc)
			{
				void* ptr = heapZone.Allocate(rand() % maxSize);
				allocatedStuff.push_back(ptr);
			}
			else
			{
				if (!allocatedStuff.empty())
				{
					int index = rand() % allocatedStuff.size();
					void* d = allocatedStuff.at(index);
					allocatedStuff.erase(std::next(allocatedStuff.begin(), index));

					heapZone.Free(d);
				}
			}
		}



		//void* ret = heapZone.Allocate(4u);
		//void* ret2 = heapZone.Allocate(4u);
		//heapZone.Free(ret);
		//heapZone.Free(ret2);


		

		std::string const memoryLayout = GetMemoryLayout(heapZone.GetAllocator(), heapZone.GetZoneInfo());

		int i = 0;

		//heapZone.GetAllocator().IterateThroughMemoryPool();

		//HeapZoneInfo heapZoneInfo = {};
		//FreeListAllocator<c_FreeListAllocatorDefaultSettings> freeListAllocator = { heapZoneInfo };
	}

#	undef TEST_FIXTURE_NAME
#endif // ifdef NABI_ALLOCATOR_TESTS
} // namespace nabi_allocator::tests
