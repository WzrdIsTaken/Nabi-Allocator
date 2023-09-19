#pragma once

// Config Headers
#include "Config.h"

// Nabi Headers
#include "IntegerTypes.h"

/**
 * A generic struct which holds fundamental infomation about an allocator's internal block structure so helper functions only need to implement one interface.
 * Different types of allocators can populate the data in this struct in different ways.
*/

namespace nabi_allocator
{
	struct AllocatorBlockInfo final
	{
		// Meta
		uPtr m_MemoryAddress;
#ifdef NABI_ALLOCATOR_MEMORY_TAGGING
		u32 m_MemoryTag;
#endif // ifdef NABI_ALLOCATOR_MEMORY_TAGGING

		// General
		bool m_Allocated;
		bool m_Padded;

		uInt m_NumBytes;
		u32 m_Padding;
	};
} // namespace namespace nabi_allocator
