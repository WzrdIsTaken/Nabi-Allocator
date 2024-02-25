# Nabi-Allocator

NabiAllocator is a fast, easy to use, single header C++ memory allocator. It can effectively be copy-pasted into a project and initialization only takes a few lines. 

```cpp
#include "!NabiAllocator.h"

// Before your application's main loop starts
using namespace nabi_allocator;
HeapZone<DefaultFreeListAllocator> heapZone = { HeapZoneBase::c_NoParent, 1_GB , "HeapZone" };
NA_SET_HEAP_ZONE_SCOPE(&heapZone, c_NullMemoryTag);
```

It is: 
* Fast: applications have been recorded to perform up to 12% quicker vs the standard `new` and `delete` implementation.
* Simple to use: once you have initialized it, you can forget about it.
* Safe: the allocator flags memory issues such as leaks the moment they happen.

For more information and to see if NabiAllocator is right for your project, check out the [wiki](https://github.com/WzrdIsTaken/Nabi-Allocator/wiki). 
