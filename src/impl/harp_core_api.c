#include "harp_core_api.h"
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

HarpResult core_register_api(HarpCoreApi *api, const HarpApiDesc* desc, HarpApiBase** out_api) {
    if(utils_check_core_api(api) != HARP_RESULT_OK)
        return HARP_RESULT_INVALID_ARGUMENTS;

    HarpRuntime *runtime = ((HarpCoreApiImpl *)api)->p_runtime;
    return runtime_register_api(runtime, desc, out_api);
}
HarpResult core_register_handler(HarpCoreApi *api, const HarpHandlerDesc* desc) {
    if(utils_check_core_api(api) != HARP_RESULT_OK)
        return HARP_RESULT_INVALID_ARGUMENTS;

    HarpRuntime *runtime = ((HarpCoreApiImpl *)api)->p_runtime;
    return runtime_register_handler(runtime, desc);
}
HarpResult core_register_actor(HarpCoreApi *api, const HarpActorDesc* desc) {
    if(utils_check_core_api(api) != HARP_RESULT_OK)
        return HARP_RESULT_INVALID_ARGUMENTS;

    HarpRuntime *runtime = ((HarpCoreApiImpl *)api)->p_runtime;
    return runtime_register_actor(runtime, desc);
}

HarpResult core_get_api(HarpCoreApi *api, const HarpDependencyDesc *dependency, HarpApiBase **out_api) {
    if(utils_check_core_api(api) != HARP_RESULT_OK)
        return HARP_RESULT_INVALID_ARGUMENTS;

    HarpRuntime *runtime = ((HarpCoreApiImpl *)api)->p_runtime;
    return runtime_get_api(runtime, dependency, out_api);
}
HarpResult core_get_handler(HarpCoreApi *api, const HarpDependencyDesc *dependency, HarpHandlerBase** out_handler) {
    if(utils_check_core_api(api) != HARP_RESULT_OK)
        return HARP_RESULT_INVALID_ARGUMENTS;

    HarpRuntime *runtime = ((HarpCoreApiImpl *)api)->p_runtime;
    return runtime_get_handler(runtime, dependency, out_handler);
}
HarpResult core_get_api_desc(HarpCoreApi *api, const HarpName name, HarpApiDesc **out_desc) {
    if(utils_check_core_api(api) != HARP_RESULT_OK)
        return HARP_RESULT_INVALID_ARGUMENTS;

    HarpRuntime *runtime = ((HarpCoreApiImpl *)api)->p_runtime;
    return runtime_get_api_desc(runtime, name, out_desc);
}
HarpResult core_get_handler_desc(HarpCoreApi *api, const HarpName name, HarpHandlerDesc** out_desc) {
    if(utils_check_core_api(api) != HARP_RESULT_OK)
        return HARP_RESULT_INVALID_ARGUMENTS;

    HarpRuntime *runtime = ((HarpCoreApiImpl *)api)->p_runtime;
    return runtime_get_handler_desc(runtime, name, out_desc);
}
HarpResult core_get_actor_desc(HarpCoreApi *api, const HarpName name, HarpActorDesc** out_desc) {
    if(utils_check_core_api(api) != HARP_RESULT_OK)
        return HARP_RESULT_INVALID_ARGUMENTS;

    HarpRuntime *runtime = ((HarpCoreApiImpl *)api)->p_runtime;
    return runtime_get_actor_desc(runtime, name, out_desc);
}

HarpResult core_handler_initialize(HarpCoreApi *api, const HarpName name, const HarpCreatorBase* creator) {
    if(utils_check_core_api(api) != HARP_RESULT_OK)
        return HARP_RESULT_INVALID_ARGUMENTS;

    HarpRuntime *runtime = ((HarpCoreApiImpl *)api)->p_runtime;
    return runtime_handler_initialize(runtime, name, creator);
}
HarpResult core_handler_terminate(HarpCoreApi *api, const HarpName name) {
    if(utils_check_core_api(api) != HARP_RESULT_OK)
        return HARP_RESULT_INVALID_ARGUMENTS;

    HarpRuntime *runtime = ((HarpCoreApiImpl *)api)->p_runtime;
    return runtime_handler_terminate(runtime, name);
}

HarpResult core_actor_create(HarpCoreApi *api, const HarpName name, const HarpCreatorBase* creator, HarpActorBase** out_actor) {
    if(utils_check_core_api(api) != HARP_RESULT_OK)
        return HARP_RESULT_INVALID_ARGUMENTS;

    HarpRuntime *runtime = ((HarpCoreApiImpl *)api)->p_runtime;
    return runtime_actor_create(runtime, name, creator, out_actor);
}
HarpResult core_actor_destroy(HarpCoreApi *api, const HarpName name, HarpActorBase* actor) {
    if(utils_check_core_api(api) != HARP_RESULT_OK)
        return HARP_RESULT_INVALID_ARGUMENTS;

    HarpRuntime *runtime = ((HarpCoreApiImpl *)api)->p_runtime;
    return runtime_actor_destroy(runtime, name, actor);
}

HarpResult core_get_executable_directory(HarpCoreApi *api, const char **out_path) {
    if(utils_check_core_api(api) != HARP_RESULT_OK)
        return HARP_RESULT_INVALID_ARGUMENTS;

    HarpRuntime *runtime = ((HarpCoreApiImpl *)api)->p_runtime;
    return runtime_get_executable_directory(runtime, out_path);
}
HarpResult core_get_working_directory(HarpCoreApi *api, const char **out_path) {
    if(utils_check_core_api(api) != HARP_RESULT_OK)
        return HARP_RESULT_INVALID_ARGUMENTS;

    HarpRuntime *runtime = ((HarpCoreApiImpl *)api)->p_runtime;
    return runtime_get_working_directory(runtime, out_path);
}
HarpResult core_get_package_directory(HarpCoreApi *api, const HarpName name, const char **out_path) {
    if(utils_check_core_api(api) != HARP_RESULT_OK)
        return HARP_RESULT_INVALID_ARGUMENTS;

    HarpRuntime *runtime = ((HarpCoreApiImpl *)api)->p_runtime;
    return runtime_get_package_directory(runtime, name, out_path);
}

HarpResult core_get_actor_count(HarpCoreApi *api, const HarpName name, uint64_t *out_count) {
    if(utils_check_core_api(api) != HARP_RESULT_OK)
        return HARP_RESULT_INVALID_ARGUMENTS;

    HarpRuntime *runtime = ((HarpCoreApiImpl *)api)->p_runtime;
    return runtime_get_actor_count(runtime, name, out_count);
}
HarpResult core_get_actor_at(HarpCoreApi *api, const HarpName name, uint64_t index, HarpActorBase **out_actor) {
    if(utils_check_core_api(api) != HARP_RESULT_OK)
        return HARP_RESULT_INVALID_ARGUMENTS;

    HarpRuntime *runtime = ((HarpCoreApiImpl *)api)->p_runtime;
    return runtime_get_actor_at(runtime, name, index, out_actor);
}
HarpResult core_get_actors(HarpCoreApi *api, const HarpName name, uint64_t *inout_count, HarpActorBase **out_actors) {
    if(utils_check_core_api(api) != HARP_RESULT_OK)
        return HARP_RESULT_INVALID_ARGUMENTS;

    HarpRuntime *runtime = ((HarpCoreApiImpl *)api)->p_runtime;
    return runtime_get_actors(runtime, name, inout_count, out_actors);
}

/* ================================================================================ */
/*  HarpRuntime's impl                                                              */
/* ================================================================================ */

/* ================================================================================ */
/*  REGISTRATION                                                                    */
/* ================================================================================ */

HarpResult runtime_register_api(
    HarpRuntime *runtime,
    const HarpApiDesc *desc,
    HarpApiBase **out_api
) {
    if(utils_check_runtime(runtime) != HARP_RESULT_OK || utils_check_api_desc(desc) != HARP_RESULT_OK) 
        return HARP_RESULT_INVALID_ARGUMENTS;

    if(out_api == NULL)
        return HARP_RESULT_MISSING_OUTPUT;

    *out_api = NULL;

    // Allocate runtime descriptor
    HarpApiRuntimeDesc *rdesc =
        harp_alloc_global(
            runtime,
            sizeof(HarpApiRuntimeDesc),
            alignof(HarpApiRuntimeDesc)
        );

    if(rdesc == NULL)
        return HARP_RESULT_OUT_OF_MEMORY;

    memset(rdesc, 0, sizeof(HarpApiRuntimeDesc));

    // Allocate API instance
    HarpApiBase *rinst =
        harp_alloc_global(
            runtime,
            desc->instance_size,
            desc->instance_alignment
        );

    if(rinst == NULL)
        return HARP_RESULT_OUT_OF_MEMORY;

    memset(rinst, 0, desc->instance_size);

    // Copy descriptor metadata
    rdesc->_base = *desc;

    // Store name in registry memory
    HarpName rname =
        harp_registry_name(&runtime->registry, desc->name);

    if(rname == NULL)
        return HARP_RESULT_OUT_OF_MEMORY;

    rdesc->_base.name = rname;
    rdesc->instance = rinst;

    // Initialize API base
    rinst->version = desc->version;
    rinst->available = 0;

    // Bind runtime to registry
    HarpResult res =
        harp_registry_bind(
            &runtime->registry,
            rname,
            HARP_REGISTRY_ENTRY_TYPE_API,
            rdesc
        );

    if(res != HARP_RESULT_OK)
        return res;

    *out_api = rinst;

    return HARP_RESULT_OK;
}

HarpResult runtime_register_handler(
    HarpRuntime *runtime,
    const HarpHandlerDesc *desc
) {
    if(utils_check_runtime(runtime) != HARP_RESULT_OK || utils_check_handler_desc(desc) != HARP_RESULT_OK) 
        return HARP_RESULT_INVALID_ARGUMENTS;

    // Allocate runtime descriptor
    HarpHandlerRuntimeDesc *rdesc =
        harp_alloc_global(
            runtime,
            sizeof(HarpHandlerRuntimeDesc),
            alignof(HarpHandlerRuntimeDesc)
        );

    if(rdesc == NULL)
        return HARP_RESULT_OUT_OF_MEMORY;

    memset(rdesc, 0, sizeof(HarpHandlerRuntimeDesc));

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
    rdesc->_base = *desc;

    // Store name in registry memory
    HarpName rname =
        harp_registry_name(&runtime->registry, desc->name);

    if(rname == NULL)
        return HARP_RESULT_OUT_OF_MEMORY;

    rdesc->_base.name = rname;

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

        rdesc->_base.p_dependencies = rdeps;
    }

    // Initialize runtime state
    rdesc->state = HARP_RUNTIME_STATE_UNINITIALIZED;
    rdesc->dependent_count = 0;
    rdesc->actor_count = 0;

    // Initialize handler base
    rinst->name = rname;

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
    HarpActorRuntimeDesc *rdesc =
        harp_alloc_global(
            runtime,
            sizeof(HarpActorRuntimeDesc),
            alignof(HarpActorRuntimeDesc)
        );

    if(rdesc == NULL)
        return HARP_RESULT_OUT_OF_MEMORY;

    memset(rdesc, 0, sizeof(HarpActorRuntimeDesc));

    // Copy descriptor metadata
    rdesc->_base = *desc;

    // Store actor name in registry memory
    HarpName rname =
        harp_registry_name(&runtime->registry, desc->name);

    if(rname == NULL)
        return HARP_RESULT_OUT_OF_MEMORY;

    rdesc->_base.name = rname;
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

        rdesc->_base.parent_handler = rparent;
    }

    // Setup actor instance storage
    if(!hmem_setup_book(&rdesc->inst_book, 16))
        return HARP_RESULT_FAILED;

    if(!hmem_setup_block(
        &rdesc->inst_block,
        &rdesc->inst_book,
        desc->instance_size,
        desc->instance_alignment
    )) {
        hmem_teardown_book(&rdesc->inst_book);
        return HARP_RESULT_FAILED;
    }

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

HarpResult runtime_get_api(
    HarpRuntime *runtime,
    const HarpDependencyDesc *dependency,
    HarpApiBase **out_api
) {
    if(utils_check_runtime(runtime) != HARP_RESULT_OK || utils_check_dependency(dependency) != HARP_RESULT_OK) 
        return HARP_RESULT_INVALID_ARGUMENTS;

    if(out_api == NULL)
        return HARP_RESULT_MISSING_OUTPUT;

    *out_api = NULL;

    HarpApiRuntimeDesc *rdesc =
        harp_registry_get(
            &runtime->registry,
            dependency->name,
            HARP_REGISTRY_ENTRY_TYPE_API
        );

    if(rdesc == NULL)
        return HARP_RESULT_NAME_NOT_FOUND;

    if(rdesc->instance == NULL)
        return HARP_RESULT_CRITICAL_FAIL;

    if(utils_dependency_matches(dependency, rdesc->_base.version) != HARP_RESULT_OK)
        return HARP_RESULT_DEPENDENCY_VERSION_MISMATCH;

    *out_api = rdesc->instance;
    return HARP_RESULT_OK;
}

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

    HarpHandlerRuntimeDesc *rdesc =
        harp_registry_get(
            &runtime->registry,
            dependency->name,
            HARP_REGISTRY_ENTRY_TYPE_HANDLER
        );

    if(rdesc == NULL)
        return HARP_RESULT_NAME_NOT_FOUND;

    if(rdesc->instance == NULL)
        return HARP_RESULT_CRITICAL_FAIL;

    if(utils_dependency_matches(dependency, rdesc->_base.version) != HARP_RESULT_OK)
        return HARP_RESULT_DEPENDENCY_VERSION_MISMATCH;

    *out_handler = rdesc->instance;
    return HARP_RESULT_OK;
}

HarpResult runtime_get_api_desc(
    HarpRuntime *runtime,
    const HarpName name,
    HarpApiDesc **out_desc
) {
    if(utils_check_runtime(runtime) != HARP_RESULT_OK || name == NULL) 
        return HARP_RESULT_INVALID_ARGUMENTS;

    if(out_desc == NULL)
        return HARP_RESULT_MISSING_OUTPUT;

    *out_desc = NULL;

    HarpApiRuntimeDesc *rdesc =
        harp_registry_get(
            &runtime->registry,
            name,
            HARP_REGISTRY_ENTRY_TYPE_API
        );

    if(rdesc == NULL)
        return HARP_RESULT_NAME_NOT_FOUND;

    *out_desc = &rdesc->_base;
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

    HarpHandlerRuntimeDesc *rdesc =
        harp_registry_get(
            &runtime->registry,
            name,
            HARP_REGISTRY_ENTRY_TYPE_HANDLER
        );

    if(rdesc == NULL)
        return HARP_RESULT_NAME_NOT_FOUND;

    *out_desc = &rdesc->_base;
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

    HarpActorRuntimeDesc *rdesc =
        harp_registry_get(
            &runtime->registry,
            name,
            HARP_REGISTRY_ENTRY_TYPE_ACTOR
        );

    if(rdesc == NULL)
        return HARP_RESULT_NAME_NOT_FOUND;

    *out_desc = &rdesc->_base;
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
    HarpHandlerRuntimeDesc *rdesc =
        harp_registry_get(
            &runtime->registry,
            name,
            HARP_REGISTRY_ENTRY_TYPE_HANDLER
        );

    if(rdesc == NULL)
        return HARP_RESULT_NAME_NOT_FOUND;

    HarpHandlerDesc *desc = &rdesc->_base;
    HarpHandlerBase *base = rdesc->instance;

    if(base == NULL)
        return HARP_RESULT_CRITICAL_FAIL;

    // check state
    if(rdesc->state == HARP_RUNTIME_STATE_INITIALIZED)
        return HARP_RESULT_OK;

    if(rdesc->state != HARP_RUNTIME_STATE_UNINITIALIZED)
        return HARP_RESULT_INVALID_STATE;

    // check dependencies
    for(uint64_t i = 0; i < desc->dependency_count; ++i) {
        HarpDependencyDesc *ddep = &desc->p_dependencies[i];

        HarpHandlerRuntimeDesc *drdesc =
            harp_registry_get(
                &runtime->registry,
                ddep->name,
                HARP_REGISTRY_ENTRY_TYPE_HANDLER
            );

        if(drdesc == NULL)
            return HARP_RESULT_DEPENDENCY_NOT_FOUND;

        if(utils_dependency_matches(ddep, drdesc->_base.version) != HARP_RESULT_OK)
            return HARP_RESULT_DEPENDENCY_VERSION_MISMATCH;

        if(drdesc->state == HARP_RUNTIME_STATE_INITIALIZING)
            return HARP_RESULT_DEPENDENCY_CYCLE;

        if(drdesc->state != HARP_RUNTIME_STATE_INITIALIZED)
            return HARP_RESULT_DEPENDENCY_UNINITIALIZED;
    }

    // start initialization
    rdesc->state = HARP_RUNTIME_STATE_INITIALIZING;

    HarpResult res =
        desc->pfn_init(
            runtime->core_api,
            base,
            (HarpCreatorBase*)creator
        );

    if(res == HARP_RESULT_OK) {
        rdesc->state = HARP_RUNTIME_STATE_INITIALIZED;

        for(uint64_t i = 0; i < desc->dependency_count; ++i) {
            HarpDependencyDesc *ddep = &desc->p_dependencies[i];

            HarpHandlerRuntimeDesc *drdesc =
                harp_registry_get(
                    &runtime->registry,
                    ddep->name,
                    HARP_REGISTRY_ENTRY_TYPE_HANDLER
                );

            if(drdesc != NULL)
                ++drdesc->dependent_count;
        }
    } else {
        rdesc->state = HARP_RUNTIME_STATE_UNINITIALIZED;
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
    HarpHandlerRuntimeDesc *rdesc =
        harp_registry_get(
            &runtime->registry,
            name,
            HARP_REGISTRY_ENTRY_TYPE_HANDLER
        );

    if(rdesc == NULL)
        return HARP_RESULT_NAME_NOT_FOUND;

    HarpHandlerDesc *desc = &rdesc->_base;
    HarpHandlerBase *base = rdesc->instance;

    if(base == NULL)
        return HARP_RESULT_CRITICAL_FAIL;

    // check state
    if(rdesc->state == HARP_RUNTIME_STATE_UNINITIALIZED)
        return HARP_RESULT_OK;

    if(rdesc->state != HARP_RUNTIME_STATE_INITIALIZED)
        return HARP_RESULT_INVALID_STATE;

    // check dependents
    if(rdesc->dependent_count != 0)
        return HARP_RESULT_EXISTENT_DEPENDENTS;

    if(rdesc->actor_count != 0)
        return HARP_RESULT_EXISTENT_ACTORS;

    // start termination
    rdesc->state = HARP_RUNTIME_STATE_TERMINATING;

    HarpResult res =
        desc->pfn_term(
            runtime->core_api,
            base
        );

    if(res == HARP_RESULT_OK) {
        rdesc->state = HARP_RUNTIME_STATE_UNINITIALIZED;

        for(uint64_t i = 0; i < desc->dependency_count; ++i) {
            HarpDependencyDesc *ddep = &desc->p_dependencies[i];

            HarpHandlerRuntimeDesc *drdesc =
                harp_registry_get(
                    &runtime->registry,
                    ddep->name,
                    HARP_REGISTRY_ENTRY_TYPE_HANDLER
                );

            if(drdesc != NULL && drdesc->dependent_count > 0)
                --drdesc->dependent_count;
        }
    } else {
        rdesc->state = HARP_RUNTIME_STATE_INITIALIZED;
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
    HarpActorRuntimeDesc *actor_rdesc =
        harp_registry_get(
            &runtime->registry,
            name,
            HARP_REGISTRY_ENTRY_TYPE_ACTOR
        );

    if(actor_rdesc == NULL)
        return HARP_RESULT_NAME_NOT_FOUND;

    HarpActorDesc *actor_desc = &actor_rdesc->_base;

    // find parent handler runtime
    HarpHandlerRuntimeDesc *handler_rdesc =
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

    HarpHandlerDesc *handler_desc = &handler_rdesc->_base;

    // check handler state
    if(handler_rdesc->state != HARP_RUNTIME_STATE_INITIALIZED)
        return HARP_RESULT_INVALID_STATE;

    // check dependencies
    for(uint64_t i = 0; i < handler_desc->dependency_count; ++i) {
        HarpDependencyDesc *ddep = &handler_desc->p_dependencies[i];

        HarpHandlerRuntimeDesc *drdesc =
            harp_registry_get(
                &runtime->registry,
                ddep->name,
                HARP_REGISTRY_ENTRY_TYPE_HANDLER
            );

        if(drdesc == NULL)
            return HARP_RESULT_DEPENDENCY_NOT_FOUND;

        if(drdesc->_base.version < ddep->min_version)
            return HARP_RESULT_DEPENDENCY_VERSION_MISMATCH;

        if(ddep->max_version != 0 &&
           drdesc->_base.version > ddep->max_version)
            return HARP_RESULT_DEPENDENCY_VERSION_MISMATCH;

        if(drdesc->state == HARP_RUNTIME_STATE_INITIALIZING)
            return HARP_RESULT_DEPENDENCY_CYCLE;

        if(drdesc->state != HARP_RUNTIME_STATE_INITIALIZED)
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

    actor_base->name = actor_desc->name;
    actor_base->p_handler = handler_base;

    // create actor
    HarpResult res =
        actor_desc->pfn_create(
            runtime->core_api,
            actor_base,
            (HarpCreatorBase*)creator
        );

    if(res == HARP_RESULT_OK) {
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
    HarpActorRuntimeDesc *actor_rdesc =
        harp_registry_get(
            &runtime->registry,
            name,
            HARP_REGISTRY_ENTRY_TYPE_ACTOR
        );

    if(actor_rdesc == NULL)
        return HARP_RESULT_NAME_NOT_FOUND;

    HarpActorDesc *actor_desc = &actor_rdesc->_base;

    // find parent handler runtime
    HarpHandlerRuntimeDesc *handler_rdesc =
        harp_registry_get(
            &runtime->registry,
            actor_desc->parent_handler,
            HARP_REGISTRY_ENTRY_TYPE_HANDLER
        );

    if(handler_rdesc == NULL)
        return HARP_RESULT_NAME_NOT_FOUND;

    // check state
    if(handler_rdesc->state != HARP_RUNTIME_STATE_INITIALIZED)
        return HARP_RESULT_INVALID_STATE;

    // validate actor ownership
    if(!hmem_book_contains(&actor_rdesc->inst_book, actor))
        return HARP_RESULT_INVALID_ARGUMENTS;

    // destroy actor
    HarpResult res =
        actor_desc->pfn_destroy(
            runtime->core_api,
            actor
        );

    if(res == HARP_RESULT_OK) {
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

    HarpPackageRuntimeDesc *rdesc =
        harp_registry_get(
            &runtime->registry,
            name,
            HARP_REGISTRY_ENTRY_TYPE_PACKAGE
        );

    if(rdesc == NULL)
        return HARP_RESULT_NAME_NOT_FOUND;

    if(rdesc->directory == NULL)
        return HARP_RESULT_CRITICAL_FAIL;

    *out_path = rdesc->directory;
    return HARP_RESULT_OK;
}


/* ================================================================================ */
/*  ACTOR ENUMERATION                                                               */
/* ================================================================================ */

HarpResult runtime_get_actor_count(
    HarpRuntime *runtime,
    const HarpName name,
    uint64_t *out_count
) {
    if(utils_check_runtime(runtime) != HARP_RESULT_OK || name == NULL)
        return HARP_RESULT_INVALID_ARGUMENTS;

    if(out_count == NULL)
        return HARP_RESULT_MISSING_OUTPUT;

    HarpActorRuntimeDesc *rdesc = harp_registry_get(&runtime->registry, name, HARP_REGISTRY_ENTRY_TYPE_ACTOR); 
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

    HarpActorRuntimeDesc *rdesc = harp_registry_get(&runtime->registry, name, HARP_REGISTRY_ENTRY_TYPE_ACTOR); 
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

    HarpActorRuntimeDesc *rdesc = harp_registry_get(&runtime->registry, name, HARP_REGISTRY_ENTRY_TYPE_ACTOR); 
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