#pragma once

// STD Headers
#include <string>
#include <type_traits>
#include <vector>

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
	struct AllocationInfo;
} // namespace nabi_allocator

namespace nabi_allocator
{
	class HeapZoneBase;
	template<typename T>
	concept is_heap_zone = std::is_base_of_v<HeapZoneBase, T>;

	class HeapZoneBase NA_ABSTRACT
	{
	public:
		static HeapZoneBase constexpr* const c_NoParent = nullptr;

		HeapZoneBase();
		virtual ~HeapZoneBase();

		[[nodiscard]] static uInt GetHeapZoneCount() noexcept;
		[[nodiscard]] static HeapZoneBase* FindHeapZoneForPtr(void const* const ptr);

		[[nodiscard]] HeapZoneInfo& Init(HeapZoneBase* const parentZone, uInt const numBytes, std::string const& debugName);
		void Deinit();

		[[nodiscard]] inline virtual void* Allocate(AllocationInfo const& allocationInfo) = 0;
		inline virtual void Free(void* const memory) = 0;
		inline virtual void Reset() = 0;

		[[nodiscard]] inline HeapZoneInfo const& GetZoneInfo() const noexcept;
#ifdef NA_DEBUG
		[[nodiscard]] inline std::string const& GetDebugName() const noexcept;
#endif // ifdef NA_DEBUG

		[[nodiscard]] inline bool IsInitialized() const noexcept;
		[[nodiscard]] inline bool ContainsPtr(void const* const ptr) const;

	protected:
		HeapZoneInfo m_ZoneInfo;
		HeapZoneBase* m_ParentZone;
#ifdef NA_DEBUG
		std::string m_DebugName;
#endif // ifdef NA_DEBUG

	private:
		NA_SET_COPY_MOVE_CONSTRUCTORS(HeapZoneBase, delete);

		static std::vector<HeapZoneBase*> s_AllHeapZones;
	};
} // namespace nabi_allocator

// Include Inline
#include "Memory\Inl\HeapZone\HeapZoneBase.inl"
