#ifndef UTILS_HARP_API_H
#define UTILS_HARP_API_H

#include "harp_version.h"

#endif /* UTILS_HARP_API_H */


/* ================================================================================ */
/*  MACROS                                                                          */
/* ================================================================================ */

// Get base API pointer from any derived API type
// (HarpApiBase must be the first member)
#define HARP_API_GET_BASE(api_ptr) \
    ((HarpApiBase*)(api_ptr))

// Cast API pointer to a specific API type (no safety checks)
#define HARP_API_AS(type, api_ptr) \
    ((type*)(api_ptr))

// Get owning handler from API
#define HARP_API_HANDLER(api_ptr) \
    (HARP_API_GET_BASE(api_ptr)->p_handler)

// Cast owning handler to a specific handler type
#define HARP_API_HANDLER_AS(type, api_ptr) \
    ((type*)HARP_API_HANDLER(api_ptr))

// Check if API pointer is valid and currently available
#define HARP_API_AVAILABLE(api_ptr) \
    ((api_ptr) != NULL && HARP_API_GET_BASE(api_ptr)->available)

// Check if API has a valid owning handler
#define HARP_API_HAS_HANDLER(api_ptr) \
    (HARP_API_HANDLER(api_ptr) != NULL)

// Get full version from API
#define HARP_API_VERSION(api_ptr) \
    (HARP_API_GET_BASE(api_ptr)->version)

// Extract version components from API
#define HARP_API_MAJOR(api_ptr) \
    (HARP_VERSION_MAJOR(HARP_API_VERSION(api_ptr)))

#define HARP_API_MINOR(api_ptr) \
    (HARP_VERSION_MINOR(HARP_API_VERSION(api_ptr)))

#define HARP_API_PATCH(api_ptr) \
    (HARP_VERSION_PATCH(HARP_API_VERSION(api_ptr)))

// Hint: API may become unavailable after this check.
// Use only for short, non-blocking operations.
#define HARP_API_WITH(api_ptr) \
    if ((api_ptr) && HARP_API_GET_BASE(api_ptr)->available)

// Require API to be valid and available
#define HARP_API_REQUIRE(api_ptr) \
    ((api_ptr) != NULL && HARP_API_GET_BASE(api_ptr)->available)

// Require API to have a valid handler
#define HARP_API_REQUIRE_HANDLER(api_ptr) \
    ((api_ptr) != NULL && HARP_API_HANDLER(api_ptr) != NULL)


/* ================================================================================ */
/*  UTILS_UNDEF                                                                     */
/* ================================================================================ */

#ifdef HARP_UTILS_UNDEF

#undef HARP_API_GET_BASE
#undef HARP_API_AS

#undef HARP_API_HANDLER
#undef HARP_API_HANDLER_AS

#undef HARP_API_AVAILABLE
#undef HARP_API_HAS_HANDLER

#undef HARP_API_VERSION
#undef HARP_API_MAJOR
#undef HARP_API_MINOR
#undef HARP_API_PATCH

#undef HARP_API_WITH

#undef HARP_API_REQUIRE
#undef HARP_API_REQUIRE_HANDLER

#endif /* HARP_UTILS_UNDEF */