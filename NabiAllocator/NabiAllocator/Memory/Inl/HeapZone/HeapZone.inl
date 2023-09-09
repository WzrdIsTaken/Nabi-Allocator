// inl's Header
#include "HeapZone/HeapZone.h"

namespace nabi_allocator
{
	template<is_allocator T>
	inline HeapZone<T>::HeapZone(u32 const numBytes, std::string const& debugName)
		: HeapZoneBase{}
		, m_Allocator(Init(numBytes, debugName))
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
	inline void* HeapZone<T>::Allocate(u32 const numBytes)
	{
		return m_Allocator.Allocate(numBytes, m_ZoneInfo);
	}

	template<is_allocator T>
	inline void HeapZone<T>::Free(void* const memory)
	{
		// Why this function doesn't set 'memory' to nullptr: https://stackoverflow.com/q/704466/8890269
		return m_Allocator.Free(memory, m_ZoneInfo);
	}

	template<is_allocator T>
	inline T& HeapZone<T>::GetAllocator() noexcept
	{
		return m_Allocator;
	}
} // namespace nabi_allocator