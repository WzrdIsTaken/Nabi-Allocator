// cpp's Header
#include "Allocators\AllocatorStats.h"

// Nabi Headers
#include "DebugUtils.h"
#include "TypeUtils.h"

namespace nabi_allocator
{
#ifdef NA_TRACK_ALLOCATIONS
	void UpdateAllocatorStats(AllocatorStats& allocatorStats, AllocatorStatsUpdateType const updateType, 
		u64 const& allocationCountAdjustment, u64 const& allocationByteAdjustment)
	{
		switch (updateType)
		{
		case AllocatorStatsUpdateType::Allocate:
			allocatorStats.m_ActiveAllocationCount += allocationCountAdjustment;
			allocatorStats.m_ActiveBytesAllocated  += allocationByteAdjustment;
			allocatorStats.m_TotalAllocationCount  += allocationCountAdjustment;
			allocatorStats.m_TotalBytesAllocated   += allocationByteAdjustment;
			break;
		case AllocatorStatsUpdateType::Free:
			NA_ASSERT(allocatorStats.m_ActiveAllocationCount >= allocationCountAdjustment &&
			                      allocatorStats.m_ActiveBytesAllocated  >= allocationByteAdjustment  &&
				                  allocatorStats.m_TotalAllocationCount  >= allocationCountAdjustment &&
				                  allocatorStats.m_TotalBytesAllocated   >= allocationByteAdjustment,
				"Free operation results in a negative allocation or byte count. What has gone wrong?");

			allocatorStats.m_ActiveAllocationCount -= allocationCountAdjustment;
			allocatorStats.m_ActiveBytesAllocated  -= allocationByteAdjustment;
			break;
		default:
			NA_ASSERT_FAIL("Using an unexpected " << NA_NAMEOF_LITERAL(AllocatorStatsUpdateType));
			break;
		}
	}
#endif // ifdef NA_TRACK_ALLOCATIONS
} // namespace nabi_allocator
