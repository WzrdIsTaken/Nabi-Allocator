// STD Headers
#include <cstdlib>

// Library Headers
#include "gtest\gtest.h"

// Config Headers
#include "Config.h"

// Nabi Headers
#include "AllocationInfo.h"
#include "Allocators\AllocatorBase.h"
#include "Allocators\FreeListAllocator\FreeListAllocator.h"
#include "Allocators\FreeListAllocator\FreeListAllocatorSettings.h"
#include "Allocators\FreeListAllocator\SearchAlgorithms.h"
#include "Allocators\StackAllocator\StackAllocator.h"
#include "Allocators\StackAllocator\StackAllocatorSettings.h"
#include "BenchmarkUtils.h"
#include "Blueprints\AllocatorDefaultBenchmarks.h"
#include "HeapZone\HeapZone.h"
#include "HeapZone\HeapZoneBase.h"
#include "IntegerTypes.h"
#include "MemoryConstants.h"
#include "TestConstants.h"

/**
 * Benchmarks for all the allocators. 
 * Running these tests can be toggled on/off by the NA_BENCHMARKS define in Config.h.
 * 
 * We could use the Default[AllocatorName]Allocator typedef in this file, but sometimes the default
 * allocator settings are not the fastest ones, and thats what we are testing here. Plus the 
 * explicitness is good I recon...
*/

namespace nabi_allocator::tests
{
#ifdef NA_BENCHMARKS
#	define NA_FIXTURE_NAME NA_BENCHMARK_FIXTURE_NAME(AllocatorBenchmarks)

	bool constexpr c_PrintBenchmarkResults = true;
	uInt constexpr c_HeapZoneSize = 20_MB;

	class UntilWeWriteAnUnmangedAllocator final : public HeapZoneBase // TODO write an unmanged allocator / heap 'zone' and replace this..!
	{
	public:
		[[nodiscard]] inline void* Allocate(AllocationInfo const& allocationInfo) override
		{
			return std::malloc(allocationInfo.m_NumBytes);
		}

		inline void Free(void* const memory) override
		{
			std::free(memory);
		}

		inline void Reset() override
		{
			// Can't use NA_FUNCTION_NOT_IMPLEMENTED because its called in the benchmarks
		}
	};

	// --- Unmanaged Allocator ---

	NA_BENCHMARK(NA_FIXTURE_NAME, UnmanagedAllocatorAllocThenFree)
	{
		UntilWeWriteAnUnmangedAllocator test = {};
		blueprints::AllocatorAllocThenFree(test, c_PrintBenchmarkResults);
	}

	NA_BENCHMARK(NA_FIXTURE_NAME, UnmanagedAllocatorVaryingSizeAllocThenFree)
	{
		UntilWeWriteAnUnmangedAllocator test = {};
		blueprints::AllocatorVaryingSizeAllocThenFree(test, c_PrintBenchmarkResults);
	}

	// --- Free List Allocator ---

	FreeListAllocatorSettings constexpr c_FreeListAllocatorBenchmarkSettings =
	{
		.m_SearchAlgorithm = SearchAlgorithm::FirstFit,
		.m_BestFitLeniency = 0u
	};
	using BenchmarkFreeListAllocator = HeapZone<FreeListAllocator<c_FreeListAllocatorBenchmarkSettings>>;

	NA_BENCHMARK(NA_FIXTURE_NAME, FreeListAllocatorAllocThenFree)
	{
		BenchmarkFreeListAllocator allocator = { HeapZoneBase::c_NoParent, c_HeapZoneSize, "TestHeapZone" };
		blueprints::AllocatorAllocThenFree(allocator, c_PrintBenchmarkResults);
	}

	NA_BENCHMARK(NA_FIXTURE_NAME, FreeListAllocatorVaryingSizeAllocThenFree)
	{
		BenchmarkFreeListAllocator allocator = { HeapZoneBase::c_NoParent, c_HeapZoneSize, "TestHeapZone" };
		blueprints::AllocatorVaryingSizeAllocThenFree(allocator, c_PrintBenchmarkResults);
	}

	// --- Stack Allocator ---

	StackAllocatorSettings constexpr c_StackAllocatorBenchmarkSettings =
	{
	};
	using BenchmarkStackAllocator = HeapZone<StackAllocator<c_StackAllocatorBenchmarkSettings>>;

	NA_BENCHMARK(NA_FIXTURE_NAME, StackAllocatorAllocThenFree)
	{
		BenchmarkStackAllocator allocator = { HeapZoneBase::c_NoParent, c_HeapZoneSize, "TestHeapZone" };
		blueprints::AllocatorAllocThenFree(allocator, c_PrintBenchmarkResults);
	}

	NA_BENCHMARK(NA_FIXTURE_NAME, StackAllocatorVaryingSizeAllocThenFree)
	{
		BenchmarkStackAllocator allocator = { HeapZoneBase::c_NoParent, c_HeapZoneSize, "TestHeapZone" };
		blueprints::AllocatorVaryingSizeAllocThenFree(allocator, c_PrintBenchmarkResults);
	}

#	undef NA_FIXTURE_NAME
#endif // ifdef NA_BENCHMARKS
} // namespace nabi_allocator::tests

/*
* Random allocations benchmark test.
* Could be useful to eg get a more realistic memory layout. But probs not too great for benchmarking as its not consistent.
* 
	template<is_heap_zone HeapZoneType>
	void RandomAllocAndFree(HeapZoneType& heapZone, u32 const allocationCount = 10'000, uInt const minAllocationSize = 4u, uInt const maxAllocationSize = 64u)
	{
		// If we use this, we need:
		// - Maths utils to seed random
		// - AND generate random number / random number in range helper func

		using namespace benchmark_utils;

		std::vector<void*> allocations;
		allocations.reserve(allocationCount);

		BenchmarkResults const results = RunBenchmark(c_BenchmarkRepetitions,
			[&]() -> void
			{
				for (u32 i = 0; i < allocationCount; ++i)
				{
					bool const allocate = rand() % 2;
					if (allocate)
					{
						uInt const allocationSize = minAllocationSize + rand() % (maxAllocationSize - minAllocationSize + 1);
						void* ptr = heapZone.Allocate(allocationSize);
						allocations.push_back(ptr);
					}
					else
					{
						if (!allocations.empty())
						{
							uInt const freeIndex = rand() % allocations.size();
							heapZone.Free(allocations.at(freeIndex));
							allocations.erase(std::next(allocations.begin(), freeIndex));
						}
					}
				}
			},
			[&]() -> void
			{
				uInt const allocationsSize = allocations.size();
				for (u32 i = 0; i < allocationsSize; ++i)
				{
					heapZone.Free(allocations.at(i));
				}

				allocations.clear();
			});


		BenchmarkResultsToString(results, c_PrintBenchmarkResults);
	}
*/
