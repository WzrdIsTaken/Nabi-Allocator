#pragma once

// Config Headers
#include "Config.h"

// Nabi Headers
#include "IntegerTypes.h"

/**
 * Tracks the allocations / memory useage of allocators if NABI_ALLOCATOR_TRACK_ALLOCATIONS is defined.
*/

namespace nabi_allocator
{
#ifdef NABI_ALLOCATOR_TRACK_ALLOCATIONS
	struct AllocatorStats final
	{ 
		u64 m_ActiveAllocationCount;
		u64 m_ActiveBytesAllocated;

		u64 m_TotalAllocationCount;
		u64 m_TotalBytesAllocated;
	};

	enum class AllocatorStatsUpdateType : u32
	{
		Allocate,
		Free,

		ENUM_COUNT
	};

	void UpdateAllocatorStats(AllocatorStats& allocatorStats, AllocatorStatsUpdateType const updateType,
		u64 const& allocationCountAdjustment, u64 const& allocationByteAdjustment);
#endif // ifdef NABI_ALLOCATOR_TRACK_ALLOCATIONS
} // namespace nabi_allocator
