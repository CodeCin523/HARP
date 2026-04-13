#ifndef UTIL_HARP_VERSION_H
#define UTIL_HARP_VERSION_H

#endif /* UTIL_HARP_VERSION_H */


/* ================================================================================ */
/*  MACROS                                                                          */
/* ================================================================================ */

#define HARP_MAKE_VERSION(major, minor, patch) \
    ((((uint32_t)(major) & 0xFF) << 24) | \
     (((uint32_t)(minor) & 0xFF) << 16) | \
     ((uint32_t)(patch) & 0xFFFF))

#define HARP_VERSION_MAJOR(v)   (((v) >> 24) & 0xFF)
#define HARP_VERSION_MINOR(v)   (((v) >> 16) & 0xFF)
#define HARP_VERSION_PATCH(v)   ((v) & 0xFFFF)

#define HARP_VERSION_EQUAL(v1, v2)  ((v1) == (v2))
#define HARP_VERSION_MAJOR_EQUAL(v1, v2) (HARP_VERSION_MAJOR(v1) == HARP_VERSION_MAJOR(v2))
#define HARP_VERSION_MINOR_EQUAL(v1, v2) (HARP_VERSION_MINOR(v1) == HARP_VERSION_MINOR(v2))
#define HARP_VERSION_PATCH_EQUAL(v1, v2) (HARP_VERSION_PATCH(v1) == HARP_VERSION_PATCH(v2))


/* ================================================================================ */
/*  UTILS_UNDEF                                                                     */
/* ================================================================================ */

#ifdef  HARP_UTILS_UNDEF

#undef HARP_MAKE_VERSION

#undef HARP_VERSION_MAJOR
#undef HARP_VERSION_MINOR
#undef HARP_VERSION_PATCH

#undef HARP_VERSION_EQUAL
#undef HARP_VERSION_MAJOR_EQUAL
#undef HARP_VERSION_MINOR_EQUAL

#endif /* HARP_UTILS_UNDEF */