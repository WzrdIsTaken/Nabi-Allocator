// STD Headers
#include <memory>

// Library Headers
#include "gtest\gtest.h"

// Config Headers
#include "Config.h"

// Nabi Headers
#include "AllocationInfo.h"
#include "Allocators\BlockInfoIndex.h"
#include "Allocators\FreeListAllocator\FreeListAllocator.h"
#include "Allocators\StackAllocator\StackAllocator.h"
#include "Allocators\AllocatorUtils.h"
#include "HeapZone\HeapZone.h"
#include "HeapZone\HeapZoneInfo.h"
#include "HeapZone\HeapZoneScope.h"
#include "MemoryCommand.h"
#include "MemoryConstants.h"
#include "Operations\BitOperations.h"
#include "TestConstants.h"
#include "TypeUtils.h"

/**
 * A test for the full workflow of NabiAllocator.
 * 
 * Note:
 *	- If NA_OVERRIDE_NEW_DELETE isn't defined, these will just run with the default new/delete implementation
 *    (or with whatever implementation a user has defined I guess..).
 *		- Also note if this isn't defined then checking memory tags of globally allocated memory won't work
 *        (see the FullWorkFlow test - it *will* compile but *won't* meet the condition).
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
			HeapZone<DefaultFreeListAllocator>, // Heap zone type
			HeapZoneBase::c_NoParent,           // Heap zone parent
			1_MB,                               // Heap zone size
			"Allocator",                        // Heap zone debug name
			c_NullMemoryTag                     // Heap zone scope memory tag
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
		uInt constexpr parentZoneNumBytes = 256u;
		uInt constexpr childZoneNumBytes = parentZoneNumBytes / 2u;

		enum class MemoryTag : memoryTag
		{
			General    = 1u << 1u,
			Rendering  = 1u << 2u, // Just some example sections where allocations could come from
			All        = ~0u,

			ENUM_COUNT = 3u
		};

		HeapZone<DefaultStackAllocator> parentZone = { HeapZoneBase::c_NoParent, parentZoneNumBytes, "ParentZone" };
		HeapZone<DefaultFreeListAllocator> childZone = { &parentZone, childZoneNumBytes, "ChildZone" };
		NA_SET_HEAP_ZONE_SCOPE(&childZone, type_utils::ToUnderlying(MemoryTag::General)); // Initial scope

		auto const verifyBlockMemoryTag =
			[](uPtr const heapZoneStart, MemoryTag const _memoryTag) -> bool
			{
				bool result = false;
				BlockHeader const* const blockHeader = NA_REINTERPRET_MEMORY_DEFAULT(BlockHeader, heapZoneStart);

				result =
#ifdef NA_MEMORY_TAGGING
						(blockHeader->m_MemoryTag == type_utils::ToUnderlying(_memoryTag))
#else
						true
#endif // ifdef NA_MEMORY_TAGGING
					;

				return result;
			};
		uPtr const childZoneStart = childZone.GetZoneInfo().m_Start;

		// General tag allocation
		{
			auto const* const allocation = new int();
#ifdef NA_OVERRIDE_NEW_DELETE
			EXPECT_TRUE(verifyBlockMemoryTag(childZoneStart, MemoryTag::General));
#endif // ifdef NA_OVERRIDE_NEW_DELETE
			delete allocation;
		}

		// Switch tag and allocate
		{
			NA_SET_HEAP_ZONE_SCOPE(c_SameZone, type_utils::ToUnderlying(MemoryTag::Rendering));
			auto const* const allocation = new int();
#ifdef NA_OVERRIDE_NEW_DELETE
			EXPECT_TRUE(verifyBlockMemoryTag(childZoneStart, MemoryTag::Rendering));
#endif // ifdef NA_OVERRIDE_NEW_DELETE
			delete allocation;
		}

		// Switch to the unmanaged heap and allocate
		{
			NA_SET_HEAP_ZONE_SCOPE(&c_UnmanagedHeap, c_SameTag);
			auto const* const allocation = new int(21);
			EXPECT_TRUE(allocation); EXPECT_EQ(*allocation, 21);
			delete allocation;
		}

		// Check heap zone usage is as expected
#ifdef NA_TRACK_ALLOCATIONS
		EXPECT_EQ(parentZone.GetAllocator().GetStats().m_ActiveAllocationCount, 1u);
		EXPECT_EQ(childZone.GetAllocator().GetStats().m_ActiveAllocationCount, 0u);
#endif // ifdef NA_TRACK_ALLOCATIONS
	}

	TEST(NA_FIXTURE_NAME, LocalHeapZoneWorkFlow)
	{
		// *The 'no global heap zone workflow' is tested, well, in a lot of other places. Eg: [Free/Stack]AllocatorTests.cpp->AllocatorDefaultTests.inl*

		NA_MAKE_HEAP_ZONE_AND_SET_SCOPE(
			HeapZone<DefaultFreeListAllocator>, // Heap zone type
			HeapZoneBase::c_NoParent,           // Heap zone parent
			256u,                               // Heap zone size
			"Allocator",                        // Heap zone debug name
			c_NullMemoryTag                     // Heap zone scope memory tag
		);

		HeapZoneBase* const topHeapZone = MemoryCommand::Instance().GetTopHeapZoneScope()->GetHeapZone();
		{
			HeapZone<DefaultFreeListAllocator> localZone = { topHeapZone, 64u, "LocalZone" };
			EXPECT_EQ(MemoryCommand::Instance().GetHeapZoneScopeCount(), 1u);

			void* const allocation = localZone.Allocate(NA_MAKE_ALLOCATION_INFO(4u, c_NullMemoryTag));
			localZone.Free(allocation);
		}

#if defined NA_TRACK_ALLOCATIONS && defined NA_DEBUG
		EXPECT_EQ(reinterpret_cast<HeapZone<DefaultFreeListAllocator>*>(topHeapZone)->GetAllocator().GetStats().m_ActiveAllocationCount, 0u);
#endif // ifdef NA_TRACK_ALLOCATIONS && NA_DEBUG
	}

#	undef NA_FIXTURE_NAME
#endif // ifdef NA_WORKFLOWS
}
