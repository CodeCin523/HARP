#ifndef UTILS_HARP_BUILD_H
#define UTILS_HARP_BUILD_H

#ifdef __cplusplus
extern "C" {
#endif


/* ================================================================================ */
/*  MACROS                                                                          */
/* ================================================================================ */

#define HARP_DEBUG   0
#define HARP_RELEASE 0

#ifdef NDEBUG
    #undef HARP_RELEASE
    #define HARP_RELEASE 1
#else
    #undef HARP_DEBUG
    #define HARP_DEBUG 1
#endif



#ifdef __cplusplus
}
#endif

#endif /* UTILS_HARP_BUILD_H */