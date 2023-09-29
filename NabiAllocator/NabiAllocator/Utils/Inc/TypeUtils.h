#pragma once

// STD Headers
#include <type_traits>

// Nabi Headers
#include "TemplateConcepts.h"

/**
 * A collection of helper macros and functions for classes/structs/enums/etc.
*/

#define NA_SET_COPY_MOVE_CONSTRUCTORS(type, operation) \
	NA_SET_COPY_CONSTRUCTORS(type, operation) \
    NA_SET_MOVE_CONSTRUCTORS(type, operation)

#define NA_SET_COPY_CONSTRUCTORS(type, operation) \
     type(type&) = operation; \
     type(type const&) = operation; \

#define NA_SET_MOVE_CONSTRUCTORS(type, operation) \
    type(type&&) = operation; \
    type(type const&&) = operation;

#define NA_NAMEOF(arg) ((decltype(&arg))nullptr, #arg) // C# nameof functionality. Source: https://stackoverflow.com/a/38697366/8890269
#define NA_NAMEOF_LITERAL(arg) #arg // Some things, like static asserts, require a string literal

namespace nabi_allocator::type_utils
{
	// Used to convert an enum to its underlying type (note - to_underlying will do this in cpp23)
	template <is_enum Enum>
    [[nodiscard]] inline constexpr typename std::underlying_type<Enum>::type ToUnderlying(Enum const e) noexcept;

    template<typename TypeOne, typename TypeTwo>
    [[nodiscard]] inline constexpr TypeTwo ReinterpretCastIfNeeded(TypeOne const value);
} // namespace nabi_allocator::type_utils

// Include Inline
#include "Utils\Inl\TypeUtils.inl"
