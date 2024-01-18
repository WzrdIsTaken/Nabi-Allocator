// cpp's Header
#include "MemoryCommand.h"

// STD Headers
#include <optional>
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
			toSet = fallback; \
		} \
		else \
		{ \
			fallback = __VA_ARGS__ current; \
			toSet = __VA_ARGS__ current; \
		} \
	}

#ifdef NA_THREAD_LOCAL_HEAPS
#	define NA_HEAP_ZONE_STORAGE thread_local
#else
	// If this is defined, then HeapZoneScopes, LastHeapZone and LastMemoryTag kinda should be s_... ):
#	define NA_HEAP_ZONE_STORAGE static
#endif // ifdef NA_THREAD_LOCAL_HEAPS

namespace nabi_allocator
{
	NA_HEAP_ZONE_STORAGE std::stack<std::reference_wrapper<HeapZoneScope const>> g_HeapZoneScopes = {};
	NA_HEAP_ZONE_STORAGE HeapZoneBase* g_LastHeapZone = nullptr;
	NA_HEAP_ZONE_STORAGE memoryTag g_LastMemoryTag = c_NullMemoryTag;

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
#ifdef NA_SAFE_ALLOC_FREE_EARLY_OUT
		if (numBytes == 0u) [[unlikely]]
		{
			NA_ASSERT_FAIL("Attempting to allocate 0 bytes");
			return nullptr;
		}
#endif // ifdef NA_SAFE_ALLOC_FREE_EARLY_OUT

		void* memory = nullptr;

		HeapZoneScope const* const topHeapZoneScope = GetTopHeapZoneScope();
		if (topHeapZoneScope) [[likely]]
		{
			HeapZoneBase* topHeapZone = nullptr;
			memoryTag topMemoryTag = c_NullMemoryTag;

			NA_GET_WITH_FALLBACK(topHeapZoneScope->GetHeapZone(), g_LastHeapZone, topHeapZone);
#ifdef NA_MEMORY_TAGGING
			NA_GET_WITH_FALLBACK(topHeapZoneScope->GetMemoryTag(), g_LastMemoryTag, topMemoryTag, *);
#endif // ifdef NA_MEMORY_TAGGING

			if (topHeapZone == &c_UnmanagedHeap)
			{
				goto unmanagedAllocatorAlloc;
			}
			else
			{
				memory = topHeapZone->Allocate(NA_MAKE_ALLOCATION_INFO(numBytes, topMemoryTag));
#ifdef NA_MALLOC_IF_OUT_OF_MEMORY
				// Something went wrong while allocating its probably that the free memory in the heapzone is insufficient
				// for the allocation (but regardless) just try and malloc the memory instead to hopefully prevent a crash.
				if (!memory) [[unlikely]]
				{
					goto unmanagedAllocatorAlloc;
				}
#endif // ifdef NA_MALLOC_IF_OUT_OF_MEMORY
			}
		}
		else [[unlikely]]
		{
		unmanagedAllocatorAlloc:
			memory = m_UnmanagedHeap.Allocate(numBytes);
		}

		return memory;
	}

	void MemoryCommand::Free(void* const memory)
	{
#ifdef NA_SAFE_ALLOC_FREE_EARLY_OUT
		if (!memory) [[unlikely]]
		{
			NA_ASSERT_FAIL("Attempting to free nullptr");
			return;
		}
#endif // ifdef NA_SAFE_ALLOC_FREE_EARLY_OUT

		if (g_LastHeapZone)
		{
			if (g_LastHeapZone == &c_UnmanagedHeap)
			{
				goto unmanagedAllocatorFree;
			}
			else if (g_LastHeapZone->ContainsPtr(memory))
			{
				g_LastHeapZone->Free(memory);
				return;
			}
		}

		{
			HeapZoneBase* const heapZoneWhichMadeAllocation = HeapZoneBase::FindHeapZoneForPtr(memory);
			if (heapZoneWhichMadeAllocation)
			{
				heapZoneWhichMadeAllocation->Free(memory);
				return;
			}
			else
			{
				goto unmanagedAllocatorFree;
			}
		}

	unmanagedAllocatorFree:
		m_UnmanagedHeap.Free(memory);
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
		if (!g_HeapZoneScopes.empty()) [[likely]]
		{
			topHeapZoneScope = &g_HeapZoneScopes.top().get();
		}

		return topHeapZoneScope;
	}

	UnmanagedHeap const& MemoryCommand::GetUnmanagedHeap() const noexcept
	{
		return m_UnmanagedHeap;
	}

	uInt MemoryCommand::GetHeapZoneScopeCount() const noexcept
	{
		return static_cast<uInt>(g_HeapZoneScopes.size());
	}
} // namespace nabi_allocator

#undef NA_GET_WITH_FALLBACK
#undef NA_HEAP_ZONE_STORAGE

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
