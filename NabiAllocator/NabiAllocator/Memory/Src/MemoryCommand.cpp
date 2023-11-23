// cpp's Header
#include "MemoryCommand.h"

// STD Headers
#include <stack>
#include <type_traits>

// Config Headers
#include "Config.h"

// Nabi Headers
#include "AllocationInfo.h"
#include "DebugUtils.h"
#include "HeapZone\HeapZoneBase.h"
#include "HeapZone\HeapZoneScope.h"
#include "MemoryConstants.h"

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
	thread_local std::stack<std::reference_wrapper<HeapZoneScope const>> g_HeapZoneScopes = {};
	thread_local HeapZoneBase* g_LastHeapZone = nullptr;
	thread_local memoryTag g_LastMemoryTag = c_NullMemoryTag;

	MemoryCommand::MemoryCommand()
		: m_UnmanagedHeap{}
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

			NA_GET_WITH_FALLBACK(topHeapZoneScope->GetHeapZone(), g_LastHeapZone, topHeapZone);
#ifdef NA_MEMORY_TAGGING
			NA_GET_WITH_FALLBACK(topHeapZoneScope->GetMemoryTag(), g_LastMemoryTag, topMemoryTag, *);
#endif // ifdef NA_MEMORY_TAGGING

			memory = topHeapZone->Allocate(NA_MAKE_ALLOCATION_INFO(numBytes, topMemoryTag));
		}
		else
		{
			memory = m_UnmanagedHeap.Allocate(numBytes);
		}

		return memory;
	}

	void MemoryCommand::Free(void* const memory)
	{
		if (g_LastHeapZone)
		{
			if (g_LastHeapZone->ContainsPtr(memory))
			{
				g_LastHeapZone->Free(memory);
			}
			else
			{
				HeapZoneBase* const heapZoneWhichMadeAllocation = HeapZoneBase::FindHeapZoneForPtr(memory);
				if (heapZoneWhichMadeAllocation)
				{
					heapZoneWhichMadeAllocation->Free(memory);
				}
			}
		}
		else
		{
			m_UnmanagedHeap.Free(memory);
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
			g_LastHeapZone = nullptr;
			g_LastMemoryTag = c_NullMemoryTag;
		}
	}

	HeapZoneScope const* const MemoryCommand::GetTopHeapZoneScope() const noexcept
	{
		HeapZoneScope const* topHeapZoneScope = nullptr;
		if (!g_HeapZoneScopes.empty())
		{
			topHeapZoneScope = &g_HeapZoneScopes.top().get();
		}

		return topHeapZoneScope;
	}

	UnmanagedHeap const& MemoryCommand::GetUnmanagedHeap() const noexcept
	{
		return m_UnmanagedHeap;
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
