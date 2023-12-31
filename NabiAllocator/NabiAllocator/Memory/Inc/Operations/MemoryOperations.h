#pragma once

// STD Headers
#include <type_traits>

// Nabi Headers
#include "IntegerTypes.h"
#include "TemplateConcepts.h"
#include "TypeUtils.h"

/**
 * Functions & macros to perform memory manipulation.
*/

#define NA_REINTERPRET_MEMORY(returnPtrType, startPtr, operation, numBytes) \
	reinterpret_cast<returnPtrType*>(nabi_allocator::type_utils::ReinterpretCastIfNeeded \
		<std::remove_const_t<decltype(startPtr)>, nabi_allocator::uPtr>(startPtr) operation numBytes)
#define NA_REINTERPRET_MEMORY_DEFAULT(returnPtrType, startPtr) \
	NA_REINTERPRET_MEMORY(returnPtrType, startPtr, +, 0u)

#define NA_TO_UPTR(arg) reinterpret_cast<nabi_allocator::uPtr>(arg)
#define NA_TO_VPTR(arg) reinterpret_cast<void*>(arg)

namespace nabi_allocator::memory_operations
{
	template<typename T>
	[[nodiscard]] inline T RequestMemoryFromOS(uInt const numBytes);
	template<typename T>
	inline void ReleaseMemoryToOS(T*& memory);

	template<typename T>
	inline void ResetMemory(T* const memory);
	inline void ResetMemory(void* const destination, uInt const numBytes);

	[[nodiscard]] inline constexpr uInt GetMemorySize(uPtr const start, uPtr const end) noexcept;
	[[nodiscard]] inline uInt GetMemorySize(void const* const start, void const* const end);

	template<is_integral T>
	[[nodiscard]] inline constexpr bool IsAlligned(T const value, uInt const allignment) noexcept;
	[[nodiscard]] inline bool IsAlligned(void const* const address, uInt const allignment);

	[[nodiscard]] inline constexpr bool IsPtrInRange(uPtr const start, uPtr const end, uPtr const ptr) noexcept;
	[[nodiscard]] inline bool IsPtrInRange(void const* const start, void const* const end, void const* const ptr);
} // namespace nabi_allocator

// Inline Inline
#include "Memory\Inl\Operations\MemoryOperations.inl"
