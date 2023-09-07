// inl's Header
#include "HeapZoneBase.h"

// Nabi Headers
#include "MemoryOperations.h"

namespace nabi_allocator
{
	inline HeapZoneBase::HeapZoneBase()
		: m_ZoneInfo{ c_NulluPtr, c_NulluPtr }
#ifdef NABI_ALLOCATOR_DEBUG
		, m_DebugName("NotAssigned")
#endif // ifdef NABI_ALLOCATOR_DEBUG
	{
	}

	inline HeapZoneInfo const& HeapZoneBase::GetZoneInfo() const noexcept
	{
		return m_ZoneInfo;
	}

#ifdef NABI_ALLOCATOR_DEBUG
	inline std::string const& HeapZoneBase::GetDebugName() const noexcept
	{
		return m_DebugName;
	}
#endif // ifdef NABI_ALLOCATOR_DEBUG

	inline bool HeapZoneBase::IsInitialized() const noexcept
	{
		return m_ZoneInfo.m_Start && m_ZoneInfo.m_End;
	}
} // namespace nabi_allocator
