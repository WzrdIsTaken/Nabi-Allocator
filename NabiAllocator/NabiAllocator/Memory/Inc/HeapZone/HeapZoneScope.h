#pragma once

// Config Headers
#include "Config.h"

// Nabi Headers
#include "IntegerTypes.h"
#include "MemoryConstants.h"
#include "TypeUtils.h"

/**
 * Controls which heap zone and memory tag is used. 
 * Passing in a nullptr instead of heap zone or tag will default to the last set value.
 * 
 * The HeapZoneScope uses RAIL, so it will be registered / unregistered with the MemoryCommand
 * on its creation/destruction.
*/

namespace nabi_allocator
{
	class HeapZoneBase;
} // namespace nabi_allocator

namespace nabi_allocator
{
	class HeapZoneScope final
	{
	public:
#ifdef NA_TESTS
		// This is kinda jank. Perhaps some sort of mocking instead? tl;dr - for  
		// some tests I don't want the scope to register with the MemoryCommand.
		HeapZoneScope() : m_HeapZone(nullptr), m_MemoryTag(nullptr) {};
#endif // ifdef NA_TESTS

		inline HeapZoneScope(HeapZoneBase* const heapZone, MemoryTag const* const memoryTag);
		inline ~HeapZoneScope();

		 [[nodiscard]] inline HeapZoneBase* const GetHeapZone() const noexcept;
#ifdef NA_MEMORY_TAGGING
		 [[nodiscard]] inline MemoryTag const* const GetMemoryTag() const noexcept;
#endif // ifdef NA_MEMORY_TAGGING

	private:
		NA_SET_COPY_MOVE_CONSTRUCTORS(HeapZoneScope, delete);

		HeapZoneBase* const m_HeapZone;
#ifdef NA_MEMORY_TAGGING
		MemoryTag const* const m_MemoryTag;
#endif // ifdef NA_MEMORY_TAGGING
	};
} // namespace nabi_allocator

// Include Inline
#include "Memory\Inl\HeapZone\HeapZoneScope.inl"
