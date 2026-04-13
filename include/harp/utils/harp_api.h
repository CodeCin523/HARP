#ifndef UTILS_HARP_API_H
#define UTILS_HARP_API_H

#include "harp_version.h"

#endif /* UTILS_HARP_API_H */


/* ================================================================================ */
/*  MACROS                                                                          */
/* ================================================================================ */

// Get base API pointer from any derived API type (base must be first member)
#define HARP_API_GET_BASE(api_ptr) ((HarpApiBase*)(api_ptr))

// Cast API pointer to a specific API type (no safety checks)
#define HARP_API_AS(type, api_ptr) ((type*)(api_ptr))

// Check if API pointer is valid and currently available
#define HARP_API_AVAILABLE(api_ptr) \
    ((api_ptr) != NULL && HARP_API_GET_BASE(api_ptr)->available)

// Get full version from API (valid for base or derived)
#define HARP_API_VERSION(api_ptr) (HARP_API_GET_BASE(api_ptr)->version)

// Extract version components from API
#define HARP_API_MAJOR(api_ptr) (HARP_VERSION_MAJOR(HARP_API_VERSION(api_ptr)))
#define HARP_API_MINOR(api_ptr) (HARP_VERSION_MINOR(HARP_API_VERSION(api_ptr)))
#define HARP_API_PATCH(api_ptr) (HARP_VERSION_PATCH(HARP_API_VERSION(api_ptr)))

// Hint: API may become unavailable after this check.
// Use only for short, non-blocking operations.
#define HARP_API_WITH(api_ptr) \
    if ((api_ptr) && HARP_API_GET_BASE(api_ptr)->available)

// Require API to be valid and available (use in explicit checks)
#define HARP_API_REQUIRE(api_ptr) \
    ((api_ptr) != NULL && HARP_API_GET_BASE(api_ptr)->available)


/* ================================================================================ */
/*  UTILS_UNDEF                                                                     */
/* ================================================================================ */

#ifdef  HARP_UTILS_UNDEF

#undef HARP_API_GET_BASE
#undef HARP_API_AS

#undef HARP_API_AVAILABLE

#undef HARP_API_VERSION
#undef HARP_API_MAJOR
#undef HARP_API_MINOR
#undef HARP_API_PATCH

#undef HARP_API_WITH
#undef HARP_API_REQUIRE

#endif /* HARP_UTILS_UNDEF */