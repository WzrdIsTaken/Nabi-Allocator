// STD Headers
#include <new>

// Config Headers
#include "Config.h"

// Nabi Headers
#include "IntegerTypes.h"
#include "MemoryCommand.h"

/**
 * The global new/delete overrides.
 * 
 * Note:
 *  - I was getting some warnings about inconsistent annotations with my new override. I tried this 
 *    https://stackoverflow.com/q/68569843/8890269 but it didn't quite work (perhaps I just wasn't
 *    doing it right...) so just opted to disable the warnings instead :p.
 *  - The reason we don't just default to malloc/free but still override the operators if NA_OVERR..
 *    isn't defined is because we don't want to change the default behaviour *at all*. Furthermore,
 *    a user may have a different new/delete override they are using. 
*/

namespace nabi_allocator
{
    [[nodiscard]] inline void* New(std::size_t const numBytes) noexcept(false)
    {
        return MemoryCommand::Instance().Allocate(static_cast<uInt>(numBytes));
    }

    inline void Delete(void* const memory) noexcept
    {
        return MemoryCommand::Instance().Free(memory);
    }
} // namespace nabi_allocator

#ifdef NA_OVERRIDE_NEW_DELETE
#   pragma warning(disable: 28196; disable: 28251; disable: 6387)

        [[nodiscard]] void* operator new(std::size_t const numBytes) noexcept(false)
        {
            return nabi_allocator::New(numBytes);
        }

        [[nodiscard]] void* operator new[](std::size_t const numBytes) noexcept(false)
        {
            return nabi_allocator::New(numBytes);
        }

#   pragma warning(default: 28196; default: 28251; default: 6387)

    void operator delete(void* const memory) noexcept
    {
        nabi_allocator::Delete(memory);
    }

    void operator delete[](void* const memory) noexcept
    {
        nabi_allocator::Delete(memory);
    }
#endif // ifdef NA_OVERRIDE_NEW_DELETE
