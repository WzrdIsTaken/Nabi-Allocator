// cpp's Header
#include "MemoryCommand.h"

// STD Headers
#include <optional>

// Config Headers
#include "Config.h"

// Nabi Headers
#include "AllocationInfo.h"
#include "DebugUtils.h"
#include "HeapZone\HeapZoneBase.h"
#include "HeapZone\HeapZoneScope.h"

#define NA_GET_WITH_FALLBACK(current, fallback, toSet, ...) /*4/4 macro*/\
	{ \
		if (!current) \
		{ \
			NA_ASSERT(fallback, "Can't fallback if its never been set"); \
			toSet = fallback; \
		} \
		else \
		{ \
			fallback = __VA_ARGS__ current; \
			toSet = __VA_ARGS__ current; \
		} \
	}

namespace nabi_allocator
{
	// TODO should it be thread local? IF NOT it should be s_HeapZoneScopes . TODO add why we do this to the notes section
	static std::stack<std::reference_wrapper<HeapZoneScope const>> g_HeapZoneScopes = {}; // See Notes section for why we have to do this and note why its thread local

	MemoryCommand::MemoryCommand()
		: m_LastHeapZone(nullptr)
		, m_LastMemoryTag(c_NullMemoryTag)
	{
	}

	MemoryCommand::~MemoryCommand()
	{
		NA_ASSERT(g_HeapZoneScopes.empty(),
			"MemoryCommand's destructor called but there are still HeapZoneScopes on the stack");
	}

	void* MemoryCommand::Allocate(uInt const numBytes)
	{
		void* memory = nullptr;

		HeapZoneScope const* const topHeapZoneScope = GetTopHeapZoneScope();
		if (topHeapZoneScope)
		{
			HeapZoneBase* topHeapZone = nullptr;
			memoryTag topMemoryTag = c_NullMemoryTag;

			NA_GET_WITH_FALLBACK(topHeapZoneScope->GetHeapZone(), m_LastHeapZone, topHeapZone);
#ifdef NA_MEMORY_TAGGING
			NA_GET_WITH_FALLBACK(topHeapZoneScope->GetMemoryTag(), m_LastMemoryTag, topMemoryTag, *);
#endif // ifdef NA_MEMORY_TAGGING

			memory = topHeapZone->Allocate(NA_MAKE_ALLOCATION_INFO(numBytes, topMemoryTag));
		}
		else
		{
			memory = std::malloc(static_cast<std::size_t>(numBytes));
		}

		return memory;
	}

	void MemoryCommand::Free(void* const memory)
	{
		if (m_LastHeapZone)
		{
			if (HeapZoneBase::ContainsPtr(*m_LastHeapZone, memory))
			{
				m_LastHeapZone->Free(memory);
				return;
			}
		}
		
		HeapZoneBase* heapZoneWhichMadeAllocation = HeapZoneBase::FindHeapZoneForPtr(memory);
		if (heapZoneWhichMadeAllocation)
		{
			heapZoneWhichMadeAllocation->Free(memory);
		}
		else
		{
			std::free(memory);
		}
	}

	void MemoryCommand::PushHeapZoneScope(HeapZoneScope const& heapZoneScope)
	{
		g_HeapZoneScopes.push(heapZoneScope);
	}

	void MemoryCommand::PopHeapZoneScope(HeapZoneScope const& heapZoneScope)
	{
#ifdef NA_DEBUG
		HeapZoneScope const* const topHeapZoneScope = GetTopHeapZoneScope();
		NA_ASSERT(topHeapZoneScope, "Trying to pop a heap zone scope, but the stack is empty");

		if (topHeapZoneScope)
		{
			NA_ASSERT(topHeapZoneScope->GetHeapZone() == heapZoneScope.GetHeapZone(),
				"Popping a heap zone scope which isn't at the top of the stack");
		}
#endif // ifdef NA_DEBUG

		g_HeapZoneScopes.pop();
		if (g_HeapZoneScopes.empty())
		{
			// TODO is the last heap zone ever not the top heap zone? do we need this?
			// what is last heap zone even doing> [ok ik what its doing but could we do another way no i dont thinkso?]
			// TODO is the if checks we have if allocate/free the best way we've got of doing things? w/ std malloc and free

			m_LastHeapZone = nullptr;
			m_LastMemoryTag = c_NullMemoryTag;
		}
	}

	HeapZoneScope const* const MemoryCommand::GetTopHeapZoneScope() const noexcept
	{
		if (!g_HeapZoneScopes.empty()) [[likely]]
		{
			return &g_HeapZoneScopes.top().get();
		}
		else [[unlikely]]
		{
			return nullptr;
		}

		return nullptr;
	}
} // namespace nabi_allocator

#undef GET_WITH_FALLBACK

/*
	* Do we want something like this..?
	*  On the one hand, its adds some new functionality. 
	*  On the other, when would we ever want to do this? And we would have to add a bunch more checks.
	*	 
		void MemoryCommand::Reset()
		{
			g_HeapZoneScopes = {};
		}
*/
