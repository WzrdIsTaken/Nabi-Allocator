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
		using namespace free_list_allocator;	// TODO yh get rid of these namespaces
		using namespace stack_allocator;

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
		NA_HEAP_ZONE_SCOPE(&childZone, type_utils::ToUnderlying(MemoryTag::General), &memoryCommand); // Initial scope
		*/
	}

#	undef NA_FIXTURE_NAME
#endif // ifdef NA_WORKFLOWS
}
