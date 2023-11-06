#pragma once

// STD Headers
#include <optional>

// Config Headers
#include "Config.h"

// Nabi Headers
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
 *	- va_args in the NA_HEAP_ZONE_SCOPE macro are needed to support the bot custom memory command thing.
*/

#define NA_HEAP_ZONE_SCOPE(heapZone, _memoryTag, ...) \
	HeapZoneScope const heapZoneScope = { heapZone, _memoryTag, __VA_ARGS__ };

namespace nabi_allocator
{
	class HeapZoneBase;
	class MemoryCommand;
} // namespace nabi_allocator

namespace nabi_allocator
{
	class HeapZoneScope final
	{
	public:
		static HeapZoneBase constexpr* const c_SameZone = nullptr;
		static std::optional<memoryTag const> constexpr c_SameTag = std::nullopt;

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

#if defined NA_DEBUG || defined NA_TESTS || defined NA_WORKFLOWS
		MemoryCommand* const m_CustomMemoryCommand; // This is jank. Perhaps some sort of mocking instead? Also included in debug cos it might be useful
#endif // ifdef NA_DEBUG || NA_TESTS || NA_WORKFLOWS
	};
} // namespace nabi_allocator

// Include Inline
#include "Memory\Inl\HeapZone\HeapZoneScope.inl"
