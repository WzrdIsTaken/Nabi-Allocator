#pragma once

// STD Headers
#include <type_traits>

/**
 * Generic concepts for templates. 
 * We could use the "requires" keword with each template, but I feel like that makes things cluttered.
*/

namespace nabi_allocator
{
	template<typename T>
	concept is_integral = std::is_integral<T>::value;
} // namespace nabi_allocator
