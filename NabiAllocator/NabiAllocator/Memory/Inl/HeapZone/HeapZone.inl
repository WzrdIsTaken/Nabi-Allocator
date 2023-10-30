// inl's Header
#include "HeapZone/HeapZone.h"

// Nabi Headers
#include "AllocationInfo.h"

namespace nabi_allocator
{
	template<is_allocator T>
	inline HeapZone<T>::HeapZone(HeapZoneBase* const parentZone, uInt const numBytes, std::string const& debugName)
		: HeapZoneBase{}
		, m_Allocator(Init(parentZone, numBytes, debugName))
	{
	}

	template<is_allocator T>
	inline HeapZone<T>::~HeapZone()
	{
		if (IsInitialized())
		{
			Deinit();
		}
	}

	template<is_allocator T>
	inline void* HeapZone<T>::Allocate(AllocationInfo const& allocationInfo)
	{
		return m_Allocator.Allocate(allocationInfo, m_ZoneInfo);
	}

	template<is_allocator T>
	inline void HeapZone<T>::Free(void* const memory)
	{
		// Why this function doesn't set "memory" to nullptr: https://stackoverflow.com/q/704466/8890269
		return m_Allocator.Free(memory, m_ZoneInfo);
	}

	template<is_allocator T>
	inline void HeapZone<T>::Reset()
	{
		return m_Allocator.Reset(m_ZoneInfo);
	}

	template<is_allocator T>
	inline T& HeapZone<T>::GetAllocator() noexcept
	{
		return m_Allocator;
	}
} // namespace nabi_allocator
