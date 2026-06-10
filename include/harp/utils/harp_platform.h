#ifndef UTILS_HARP_PLATFORM_H
#define UTILS_HARP_PLATFORM_H

#ifdef __cplusplus
extern "C" {
#endif


/* ================================================================================ */
/*  PLATFORM                                                                        */
/* ================================================================================ */

#define HARP_PLATFORM_UNKNOWN 0
#define HARP_PLATFORM_WINDOWS 0
#define HARP_PLATFORM_LINUX 0

#if defined(_WIN32) || defined(_WIN64)
    #undef HARP_PLATFORM_WINDOWS
    #define HARP_PLATFORM_WINDOWS 1
#elif defined(__linux__)
    #undef HARP_PLATFORM_LINUX
    #define HARP_PLATFORM_LINUX 1
#else
    #undef HARP_PLATFORM_UNKNOWN
    #define HARP_PLATFORM_UNKNOWN 1
#endif


/* ================================================================================ */
/*  ARCHITECTURE                                                                    */
/* ================================================================================ */

#define HARP_ARCH_UNKNOWN 0
#define HARP_ARCH_X86_64 0
#define HARP_ARCH_X86 0
#define HARP_ARCH_ARM64 0
#define HARP_ARCH_ARM32 0

#if defined(__x86_64__) || defined(_M_X64)
    #undef HARP_ARCH_X86_64
    #define HARP_ARCH_X86_64 1
#elif defined(__i386__) || defined(_M_IX86)
    #undef HARP_ARCH_X86
    #define HARP_ARCH_X86 1
#elif defined(__aarch64__) || defined(_M_ARM64)
    #undef HARP_ARCH_ARM64
    #define HARP_ARCH_ARM64 1
#elif defined(__arm__) || defined(_M_ARM)
    #undef HARP_ARCH_ARM32
    #define HARP_ARCH_ARM32 1
#else
    #undef HARP_ARCH_UNKNOWN
    #define HARP_ARCH_UNKNOWN 1
#endif


#ifdef __cplusplus
}
#endif

#endif /* UTILS_HARP_PLATFORM_H */


#ifdef HARP_UTILS_UNDEF
    #error HARP_UTILS_UNDEF Invalid.
#endif
