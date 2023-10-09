#pragma once

/**
 * Settings for the StackAllocator.
*/

namespace nabi_allocator::stack_allocator
{
	struct StackAllocatorSettings final
	{
		// As of 09/10/23... this is empty.
		// And ngl, I can't think of any settings I would want right now...
		// But its future proof at least!
	};

	StackAllocatorSettings constexpr c_StackAllocatorDefaultSettings =
	{
	};
} // namespace nabi_allocator::stack_allocator
