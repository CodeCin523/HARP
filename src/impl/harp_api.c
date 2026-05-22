#include "harp_api.h"
#include "hmem/hmem_os.h"
#include <string.h>


static HarpRuntime* harp_runtime_from_handler(HarpHandlerBase *handler) {
    return (HarpRuntime*)handler;
}
static void* harp_desc_alloc(
    HarpRuntime *runtime,
    size_t size,
    size_t alignment
) {
    if(runtime == NULL || size == 0 || alignment == 0)
        return NULL;

    // First allocation attempt
    void *ptr = hmem_arena_alloc(
        &runtime->desc_arena,
        size,
        alignment
    );

    if(ptr != NULL)
        return ptr;

    // Arena exhausted: allocate a new OS-backed page
    hmem_page_t page = {0};

    page.pool = hmem_os_alloc_pages(runtime->page_size);

    if(page.pool == NULL)
        return NULL;

    page.capacity = runtime->page_size;

    // Push page into descriptor book
    if(!hmem_book_push(
        &runtime->desc_book,
        &page
    )) {
        hmem_os_free_pages(
            page.pool,
            page.capacity
        );

        return NULL;
    }

    // Retry allocation after growth
    return hmem_arena_alloc(
        &runtime->desc_arena,
        size,
        alignment
    );
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
    HarpApiDescImpl *impl =
        harp_desc_alloc( runtime, sizeof(HarpApiDescImpl), alignof(HarpApiDescImpl) );
    if(impl == NULL)
        return HARP_RESULT_FAILED;

    memset(impl, 0, sizeof(HarpApiDescImpl));

    // Allocate API instance
    HarpApiBase *api =
        harp_desc_alloc( runtime, desc->instance_size, desc->instance_alignment );
    if(api == NULL)
        return HARP_RESULT_FAILED;

    memset(api, 0, desc->instance_size);

    // Copy descriptor metadata
    impl->_base = *desc;
    impl->p_api = api;

    // Initialize API base
    api->version = desc->version;
    api->available = 1;
    api->p_handler = handler;

    // Finalize registry entry
    entry->ptr = impl;
    entry->type = HARP_REGISTRY_ENTRY_TYPE_API;

    *out_api = api;

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

    // Allocate handler descriptor implementation
    HarpHandlerDescImpl *impl =
        harp_desc_alloc( runtime, sizeof(HarpHandlerDescImpl), alignof(HarpHandlerDescImpl) );
    if(impl == NULL)
        return HARP_RESULT_FAILED;

    memset(impl, 0, sizeof(HarpHandlerDescImpl));

    // Allocate handler instance
    HarpHandlerBase *new_handler =
        harp_desc_alloc( runtime, desc->instance_size, desc->instance_alignment );
    if(new_handler == NULL)
        return HARP_RESULT_FAILED;

    memset(new_handler, 0, desc->instance_size);

    // Copy descriptor metadata
    impl->_base = *desc;

    // Initialize handler base
    new_handler->name = desc->name;

    // Finalize registry entry
    entry->ptr = impl;
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

    // Allocate actor descriptor implementation
    HarpActorDescImpl *impl =
        harp_desc_alloc( runtime, sizeof(HarpActorDescImpl), alignof(HarpActorDescImpl) );
    if(impl == NULL)
        return HARP_RESULT_FAILED;

    memset(impl, 0, sizeof(HarpActorDescImpl));

    // Copy descriptor metadata
    impl->_base = *desc;

    // Finalize registry entry
    entry->ptr = impl;
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