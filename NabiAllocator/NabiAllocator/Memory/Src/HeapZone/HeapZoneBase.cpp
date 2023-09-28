// cpp's Header
#include "HeapZone/HeapZoneBase.h"

// Nabi Headers
#include "DebugUtils.h"
#include "Operations/MemoryOperations.h"
#include "TypeUtils.h"

namespace nabi_allocator
{
	HeapZoneInfo& HeapZoneBase::Init(HeapZoneBase* const parentZone, uInt const numBytes, std::string const& debugName)
	{
		NA_ASSERT(!IsInitialized(), "Heap zone is already initialized");

		if (parentZone)
		{
			m_ZoneInfo.m_Start = NA_TO_UPTR(parentZone->Allocate(numBytes));
		}
		else
		{
			m_ZoneInfo.m_Start = memory_operations::RequestMemoryFromOS<uPtr>(numBytes);
		}
		m_ZoneInfo.m_End = m_ZoneInfo.m_Start + numBytes;

		m_ParentZone = parentZone;
#ifdef NA_DEBUG
		m_DebugName = debugName;
#endif // ifdef NA_DEBUG

		return m_ZoneInfo;
	}

	void HeapZoneBase::Deinit()
	{
		NA_ASSERT(IsInitialized(), "Heap zone is not initialized");

		void* zoneStart = NA_TO_VPTR(m_ZoneInfo.m_Start);
		memory_operations::ReleaseMemoryToOS(zoneStart);

		m_ZoneInfo.m_Start = c_NulluPtr;
		m_ZoneInfo.m_End = c_NulluPtr;
	}
} // namespace nabi_allocator
