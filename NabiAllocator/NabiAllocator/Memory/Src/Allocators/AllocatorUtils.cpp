// cpp's Header
#include "Allocators\AllocatorUtils.h"

// STD Headers
#include <sstream>

// Nabi Headers
#include "Allocators\AllocatorBase.h"
#include "Allocators\AllocatorBlockInfo.h"

namespace nabi_allocator
{
	namespace
	{
		std::string ConvertAndFormatSStream(std::ostringstream const& stream)
		{
			std::string string = stream.str();
			string.pop_back(); // Remove the last space
			return string;
		}
	} // anonymous namespace

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

		return ConvertAndFormatSStream(output);
	}

#ifdef NA_MEMORY_TAGGING
	std::string GetMemoryUsage(AllocatorBase const& allocator, HeapZoneInfo const& heapZoneInfo,
		std::optional<std::function<std::string(memoryTag const)>> tagAliases)
	{
		// Format: Tag[number of bytes] [space] [next entry]
		std::ostringstream output = {};
		std::unordered_map<memoryTag, uInt> tagByteUsage = {};

		allocator.IterateThroughHeapZone(
			[&output, &tagByteUsage](AllocatorBlockInfo const& blockInfo) -> bool
			{
				auto const result = tagByteUsage.try_emplace(blockInfo.m_MemoryTag, blockInfo.m_NumBytes);
				bool const newItem = result.second;
				if (!newItem)
				{
					result.first->second += blockInfo.m_NumBytes;
				}

				bool constexpr continueLooping = true;
				return continueLooping;
			}, heapZoneInfo);

		for (auto const [key, value] : tagByteUsage)
		{
			char constexpr spaceSymbol = ' ';
			output << (tagAliases ? (*tagAliases)(key) : std::to_string(key)) << value << spaceSymbol;
		}

		return ConvertAndFormatSStream(output);
	}
#endif // ifdef NA_MEMORY_TAGGING
} // namespace nabi_allocator
