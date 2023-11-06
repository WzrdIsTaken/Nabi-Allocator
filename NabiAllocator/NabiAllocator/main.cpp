// STD Headers
#include <cstdlib>
#include <string>

// Library Headers
#include "gtest\gtest.h"

// Config Headers
#include "Config.h"
#include "Version.h"

// Nabi Headers
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
	NA_LOG(NA_LOG_PREP, NA_LOG_INFO, NA_LOG_CATEGORY_CORE, "Launching Nabi Allocator, version " << 
		NA_VERSION << ".", NA_LOG_END_NL);

#ifdef NA_DEBUG
	_CrtSetReportMode(_CRT_ASSERT, _CRTDBG_MODE_WNDW);
#endif // ifdef NA_DEBUG

#if defined NA_TESTS || defined NA_BENCHMARKS
	std::string filterPattern = {};
#	ifdef NA_TESTS
		filterPattern += NA_CONCAT(NA_XSTR(NA_TEST_IDENTIFIER), NA_GTEST_FILTER_SEPERATOR);
#	endif // ifdef NA_TESTS
#	ifdef NA_BENCHMARKS
		filterPattern += NA_CONCAT(NA_XSTR(NA_BENCHMARK_IDENTIFIER), NA_GTEST_FILTER_SEPERATOR);
#	endif // ifdef NA_BENCHMARKS
#	ifdef NA_WORKFLOWS
		filterPattern += NA_CONCAT(NA_XSTR(NA_WORKFLOW_IDENTIFIER), NA_GTEST_FILTER_SEPERATOR);
#	endif // ifdef NA_WORKFLOWS
	::testing::GTEST_FLAG(filter) = filterPattern.c_str();

	::testing::InitGoogleTest(&argc, argv);
	s32 const testResults = RUN_ALL_TESTS();

	NA_ASSERT(testResults == EXIT_SUCCESS, 
		"One or more of the tests failed. See the console output for details, or run the test explorer.");
	NA_LOG(NA_LOG_PREP, NA_LOG_INFO, NA_LOG_CATEGORY_TEST, "All tests complete!", NA_LOG_END_NL);
#endif // ifdef NA_TESTS || NA_BENCHMARKS

	return EXIT_SUCCESS;
}
