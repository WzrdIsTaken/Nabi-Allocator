// cpp's Header
#include "Allocators\AllocatorStats.h"

// Nabi Headers
#include "DebugUtils.h"
#include "TypeUtils.h"

#define NABI_ALLOCATOR_UPDATE_ALLOCATOR_STATS(allocatorStats, operation, countAdjustment, byteAdjustment) \
	allocatorStats.m_ActiveAllocationCount += countAdjustment; \
	allocatorStats.m_ActiveBytesAllocated  += byteAdjustment; \
	allocatorStats.m_TotalAllocationCount  += countAdjustment; \
	allocatorStats.m_TotalBytesAllocated   += byteAdjustment;

namespace nabi_allocator
{
	void UpdateAllocatorStats(AllocatorStats& allocatorStats, AllocatorStatsUpdateType const updateType, 
		u64 const& allocationCountAdjustment, u64 const& allocationByteAdjustment)
	{
		switch (updateType)
		{
		case AllocatorStatsUpdateType::Allocate:
			NABI_ALLOCATOR_UPDATE_ALLOCATOR_STATS(allocatorStats, +, 
				allocationCountAdjustment, allocationByteAdjustment);
			break;
		case AllocatorStatsUpdateType::Free:
			NABI_ALLOCATOR_ASSERT(allocatorStats.m_ActiveAllocationCount >= allocationCountAdjustment &&
			                      allocatorStats.m_ActiveBytesAllocated  >= allocationByteAdjustment  &&
				                  allocatorStats.m_TotalAllocationCount  >= allocationCountAdjustment &&
				                  allocatorStats.m_TotalBytesAllocated   >= allocationByteAdjustment,
				"Free operation results in a negative allocation or byte count. What has gone wrong?");

			NABI_ALLOCATOR_UPDATE_ALLOCATOR_STATS(allocatorStats, -, 
				allocationCountAdjustment, allocationByteAdjustment);
			break;
		default:
			NABI_ALLOCATOR_ASSERT_FAIL("Using an unexpected " << NABI_ALLOCATOR_NAMEOF_LITERAL(AllocatorStatsUpdateType));
			break;
		}
	}
} // namespace nabi_allocator

#undef NABI_ALLOCATOR_UPDATE_ALLOCATOR_STATS
