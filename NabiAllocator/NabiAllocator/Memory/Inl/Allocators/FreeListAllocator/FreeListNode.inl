// inl's Header
#include "Allocators\FreeListAllocator\FreeListNode.h"

// Nabi Headers
#include "DebugUtils.h"

namespace nabi_allocator::free_list_allocator
{
	inline void ResetFreeListNode(FreeListNode* const& node)
	{
		NA_ASSERT(node, NA_NAMEOF_LITERAL(node) " is null");

		node->m_Next = nullptr;
		node->m_Previous = nullptr;
	}
} // namespace nabi_allocator::free_list_allocator
