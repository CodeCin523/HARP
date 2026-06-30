#ifndef HARP_CORE_HANDLER_H
#define HARP_CORE_HANDLER_H

#include <harp/harp.h>
#include <harp/utils/harp_platform.h>

#if HARP_PLATFORM_WINDOWS
#include <windows.h>
#elif HARP_PLATFORM_LINUX
#include <time.h>
#endif


typedef struct HarpCoreHandlerImpl {
    HarpCoreHandler interface;
    HarpRuntime *p_runtime;

#if HARP_PLATFORM_WINDOWS
    LARGE_INTEGER start_time;
    LARGE_INTEGER frequency;
#elif HARP_PLATFORM_LINUX
    struct timespec start_time;
#endif
} HarpCoreHandlerImpl;


// Registration
HarpResult core_register_handler(const HarpCoreHandler *h, const HarpHandlerDesc* desc, HarpHandlerBase **out_handler);
HarpResult core_register_actor(const HarpCoreHandler *h, const HarpActorDesc* desc);

// Retrieval
HarpResult core_get_handler(const HarpCoreHandler *h, const HarpDependencyDesc *dependency, HarpHandlerBase** out_handler);

HarpResult core_get_actor_count(const HarpCoreHandler *h, const HarpDependencyDesc *dependency, uint64_t *out_count);
HarpResult core_get_actor_at(const HarpCoreHandler *h, const HarpDependencyDesc *dependency, uint64_t index, HarpActorBase **out_actor);
HarpResult core_get_actors(const HarpCoreHandler *h, const HarpDependencyDesc *dependency, uint64_t *inout_count, HarpActorBase **out_actors);

HarpResult core_get_handler_desc(const HarpCoreHandler *h, const HarpDependencyDesc *dependency, HarpHandlerDesc** out_desc);
HarpResult core_get_actor_desc(const HarpCoreHandler *h, const HarpDependencyDesc *dependency, HarpActorDesc** out_desc);

// Handler
HarpResult core_handler_initialize(const HarpCoreHandler *h, const HarpName name, const HarpCreatorBase* creator);
HarpResult core_handler_terminate(const HarpCoreHandler *h, const HarpName name);

HarpResult core_handler_set_serving(const HarpCoreHandler *h, HarpHandlerBase *base, uint8_t value);
HarpResult core_handler_set_failed(const HarpCoreHandler *h, HarpHandlerBase *base, uint8_t value);

// Actor
HarpResult core_actor_create(const HarpCoreHandler *h, const HarpName name, const HarpCreatorBase* creator, HarpActorBase** out_actor);
HarpResult core_actor_destroy(const HarpCoreHandler *h, const HarpName name, HarpActorBase* actor);

HarpResult core_actor_set_serving(const HarpCoreHandler *h, HarpActorBase *base, uint8_t value);
HarpResult core_actor_set_failed(const HarpCoreHandler *h, HarpActorBase *base, uint8_t value);

// Paths
HarpResult core_get_executable_directory(const HarpCoreHandler *h, const char **out_path);
HarpResult core_get_working_directory(const HarpCoreHandler *h, const char **out_path);
HarpResult core_get_package_directory(const HarpCoreHandler *h, const HarpName name, const char **out_path);

// Time
HarpResult core_get_uptime_s(const HarpCoreHandler *h, uint64_t *out_time);
HarpResult core_get_uptime_ms(const HarpCoreHandler *h, uint64_t *out_time);
HarpResult core_get_uptime_ns(const HarpCoreHandler *h, uint64_t *out_time);


HarpResult init_core_handler(HarpCoreHandler *core_handler, HarpHandlerBase *base, HarpCreatorBase *creator);
HarpResult term_core_handler(HarpCoreHandler *core_handler, HarpHandlerBase *base);


#endif /* HARP_CORE_HANDLER_H */
