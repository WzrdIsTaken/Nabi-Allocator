#pragma once

/**
 * A collection of macros to help create other macros.
*/

#define NA_STR(arg) #arg
#define NA_XSTR(arg) NA_STR(arg)
#define NA_CONCAT(arg1, arg2) arg1 ## arg2
#define NA_EXPAND(arg) arg

#define NA_MACRO_NOT_DEFINED (void(0))
