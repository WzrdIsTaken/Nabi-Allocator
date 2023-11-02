#pragma once

// Config Headers
#include "Config.h"

// Nabi Headers
#include "IntegerTypes.h"
#include "MemoryConstants.h"

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
#ifdef NA_MEMORY_TAGGING
		memoryTag m_MemoryTag;
#endif // ifdef NA_MEMORY_TAGGING

		// General
		void* m_PayloadPtr;

		bool m_Allocated;
		bool m_Padded;

		uInt m_NumBytes;
		u32 m_Padding;
	};
} // namespace namespace nabi_allocator
