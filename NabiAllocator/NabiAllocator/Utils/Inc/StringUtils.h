#pragma once

// STD Headers
#include <sstream>

/**
* Some helper stuff for string manipulation and the like :p.
*/

#define NA_MAKE_STRING_FROM_STREAM(stringAssignment, content) \
	std::ostringstream stringStream = {}; \
	stringStream << content; \
	stringAssignment = stringStream.str();
