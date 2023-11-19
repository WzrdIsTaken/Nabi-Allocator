#pragma once

// STD Headers
#include <string>

/**
 * Some utility function(s...) for basic file manipulation.
*/

namespace nabi_allocator::file_utils
{
	// For quick & dirty logging just set the path to C:\Users\Ben\Desktop\Game Dev\NabiAllocator\Nabi-Allocator\logs\[FileName].txt
	bool CreateAndWriteFile(std::string const& fileName, std::string const& fileContent, bool const timeStampFile = true);
} // namespace nabi_allocator::file_utils
