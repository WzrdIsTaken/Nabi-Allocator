#pragma once

// STD Headers
#include <deque>
#include <functional>
#include <optional>

// Config Headers
#include "Config.h"

// Nabi Headers
#include "Allocators\AllocatorBase.h"
#include "IntegerTypes.h"
#include "StackAllocatorSettings.h"
#include "TypeUtils.h"

/**
 * A stack allocator works, as you might guess, exactly like a stack. It has the advantages of being faster and 
 * having lower memory overhead compaired to something like a free list allocator as we don't have to store as 
 * much infomation about a block or manage free memory. However, it is much less flexible as memory can only be
 * allocated and freed from the top of the stack.
 * 
 * The memory layout of a block is as follows:
 * - Payload -> Padding -> BlockPadding Struct -> BlockHeader Struct
 * 
 * Notes:
 *	- m_PreviousPosition is not the last value m_CurrentPosition was set to, but the previous block's position.
 *    It is used to make sure that freed memory always comes from the top of the stack.
*/

namespace nabi_allocator
{
	struct AllocationInfo;
	struct AllocatorBlockInfo;
	struct HeapZoneInfo;
} // namespace nabi_allocator

namespace nabi_allocator
{
	template<StackAllocatorSettings Settings>
	class StackAllocator;
	using DefaultStackAllocator = StackAllocator<c_StackAllocatorDefaultSettings>;

	template<StackAllocatorSettings Settings>
	class StackAllocator final : public AllocatorBase
	{
	public:
		explicit StackAllocator(HeapZoneInfo const& heapZoneInfo);
		~StackAllocator() override;

		[[nodiscard]] void* Allocate(AllocationInfo const& allocationInfo, HeapZoneInfo const& heapZoneInfo) override;
		void Free(void* memory, HeapZoneInfo const& heapZoneInfo) override;
		void Reset(HeapZoneInfo const& heapZoneInfo) override;

		std::deque<AllocatorBlockInfo> IterateThroughHeapZone(
			std::optional<std::function<bool(AllocatorBlockInfo const&)>> const action, HeapZoneInfo const& heapZoneInfo) const override;

	private:
		NA_SET_COPY_MOVE_CONSTRUCTORS(StackAllocator, delete);

		void InitMemory(HeapZoneInfo const& heapZoneInfo);

		uPtr m_CurrentPosition;
#ifdef NA_DEBUG
		uPtr m_PreviousPosition;
#endif // ifdef NA_DEBUG
	};
} // namespace nabi_allocator

// Include Include
#include "Memory\Inl\Allocators\StackAllocator\StackAllocator.inl"
