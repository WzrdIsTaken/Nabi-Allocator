#pragma once

// STD Headers
#include <deque>
#include <functional>
#include <optional>

// Nabi Headers
#include "Allocators\AllocatorBase.h"
#include "FreeListAllocatorSettings.h"
#include "IntegerTypes.h"
#include "TypeUtils.h"

/**
* A free list allocator manages a pool of memory by dividing it up into blocks based on allocation requests.
* - It maintains a linked list of all the free blocks to quickly find chunks of memory.
* - Adjacent free blocks are coalesced to form larger blocks.
* - Free blocks are found using the set search algorithm. 
* 
* The memory layout of a block is as follows:
* - BlockHeader Struct -> Payload -> Padding -> BlockPadding Struct -> BlockFooter Struct
* 
* Notes:
* - This video was particularly helpful for understanding how a free list allocator works: https://www.youtube.com/watch?v=UTii4dyhR5c 
* - Normally I pass things which cannot be null as references. However in this allocator I pass things as pointers and then assert
*   if they are null. This is because in these cases we are often manipulating and reinterpret casting memory, so a dereferenced
*   pointer would just have to do referenced again anyway.
*/

namespace nabi_allocator
{
	struct AllocatorBlockInfo;
	struct AllocationInfo;
	struct BlockHeader;
	struct FreeListNode;
	struct HeapZoneInfo;
} // namespace nabi_allocator

namespace nabi_allocator
{
	template<FreeListAllocatorSettings Settings>
	class FreeListAllocator;
	using DefaultFreeListAllocator = FreeListAllocator<c_FreeListAllocatorDefaultSettings>;

	template<FreeListAllocatorSettings Settings>
	class FreeListAllocator final : public AllocatorBase
	{
	public:
		explicit FreeListAllocator(HeapZoneInfo const& heapZoneInfo);
		~FreeListAllocator() override;

		[[nodiscard]] void* Allocate(AllocationInfo const& allocationInfo, HeapZoneInfo const& heapZoneInfo) override;
		void Free(void* memory, HeapZoneInfo const& heapZoneInfo) override;
		void Reset(HeapZoneInfo const& heapZoneInfo) override;

		std::deque<AllocatorBlockInfo> IterateThroughHeapZone(
			std::optional<std::function<bool(AllocatorBlockInfo const&)>> action, HeapZoneInfo const& heapZoneInfo) const override;

	private:
		NA_SET_COPY_MOVE_CONSTRUCTORS(FreeListAllocator, delete);

		void InitMemory(HeapZoneInfo const& heapZoneInfo);

		[[nodiscard]] BlockHeader* const TryFindFreeBlock(uInt const numBytes) const;
		void TryCoalesceBlock(BlockHeader* const blockHeader, HeapZoneInfo const& heapZoneInfo);
		
		void AddFreeBlock(void* const blockStartPtr, uInt const numBytes);
		void RemoveFreeBlock(void* const blockStartPtr);

		FreeListNode* m_FreeList;
	};
} // namespace nabi_allocator

// Include Inline
#include "Memory\Inl\Allocators\FreeListAllocator\FreeListAllocator.inl"
