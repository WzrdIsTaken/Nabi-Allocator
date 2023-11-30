// inl's Header
#include "HeapZone\HeapZoneScope.h"

// Nabi Headers
#include "MemoryCommand.h"

namespace nabi_allocator
{
	inline HeapZoneScope::HeapZoneScope(HeapZoneBase* const heapZone, std::optional<memoryTag const> const memoryTag, MemoryCommand* const customMemoryCommand)
		: m_HeapZone(heapZone)
#ifdef NA_MEMORY_TAGGING
		, m_MemoryTag(memoryTag)
#endif // ifdef NA_MEMORY_TAGGING
		, m_CustomMemoryCommand(customMemoryCommand)
	{
		if (!m_CustomMemoryCommand)
		{
			MemoryCommand::Instance().PushHeapZoneScope(*this);
		}
		else
		{
			m_CustomMemoryCommand->PushHeapZoneScope(*this);
		}
	}

	inline HeapZoneScope::~HeapZoneScope()
	{
		if (!m_CustomMemoryCommand)
		{
			MemoryCommand::Instance().PopHeapZoneScope(*this);
		}
		else
		{
			m_CustomMemoryCommand->PopHeapZoneScope(*this);
		}
	}

	inline HeapZoneBase* const HeapZoneScope::GetHeapZone() const noexcept
	{
		return m_HeapZone;
	}

#ifdef NA_MEMORY_TAGGING
	inline std::optional<memoryTag const> const& HeapZoneScope::GetMemoryTag() const noexcept
	{
		return m_MemoryTag;
	}
#endif // ifdef NA_MEMORY_TAGGING
} // namespace nabi_allocator
