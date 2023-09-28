// STD Headers
#include <cstdlib>
#include <string>

// Library Headers
#include "gtest\gtest.h"

// Config Headers
#include "Config.h"
#include "Version.h"

// Nabi Headers
#include "CharacterConstants.h"
#include "DebugUtils.h"
#include "IntegerTypes.h"
#include "MacroUtils.h"
#include "TestConstants.h"

/**
* butterfly
*/

using namespace nabi_allocator;

s32 main(s32 argc, char* argv[])
{
	NABI_ALLOCATOR_LOG("Launching Nabi Allocator, version " << NABI_ALLOCATOR_VERSION << "." << character_constants::c_NewLine);

#ifdef NABI_ALLOCATOR_DEBUG
	_CrtSetReportMode(_CRT_ASSERT, _CRTDBG_MODE_WNDW);
#endif // ifdef NABI_ALLOCATOR_DEBUG

#if defined NABI_ALLOCATOR_TESTS || defined NABI_ALLOCATOR_BENCHMARKS
	std::string filterPattern = {};
#	ifdef NABI_ALLOCATOR_TESTS
		filterPattern += NABI_ALLOCATOR_CONCAT(NABI_ALLOCATOR_XSTR(NABI_ALLOCATOR_TEST_IDENTIFIER), "*:");
#	endif // ifdef NABI_ALLOCATOR_TESTS
#	ifdef NABI_ALLOCATOR_BENCHMARKS
		filterPattern += NABI_ALLOCATOR_CONCAT(NABI_ALLOCATOR_XSTR(NABI_ALLOCATOR_BENCHMARK_IDENTIFIER), "*:");
#	endif // ifdef NABI_ALLOCATOR_BENCHMARKS
	::testing::GTEST_FLAG(filter) = filterPattern.c_str();

	::testing::InitGoogleTest(&argc, argv);
	s32 const testResults = RUN_ALL_TESTS();

	NABI_ALLOCATOR_ASSERT(testResults == EXIT_SUCCESS, 
		"One or more of the tests failed. See the console output for details, or run the test explorer.");
	NABI_ALLOCATOR_LOG("All tests complete!" << character_constants::c_NewLine);
#endif // ifdef NABI_ALLOCATOR_TESTS || NABI_ALLOCATOR_BENCHMARKS

	/*
	using namespace free_list_allocator;

	FreeListAllocatorSettings constexpr se =
	{
		SearchAlgorithm::FirstFit,
		0u
	};

	HeapZone<FreeListAllocator<se>> heapZone(1 * 1024 * 1024, "TestHeapZone");
	std::vector<void*> ptrs = {};
	ptrs.reserve(10000);

	//auto startTime = std::chrono::steady_clock::now();
	auto stopWatch = profile_utils::StopWatch{};

	for (int i = 0; i < 10000; i++)
	{
		void* ptr = heapZone.Allocate(4);
		//void* ptr = malloc(4);
		ptrs.push_back(ptr);

	
		bool alloc = rand() % 2;
		if (alloc)
		{
			//void* ptr = malloc(rand() % 200);
			void* ptr = heapZone.Allocate(rand() % 2000);
			ptrs.push_back(ptr);
		}
		else
		{
			if (!ptrs.empty())
			{
				uInt index = rand() % ptrs.size();

				void* ptr = ptrs.at(index);
				ptrs.erase(std::next(ptrs.begin(), index));
				//free(ptr);
				heapZone.Free(ptr);
			}
		}
	
	}

	auto duration1 = stopWatch.GetElapsedTime();

	//std::string s = GetMemoryLayout(heapZone.GetAllocator(), heapZone.GetZoneInfo());
	//int i = 0;

	for (uInt i = 0; i < ptrs.size(); ++i)
	{
		heapZone.Free(ptrs[i]);
		//free(ptrs[i]);
	}

	//auto endTime2 = std::chrono::high_resolution_clock::now();

	
	auto duration2 = stopWatch.GetElapsedTime();;

	std::cout << "Alloc:" << duration1;
	std::cout << "\n";
	std::cout << "Free: " << duration2;
	std::cout << "\n";
	std::cout << "Total: " << duration1 + duration2;
	std::cout << "\n";

	*/

	//#include "HeapZone\HeapZone.h"
//#include "Allocators\FreeListAllocator\FreeListAllocator.h"
//#include <chrono>
//#include "ProfileUtils.h"

	// TOOD make a stopwatch class or something https://stackoverflow.com/questions/60572206/how-to-time-a-methods-execution-using-chrono-in-c
	// We can then use than class to profile futher. Apperently the TryCoalesceBlock function is the slowest now. It has some futher function calls
	// in it though, and I can't see exactly what I can push futher right now.

	// Also profile the use of forceinline in the sections named in todo?

	// Then clean up main etc.
	// Write some proper generic benchmark stuff that takes in an AllocatorBase

	// ^ clean up all the code i wrote doing this. 

	// TODO is there anything we need to add to the additional include dirs?
	// or less? dont actually include inl perhaps..? idk..

	return EXIT_SUCCESS;
}
