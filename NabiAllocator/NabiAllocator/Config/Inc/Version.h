#pragma once

// Nabi Headers
#include "MacroUtils.h"

/**
 * Nabi Allocator's version, used to ensure backwards compatibility.
 * (if I ever update this project once its 'released'...)
*/

#define NABI_ALLOCATOR_VERSION_MAJOR 0
#define NABI_ALLOCATOR_VERSION_MINOR 0
#define NABI_ALLOCATOR_VERSION_PATCH 0

#define NABI_ALLOCATOR_VERSION \
    NABI_ALLOCATOR_XSTR(NABI_ALLOCATOR_VERSION_MAJOR) "." \
    NABI_ALLOCATOR_XSTR(NABI_ALLOCATOR_VERSION_MINOR) "." \
    NABI_ALLOCATOR_XSTR(NABI_ALLOCATOR_VERSION_PATCH)
