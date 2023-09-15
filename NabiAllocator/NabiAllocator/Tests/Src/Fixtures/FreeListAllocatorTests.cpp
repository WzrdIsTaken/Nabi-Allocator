// Library Headers
#include "gtest\gtest.h"

// Config Headers
#include "Config.h"

/**
 * Tests for FreeListAllocator
*/

namespace nabi_allocator::tests
{
#ifdef NABI_ALLOCATOR_TESTS
#	define TEST_FIXTURE_NAME BitOperationsTests

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
	// - Make sure you test with all the defines in Config.h enabled/disabled.

#	undef TEST_FIXTURE_NAME
#endif // ifdef NABI_ALLOCATOR_TESTS
} // namespace nabi_allocator::tests
