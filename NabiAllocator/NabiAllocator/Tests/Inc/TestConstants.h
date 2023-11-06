#pragma once

// Nabi Headers
#include "MacroUtils.h"

/**
* These constants are used to ensure that all test fixtures follow a naming convention so they can be filtered.
* See main.cpp -> ::testing::GTEST_FLAG(filter) = filterPattern.c_str();
*/

#define NA_BENCHMARK_IDENTIFIER b_
#define NA_TEST_IDENTIFIER t_
#define NA_WORKFLOW_IDENTIFIER w_

#define NA_BENCHMARK_FIXTURE_NAME(benchmarkFixtureName) NA_MAKE_FIXTURE_NAME(NA_BENCHMARK_IDENTIFIER, benchmarkFixtureName)
#define NA_TEST_FIXTURE_NAME(testFixtureName) NA_MAKE_FIXTURE_NAME(NA_TEST_IDENTIFIER, testFixtureName)
#define NA_WORKFLOW_FIXTURE_NAME(workflowFixtureName) NA_MAKE_FIXTURE_NAME(NA_WORKFLOW_IDENTIFIER, workflowFixtureName)

#define NA_GTEST_FILTER_SEPERATOR "*:"

#define NA_MAKE_FIXTURE_NAME(identifier, fixtureName) NA_CONCAT(NA_EXPAND(identifier), fixtureName)
#define NA_MAKE_GTEST_FILTER(identifier) NA_CONCAT(NA_XSTR(identifier), NA_GTEST_FILTER_SEPERATOR)
