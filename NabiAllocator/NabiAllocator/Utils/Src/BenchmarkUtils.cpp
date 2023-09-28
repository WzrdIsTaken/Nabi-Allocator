// cpp's Header
#include "BenchmarkUtils.h"

// STD Headers
#include <iomanip>
#include <limits>
#include <sstream>

// Nabi Headers
#include "CharacterConstants.h"

namespace nabi_allocator::benchmark_utils
{
#ifdef NABI_ALLOCATOR_BENCHMARKS
	std::string BenchmarkResultsToString(BenchmarkResults const& results, bool const print)
	{
		using namespace character_constants;

		auto getTimeWithPrecision =
			[](f64 const time) -> std::string
			{
				int constexpr precision = std::numeric_limits<f64>::digits10;

				std::ostringstream stream = {}; // I'm not sure if there is a better way to do this...
				stream << std::fixed << std::setprecision(precision) << time;
				return stream.str();
			};

		std::ostringstream stream = {};
		stream << "Ran Benchmark " << results.m_Repetitions << " Times."          << c_NewLine <<
		       	  "Fastest Time: " << getTimeWithPrecision(results.m_FastestTime) << c_NewLine <<
			      "Slowest Time: " << getTimeWithPrecision(results.m_SlowestTime) << c_NewLine <<
			      "Average Time: " << getTimeWithPrecision(results.m_AverageTime);

		std::string const streamAsString = stream.str();
		if (print)
		{
#ifdef NABI_ALLOCATOR_DEBUG
			NABI_ALLOCATOR_LOG(streamAsString);
#else
			std::cout << streamAsString << std::endl; // If NABI_ALLOCATOR_DEBUG is not defined we can't use LOG
#endif // ifdef NABI_ALLOCATOR_DEBUG
		}

		return streamAsString;
	}
#endif // ifdef NABI_ALLOCATOR_BENCHMARKS
} // namespace nabi_allocator::benchmark_utils
