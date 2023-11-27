// cpp's Header
#include "Allocators\AllocatorBase.h"

// Nabi Headers
#include "Allocators\BlockInfo.h"
#include "Allocators\BlockPadding.h"
#include "Operations\MemoryOperations.h"

namespace nabi_allocator
{
	AllocatorBlockInfo AllocatorBase::IterateThroughHeapZoneHelper(
		uInt const blockHeaderPosition, std::function<s64(uInt const)> const calculateBlockPaddingAdjustment) const
	{
		AllocatorBlockInfo allocatorBlockInfo = {};
		BlockHeader const* blockHeader = nullptr;
		BlockInfoContent blockInfoContent = {};

		blockHeader = NA_REINTERPRET_MEMORY_DEFAULT(BlockHeader, blockHeaderPosition);
		UnloadBlockInfo(blockInfoContent, *blockHeader);

		// Store the block's infomation in allocatorBlockInfo#
		allocatorBlockInfo.m_MemoryAddress = NA_TO_UPTR(blockHeader);
#ifdef NA_MEMORY_TAGGING
		allocatorBlockInfo.m_MemoryTag = blockInfoContent.m_MemoryTag;
#endif // ifdef NA_MEMORY_TAGGING

		allocatorBlockInfo.m_PayloadPtr = NA_REINTERPRET_MEMORY(void, blockHeader, +, c_BlockHeaderSize);
		allocatorBlockInfo.m_Allocated = blockInfoContent.m_Allocated;
		allocatorBlockInfo.m_Padded = blockInfoContent.m_Padded;

		u32 padding = 0u;
		if (allocatorBlockInfo.m_Padded)
		{
			BlockPadding const* const blockPadding =
				NA_REINTERPRET_MEMORY(BlockPadding, blockHeader, +, calculateBlockPaddingAdjustment(blockInfoContent.m_NumBytes));
			padding = static_cast<u32>(blockPadding->m_Padding);
		}

		allocatorBlockInfo.m_NumBytes = blockInfoContent.m_NumBytes;
		allocatorBlockInfo.m_Padding = padding;

		return allocatorBlockInfo;
	}
} // namespace nabi_allocator
