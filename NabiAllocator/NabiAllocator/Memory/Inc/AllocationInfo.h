#pragma once

// Config Headers
#include "Config.h"

// Nabi Headers
#include "IntegerTypes.h"
#include "MemoryConstants.h"

/**
 * Infomation about a specific allocation. 
 * Set in the overriden new operator, and passed down through a heap zone into an allocator.
*/

#ifdef NA_MEMORY_TAGGING
#	define NA_MAKE_ALLOCATION_INFO(numBytes, memoryTag) { numBytes, memoryTag }
#else
#	define NA_MAKE_ALLOCATION_INFO(numBytes, memoryTag) { numBytes }
#endif // ifdef NA_MEMORY_TAGGING

namespace nabi_allocator
{
	struct AllocationInfo final
	{
		uInt m_NumBytes;
#ifdef NA_MEMORY_TAGGING
		memoryTag m_MemoryTag;
#endif // ifdef NA_MEMORY_TAGGING
	};
} // namespace nabi_allocator
