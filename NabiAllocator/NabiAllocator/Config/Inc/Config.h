#pragma once

/**
 * These defines can be used to toggle on/off featues of NabiAllocator.
*/

// Development
#define NA_DEBUG // Enables asserts, logging, etc

// Allocator
#define NA_TRACK_ALLOCATIONS // Memory allocators will track the count and total size of active and all time allocated objects 
#define NA_MEMORY_TAGGING // Adds metadata to all blocks. Note: this increases the size of each blocks by 4 bytes

#define NA_FREE_LIST_NODE_PTRS // The free list node uses pointers rather than offsets [TODO - say if this is faster/it makes the min block size larger]

// Tests
#define NA_TESTS // Run unit tests
#define NA_BENCHMARKS // Run benchmark tests

#define NA_BENCHMARK_RUN_COUNT 3u // If NA_BENCHMARKS is defined, how many times to run each benchmark
