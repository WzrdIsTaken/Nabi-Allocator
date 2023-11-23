// inl's Header
#include "UnmanagedHeap.h"

// Nabi Headers
#include "Operations\MemoryOperations.h"

namespace nabi_allocator
{
	inline void* UnmanagedHeap::Allocate(uInt const numBytes) const
	{
		return memory_operations::RequestMemoryFromOS<void*>(numBytes);
	}

	inline void UnmanagedHeap::Free(void* /*const :p*/ memory) const
	{
		memory_operations::ReleaseMemoryToOS(memory); // Note - it is intended that the "memory" ptr is not set to null
	}
} // namespace nabi_allocator
