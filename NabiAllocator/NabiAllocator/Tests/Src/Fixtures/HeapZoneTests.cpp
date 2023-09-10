// Library Headers
#include "gtest\gtest.h"

// Config Headers
#include "Config.h"

// Nabi Headers
#include "Allocators/AllocatorBase.h"
#include "HeapZone/HeapZone.h"
#include "HeapZone/HeapZoneInfo.h"
#include "IntegerTypes.h"
#include "MemoryConstants.h"
#include "Operations/MemoryOperations.h"

/**
 * Tests for HeapZoneBase / HeapZone
*/

namespace nabi_allocator::tests
{
#ifdef NABI_ALLOCATOR_TESTS
#	define TEST_FIXTURE_NAME HeapZoneTests

	class MockAllocator final : public AllocatorBase
	{
	public:
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
		HeapZone<MockAllocator> heapZone{ c_BlockAllignment, "TestHeapZone" };
		HeapZoneInfo const& heapZoneInfo = heapZone.GetZoneInfo();

		{
			uPtr const heapZoneSize = memory_operations::GetMemorySize(heapZoneInfo.m_Start, heapZoneInfo.m_End);
			EXPECT_EQ(c_BlockAllignment, heapZoneSize);
		}
		
		heapZone.Deinit();

		{
			uPtr const heapZoneSize = memory_operations::GetMemorySize(heapZoneInfo.m_Start, heapZoneInfo.m_End);
			EXPECT_EQ(0u, heapZoneSize);
		}
	}

	TEST(TEST_FIXTURE_NAME, CheckAllocatorCalls)
	{
		HeapZone<MockAllocator> heapZone{ c_BlockAllignment, "TestHeapZone" };

		void* ptr = heapZone.Allocate(1u);
		EXPECT_EQ(1u, heapZone.GetAllocator().GetAllocationCount());

		heapZone.Free(ptr);
		EXPECT_EQ(0u, heapZone.GetAllocator().GetAllocationCount());
	}

#	undef TEST_FIXTURE_NAME
#endif // ifdef NABI_ALLOCATOR_TESTS
} // namespace nabi_allocator::tests
