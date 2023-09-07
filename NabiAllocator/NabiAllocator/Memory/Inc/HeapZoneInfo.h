#pragma once

// Nabi Headers
#include "IntegerTypes.h"

/**
 * Stores the start and end pointers of a HeapZone. From this infomation, the size 
 * of a HeapZone and whether a HeapZone is initialized can be obtained. 
*/

namespace nabi_allocator
{
	struct HeapZoneInfo final
	{
		uPtr m_Start;
		uPtr m_End;
	};
} // namespace nabi_allocator
