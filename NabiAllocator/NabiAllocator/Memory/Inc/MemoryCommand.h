#pragma once

// STD Headers
#include <stack>

// Nabi Headers
#include "IntegerTypes.h"
#include "MemoryConstants.h"
#include "TypeUtils.h"

#include <vector>

/**
 * A global singleton which the overriden new/delete operators route allocations through.
 * The MemoryCommand manages which heap zone and memory tag to use.
*/

// TODO add to notes section why we have to do the same stuff
// TODO ctrl+f TODO in this file / cpp
// -Annotate the MemoryCommand::Allocate / Free if checks with likelyand unlikely as well ?
/*
* Honeslty we want all the static memory just to be allocated on the heap? i guess it will be. but we
* still want it rotuing from the memory command. how can we do this?
* 
* // its ok though, because all static varibales will go in here until an unamanged heap is setup anyway (by the user)
// I think the unmanaged heap will have to be set up by the user, cos otherwise we will have some static variabls allocated there and others just by cpp
// We can just have a macro called like NA_SETUP / NA_DEFAULT_SETUP in a new config file, and they will set up an unmanaged heap and a statck/free list alloc on atop?

//TODOHow is our plan with the unmanaged heap going to work for freeing.. because we need to find heap zone for ptr
//Perhaps if we can't find it, just call the normal free?
// so yeah i dont think the memory command functions can default to an unamaeheap they make themselves

	// we need a way to find which heap zone owns memory
	// also we need some asserts eg if top heap zope scope doesnt exist

	Is the way we are routing to the default malloc/free most optimimal?
	
	Add Notes.

	We need a note that all static allocations will happen through the default malloc/free cos no heap zome as been setup.
		-> Or will we setup?


	Tidy up the memory command (this!)

	do we even want the liely / unlilely stuff? i think we should have it somewhere just because its kinda cool, but idk abouithere?
		also have a goto somewhere? :D
*/

namespace nabi_allocator
{
	class HeapZoneBase;
	class HeapZoneScope;
} // namespace nabi_allocator

namespace nabi_allocator
{
	class MemoryCommand final
	{
		NA_IMPLEMENT_SINGLETON(MemoryCommand)

	public:
		MemoryCommand();
		~MemoryCommand();

		[[nodiscard]] void* Allocate(uInt const numBytes);
		void Free(void* const memory);

		void PushHeapZoneScope(HeapZoneScope const& heapZoneScope);
		void PopHeapZoneScope(HeapZoneScope const& heapZoneScope);

		[[nodiscard]] HeapZoneScope const* const GetTopHeapZoneScope() const noexcept;

	private:
		NA_SET_COPY_MOVE_CONSTRUCTORS(MemoryCommand, delete);

		// TODO should I actually have the base heap zone now.. unmanaged heap or free list alloc?
		// TODO do we want these to be thread local as well? or the whole singleton to be thread local?
		HeapZoneBase* m_LastHeapZone;
		memoryTag m_LastMemoryTag;
	};
} // namespace nabi_allocator
