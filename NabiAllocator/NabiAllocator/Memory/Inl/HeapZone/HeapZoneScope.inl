// inl's Header
#include "HeapZone\HeapZoneScope.h"

// Nabi Headers
#include "MemoryCommand.h"

namespace nabi_allocator
{
	inline HeapZoneScope::HeapZoneScope(HeapZoneBase* const heapZone, MemoryTag const* const memoryTag, bool const registerWithMemoryCommand)
		: m_HeapZone(heapZone)
#ifdef NA_MEMORY_TAGGING
		, m_MemoryTag(memoryTag)
#endif // ifdef NA_MEMORY_TAGGING
#if defined NA_DEBUG || defined NA_TESTS
		, m_RegisterWithMemoryCommand(registerWithMemoryCommand)
#endif // ifdef NA_DEBUG || NA_TESTS
	{
#if defined NA_DEBUG || defined NA_TESTS
		if (!m_RegisterWithMemoryCommand) return;
#endif // ifdef NA_DEBUG || NA_TESTS

		MemoryCommand::Instance()->PushHeapZoneScope(*this);
	}

	inline HeapZoneScope::~HeapZoneScope()
	{
#if defined NA_DEBUG || defined NA_TESTS
		if (!m_RegisterWithMemoryCommand) return;
#endif // ifdef NA_DEBUG || NA_TESTS

		MemoryCommand::Instance()->PopHeapZoneScope(*this);
	}

	inline HeapZoneBase* const HeapZoneScope::GetHeapZone() const noexcept
	{
		return m_HeapZone;
	}

#ifdef NA_MEMORY_TAGGING
	inline MemoryTag const* const HeapZoneScope::GetMemoryTag() const noexcept
	{
		return m_MemoryTag;
	}
#endif // ifdef NA_MEMORY_TAGGING
} // namespace nabi_allocator
