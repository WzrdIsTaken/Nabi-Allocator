#pragma once

// Nabi Headers
#include "MacroUtils.h"

/**
* These constants are used to ensure that all test fixtures follow a naming convention so they can be filtered.
* See main.cpp -> ::testing::GTEST_FLAG(filter) = filterPattern.c_str();
*/

#define NABI_ALLOCATOR_TEST_IDENTIFIER t_
#define NABI_ALLOCATOR_BENCHMARK_IDENTIFIER b_

#define NABI_ALLOCATOR_TEST_FIXTURE_NAME(testFixtureName) NABI_ALLOCATOR_CONCAT(NABI_ALLOCATOR_EXPAND(NABI_ALLOCATOR_TEST_IDENTIFIER), testFixtureName)
#define NABI_ALLOCATOR_BENCHMARK_FIXTURE_NAME(benchmarkFixtureName) NABI_ALLOCATOR_CONCAT(NABI_ALLOCATOR_EXPAND(NABI_ALLOCATOR_BENCHMARK_IDENTIFIER), benchmarkFixtureName)

#define NABI_ALLOCATOR_GTEST_FILTER_SEPERATOR "*:"
