// Inl's Header
#include "Blueprints\AllocatorDefaultBenchmarks.h"

// STD Headers
#include <vector>

// Nabi Headers
#include "BenchmarkUtils.h"
#include "DebugUtils.h"

namespace nabi_allocator::tests::blueprints
{
#ifdef NA_TESTS
	template<is_heap_zone HeapZoneType>
	void AllocatorAllocThenFree(HeapZoneType& heapZone, bool const printBenchmarkResults)
	{
		// 10k 4 byte allocations + 10k frees

		using namespace benchmark_utils;

		uInt constexpr allocationCount = 10'000;
		uInt constexpr allocationSize = 4u;

		std::vector<void*> allocations;
		allocations.reserve(allocationCount);

		BenchmarkResults const results = RunBenchmark(
			[&]() -> void
			{
				for (uInt i = 0u; i < allocationCount; ++i)
				{
					void* ptr = heapZone.Allocate(allocationSize);
					allocations.push_back(ptr);
				}

				for (uInt i = allocationCount; i > 0u; --i)
				{
					heapZone.Free(allocations.at(i - 1u));
				}
			},
			[&]() -> void
			{
				allocations.clear();
			});


		BenchmarkResultsToString(results, printBenchmarkResults);
	}

	template<is_heap_zone HeapZoneType>
	void AllocatorVaryingSizeAllocThenFree(HeapZoneType& heapZone, bool const printBenchmarkResults)
	{
		// 10k 16 bytes allocations + 1k 256 bytes allocations + 50 2Mb allocations/frees

		using namespace benchmark_utils;

		uInt constexpr allocationCount16Byte = 10'000;
		uInt constexpr allocationSize16Byte = 16u;

		uInt constexpr allocationCount256Byte = 1000;
		uInt constexpr allocationSize256Byte = 256u;

		uInt constexpr allocationCount2Mb = 50;
		uInt constexpr allocationSize2Mb = 2_MB;

		std::vector<void*> allocations;
		allocations.reserve(allocationCount16Byte + allocationCount256Byte);

		BenchmarkResults const results = RunBenchmark(
			[&]() -> void
			{
				// 10k 16 bytes allocations
				for (uInt i = 0; i < allocationCount16Byte; ++i)
				{
					void* ptr = heapZone.Allocate(allocationSize16Byte);
					allocations.push_back(ptr);
				}

				// 1k 256 bytes allocations
				for (uInt i = 0; i < allocationCount256Byte; ++i)
				{
					void* ptr = heapZone.Allocate(allocationSize256Byte);
					allocations.push_back(ptr);
				}

				// 50 2Mb allocations/deallocations
				for (uInt i = 0; i < allocationCount2Mb; ++i)
				{
					void* ptr = heapZone.Allocate(allocationSize2Mb);
					heapZone.Free(ptr);
				}
			},
			[&]() -> void
			{
				heapZone.Reset();
				allocations.clear();
			});


		BenchmarkResultsToString(results, printBenchmarkResults);
	}
#endif // ifdef NA_TESTS
} // namespace nabi_allocator::tests::blueprints
