#pragma once

// Nabi Headers
#include "IntegerTypes.h"
#include "TypeUtils.h"

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
*/

namespace nabi_allocator
{
	class HeapZoneScope;
} // namespace nabi_allocator

namespace nabi_allocator
{
	class MemoryCommand final
	{
		NA_IMPLEMENT_SINGLETON(MemoryCommand)

	public:
		MemoryCommand() = default;
		~MemoryCommand();

		[[nodiscard]] void* Allocate(uInt const numBytes);
		void Free(void* const memory);

		void PushHeapZoneScope(HeapZoneScope const& heapZoneScope);
		void PopHeapZoneScope(HeapZoneScope const& heapZoneScope);

		[[nodiscard]] HeapZoneScope const* const GetTopHeapZoneScope() const noexcept;

	private:
		NA_SET_COPY_MOVE_CONSTRUCTORS(MemoryCommand, delete);
	};
} // namespace nabi_allocator
