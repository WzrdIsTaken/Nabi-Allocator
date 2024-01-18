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

	inline AllocatorBase::~AllocatorBase()
	{
#ifdef NA_TRACK_ALLOCATIONS
		NA_ASSERT(m_AllocatorStats.m_ActiveAllocationCount == 0u && m_AllocatorStats.m_ActiveBytesAllocated == 0u,
			"An allocator's destructor was called, but objects it allocated are still active! Attempting to free those objects now may cause a crash.");
#endif // ifdef NA_TRACK_ALLOCATIONS
	}

#ifdef NA_TRACK_ALLOCATIONS
	inline AllocatorStats const& AllocatorBase::GetStats() const noexcept
	{
		return m_AllocatorStats;
	}
#endif // ifdef NA_TRACK_ALLOCATIONS
} // namespace nabi_allocator
