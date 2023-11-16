// inl's Header
#include "Blueprints\AllocatorDefaultTests.h"

// STD Headers
#include <unordered_map>

// Library Headers
#include "gtest\gtest.h"

// Nabi Headers
#include "AllocationInfo.h"
#include "Allocators\AllocatorBlockInfo.h"
#include "Allocators\AllocatorUtils.h"
#include "DebugUtils.h"
#include "HeapZone\HeapZone.h"
#include "MemoryConstants.h"
#include "TypeUtils.h"

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

#	ifdef NA_MEMORY_TAGGING
		template<is_heap_zone HeapZoneType>
		void AllocatorMemoryTagTest(uInt const heapZoneSize,
			std::string const& expectedX64MemoryTaggingUsage, std::string const& expectedX64Usage,
			std::string const& expectedX86MemoryTaggingUsage, std::string const& expectedX86Usage,
			std::string const& expectedFreeUsage)
		{
			enum class MemoryTag : memoryTag
			{
				One = 1u << 1u,
				Two = 1u << 2u,
				All = ~0u,

				ENUM_COUNT = 3u
			};

			static auto const tagToString =
				[](memoryTag const tag) -> std::string
				{
					std::unordered_map<memoryTag, std::string> const tagToStringMap =
					{
						{ type_utils::ToUnderlying(MemoryTag::One), "One"},
						{ type_utils::ToUnderlying(MemoryTag::Two), "Two"},
						{ c_NullMemoryTag, "Free" }
					};

					std::string tagName = "";

					if (tagToStringMap.contains(tag))
					{
						tagName = tagToStringMap.at(tag);
					}
					else
					{
						NA_ASSERT_FAIL("Tag \"" << tag << "\" is not in the " NA_NAMEOF_LITERAL(tagToStringMap) " umap");
					}

					return tagName;
				};

			HeapZoneType heapZone{ HeapZoneBase::c_NoParent, heapZoneSize, "TestHeapZone" };
			auto const& allocator = heapZone.GetAllocator();
			auto const& heapZoneInfo = heapZone.GetZoneInfo();

			void* const ptr1 = heapZone.Allocate(NA_MAKE_ALLOCATION_INFO(4u, type_utils::ToUnderlying(MemoryTag::One)));
			void* const ptr2 = heapZone.Allocate(NA_MAKE_ALLOCATION_INFO(4u, type_utils::ToUnderlying(MemoryTag::Two)));
			{
				std::string const expectedLayout =
#		ifdef _M_X64
#			ifdef NA_MEMORY_TAGGING
					expectedX64MemoryTaggingUsage
#			else
					expectedX64Usage
#			endif // ifdef NA_MEMORY_TAGGING 
#		elif _M_IX86
#			ifdef NA_MEMORY_TAGGING
					expectedX86MemoryTaggingUsage
#			else
					expectedX86Usage
#			endif // ifdef NA_MEMORY_TAGGING 
#		else
#			error "Unsupported architecture"
#		endif // ifdef _M_IX86, elif _M_IX86
					;

				std::string const actualUsage = GetFullMemoryUsage(allocator, heapZoneInfo, tagToString);
				EXPECT_EQ(expectedLayout, actualUsage);
			}

			heapZone.Free(ptr2); // Free ptr2 first to comply with the StackAllocator
			heapZone.Free(ptr1);
			{
				std::string const actualUsage = GetFullMemoryUsage(allocator, heapZoneInfo, tagToString);
				EXPECT_EQ(expectedFreeUsage, actualUsage);
			}
		}
#	endif // ifdef NA_MEMORY_TAGGING

#	ifdef NA_TRACK_ALLOCATIONS
		template<is_heap_zone HeapZoneType>
		void AllocatorAllocationTrackingTest(uInt const heapZoneSize, 
			uInt const expectedX64MemoryTaggingAllocationSize, uInt const expectedX64AllocationSize,
			uInt const expectedX86MemoryTaggingAllocationSize, uInt const expectedX86AllocationSize)
		{
			HeapZoneType heapZone{ HeapZoneBase::c_NoParent, heapZoneSize, "TestHeapZone" };
			auto const& allocatorStats = heapZone.GetAllocator().GetStats();

			auto verifyAllocatorStats =
				[&allocatorStats](u64 activeAllocations, u64 activeBytes, u64 totalAllocations, u64 totalBytes) -> bool
				{
					bool result = true;
					result &= allocatorStats.m_ActiveAllocationCount == activeAllocations;
					result &= allocatorStats.m_ActiveBytesAllocated == activeBytes;
					result &= allocatorStats.m_TotalAllocationCount == totalAllocations;
					result &= allocatorStats.m_TotalBytesAllocated == totalBytes;
					return result;
				};
			EXPECT_TRUE(verifyAllocatorStats(0ull, 0ull, 0ull, 0ull));

			u64 const expectedAllocationSize = static_cast<u64>(
#		ifdef _M_X64
#			ifdef NA_MEMORY_TAGGING
					expectedX64MemoryTaggingAllocationSize
#			else
					expectedX64AllocationSize
#			endif // ifdef NA_MEMORY_TAGGING 
#		elif _M_IX86
#			ifdef NA_MEMORY_TAGGING
					expectedX86MemoryTaggingAllocationSize
#			else
					expectedX86AllocationSize
#			endif // ifdef NA_MEMORY_TAGGING 
#		else
#			error "Unsupported architecture"
#		endif // ifdef _M_IX86, elif _M_IX86
				);

			void* const ptr = heapZone.Allocate(NA_MAKE_ALLOCATION_INFO(4u, c_NullMemoryTag));
			EXPECT_TRUE(verifyAllocatorStats(1ull, expectedAllocationSize, 1ull, expectedAllocationSize));

			heapZone.Free(ptr);
			EXPECT_TRUE(verifyAllocatorStats(0ull, 0ull, 1ull, expectedAllocationSize));
		}
#	endif // ifdef NA_TRACK_ALLOCATIONS
#endif // ifdef NA_TESTS
} // namespace nabi_allocator::tests::blueprints
