// inl's Header
#include "Allocators\AllocatorBase.h"

namespace nabi_allocator
{
	inline AllocatorBase::AllocatorBase()
#ifdef NA_TRACK_ALLOCATIONS
		: m_AllocatorStats{}
#endif // ifdef NA_TRACK_ALLOCATIONS
	{
	}
} // namespace nabi_allocator
