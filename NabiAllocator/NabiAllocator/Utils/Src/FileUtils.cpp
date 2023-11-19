// cpp's Header
#include "FileUtils.h"

// STD Headers
#include <algorithm>
#include <ctime>
#include <fstream>

// Nabi Headers
#include "DebugUtils.h"

namespace nabi_allocator::file_utils
{
	bool CreateAndWriteFile(std::string const& fileName, std::string const& fileContent, bool const tileStampFile)
	{
		// We only need the one file related function in this program :p
		// No sense overcomplicating things...

		NA_LOG(NA_LOG_PREP, NA_LOG_INFO, NA_LOG_CATEGORY_FILE, "Creating a file, name " << NA_WRAP("\"", fileName), NA_LOG_END);

		if (tileStampFile)
		{
#pragma warning(disable : 4996) // ;_; https://tinyurl.com/7t3prpy5 
			std::time_t const time = std::time(nullptr);
			std::tm const* const localTime = std::localtime(&time);
			std::string timeAsString = std::asctime(localTime);
			std::replace(timeAsString.begin(), timeAsString.end(), ':', '.');
			const_cast<std::string&>(fileName).insert(fileName.find_last_of('.'), 
				" - " + timeAsString.substr(0u, timeAsString.length() - 1u)); // Removes a newline char (seems to be added by asctime)
#pragma warning(default: 4996)
		}

		auto file = std::ofstream{ fileName };
		file << fileContent;
		file.close();

		bool constexpr result = true;
		return result;
	}
} // namespace nabi_allocator::file_utils
