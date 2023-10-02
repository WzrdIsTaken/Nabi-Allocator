// inl's Header
#include "Operations/MemoryOperations.h"

// STD Headers
#include <cstdlib>
#include <cstring>

// Nabi Headers
#include "DebugUtils.h"

namespace nabi_allocator::memory_operations
{
	template<typename T>
	inline T RequestMemoryFromOS(uInt const numBytes)
	{
		NA_ASSERT(numBytes > 0u, "Requesting 0 bytes");

		void* const memory = std::malloc(static_cast<std::size_t>(numBytes));
		NA_ASSERT(memory, "Call to malloc requesting " << numBytes << " failed! Are we out of memory?");

		return reinterpret_cast<T>(memory);
	}

	template<typename T>
	inline void ReleaseMemoryToOS(T*& memory)
	{
		NA_ASSERT(memory, "Can't free null memory!");

		std::free(memory);
		memory = nullptr;
	}

	template<typename T>
	inline void ResetMemory(T* const memory)
	{
		NA_ASSERT(memory, "Can't reset null memory!");
		std::memset(memory, NULL, sizeof(T));
	}

	inline void ResetMemory(void* const destination, uInt const numBytes)
	{
		NA_ASSERT(destination, "Can't reset null memory!");
		std::memset(destination, NULL, static_cast<std::size_t>(numBytes));
	}

	inline constexpr uInt GetMemorySize(uPtr const start, uPtr const end) noexcept
	{
		return end - start;
	}

	inline uInt GetMemorySize(void const* const start, void const* const end)
	{
		return GetMemorySize(NA_TO_UPTR(start), NA_TO_UPTR(end));
	}

	template<is_integral T>
	inline constexpr bool IsAlligned(T const value, uInt const allignment) noexcept
	{
		return (static_cast<uInt>(value) % allignment) == 0u;
	}

	inline bool IsAlligned(void const* const address, uInt const allignment)
	{
		return IsAlligned(NA_TO_UPTR(address), allignment);
	}

	inline constexpr bool IsPtrInRange(uPtr const start, uPtr const end, uPtr const ptr) noexcept
	{
		return ptr >= start && ptr < end;
	}

	inline bool IsPtrInRange(void const* const start, void const* const end, void const* const ptr)
	{
		return IsPtrInRange(NA_TO_UPTR(start), NA_TO_UPTR(end), NA_TO_UPTR(ptr));
	}
} // namespace nabi_allocator::memory_operations
