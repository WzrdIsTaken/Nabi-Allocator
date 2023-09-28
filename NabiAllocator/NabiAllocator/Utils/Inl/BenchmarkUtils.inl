// inl's Header
#include "BenchmarkUtils.h"

// STD Headers
#include <algorithm>
#include <numeric>
#include <vector>

// Nabi Headers
#include "DebugUtils.h"

namespace nabi_allocator::benchmark_utils
{
#ifdef NABI_ALLOCATOR_BENCHMARKS
	// --- StopWatch ---

	inline StopWatch::StopWatch() noexcept
		: m_Start{}
	{
		Reset();
	}

	inline f64 StopWatch::GetElapsedTime() const noexcept
	{
		auto const now = Clock::now();
		return std::chrono::duration_cast<Resolution>(now - m_Start).count();
	}

	inline void StopWatch::Reset() noexcept
	{
		m_Start = Clock::now();
	}

	// --- Free Functions --- 

	template<is_function BenchmarkFunc, is_function ResetFunc>
	BenchmarkResults RunBenchmark(BenchmarkFunc benchmarkFunc, ResetFunc resetFunc, u32 const repetitions)
	{
		NABI_ALLOCATOR_ASSERT_DEFAULT(repetitions > 0u);

		// Setup
		StopWatch stopWatch = {};
		std::vector<f64> times;
		times.reserve(repetitions);

		// Run
		for (u32 i = 0; i < repetitions; ++i)
		{
			stopWatch.Reset();
			benchmarkFunc();

			f64 const elapsedTime = stopWatch.GetElapsedTime();
			times.push_back(elapsedTime);

			resetFunc();
		}

		// Results
		std::sort(times.begin(), times.end());

		BenchmarkResults results = {};
		results.m_FastestTime = times.front();
		results.m_SlowestTime = times.back();
		results.m_AverageTime = times.empty() ? 0.0 :
			std::accumulate(times.begin(), times.end(), 0.0) / static_cast<uInt>(repetitions);
		results.m_Repetitions = repetitions;

		return results;
	}
#endif // ifdef NABI_ALLOCATOR_BENCHMARKS
} // namespace nabi_allocator::benchmark_utils
