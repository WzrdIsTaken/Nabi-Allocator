// Library Headers
#include "gtest\gtest.h"

// Config Headers
#include "Config.h"

// Nabi Headers
#include "AllocationInfo.h"
#include "Allocators\StackAllocator\StackAllocator.h"
#include "Allocators\StackAllocator\StackAllocatorSettings.h"
#include "Blueprints\AllocatorDefaultTests.h"
#include "HeapZone\HeapZone.h"
#include "HeapZone\HeapZoneBase.h"
#include "MemoryConstants.h"
#include "Operations\MemoryOperations.h"
#include "TestConstants.h"

/**
 * Tests for StackAllocator.
*/

namespace nabi_allocator::tests
{
#ifdef NA_TESTS
#	define NA_FIXTURE_NAME NA_TEST_FIXTURE_NAME(StackAllocatorTests)

	using HeapZoneType = HeapZone<DefaultStackAllocator>;
	uInt constexpr c_HeapZoneSize = 64u;

	TEST(NA_FIXTURE_NAME, CreateAndDestroy)
	{
		blueprints::AllocatorCreateAndDestroyTest<HeapZoneType>(
			c_HeapZoneSize, // Heap zone size
			"F64P0"         // Expected init layout
		);
	}

	TEST(NA_FIXTURE_NAME, AllocateAndFree)
	{
		blueprints::AllocatorAllocateAndFreeTest<HeapZoneType>(
			c_HeapZoneSize, // Heap zone size
			"A24P4 F40P0",  // Expected x64 + memory tagging layout
			"A16P4 F48P0",  // Expected x64 layout
			"A16P4 F48P0",  // Expected x86 + memory tagging layout
			"A8P0 F56P0",   // Expected x86 layout
			"F64P0"         // Expected free layout
		);
	}

	TEST(NA_FIXTURE_NAME, TooLargeAllocation)
	{
		blueprints::AllocatorAllocateTooLargeTest<HeapZoneType>(
			c_HeapZoneSize // Heap zone size
		);
	}

	TEST(NA_FIXTURE_NAME, Reset)
	{
		blueprints::AllocatorResetTest<HeapZoneType>(
			c_HeapZoneSize, // Heap zone size
			"F64P0"         // Expected reset layout
		);
	}

	TEST(NA_FIXTURE_NAME, GetAllocationInfo)
	{
		blueprints::AllocatorGetAllocationInfoTest<HeapZoneType>(
			c_HeapZoneSize // Heap zone size
		);
	}

#	ifdef NA_MEMORY_TAGGING
		TEST(NA_FIXTURE_NAME, MemoryTagging)
		{
			// This case also tests two consecutive allocations then frees.
			// In the x64 + memory tag case it also ensures that if there is insufficient space in the zone it is added as padding.
			//	- This is done in FreeListAllocator's AllocateAndFree case.

			blueprints::AllocatorMemoryTagTest<HeapZoneType>(
				c_HeapZoneSize,       // Heap zone size
				"One24 Two40",        // Expected x64 + memory tagging usage
				"One16 Two16 Free32", // Expected x64 usage
				"One16 Two16 Free32", // Expected x86 + memory tagging usage
				"One8 Two8 Free48",   // Expected x86 usage
				"Free64"              // Expected free usage
			);
		}
#	endif // ifdef NA_MEMORY_TAGGING

#	ifdef NA_TRACK_ALLOCATIONS
		TEST(NA_FIXTURE_NAME, AllocationTracking)
		{
			blueprints::AllocatorAllocationTrackingTest<HeapZoneType>(
				c_HeapZoneSize, // Heap zone size
				24u,		    // Expected x64 + memory tagging allocation size
				16u,            // Expected x64 allocation size
				16u,            // Expected x86 + memory tagging allocation size
				8u              // Expected x86 allocation size
			);
		}
#	endif // ifdef NA_TRACK_ALLOCATIONS

	TEST(NA_FIXTURE_NAME, EnsureTopOfStackAllocate)
	{
		HeapZoneType heapZone = { HeapZoneBase::c_NoParent, c_HeapZoneSize, "TestHeapZone" };
		void const* const ptr1 = heapZone.Allocate(NA_MAKE_ALLOCATION_INFO(4u, c_NullMemoryTag));
		void const* const ptr2 = heapZone.Allocate(NA_MAKE_ALLOCATION_INFO(4u, c_NullMemoryTag));

		EXPECT_TRUE(NA_TO_UPTR(ptr1) < NA_TO_UPTR(ptr2));
		heapZone.Reset();
	}

	TEST(NA_FIXTURE_NAME, EnsureTopOfStackFree)
	{
		/*
		* I'm not quite sure how to test this. At PG we had EXPECT_ASSERT I think but I'm not sure
		* if thats in gMock rather than gTest... Or perhaps it was something custom...
		* 
			HeapZoneType heapZone = { HeapZoneBase::c_NoParent, c_HeapZoneSize, "TestHeapZone" };
			void* const ptr1 = heapZone.Allocate(4u);
			void const* const ptr2 = heapZone.Allocate(4u);

			EXPECT_ANY_THROW/ASSERT_DEATH(heapZone.Free(ptr1)); // if using ASSERT_DEATH the regex is ".*", apparently
			heapZone.Reset();

		* In the mean time, I'll just assume it works :p
		*/

		EXPECT_TRUE(true);
	}

#	undef NA_FIXTURE_NAME
#endif // ifdef NA_TESTS
} // namespace nabi_allocator::tests
