#pragma once

// Nabi Headers
#include "MacroUtils.h"

/**
* These constants are used to ensure that all test fixtures follow a naming convention so they can be filtered.
* See main.cpp -> ::testing::GTEST_FLAG(filter) = filterPattern.c_str();
*/

#define NA_TEST_IDENTIFIER t_
#define NA_BENCHMARK_IDENTIFIER b_

#define NA_TEST_FIXTURE_NAME(testFixtureName) NA_CONCAT(NA_EXPAND(NA_TEST_IDENTIFIER), testFixtureName)
#define NA_BENCHMARK_FIXTURE_NAME(benchmarkFixtureName) NA_CONCAT(NA_EXPAND(NA_BENCHMARK_IDENTIFIER), benchmarkFixtureName)

#define NA_GTEST_FILTER_SEPERATOR "*:"
