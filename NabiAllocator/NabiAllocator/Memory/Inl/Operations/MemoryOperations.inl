// inl's Header
#include "Operations/MemoryOperations.h"

// STD Headers
#include <cstdlib>

// Nabi Headers
#include "DebugUtils.h"

namespace nabi_allocator::memory_operations
{
	template<typename T>
	inline T RequestMemoryFromOS(u32 const numBytes)
	{
		NABI_ALLOCATOR_ASSERT(numBytes > 0u, "Requesting 0 bytes");

		void* const memory = std::malloc(static_cast<std::size_t>(numBytes));
		NABI_ALLOCATOR_ASSERT(memory, "Call to malloc requesting " << numBytes << " failed! Are we out of memory?");

		return reinterpret_cast<T>(memory);
	}

	template<typename T>
	inline void ReleaseMemoryToOS(T*& memory)
	{
		NABI_ALLOCATOR_ASSERT(memory, "Can't free null memory!");

		std::free(memory);
		memory = nullptr;
	}

	template<typename T>
	inline void ResetMemory(T* const memory)
	{
		NABI_ALLOCATOR_ASSERT(memory, "Can't reset null memory!");
		std::memset(memory, NULL, sizeof(T));
	}

	inline void ResetMemory(void* const destination, u32 const size)
	{
		NABI_ALLOCATOR_ASSERT(destination, "Can't reset null memory!");
		std::memset(destination, NULL, size);
	}

	inline uPtr GetMemorySize(void const* const start, void const* const end)
	{
		return GetMemorySize(NABI_ALLOCATOR_TO_UPTR(start), NABI_ALLOCATOR_TO_UPTR(end));
	}

	inline constexpr uPtr GetMemorySize(uPtr const start, uPtr const end) noexcept
	{
		return end - start;
	}

	template<is_integral T>
	inline constexpr bool IsAlligned(T const value, u32 const allignment) noexcept
	{
		return (static_cast<u32>(value) % allignment) == 0u;
	}
} // namespace nabi_allocator::memory_operations
