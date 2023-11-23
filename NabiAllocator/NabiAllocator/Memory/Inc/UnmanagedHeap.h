#pragma once

// Nabi Headers
#include "IntegerTypes.h"
#include "TypeUtils.h"

/**
 * A basic allocator, wrapping malloc() and free(). Its used in the MemoryCommand when no heap zone has been set, or the user
 * has specified to use it via c_UnmanagedHeap. Ngl, the whole workflow with this is kinda jank (at least right now...
 * will it ever change..? :p) because I cba to refactor some code. But - it works! 
 * 
 * The problem of having an unmanaged allocator is one I probably should have solved much earlier in the project. 
 * Now I'm left with the choice of either hacking it in, or refactoring lots of stuff to make it play nice.
 * I chose the former option, I can always optimise it later. After all, no one cares how nice your code is if it
 * doesn't actually work...
 * 
 * The two nicer solutions I thought of were either to have a proper UnmanagedHeap class which sets in an inheritance tree
 * along with HeapZone (perhaps a base Heap class even?), or to have an UnmanagedAllocator. Both of these options had pros and cons.
 * 
 * Note:
 *	- The reason this class has private constructors is so this class can only be created in the MemoryCommand. 
 *  - The friend stuff works like this: https://stackoverflow.com/questions/4492062/why-does-a-c-friend-class-need-a-forward-declaration-only-in-other-namespaces
*/

namespace nabi_allocator
{
	class UnmanagedHeap final
	{
	public:
		inline void* Allocate(uInt const numBytes) const;
		inline void Free(void* const memory) const;

	private:
		UnmanagedHeap() = default;
		~UnmanagedHeap() = default;
		NA_SET_COPY_MOVE_CONSTRUCTORS(UnmanagedHeap, delete);

		friend class MemoryCommand;
	};
} // namespace nabi_allocator

// Include Inline
#include "Memory\Inl\UnmanagedHeap.inl"
