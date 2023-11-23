// inl's Header
#include "HeapZone/HeapZoneBase.h"

// Nabi Headers
#include "Operations/MemoryOperations.h"

namespace nabi_allocator
{
	inline HeapZoneBase::HeapZoneBase()
		: m_ZoneInfo{ c_NulluPtr, c_NulluPtr }
		, m_ParentZone(nullptr)
#ifdef NA_DEBUG
		, m_DebugName("NotAssigned")
#endif // ifdef NA_DEBUG
	{
		s_AllHeapZones.push_back(this);
	}

	inline HeapZoneBase::~HeapZoneBase()
	{
		s_AllHeapZones.erase(std::remove(s_AllHeapZones.begin(), s_AllHeapZones.end(), this));
	}

	inline HeapZoneInfo const& HeapZoneBase::GetZoneInfo() const noexcept
	{
		return m_ZoneInfo;
	}

#ifdef NA_DEBUG
	inline std::string const& HeapZoneBase::GetDebugName() const noexcept
	{
		return m_DebugName;
	}
#endif // ifdef NA_DEBUG

	inline bool HeapZoneBase::IsInitialized() const noexcept
	{
		return m_ZoneInfo.m_Start && m_ZoneInfo.m_End;
	}

	inline bool HeapZoneBase::ContainsPtr(void const* const ptr) const
	{
		return memory_operations::IsPtrInRange(m_ZoneInfo.m_Start, m_ZoneInfo.m_End, NA_TO_UPTR(ptr));
	}
} // namespace nabi_allocator
