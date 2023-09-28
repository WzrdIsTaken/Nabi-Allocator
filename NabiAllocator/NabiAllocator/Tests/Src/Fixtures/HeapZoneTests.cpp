// Library Headers
#include "gtest\gtest.h"

// Config Headers
#include "Config.h"

// Nabi Headers
#include "Allocators/AllocatorBase.h"
#include "Allocators/AllocatorBlockInfo.h"
#include "DebugUtils.h"
#include "HeapZone/HeapZone.h"
#include "HeapZone/HeapZoneInfo.h"
#include "IntegerTypes.h"
#include "Operations/MemoryOperations.h"
#include "TestConstants.h"

/**
 * Tests for HeapZoneBase / HeapZone.
*/

namespace nabi_allocator::tests
{
#ifdef NA_TESTS
#	define NA_FIXTURE_NAME NA_TEST_FIXTURE_NAME(HeapZoneTests)

	static uInt constexpr c_HeapZoneSize = 1u;

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

		std::deque<AllocatorBlockInfo> IterateThroughHeapZone(
			std::optional<std::function<bool(AllocatorBlockInfo const&)>> /*action*/, HeapZoneInfo const& /*heapZoneInfo*/) const override
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
		HeapZone<MockAllocator> heapZone{ HeapZoneBase::c_NoParent, c_HeapZoneSize, "TestHeapZone" };
		HeapZoneInfo const& heapZoneInfo = heapZone.GetZoneInfo();

		{
			uPtr const size = memory_operations::GetMemorySize(heapZoneInfo.m_Start, heapZoneInfo.m_End);
			EXPECT_EQ(c_HeapZoneSize, size);
		}
		
		heapZone.Deinit();

		{
			uPtr const size = memory_operations::GetMemorySize(heapZoneInfo.m_Start, heapZoneInfo.m_End);
			EXPECT_EQ(0u, size);
		}
	}

	TEST(NA_FIXTURE_NAME, CheckAllocatorCalls)
	{
		HeapZone<MockAllocator> heapZone{ HeapZoneBase::c_NoParent, c_HeapZoneSize, "TestHeapZone" };
		MockAllocator const& mockAllocator = heapZone.GetAllocator();

		void* ptr = heapZone.Allocate(1u);
		EXPECT_EQ(1u, mockAllocator.GetAllocationCount());

		heapZone.Free(ptr);
		EXPECT_EQ(0u, mockAllocator.GetAllocationCount());
	}

#	undef NA_FIXTURE_NAME
#endif // ifdef NA_TESTS
} // namespace nabi_allocator::tests
