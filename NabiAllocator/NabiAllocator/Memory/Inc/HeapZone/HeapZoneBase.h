#pragma once

// STD Headers
#include <string>

// Config Headers
#include "Config.h"

// Nabi Headers
#include "HeapZoneInfo.h"
#include "IntegerTypes.h"
#include "TypeUtils.h"

/**
 * The base class for all HeapZones. HeapZones have a base so that they can be grouped regardless of which allocator they use.
 * If HeapZones have a parent, then their memory will be allocated from the parent rather than using malloc. This allows for
 * a design where malloc only needs to be called once.
*/

namespace nabi_allocator
{
	class HeapZoneBase;
	template<typename T>
	concept is_heap_zone = std::is_base_of_v<HeapZoneBase, T>;

	class HeapZoneBase abstract
	{
	public:
		static HeapZoneBase constexpr* const c_NoParent = nullptr;

		HeapZoneBase();
		virtual ~HeapZoneBase() = default;

		HeapZoneInfo& Init(HeapZoneBase* const parentZone, uInt const numBytes, std::string const& debugName);
		void Deinit();

		[[nodiscard]] inline virtual void* Allocate(uInt const numBytes) = 0;
		inline virtual void Free(void* const memory) = 0;

		inline HeapZoneInfo const& GetZoneInfo() const noexcept;
#ifdef NABI_ALLOCATOR_DEBUG
		inline std::string const& GetDebugName() const noexcept;
#endif // ifdef NABI_ALLOCATOR_DEBUG

		inline bool IsInitialized() const noexcept;

	protected:
		HeapZoneInfo m_ZoneInfo;
		HeapZoneBase* m_ParentZone;
#ifdef NABI_ALLOCATOR_DEBUG
		std::string m_DebugName;
#endif // ifdef NABI_ALLOCATOR_DEBUG

	private:
		NABI_ALLOCATOR_SET_COPY_MOVE_CONSTRUCTORS(HeapZoneBase, delete);
	};
} // namespace nabi_allocator

// Include Inline
#include "Memory\Inl\HeapZone\HeapZoneBase.inl"
