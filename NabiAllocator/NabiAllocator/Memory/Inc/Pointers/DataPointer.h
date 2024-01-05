#pragma once

// Nabi Headers
#include "IntegerTypes.h"

/**
 * This struct packs a pointer and 16/8 bits of extra data together in the footprint of a regular pointer.
 * We can do this because many of the high bits from a pointer are not actually used. However, at some point
 * in the future this code *may* break if these bits start being used. But, that probably won't happen any time soon... (until I finish my dis at least :p)
 * 
 * Note:
 *	- If the pointer is 8 byte alligned, then the lowest 3 bits of a pointer could also be used I think (similar to BlockInfo).
 *  - The reason that this struct isn't behind the NA_DATA_POINTER define is because someone may want to use this logic outside of the allocator.
 *    Because, well, the idea is kinda neat! (its not my original idea fyi...!! i first saw it here: https://stackoverflow.com/a/63580741/8890269)
*/

namespace nabi_allocator
{
	template<typename T>
	struct DataPointer final
	{
#ifdef _M_X64
		uPtr m_Ptr : 48;
		uInt m_Data : 16;
#elif _M_IX86
		uPtr m_Ptr : 24;
		uInt m_Data : 8;
#else
#	error "Unsupported architecture"
#endif // ifdef _M_IX86, elif _M_IX86

		inline DataPointer(T const* const ptr);
		inline DataPointer(T const* const ptr, uInt const data);

		[[nodiscard]] inline operator T*() const;
		[[nodiscard]] inline T* operator->() const;
	};

	// The DataPointer *must* be the same size as a normal pointer
	static_assert(sizeof(DataPointer<void>) == sizeof(nullptr));
	static_assert(sizeof(DataPointer<s32>) == sizeof(s32*));
} // namespace nabi_allocator

// Include Inline
#include "Memory\Inl\Pointers\DataPointer.inl"
