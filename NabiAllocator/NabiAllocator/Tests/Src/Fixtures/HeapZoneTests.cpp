// Library Headers
#include "gtest\gtest.h"

// Config Headers
#include "Config.h"

// Nabi Headers
#include "AllocationInfo.h"
#include "Allocators/AllocatorBase.h"
#include "Allocators/AllocatorBlockInfo.h"
#include "Allocators/FreeListAllocator/FreeListAllocator.h"
#include "DebugUtils.h"
#include "HeapZone/HeapZone.h"
#include "HeapZone/HeapZoneInfo.h"
#include "IntegerTypes.h"
#include "MemoryConstants.h"
#include "Operations/MemoryOperations.h"
#include "TestConstants.h"

/**
 * Tests for HeapZoneBase / HeapZone.
*/

namespace nabi_allocator::tests
{
#ifdef NA_TESTS
#	define NA_FIXTURE_NAME NA_TEST_FIXTURE_NAME(HeapZoneTests)

	uInt constexpr c_MockHeapZoneSize = 1u;
	uInt constexpr c_HeapZoneSize = 64u;
	uInt constexpr c_LargeHeapZoneSize = 128u;

	class MockAllocator final : public AllocatorBase
	{
	public:
		MockAllocator(HeapZoneInfo const& /*heapZoneInfo*/)
			: m_AllocationCount(0u)
		{
		}

		[[nodiscard]] void* Allocate(AllocationInfo const& /*allocationInfo*/, HeapZoneInfo const& /*heapZoneInfo*/) override
		{
			++m_AllocationCount;
			return nullptr;
		}

		void Free(void* memory, HeapZoneInfo const& /*heapZoneInfo*/) override
		{
			--m_AllocationCount;
		}

		void Reset(HeapZoneInfo const& /*heapZoneInfo*/) override
		{
			m_AllocationCount = 0;
		}

		std::deque<AllocatorBlockInfo> IterateThroughHeapZone(
			std::optional<std::function<bool(AllocatorBlockInfo const&)>> const /*action*/, HeapZoneInfo const& /*heapZoneInfo*/) const override
		{
			NA_FUNCTION_NOT_IMPLEMENTED;
			return {};
		}

		[[nodiscard]] inline u32 GetAllocationCount() const noexcept
		{
			return m_AllocationCount;
		}

	private:
		u32 m_AllocationCount;
	};

	TEST(NA_FIXTURE_NAME, CreateAndDestroy)
	{
		HeapZone<MockAllocator> heapZone = { HeapZoneBase::c_NoParent, c_MockHeapZoneSize, "TestHeapZone" };
		HeapZoneInfo const& heapZoneInfo = heapZone.GetZoneInfo();

		{
			uPtr const size = memory_operations::GetMemorySize(heapZoneInfo.m_Start, heapZoneInfo.m_End);
			EXPECT_EQ(c_MockHeapZoneSize, size);
		}
		
		heapZone.Deinit();

		{
			uPtr const size = memory_operations::GetMemorySize(heapZoneInfo.m_Start, heapZoneInfo.m_End);
			EXPECT_EQ(0u, size);
		}
	}

	TEST(NA_FIXTURE_NAME, CheckAllocatorCalls)
	{
		HeapZone<MockAllocator> heapZone = { HeapZoneBase::c_NoParent, c_MockHeapZoneSize, "TestHeapZone" };
		MockAllocator const& mockAllocator = heapZone.GetAllocator();

		void* const ptr1 = heapZone.Allocate(NA_MAKE_ALLOCATION_INFO(1u, c_NullMemoryTag));
		void const* const ptr2 = heapZone.Allocate(NA_MAKE_ALLOCATION_INFO(1u, c_NullMemoryTag));
		EXPECT_EQ(2u, mockAllocator.GetAllocationCount());

		heapZone.Free(ptr1);
		EXPECT_EQ(1u, mockAllocator.GetAllocationCount());

		heapZone.Reset();
		EXPECT_EQ(0u, mockAllocator.GetAllocationCount());
	}

	TEST(NA_FIXTURE_NAME, ParentZone)
	{
		HeapZone<DefaultFreeListAllocator> parentZone = { HeapZoneBase::c_NoParent, c_LargeHeapZoneSize, "ParentZone" };
		HeapZone<DefaultFreeListAllocator> childZone = { &parentZone, c_HeapZoneSize, "ChildZone" };

		HeapZoneInfo const& parentZoneInfo = parentZone.GetZoneInfo();
		HeapZoneInfo const& childZoneInfo = childZone.GetZoneInfo();

		EXPECT_EQ(parentZoneInfo.m_Start + c_BlockHeaderSize, childZoneInfo.m_Start);
		EXPECT_EQ(parentZoneInfo.m_Start + c_BlockHeaderSize + c_HeapZoneSize, childZoneInfo.m_End);
	}

	TEST(NA_FIXTURE_NAME, AllHeapZonesUpdates)
	{
		EXPECT_EQ(HeapZoneBase::GetHeapZoneCount(), 0u);

		{
			HeapZone<MockAllocator> heapZone1 = { HeapZoneBase::c_NoParent, c_MockHeapZoneSize, "TestHeapZone1" };
			EXPECT_EQ(HeapZoneBase::GetHeapZoneCount(), 1u);

			{
				HeapZone<MockAllocator> heapZone2 = { HeapZoneBase::c_NoParent, c_MockHeapZoneSize, "TestHeapZone2" };
				EXPECT_EQ(HeapZoneBase::GetHeapZoneCount(), 2u);
			}

			EXPECT_EQ(HeapZoneBase::GetHeapZoneCount(), 1u);
		}
		
		EXPECT_EQ(HeapZoneBase::GetHeapZoneCount(), 0u);
	}

	TEST(NA_FIXTURE_NAME, FindHeapZoneForPtr)
	{
		HeapZone<DefaultFreeListAllocator> heapZone1 = { HeapZoneBase::c_NoParent, c_HeapZoneSize, "TestHeapZone1" };
		HeapZone<DefaultFreeListAllocator> heapZone2 = { HeapZoneBase::c_NoParent, c_HeapZoneSize, "TestHeapZone2" };

		void* ptr1 = heapZone1.Allocate(NA_MAKE_ALLOCATION_INFO(4u, c_NullMemoryTag));
		void* ptr2 = heapZone2.Allocate(NA_MAKE_ALLOCATION_INFO(4u, c_NullMemoryTag));

		EXPECT_TRUE(heapZone1.ContainsPtr(ptr1));
		EXPECT_FALSE(heapZone1.ContainsPtr(ptr2));

		EXPECT_EQ(HeapZoneBase::FindHeapZoneForPtr(ptr2), &heapZone2);
		EXPECT_FALSE(HeapZoneBase::FindHeapZoneForPtr(ptr2) == &heapZone1);

		heapZone1.Free(ptr1);
		heapZone2.Free(ptr2);
	}

#	undef NA_FIXTURE_NAME
#endif // ifdef NA_TESTS
} // namespace nabi_allocator::tests
