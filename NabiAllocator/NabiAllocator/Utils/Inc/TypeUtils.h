#pragma once

// STD Headers
#include <type_traits>

/**
 * A collection of helper macros and functions for classes/structs/enums/etc.
*/

#define NABI_ALLOCATOR_DELETE_COPY_MOVE_CONSTRUCTORS(type) \
    NABI_ALLOCATOR_DELETE_COPY_CONSTRUCTORS(type) \
    NABI_ALLOCATOR_DELETE_MOVE_CONSTRUCTORS(type)

#define NABI_ALLOCATOR_DELETE_COPY_CONSTRUCTORS(type) \
     type(type&) = delete; \
     type(type const&) = delete; \

#define NABI_ALLOCATOR_DELETE_MOVE_CONSTRUCTORS(type) \
    type(type&&) = delete; \
    type(type const&&) = delete;

namespace nabi_allocator::type_utils
{
	// Used to convert an enum to its underlying type (note - to_underlying will do this in cpp23)
	template <typename Enum>
	inline constexpr typename std::underlying_type<Enum>::type ToUnderlying(Enum e) noexcept
	{
		return static_cast<typename std::underlying_type<Enum>::type>(e);
	}
} // namespace nabi_allocator::type_utils
