// STD Headers
#include <cstdlib>

// Library Headers
#include "gtest\gtest.h"

// Config Headers
#include "Config.h"
#include "Version.h"

// Nabi Headers
#include "CharacterConstants.h"
#include "DebugUtils.h"
#include "IntegerTypes.h"

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

#ifdef NABI_ALLOCATOR_TESTS
	::testing::InitGoogleTest(&argc, argv);
	s32 const testResults = RUN_ALL_TESTS();

	NABI_ALLOCATOR_ASSERT(testResults == EXIT_SUCCESS, 
		"One or more of the tests failed. See the console output for details, or run the test explorer.");
	NABI_ALLOCATOR_LOG("All tests complete!" << character_constants::c_NewLine);
#endif // ifdef NABI_ALLOCATOR_TESTS

	return EXIT_SUCCESS;
}
