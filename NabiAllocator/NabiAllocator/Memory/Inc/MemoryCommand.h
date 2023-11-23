#pragma once

// Nabi Headers
#include "IntegerTypes.h"
#include "TypeUtils.h"
#include "UnmanagedHeap.h"

/**
 * A global singleton which the overriden new/delete operators route allocations through.
 * The MemoryCommand manages which heap zone and memory tag to use.
 * 
 * Notes:
 *  - Static allocations will happen through the default allocation path, set as std::malloc/std::free.
 *	- The g_HeapZoneScopes stack and other scope related stuff is thread_local and declared in the cpp.
 *    This is because we don't want, eg, different threads changing the scope for the main thread.
 *  - I'm using [[likely]] & [[unlikely]] in the implementation... thoughts on their usage?
 *		- The [[unlikely]] branch should only be called for static memory...
 *  - The constructor/destructor can't be private because I create a local memory command in tests.
 *		- Think of a way to solve this?
 *  - The reason c_UnmanagedHeap is outside of the MemoryCommand is, well, because I want it at 
 *    namespace scope so it matches with c_SameZone and c_SameTag (defined in HeapZoneScope.h).
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
		[[nodiscard]] UnmanagedHeap const& GetUnmanagedHeap() const noexcept;

	private:
		NA_SET_COPY_MOVE_CONSTRUCTORS(MemoryCommand, delete);

		UnmanagedHeap m_UnmanagedHeap;
	};

	// The whole workflow with this UnmanagedHeap heap is so jank (here and in the cpp), but honestly...
	// I kinda like it xD. I'll change it later if need be. As for now, its fine.......
	inline HeapZoneBase /*const*/& c_UnmanagedHeap =
		reinterpret_cast<HeapZoneBase&>(
			const_cast<UnmanagedHeap&>(
				MemoryCommand::Instance().GetUnmanagedHeap()
		));
} // namespace nabi_allocator
