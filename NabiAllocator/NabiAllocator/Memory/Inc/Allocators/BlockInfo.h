#pragma once

// Config Headers
#include "Config.h"

// Nabi Headers
#include "IntegerTypes.h"
#include "MemoryConstants.h"
#include "TypeUtils.h"

/**
* All memory blocks start with a header and end with a footer. These structures hold key infomation about a block,
* such as its size, so that the free list allocator can quickly discern its characteristics.
* 
* - The infomation relevant to the allocator is duplicated in the header and footer to reduce cache misses. 
* - Its important to keep the sizes of the blocks as small as possible and allignment friendly.
*/

namespace nabi_allocator
{
	struct BlockBase abstract
	{
		uInt m_BlockInfo;
	};

	struct BlockHeader final : public BlockBase
	{
#ifdef NA_MEMORY_TAGGING
		MemoryTag m_MemoryTag;

#ifdef _M_X64
		char const c_Padding[4];
#endif // ifdef _M_X64
#endif // ifdef NA_MEMORY_TAGGING
	};

	struct BlockFooter final : public BlockBase
	{
	};

	struct BlockInfoContent final
	{
		bool m_Allocated;
		bool m_Padded;
		uInt m_NumBytes;

#ifdef NA_MEMORY_TAGGING
		MemoryTag m_MemoryTag;
#endif // ifdef NA_MEMORY_TAGGING
	};

	uInt constexpr c_BlockHeaderSize = sizeof(BlockHeader);
	uInt constexpr c_BlockFooterSize = sizeof(BlockFooter);

	static_assert(c_BlockHeaderSize ==
#ifdef _M_X64
#	ifdef NA_MEMORY_TAGGING
		16u
#	else
		8u
#	endif // ifdef NA_MEMORY_TAGGING 
#elif _M_IX86
#	ifdef NA_MEMORY_TAGGING
		8u
#	else
		4u
#	endif // ifdef NA_MEMORY_TAGGING 
#else
#	error "Unsupported architecture"
#endif // ifdef _M_IX86, elif _M_IX86
		, NA_NAMEOF_LITERAL(BlockHeader) " is not the expected size");
	static_assert(c_BlockFooterSize == 
#ifdef _M_X64
		8u
#elif _M_IX86
		4u
#else
#	error "Unsupported architecture"
#endif // ifdef _M_IX86, elif _M_IX86
		, NA_NAMEOF_LITERAL(BlockFooter) " is not the expected size");

	void LoadBlockInfo(BlockInfoContent const& blockInfoContent, BlockHeader& blockHeader, BlockFooter* const blockFooter = nullptr) noexcept;
	void UnloadBlockInfo(BlockInfoContent& blockInfoContent, BlockHeader const& blockHeader) noexcept;
	void UnloadBlockInfo(BlockInfoContent& blockInfoContent, BlockFooter const& blockFooter) noexcept;
} // namespace nabi_allocator
