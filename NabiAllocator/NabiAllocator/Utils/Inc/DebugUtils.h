#pragma once

// Config Headers
#include "Config.h"

// Nabi Headers
#include "MacroUtils.h"

/**
 * A collection of assert and logging macros to ease development and debugging.
 * Nabi has a more complex debug/logging system, but for this allocator I think these simple macros are all that is required.
 * 
 * The NABI_ALLOCATOR_ASSERT, NABI_ALLOCATOR_ASSERT_FAIL and NABI_ALLOCATOR_LOG macros can easily be rewired to use engine 
 * specific implementations or disabled.
*/

#ifdef NABI_ALLOCATOR_DEBUG
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
#	define NABI_ALLOCATOR_FULL_LOCATION  __FILE__ << " " << __LINE__ << " "
#	define NABI_ALLOCATOR_SHORT_LOCATION \
		std::string(__FILE__).substr(std::string(__FILE__).find_last_of('\\') + 1u, std::string(__FILE__).size()) << " " << __LINE__ <<
#	define NABI_ALLOCATOR_SHORT_LOCATION_END(line) \
	NABI_ALLOCATOR_SPACE(nabi_allocator::debug_utils::c_LogLocationIndent - std::string(NABI_ALLOCATOR_STR(line)).length()) 

// Format
#	define NABI_ALLOCATOR_ASSERT_PREP "ASSERT"
#	define NABI_ALLOCATOR_LOG_PREP    NABI_ALLOCATOR_SHORT_LOCATION NABI_ALLOCATOR_SHORT_LOCATION_END(__LINE__)

#	define NABI_ALLOCATOR_MSG_DIVIDER ": "
#	define NABI_ALLOCATOR_MSG_END     std::endl

#	define NABI_ALLOCATOR_SPACE(numberOfSpaces) std::string(numberOfSpaces, ' ')
#	define NABI_ALLOCATOR_WRAP(item, wrapCharacter) wrapCharacter << item << wrapCharacter

// Macro creation
#	define NABI_ALLOCATOR_MAKE_MSG_STRING(stringVariable, content) \
	std::ostringstream msgStream = {}; \
	msgStream << content; \
	std::string stringVariable = msgStream.str();

// --- Assertion Macros ---

// Implementation
#   define NABI_ALLOCATOR_ASSERT_BASE(condition, message) \
		do \
		{ \
			if (!(condition)) { \
				NABI_ALLOCATOR_MAKE_MSG_STRING(assertString, \
					NABI_ALLOCATOR_ASSERT_PREP << NABI_ALLOCATOR_MSG_DIVIDER << message << NABI_ALLOCATOR_MSG_END) \
				\
				NABI_ALLOCATOR_LOG(assertString); \
				_RPTF0(_CRT_ASSERT, assertString.c_str()); \
			} \
		} \
		while (false)

// Use
#   define NABI_ALLOCATOR_ASSERT(condition, message) NABI_ALLOCATOR_ASSERT_BASE(condition, message)
#	define NABI_ALLOCATOR_ASSERT_DEFAULT(condition) NABI_ALLOCATOR_ASSERT_BASE(condition, NABI_ALLOCATOR_STR(condition))
#   define NABI_ALLOCATOR_ASSERT_FAIL(message) NABI_ALLOCATOR_ASSERT(false, message)

// --- Logging Macros ---

// Implementation
#	define NABI_ALLOCATOR_LOG_BASE(message) \
	do \
	{ \
		NABI_ALLOCATOR_MAKE_MSG_STRING(logString, \
			NABI_ALLOCATOR_LOG_PREP << message << NABI_ALLOCATOR_MSG_END) \
		\
		std::cout << logString; \
	} \
	while (false)

// Use
#	define NABI_ALLOCATOR_LOG(message) NABI_ALLOCATOR_LOG_BASE(message)

// --- Other Debug Macros ---

// Use
#	define NABI_ALLOCATOR_FUNCTION_NOT_IMPLEMENTED NABI_ALLOCATOR_ASSERT_FAIL("The function " << __FUNCTION__ << " is not implemented!")
#else
#	pragma warning( push ) // Push but never pop
#	pragma warning( disable : 4002 ) // Disables 'too many arguements for function-like macro' warning we get when using (void(0))

#   define NABI_ALLOCATOR_ASSERT(condition, message) NABI_ALLOCATOR_MACRO_NOT_DEFINED
#	define NABI_ALLOCATOR_ASSERT_DEFAULT(condition) NABI_ALLOCATOR_MACRO_NOT_DEFINED
#   define NABI_ALLOCATOR_ASSERT_FAIL(message) NABI_ALLOCATOR_MACRO_NOT_DEFINED
#	define NABI_ALLOCATOR_LOG(message) NABI_ALLOCATOR_MACRO_NOT_DEFINED
#	define NABI_ALLOCATOR_FUNCTION_NOT_IMPLEMENTED NABI_ALLOCATOR_MACRO_NOT_DEFINED
#endif // ifdef NABI_ALLOCATOR_DEBUG
