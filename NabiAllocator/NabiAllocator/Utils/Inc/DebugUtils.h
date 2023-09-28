#pragma once

// Config Headers
#include "Config.h"

// Nabi Headers
#include "MacroUtils.h"

/**
 * A collection of assert and logging macros to ease development and debugging.
 * Nabi has a more complex debug/logging system, but for this allocator I think these simple macros are all that is required.
 * 
 * The NA_ASSERT, NA_ASSERT_FAIL and NA_LOG macros can easily be rewired to use engine 
 * specific implementations or disabled.
*/

#ifdef NA_DEBUG
// STD Headers
#	include <crtdbg.h>
#   include <iostream>
#	include <sstream>
#	include <string>

// Nabi Headers
#	include "IntegerTypes.h"

// --- Helper Macros --- 

namespace nabi_allocator::debug_utils
{
	// This ensures that all log messages, no matter what line they are on, are in line with each other
	// Eg: main.cpp 21  [msg] (two spaces), main.cpp 123 [msg] (one space)
	u32 constexpr c_LogLocationIndent = 4u;
} // namespace nabi_allocator::debug_utils

// Location
#	define NA_FULL_LOCATION  __FILE__ << " " << __LINE__ << " "
#	define NA_SHORT_LOCATION \
		std::string(__FILE__).substr(std::string(__FILE__).find_last_of('\\') + 1u, std::string(__FILE__).size()) << " " << __LINE__ <<
#	define NA_SHORT_LOCATION_END(line) \
	NA_SPACE(nabi_allocator::debug_utils::c_LogLocationIndent - std::string(NA_STR(line)).length()) 

// Format
#	define NA_ASSERT_PREP "ASSERT"
#	define NA_LOG_PREP    NA_SHORT_LOCATION NA_SHORT_LOCATION_END(__LINE__)

#	define NA_MSG_DIVIDER ": "
#	define NA_MSG_END     std::endl

#	define NA_SPACE(numberOfSpaces) std::string(numberOfSpaces, ' ')
#	define NA_WRAP(item, wrapCharacter) wrapCharacter << item << wrapCharacter

// Macro creation
#	define NA_MAKE_MSG_STRING(stringVariable, content) \
	std::ostringstream msgStream = {}; \
	msgStream << content; \
	std::string stringVariable = msgStream.str();

// --- Assertion Macros ---

// Implementation
#   define NA_ASSERT_BASE(condition, message) \
		do \
		{ \
			if (!(condition)) { \
				NA_MAKE_MSG_STRING(assertString, \
					NA_ASSERT_PREP << NA_MSG_DIVIDER << message << NA_MSG_END) \
				\
				NA_LOG(assertString); \
				_RPTF0(_CRT_ASSERT, assertString.c_str()); \
			} \
		} \
		while (false)

// Use
#   define NA_ASSERT(condition, message) NA_ASSERT_BASE(condition, message)
#	define NA_ASSERT_DEFAULT(condition) NA_ASSERT_BASE(condition, NA_STR(condition))
#   define NA_ASSERT_FAIL(message) NA_ASSERT(false, message)

// --- Logging Macros ---

// Implementation
#	define NA_LOG_BASE(message) \
	do \
	{ \
		NA_MAKE_MSG_STRING(logString, \
			NA_LOG_PREP << message << NA_MSG_END) \
		\
		std::cout << logString; \
	} \
	while (false)

// Use
#	define NA_LOG(message) NA_LOG_BASE(message)

// --- Other Debug Macros ---

// Use
#	define NA_FUNCTION_NOT_IMPLEMENTED NA_ASSERT_FAIL("The function " << __FUNCTION__ << " is not implemented!")
#else
#	pragma warning( push ) // Push but never pop
#	pragma warning( disable : 4002 ) // Disables 'too many arguements for function-like macro' warning we get when using (void(0))

#   define NA_ASSERT(condition, message) NA_MACRO_NOT_DEFINED
#	define NA_ASSERT_DEFAULT(condition) NA_MACRO_NOT_DEFINED
#   define NA_ASSERT_FAIL(message) NA_MACRO_NOT_DEFINED
#	define NA_LOG(message) NA_MACRO_NOT_DEFINED
#	define NA_FUNCTION_NOT_IMPLEMENTED NA_MACRO_NOT_DEFINED
#endif // ifdef NA_DEBUG
