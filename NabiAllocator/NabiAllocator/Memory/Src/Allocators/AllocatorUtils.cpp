// cpp's Header
#include "Allocators\AllocatorUtils.h"

// STD Headers
#include <sstream>

// Nabi Headers
#include "Allocators\AllocatorBase.h"
#include "Allocators\AllocatorBlockInfo.h"
#include "CharacterConstants.h"

namespace nabi_allocator
{
	using namespace character_constants;

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
				output << (blockInfo.m_Allocated ? c_AllocatedSymbol : c_FreeSymbol);
				output << blockInfo.m_NumBytes;
				output << c_PaddingSymbol;
				output << blockInfo.m_Padding;
				output << c_SpaceSymbol;

				bool constexpr continueLooping = true;
				return continueLooping;
			}, heapZoneInfo);

		return ConvertAndFormatSStream(output);
	}

	std::string GetMemoryUsage(AllocatorBase const& allocator, HeapZoneInfo const& heapZoneInfo)
	{
		// Format: A(llocated)[number of bytes] F(ree)[number of bytes]
		std::ostringstream output = {};
		uInt allocatedBytes = 0u;
		uInt freeBytes = 0u;

		allocator.IterateThroughHeapZone(
			[&output, &allocatedBytes, &freeBytes](AllocatorBlockInfo const& blockInfo) -> bool
			{
				if (blockInfo.m_Allocated)
				{
					allocatedBytes += blockInfo.m_NumBytes;
				}
				else
				{
					freeBytes += blockInfo.m_NumBytes;
				}

				bool constexpr continueLooping = true;
				return continueLooping;
			}, heapZoneInfo);

		output << c_AllocatedSymbol << allocatedBytes << c_SpaceSymbol << c_FreeSymbol << freeBytes;
		return output.str();
	}

#ifdef NA_MEMORY_TAGGING
	std::string GetFullMemoryUsage(AllocatorBase const& allocator, HeapZoneInfo const& heapZoneInfo,
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
			output << (tagAliases ? (*tagAliases)(key) : std::to_string(key)) << value << c_SpaceSymbol;
		}

		return ConvertAndFormatSStream(output);
	}
#endif // ifdef NA_MEMORY_TAGGING
} // namespace nabi_allocator
