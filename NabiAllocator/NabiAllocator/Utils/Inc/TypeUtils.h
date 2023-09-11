#pragma once

// STD Headers
#include <type_traits>

/**
 * A collection of helper macros and functions for classes/structs/enums/etc.
*/

#define NABI_ALLOCATOR_SET_COPY_MOVE_CONSTRUCTORS(type, operation) \
	NABI_ALLOCATOR_SET_COPY_CONSTRUCTORS(type, operation) \
    NABI_ALLOCATOR_SET_MOVE_CONSTRUCTORS(type, operation)

#define NABI_ALLOCATOR_SET_COPY_CONSTRUCTORS(type, operation) \
     type(type&) = operation; \
     type(type const&) = operation; \

#define NABI_ALLOCATOR_SET_MOVE_CONSTRUCTORS(type, operation) \
    type(type&&) = operation; \
    type(type const&&) = operation;

#define NABI_ALLOCATOR_NAMEOF(arg) ((decltype(&arg))nullptr, #arg) // C# nameof functionality. Source: https://stackoverflow.com/a/38697366/8890269
#define NABI_ALLOCATOR_NAMEOF_LITERAL(arg) #arg // Some things, like static asserts, require a string literal

namespace nabi_allocator::type_utils
{
	// Used to convert an enum to its underlying type (note - to_underlying will do this in cpp23)
	template <typename Enum>
	inline constexpr typename std::underlying_type<Enum>::type ToUnderlying(Enum const e) noexcept
	{
		return static_cast<typename std::underlying_type<Enum>::type>(e);
	}
} // namespace nabi_allocator::type_utils
