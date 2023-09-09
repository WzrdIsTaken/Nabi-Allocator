// cpp's Header
#include "HeapZone/HeapZoneBase.h"

// Nabi Headers
#include "DebugUtils.h"
#include "MemoryConstants.h"
#include "Operations/MemoryOperations.h"
#include "TypeUtils.h"

namespace nabi_allocator
{
	HeapZoneInfo& HeapZoneBase::Init(u32 const numBytes, std::string const& debugName)
	{
		NABI_ALLOCATOR_ASSERT(!IsInitialized(),
			"Heap zone is already initialized");
		NABI_ALLOCATOR_ASSERT(memory_operations::IsAlligned(numBytes, c_BlockAllignment),
			"The size of the heap zone must be exactly divisible by " << NABI_ALLOCATOR_NAMEOF(c_BlockAllignment));

		m_ZoneInfo.m_Start = memory_operations::RequestMemoryFromOS<uPtr>(numBytes);
		m_ZoneInfo.m_End = m_ZoneInfo.m_Start + static_cast<uPtr>(numBytes);

#ifdef NABI_ALLOCATOR_DEBUG
		m_DebugName = debugName;
#endif // ifdef NABI_ALLOCATOR_DEBUG

		return m_ZoneInfo;
	}

	void HeapZoneBase::Deinit()
	{
		NABI_ALLOCATOR_ASSERT(IsInitialized(), "Heap zone is not initialized");

		void* zoneStart = NABI_ALLOCATOR_TO_VPTR(m_ZoneInfo.m_Start);
		memory_operations::ReleaseMemoryToOS(zoneStart);

		m_ZoneInfo.m_Start = c_NulluPtr;
		m_ZoneInfo.m_End = c_NulluPtr;
	}
} // namespace nabi_allocator
