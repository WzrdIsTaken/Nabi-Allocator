#pragma once

// Nabi Headers
#include "IntegerTypes.h"
#include "MemoryConstants.h"
#include "TemplateConcepts.h"

/**
 * Functions & macros to perform memory manipulation.
*/

#define NABI_ALLOCATOR_TO_UPTR(arg) reinterpret_cast<nabi_allocator::uPtr>(arg)
#define NABI_ALLOCATOR_TO_VPTR(arg) reinterpret_cast<void*>(arg)

namespace nabi_allocator::memory_operations
{
	template<typename T>
	[[nodiscard]] inline T RequestMemoryFromOS(uInt const numBytes);
	template<typename T>
	inline void ReleaseMemoryToOS(T*& memory);

	template<typename T>
	inline void ResetMemory(T* const memory);
	inline void ResetMemory(void* const destination, uInt const numBytes);

	[[nodiscard]] inline uInt GetMemorySize(void const* const start, void const* const end);
	[[nodiscard]] inline constexpr uInt GetMemorySize(uPtr const start, uPtr const end) noexcept;

	template<is_integral T>
	[[nodiscard]] inline constexpr bool IsAlligned(T const value, uInt const allignment) noexcept;
	[[nodiscard]] inline bool IsAlligned(void const* const address, uInt const allignment);
} // namespace nabi_allocator

// Inline Inline
#include "Operations/MemoryOperations.inl"
