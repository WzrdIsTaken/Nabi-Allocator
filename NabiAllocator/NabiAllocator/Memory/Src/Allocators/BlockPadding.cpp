// cpp's Header
#include "Allocators\BlockPadding.h"

// Nabi Headers
#include "Operations\MemoryOperations.h"

namespace nabi_allocator
{
	void AllocateBlockPadding(uPtr const currentPosition, uInt const padding)
	{
		// We want to allocate the padding one byte before the footer. 
		// Its ok to do padding - c_BlockPaddingSize, because if "requiresPadding" is true, "padding" will always be > 1 and c_BlockPaddingSize is 1.

		uInt const blockPaddingStructPosition = padding - c_BlockPaddingSize;
		BlockPadding* const blockPadding = NA_REINTERPRET_MEMORY(BlockPadding, currentPosition, +, blockPaddingStructPosition);
		blockPadding->m_Padding = static_cast<u8>(padding);
	}
} // namespace nabi_allocator
