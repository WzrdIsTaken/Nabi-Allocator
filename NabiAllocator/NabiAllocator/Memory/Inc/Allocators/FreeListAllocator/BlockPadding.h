#pragma once

// Nabi Headers
#include "IntegerTypes.h"
#include "TypeUtils.h"

/**
* If the a memory block isn't a multiple of 8, then this struct is used to pad out the size.
* 
* If a block is padded, then the minimum padding is 1. Therefore, in this space we can store this BlockPadding struct
* which contains a u8 'for free'. The maximum padding of a block in isolation 7. However, if memory block is too large
* for the required number of bytes for an allocation but too small to be its own block (size < FreeListAllocator::c_MinBlockSize)
* then then the padding will be originalBlockSize - requiredBlockSize.
*/

namespace nabi_allocator::free_list_allocator
{
	struct BlockPadding final
	{
		u8 m_Padding;
	};
	
	uInt constexpr c_BlockPaddingSize = sizeof(BlockPadding);

	static_assert(c_BlockPaddingSize == 1u, NA_NAMEOF_LITERAL(BlockPadding) "is not the expected size");
} // namespace nabi_allocator::free_list_allocator
