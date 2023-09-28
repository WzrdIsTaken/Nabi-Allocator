#pragma once

// Nabi Headers
#include "IntegerTypes.h"
#include "TypeUtils.h"

/**
 * These nodes are used to create an implicit free list for the FreeListAllocator.
 * When a block of memory is freed, using the space that was occupied by the allocated payload,
 * we can store this struct for free. We link it with the previous and next free blocks to create
 * a linked list, and so when the allocator needs to find a free block to use it only has to 
 * search the whats in the list.
*/

namespace nabi_allocator::free_list_allocator
{
	struct FreeListNode final
	{
		FreeListNode* m_Previous;
		FreeListNode* m_Next;
	};

	uInt constexpr c_FreeListNodeSize = sizeof(FreeListNode);

	static_assert(c_FreeListNodeSize == 
#ifdef _M_X64
		16u
#elif _M_IX86
		8u
#else
#	error "Unsupported architecture"
#endif // ifdef _M_IX86, elif _M_IX86
		, NA_NAMEOF_LITERAL(FreeListNode) "is not the expected size");
} // namespace nabi_allocator::free_list_allocator

