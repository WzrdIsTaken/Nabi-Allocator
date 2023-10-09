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
#include "Blueprints\AllocatorDefaultTests.h"
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
#ifdef NA_TESTS
#	define NA_FIXTURE_NAME NA_TEST_FIXTURE_NAME(FreeListAllocatorTests)

	using namespace free_list_allocator;
	using HeapZoneType = HeapZone<DefaultFreeListAllocator>;

	uInt constexpr c_SmallHeapZoneSize = 64u;
	uInt constexpr c_LargeHeapZoneSize = 256u;

	std::vector<BlockInfoContent> const c_SearchAlgorithmBlocks = 
	{
		{ false, false, 64u }, { true,  false, 32u }, { false, false, 32u } // Order: m_Allocated, m_Padded, m_NumBytes
	};
	std::vector<BlockInfoContent> const c_CoalesceBlocks = 
	{
		{ false, false, 32u }, { false,  false, 32u }
	};

	void* AllocateAndSetUpMockMemory(std::vector<BlockInfoContent> const& blockInfoContent)
	{
		uInt const memoryNumBytes = (c_BlockHeaderSize + c_FreeListNodeSize + c_BlockFooterSize) * blockInfoContent.size();
		void* memory = memory_operations::RequestMemoryFromOS<void*>(memoryNumBytes);

		uPtr memoryAddress = NA_TO_UPTR(memory) + (memoryNumBytes - c_BlockFooterSize);
		FreeListNode* prevNode = nullptr;

		for (auto it = blockInfoContent.rbegin(); it != blockInfoContent.rend(); ++it) // We need to iterate backwards to set the node->m_Next ptr
		{
			BlockFooter* const footer = NA_REINTERPRET_MEMORY_DEFAULT(BlockFooter, memoryAddress);
			FreeListNode* const node = NA_REINTERPRET_MEMORY(FreeListNode, footer, -, c_FreeListNodeSize);
			BlockHeader* const header = NA_REINTERPRET_MEMORY(BlockHeader, node, -, c_BlockHeaderSize);

			LoadBlockInfo(*it, *header, footer);
			node->m_Previous = nullptr;
			node->m_Next = prevNode;
		
			memoryAddress = NA_TO_UPTR(header) - c_BlockFooterSize;
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

	TEST(NA_FIXTURE_NAME, CreateAndDestroy)
	{
		blueprints::AllocatorCreateAndDestroyTest<HeapZoneType>(
			c_SmallHeapZoneSize,  // Heap zone size
			"F64P0"               // Expected init layout
		);
	}

	TEST(NA_FIXTURE_NAME, AllocateAndFree)
	{
		blueprints::AllocatorAllocateAndFreeTest<HeapZoneType>(
			c_SmallHeapZoneSize,  // Heap zone size
			"A64P36",             // Expected x64 + memory tagging layout
			"A32P12 F32P0",       // Expected x64 layout
			"A24P8 F40P0",        // Expected x86 + memory tagging layout
			"A16P4 F48P0",        // Expected x86 layout
			"F64P0"               // Expected init layout
		);
	}

	TEST(NA_FIXTURE_NAME, Reset)
	{
		blueprints::AllocatorResetTest<HeapZoneType>(
			c_SmallHeapZoneSize,  // Heap zone size
			"F64P0"               // Expected reset layout
		);
	}

	TEST(NA_FIXTURE_NAME, BestFitSearch)
	{
		void* memory = AllocateAndSetUpMockMemory(c_SearchAlgorithmBlocks);
		FreeListNode const* const firstFreeListNode = NA_REINTERPRET_MEMORY(FreeListNode, memory, +, c_BlockHeaderSize);

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

	TEST(NA_FIXTURE_NAME, FirstFitSearch)
	{
		void* memory = AllocateAndSetUpMockMemory(c_SearchAlgorithmBlocks);
		FreeListNode const* const firstFreeListNode = NA_REINTERPRET_MEMORY(FreeListNode, memory, +, c_BlockHeaderSize);

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

	TEST(NA_FIXTURE_NAME, CoalesceBlock)
	{
		HeapZoneType heapZone{ HeapZoneBase::c_NoParent, c_LargeHeapZoneSize, "TestHeapZone" };
		auto const& allocator = heapZone.GetAllocator();
		auto const& heapZoneInfo = heapZone.GetZoneInfo();

		// Expected layout after three allocations
		void* const ptr1 = heapZone.Allocate(4u);
		void* const ptr2 = heapZone.Allocate(4u);
		void* const ptr3 = heapZone.Allocate(4u);
		{
			std::string const expectedLayout =
#ifdef _M_X64
#	ifdef NA_MEMORY_TAGGING
				"A40P12 A40P12 A40P12 F136P0"
#	else
				"A32P12 A32P12 A32P12 F160P0"
#	endif // ifdef NA_MEMORY_TAGGING 
#elif _M_IX86
#	ifdef NA_MEMORY_TAGGING
				"A24P8 A24P8 A24P8 F184P0"
#	else
				"A16P4 A16P4 A16P4 F208P0"
#	endif // ifdef NA_MEMORY_TAGGING 
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
#	ifdef NA_MEMORY_TAGGING
				"F40P0 A40P12 A40P12 F136P0"
#	else
				"F32P0 A32P12 A32P12 F160P0"
#	endif // ifdef NA_MEMORY_TAGGING 
#elif _M_IX86
#	ifdef NA_MEMORY_TAGGING
				"F24P0 A24P8 A24P8 F184P0"
#	else
				"F16P0 A16P4 A16P4 F208P0"
#	endif // ifdef NA_MEMORY_TAGGING 
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
#	ifdef NA_MEMORY_TAGGING
				"F80P0 A40P12 F136P0"
#	else
				"F64P0 A32P12 F160P0"
#	endif // ifdef NA_MEMORY_TAGGING 
#elif _M_IX86
#	ifdef NA_MEMORY_TAGGING
				"F48P0 A24P8 F184P0"
#	else
				"F32P0 A16P4 F208P0"
#	endif // ifdef NA_MEMORY_TAGGING 
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

	TEST(NA_FIXTURE_NAME, FreeListNodePtrsCorrect)
	{
		HeapZoneType heapZone{ HeapZoneBase::c_NoParent, c_LargeHeapZoneSize, "TestHeapZone" };
		auto const& heapZoneInfo = heapZone.GetZoneInfo();

		// Expected starting free list structure 
		FreeListNode const* const firstFreeListNode = NA_REINTERPRET_MEMORY_DEFAULT(FreeListNode, (heapZoneInfo.m_Start + c_BlockHeaderSize));
		EXPECT_FALSE(firstFreeListNode->m_Next);
		EXPECT_FALSE(firstFreeListNode->m_Previous);
		
		// Force a memory layout of F-A-F-A-F to create a free list
		void* const ptr1 = heapZone.Allocate(4u);
		void* const ptr2 = heapZone.Allocate(4u);
		void* const ptr3 = heapZone.Allocate(4u);
		void* const ptr4 = heapZone.Allocate(4u);

		heapZone.Free(ptr1);
		heapZone.Free(ptr3);

		FreeListNode const* const freeListNode1 = NA_REINTERPRET_MEMORY_DEFAULT(FreeListNode, ptr1); // The free list node starts where the payload was
		FreeListNode const* const freeListNode3 = NA_REINTERPRET_MEMORY_DEFAULT(FreeListNode, ptr3);

		uInt const ptr4Size = bit_operations::RightShiftBit(NA_REINTERPRET_MEMORY(BlockHeader, ptr4, -, c_BlockHeaderSize)->m_BlockInfo,
			type_utils::ToUnderlying(BlockInfoIndex::SizeStart)); // 4/4 code
		FreeListNode const* const lastFreeListNode = NA_REINTERPRET_MEMORY(FreeListNode, ptr4, +, ptr4Size);

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

#	undef NA_FIXTURE_NAME
#endif // ifdef NA_TESTS
} // namespace nabi_allocator::tests
