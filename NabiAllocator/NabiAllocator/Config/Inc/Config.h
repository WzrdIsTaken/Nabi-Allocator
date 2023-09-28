#pragma once

/**
 * These defines can be used to toggle on/off featues of NabiAllocator.
*/

// Development
#define NABI_ALLOCATOR_DEBUG // Enables asserts, logging, etc

// Allocator
#define NABI_ALLOCATOR_TRACK_ALLOCATIONS // Memory allocators will track the count and total size of active and all time allocated objects 
#define NABI_ALLOCATOR_MEMORY_TAGGING // Adds metadata to all blocks. Note: this increases the size of each blocks by 4 bytes

// Tests
#define NABI_ALLOCATOR_TESTS // Run unit tests
//#define NABI_ALLOCATOR_BENCHMARKS // Run benchmark tests

#define NABI_ALLOCATOR_BENCHMARK_RUN_COUNT 3u // If NABI_ALLOCATOR_BENCHMARKS is defined, how many times to run each benchmark
