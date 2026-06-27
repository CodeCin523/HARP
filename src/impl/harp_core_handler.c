#include "harp_core_handler.h"
#include "runtime/harp_runtime.h"
#include "utils.h"

#include <hmem/hmem_block.h>
#include <hmem/hmem_os.h>
#include <hmem/utils/hmem_align.h>

#include <stdlib.h>
#include <string.h>
#include <stdalign.h>


/* ================================================================================ */
/*  HarpCoreApi's impl                                                              */
/* ================================================================================ */

HarpResult handler_register_handler(const HarpCoreHandler *h, const HarpHandlerDesc* desc) {
    if(utils_check_core_handler(h) != HARP_RESULT_OK)
        return HARP_RESULT_INVALID_ARGUMENTS;

    HarpRuntime *runtime = ((HarpCoreHandlerImpl *)h)->p_runtime;
    return runtime_register_handler(runtime, desc);
}
HarpResult handler_register_actor(const HarpCoreHandler *h, const HarpActorDesc* desc) {
    if(utils_check_core_handler(h) != HARP_RESULT_OK)
        return HARP_RESULT_INVALID_ARGUMENTS;

    HarpRuntime *runtime = ((HarpCoreHandlerImpl *)h)->p_runtime;
    return runtime_register_actor(runtime, desc);
}

HarpResult handler_get_handler(const HarpCoreHandler *h, const HarpDependencyDesc *dependency, HarpHandlerBase** out_handler) {
    if(utils_check_core_handler(h) != HARP_RESULT_OK)
        return HARP_RESULT_INVALID_ARGUMENTS;

    HarpRuntime *runtime = ((HarpCoreHandlerImpl *)h)->p_runtime;
    return runtime_get_handler(runtime, dependency, out_handler);
}

HarpResult handler_get_actor_count(const HarpCoreHandler *h, const HarpName name, uint64_t *out_count) {
    if(utils_check_core_handler(h) != HARP_RESULT_OK)
        return HARP_RESULT_INVALID_ARGUMENTS;

    HarpRuntime *runtime = ((HarpCoreHandlerImpl *)h)->p_runtime;
    return runtime_get_actor_count(runtime, name, out_count);
}
HarpResult handler_get_actor_at(const HarpCoreHandler *h, const HarpName name, uint64_t index, HarpActorBase **out_actor) {
    if(utils_check_core_handler(h) != HARP_RESULT_OK)
        return HARP_RESULT_INVALID_ARGUMENTS;

    HarpRuntime *runtime = ((HarpCoreHandlerImpl *)h)->p_runtime;
    return runtime_get_actor_at(runtime, name, index, out_actor);
}
HarpResult handler_get_actors(const HarpCoreHandler *h, const HarpName name, uint64_t *inout_count, HarpActorBase **out_actors) {
    if(utils_check_core_handler(h) != HARP_RESULT_OK)
        return HARP_RESULT_INVALID_ARGUMENTS;

    HarpRuntime *runtime = ((HarpCoreHandlerImpl *)h)->p_runtime;
    return runtime_get_actors(runtime, name, inout_count, out_actors);
}

HarpResult handler_get_handler_desc(const HarpCoreHandler *h, const HarpName name, HarpHandlerDesc** out_desc) {
    if(utils_check_core_handler(h) != HARP_RESULT_OK)
        return HARP_RESULT_INVALID_ARGUMENTS;

    HarpRuntime *runtime = ((HarpCoreHandlerImpl *)h)->p_runtime;
    return runtime_get_handler_desc(runtime, name, out_desc);
}
HarpResult handler_get_actor_desc(const HarpCoreHandler *h, const HarpName name, HarpActorDesc** out_desc) {
    if(utils_check_core_handler(h) != HARP_RESULT_OK)
        return HARP_RESULT_INVALID_ARGUMENTS;

    HarpRuntime *runtime = ((HarpCoreHandlerImpl *)h)->p_runtime;
    return runtime_get_actor_desc(runtime, name, out_desc);
}

HarpResult handler_handler_initialize(const HarpCoreHandler *h, const HarpName name, const HarpCreatorBase* creator) {
    if(utils_check_core_handler(h) != HARP_RESULT_OK)
        return HARP_RESULT_INVALID_ARGUMENTS;

    HarpRuntime *runtime = ((HarpCoreHandlerImpl *)h)->p_runtime;
    return runtime_handler_initialize(runtime, name, creator);
}
HarpResult handler_handler_terminate(const HarpCoreHandler *h, const HarpName name) {
    if(utils_check_core_handler(h) != HARP_RESULT_OK)
        return HARP_RESULT_INVALID_ARGUMENTS;

    HarpRuntime *runtime = ((HarpCoreHandlerImpl *)h)->p_runtime;
    return runtime_handler_terminate(runtime, name);
}

HarpResult handler_actor_create(const HarpCoreHandler *h, const HarpName name, const HarpCreatorBase* creator, HarpActorBase** out_actor) {
    if(utils_check_core_handler(h) != HARP_RESULT_OK)
        return HARP_RESULT_INVALID_ARGUMENTS;

    HarpRuntime *runtime = ((HarpCoreHandlerImpl *)h)->p_runtime;
    return runtime_actor_create(runtime, name, creator, out_actor);
}
HarpResult handler_actor_destroy(const HarpCoreHandler *h, const HarpName name, HarpActorBase* actor) {
    if(utils_check_core_handler(h) != HARP_RESULT_OK)
        return HARP_RESULT_INVALID_ARGUMENTS;

    HarpRuntime *runtime = ((HarpCoreHandlerImpl *)h)->p_runtime;
    return runtime_actor_destroy(runtime, name, actor);
}

HarpResult handler_get_executable_directory(const HarpCoreHandler *h, const char **out_path) {
    if(utils_check_core_handler(h) != HARP_RESULT_OK)
        return HARP_RESULT_INVALID_ARGUMENTS;

    HarpRuntime *runtime = ((HarpCoreHandlerImpl *)h)->p_runtime;
    return runtime_get_executable_directory(runtime, out_path);
}
HarpResult handler_get_working_directory(const HarpCoreHandler *h, const char **out_path) {
    if(utils_check_core_handler(h) != HARP_RESULT_OK)
        return HARP_RESULT_INVALID_ARGUMENTS;

    HarpRuntime *runtime = ((HarpCoreHandlerImpl *)h)->p_runtime;
    return runtime_get_working_directory(runtime, out_path);
}
HarpResult handler_get_package_directory(const HarpCoreHandler *h, const HarpName name, const char **out_path) {
    if(utils_check_core_handler(h) != HARP_RESULT_OK)
        return HARP_RESULT_INVALID_ARGUMENTS;

    HarpRuntime *runtime = ((HarpCoreHandlerImpl *)h)->p_runtime;
    return runtime_get_package_directory(runtime, name, out_path);
}

HarpResult core_get_actor_count(const HarpCoreHandler *h, const HarpName name, uint64_t *out_count) {
    if(utils_check_core_handler(h) != HARP_RESULT_OK)
        return HARP_RESULT_INVALID_ARGUMENTS;

    HarpRuntime *runtime = ((HarpCoreHandlerImpl *)h)->p_runtime;
    return runtime_get_actor_count(runtime, name, out_count);
}
HarpResult core_get_actor_at(const HarpCoreHandler *h, const HarpName name, uint64_t index, HarpActorBase **out_actor) {
    if(utils_check_core_handler(h) != HARP_RESULT_OK)
        return HARP_RESULT_INVALID_ARGUMENTS;

    HarpRuntime *runtime = ((HarpCoreHandlerImpl *)h)->p_runtime;
    return runtime_get_actor_at(runtime, name, index, out_actor);
}
HarpResult core_get_actors(const HarpCoreHandler *h, const HarpName name, uint64_t *inout_count, HarpActorBase **out_actors) {
    if(utils_check_core_handler(h) != HARP_RESULT_OK)
        return HARP_RESULT_INVALID_ARGUMENTS;

    HarpRuntime *runtime = ((HarpCoreHandlerImpl *)h)->p_runtime;
    return runtime_get_actors(runtime, name, inout_count, out_actors);
}

HarpResult handler_handler_set_serving(const HarpCoreHandler *h, HarpHandlerBase *base, uint8_t value) {
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
HarpResult handler_handler_set_failed(const HarpCoreHandler *h, HarpHandlerBase *base, uint8_t value) {
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

HarpResult handler_actor_set_serving(const HarpCoreHandler *h, HarpActorBase *base, uint8_t value) {
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
HarpResult handler_actor_set_failed(const HarpCoreHandler *h, HarpActorBase *base, uint8_t value) {
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
/*  TIME                                                                            */
/* ================================================================================ */

#if HARP_PLATFORM_WINDOWS

HarpResult handler_get_uptime_s(const HarpCoreHandler *h, uint64_t *out_time) {
    if(h == NULL || out_time == NULL)
        return HARP_RESULT_INVALID_ARGUMENTS;

    HarpCoreHandlerImpl *impl = (HarpCoreHandlerImpl *)h;
    LARGE_INTEGER now;
    QueryPerformanceCounter(&now);
    *out_time = (uint64_t)(now.QuadPart - impl->start_time.QuadPart) / (uint64_t)impl->frequency.QuadPart;
    return HARP_RESULT_OK;
}
HarpResult handler_get_uptime_ms(const HarpCoreHandler *h, uint64_t *out_time) {
    if(h == NULL || out_time == NULL)
        return HARP_RESULT_INVALID_ARGUMENTS;

    HarpCoreHandlerImpl *impl = (HarpCoreHandlerImpl *)h;
    LARGE_INTEGER now;
    QueryPerformanceCounter(&now);
    *out_time = ((uint64_t)(now.QuadPart - impl->start_time.QuadPart) * 1000ULL) / (uint64_t)impl->frequency.QuadPart;
    return HARP_RESULT_OK;
}
HarpResult handler_get_uptime_ns(const HarpCoreHandler *h, uint64_t *out_time) {
    if(h == NULL || out_time == NULL)
        return HARP_RESULT_INVALID_ARGUMENTS;

    HarpCoreHandlerImpl *impl = (HarpCoreHandlerImpl *)h;
    LARGE_INTEGER now;
    QueryPerformanceCounter(&now);
    *out_time = ((uint64_t)(now.QuadPart - impl->start_time.QuadPart) * 1000000000ULL) / (uint64_t)impl->frequency.QuadPart;
    return HARP_RESULT_OK;
}

#elif HARP_PLATFORM_LINUX

HarpResult handler_get_uptime_s(const HarpCoreHandler *h, uint64_t *out_time) {
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
HarpResult handler_get_uptime_ms(const HarpCoreHandler *h, uint64_t *out_time) {
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
HarpResult handler_get_uptime_ns(const HarpCoreHandler *h, uint64_t *out_time) {
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
/*  HarpRuntime's impl                                                              */
/* ================================================================================ */

/* ================================================================================ */
/*  REGISTRATION                                                                    */
/* ================================================================================ */

HarpResult runtime_register_handler(
    HarpRuntime *runtime,
    const HarpHandlerDesc *desc
) {
    if(utils_check_runtime(runtime) != HARP_RESULT_OK || utils_check_handler_desc(desc) != HARP_RESULT_OK) 
        return HARP_RESULT_INVALID_ARGUMENTS;

    // Allocate runtime descriptor
    HarpRuntimeHandler *rdesc =
        harp_alloc_global(
            runtime,
            sizeof(HarpRuntimeHandler),
            alignof(HarpRuntimeHandler)
        );

    if(rdesc == NULL)
        return HARP_RESULT_OUT_OF_MEMORY;

    memset(rdesc, 0, sizeof(HarpRuntimeHandler));

    // Allocate handler instance
    HarpHandlerBase *rinst =
        harp_alloc_global(
            runtime,
            desc->instance_size,
            desc->instance_alignment
        );

    if(rinst == NULL)
        return HARP_RESULT_OUT_OF_MEMORY;

    memset(rinst, 0, desc->instance_size);

    // Copy descriptor metadata
    rdesc->descriptor = *desc;

    // Store name in registry memory
    HarpName rname =
        harp_registry_name(&runtime->registry, desc->name);

    if(rname == NULL)
        return HARP_RESULT_OUT_OF_MEMORY;

    rdesc->descriptor.name = rname;

    // Copy dependencies into runtime-owned memory
    if(desc->dependency_count > 0 && desc->p_dependencies != NULL) {
        size_t deps_size =
            sizeof(HarpDependencyDesc) * desc->dependency_count;

        HarpDependencyDesc *rdeps =
            harp_alloc_global(
                runtime,
                deps_size,
                alignof(HarpDependencyDesc)
            );

        if(rdeps == NULL)
            return HARP_RESULT_OUT_OF_MEMORY;

        memcpy(
            rdeps,
            desc->p_dependencies,
            deps_size
        );

        // Normalize dependency names through registry
        for(uint64_t i = 0; i < desc->dependency_count; ++i) {
            HarpName dep_name =
                harp_registry_name(
                    &runtime->registry,
                    rdeps[i].name
                );

            if(dep_name == NULL)
                return HARP_RESULT_OUT_OF_MEMORY;

            rdeps[i].name = dep_name;
        }

        rdesc->descriptor.p_dependencies = rdeps;
    }

    // Initialize runtime state
    rdesc->dependent_count = 0;
    rdesc->actor_count = 0;

    // Initialize handler base
    HarpName *name_ptr = (HarpName *) &rinst->name;
    *name_ptr = rname;
    rinst->status = 0;

    // Save instance inside runtime descriptor
    rdesc->instance = rinst;

    // Bind runtime to registry
    HarpResult res =
        harp_registry_bind(
            &runtime->registry,
            rname,
            HARP_REGISTRY_ENTRY_TYPE_HANDLER,
            rdesc
        );

    if(res != HARP_RESULT_OK)
        return res;

    return HARP_RESULT_OK;
}

HarpResult runtime_register_actor(
    HarpRuntime *runtime,
    const HarpActorDesc *desc
) {
    if(utils_check_runtime(runtime) != HARP_RESULT_OK || utils_check_actor_desc(desc) != HARP_RESULT_OK) 
        return HARP_RESULT_INVALID_ARGUMENTS;

    // Allocate actor runtime descriptor
    HarpRuntimeActor *rdesc =
        harp_alloc_global(
            runtime,
            sizeof(HarpRuntimeActor),
            alignof(HarpRuntimeActor)
        );

    if(rdesc == NULL)
        return HARP_RESULT_OUT_OF_MEMORY;

    memset(rdesc, 0, sizeof(HarpRuntimeActor));

    // Copy descriptor metadata
    rdesc->descriptor = *desc;

    // Store actor name in registry memory
    HarpName rname =
        harp_registry_name(&runtime->registry, desc->name);

    if(rname == NULL)
        return HARP_RESULT_OUT_OF_MEMORY;

    rdesc->descriptor.name = rname;
    rdesc->page_growth_index = 0;

    // Store parent handler name in registry memory
    if(desc->parent_handler != NULL) {
        HarpName rparent =
            harp_registry_name(
                &runtime->registry,
                desc->parent_handler
            );

        if(rparent == NULL)
            return HARP_RESULT_OUT_OF_MEMORY;

        rdesc->descriptor.parent_handler = rparent;
    }

    // Setup actor instance storage
    if(harp_setup_runtime_actor(rdesc, desc->instance_size, desc->instance_alignment) != HARP_RESULT_OK)
        return HARP_RESULT_FAILED;

    // Bind runtime to registry
    HarpResult res =
        harp_registry_bind(
            &runtime->registry,
            rname,
            HARP_REGISTRY_ENTRY_TYPE_ACTOR,
            rdesc
        );

    if(res != HARP_RESULT_OK) {
        hmem_teardown_block(&rdesc->inst_block);
        hmem_teardown_book(&rdesc->inst_book);
        return res;
    }

    rdesc->actors = NULL;
    rdesc->actor_count = 0;
    rdesc->actor_capacity = 0;

    return HARP_RESULT_OK;
}


/* ================================================================================ */
/*  RETRIEVAL                                                                       */
/* ================================================================================ */

HarpResult runtime_get_handler(
    HarpRuntime *runtime,
    const HarpDependencyDesc *dependency,
    HarpHandlerBase **out_handler
) {
    if(utils_check_runtime(runtime) != HARP_RESULT_OK || utils_check_dependency(dependency) != HARP_RESULT_OK) 
        return HARP_RESULT_INVALID_ARGUMENTS;

    if(out_handler == NULL)
        return HARP_RESULT_MISSING_OUTPUT;

    *out_handler = NULL;

    HarpRuntimeHandler *rdesc =
        harp_registry_get(
            &runtime->registry,
            dependency->name,
            HARP_REGISTRY_ENTRY_TYPE_HANDLER
        );

    if(rdesc == NULL)
        return HARP_RESULT_NAME_NOT_FOUND;

    if(rdesc->instance == NULL)
        return HARP_RESULT_CRITICAL_FAIL;

    if(utils_dependency_matches(dependency, rdesc->descriptor.version) != HARP_RESULT_OK)
        return HARP_RESULT_DEPENDENCY_VERSION_MISMATCH;

    *out_handler = rdesc->instance;
    return HARP_RESULT_OK;
}


HarpResult runtime_get_actor_count(
    HarpRuntime *runtime,
    const HarpName name,
    uint64_t *out_count
) {
    if(utils_check_runtime(runtime) != HARP_RESULT_OK || name == NULL)
        return HARP_RESULT_INVALID_ARGUMENTS;

    if(out_count == NULL)
        return HARP_RESULT_MISSING_OUTPUT;

    HarpRuntimeActor *rdesc = harp_registry_get(&runtime->registry, name, HARP_REGISTRY_ENTRY_TYPE_ACTOR); 
    if(rdesc == NULL)
        return HARP_RESULT_NAME_NOT_FOUND;

    *out_count = rdesc->actor_count;
    return HARP_RESULT_OK;
}

HarpResult runtime_get_actor_at(
    HarpRuntime *runtime,
    const HarpName name,
    uint64_t index,
    HarpActorBase **out_actor
) {
    if(utils_check_runtime(runtime) != HARP_RESULT_OK || name == NULL)
        return HARP_RESULT_INVALID_ARGUMENTS;

    if(out_actor == NULL)
        return HARP_RESULT_MISSING_OUTPUT;

    HarpRuntimeActor *rdesc = harp_registry_get(&runtime->registry, name, HARP_REGISTRY_ENTRY_TYPE_ACTOR); 
    if(rdesc == NULL)
        return HARP_RESULT_NAME_NOT_FOUND;

    if(index >= rdesc->actor_count)
        return HARP_RESULT_INVALID_ARGUMENTS;

    *out_actor = rdesc->actors[index];
    return HARP_RESULT_OK;
}

HarpResult runtime_get_actors(
    HarpRuntime *runtime,
    const HarpName name,
    uint64_t *inout_count,
    HarpActorBase **out_actors
) {
    if(utils_check_runtime(runtime) != HARP_RESULT_OK || name == NULL)
        return HARP_RESULT_INVALID_ARGUMENTS;

    if(inout_count == NULL)
        return HARP_RESULT_MISSING_OUTPUT;

    HarpRuntimeActor *rdesc = harp_registry_get(&runtime->registry, name, HARP_REGISTRY_ENTRY_TYPE_ACTOR); 
    if(rdesc == NULL)
        return HARP_RESULT_NAME_NOT_FOUND;

    if(out_actors == NULL) {
        *inout_count = rdesc->actor_count;
    } else {
        uint64_t count = *inout_count;
            
        if(count > rdesc->actor_count)
            count = rdesc->actor_count;
            
        for(uint64_t i = 0; i < count; ++i)
            out_actors[i] = rdesc->actors[i];
            
        *inout_count = count;
    }

    return HARP_RESULT_OK;
}


HarpResult runtime_get_handler_desc(
    HarpRuntime *runtime,
    const HarpName name,
    HarpHandlerDesc **out_desc
) {
    if(utils_check_runtime(runtime) != HARP_RESULT_OK || name == NULL) 
        return HARP_RESULT_INVALID_ARGUMENTS;

    if(out_desc == NULL)
        return HARP_RESULT_MISSING_OUTPUT;

    *out_desc = NULL;

    HarpRuntimeHandler *rdesc =
        harp_registry_get(
            &runtime->registry,
            name,
            HARP_REGISTRY_ENTRY_TYPE_HANDLER
        );

    if(rdesc == NULL)
        return HARP_RESULT_NAME_NOT_FOUND;

    *out_desc = &rdesc->descriptor;
    return HARP_RESULT_OK;
}

HarpResult runtime_get_actor_desc(
    HarpRuntime *runtime,
    const HarpName name,
    HarpActorDesc **out_desc
) {
    if(utils_check_runtime(runtime) != HARP_RESULT_OK || name == NULL) 
        return HARP_RESULT_INVALID_ARGUMENTS;

    if(out_desc == NULL)
        return HARP_RESULT_MISSING_OUTPUT;

    *out_desc = NULL;

    HarpRuntimeActor *rdesc =
        harp_registry_get(
            &runtime->registry,
            name,
            HARP_REGISTRY_ENTRY_TYPE_ACTOR
        );

    if(rdesc == NULL)
        return HARP_RESULT_NAME_NOT_FOUND;

    *out_desc = &rdesc->descriptor;
    return HARP_RESULT_OK;
}


/* ================================================================================ */
/*  LIFECYCLE                                                                       */
/* ================================================================================ */

HarpResult runtime_handler_initialize(
    HarpRuntime *runtime,
    const HarpName name,
    const HarpCreatorBase *creator
) {
    if(utils_check_runtime(runtime) != HARP_RESULT_OK || name == NULL || creator == NULL)
        return HARP_RESULT_INVALID_ARGUMENTS;

    // find handler runtime
    HarpRuntimeHandler *rdesc =
        harp_registry_get(
            &runtime->registry,
            name,
            HARP_REGISTRY_ENTRY_TYPE_HANDLER
        );

    if(rdesc == NULL)
        return HARP_RESULT_NAME_NOT_FOUND;

    HarpHandlerDesc *desc = &rdesc->descriptor;
    HarpHandlerBase *base = rdesc->instance;

    if(base == NULL)
        return HARP_RESULT_CRITICAL_FAIL;

    // check state
    if(base->status & HARP_STATUS_FLAG_VALID)
        return HARP_RESULT_OK;

    if(base->status & (HARP_STATUS_FLAG_INITIALIZING | HARP_STATUS_FLAG_TERMINATING))
        return HARP_RESULT_INVALID_STATE;

    // check dependencies
    for(uint64_t i = 0; i < desc->dependency_count; ++i) {
        HarpDependencyDesc *ddep = &desc->p_dependencies[i];

        HarpRuntimeHandler *drdesc =
            harp_registry_get(
                &runtime->registry,
                ddep->name,
                HARP_REGISTRY_ENTRY_TYPE_HANDLER
            );

        if(drdesc == NULL)
            return HARP_RESULT_DEPENDENCY_NOT_FOUND;

        if(utils_dependency_matches(ddep, drdesc->descriptor.version) != HARP_RESULT_OK)
            return HARP_RESULT_DEPENDENCY_VERSION_MISMATCH;

        HarpHandlerBase *dbase = drdesc->instance;
        if(dbase == NULL)
            return HARP_RESULT_CRITICAL_FAIL;

        if(dbase->status & HARP_STATUS_FLAG_INITIALIZING)
            return HARP_RESULT_DEPENDENCY_CYCLE;

        if(!(dbase->status & HARP_STATUS_FLAG_VALID))
            return HARP_RESULT_DEPENDENCY_UNINITIALIZED;
    }

    // start initialization
    base->status |= HARP_STATUS_FLAG_INITIALIZING;

    HarpResult res =
        desc->pfn_init(
            runtime->core_handler,
            base,
            (HarpCreatorBase*)creator
        );

    base->status &= ~HARP_STATUS_FLAG_INITIALIZING;

    if(res == HARP_RESULT_OK) {
        base->status |= HARP_STATUS_FLAG_VALID;

        for(uint64_t i = 0; i < desc->dependency_count; ++i) {
            HarpDependencyDesc *ddep = &desc->p_dependencies[i];

            HarpRuntimeHandler *drdesc =
                harp_registry_get(
                    &runtime->registry,
                    ddep->name,
                    HARP_REGISTRY_ENTRY_TYPE_HANDLER
                );

            if(drdesc != NULL)
                ++drdesc->dependent_count;
        }
    }

    return res;
}

HarpResult runtime_handler_terminate(
    HarpRuntime *runtime,
    const HarpName name
) {
    if(utils_check_runtime(runtime) != HARP_RESULT_OK || name == NULL)
        return HARP_RESULT_INVALID_ARGUMENTS;

    // find handler runtime
    HarpRuntimeHandler *rdesc =
        harp_registry_get(
            &runtime->registry,
            name,
            HARP_REGISTRY_ENTRY_TYPE_HANDLER
        );

    if(rdesc == NULL)
        return HARP_RESULT_NAME_NOT_FOUND;

    HarpHandlerDesc *desc = &rdesc->descriptor;
    HarpHandlerBase *base = rdesc->instance;

    if(base == NULL)
        return HARP_RESULT_CRITICAL_FAIL;

    // check state
    if(!(base->status & HARP_STATUS_FLAG_VALID))
        return HARP_RESULT_OK;

    if(base->status & (HARP_STATUS_FLAG_INITIALIZING | HARP_STATUS_FLAG_TERMINATING))
        return HARP_RESULT_INVALID_STATE;

    // check dependents
    if(rdesc->dependent_count != 0)
        return HARP_RESULT_EXISTENT_DEPENDENTS;

    if(rdesc->actor_count != 0)
        return HARP_RESULT_EXISTENT_ACTORS;

    // start termination
    base->status |= HARP_STATUS_FLAG_TERMINATING;

    HarpResult res =
        desc->pfn_term(
            runtime->core_handler,
            base
        );

    base->status &= ~HARP_STATUS_FLAG_TERMINATING;

    if(res == HARP_RESULT_OK) {
        base->status &= ~HARP_STATUS_FLAG_VALID;

        for(uint64_t i = 0; i < desc->dependency_count; ++i) {
            HarpDependencyDesc *ddep = &desc->p_dependencies[i];

            HarpRuntimeHandler *drdesc =
                harp_registry_get(
                    &runtime->registry,
                    ddep->name,
                    HARP_REGISTRY_ENTRY_TYPE_HANDLER
                );

            if(drdesc != NULL && drdesc->dependent_count > 0)
                --drdesc->dependent_count;
        }
    }

    return res;
}


/* ================================================================================ */
/*  ACTORS                                                                          */
/* ================================================================================ */

HarpResult runtime_actor_create(
    HarpRuntime *runtime,
    const HarpName name,
    const HarpCreatorBase *creator,
    HarpActorBase **out_actor
) {
    if(utils_check_runtime(runtime) != HARP_RESULT_OK || name == NULL || creator == NULL)
        return HARP_RESULT_INVALID_ARGUMENTS;

    if(out_actor == NULL)
        return HARP_RESULT_MISSING_OUTPUT;

    *out_actor = NULL;

    // find actor runtime
    HarpRuntimeActor *actor_rdesc =
        harp_registry_get(
            &runtime->registry,
            name,
            HARP_REGISTRY_ENTRY_TYPE_ACTOR
        );

    if(actor_rdesc == NULL)
        return HARP_RESULT_NAME_NOT_FOUND;

    HarpActorDesc *actor_desc = &actor_rdesc->descriptor;

    // find parent handler runtime
    HarpRuntimeHandler *handler_rdesc =
        harp_registry_get(
            &runtime->registry,
            actor_desc->parent_handler,
            HARP_REGISTRY_ENTRY_TYPE_HANDLER
        );

    if(handler_rdesc == NULL)
        return HARP_RESULT_NAME_NOT_FOUND;

    HarpHandlerBase *handler_base = handler_rdesc->instance;
    if(handler_base == NULL)
        return HARP_RESULT_CRITICAL_FAIL;

    // check handler state
    if(!(handler_base->status & HARP_STATUS_FLAG_VALID))
        return HARP_RESULT_INVALID_STATE;

    // check dependencies
    HarpHandlerDesc *handler_desc = &handler_rdesc->descriptor;
    if(handler_desc == NULL)
        return HARP_RESULT_CRITICAL_FAIL;

    for(uint64_t i = 0; i < handler_desc->dependency_count; ++i) {
        HarpDependencyDesc *ddep = &handler_desc->p_dependencies[i];

        HarpRuntimeHandler *drdesc =
            harp_registry_get(
                &runtime->registry,
                ddep->name,
                HARP_REGISTRY_ENTRY_TYPE_HANDLER
            );

        if(drdesc == NULL)
            return HARP_RESULT_DEPENDENCY_NOT_FOUND;

        if(utils_dependency_matches(ddep, drdesc->descriptor.version) != HARP_RESULT_OK)
            return HARP_RESULT_DEPENDENCY_VERSION_MISMATCH;

        HarpHandlerBase *dbase = drdesc->instance;
        if(dbase == NULL)
            return HARP_RESULT_CRITICAL_FAIL;

        if(dbase->status & HARP_STATUS_FLAG_INITIALIZING)
            return HARP_RESULT_DEPENDENCY_CYCLE;

        if(!(dbase->status & HARP_STATUS_FLAG_VALID))
            return HARP_RESULT_DEPENDENCY_UNINITIALIZED;
    }

    // grow actor pointer array
    if(actor_rdesc->actor_count >= actor_rdesc->actor_capacity) {
        uint64_t new_capacity =
            actor_rdesc->actor_capacity == 0
            ? 16
            : actor_rdesc->actor_capacity * 2;

        void *new_ptr = realloc(
            actor_rdesc->actors,
            sizeof(HarpActorBase*) * new_capacity
        );

        if(new_ptr == NULL)
            return HARP_RESULT_OUT_OF_MEMORY;

        actor_rdesc->actors = new_ptr;
        actor_rdesc->actor_capacity = new_capacity;
    }

    // allocate actor instance
    HarpActorBase *actor_base = harp_alloc_actor(runtime, actor_rdesc);
    if(actor_base == NULL) {
        return HARP_RESULT_OUT_OF_MEMORY;
    }

    memset(actor_base, 0, actor_desc->instance_size);

    HarpName *name_ptr = (HarpName *) &actor_base->name;
    *name_ptr = actor_desc->name;

    // create actor
    actor_base->status |= HARP_STATUS_FLAG_INITIALIZING;

    HarpResult res =
        actor_desc->pfn_create(
            runtime->core_handler,
            actor_base,
            (HarpCreatorBase*)creator
        );

    actor_base->status &= ~HARP_STATUS_FLAG_INITIALIZING;

    if(res == HARP_RESULT_OK) {
        actor_base->status |= HARP_STATUS_FLAG_VALID;
        actor_rdesc->actors[actor_rdesc->actor_count++] = actor_base;

        *out_actor = actor_base;

        ++handler_rdesc->actor_count;
    } else {
        hmem_block_free_single(
            &actor_rdesc->inst_block,
            actor_base
        );
    }

    return res;
}

HarpResult runtime_actor_destroy(
    HarpRuntime *runtime,
    const HarpName name,
    HarpActorBase *actor
) {
    if(utils_check_runtime(runtime) != HARP_RESULT_OK || name == NULL || actor == NULL)
        return HARP_RESULT_INVALID_ARGUMENTS;

    // find actor runtime
    HarpRuntimeActor *actor_rdesc =
        harp_registry_get(
            &runtime->registry,
            name,
            HARP_REGISTRY_ENTRY_TYPE_ACTOR
        );

    if(actor_rdesc == NULL)
        return HARP_RESULT_NAME_NOT_FOUND;

    HarpActorDesc *actor_desc = &actor_rdesc->descriptor;

    // find parent handler runtime
    HarpRuntimeHandler *handler_rdesc =
        harp_registry_get(
            &runtime->registry,
            actor_desc->parent_handler,
            HARP_REGISTRY_ENTRY_TYPE_HANDLER
        );

    if(handler_rdesc == NULL)
        return HARP_RESULT_NAME_NOT_FOUND;

    // check state
    HarpHandlerBase *handler_base = handler_rdesc->instance;
    if(!(handler_base->status & HARP_STATUS_FLAG_VALID))
        return HARP_RESULT_INVALID_STATE;

    // validate actor ownership
    if(!hmem_book_contains(&actor_rdesc->inst_book, actor))
        return HARP_RESULT_INVALID_ARGUMENTS;

    // destroy actor
    actor->status |= HARP_STATUS_FLAG_TERMINATING;

    HarpResult res =
        actor_desc->pfn_destroy(
            runtime->core_handler,
            actor
        );

    actor->status &= ~HARP_STATUS_FLAG_TERMINATING;

    if(res == HARP_RESULT_OK) {
        actor->status &= ~HARP_STATUS_FLAG_VALID;

        /* remove actor from runtime list */
        for(uint64_t i = 0; i < actor_rdesc->actor_count; ++i) {
            if(actor_rdesc->actors[i] == actor) {

                actor_rdesc->actors[i] =
                    actor_rdesc->actors[actor_rdesc->actor_count-1];

                --actor_rdesc->actor_count;
                break;
            }
        }

        hmem_block_free_single(
            &actor_rdesc->inst_block,
            actor
        );

        if(handler_rdesc->actor_count > 0)
            --handler_rdesc->actor_count;
    }

    return res;
}


/* ================================================================================ */
/*  DIRECTORY PATH                                                                  */
/* ================================================================================ */

HarpResult runtime_get_executable_directory(
    HarpRuntime *runtime,
    const char **out_path
) {
    if(utils_check_runtime(runtime) != HARP_RESULT_OK)
        return HARP_RESULT_INVALID_ARGUMENTS;

    if(out_path == NULL)
        return HARP_RESULT_MISSING_OUTPUT;

    *out_path = NULL;

    if(runtime->executable_directory == NULL)
        return HARP_RESULT_CRITICAL_FAIL;

    *out_path = runtime->executable_directory;
    return HARP_RESULT_OK;
}

HarpResult runtime_get_working_directory(
    HarpRuntime *runtime,
    const char **out_path
) {
    if(utils_check_runtime(runtime) != HARP_RESULT_OK)
        return HARP_RESULT_INVALID_ARGUMENTS;

    if(out_path == NULL)
        return HARP_RESULT_MISSING_OUTPUT;

    *out_path = NULL;

    if(runtime->working_directory == NULL)
        return HARP_RESULT_CRITICAL_FAIL;

    *out_path = runtime->working_directory;
    return HARP_RESULT_OK;
}

HarpResult runtime_get_package_directory(
    HarpRuntime *runtime,
    const HarpName name,
    const char **out_path
) {
    if(utils_check_runtime(runtime) != HARP_RESULT_OK)
        return HARP_RESULT_INVALID_ARGUMENTS;

    if(out_path == NULL)
        return HARP_RESULT_MISSING_OUTPUT;

    *out_path = NULL;

    HarpLoadedPackage *pkg = harp_package_manager_get(&runtime->package_manager, name);

    if(pkg == NULL)
        return HARP_RESULT_NAME_NOT_FOUND;

    if(pkg->directory == NULL)
        return HARP_RESULT_CRITICAL_FAIL;

    *out_path = pkg->directory;
    return HARP_RESULT_OK;
}