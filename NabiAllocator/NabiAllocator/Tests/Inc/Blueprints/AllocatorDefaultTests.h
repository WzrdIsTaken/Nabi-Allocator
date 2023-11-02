#pragma once

// Nabi Headers
#include <string>

// Config Headers
#include "Config.h"

// Nabi Headers
#include "HeapZone\HeapZoneBase.h"

/*
 * A collection of standard tests for every allocator to ensure their core functionality works.
*/

namespace nabi_allocator::tests::blueprints
{
#ifdef NA_TESTS
	template<is_heap_zone HeapZoneType>
	void AllocatorCreateAndDestroyTest(uInt const heapZoneSize, std::string const& expectedInitLayout);

	template<is_heap_zone HeapZoneType>
	void AllocatorAllocateAndFreeTest(uInt const heapZoneSize,
		std::string const& expectedX64MemoryTaggingLayout, std::string const& expectedX64Layout,
		std::string const& expectedX86MemoryTaggingLayout, std::string const& expectedX86Layout,
		std::string const& expectedFreeLayout);

	template<is_heap_zone HeapZoneType>
	void AllocatorResetTest(uInt const heapZoneSize, std::string const& expectedResetLayout);

#	ifdef NA_MEMORY_TAGGING
		template<is_heap_zone HeapZoneType>
		void AllocatorMemoryTagTest(uInt const heapZoneSize,
			std::string const& expectedX64MemoryTaggingUsage, std::string const& expectedX64Usage,
			std::string const& expectedX86MemoryTaggingUsage, std::string const& expectedX86Usage,
			std::string const& expectedFreeUsage);
#	endif // ifdef NA_MEMORY_TAGGING
#endif // ifdef NA_TESTS
} // namespace nabi_allocator::tests::blueprints

// Include Inline
#include "Tests\Inl\Blueprints\AllocatorDefaultTests.inl"

/*
 * I was considering going with a macro like this:
 * 
  #define NA_IMPLEMENT_TEST(testFixtureName, testCaseName, testFunction, ...) \
		TEST(testFixtureName, testCaseName) \
		{ \
			NA_CONCAT(nabi_allocator::tests::, testFunction)(__VA_ARGS__); \
		}

   #define NA_IMPLEMENT_DEFAULT_ALLOCATOR_TESTS(testFixtureName, allocatorType, heapZoneSize, \
	expectedInitLayout, \
	expectedX64MemoryTaggingLayout, expectedX64Layout, expectedX86MemoryTaggingLayout, expectedX86Layout, expectedFreeLayout, \
	expectedResetLayout) \
		NA_IMPLEMENT_TEST(testFixtureName, CreateAndDestroy, blueprints::AllocatorCreateAndDestroyTest<allocatorType>, heapZoneSize, \
			expectedInitLayout) \
			\
		NA_IMPLEMENT_TEST(testFixtureName, AllocateAndFree, blueprints::AllocatorAllocateAndFreeTest<allocatorType>, heapZoneSize, \
			expectedX64MemoryTaggingLayout, expectedX64Layout, expectedX86MemoryTaggingLayout, expectedX86Layout, expectedFreeLayout) \
			\
		NA_IMPLEMENT_TEST(testFixtureName, Reset, blueprints::AllocatorResetTest<allocatorType>, heapZoneSize, \
			expectedResetLayout) 

 * (or something) to implement the default tests. But looking at it... it seems a little jank. Also if one of the tests starts
 * failing, with a macro like this its a lot harder to breakpoint in and examine whats going on. Despite the code repetition,
 * I think verbosness is good in this case.
*/
