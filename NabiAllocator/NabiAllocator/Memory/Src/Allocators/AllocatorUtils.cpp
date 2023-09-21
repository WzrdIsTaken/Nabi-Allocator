// cpp's Header
#include "Allocators\AllocatorUtils.h"

// STD Headers
#include <sstream>

// Nabi Headers
#include "Allocators\AllocatorBase.h"
#include "Allocators\AllocatorBlockInfo.h"

namespace nabi_allocator
{
	std::string GetMemoryLayout(AllocatorBase const& allocator, HeapZoneInfo const& heapZoneInfo)
	{
		// Format: F(ree)/A(llocated)[number of bytes]P(adding)[number of bytes] [space] [next entry]
		std::ostringstream output = {};

		allocator.IterateThroughHeapZone(
			[&output](AllocatorBlockInfo const& blockInfo) -> bool
			{
				char constexpr freeSymbol      = 'F';
				char constexpr allocatedSymbol = 'A';
				char constexpr paddingSymbol   = 'P';
				char constexpr spaceSymbol     = ' ';

				output << (blockInfo.m_Allocated ? allocatedSymbol : freeSymbol);
				output << blockInfo.m_NumBytes;
				output << paddingSymbol;
				output << blockInfo.m_Padding;
				output << spaceSymbol;

				bool constexpr continueLooping = true;
				return continueLooping;
			}, heapZoneInfo);

		std::string outputString = output.str();
		outputString.pop_back(); // Remove the last space
		return outputString;
	}
} // namespace nabi_allocator
