// inl's Header
#include "HeapZone\HeapZoneScope.h"

// Nabi Headers
#include "MemoryCommand.h"

namespace nabi_allocator
{
	inline HeapZoneScope::HeapZoneScope(HeapZoneBase* const heapZone, MemoryTag const* const memoryTag)
		: m_HeapZone(heapZone)
#ifdef NA_MEMORY_TAGGING
		, m_MemoryTag(memoryTag)
#endif // ifdef NA_MEMORY_TAGGING
	{
		MemoryCommand::Instance()->PushHeapZoneScope(*this);
	}

	inline HeapZoneScope::~HeapZoneScope()
	{
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
