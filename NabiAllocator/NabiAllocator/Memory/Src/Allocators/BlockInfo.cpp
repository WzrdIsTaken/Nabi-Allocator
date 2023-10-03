// cpp's Header
#include "Allocators\BlockInfo.h"

// Nabi Headers
#include "Allocators\BlockInfoIndex.h"
#include "Operations\BitOperations.h"
#include "TypeUtils.h"

namespace nabi_allocator
{
	void LoadBlockInfo(BlockInfoContent const& blockInfoContent, BlockBase& blockOne, BlockBase* const blockTwo) noexcept
	{
		uInt blockInfo = blockInfoContent.m_NumBytes;

		blockInfo = bit_operations::LeftShiftBit(blockInfo, type_utils::ToUnderlying(BlockInfoIndex::SizeStart));
		blockInfo = bit_operations::SetBit(blockInfo, type_utils::ToUnderlying(BlockInfoIndex::Padded), blockInfoContent.m_Padded);
		blockInfo = bit_operations::SetBit(blockInfo, type_utils::ToUnderlying(BlockInfoIndex::Allocated), blockInfoContent.m_Allocated);

		blockOne.m_BlockInfo = blockInfo;
		if (blockTwo)
		{
			blockTwo->m_BlockInfo = blockInfo;
		}
	}

	void UnloadBlockInfo(BlockInfoContent& blockInfoContent, BlockBase const& block) noexcept
	{
		uInt const blockInfo = block.m_BlockInfo;

		blockInfoContent.m_Allocated = bit_operations::GetBit(blockInfo, type_utils::ToUnderlying(BlockInfoIndex::Allocated));
		blockInfoContent.m_Padded = bit_operations::GetBit(blockInfo, type_utils::ToUnderlying(BlockInfoIndex::Padded));
		blockInfoContent.m_NumBytes = bit_operations::RightShiftBit(blockInfo, type_utils::ToUnderlying(BlockInfoIndex::SizeStart));
	}
} // namespace nabi_allocator
