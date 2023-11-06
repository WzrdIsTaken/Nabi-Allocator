// cpp's Header
#include "MemoryCommand.h"

// Config Headers
#include "Config.h"

// Nabi Headers
#include "DebugUtils.h"
#include "HeapZone\HeapZoneScope.h"

namespace nabi_allocator
{
	MemoryCommand::MemoryCommand()
		: m_HeapZoneScopes{}
	{
	}

	MemoryCommand::~MemoryCommand()
	{
		NA_ASSERT(m_HeapZoneScopes.empty(), 
			"MemoryCommand's destructor called but there are still HeapZoneScopes on the stack");
	}

	void MemoryCommand::PushHeapZoneScope(HeapZoneScope const& heapZoneScope)
	{
		m_HeapZoneScopes.push(heapZoneScope);
	}

	void MemoryCommand::PopHeapZoneScope(HeapZoneScope const& heapZoneScope)
	{
#ifdef NA_DEBUG
		HeapZoneScope const* const topHeapZoneScope = GetTopHeapZoneScope();
		NA_ASSERT(topHeapZoneScope, "Trying to pop a heap zone scope, but the stack is empty");

		if (topHeapZoneScope)
		{
			NA_ASSERT(topHeapZoneScope->GetHeapZone() == heapZoneScope.GetHeapZone(),
				"Popping a heap zone scope which isn't at the top of the stack");
		}
#endif // ifdef NA_DEBUG

		m_HeapZoneScopes.pop();
	}

	HeapZoneScope const* const MemoryCommand::GetTopHeapZoneScope() const noexcept
	{
		if (!m_HeapZoneScopes.empty()) [[likely]]
		{
			return &m_HeapZoneScopes.top().get();
		}
		else [[unlikely]]
		{
			return nullptr;
		}
	}

	void MemoryCommand::Reset()
	{
		m_HeapZoneScopes = {};
	}
} // namespace nabi_allocator
