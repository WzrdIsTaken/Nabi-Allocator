// cpp's Header
#include "Allocators\FreeListAllocator\SearchAlgorithms.h"

// STD Headers
#include <limits>

// Nabi Headers
#include "Allocators\BlockInfo.h"
#include "Allocators\BlockInfoIndex.h"
#include "Allocators\FreeListAllocator\FreeListNode.h"
#include "DebugUtils.h"
#include "Operations\BitOperations.h"
#include "Operations\MemoryOperations.h"
#include "TypeUtils.h"

namespace nabi_allocator
{
	// The code in these FindVia functions is repeated a bit, but I think this is ok as pulling it out into 
	// another function (or macro) would reduce readability and possibly performance.

    BlockHeader* const FindViaBestFit(FreeListNode const* freeListNode, uInt const numBytes, uInt const leniency)
    {
		// Iterate through the free list to try and find a block with a size as close as possible to numBytes + leniency

		BlockHeader* bestBlock = nullptr;
		uInt bestBlockSize = std::numeric_limits<uInt>::max();

		while (freeListNode)
		{
			BlockHeader* const freeBlockHeader = NA_REINTERPRET_MEMORY(BlockHeader, freeListNode, -, c_BlockHeaderSize);
			uInt const freeBlockNumBytes = bit_operations::RightShiftBit(freeBlockHeader->m_BlockInfo, type_utils::ToUnderlying(BlockInfoIndex::SizeStart));

			if (freeBlockNumBytes >= (numBytes - leniency) && freeBlockNumBytes <= (numBytes + leniency))
			{
				bestBlock = freeBlockHeader;
				break;
			}
			else
			{
				if (freeBlockNumBytes >= numBytes && freeBlockNumBytes < bestBlockSize)
				{
					bestBlock = freeBlockHeader;
					bestBlockSize = freeBlockNumBytes;
				}

				freeListNode = freeListNode->m_Next;
			}
		}

		return bestBlock;
    }

	BlockHeader* const FindViaFirstFit(FreeListNode const* freeListNode, uInt const numBytes)
	{
		// Iterate through the free list until we find a block with size greater than or equal to numBytes

		while (freeListNode)
		{
			BlockHeader* const freeBlockHeader = NA_REINTERPRET_MEMORY(BlockHeader, freeListNode, -, c_BlockHeaderSize);
			uInt const freeBlockNumBytes = bit_operations::RightShiftBit(freeBlockHeader->m_BlockInfo, type_utils::ToUnderlying(BlockInfoIndex::SizeStart));

			if (numBytes <= freeBlockNumBytes)
			{
				return freeBlockHeader;
			}
			else
			{
				freeListNode = freeListNode->m_Next;
			}
		}

		return nullptr;
	}

	BlockHeader* const FindViaNextFit(FreeListNode const* /*freeListNode*/, uInt const /*numBytes*/)
	{
		NA_FUNCTION_NOT_IMPLEMENTED;
		return nullptr;
	}
} // namespace nabi_allocator
