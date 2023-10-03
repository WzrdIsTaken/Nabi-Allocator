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
 * TODO desc
*/

namespace nabi_allocator
{
	struct AllocatorBlockInfo;
	struct HeapZoneInfo;
} // namespace nabi_allocator
namespace nabi_allocator::stack_allocator
{

} // namespace nabi_allocator::stack_allocator

namespace nabi_allocator::stack_allocator
{
	template<StackAllocatorSettings Settings>
	class StackAllocator final : public AllocatorBase
	{
	public:
		explicit StackAllocator(HeapZoneInfo const& heapZoneInfo);
		~StackAllocator() override;

		[[nodiscard]] void* Allocate(uInt const numBytes, HeapZoneInfo const& heapZoneInfo) override;
		void Free(void* memory, HeapZoneInfo const& heapZoneInfo) override;

		std::deque<AllocatorBlockInfo> IterateThroughHeapZone(
			std::optional<std::function<bool(AllocatorBlockInfo const&)>> action, HeapZoneInfo const& heapZoneInfo) const override;

	private:
		NA_SET_COPY_MOVE_CONSTRUCTORS(StackAllocator, delete);

		void* m_CurrentPosition;
#ifdef NA_DEBUG
		void* m_PreviousPosition;
#endif // ifdef NA_DEBUG
	};
} // namespace nabi_allocator::stack_allocator
