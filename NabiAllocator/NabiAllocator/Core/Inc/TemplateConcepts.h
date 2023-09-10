#pragma once

// STD Headers
#include <type_traits>

/**
 * Generic concepts for templates. 
 * 
 * We could use the "requires" keword with each template, but I feel like that makes things cluttered.
 * Another option is to use static asserts in the function implementation... would this be better?
*/

namespace nabi_allocator
{
	template<typename T>
	concept is_integral = std::is_integral_v<T>;

	template<typename T>
	concept is_pointer = std::is_pointer_v<T>;
} // namespace nabi_allocator
