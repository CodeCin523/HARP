#include "harp_core_api.h"
#include "runtime/harp_runtime.h"

#include "hmem/hmem_os.h"

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

    // Reserve registry entry FIRST
    HarpRegistryEntry *entry =
        harp_registry_insert(runtime, &runtime->registry, desc->name);
    if(entry == NULL)
        return HARP_RESULT_NAME_EXISTS;

    // Allocate runtime descriptor
    HarpApiRuntimeDesc *rdesc =
        harp_runtime_global_alloc(
            runtime,
            sizeof(HarpApiRuntimeDesc),
            alignof(HarpApiRuntimeDesc)
        );

    if(rdesc == NULL)
        return HARP_RESULT_OUT_OF_MEMORY;

    memset(rdesc, 0, sizeof(HarpApiRuntimeDesc));

    // Allocate API instance
    HarpApiBase *rinst =
        harp_runtime_global_alloc(
            runtime,
            desc->instance_size,
            desc->instance_alignment
        );

    if(rinst == NULL)
        return HARP_RESULT_OUT_OF_MEMORY;

    memset(rinst, 0, desc->instance_size);

    // Copy descriptor metadata
    rdesc->_base = *desc;

    // Copy name into runtime-owned memory
    size_t name_len = strlen(desc->name) + 1;

    char *rname =
        harp_runtime_global_alloc(
            runtime,
            name_len,
            alignof(char)
        );

    if(rname == NULL)
        return HARP_RESULT_OUT_OF_MEMORY;

    memcpy(rname, desc->name, name_len);

    rdesc->_base.name = rname;

    // Initialize API base
    rinst->version = desc->version;
    rinst->available = 0;

    // Finalize registry entry
    entry->p_desc = rdesc;
    entry->p_inst = rinst;
    entry->type = HARP_REGISTRY_ENTRY_TYPE_API;

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

    // Reserve registry entry FIRST
    HarpRegistryEntry *entry =
        harp_registry_insert(runtime, &runtime->registry, desc->name);
    if(entry == NULL)
        return HARP_RESULT_NAME_EXISTS;

    // Allocate runtime descriptor
    HarpHandlerRuntimeDesc *rdesc =
        harp_runtime_global_alloc(
            runtime,
            sizeof(HarpHandlerRuntimeDesc),
            alignof(HarpHandlerRuntimeDesc)
        );

    if(rdesc == NULL)
        return HARP_RESULT_OUT_OF_MEMORY;

    memset(rdesc, 0, sizeof(HarpHandlerRuntimeDesc));

    // Allocate handler instance
    HarpHandlerBase *rinst =
        harp_runtime_global_alloc(
            runtime,
            desc->instance_size,
            desc->instance_alignment
        );

    if(rinst == NULL)
        return HARP_RESULT_OUT_OF_MEMORY;

    memset(rinst, 0, desc->instance_size);

    // Copy descriptor metadata
    rdesc->_base = *desc;

    // Copy name into runtime-owned memory
    size_t name_len = strlen(desc->name) + 1;

    char *rname =
        harp_runtime_global_alloc(
            runtime,
            name_len,
            alignof(char)
        );

    if(rname == NULL)
        return HARP_RESULT_OUT_OF_MEMORY;

    memcpy(rname, desc->name, name_len);

    rdesc->_base.name = rname;

    // Copy dependencies into runtime-owned memory
    if(desc->dependency_count > 0 && desc->p_dependencies != NULL) {
        size_t deps_size =
            sizeof(HarpDependencyDesc) * desc->dependency_count;

        HarpDependencyDesc *rdeps =
            harp_runtime_global_alloc(
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

        rdesc->_base.p_dependencies = rdeps;
    }

    // Initialize runtime state
    rdesc->state = HARP_RUNTIME_STATE_UNINITIALIZED;
    rdesc->dependent_count = 0;
    rdesc->actor_count = 0;

    // Initialize handler base
    rinst->name = rdesc->_base.name;

    // Finalize registry entry
    entry->p_desc = rdesc;
    entry->p_inst = rinst;
    entry->type = HARP_REGISTRY_ENTRY_TYPE_HANDLER;

    return HARP_RESULT_OK;
}

HarpResult register_actor(
    HarpHandlerBase *handler,
    const HarpActorDesc *desc
) {
    if(handler == NULL || desc == NULL)
        return HARP_RESULT_INVALID_ARGUMENTS;

    HarpRuntime *runtime = (HarpRuntime*)handler;

    // Reserve registry entry FIRST
    HarpRegistryEntry *entry =
        harp_registry_insert(runtime, &runtime->registry, desc->name);
    if(entry == NULL)
        return HARP_RESULT_NAME_EXISTS;

    // Allocate actor runtime descriptor
    HarpActorRuntimeDesc *rdesc =
        harp_runtime_global_alloc(
            runtime,
            sizeof(HarpActorRuntimeDesc),
            alignof(HarpActorRuntimeDesc)
        );

    if(rdesc == NULL)
        return HARP_RESULT_OUT_OF_MEMORY;

    memset(rdesc, 0, sizeof(HarpActorRuntimeDesc));

    // Copy descriptor metadata
    rdesc->_base = *desc;

    // Copy actor name into runtime-owned memory
    size_t name_len = strlen(desc->name) + 1;

    char *rname =
        harp_runtime_global_alloc(
            runtime,
            name_len,
            alignof(char)
        );

    if(rname == NULL)
        return HARP_RESULT_OUT_OF_MEMORY;

    memcpy(rname, desc->name, name_len);

    rdesc->_base.name = rname;

    // Copy parent name into runtime-owned memory
    if(desc->parent_handler != NULL) {
        size_t parent_len = strlen(desc->parent_handler) + 1;

        char *rparent =
            harp_runtime_global_alloc(
                runtime,
                parent_len,
                alignof(char)
            );

        if(rparent == NULL)
            return HARP_RESULT_OUT_OF_MEMORY;

        memcpy(rparent, desc->parent_handler, parent_len);

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

    // Finalize registry entry
    entry->p_desc = rdesc;
    entry->p_inst = NULL;
    entry->type = HARP_REGISTRY_ENTRY_TYPE_ACTOR;

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

    // Try to find entry
    HarpRegistryEntry *entry =
        harp_registry_find(runtime, &runtime->registry, name);
    if(entry == NULL)
        return HARP_RESULT_NAME_NOT_FOUND;
    if(entry->type != HARP_REGISTRY_ENTRY_TYPE_API)
        return HARP_RESULT_NAME_TYPE_MISMATCH;
    if(entry->p_inst == NULL)
        return HARP_RESULT_CRITICAL_FAIL;

    *out_api = entry->p_inst;
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

    // Try to find entry
    HarpRegistryEntry *entry =
        harp_registry_find(runtime, &runtime->registry, name);
    if(entry == NULL)
        return HARP_RESULT_NAME_NOT_FOUND;

    if(entry->type != HARP_REGISTRY_ENTRY_TYPE_HANDLER)
        return HARP_RESULT_NAME_TYPE_MISMATCH;
    if(entry->p_inst == NULL)
        return HARP_RESULT_CRITICAL_FAIL;

    *out_handler = entry->p_inst;
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

    // Try to find entry
    HarpRegistryEntry *entry =
        harp_registry_find(runtime, &runtime->registry, name);
    if(entry == NULL)
        return HARP_RESULT_NAME_NOT_FOUND;

    if(entry->type != HARP_REGISTRY_ENTRY_TYPE_API)
        return HARP_RESULT_NAME_TYPE_MISMATCH;
    if(entry->p_desc == NULL)
        return HARP_RESULT_CRITICAL_FAIL;

    *out_desc = &((HarpApiRuntimeDesc*)entry->p_desc)->_base;
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

    // Try to find entry
    HarpRegistryEntry *entry =
        harp_registry_find(runtime, &runtime->registry, name);
    if(entry == NULL)
        return HARP_RESULT_NAME_NOT_FOUND;
    
    if(entry->type != HARP_REGISTRY_ENTRY_TYPE_HANDLER)
        return HARP_RESULT_NAME_TYPE_MISMATCH;
    if(entry->p_desc == NULL)
        return HARP_RESULT_CRITICAL_FAIL;

    *out_desc = &((HarpHandlerRuntimeDesc*)entry->p_desc)->_base;
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

    // Try to find entry
    HarpRegistryEntry *entry =
        harp_registry_find(runtime, &runtime->registry, name);
    if(entry == NULL)
        return HARP_RESULT_NAME_NOT_FOUND;
    
    if(entry->type != HARP_REGISTRY_ENTRY_TYPE_ACTOR)
        return HARP_RESULT_NAME_TYPE_MISMATCH;
    if(entry->p_desc == NULL)
        return HARP_RESULT_CRITICAL_FAIL;

    *out_desc = &((HarpActorRuntimeDesc*)entry->p_desc)->_base;
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

    // find everything
    HarpRegistryEntry *entry =
        harp_registry_find(runtime, &runtime->registry, name);
    if(entry == NULL)
        return HARP_RESULT_NAME_NOT_FOUND;
    if(entry->type != HARP_REGISTRY_ENTRY_TYPE_HANDLER)
        return HARP_RESULT_NAME_TYPE_MISMATCH;

    HarpHandlerRuntimeDesc *rdesc = entry->p_desc;
    if(rdesc == NULL)
        return HARP_RESULT_CRITICAL_FAIL;
    HarpHandlerDesc *desc = &rdesc->_base;
    HarpHandlerBase *base = entry->p_inst;
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
        HarpRegistryEntry *dentry =
            harp_registry_find(runtime, &runtime->registry, ddep->name);
        if(dentry == NULL)
            return HARP_RESULT_DEPENDENCY_NOT_FOUND;
        if(dentry->type != HARP_REGISTRY_ENTRY_TYPE_HANDLER)
            return HARP_RESULT_NAME_TYPE_MISMATCH; // should be something else
        if(dentry->p_desc == NULL)
            return HARP_RESULT_CRITICAL_FAIL;

        HarpHandlerRuntimeDesc *drdesc = dentry->p_desc;

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
    HarpResult res = desc->pfn_init(runtime->core_api, base, (HarpCreatorBase*)creator);
    
    if(res == HARP_RESULT_OK) {
        rdesc->state = HARP_RUNTIME_STATE_INITIALIZED;

        for(uint64_t i = 0; i < desc->dependency_count; ++i) {
            HarpDependencyDesc *ddep = &desc->p_dependencies[i];
            HarpRegistryEntry *dentry =
                harp_registry_find(runtime, &runtime->registry, ddep->name);
            if(dentry == NULL || dentry->type != HARP_REGISTRY_ENTRY_TYPE_HANDLER || dentry->p_desc == NULL)
                continue; // should never happen

            HarpHandlerRuntimeDesc *drdesc = dentry->p_desc;
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

    // find everything
    HarpRegistryEntry *entry =
        harp_registry_find(runtime, &runtime->registry, name);
    if(entry == NULL)
        return HARP_RESULT_NAME_NOT_FOUND;
    if(entry->type != HARP_REGISTRY_ENTRY_TYPE_HANDLER)
        return HARP_RESULT_NAME_TYPE_MISMATCH;

    HarpHandlerRuntimeDesc *rdesc = entry->p_desc;
    if(rdesc == NULL)
        return HARP_RESULT_CRITICAL_FAIL;
    HarpHandlerDesc *desc = &rdesc->_base;
    HarpHandlerBase *base = entry->p_inst;
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
    HarpResult res = desc->pfn_term(runtime->core_api, base);

    if(res == HARP_RESULT_OK) {
        rdesc->state = HARP_RUNTIME_STATE_UNINITIALIZED;

        for(uint64_t i = 0; i < desc->dependency_count; ++i) {
            HarpDependencyDesc *ddep = &desc->p_dependencies[i];
            HarpRegistryEntry *dentry =
                harp_registry_find(runtime, &runtime->registry, ddep->name);
            if(dentry == NULL || dentry->type != HARP_REGISTRY_ENTRY_TYPE_HANDLER || dentry->p_desc == NULL)
                continue; // should never happen

            HarpHandlerRuntimeDesc *drdesc = dentry->p_desc;
            if(drdesc->dependent_count > 0)
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

}

HarpResult actor_destroy(
    HarpHandlerBase *handler,
    const HarpName name,
    HarpActorBase *actor
) {

}