// STD Headers
#include <memory>

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

	class TestClass
	{
	public:
		static u32 s_Count;

		TestClass()
		{
			++s_Count;
		}
		~TestClass()
		{
			--s_Count;
		}
	};
	u32 TestClass::s_Count = 0u;

	TEST(NA_FIXTURE_NAME, BasicWorkFlow)
	{
		NA_MAKE_HEAP_ZONE_AND_SET_SCOPE(
			HeapZone<FreeListAllocator<c_FreeListAllocatorDefaultSettings>>, // Heap zone type
			HeapZoneBase::c_NoParent,                                        // Heap zone parent
			1_MB,                                                            // Heap zone size
			"Allocator",                                                     // Heap zone debug name
			c_NullMemoryTag                                                  // Heap zone scope memory tag
		);

		// Raw ptrs
		auto const* const individual = new TestClass();
		auto const* const arr = new TestClass[3u];
		EXPECT_EQ(TestClass::s_Count, 4u);

		// STD container
		{
			std::vector<TestClass*> vec = { new TestClass(), new TestClass(), new TestClass() };
			EXPECT_EQ(TestClass::s_Count, 7u);

			for (uInt i = 0u; i < 50u; ++i) 
			{
				vec.push_back(new TestClass());
			}
			EXPECT_EQ(TestClass::s_Count, 57u);

			for (TestClass* tC : vec) 
			{
				delete tC;
			}
			vec.clear();
			EXPECT_EQ(TestClass::s_Count, 4u);
		}

		// Smart ptrs
		{
			{
				auto const uniquePtr = std::make_unique<TestClass>();
				auto const sharedPtr = std::make_shared<TestClass>();
				std::weak_ptr<TestClass> const weakPtr = sharedPtr;
				EXPECT_EQ(TestClass::s_Count, 6u);
			}
			EXPECT_EQ(TestClass::s_Count, 4u);
		}
		
		delete individual;
		delete[] arr;
		EXPECT_EQ(TestClass::s_Count, 0u);
	}

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
