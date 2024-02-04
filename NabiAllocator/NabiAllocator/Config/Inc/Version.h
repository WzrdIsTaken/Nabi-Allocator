#pragma once

// Nabi Headers
#include "MacroUtils.h"

/**
 * Nabi Allocator's version, used to ensure backwards compatibility.
 * (if I ever update this project once its 'released'...)
*/

#define NA_VERSION_MAJOR 1 // honestly the allocator was finished a while ago, but on 04/02/24 14:39 i finally changed this to "1" :)
#define NA_VERSION_MINOR 0
#define NA_VERSION_PATCH 0

#define NA_VERSION \
    NA_XSTR(NA_VERSION_MAJOR) "." \
    NA_XSTR(NA_VERSION_MINOR) "." \
    NA_XSTR(NA_VERSION_PATCH)
