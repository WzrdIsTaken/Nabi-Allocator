// Library Headers
#include "gtest\gtest.h"

// Config Headers
#include "Config.h"

// Nabi Headers
#include "Allocators/AllocatorBase.h"
#include "HeapZone/HeapZone.h"
#include "HeapZone/HeapZoneInfo.h"
#include "IntegerTypes.h"
#include "Operations/MemoryOperations.h"

/**
 * Tests for HeapZoneBase / HeapZone.
 * 
 * In the tests, we set heapZoneSize to MockAllocator::c_BlockAllignment so these two values are
 * always in sync and we never hit the memory_operations::IsAlligned assert in HeapZoneBase.cpp.
*/

namespace nabi_allocator::tests
{
#ifdef NABI_ALLOCATOR_TESTS
#	define TEST_FIXTURE_NAME HeapZoneTests

	class MockAllocator final : public AllocatorBase
	{
	public:
		static uInt constexpr c_BlockAllignment = 8u;

		MockAllocator(HeapZoneInfo const& /*heapZoneInfo*/)
			: m_AllocationCount(0u)
		{
		}

		[[nodiscard]] void* Allocate(uInt const numBytes, HeapZoneInfo const& /*heapZoneInfo*/) override
		{
			++m_AllocationCount;
			return nullptr;
		}

		void Free(void* memory, HeapZoneInfo const& /*heapZoneInfo*/) override
		{
			--m_AllocationCount;
		}

		inline u32 GetAllocationCount() const noexcept
		{
			return m_AllocationCount;
		}

	private:
		u32 m_AllocationCount;
	};

	TEST(TEST_FIXTURE_NAME, CreateAndDestroyHeapZone)
	{
		uInt constexpr heapZoneSize = MockAllocator::c_BlockAllignment;
		HeapZone<MockAllocator> heapZone{ heapZoneSize, "TestHeapZone" };
		HeapZoneInfo const& heapZoneInfo = heapZone.GetZoneInfo();

		{
			uPtr const size = memory_operations::GetMemorySize(heapZoneInfo.m_Start, heapZoneInfo.m_End);
			EXPECT_EQ(heapZoneSize, size);
		}
		
		heapZone.Deinit();

		{
			uPtr const heapZoneSize = memory_operations::GetMemorySize(heapZoneInfo.m_Start, heapZoneInfo.m_End);
			EXPECT_EQ(0u, heapZoneSize);
		}
	}

	TEST(TEST_FIXTURE_NAME, CheckAllocatorCalls)
	{
		uInt constexpr heapZoneSize = MockAllocator::c_BlockAllignment;
		HeapZone<MockAllocator> heapZone{ heapZoneSize, "TestHeapZone" };

		void* ptr = heapZone.Allocate(1u);
		EXPECT_EQ(1u, heapZone.GetAllocator().GetAllocationCount());

		heapZone.Free(ptr);
		EXPECT_EQ(0u, heapZone.GetAllocator().GetAllocationCount());
	}

#	undef TEST_FIXTURE_NAME
#endif // ifdef NABI_ALLOCATOR_TESTS
} // namespace nabi_allocator::tests
