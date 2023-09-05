#pragma once

/**
 * These defines can be used to toggle on/off featues of NabiAllocator.
*/

#define NABI_ALLOCATOR_DEBUG // Enables asserts, logging, etc
#define NABI_ALLOCATOR_TESTS // Runs the test suite

#define NABI_ALLOCATOR_TRACK_ALLOCATIONS // Memory allocators will track the count and total size of active and all time allocated objects 
#define NABI_ALLOCATOR_MEMORY_TAGGING // Adds metadata to all blocks. Note: this increases the size of each blocks by 4 bytes
