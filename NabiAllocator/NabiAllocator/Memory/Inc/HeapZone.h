#pragma once

// Nabi Header
#include "AllocatorBase.h"
#include "HeapZoneBase.h"
#include "IntegerTypes.h"
#include "TypeUtils.h"

/**
 * HeapZones mark out a section of memory which their templated allocator manages.
 * An application can have multiple HeapZones, and which one is used is controlled by the MemoryCommand.
*/

namespace nabi_allocator
{
	template<is_allocator T>
	class HeapZone final : public HeapZoneBase
	{
	public:
		HeapZone(u32 const numBytes, std::string const& debugName);
		~HeapZone() override;

		[[nodiscard]] inline virtual void* Allocate(u32 const numBytes) override;
		inline virtual void Free(void* const memory) override;

		inline T& GetAllocator() noexcept;

	private:
		NABI_ALLOCATOR_SET_COPY_MOVE_CONSTRUCTORS(HeapZone, delete);

		T m_Allocator;
	};
} // namespace nabi_allocator

// Include Inline
#include "HeapZone.inl"
