#pragma once

/**
 * These defines can be used to toggle on/off featues of NabiAllocator.
*/

// Development / Usage
#define NA_OVERRIDE_NEW_DELETE // Routes all new/delete calls through NabiAllocator's MemoryCommand
#define NA_DEBUG // Enables asserts, logging, etc

// Heap Zone
#define NA_THREAD_SAFE_HEAP_ZONE // Adds a mutex/lock in HeapZone::Allocate and Free (default std::malloc/std::free are already thread safe)

// Allocator
#define NA_TRACK_ALLOCATIONS // Memory allocators will track the count and total size of active and all time allocated objects 
#define NA_MEMORY_TAGGING // Adds metadata to all blocks. Note: this increases the size of each blocks by sizeof(nabi_allocator::memoryTag (in MemoryConstants.h))

// Tests
#define NA_TESTS // Run unit tests
#define NA_BENCHMARKS // Run benchmark tests
#define NA_WORKFLOWS // Run workflow tests

#define NA_BENCHMARK_RUN_COUNT 3u // If NA_BENCHMARKS is defined, how many times to run each benchmark
