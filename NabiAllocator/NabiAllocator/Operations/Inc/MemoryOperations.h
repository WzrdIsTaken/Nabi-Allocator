#pragma once

// Nabi Headers
#include "IntegerTypes.h"
#include "MemoryConstants.h"
#include "TemplateConcepts.h"

/**
 * Functions & macros to perform memory manipulation.
*/

#define TO_UPTR(arg) reinterpret_cast<nabi_allocator::uPtr>(arg)
#define TO_VPTR(arg) reinterpret_cast<void*>(arg)

namespace nabi_allocator::memory_operations
{
	template<typename T>
	[[nodiscard]] inline T RequestMemoryFromOS(u32 const numBytes);
	template<typename T>
	inline void ReleaseMemoryToOS(T*& memory);

	template<typename T>
	inline void ResetMemory(T* const memory);
	inline void ResetMemory(void* const destination, u32 const size);

	[[nodiscard]] inline uPtr GetMemorySize(void const* const start, void const* const end);
	[[nodiscard]] inline constexpr uPtr GetMemorySize(uPtr const start, uPtr const end) noexcept;

	template<is_integral T>
	[[nodiscard]] inline constexpr bool IsAlligned(T const value, u32 const allignment = c_MemoryAllignment) noexcept;
} // namespace nabi_allocator

// Inline Inline
#include "MemoryOperations.inl"
