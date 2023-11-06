// STD Headers
#include <optional>

// Config Headers
#include "Config.h"

// Nabi Headers
#include "AllocationInfo.h"
#include "DebugUtils.h"
#include "HeapZone\HeapZone.h"
#include "HeapZone\HeapZoneBase.h"
#include "HeapZone\HeapZoneScope.h"
#include "IntegerTypes.h"
#include "MemoryCommand.h"
#include "MemoryConstants.h"

/**
 * TODO desc 
*/

using namespace nabi_allocator;

// TODO - we should have a header/cpp called like NewDeleteLogic which new/delete call into.
// then the stuff is testabable.
// New and delete should only have one line on code inside them, to these functions.
// Then the FullWorkFlow.cpp can call into them

template<typename T>
[[nodiscard]] inline T* GetWithFallback(T const* current, T const* fallback)
{
    if (!current)
    {
        NA_ASSERT(fallback, "Can't fallback if its never been set");
        current = fallback;
    }

    fallback = current;
    return current;
}

/*
[[nodiscard]] void* operator new(uInt numBytes) noexcept(false)
{
   
    static HeapZoneBase* lastHeapZone = nullptr;
    static std::optional<memoryTag const> lastMemoryTag = std::nullopt;

    HeapZoneScope const* const heapZoneScope = MemoryCommand::Instance()->GetTopHeapZoneScope();
    NA_ASSERT(heapZoneScope, "new called but no heap zone scopes have been pushed yet");

    HeapZoneBase* const heapZone = GetWithFallback(heapZoneScope->GetHeapZone(), lastHeapZone);
#ifdef NA_MEMORY_TAGGING
    std::optional<memoryTag const> const memoryTag = *GetWithFallback(&heapZoneScope->GetMemoryTag(), &lastMemoryTag); // TODO this is jank
#endif // ifdef NA_MEMORY_TAGGING

    return heapZone->Allocate(NA_MAKE_ALLOCATION_INFO(numBytes, memoryTag.value()));
  
}

void operator delete(void* memory) noexcept
{
    // TODOs
    // see https://github.com/ScrewjankGames/ScrewjankEngine/blob/master/Source/Runtime/src/system/memory/Memory.cpp
    // we need a way to find which heap zone owns memory
    // also we need some asserts eg if top heap zope scope doesnt exist
    
    // my idea is that heap zone scope owns the tag. it can be passed down to the allocator? rather than just accessing with the singleton..?
    // ctrl + f TODO

    // ALSO dont forget to override array new/delete https://stackoverflow.com/questions/8186018/how-to-properly-replace-global-new-delete-operators
}
*/
