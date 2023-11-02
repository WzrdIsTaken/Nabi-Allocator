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
		inline HeapZoneScope(HeapZoneBase* const heapZone, memoryTag const* const memoryTag, bool const registerWithMemoryCommand = true);
		inline ~HeapZoneScope();

		 [[nodiscard]] inline HeapZoneBase* const GetHeapZone() const noexcept;
#ifdef NA_MEMORY_TAGGING
		 [[nodiscard]] inline memoryTag const* const GetMemoryTag() const noexcept;
#endif // ifdef NA_MEMORY_TAGGING

	private:
		NA_SET_COPY_MOVE_CONSTRUCTORS(HeapZoneScope, delete);

		HeapZoneBase* const m_HeapZone;
#ifdef NA_MEMORY_TAGGING
		memoryTag const* const m_MemoryTag;
#endif // ifdef NA_MEMORY_TAGGING

#if defined NA_DEBUG || defined NA_TESTS
		bool m_RegisterWithMemoryCommand; // This is jank. Perhaps some sort of mocking instead? Also included in debug cos it might be useful
#endif // ifdef NA_DEBUG || NA_TESTS
	};
} // namespace nabi_allocator

// Include Inline
#include "Memory\Inl\HeapZone\HeapZoneScope.inl"
