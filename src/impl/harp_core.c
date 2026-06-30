#include "harp_core.h"
#include "runtime/harp_runtime.h"
#include "utils.h"

#include <harp/utils/harp_helpers.h>

#include <hmem/hmem_block.h>
#include <hmem/hmem_os.h>
#include <hmem/utils/hmem_align.h>

#include <string.h>
#include <stdalign.h>


/* ================================================================================ */
/*  REGISTRATION                                                                    */
/* ================================================================================ */

HarpResult core_register_handler(const HarpCoreHandler *h, const HarpHandlerDesc *desc, HarpHandlerBase **out_handler) {
    HARP_CHECK_ARG(utils_check_core_handler(h) == HARP_RESULT_OK, HARP_RESULT_INVALID_ARGUMENTS);
    HARP_CHECK_ARG(utils_check_handler_desc(desc) == HARP_RESULT_OK, HARP_RESULT_INVALID_ARGUMENTS);

    HarpRuntime *runtime = ((HarpCoreHandlerImpl *)h)->p_runtime;
    HARP_CHECK_CRITICAL(runtime != NULL);

    return harp_runtime_register_handler(runtime, desc, out_handler);
}
HarpResult core_register_actor(const HarpCoreHandler *h, const HarpActorDesc *desc) {
    HARP_CHECK_ARG(utils_check_core_handler(h) == HARP_RESULT_OK, HARP_RESULT_INVALID_ARGUMENTS);
    HARP_CHECK_ARG(utils_check_actor_desc(desc) == HARP_RESULT_OK, HARP_RESULT_INVALID_ARGUMENTS);

    HarpRuntime *runtime = ((HarpCoreHandlerImpl *)h)->p_runtime;
    HARP_CHECK_CRITICAL(runtime != NULL);

    return harp_runtime_register_actor(runtime, desc);
}


/* ================================================================================ */
/*  RETRIEVAL                                                                       */
/* ================================================================================ */

HarpResult core_get_handler(const HarpCoreHandler *h, const HarpDependencyDesc *dependency, HarpHandlerBase** out_handler) {
    HARP_CHECK_ARG(utils_check_core_handler(h) == HARP_RESULT_OK, HARP_RESULT_INVALID_ARGUMENTS);
    HARP_CHECK_ARG(utils_check_dependency(dependency) == HARP_RESULT_OK, HARP_RESULT_INVALID_ARGUMENTS);
    HARP_CHECK_ARG(out_handler != NULL, HARP_RESULT_MISSING_OUTPUT);

    HarpRuntime *runtime = ((HarpCoreHandlerImpl *)h)->p_runtime;
    HARP_CHECK_CRITICAL(runtime != NULL);

    // retrieve
    HarpRuntimeHandler *rhdl =
        harp_registry_get_runtime(&runtime->registry, dependency->name, HARP_REGISTRY_ENTRY_TYPE_HANDLER);
    
    if(rhdl == NULL)
        return HARP_RESULT_NAME_NOT_FOUND;
    if(utils_dependency_matches(dependency, rhdl->descriptor.version) != HARP_RESULT_OK)
        return HARP_RESULT_DEPENDENCY_VERSION_MISMATCH;

    HARP_CHECK_CRITICAL(rhdl->instance != NULL);

    // return
    *out_handler = rhdl->instance;
    return HARP_RESULT_OK;
}

HarpResult core_get_actor_count(const HarpCoreHandler *h, const HarpDependencyDesc *dependency, uint64_t *out_count) {
    HARP_CHECK_ARG(utils_check_core_handler(h) == HARP_RESULT_OK, HARP_RESULT_INVALID_ARGUMENTS);
    HARP_CHECK_ARG(utils_check_dependency(dependency) == HARP_RESULT_OK, HARP_RESULT_INVALID_ARGUMENTS);
    HARP_CHECK_ARG(out_count != NULL, HARP_RESULT_MISSING_OUTPUT);

    HarpRuntime *runtime = ((HarpCoreHandlerImpl *)h)->p_runtime;
    HARP_CHECK_CRITICAL(runtime != NULL);

    // retrieve
    HarpRuntimeActor *ract =
        harp_registry_get_runtime(&runtime->registry, dependency->name, HARP_REGISTRY_ENTRY_TYPE_ACTOR);

    if(ract == NULL)
        return HARP_RESULT_NAME_NOT_FOUND;
    if(utils_dependency_matches(dependency, ract->descriptor.version) != HARP_RESULT_OK)
        return HARP_RESULT_DEPENDENCY_VERSION_MISMATCH;

    // return
    *out_count = ract->actor_count;
    return HARP_RESULT_OK;
}
HarpResult core_get_actor_at(const HarpCoreHandler *h, const HarpDependencyDesc *dependency, uint64_t index, HarpActorBase **out_actor) {
    HARP_CHECK_ARG(utils_check_core_handler(h) == HARP_RESULT_OK, HARP_RESULT_INVALID_ARGUMENTS);
    HARP_CHECK_ARG(utils_check_dependency(dependency) == HARP_RESULT_OK, HARP_RESULT_INVALID_ARGUMENTS);
    HARP_CHECK_ARG(out_actor != NULL, HARP_RESULT_MISSING_OUTPUT);

    HarpRuntime *runtime = ((HarpCoreHandlerImpl *)h)->p_runtime;
    HARP_CHECK_CRITICAL(runtime != NULL);

    // retrieve
    HarpRuntimeActor *ract =
        harp_registry_get_runtime(&runtime->registry, dependency->name, HARP_REGISTRY_ENTRY_TYPE_ACTOR);

    if(ract == NULL)
        return HARP_RESULT_NAME_NOT_FOUND;
    if(utils_dependency_matches(dependency, ract->descriptor.version) != HARP_RESULT_OK)
        return HARP_RESULT_DEPENDENCY_VERSION_MISMATCH;

    // return
    if(index >= ract->actor_count)
        return HARP_RESULT_INVALID_ARGUMENTS;

    *out_actor = ract->actors[index];
    return HARP_RESULT_OK;
}
HarpResult core_get_actors(const HarpCoreHandler *h, const HarpDependencyDesc *dependency, uint64_t *inout_count, HarpActorBase **out_actors) {
    HARP_CHECK_ARG(utils_check_core_handler(h) == HARP_RESULT_OK, HARP_RESULT_INVALID_ARGUMENTS);
    HARP_CHECK_ARG(utils_check_dependency(dependency) == HARP_RESULT_OK, HARP_RESULT_INVALID_ARGUMENTS);
    HARP_CHECK_ARG(inout_count != NULL, HARP_RESULT_MISSING_OUTPUT);

    HarpRuntime *runtime = ((HarpCoreHandlerImpl *)h)->p_runtime;
    HARP_CHECK_CRITICAL(runtime != NULL);

    // retrieve
    HarpRuntimeActor *ract =
        harp_registry_get_runtime(&runtime->registry, dependency->name, HARP_REGISTRY_ENTRY_TYPE_ACTOR);

    if(ract == NULL)
        return HARP_RESULT_NAME_NOT_FOUND;
    if(utils_dependency_matches(dependency, ract->descriptor.version) != HARP_RESULT_OK)
        return HARP_RESULT_DEPENDENCY_VERSION_MISMATCH;

    // return
    if(out_actors == NULL) {
        *inout_count = ract->actor_count;
    } else {
        uint64_t count = *inout_count;
            
        if(count > ract->actor_count)
            count = ract->actor_count;
            
        for(uint64_t i = 0; i < count; ++i)
            out_actors[i] = ract->actors[i];
        
        *inout_count = count;
    }

    return HARP_RESULT_OK;
}

HarpResult core_get_handler_desc(const HarpCoreHandler *h, const HarpDependencyDesc *dependency, HarpHandlerDesc** out_desc) {
    HARP_CHECK_ARG(utils_check_core_handler(h) == HARP_RESULT_OK, HARP_RESULT_INVALID_ARGUMENTS);
    HARP_CHECK_ARG(utils_check_dependency(dependency) == HARP_RESULT_OK, HARP_RESULT_INVALID_ARGUMENTS);
    HARP_CHECK_ARG(out_desc != NULL, HARP_RESULT_MISSING_OUTPUT);

    HarpRuntime *runtime = ((HarpCoreHandlerImpl *)h)->p_runtime;
    HARP_CHECK_CRITICAL(runtime != NULL);

    // retrieve
    HarpRuntimeHandler *rhdl =
        harp_registry_get_runtime(&runtime->registry, dependency->name, HARP_REGISTRY_ENTRY_TYPE_HANDLER);
    
    if(rhdl == NULL)
        return HARP_RESULT_NAME_NOT_FOUND;
    if(utils_dependency_matches(dependency, rhdl->descriptor.version) != HARP_RESULT_OK)
        return HARP_RESULT_DEPENDENCY_VERSION_MISMATCH;

    HARP_CHECK_CRITICAL(rhdl->instance != NULL);

    // return
    *out_desc = &rhdl->descriptor;
    return HARP_RESULT_OK;
}
HarpResult core_get_actor_desc(const HarpCoreHandler *h, const HarpDependencyDesc *dependency, HarpActorDesc** out_desc) {
    HARP_CHECK_ARG(utils_check_core_handler(h) == HARP_RESULT_OK, HARP_RESULT_INVALID_ARGUMENTS);
    HARP_CHECK_ARG(utils_check_dependency(dependency) == HARP_RESULT_OK, HARP_RESULT_INVALID_ARGUMENTS);
    HARP_CHECK_ARG(out_desc != NULL, HARP_RESULT_MISSING_OUTPUT);

    HarpRuntime *runtime = ((HarpCoreHandlerImpl *)h)->p_runtime;
    HARP_CHECK_CRITICAL(runtime != NULL);

    // retrieve
    HarpRuntimeActor *ract =
        harp_registry_get_runtime(&runtime->registry, dependency->name, HARP_REGISTRY_ENTRY_TYPE_ACTOR);
    
    if(ract == NULL)
        return HARP_RESULT_NAME_NOT_FOUND;
    if(utils_dependency_matches(dependency, ract->descriptor.version) != HARP_RESULT_OK)
        return HARP_RESULT_DEPENDENCY_VERSION_MISMATCH;

    // return
    *out_desc = &ract->descriptor;
    return HARP_RESULT_OK;
}


/* ================================================================================ */
/*  HANDLER                                                                         */
/* ================================================================================ */

HarpResult core_handler_initialize(const HarpCoreHandler *h, const HarpName name, const HarpCreatorBase* creator) {
    HARP_CHECK_ARG(utils_check_core_handler(h) == HARP_RESULT_OK, HARP_RESULT_INVALID_ARGUMENTS);
    HARP_CHECK_ARG(name != NULL, HARP_RESULT_INVALID_ARGUMENTS);
    HARP_CHECK_ARG(creator != NULL, HARP_RESULT_INVALID_ARGUMENTS);

    HarpRuntime *runtime = ((HarpCoreHandlerImpl *)h)->p_runtime;
    HARP_CHECK_CRITICAL(runtime != NULL);

    // retrieve
    HarpRuntimeHandler *rhdl =
        harp_registry_get_runtime(&runtime->registry, name, HARP_REGISTRY_ENTRY_TYPE_HANDLER);
    
    if(rhdl == NULL)
        return HARP_RESULT_NAME_NOT_FOUND;

    // initialize
    return harp_rhdl_initialize(rhdl, runtime, (HarpCreatorBase *) creator); // maybe create a copy of the creator ?
}
HarpResult core_handler_terminate(const HarpCoreHandler *h, const HarpName name) {
    HARP_CHECK_ARG(utils_check_core_handler(h) == HARP_RESULT_OK, HARP_RESULT_INVALID_ARGUMENTS);
    HARP_CHECK_ARG(name != NULL, HARP_RESULT_INVALID_ARGUMENTS);

    HarpRuntime *runtime = ((HarpCoreHandlerImpl *)h)->p_runtime;
    HARP_CHECK_CRITICAL(runtime != NULL);

    // retrieve
    HarpRuntimeHandler *rhdl =
        harp_registry_get_runtime(&runtime->registry, name, HARP_REGISTRY_ENTRY_TYPE_HANDLER);
    
    if(rhdl == NULL)
        return HARP_RESULT_NAME_NOT_FOUND;

    // terminate
    return harp_rhdl_terminate(rhdl, runtime);
}

HarpResult core_handler_set_serving(const HarpCoreHandler *h, HarpHandlerBase *base, uint8_t value) {
    (void)h;
    if(base->status & HARP_STATUS_FLAG_SERVING) {
        if(value)
            return HARP_RESULT_INVALID_STATE;

        base->status &= ~HARP_STATUS_FLAG_SERVING;
    }else {
        if(value == 0)
            return HARP_RESULT_INVALID_STATE;

        base->status |= HARP_STATUS_FLAG_SERVING;
    }

    return HARP_RESULT_OK;
}
HarpResult core_handler_set_failed(const HarpCoreHandler *h, HarpHandlerBase *base, uint8_t value) {
    (void)h;
    if(base->status & HARP_STATUS_FLAG_FAILED) {
        if(value)
            return HARP_RESULT_INVALID_STATE;

        base->status &= ~HARP_STATUS_FLAG_FAILED;
    }else {
        if(value == 0)
            return HARP_RESULT_INVALID_STATE;

        base->status |= HARP_STATUS_FLAG_FAILED;
    }

    return HARP_RESULT_OK;
}


/* ================================================================================ */
/*  ACTOR                                                                           */
/* ================================================================================ */

HarpResult core_actor_create(const HarpCoreHandler *h, const HarpName name, const HarpCreatorBase* creator, HarpActorBase** out_actor) {
    HARP_CHECK_ARG(utils_check_core_handler(h) == HARP_RESULT_OK, HARP_RESULT_INVALID_ARGUMENTS);
    HARP_CHECK_ARG(name != NULL, HARP_RESULT_INVALID_ARGUMENTS);
    HARP_CHECK_ARG(creator != NULL, HARP_RESULT_INVALID_ARGUMENTS);

    HarpRuntime *runtime = ((HarpCoreHandlerImpl *)h)->p_runtime;
    HARP_CHECK_CRITICAL(runtime != NULL);

    // retrieve
    HarpRuntimeActor *ract =
        harp_registry_get_runtime(&runtime->registry, name, HARP_REGISTRY_ENTRY_TYPE_ACTOR);
    
    if(ract == NULL)
        return HARP_RESULT_NAME_NOT_FOUND;

    // create
    HarpActorBase *actor = NULL;

    HarpResult res = harp_ract_alloc_actor(ract, runtime, &actor);
    if(res != HARP_RESULT_OK)
        return res;
    res = harp_ract_create(ract, runtime, actor, (HarpCreatorBase *) creator); // maybe create a copy of the creator ?

    if(res != HARP_RESULT_OK) {
        harp_ract_free_actor(ract, runtime, actor);
        return res;
    }

    *out_actor = actor;
    return HARP_RESULT_OK;
}
HarpResult core_actor_destroy(const HarpCoreHandler *h, const HarpName name, HarpActorBase* actor) {
    HARP_CHECK_ARG(utils_check_core_handler(h) == HARP_RESULT_OK, HARP_RESULT_INVALID_ARGUMENTS);
    HARP_CHECK_ARG(name != NULL, HARP_RESULT_INVALID_ARGUMENTS);
    HARP_CHECK_ARG(actor != NULL, HARP_RESULT_INVALID_ARGUMENTS);

    HarpRuntime *runtime = ((HarpCoreHandlerImpl *)h)->p_runtime;
    HARP_CHECK_CRITICAL(runtime != NULL);

    // retrieve
    HarpRuntimeActor *ract =
        harp_registry_get_runtime(&runtime->registry, name, HARP_REGISTRY_ENTRY_TYPE_ACTOR);
    
    if(ract == NULL)
        return HARP_RESULT_NAME_NOT_FOUND;

    // destroy
    HarpResult res = harp_ract_destroy(ract, runtime, actor);

    if(res == HARP_RESULT_OK)
        harp_ract_free_actor(ract, runtime, actor);

    return res;
}

HarpResult core_actor_set_serving(const HarpCoreHandler *h, HarpActorBase *base, uint8_t value) {
    (void)h;
    if(base->status & HARP_STATUS_FLAG_SERVING) {
        if(value)
            return HARP_RESULT_INVALID_STATE;

        base->status &= ~HARP_STATUS_FLAG_SERVING;
    }else {
        if(value == 0)
            return HARP_RESULT_INVALID_STATE;

        base->status |= HARP_STATUS_FLAG_SERVING;
    }

    return HARP_RESULT_OK;
}
HarpResult core_actor_set_failed(const HarpCoreHandler *h, HarpActorBase *base, uint8_t value) {
    (void)h;
    if(base->status & HARP_STATUS_FLAG_FAILED) {
        if(value)
            return HARP_RESULT_INVALID_STATE;

        base->status &= ~HARP_STATUS_FLAG_FAILED;
    }else {
        if(value == 0)
            return HARP_RESULT_INVALID_STATE;

        base->status |= HARP_STATUS_FLAG_FAILED;
    }

    return HARP_RESULT_OK;
}


/* ================================================================================ */
/*  PATHS                                                                           */
/* ================================================================================ */

HarpResult core_get_executable_directory(const HarpCoreHandler *h, const char **out_path) {
    HARP_CHECK_ARG(utils_check_core_handler(h) == HARP_RESULT_OK, HARP_RESULT_INVALID_ARGUMENTS);
    HARP_CHECK_ARG(out_path != NULL, HARP_RESULT_MISSING_OUTPUT);
    *out_path = NULL;

    HarpRuntime *runtime = ((HarpCoreHandlerImpl *)h)->p_runtime;
    HARP_CHECK_CRITICAL(runtime != NULL);
    
    // retrieve path
    HARP_CHECK_CRITICAL(runtime->executable_directory != NULL);

    *out_path = runtime->executable_directory;
    return HARP_RESULT_OK;
}
HarpResult core_get_working_directory(const HarpCoreHandler *h, const char **out_path) {
    HARP_CHECK_ARG(utils_check_core_handler(h) == HARP_RESULT_OK, HARP_RESULT_INVALID_ARGUMENTS);
    HARP_CHECK_ARG(out_path != NULL, HARP_RESULT_MISSING_OUTPUT);
    *out_path = NULL;

    HarpRuntime *runtime = ((HarpCoreHandlerImpl *)h)->p_runtime;
    HARP_CHECK_CRITICAL(runtime != NULL);
    
    // retrieve path
    HARP_CHECK_CRITICAL(runtime->working_directory != NULL);

    *out_path = runtime->working_directory;
    return HARP_RESULT_OK;
}
HarpResult core_get_package_directory(const HarpCoreHandler *h, const HarpName name, const char **out_path) {
    HARP_CHECK_ARG(utils_check_core_handler(h) == HARP_RESULT_OK, HARP_RESULT_INVALID_ARGUMENTS);
    HARP_CHECK_ARG(name != NULL, HARP_RESULT_INVALID_ARGUMENTS);
    HARP_CHECK_ARG(out_path != NULL, HARP_RESULT_MISSING_OUTPUT);
    *out_path = NULL;

    HarpRuntime *runtime = ((HarpCoreHandlerImpl *)h)->p_runtime;
    HARP_CHECK_CRITICAL(runtime != NULL);

    // retrieve package
    HarpLoadedPackage *pkg =
        harp_package_manager_get(&runtime->package_manager, name);
    
    if(pkg == NULL)
        return HARP_RESULT_NAME_NOT_FOUND;

    // retrieve path
    if(pkg->directory == NULL)
        return HARP_RESULT_FAILED;

    *out_path = pkg->directory;
    return HARP_RESULT_OK;
}


/* ================================================================================ */
/*  TIME                                                                            */
/* ================================================================================ */

#if HARP_PLATFORM_WINDOWS

HarpResult core_get_uptime_s(const HarpCoreHandler *h, uint64_t *out_time) {
    if(h == NULL || out_time == NULL)
        return HARP_RESULT_INVALID_ARGUMENTS;

    HarpCoreHandlerImpl *impl = (HarpCoreHandlerImpl *)h;
    LARGE_INTEGER now;
    QueryPerformanceCounter(&now);
    *out_time = (uint64_t)(now.QuadPart - impl->start_time.QuadPart) / (uint64_t)impl->frequency.QuadPart;
    return HARP_RESULT_OK;
}
HarpResult core_get_uptime_ms(const HarpCoreHandler *h, uint64_t *out_time) {
    if(h == NULL || out_time == NULL)
        return HARP_RESULT_INVALID_ARGUMENTS;

    HarpCoreHandlerImpl *impl = (HarpCoreHandlerImpl *)h;
    LARGE_INTEGER now;
    QueryPerformanceCounter(&now);
    *out_time = ((uint64_t)(now.QuadPart - impl->start_time.QuadPart) * 1000ULL) / (uint64_t)impl->frequency.QuadPart;
    return HARP_RESULT_OK;
}
HarpResult core_get_uptime_ns(const HarpCoreHandler *h, uint64_t *out_time) {
    if(h == NULL || out_time == NULL)
        return HARP_RESULT_INVALID_ARGUMENTS;

    HarpCoreHandlerImpl *impl = (HarpCoreHandlerImpl *)h;
    LARGE_INTEGER now;
    QueryPerformanceCounter(&now);
    *out_time = ((uint64_t)(now.QuadPart - impl->start_time.QuadPart) * 1000000000ULL) / (uint64_t)impl->frequency.QuadPart;
    return HARP_RESULT_OK;
}

#elif HARP_PLATFORM_LINUX

HarpResult core_get_uptime_s(const HarpCoreHandler *h, uint64_t *out_time) {
    if(h == NULL || out_time == NULL)
        return HARP_RESULT_INVALID_ARGUMENTS;

    HarpCoreHandlerImpl *impl = (HarpCoreHandlerImpl *)h;
    struct timespec now;
    clock_gettime(CLOCK_MONOTONIC, &now);
    uint64_t now_ns = (uint64_t)now.tv_sec * 1000000000ULL + (uint64_t)now.tv_nsec;
    uint64_t start_ns = (uint64_t)impl->start_time.tv_sec * 1000000000ULL + (uint64_t)impl->start_time.tv_nsec;
    *out_time = (now_ns - start_ns) / 1000000000ULL;
    return HARP_RESULT_OK;
}
HarpResult core_get_uptime_ms(const HarpCoreHandler *h, uint64_t *out_time) {
    if(h == NULL || out_time == NULL)
        return HARP_RESULT_INVALID_ARGUMENTS;

    HarpCoreHandlerImpl *impl = (HarpCoreHandlerImpl *)h;
    struct timespec now;
    clock_gettime(CLOCK_MONOTONIC, &now);
    uint64_t now_ns = (uint64_t)now.tv_sec * 1000000000ULL + (uint64_t)now.tv_nsec;
    uint64_t start_ns = (uint64_t)impl->start_time.tv_sec * 1000000000ULL + (uint64_t)impl->start_time.tv_nsec;
    *out_time = (now_ns - start_ns) / 1000000ULL;
    return HARP_RESULT_OK;
}
HarpResult core_get_uptime_ns(const HarpCoreHandler *h, uint64_t *out_time) {
    if(h == NULL || out_time == NULL)
        return HARP_RESULT_INVALID_ARGUMENTS;

    HarpCoreHandlerImpl *impl = (HarpCoreHandlerImpl *)h;
    struct timespec now;
    clock_gettime(CLOCK_MONOTONIC, &now);
    uint64_t now_ns = (uint64_t)now.tv_sec * 1000000000ULL + (uint64_t)now.tv_nsec;
    uint64_t start_ns = (uint64_t)impl->start_time.tv_sec * 1000000000ULL + (uint64_t)impl->start_time.tv_nsec;
    *out_time = now_ns - start_ns;
    return HARP_RESULT_OK;
}

#endif


/* ================================================================================ */
/*  CORE HANDLER                                                                    */
/* ================================================================================ */

HarpResult init_core_handler(HarpCoreHandler *core_handler, HarpHandlerBase *base, HarpCreatorBase *creator) {
    HARP_UNUSED(core_handler);
    HARP_UNUSED(creator);

    HarpCoreHandlerImpl *handler = HARP_HANDLER_AS(HarpCoreHandlerImpl, base);

    #if HARP_PLATFORM_WINDOWS
    QueryPerformanceFrequency(&handler->frequency);
    QueryPerformanceCounter(&handler->start_time);
#elif HARP_PLATFORM_LINUX
    clock_gettime(CLOCK_MONOTONIC, &handler->start_time);
#endif

    return HARP_RESULT_OK;
}
HarpResult term_core_handler(HarpCoreHandler *core_handler, HarpHandlerBase *base) {
    HARP_UNUSED(core_handler);

    HarpCoreHandlerImpl *handler = HARP_HANDLER_AS(HarpCoreHandlerImpl, base);
    if(handler->p_runtime->core_handler != NULL)
        return HARP_RESULT_INVALID_STATE;

    return HARP_RESULT_OK;
}

