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
	struct HeapZoneInfo;
} // namespace nabi_allocator
namespace nabi_allocator::free_list_allocator
{
	struct BlockHeader;
	struct FreeListNode;
} // namespace nabi_allocator::free_list_allocator

namespace nabi_allocator::free_list_allocator
{
	template<FreeListAllocatorSettings Settings>
	class FreeListAllocator final : public AllocatorBase
	{
	public:
		FreeListAllocator(HeapZoneInfo const& heapZoneInfo);
		~FreeListAllocator();

		[[nodiscard]] void* Allocate(uInt const numBytes, HeapZoneInfo const& heapZoneInfo) override;
		void Free(void* memory, HeapZoneInfo const& heapZoneInfo) override;

		virtual std::deque<AllocatorBlockInfo> IterateThroughHeapZone(
			std::optional<std::function<bool(AllocatorBlockInfo const&)>> action, HeapZoneInfo const& heapZoneInfo) const override;

	private:
		NABI_ALLOCATOR_SET_COPY_MOVE_CONSTRUCTORS(FreeListAllocator, delete);

		[[nodiscard]] BlockHeader* const TryFindFreeBlock(uInt const numBytes) const;
		void TryCoalesceBlock(BlockHeader* const blockHeader, HeapZoneInfo const& heapZoneInfo);
		
		void AddFreeListNode(FreeListNode* const node);
		void AddFreeBlock(void* const blockStartPtr, uInt const numBytes);
		void RemoveFreeBlock(void* const blockStartPtr);

		FreeListNode* m_FreeList;
	};
} // namespace nabi_allocator::free_list_allocator

// Include Inline
#include "Allocators\FreeListAllocator\FreeListAllocator.inl"
