#pragma once

// Nabi Headers
#include "HeapZone\HeapZoneBase.h"
#include "IntegerTypes.h"

/**
 * A collection of standard benchmarks for every allocator to compair their speeds.
*/

namespace nabi_allocator::tests::blueprints
{
#ifdef NA_BENCHMARKS
	uInt constexpr c_BenchmarkHeapZoneSize = 20_MB;

	template<is_heap_zone HeapZoneType>
	void AllocatorAllocThenFree(HeapZoneType& heapZone, bool const printBenchmarkResults);

	template<is_heap_zone HeapZoneType>
	void AllocatorVaryingSizeAllocThenFree(HeapZoneType& heapZone, bool const printBenchmarkResults);
#endif // ifdef NA_BENCHMARKS
} // namespace nabi_allocator::tests::blueprints

// Include Inline
#include "Tests\Inl\Blueprints\AllocatorDefaultBenchmarks.inl"
