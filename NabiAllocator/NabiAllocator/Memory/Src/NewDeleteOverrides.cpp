// STD Headers
#include <new>

// Config Headers
#include "Config.h"

// Nabi Headers
#include "IntegerTypes.h"
#include "MemoryCommand.h"

/**
 * The global new/delete overrides. If NA_OVERRIDE_NEW_DELETE isn't defined they will just default to std::malloc/std::free.
 * 
 * Note:
 *  - I was getting some warnings about inconsistent annotatings with my new override. I tried this 
 *    https://stackoverflow.com/q/68569843/8890269 but it didn't quite work (perhaps I just wasn't
 *    doing it right...) so just opted to disable the warnings instead :p.
*/

namespace nabi_allocator
{
    [[nodiscard]] inline void* New(std::size_t const numBytes) noexcept(false)
    {
#ifdef NA_OVERRIDE_NEW_DELETE
        return MemoryCommand::Instance().Allocate(static_cast<uInt>(numBytes));
#else 
        return std::malloc(numBytes);
#endif // ifdef NA_OVERRIDE_NEW_DELETE
    }

    inline void Delete(void* const memory) noexcept
    {
#ifdef NA_OVERRIDE_NEW_DELETE
        return MemoryCommand::Instance().Free(memory);
#else 
        std::free(memory);
#endif // ifdef NA_OVERRIDE_NEW_DELETE
    }
} // namespace nabi_allocator

#pragma warning(disable: 28196; disable: 28251; disable: 6387)

    [[nodiscard]] void* operator new(std::size_t const numBytes) noexcept(false)
    {
        return nabi_allocator::New(numBytes);
    }

    [[nodiscard]] void* operator new[](std::size_t const numBytes) noexcept(false)
    {
        return nabi_allocator::New(numBytes);
    }

#pragma warning(default: 28196; default: 28251; default: 6387)

void operator delete(void* const memory) noexcept
{
    nabi_allocator::Delete(memory);
}

void operator delete[](void* const memory) noexcept
{
    nabi_allocator::Delete(memory);
}
