// Library Headers
#include "gtest\gtest.h"

// Config Headers
#include "Config.h"

// Nabi Headers
#include "AllocationInfo.h"
#include "Allocators\FreeListAllocator\FreeListAllocator.h"
#include "Allocators\StackAllocator\StackAllocator.h"
#include "HeapZone\HeapZone.h"
#include "HeapZone\HeapZoneInfo.h"
#include "HeapZone\HeapZoneScope.h"
#include "MemoryCommand.h"
#include "MemoryConstants.h"
#include "TestConstants.h"
#include "TypeUtils.h"

/**
 * A test for the full workflow of NabiAllocator.
*/

namespace nabi_allocator::tests
{
#ifdef NA_WORKFLOWS
#	define NA_FIXTURE_NAME NA_WORKFLOW_FIXTURE_NAME(WorkFlows)

	TEST(NA_FIXTURE_NAME, FullWorkFlow)
	{
		/*
		* TODO continue with this when we have the new / delete stuff done. otherwise 
		* 
		uInt constexpr parentZoneNumBytes = 256u;
		uInt constexpr childZoneNumBytes = parentZoneNumBytes;

		enum class MemoryTag : memoryTag
		{
			General = 1u << 1u,
			Rendering = 1u << 2u, // Just some example sections where allocations could come from
			All = ~0u,

			ENUM_COUNT = 3u
		};

		MemoryCommand memoryCommand = {};
		HeapZone<StackAllocator<c_StackAllocatorDefaultSettings>> parentZone = { HeapZoneBase::c_NoParent, parentZoneNumBytes, "ParentZone" };
		HeapZone<FreeListAllocator<c_FreeListAllocatorDefaultSettings>> childZone = { &parentZone, childZoneNumBytes, "ChildZone" };
		NA_SET_HEAP_ZONE_SCOPE(&childZone, type_utils::ToUnderlying(MemoryTag::General), &memoryCommand); // Initial scope

		^ actually, use NA_MAKE_HEAP_ZONE_AND_SET_SCOPE here
		  (or at least have a test in FullWorkflow which uses it. Perhaps a BasicWorkFlow test which just has the absolute basics it needs?)

		*/

		/*
		NA_MAKE_HEAP_ZONE_AND_SET_SCOPE(
			HeapZone<StackAllocator<c_StackAllocatorDefaultSettings>>, // Heap zone type
			HeapZoneBase::c_NoParent,                                  // Heap zone parent
			64u,                                                       // Heap zone size
			"Test",                                                    // Heap zone debug name
			c_NullMemoryTag                                            // Heap zone scope memory tag
		);
		*/
	}

#	undef NA_FIXTURE_NAME
#endif // ifdef NA_WORKFLOWS
}
