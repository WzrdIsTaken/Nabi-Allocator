// inl's Header
#include "Pointers\DataPointer.h"

// Nabi Headers
#include "Operations\MemoryOperations.h"

namespace nabi_allocator
{
	template<typename T>
	inline DataPointer<T>::DataPointer(T const* const ptr)
		: m_Ptr(NA_TO_UPTR(ptr))
		, m_Data(static_cast<uInt>(0))
	{
	}

	template<typename T>
	inline DataPointer<T>::DataPointer(T const* const ptr, uInt const data)
		: m_Ptr(NA_TO_UPTR(ptr))
		, m_Data(data)
	{
	}

	template<typename T>
	inline DataPointer<T>::operator T*() const
	{
		return reinterpret_cast<T*>(m_Ptr);
	}

	template<typename T>
	inline T* DataPointer<T>::operator->() const
	{
		return reinterpret_cast<T*>(m_Ptr);
	}
} // namespace nabi_allocator
