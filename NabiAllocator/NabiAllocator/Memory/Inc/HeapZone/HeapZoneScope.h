#pragma once

// STD Headers
#include <optional>
#include <type_traits>

// Config Headers
#include "Config.h"

// Nabi Headers
#include "HeapZoneBase.h"
#include "IntegerTypes.h"
#include "MemoryConstants.h"
#include "TypeUtils.h"

/**
 * Controls which heap zone and memory tag is used. 
 * Passing in a nullptr/nullopt instead of heap zone or tag will default to the last set value.
 * 
 * The HeapZoneScope uses RAII so it will be registered / unregistered with the MemoryCommand on its creation/destruction.
 * 
 * Note:
 *	- We take in an optional memoryTag because we aren't going to reference the it, it will probs be an enum value or something.
 *	- va_args in the NA_MAKE_HEAP_ZONE_AND_SET_SCOPE & NA_SET_HEAP_ZONE_SCOPE macro are needed to support the bot custom memory command thing.
 *  - NA_MAKE_HEAP_ZONE_AND_SET_SCOPE is the one macro a user needs to write in their code, eg above the main loop, to start using nabi allocator.
*/

#define NA_MAKE_HEAP_ZONE_AND_SET_SCOPE(zoneType, zoneParent, zoneNumBytes, zoneDebugName, scopeMemoryTag, ...) \
	static_assert(std::is_base_of<nabi_allocator::HeapZoneBase, zoneType>(), "HeapZone's must inherit from " NA_NAMEOF_LITERAL(nabi_allocator::HeapZoneBase)); \
	zoneType heapZone = { zoneParent, zoneNumBytes, zoneDebugName }; \
	NA_SET_HEAP_ZONE_SCOPE(&heapZone, scopeMemoryTag, __VA_ARGS__);
	
#define NA_SET_HEAP_ZONE_SCOPE(heapZone, _memoryTag, ...) \
	nabi_allocator::HeapZoneScope const heapZoneScope = { heapZone, _memoryTag, __VA_ARGS__ };

namespace nabi_allocator
{
	class MemoryCommand;
} // namespace nabi_allocator

namespace nabi_allocator
{
	HeapZoneBase constexpr* const c_SameZone = nullptr;
	std::optional<memoryTag const> constexpr c_SameTag = std::nullopt;

	class HeapZoneScope final
	{
	public:
		inline HeapZoneScope(HeapZoneBase* const heapZone, std::optional<memoryTag const> const memoryTag, MemoryCommand* const customMemoryCommand = nullptr);
		inline ~HeapZoneScope();

		 [[nodiscard]] inline HeapZoneBase* const GetHeapZone() const noexcept;
#ifdef NA_MEMORY_TAGGING
		 [[nodiscard]] inline std::optional<memoryTag const> const& GetMemoryTag() const noexcept;
#endif // ifdef NA_MEMORY_TAGGING

	private:
		NA_SET_COPY_MOVE_CONSTRUCTORS(HeapZoneScope, delete);

		HeapZoneBase* const m_HeapZone;
#ifdef NA_MEMORY_TAGGING
		std::optional<memoryTag const> const m_MemoryTag;
#endif // ifdef NA_MEMORY_TAGGING

		// This logic is half jank half kinda needed. It's half jank because it started off as a hack to be able to test 
		// instanced MemoryCommandsin isolation (eg, in MemoryCommandTests.cpp), but then it became half needed because 
		// if NA_OVERRIDE_NEW_DELETE is not defined and the user still wants to use heap zone scopes they will need it.
		MemoryCommand* const m_CustomMemoryCommand;
	};
} // namespace nabi_allocator

// Include Inline
#include "Memory\Inl\HeapZone\HeapZoneScope.inl"
