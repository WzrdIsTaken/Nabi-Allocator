// cpp's Header
#include "Allocators\FreeListAllocator\FreeListNode.h"

// Nabi Headers
#include "DebugUtils.h"
#include "Operations\MemoryOperations.h"

namespace nabi_allocator::free_list_allocator
{
	void AddFreeListNode(FreeListNode*& freelist, FreeListNode* const& node)
	{
		NA_ASSERT(node, NA_NAMEOF_LITERAL(node) " is null");

		// If there are no free blocks, this block becomes the head of the list
		if (!freelist)
		{
			freelist = node;
			return;
		}

		// If the memory of block is before the memory of the list's head, then make this block the new head to ensure cache performance
		uPtr const nodeAddress = NA_TO_UPTR(node);
		if (nodeAddress < NA_TO_UPTR(freelist))
		{
			node->m_Next = freelist;
			freelist->m_Previous = node;
			freelist = node;
			return;
		}

		// Otherwise iterate through the free blocks until we find a place where the previous node's memory address is smaller and the next node's is bigger
		FreeListNode* currentNode = freelist;

		while (currentNode->m_Next)
		{
			if (nodeAddress > NA_TO_UPTR(currentNode) &&
				nodeAddress < NA_TO_UPTR(currentNode->m_Next))
			{
				node->m_Next = currentNode->m_Next;
				currentNode->m_Next->m_Previous = node;
				node->m_Previous = currentNode;
				currentNode->m_Next = node;
				return;
			}
			else
			{
				currentNode = currentNode->m_Next;
			}
		}

		// If no place between two blocks was found, the memory location of this block must be greater than those in the list. Add it to the end
		currentNode->m_Next = node;
		node->m_Previous = currentNode;
	}

	void RemoveFreeListNode(FreeListNode*& freelist, FreeListNode* const& node)
	{
		NA_ASSERT(node, NA_NAMEOF_LITERAL(node) " is null");

		// If the free list node is the start of the free list, rewire the head of the list
		if (freelist == node)
		{
			freelist = node->m_Next;

#if defined NA_DEBUG || defined NA_TESTS
			// Not necessary, but makes the debug output easier to reason
			if (freelist)
			{
				freelist->m_Previous = nullptr;
			}
#endif // ifdef NA_DEBUG || NA_TESTS
		}
		else
		{
			// Otherwise, rewire the other nodes in the linked list
			if (node->m_Previous)
			{
				node->m_Previous->m_Next = node->m_Next;
			}
			if (node->m_Next)
			{
				node->m_Next->m_Previous = node->m_Previous;
			}
		}

#if defined NA_DEBUG || defined NA_TESTS
		// Same deal as above.
		ResetFreeListNode(node);
#endif // ifdef NA_DEBUG || NA_TESTS
	}
} // namespace nabi_allocator::free_list_allocator
