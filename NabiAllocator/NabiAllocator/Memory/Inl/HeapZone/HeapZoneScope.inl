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
#if defined NA_DEBUG || defined NA_TESTS || defined NA_WORKFLOWS
		, m_CustomMemoryCommand(customMemoryCommand)
#endif // ifdef NA_DEBUG || NA_TESTS || NA_WORKFLOWS
	{
#if defined NA_DEBUG || defined NA_TESTS || defined NA_WORKFLOWS
		if (m_CustomMemoryCommand)
		{
			m_CustomMemoryCommand->PushHeapZoneScope(*this);
			return;
		}
#endif // ifdef NA_DEBUG || NA_TESTS || NA_WORKFLOWS

		MemoryCommand::Instance()->PushHeapZoneScope(*this);
	}

	inline HeapZoneScope::~HeapZoneScope()
	{
#if defined NA_DEBUG || defined NA_TESTS || defined NA_WORKFLOWS
		if (m_CustomMemoryCommand)
		{
			m_CustomMemoryCommand->PopHeapZoneScope(*this);
			return;
		}
#endif // ifdef NA_DEBUG || NA_TESTS || NA_WORKFLOWS

		MemoryCommand::Instance()->PopHeapZoneScope(*this);
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
