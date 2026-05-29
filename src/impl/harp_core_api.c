#include "harp_core_api.h"
#include "runtime/harp_runtime.h"

#include <hmem/hmem_block.h>
#include <hmem/hmem_os.h>
#include <hmem/utils/hmem_align.h>

#include <string.h>
#include <stdalign.h>


static HarpRuntime* harp_runtime_from_handler(HarpHandlerBase *handler) {
    return (HarpRuntime*)handler;
}


/* ================================================================================ */
/*  REGISTRATION                                                                    */
/* ================================================================================ */

HarpResult register_api(
    HarpHandlerBase *handler,
    const HarpApiDesc *desc,
    HarpApiBase **out_api
) {
    if(handler == NULL || desc == NULL)
        return HARP_RESULT_INVALID_ARGUMENTS;

    if(out_api == NULL)
        return HARP_RESULT_MISSING_OUTPUT;

    *out_api = NULL;

    HarpRuntime *runtime = (HarpRuntime*)handler;

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

HarpResult register_handler(
    HarpHandlerBase *handler,
    const HarpHandlerDesc *desc
) {
    if(handler == NULL || desc == NULL)
        return HARP_RESULT_INVALID_ARGUMENTS;

    HarpRuntime *runtime = (HarpRuntime*)handler;

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

HarpResult register_actor(
    HarpHandlerBase *handler,
    const HarpActorDesc *desc
) {
    if(handler == NULL || desc == NULL)
        return HARP_RESULT_INVALID_ARGUMENTS;

    HarpRuntime *runtime = (HarpRuntime*)handler;

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
    rdesc->growth_index = 0;

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

    return HARP_RESULT_OK;
}


/* ================================================================================ */
/*  RETRIEVAL                                                                       */
/* ================================================================================ */

HarpResult get_api(
    HarpHandlerBase *handler,
    const HarpName name,
    HarpApiBase **out_api
) {
    if(handler == NULL || name == NULL)
        return HARP_RESULT_INVALID_ARGUMENTS;

    if(out_api == NULL)
        return HARP_RESULT_MISSING_OUTPUT;

    *out_api = NULL;

    HarpRuntime *runtime = (HarpRuntime*)handler;

    HarpApiRuntimeDesc *rdesc =
        harp_registry_get(
            &runtime->registry,
            name,
            HARP_REGISTRY_ENTRY_TYPE_API
        );

    if(rdesc == NULL)
        return HARP_RESULT_NAME_NOT_FOUND;

    if(rdesc->instance == NULL)
        return HARP_RESULT_CRITICAL_FAIL;

    *out_api = rdesc->instance;
    return HARP_RESULT_OK;
}

HarpResult get_handler(
    HarpHandlerBase *handler,
    const HarpName name,
    HarpHandlerBase **out_handler
) {
    if(handler == NULL || name == NULL)
        return HARP_RESULT_INVALID_ARGUMENTS;

    if(out_handler == NULL)
        return HARP_RESULT_MISSING_OUTPUT;

    *out_handler = NULL;

    HarpRuntime *runtime = (HarpRuntime*)handler;

    HarpHandlerRuntimeDesc *rdesc =
        harp_registry_get(
            &runtime->registry,
            name,
            HARP_REGISTRY_ENTRY_TYPE_HANDLER
        );

    if(rdesc == NULL)
        return HARP_RESULT_NAME_NOT_FOUND;

    if(rdesc->instance == NULL)
        return HARP_RESULT_CRITICAL_FAIL;

    *out_handler = rdesc->instance;
    return HARP_RESULT_OK;
}

HarpResult get_api_desc(
    HarpHandlerBase *handler,
    const HarpName name,
    HarpApiDesc **out_desc
) {
    if(handler == NULL || name == NULL)
        return HARP_RESULT_INVALID_ARGUMENTS;

    if(out_desc == NULL)
        return HARP_RESULT_MISSING_OUTPUT;

    *out_desc = NULL;

    HarpRuntime *runtime = (HarpRuntime*)handler;

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

HarpResult get_handler_desc(
    HarpHandlerBase *handler,
    const HarpName name,
    HarpHandlerDesc **out_desc
) {
    if(handler == NULL || name == NULL)
        return HARP_RESULT_INVALID_ARGUMENTS;

    if(out_desc == NULL)
        return HARP_RESULT_MISSING_OUTPUT;

    *out_desc = NULL;

    HarpRuntime *runtime = (HarpRuntime*)handler;

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

HarpResult get_actor_desc(
    HarpHandlerBase *handler,
    const HarpName name,
    HarpActorDesc **out_desc
) {
    if(handler == NULL || name == NULL)
        return HARP_RESULT_INVALID_ARGUMENTS;

    if(out_desc == NULL)
        return HARP_RESULT_MISSING_OUTPUT;

    *out_desc = NULL;

    HarpRuntime *runtime = (HarpRuntime*)handler;

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

/* static HarpResult handler_check_dependencies(HarpRuntime *runtime, HarpHandlerDesc *desc) {
    if(runtime == NULL || desc == NULL) return HARP_RESULT_INVALID_ARGUMENTS;

    for(uint64_t i = 0; i < desc->dependency_count; ++i) {
        HarpDependencyDesc *ddep = &desc->p_dependencies[i];
        HarpRegistryEntry *dentry =
            harp_registry_find(runtime, &runtime->registry, ddep->name);
        if(dentry == NULL || dentry->type != HARP_REGISTRY_ENTRY_TYPE_HANDLER || dentry->p_desc == NULL)
            return HARP_RESULT_DEPENDENCY_NOT_FOUND;

        HarpHandlerRuntimeDesc *drdesc = dentry->p_desc;

        if(drdesc->_base.version < ddep->min_version)
            return HARP_RESULT_DEPENDENCY_VERSION_MISMATCH;

        if(ddep->max_version != 0 &&
           drdesc->_base.version > ddep->max_version)
            return HARP_RESULT_DEPENDENCY_VERSION_MISMATCH;

        if(drdesc->state != HARP_RUNTIME_STATE_INITIALIZED)
            return HARP_RESULT_DEPENDENCY_UNINITIALIZED;
    }

    return HARP_RESULT_OK;
} */

HarpResult handler_initialize(
    HarpHandlerBase *handler,
    const HarpName name,
    const HarpCreatorBase *creator
) {
    if(handler == NULL || name == NULL || creator == NULL)
        return HARP_RESULT_INVALID_ARGUMENTS;

    HarpRuntime *runtime = (HarpRuntime*)handler;

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

HarpResult handler_terminate(
    HarpHandlerBase *handler,
    const HarpName name
) {
    if(handler == NULL || name == NULL)
        return HARP_RESULT_INVALID_ARGUMENTS;

    HarpRuntime *runtime = (HarpRuntime*)handler;

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

HarpResult actor_create(
    HarpHandlerBase *handler,
    const HarpName name,
    const HarpCreatorBase *creator,
    HarpActorBase **out_actor
) {
    if(handler == NULL || name == NULL || creator == NULL)
        return HARP_RESULT_INVALID_ARGUMENTS;

    if(out_actor == NULL)
        return HARP_RESULT_MISSING_OUTPUT;

    HarpRuntime *runtime = (HarpRuntime*)handler;

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

HarpResult actor_destroy(
    HarpHandlerBase *handler,
    const HarpName name,
    HarpActorBase *actor
) {
    if(handler == NULL || name == NULL || actor == NULL)
        return HARP_RESULT_INVALID_ARGUMENTS;

    HarpRuntime *runtime = (HarpRuntime*)handler;

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
        hmem_block_free_single(
            &actor_rdesc->inst_block,
            actor
        );

        if(handler_rdesc->actor_count > 0)
            --handler_rdesc->actor_count;
    }

    return res;
}