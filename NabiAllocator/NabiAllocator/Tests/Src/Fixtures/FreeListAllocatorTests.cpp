// STD Headers
#include <optional>
#include <string>
#include <utility>
#include <vector>

// Library Headers
#include "gtest\gtest.h"

// Config Headers
#include "Config.h"

// Nabi Headers
#include "Allocators\AllocatorUtils.h"
#include "Allocators\FreeListAllocator\FreeListAllocator.h"
#include "Allocators\FreeListAllocator\FreeListAllocatorSettings.h"
#include "Allocators\FreeListAllocator\SearchAlgorithms.h"
#include "HeapZone\HeapZone.h"
#include "HeapZone\HeapZoneInfo.h"
#include "Operations\BitOperations.h"
#include "Operations\MemoryOperations.h"
#include "TestConstants.h"
#include "TypeUtils.h"

/**
 * Tests for FreeListAllocator.
 * 
 * Note: I am unsure if my tests for the search algorithms are ok. On the one hand, they test the smallest part of the code.
 * On the other, they don't test the interface of the allocator itself. Thoughts?
*/

namespace nabi_allocator::tests
{
#ifdef NABI_ALLOCATOR_TESTS
#	define TEST_FIXTURE_NAME NABI_ALLOCATOR_TEST_FIXTURE_NAME(FreeListAllocatorTests)

	using namespace free_list_allocator;

	uInt constexpr c_SmallTestHeapZoneSize = 64u;
	uInt constexpr c_LargeTestHeapZoneSize = 256u;

	std::vector<BlockInfoContent> const c_TestSearchAlgorithmBlocks = 
	{
		{ false, false, 64u }, { true,  false, 32u }, { false, false, 32u } // Order: m_Allocated, m_Padded, m_NumBytes
	};
	std::vector<BlockInfoContent> const c_TestCoalesceBlocks = 
	{
		{ false, false, 32u }, { false,  false, 32u }
	};

	void* AllocateAndSetUpMockMemory(std::vector<BlockInfoContent> const& blockInfoContent)
	{
		uInt const memoryNumBytes = (c_BlockHeaderSize + c_FreeListNodeSize + c_BlockFooterSize) * blockInfoContent.size();
		void* memory = memory_operations::RequestMemoryFromOS<void*>(memoryNumBytes);

		uPtr memoryAddress = NABI_ALLOCATOR_TO_UPTR(memory) + (memoryNumBytes - c_BlockFooterSize);
		FreeListNode* prevNode = nullptr;

		for (auto it = blockInfoContent.rbegin(); it != blockInfoContent.rend(); ++it) // We need to iterate backwards to set the node->m_Next ptr
		{
			BlockFooter* const footer = NABI_ALLOCATOR_REINTERPRET_MEMORY(BlockFooter, nullptr, +, memoryAddress);
			FreeListNode* const node = NABI_ALLOCATOR_REINTERPRET_MEMORY(FreeListNode, footer, -, c_FreeListNodeSize);
			BlockHeader* const header = NABI_ALLOCATOR_REINTERPRET_MEMORY(BlockHeader, node, -, c_BlockHeaderSize);

			LoadBlockInfo(*it, *header, footer);
			node->m_Previous = nullptr;
			node->m_Next = prevNode;
		
			memoryAddress = NABI_ALLOCATOR_TO_UPTR(header) - c_BlockFooterSize;
			prevNode = node;
		}

		return memory;
	}

	template<typename SearchFunction, typename... Args>
	std::optional<uInt> TryFindBlock(SearchFunction const searchFunction, FreeListNode const* const firstFreeListNode, Args&&... args)
	{
		std::optional<uInt> result = std::nullopt;

		BlockHeader const* const foundBlockHeader = searchFunction(firstFreeListNode, std::forward<Args>(args)...);
		if (foundBlockHeader)
		{
			result = bit_operations::RightShiftBit(foundBlockHeader->m_BlockInfo, type_utils::ToUnderlying(BlockInfoIndex::SizeStart));
		}

		return result;
	}

	TEST(TEST_FIXTURE_NAME, CreateAndDestroy)
	{
		HeapZone<FreeListAllocator<c_FreeListAllocatorDefaultSettings>> heapZone(c_SmallTestHeapZoneSize, "TestHeapZone");

		std::string const expectedLayout = "F64P0";
		std::string const actualLayout = GetMemoryLayout(heapZone.GetAllocator(), heapZone.GetZoneInfo());
		EXPECT_EQ(expectedLayout, actualLayout);
	}

	TEST(TEST_FIXTURE_NAME, AllocateAndFree)
	{
		HeapZone<FreeListAllocator<c_FreeListAllocatorDefaultSettings>> heapZone(c_SmallTestHeapZoneSize, "TestHeapZone");
		auto const& allocator = heapZone.GetAllocator();
		auto const& heapZoneInfo = heapZone.GetZoneInfo();

		void* const ptr = heapZone.Allocate(4u);
		{
			std::string const expectedLayout =
#ifdef _M_X64
#	ifdef NABI_ALLOCATOR_MEMORY_TAGGING
				"A64P36"
#	else
				"A32P12 F32P0"
#	endif // ifdef NABI_ALLOCATOR_MEMORY_TAGGING 
#elif _M_IX86
#	ifdef NABI_ALLOCATOR_MEMORY_TAGGING
				"A24P8 F40P0"
#	else
				"A16P4 F48P0"
#	endif // ifdef NABI_ALLOCATOR_MEMORY_TAGGING 
#else
#	error "Unsupported architecture"
#endif // ifdef _M_IX86, elif _M_IX86
				;
			std::string const actualLayout = GetMemoryLayout(allocator, heapZoneInfo);
			EXPECT_EQ(expectedLayout, actualLayout);
		}

		heapZone.Free(ptr);
		{
			std::string const expectedLayout = "F64P0";
			std::string const actualLayout = GetMemoryLayout(allocator, heapZoneInfo);
			EXPECT_EQ(expectedLayout, actualLayout);
		}
	}

	TEST(TEST_FIXTURE_NAME, BestFitSearch)
	{
		void* memory = AllocateAndSetUpMockMemory(c_TestSearchAlgorithmBlocks);
		FreeListNode const* const firstFreeListNode = NABI_ALLOCATOR_REINTERPRET_MEMORY(FreeListNode, memory, +, c_BlockHeaderSize);

		{
			uInt const foundBlockHeaderSize = TryFindBlock(&FindViaBestFit, firstFreeListNode, 32u, 0u).value();
			EXPECT_EQ(32u, foundBlockHeaderSize);
		}

		{
			uInt const foundBlockHeaderSize = TryFindBlock(&FindViaBestFit, firstFreeListNode, 32u, 32u).value();
			EXPECT_EQ(64u, foundBlockHeaderSize);
		}

		{
			std::optional<uInt> const foundBlockHeaderSize = TryFindBlock(&FindViaBestFit,firstFreeListNode, 65u, 0u);
			EXPECT_FALSE(foundBlockHeaderSize);
		}

		memory_operations::ReleaseMemoryToOS(memory);
	}

	TEST(TEST_FIXTURE_NAME, FirstFitSearch)
	{
		void* memory = AllocateAndSetUpMockMemory(c_TestSearchAlgorithmBlocks);
		FreeListNode const* const firstFreeListNode = NABI_ALLOCATOR_REINTERPRET_MEMORY(FreeListNode, memory, +, c_BlockHeaderSize);

		{
			uInt const foundBlockHeaderSize = TryFindBlock(&FindViaFirstFit, firstFreeListNode, 32u).value();
			EXPECT_EQ(64u, foundBlockHeaderSize);
		}

		{
			std::optional<uInt> const foundBlockHeaderSize = TryFindBlock(&FindViaFirstFit, firstFreeListNode, 65u);
			EXPECT_FALSE(foundBlockHeaderSize);
		}

		memory_operations::ReleaseMemoryToOS(memory);
	}

	TEST(TEST_FIXTURE_NAME, CoalesceBlock)
	{
		HeapZone<FreeListAllocator<c_FreeListAllocatorDefaultSettings>> heapZone(c_LargeTestHeapZoneSize, "TestHeapZone");
		auto const& allocator = heapZone.GetAllocator();
		auto const& heapZoneInfo = heapZone.GetZoneInfo();

		// Expected layout after three allocations
		void* const ptr1 = heapZone.Allocate(4u);
		void* const ptr2 = heapZone.Allocate(4u);
		void* const ptr3 = heapZone.Allocate(4u);
		{
			std::string const expectedLayout =
#ifdef _M_X64
#	ifdef NABI_ALLOCATOR_MEMORY_TAGGING
				"A40P12 A40P12 A40P12 F136P0"
#	else
				"A32P12 A32P12 A32P12 F160P0"
#	endif // ifdef NABI_ALLOCATOR_MEMORY_TAGGING 
#elif _M_IX86
#	ifdef NABI_ALLOCATOR_MEMORY_TAGGING
				"A24P8 A24P8 A24P8 F184P0"
#	else
				"A16P4 A16P4 A16P4 F208P0"
#	endif // ifdef NABI_ALLOCATOR_MEMORY_TAGGING 
#else
#	error "Unsupported architecture"
#endif // ifdef _M_IX86, elif _M_IX86
				;
			std::string const actualLayout = GetMemoryLayout(allocator, heapZoneInfo);
			EXPECT_EQ(expectedLayout, actualLayout);
		}

		// Expected layout after the first allocated ptr is freed
		heapZone.Free(ptr1);
		{
			std::string const expectedLayout =
#ifdef _M_X64
#	ifdef NABI_ALLOCATOR_MEMORY_TAGGING
				"F40P0 A40P12 A40P12 F136P0"
#	else
				"F32P0 A32P12 A32P12 F160P0"
#	endif // ifdef NABI_ALLOCATOR_MEMORY_TAGGING 
#elif _M_IX86
#	ifdef NABI_ALLOCATOR_MEMORY_TAGGING
				"F24P0 A24P8 A24P8 F184P0"
#	else
				"F16P0 A16P4 A16P4 F208P0"
#	endif // ifdef NABI_ALLOCATOR_MEMORY_TAGGING 
#else
#	error "Unsupported architecture"
#endif // ifdef _M_IX86, elif _M_IX86
				;
			std::string const actualLayout = GetMemoryLayout(allocator, heapZoneInfo);
			EXPECT_EQ(expectedLayout, actualLayout);
		}

		// Expected layout after the second allocated ptr is freed
		heapZone.Free(ptr2);
		{
			std::string const expectedLayout =
#ifdef _M_X64
#	ifdef NABI_ALLOCATOR_MEMORY_TAGGING
				"F80P0 A40P12 F136P0"
#	else
				"F64P0 A32P12 F160P0"
#	endif // ifdef NABI_ALLOCATOR_MEMORY_TAGGING 
#elif _M_IX86
#	ifdef NABI_ALLOCATOR_MEMORY_TAGGING
				"F48P0 A24P8 F184P0"
#	else
				"F32P0 A16P4 F208P0"
#	endif // ifdef NABI_ALLOCATOR_MEMORY_TAGGING 
#else
#	error "Unsupported architecture"
#endif // ifdef _M_IX86, elif _M_IX86
				;
			std::string const actualLayout = GetMemoryLayout(allocator, heapZoneInfo);
			EXPECT_EQ(expectedLayout, actualLayout);
		}

		// Expected layout after the third allocated ptr is freed
		heapZone.Free(ptr3);
		{
			std::string const expectedLayout = "F256P0";
			std::string const actualLayout = GetMemoryLayout(allocator, heapZoneInfo);
			EXPECT_EQ(expectedLayout, actualLayout);
		}
	}

	TEST(TEST_FIXTURE_NAME, FreeListNodePtrsCorrect)
	{
		HeapZone<FreeListAllocator<c_FreeListAllocatorDefaultSettings>> heapZone(c_LargeTestHeapZoneSize, "TestHeapZone");
		auto const& heapZoneInfo = heapZone.GetZoneInfo();

		// Expected starting free list structure 
		FreeListNode const* const firstFreeListNode = NABI_ALLOCATOR_REINTERPRET_MEMORY(FreeListNode, nullptr, +, (heapZoneInfo.m_Start + c_BlockHeaderSize));
		EXPECT_FALSE(firstFreeListNode->m_Next);
		EXPECT_FALSE(firstFreeListNode->m_Previous);
		
		// Force a memory layout of F-A-F-A-F to create a free list
		void* const ptr1 = heapZone.Allocate(4u);
		void* const ptr2 = heapZone.Allocate(4u);
		void* const ptr3 = heapZone.Allocate(4u);
		void* const ptr4 = heapZone.Allocate(4u);

		heapZone.Free(ptr1);
		heapZone.Free(ptr3);

		FreeListNode const* const freeListNode1 = NABI_ALLOCATOR_REINTERPRET_MEMORY_DEFAULT(FreeListNode, ptr1); // The free list node starts where the payload was
		FreeListNode const* const freeListNode3 = NABI_ALLOCATOR_REINTERPRET_MEMORY_DEFAULT(FreeListNode, ptr3);

		uInt const ptr4Size = bit_operations::RightShiftBit(NABI_ALLOCATOR_REINTERPRET_MEMORY(BlockHeader, ptr4, -, c_BlockHeaderSize)->m_BlockInfo,
			type_utils::ToUnderlying(BlockInfoIndex::SizeStart)); // 4/4 code
		FreeListNode const* const lastFreeListNode = NABI_ALLOCATOR_REINTERPRET_MEMORY(FreeListNode, ptr4, +, ptr4Size);

		// Check that the free list structure is expected
		EXPECT_FALSE(freeListNode1->m_Previous);
		EXPECT_EQ(freeListNode1->m_Next, freeListNode3);
		EXPECT_EQ(freeListNode3->m_Previous, freeListNode1);
		
		// Allocate into the first free block, and check that the free list updates
		void* const ptr5 = heapZone.Allocate(4u);
		EXPECT_FALSE(freeListNode3->m_Previous);	

		// Allocate into the second free block so only the last free block remains and check that the free list is one element again
		void* const ptr6 = heapZone.Allocate(4u);
		EXPECT_FALSE(lastFreeListNode->m_Previous);
		EXPECT_FALSE(lastFreeListNode->m_Next);

		// Free all the memory allocated
		heapZone.Free(ptr2);
		heapZone.Free(ptr4);
		heapZone.Free(ptr5);
		heapZone.Free(ptr6);
	}

#	undef TEST_FIXTURE_NAME
#endif // ifdef NABI_ALLOCATOR_TESTS
} // namespace nabi_allocator::tests
