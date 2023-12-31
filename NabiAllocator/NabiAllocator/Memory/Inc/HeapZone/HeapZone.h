#pragma once

// STD Headers
#include <mutex>

// Config Headers
#include "Config.h"

// Nabi Header
#include "Allocators/AllocatorBase.h"
#include "HeapZoneBase.h"
#include "IntegerTypes.h"
#include "TypeUtils.h"

/**
 * HeapZones mark out a section of memory which their templated allocator manages.
 * An application can have multiple HeapZones, and which one is used is controlled by the MemoryCommand.
*/

namespace nabi_allocator
{
	struct AllocationInfo;
} // namespace nabi_allocator

namespace nabi_allocator
{
	template<is_allocator T>
	class HeapZone final : public HeapZoneBase
	{
	public:
		HeapZone(HeapZoneBase* const parentZone, uInt const numBytes, std::string const& debugName);
		~HeapZone() override;

		[[nodiscard]] inline void* Allocate(AllocationInfo const& allocationInfo) override;
		inline void Free(void* const memory) override;
		inline void Reset() override;

		[[nodiscard]] inline T& GetAllocator() noexcept;

	private:
		NA_SET_COPY_MOVE_CONSTRUCTORS(HeapZone, delete);

		T m_Allocator;
#ifdef NA_THREAD_SAFE_HEAP_ZONE
		std::mutex m_Mutex;
#endif // ifdef NA_THREAD_SAFE_HEAP_ZONE
	};
} // namespace nabi_allocator

// Include Inline
#include "Memory\Inl\HeapZone\HeapZone.inl"
