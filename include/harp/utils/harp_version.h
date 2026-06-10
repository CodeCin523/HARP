#ifndef UTIL_HARP_VERSION_H
#define UTIL_HARP_VERSION_H

#ifdef __cplusplus
extern "C" {
#endif


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


#ifdef __cplusplus
}
#endif

#endif /* UTIL_HARP_VERSION_H */


#ifdef HARP_UTILS_UNDEF
    #error HARP_UTILS_UNDEF Invalid.
#endif