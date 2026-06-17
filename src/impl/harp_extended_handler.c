#include "harp_extended_handler.h"


#if HARP_PLATFORM_WINDOWS

HarpResult extended_get_uptime_s(const HarpExtendedHandler *h, uint64_t *out_time) {
    if(h == NULL || out_time == NULL)
        return HARP_RESULT_INVALID_ARGUMENTS;

    HarpExtendedHandlerImpl *handler = (HarpExtendedHandlerImpl *)h;

    LARGE_INTEGER now;
    QueryPerformanceCounter(&now);

    uint64_t elapsed_ticks = (uint64_t)(now.QuadPart - handler->start_time.QuadPart);
    *out_time = elapsed_ticks / (uint64_t)handler->frequency.QuadPart;

    return HARP_RESULT_OK;
}
HarpResult extended_get_uptime_ms(const HarpExtendedHandler *h, uint64_t *out_time) {
    if(h == NULL || out_time == NULL)
        return HARP_RESULT_INVALID_ARGUMENTS;

    HarpExtendedHandlerImpl *handler = (HarpExtendedHandlerImpl *)h;

    LARGE_INTEGER now;
    QueryPerformanceCounter(&now);

    uint64_t elapsed_ticks = (uint64_t)(now.QuadPart - handler->start_time.QuadPart);
    *out_time = (elapsed_ticks * 1000ULL) / (uint64_t)handler->frequency.QuadPart;

    return HARP_RESULT_OK;
}
HarpResult extended_get_uptime_ns(const HarpExtendedHandler *h, uint64_t *out_time) {
    if(h == NULL || out_time == NULL)
        return HARP_RESULT_INVALID_ARGUMENTS;

    HarpExtendedHandlerImpl *handler = (HarpExtendedHandlerImpl *)h;

    LARGE_INTEGER now;
    QueryPerformanceCounter(&now);

    uint64_t elapsed_ticks = (uint64_t)(now.QuadPart - handler->start_time.QuadPart);
    *out_time = (elapsed_ticks * 1000000000ULL) / (uint64_t)handler->frequency.QuadPart;

    return HARP_RESULT_OK;
}

#elif HARP_PLATFORM_LINUX

HarpResult extended_get_uptime_s(const HarpExtendedHandler *h, uint64_t *out_time) {
    return HARP_RESULT_CRITICAL_FAIL;
}
HarpResult extended_get_uptime_ms(const HarpExtendedHandler *h, uint64_t *out_time) {
    return HARP_RESULT_CRITICAL_FAIL;
}
HarpResult extended_get_uptime_ns(const HarpExtendedHandler *h, uint64_t *out_time) {
    return HARP_RESULT_CRITICAL_FAIL;
}

#endif


/* ================================================================================ */
/*  EXTENDED HANDLER                                                                */
/* ================================================================================ */

HarpResult init_extended(HarpCoreHandler *core_handler, HarpHandlerBase *base, HarpCreatorBase *creator) {
    (void)core_handler;
    (void)creator;

    HarpExtendedHandlerImpl *handler = (HarpExtendedHandlerImpl *)base;

#if HARP_PLATFORM_WINDOWS
    QueryPerformanceFrequency(&handler->frequency);
    QueryPerformanceCounter(&handler->start_time);
#elif HARP_PLATFORM_LINUX
#endif

    return HARP_RESULT_OK;
}
HarpResult term_extended(HarpCoreHandler *core_handler, HarpHandlerBase *base) {
    (void)core_handler;
    (void)base;

    return HARP_RESULT_OK;
}