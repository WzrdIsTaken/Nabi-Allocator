#pragma once

// Nabi Headers
#include "IntegerTypes.h"
#include "TypeUtils.h"

/**
* If the a memory block isn't a multiple of 8, then this struct is used to pad out the size.
* 
* If the block is padded, then the minimum padding is 1. Therefore, in this space we can store the BlockPadding struct
* 'for free' which contains a u8. The maximum padding of a block is 7.
*/

namespace nabi_allocator::free_list_allocator
{
	struct BlockPadding final
	{
		u8 m_Padding;
	};
	
	uInt constexpr c_BlockPaddingSize = sizeof(BlockPadding);

	static_assert(c_BlockPaddingSize == 1u, NABI_ALLOCATOR_NAMEOF_LITERAL(BlockPadding) "is not the expected size");
} // namespace nabi_allocator::free_list_allocator
