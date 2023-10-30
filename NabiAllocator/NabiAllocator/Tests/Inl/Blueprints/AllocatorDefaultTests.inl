// inl's Header
#include "Blueprints\AllocatorDefaultTests.h"

// Library Headers
#include "gtest\gtest.h"

// Nabi Headers
#include "AllocationInfo.h"
#include "Allocators\AllocatorBlockInfo.h"
#include "Allocators\AllocatorUtils.h"
#include "HeapZone\HeapZone.h"
#include "MemoryConstants.h"

namespace nabi_allocator::tests::blueprints
{
#ifdef NA_TESTS
	template<is_heap_zone HeapZoneType>
	void AllocatorCreateAndDestroyTest(uInt const heapZoneSize, std::string const& expectedInitLayout)
	{
		HeapZoneType heapZone{ HeapZoneBase::c_NoParent, heapZoneSize, "TestHeapZone" };

		std::string const actualLayout = GetMemoryLayout(heapZone.GetAllocator(), heapZone.GetZoneInfo());
		EXPECT_EQ(expectedInitLayout, actualLayout);
	}

	template<is_heap_zone HeapZoneType>
	void AllocatorAllocateAndFreeTest(uInt const heapZoneSize, 
		std::string const& expectedX64MemoryTaggingLayout, std::string const& expectedX64Layout, 
		std::string const& expectedX86MemoryTaggingLayout, std::string const& expectedX86Layout,
		std::string const& expectedFreeLayout)
	{
		HeapZoneType heapZone{ HeapZoneBase::c_NoParent, heapZoneSize, "TestHeapZone" };
		auto const& allocator = heapZone.GetAllocator();
		auto const& heapZoneInfo = heapZone.GetZoneInfo();

		void* const ptr = heapZone.Allocate(NA_MAKE_ALLOCATION_INFO(4u, c_NullMemoryTag));
		{
			std::string const expectedLayout =
#ifdef _M_X64
#	ifdef NA_MEMORY_TAGGING
				expectedX64MemoryTaggingLayout
#	else
				expectedX64Layout
#	endif // ifdef NA_MEMORY_TAGGING 
#elif _M_IX86
#	ifdef NA_MEMORY_TAGGING
				expectedX86MemoryTaggingLayout
#	else
				expectedX86Layout
#	endif // ifdef NA_MEMORY_TAGGING 
#else
#	error "Unsupported architecture"
#endif // ifdef _M_IX86, elif _M_IX86
				;
			std::string const actualLayout = GetMemoryLayout(allocator, heapZoneInfo);
			EXPECT_EQ(expectedLayout, actualLayout);
		}

		heapZone.Free(ptr);
		{
			std::string const actualLayout = GetMemoryLayout(allocator, heapZoneInfo);
			EXPECT_EQ(expectedFreeLayout, actualLayout);
		}
	}

	template<is_heap_zone HeapZoneType>
	void AllocatorResetTest(uInt const heapZoneSize, std::string const& expectedResetLayout)
	{
		HeapZoneType heapZone{ HeapZoneBase::c_NoParent, heapZoneSize, "TestHeapZone" };
		void* const ptr = heapZone.Allocate(NA_MAKE_ALLOCATION_INFO(4u, c_NullMemoryTag));
		heapZone.Reset();

		std::string const actualLayout = GetMemoryLayout(heapZone.GetAllocator(), heapZone.GetZoneInfo());
		EXPECT_EQ(expectedResetLayout, actualLayout);
	}
#endif 
} // namespace nabi_allocator::tests::blueprints
