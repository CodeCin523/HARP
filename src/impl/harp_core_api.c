#include "harp_core_api.h"
#include "runtime/harp_runtime.h"

#include "hmem/hmem_os.h"
#include <string.h>


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
        harp_registry_insert( runtime, &runtime->registry, desc->name );
    if(entry == NULL)
        return HARP_RESULT_NAME_EXISTS;

    // Allocate descriptor implementation
    HarpApiRuntimeDesc *rdesc =
        harp_runtime_global_alloc( runtime, sizeof(HarpApiRuntimeDesc), alignof(HarpApiRuntimeDesc) );
    if(rdesc == NULL)
        return HARP_RESULT_FAILED;

    memset(rdesc, 0, sizeof(HarpApiRuntimeDesc));

    // Allocate API instance
    HarpApiBase *rinst =
        harp_runtime_global_alloc( runtime, desc->instance_size, desc->instance_alignment );
    if(rinst == NULL)
        return HARP_RESULT_FAILED;

    memset(rinst, 0, desc->instance_size);

    // Copy descriptor metadata
    rdesc->_base = *desc;

    // Initialize API base
    rinst->version = desc->version;
    rinst->available = 1;

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
        harp_registry_insert( runtime, &runtime->registry, desc->name );
    if(entry == NULL)
        return HARP_RESULT_NAME_EXISTS;

    // Allocate runtime descriptor
    HarpHandlerRuntimeDesc *rdesc =
        harp_runtime_global_alloc( runtime, sizeof(HarpHandlerRuntimeDesc), alignof(HarpHandlerRuntimeDesc) );
    if(rdesc == NULL)
        return HARP_RESULT_FAILED;

    memset(rdesc, 0, sizeof(HarpHandlerRuntimeDesc));

    // Allocate handler instance
    HarpHandlerBase *rinst =
        harp_runtime_global_alloc( runtime, desc->instance_size, desc->instance_alignment );
    if(rinst == NULL)
        return HARP_RESULT_FAILED;

    memset(rinst, 0, desc->instance_size);

    // Copy descriptor metadata
    rdesc->_base = *desc;

    // Initialize handler base
    rinst->name = desc->name;

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
        harp_registry_insert( runtime, &runtime->registry, desc->name );
    if(entry == NULL)
        return HARP_RESULT_NAME_EXISTS;

    // Allocate actor runtime descriptor
    HarpActorRuntimeDesc *rdesc =
        harp_runtime_global_alloc( runtime, sizeof(HarpActorRuntimeDesc), alignof(HarpActorRuntimeDesc) );
    if(rdesc == NULL)
        return HARP_RESULT_FAILED;

    memset(rdesc, 0, sizeof(HarpActorRuntimeDesc));

    // Copy descriptor metadata
    rdesc->_base = *desc;

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

}

HarpResult get_handler(
    HarpHandlerBase *handler,
    const HarpName name,
    HarpHandlerBase **out_handler
) {

}

HarpResult get_api_desc(
    HarpHandlerBase *handler,
    const HarpName name,
    HarpApiDesc **out_desc
) {

}

HarpResult get_handler_desc(
    HarpHandlerBase *handler,
    const HarpName name,
    HarpHandlerDesc **out_desc
) {

}

HarpResult get_actor_desc(
    HarpHandlerBase *handler,
    const HarpName name,
    HarpActorDesc **out_desc
) {

}


/* ================================================================================ */
/*  LIFECYCLE                                                                       */
/* ================================================================================ */

HarpResult handler_initialize(
    HarpHandlerBase *handler,
    const HarpName name,
    const HarpCreatorBase *creator
) {

}

HarpResult handler_terminate(
    HarpHandlerBase *handler,
    const HarpName name
) {

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