#ifndef HARP_EXTENDED_HANDLER_H
#define HARP_EXTENDED_HANDLER_H

#include <harp/harp.h>

#include <harp/utils/harp_platform.h>

#if HARP_PLATFORM_WINDOWS
#include <windows.h>
#elif HARP_PLATFORM_LINUX

#endif

typedef struct HarpExtendedHandlerImpl {
    HarpExtendedHandler interface;
#if HARP_PLATFORM_WINDOWS
    LARGE_INTEGER start_time;
    LARGE_INTEGER frequency;
#elif HARP_PLATFORM_LINUX
#endif
} HarpExtendedHandlerImpl;


HarpResult extended_get_uptime_s(const HarpExtendedHandler *h, uint64_t *out_time);
HarpResult extended_get_uptime_ms(const HarpExtendedHandler *h, uint64_t *out_time);
HarpResult extended_get_uptime_ns(const HarpExtendedHandler *h, uint64_t *out_time);


HarpResult init_extended(HarpCoreHandler *core_handler, HarpHandlerBase *base, HarpCreatorBase *creator);
HarpResult term_extended(HarpCoreHandler *core_handler, HarpHandlerBase *base);


#endif /* HARP_EXTENDED_HANDLER_H */