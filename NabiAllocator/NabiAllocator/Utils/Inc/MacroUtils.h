#pragma once

/**
 * A collection of macros to help create other macros.
*/

#define NABI_ALLOCATOR_STR(arg) #arg
#define NABI_ALLOCATOR_XSTR(arg) NABI_ALLOCATOR_STR(arg)
#define NABI_ALLOCATOR_CONCAT(arg1, arg2) arg1 ## arg2
#define NABI_ALLOCATOR_EXPAND(arg) arg

#define NABI_ALLOCATOR_MACRO_NOT_DEFINED (void(0))
