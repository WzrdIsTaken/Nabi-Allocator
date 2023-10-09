// cpp's Header
#include "Allocators\BlockInfo.h"

// Nabi Headers
#include "Allocators\BlockInfoIndex.h"
#include "Operations\BitOperations.h"
#include "TypeUtils.h"

namespace nabi_allocator
{
	// I was considering having one Load/UnloadBlockInfo function, templating it and using if constexpr (is_same_as_v<BlockHeader, T>)
	// to check if we should populate the memory tag field. But this just felt... wierd...
	// Perhaps I should have two LoadBlockInfo functions as well..?

	namespace
	{
		void GetBlockInfo(uInt& blockInfo, BlockInfoContent const& blockInfoContent) noexcept
		{
			blockInfo = bit_operations::LeftShiftBit(blockInfo, type_utils::ToUnderlying(BlockInfoIndex::SizeStart));
			blockInfo = bit_operations::SetBit(blockInfo, type_utils::ToUnderlying(BlockInfoIndex::Padded), blockInfoContent.m_Padded);
			blockInfo = bit_operations::SetBit(blockInfo, type_utils::ToUnderlying(BlockInfoIndex::Allocated), blockInfoContent.m_Allocated);
		}

		void SetBlockInfo(uInt const& blockInfo, BlockInfoContent& blockInfoContent) noexcept
		{
			blockInfoContent.m_Allocated = bit_operations::GetBit(blockInfo, type_utils::ToUnderlying(BlockInfoIndex::Allocated));
			blockInfoContent.m_Padded = bit_operations::GetBit(blockInfo, type_utils::ToUnderlying(BlockInfoIndex::Padded));
			blockInfoContent.m_NumBytes = bit_operations::RightShiftBit(blockInfo, type_utils::ToUnderlying(BlockInfoIndex::SizeStart));
		}
	} // anonymous namespace

	void LoadBlockInfo(BlockInfoContent const& blockInfoContent, BlockHeader& blockHeader, BlockFooter* const blockFooter) noexcept
	{
		uInt blockInfo = blockInfoContent.m_NumBytes;
		GetBlockInfo(blockInfo, blockInfoContent);

		blockHeader.m_BlockInfo = blockInfo;
#ifdef NA_MEMORY_TAGGING
		blockHeader.m_MemoryTag = blockInfoContent.m_MemoryTag;
#endif // ifdef NA_MEMORY_TAGGING

		if (blockFooter)
		{
			blockFooter->m_BlockInfo = blockInfo;
		}
	}

	void UnloadBlockInfo(BlockInfoContent& blockInfoContent, BlockHeader const& blockHeader) noexcept
	{
		SetBlockInfo(blockHeader.m_BlockInfo, blockInfoContent);

#ifdef NA_MEMORY_TAGGING
		blockInfoContent.m_MemoryTag = blockHeader.m_MemoryTag;
#endif // ifdef NA_MEMORY_TAGGING
	}

	void UnloadBlockInfo(BlockInfoContent& blockInfoContent, BlockFooter const& blockFooter) noexcept
	{
		SetBlockInfo(blockFooter.m_BlockInfo, blockInfoContent);
	}
} // namespace nabi_allocator
