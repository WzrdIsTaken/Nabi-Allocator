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
*/

namespace nabi_allocator::tests
{
#ifdef NABI_ALLOCATOR_TESTS
#	define TEST_FIXTURE_NAME HeapZoneTests

	static uInt constexpr c_TestHeapZoneSize = 1u;

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
		HeapZone<MockAllocator> heapZone{ c_TestHeapZoneSize, "TestHeapZone" };
		HeapZoneInfo const& heapZoneInfo = heapZone.GetZoneInfo();

		{
			uPtr const size = memory_operations::GetMemorySize(heapZoneInfo.m_Start, heapZoneInfo.m_End);
			EXPECT_EQ(c_TestHeapZoneSize, size);
		}
		
		heapZone.Deinit();

		{
			uPtr const size = memory_operations::GetMemorySize(heapZoneInfo.m_Start, heapZoneInfo.m_End);
			EXPECT_EQ(0u, size);
		}
	}

	TEST(TEST_FIXTURE_NAME, CheckAllocatorCalls)
	{
		HeapZone<MockAllocator> heapZone{ c_TestHeapZoneSize, "TestHeapZone" };
		MockAllocator const& mockAllocator = heapZone.GetAllocator();

		void* ptr = heapZone.Allocate(1u);
		EXPECT_EQ(1u, mockAllocator.GetAllocationCount());

		heapZone.Free(ptr);
		EXPECT_EQ(0u, mockAllocator.GetAllocationCount());
	}

#	undef TEST_FIXTURE_NAME
#endif // ifdef NABI_ALLOCATOR_TESTS
} // namespace nabi_allocator::tests
