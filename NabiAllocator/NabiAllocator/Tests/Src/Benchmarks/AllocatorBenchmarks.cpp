// STD Headers
#include <cstdlib>
#include <vector>

// Library Headers
#include "gtest\gtest.h"

// Config Headers
#include "Config.h"

// Nabi Headers
#include "Allocators\AllocatorBase.h"
#include "Allocators\FreeListAllocator\FreeListAllocator.h"
#include "Allocators\FreeListAllocator\FreeListAllocatorSettings.h"
#include "Allocators\FreeListAllocator\SearchAlgorithms.h"
#include "BenchmarkUtils.h"
#include "DebugUtils.h"
#include "HeapZone\HeapZone.h"
#include "HeapZone\HeapZoneBase.h"
#include "IntegerTypes.h"
#include "MemoryConstants.h"
#include "TestConstants.h"

/**
 * Benchmarks for all the allocators. 
 * Running these tests can be toggled on/off by the NABI_ALLOCATOR_BENCHMARKS define in Config.h.
*/

namespace nabi_allocator::tests
{
#ifdef NABI_ALLOCATOR_BENCHMARKS
#	define TEST_BENCHMARK_NAME NABI_ALLOCATOR_BENCHMARK_FIXTURE_NAME(AllocatorBenchmarks)

	bool constexpr c_PrintBenchmarkResults = true;
	uInt constexpr c_HeapZoneSize = 20_MB;

	class UntilWeWriteAnUnmangedAllocator final : public HeapZoneBase // TODO write an unmanged allocator / heap 'zone' and replace this..!
	{
	public:
		[[nodiscard]] inline void* Allocate(uInt const numBytes) override
		{
			return std::malloc(numBytes);
		}

		inline void Free(void* const memory) override
		{
			std::free(memory);
		}
	};

	template<is_heap_zone HeapZoneType>
	void AllocThenFree(HeapZoneType& heapZone)
	{
		// 10k 4 byte allocations + 10k frees

		using namespace benchmark_utils;

		u32 constexpr allocationCount = 10'000;
		uInt constexpr allocationSize = 4u;

		std::vector<void*> allocations;
		allocations.reserve(allocationCount);

		BenchmarkResults const results = RunBenchmark(
			[&]() -> void
			{
				for (u32 i = 0; i < allocationCount; ++i)
				{
					void* ptr = heapZone.Allocate(allocationSize);
					allocations.push_back(ptr);
				}

				for (u32 i = 0; i < allocationCount; ++i)
				{
					heapZone.Free(allocations.at(i));
				}
			},
			[&]() -> void
			{
				allocations.clear();
			});

		
		BenchmarkResultsToString(results, c_PrintBenchmarkResults);
	}

	template<is_heap_zone HeapZoneType>
	void VaryingSizeAllocThenFree(HeapZoneType& heapZone)
	{
		// 10k 16 bytes allocations + 1k 256 bytes allocations + 50 2Mb allocations/frees

		using namespace benchmark_utils;

		u32 constexpr allocationCount16Byte = 10'000;
		uInt constexpr allocationSize16Byte = 16u;

		u32 constexpr allocationCount256Byte = 1000;
		uInt constexpr allocationSize256Byte = 256u;

		u32 constexpr allocationCount2Mb = 50;
		uInt constexpr allocationSize2Mb = 2_MB;

		std::vector<void*> allocations;
		allocations.reserve(allocationCount16Byte + allocationCount256Byte + allocationCount2Mb);

		BenchmarkResults const results = RunBenchmark(
			[&]() -> void
			{
				// 10k 16 bytes allocations
				for (u32 i = 0; i < allocationCount16Byte; ++i)
				{
					void* ptr = heapZone.Allocate(allocationSize16Byte);
					allocations.push_back(ptr);
				}


				// 1k 256 bytes allocations
				for (u32 i = 0; i < allocationCount256Byte; ++i)
				{
					void* ptr = heapZone.Allocate(allocationSize256Byte);
					allocations.push_back(ptr);
				}

				// 50 2Mb allocations/deallocations
				for (u32 i = 0; i < allocationCount2Mb; ++i)
				{
					void* ptr = heapZone.Allocate(allocationSize2Mb);
					heapZone.Free(ptr);
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

	// --- Unmanaged Allocator ---

	BENCHMARK(TEST_BENCHMARK_NAME, UnmanagedAllocatorAllocThenFree)
	{
		UntilWeWriteAnUnmangedAllocator test = {};
		AllocThenFree(test);
	}

	BENCHMARK(TEST_BENCHMARK_NAME, UnmanagedAllocatorVaryingSizeAllocThenFree)
	{
		UntilWeWriteAnUnmangedAllocator test = {};
		VaryingSizeAllocThenFree(test);
	}

	// --- FreeListAllocator ---

	free_list_allocator::FreeListAllocatorSettings constexpr c_FreeListAllocatorSettings =
	{
		.m_SearchAlgorithm = free_list_allocator::SearchAlgorithm::FirstFit,
		.m_BestFitLeniency = 0u
	};

	BENCHMARK(TEST_BENCHMARK_NAME, FreeListAllocatorAllocThenFree)
	{
		HeapZone<free_list_allocator::FreeListAllocator<c_FreeListAllocatorSettings>> heapZone = { c_HeapZoneSize, "TestHeapZone" };
		AllocThenFree(heapZone);
	}

	BENCHMARK(TEST_BENCHMARK_NAME, FreeListAllocatorVaryingSizeAllocThenFree)
	{
		HeapZone<free_list_allocator::FreeListAllocator<c_FreeListAllocatorSettings>> heapZone = { c_HeapZoneSize, "TestHeapZone" };
		VaryingSizeAllocThenFree(heapZone);
	}

#	undef TEST_BENCHMARK_NAME
#endif // ifdef NABI_ALLOCATOR_BENCHMARKS
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
