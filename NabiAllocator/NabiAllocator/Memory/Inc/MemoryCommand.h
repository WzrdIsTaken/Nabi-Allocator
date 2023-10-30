#pragma once

// STD Headers
#include <stack>

// Nabi Headers
#include "TypeUtils.h"

/**
 * A global singleton which manages what heap zone to use.
*/

namespace nabi_allocator
{
	class HeapZoneScope;
} // namespace nabi_allocator

namespace nabi_allocator
{
	class MemoryCommand final
	{
	public:
		NA_IMPLEMENT_SINGLETON(MemoryCommand)

		MemoryCommand();
		~MemoryCommand();

		void PushHeapZoneScope(HeapZoneScope const& heapZoneScope);
		void PopHeapZoneScope(HeapZoneScope const& heapZoneScope);

		[[nodiscard]] HeapZoneScope const* const GetTopHeapZoneScope() const noexcept;
		void Reset();

	private:
		NA_SET_COPY_MOVE_CONSTRUCTORS(MemoryCommand, delete);

		std::stack<std::reference_wrapper<HeapZoneScope const>> m_HeapZoneScopes;
	};
} // namespace nabi_allocator
