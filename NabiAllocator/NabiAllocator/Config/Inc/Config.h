#pragma once

/**
 * These defines can be used to toggle on/off featues of NabiAllocator.
*/

// --- Development / Usage ---
#define NA_OVERRIDE_NEW_DELETE // Routes all new/delete calls through NabiAllocator's MemoryCommand
#define NA_DEBUG // Enables asserts, logging, etc
//#define NA_THREAD_LOCAL_HEAPS // Heap zones and memory tags will be thread local. However if memory is allocated on one thread and freed from another, it still won't crash.

#define NA_NON_STANDARD_CPP // Use non standard C++ like __forceinline
#ifdef NA_NON_STANDARD_CPP
#	define NA_FORCE_INLINE __forceinline
#else
#	define NA_FORCE_INLINE inline
#endif // ifdef NA_NON_STANDARD_CPP

//#define NA_DEFINE_SHORT_NAMESPACE // Adds the option to use a shorter namespace, rather than typing out nabi_allocator every time...
#ifdef NA_DEFINE_SHORT_NAMESPACE
	namespace nabi_allocator {};
	namespace na = nabi_allocator;
#endif // ifdef NA_DEFINE_SHORT_NAMESPACE

#define NA_ASSERT_DEFINTION(message) _RPTF0(_CRT_ASSERT, message) // NabiAllocator will call this macro to assert. Note: If you override this, make sure that the assert will always fire (eg: assert(false))
#define NA_LOG_DEFINTION(message) std::cout << message // NabiAllocator will call this macro to log

// --- Memory Command ---
#define NA_MALLOC_IF_OUT_OF_MEMORY // If an heapzone is out of memory, rather than asserting and failing to allocate just try and malloc the memory
//#define NA_SAFE_ALLOC_FREE_EARLY_OUT // If an allocation or free is going to fail (eg: freeing nullptr), then don't attempt it. Note: an assert will still fire

// --- Heap Zone --- 
#define NA_THREAD_SAFE_HEAP_ZONE // Adds a mutex/lock in HeapZone::Allocate and Free (default std::malloc/std::free are already thread safe)

// --- Allocator ---
#define NA_TRACK_ALLOCATIONS // Memory allocators will track the count and total size of active and all time allocated objects 
#define NA_MEMORY_TAGGING // Adds metadata to all blocks. Note: this increases the size of each blocks by sizeof(nabi_allocator::memoryTag (in MemoryConstants.h))

// --- Tests ---
#define NA_TESTS // Run unit tests
#define NA_BENCHMARKS // Run benchmark tests
#define NA_WORKFLOWS // Run workflow tests

#define NA_BENCHMARK_RUN_COUNT 3u // If NA_BENCHMARKS is defined, how many times to run each benchmark
