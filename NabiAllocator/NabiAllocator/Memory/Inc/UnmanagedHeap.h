#pragma once

// Nabi Headers
#include "IntegerTypes.h"
#include "TypeUtils.h"

/*
- UnmanagedHeap, doesnt even inherit
- Default malloc/free funcs
- created in the memory command (on the stack? can we constexpr set the ref?)
- default allocations are routed through it
- heap zone scope can be set to it
- if the scope is the unmanaged heap, then, just idk goto the if statement*/

// TODO have a base Heap class? then would need pure virtual destructor / rule of 6 ;p
// TODO have a test for c_SameTag and c_SameZone

/**
 * TODO desc + explain how unmanaged allocator is a little different and how i probs should have thought about how it would work at the start of the proj
 * + what the other options I considered were (eg: unmanaged heap)
*/

// I think the HeapZone::Contains ptr shouldnt be static anyway. Just public virtual

/**
 * TODO Decs
 * 
 * Note:
 *	- TODO something about the privae constructors
 *   - something about this https://stackoverflow.com/questions/4492062/why-does-a-c-friend-class-need-a-forward-declaration-only-in-other-namespaces
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
