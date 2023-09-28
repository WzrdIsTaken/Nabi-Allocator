#pragma once

// STD Headers
#include <chrono>
#include <string>

// Library Headers
#include "gtest\gtest.h"

// Config Headers
#include "Config.h"

// Nabi Headers
#include "FloatingPointTypes.h"
#include "IntegerTypes.h"
#include "TemplateConcepts.h"

/**
 * Everything you need to benchmark code!
 * There is also a google benchmark library I think - so we could perhaps use that? 
*/

namespace nabi_allocator::benchmark_utils
{
#ifdef NABI_ALLOCATOR_BENCHMARKS
#	define /*NABI_ALLOCATOR_?*/BENCHMARK(benchmarkFixtureName, benchmarkCaseName) TEST(benchmarkFixtureName, benchmarkCaseName)

	static_assert(NABI_ALLOCATOR_BENCHMARK_RUN_COUNT > 0u);

	class StopWatch final
	{
	public:
		using Clock = std::chrono::system_clock;
		using TimePoint = std::chrono::time_point<Clock>;
		using Resolution = std::chrono::duration<f64>;

		inline StopWatch() noexcept;

		inline f64 GetElapsedTime() const noexcept;
		inline void Reset() noexcept;

	private:
		TimePoint m_Start;
	};

	struct BenchmarkResults final
	{
		f64 m_FastestTime;
		f64 m_SlowestTime;
		f64 m_AverageTime;
		u32 m_Repetitions;
	};

	template<is_function BenchmarkFunc, is_function ResetFunc>
	[[nodiscard]] BenchmarkResults RunBenchmark(BenchmarkFunc benchmarkFunc, ResetFunc resetFunc, u32 const repetitions = NABI_ALLOCATOR_BENCHMARK_RUN_COUNT);
	std::string BenchmarkResultsToString(BenchmarkResults const& results, bool const print);
#endif // #ifdef NABI_ALLOCATOR_BENCHMARK
} // namespace nabi_allocator::benchmark_utils

// Include Inline
#include "Utils\Inl\BenchmarkUtils.inl"