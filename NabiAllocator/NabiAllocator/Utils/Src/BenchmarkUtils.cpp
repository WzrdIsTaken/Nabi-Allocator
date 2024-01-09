// cpp's Header
#include "BenchmarkUtils.h"

// STD Headers
#include <iomanip>
#include <limits>

// Nabi Headers
#include "CharacterConstants.h"
#include "StringUtils.h"

namespace nabi_allocator::benchmark_utils
{
#ifdef NA_BENCHMARKS
	std::string BenchmarkResultsToString(BenchmarkResults const& results, bool const print)
	{
		using namespace character_constants;

		auto const getTimeWithPrecision =
			[](f64 const time) -> std::string
			{
				s32 constexpr precision = std::numeric_limits<f64>::digits10;

				std::ostringstream stream = {}; // I'm not sure if there is a better way to do this...
				stream << std::fixed << std::setprecision(precision) << time;
				return stream.str();
			};

		NA_MAKE_STRING_FROM_STREAM(std::string const resultsAsString,
			"Ran Benchmark " << results.m_Repetitions << " Times."          << c_NewLine <<
			"Fastest Time: " << getTimeWithPrecision(results.m_FastestTime) << c_NewLine <<
			"Slowest Time: " << getTimeWithPrecision(results.m_SlowestTime) << c_NewLine <<
			"Average Time: " << getTimeWithPrecision(results.m_AverageTime));

		if (print)
		{
#ifdef NA_DEBUG
			NA_LOG(NA_LOG_PREP, NA_LOG_INFO, NA_LOG_CATEGORY_TEST, resultsAsString, NA_LOG_END);
#else
			std::cout << resultsAsString << std::endl; // If NA_DEBUG is not defined we can't use LOG
#endif // ifdef NA_DEBUG
		}

		return resultsAsString;
	}
#endif // ifdef NA_BENCHMARKS
} // namespace nabi_allocator::benchmark_utils
