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
		filterPattern += NABI_ALLOCATOR_CONCAT(NABI_ALLOCATOR_XSTR(NABI_ALLOCATOR_TEST_IDENTIFIER), NABI_ALLOCATOR_GTEST_FILTER_SEPERATOR);
#	endif // ifdef NABI_ALLOCATOR_TESTS
#	ifdef NABI_ALLOCATOR_BENCHMARKS
		filterPattern += NABI_ALLOCATOR_CONCAT(NABI_ALLOCATOR_XSTR(NABI_ALLOCATOR_BENCHMARK_IDENTIFIER), NABI_ALLOCATOR_GTEST_FILTER_SEPERATOR);
#	endif // ifdef NABI_ALLOCATOR_BENCHMARKS
	::testing::GTEST_FLAG(filter) = filterPattern.c_str();

	::testing::InitGoogleTest(&argc, argv);
	s32 const testResults = RUN_ALL_TESTS();

	NABI_ALLOCATOR_ASSERT(testResults == EXIT_SUCCESS, 
		"One or more of the tests failed. See the console output for details, or run the test explorer.");
	NABI_ALLOCATOR_LOG("All tests complete!" << character_constants::c_NewLine);
#endif // ifdef NABI_ALLOCATOR_TESTS || NABI_ALLOCATOR_BENCHMARKS

	return EXIT_SUCCESS;
}
