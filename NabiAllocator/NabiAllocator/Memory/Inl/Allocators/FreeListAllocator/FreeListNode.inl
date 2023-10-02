// inl's Header
#include "Allocators\FreeListAllocator\FreeListNode.h"

// Nabi Headers
#include "DebugUtils.h"

namespace nabi_allocator::free_list_allocator
{
	inline void ResetFreeListNode(FreeListNode* const& node)
	{
		NA_ASSERT(node, NA_NAMEOF_LITERAL(node) " is null");

#ifdef NA_FREE_LIST_NODE_PTRS
		node->m_Next = nullptr;
		node->m_Previous = nullptr;
#else
		node->m_Next = static_cast<u32>(NULL);
		node->m_Previous = static_cast<u32>(NULL);
#endif // ifdef NA_FREE_LIST_NODE_PTRS
	}
} // namespace nabi_allocator::free_list_allocator
