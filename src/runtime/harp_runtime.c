#include "harp_runtime.h"
#include "utils.h"

#include <harp/utils/harp_helpers.h>

#include <hmem/hmem_os.h>
#include <hmem/utils/hmem_align.h>

#include <unistd.h>
#include <limits.h>
#include <string.h>
#include <stdlib.h>
#include <stdalign.h>


/* ================================================================================ */
/*  RUNTIME                                                                         */
/* ================================================================================ */

static char *runtime_strdup(HarpRuntime *runtime, const char *str) {
    HARP_CHECK_CRITICAL(runtime != NULL && str != NULL);

    size_t len = strlen(str) + 1;

    char *dst = harp_runtime_alloc_global(runtime, len, alignof(char));
    if(dst == NULL)
        return NULL;

    memcpy(dst, str, len);
    return dst;
}
static HarpResult runtime_setup_paths(HarpRuntime *runtime, const HarpRuntimeDesc *desc) {
    HARP_CHECK_CRITICAL(runtime != NULL && desc != NULL);

    runtime->executable_directory = NULL;
    runtime->working_directory = NULL;

    { // working directory
        char cwd[PATH_MAX];

        if(getcwd(cwd, sizeof(cwd)) == NULL)
            return HARP_RESULT_FAILED;

        runtime->working_directory = runtime_strdup(runtime, cwd);
        if(runtime->working_directory == NULL)
            return HARP_RESULT_OUT_OF_MEMORY;
    }

    { // executable directory
        if(desc->executable_path == NULL)
            return HARP_RESULT_INVALID_ARGUMENTS;

        char resolved[PATH_MAX];

        if(realpath(desc->executable_path, resolved) == NULL)
            return HARP_RESULT_FAILED;

        char *last_sep = strrchr(resolved, '/');

#ifdef _WIN32
        char *last_backslash = strrchr(resolved, '\\');

        if(last_backslash != NULL &&
           (last_sep == NULL || last_backslash > last_sep))
            last_sep = last_backslash;
#endif

        if(last_sep == NULL)
            return HARP_RESULT_FAILED;

        *last_sep = '\0';

        runtime->executable_directory = runtime_strdup(runtime, resolved);
        if(runtime->executable_directory == NULL)
            return HARP_RESULT_OUT_OF_MEMORY;
    }

    return HARP_RESULT_OK;
}

HarpResult harp_setup_runtime(HarpRuntime *runtime, const HarpRuntimeDesc *desc) {
    HARP_CHECK_CRITICAL(runtime != NULL && desc != NULL);

    harp_setup_registry(&runtime->registry);
    harp_setup_package_manager(&runtime->package_manager);

    runtime->page_size = hmem_clamp(hmem_os_page_size(), 16384, SIZE_MAX);

    if(!hmem_setup_book(&runtime->global_book, 16))
        goto fail_registry;

    hmem_page_t page = {0};
    page.pool = hmem_os_alloc_pages(runtime->page_size);
    if(!page.pool)
        goto fail_book;

    page.capacity = runtime->page_size;

    if(!hmem_book_push(&runtime->global_book, &page)) {
        hmem_os_free_pages(page.pool, page.capacity);
        goto fail_book;
    }

    if(!hmem_setup_arena(&runtime->global_arena, &runtime->global_book))
        goto fail_pages;

    if(runtime_setup_paths(runtime, desc) != HARP_RESULT_OK)
        goto fail_pages;

    runtime->core_handler = NULL;

    return HARP_RESULT_OK;

fail_pages:
    while(hmem_book_pop(&runtime->global_book, &page)) {
        hmem_os_free_pages(page.pool, page.capacity);
        hmem_teardown_page(&page);
    }

fail_book:
    hmem_teardown_book(&runtime->global_book);

fail_registry:
    return HARP_RESULT_FAILED;
}
void harp_teardown_runtime(HarpRuntime *runtime) {
    HARP_CHECK_CRITICAL(runtime != NULL);

    harp_teardown_package_manager(&runtime->package_manager);

    hmem_teardown_arena(&runtime->global_arena);

    hmem_page_t page;
    while(hmem_book_pop(&runtime->global_book, &page)) {
        hmem_os_free_pages(page.pool, page.capacity);
        hmem_teardown_page(&page);
    }

    hmem_teardown_book(&runtime->global_book);

    harp_teardown_registry(&runtime->registry);
}

void *harp_runtime_alloc_global(HarpRuntime *runtime, size_t size, size_t alignment) {
    HARP_CHECK_CRITICAL(runtime != NULL && size != 0 && alignment != 0);

    void *ptr = hmem_arena_alloc(&runtime->global_arena, size, alignment);
    if(ptr != NULL)
        return ptr;

    hmem_page_t page = {0};
    page.pool = hmem_os_alloc_pages(runtime->page_size);
    if(page.pool == NULL)
        return NULL;

    page.capacity = runtime->page_size;

    if(!hmem_book_push(&runtime->global_book, &page)) {
        hmem_os_free_pages(page.pool, page.capacity);
        return NULL;
    }

    return hmem_arena_alloc(&runtime->global_arena, size, alignment);
}
HarpResult harp_runtime_alloc_rhdl(HarpRuntime *runtime, HarpHandlerDesc *desc, HarpRuntimeHandler **out_rhdl, HarpHandlerBase **out_handler) {
    HARP_CHECK_CRITICAL(runtime != NULL && desc != NULL);
    HARP_CHECK_CRITICAL(out_rhdl != NULL && out_handler != NULL);
    
    HarpRuntimeHandler *tmp_rhdl = harp_runtime_alloc_global(
        runtime,
        sizeof(HarpRuntimeHandler),
        alignof(HarpRuntimeHandler)
    );
    HarpHandlerBase *tmp_handler = harp_runtime_alloc_global(
        runtime,
        desc->instance_size,
        desc->instance_alignment
    );

    if(tmp_rhdl == NULL || tmp_handler == NULL)
        return HARP_RESULT_OUT_OF_MEMORY;

    memset(tmp_rhdl, 0, sizeof(HarpRuntimeHandler));
    memset(tmp_handler, 0, desc->instance_size);

    *out_rhdl = tmp_rhdl;
    *out_handler = tmp_handler;

    return HARP_RESULT_OK;
}
HarpResult harp_runtime_alloc_ract(HarpRuntime *runtime, HarpActorDesc *desc, HarpRuntimeActor **out_ract) {
    HARP_CHECK_CRITICAL(runtime != NULL && desc != NULL);
    HARP_CHECK_CRITICAL(out_ract != NULL);

    HarpRuntimeActor *tmp_ract = harp_runtime_alloc_global(
        runtime, 
        sizeof(HarpRuntimeActor), 
        alignof(HarpRuntimeActor)
    );

    if(tmp_ract == NULL)
        return HARP_RESULT_OUT_OF_MEMORY;

    memset(tmp_ract, 0, sizeof(HarpRuntimeActor));

    *out_ract = tmp_ract;

    return HARP_RESULT_OK;
}

HarpResult harp_runtime_intern_hdl_desc(HarpRuntime *runtime, const HarpHandlerDesc *desc, HarpHandlerDesc *out_desc) {
    HARP_CHECK_CRITICAL(runtime != NULL && desc != NULL && out_desc != NULL);

    *out_desc = *desc;

    HarpName rname = harp_registry_name(&runtime->registry, desc->name);
    if(rname == NULL)
        return HARP_RESULT_OUT_OF_MEMORY;

    out_desc->name = rname;

    if(desc->dependency_count > 0 && desc->p_dependencies != NULL) {
        size_t deps_size = sizeof(HarpDependencyDesc) * desc->dependency_count;

        HarpDependencyDesc *rdeps = harp_runtime_alloc_global(runtime, deps_size, alignof(HarpDependencyDesc));
        if(rdeps == NULL)
            return HARP_RESULT_OUT_OF_MEMORY;

        memcpy(rdeps, desc->p_dependencies, deps_size);

        for(uint64_t i = 0; i < desc->dependency_count; ++i) {
            HarpName dep_rname = harp_registry_name(&runtime->registry, rdeps[i].name);
            if(dep_rname == NULL)
                return HARP_RESULT_OUT_OF_MEMORY;

            rdeps[i].name = dep_rname;
        }

        out_desc->p_dependencies = rdeps;
    }

    return HARP_RESULT_OK;
}
HarpResult harp_runtime_intern_act_desc(HarpRuntime *runtime, const HarpActorDesc *desc, HarpActorDesc *out_desc) {
    HARP_CHECK_CRITICAL(runtime != NULL && desc != NULL && out_desc != NULL);

    *out_desc = *desc;

    HarpName rname = harp_registry_name(&runtime->registry, desc->name);
    if(rname == NULL)
        return HARP_RESULT_OUT_OF_MEMORY;

    out_desc->name = rname;

    HarpName rparent = harp_registry_name(&runtime->registry, desc->parent_handler.name);
    if(rparent == NULL)
        return HARP_RESULT_OUT_OF_MEMORY;

    out_desc->parent_handler.name = rparent;

    return HARP_RESULT_OK;
}


/* ================================================================================ */
/*  HarpRuntimeHandler                                                              */
/* ================================================================================ */

HarpResult harp_setup_rhdl(HarpRuntimeHandler *rhdl, HarpHandlerDesc *desc, HarpHandlerBase *handler) {
    HARP_CHECK_CRITICAL(rhdl != NULL && desc != NULL && handler != NULL);

    HarpName *name_ptr = (HarpName *) &handler->name;
    *name_ptr = desc->name;
    handler->status = 0;

    rhdl->descriptor      = *desc;
    rhdl->instance        = handler;
    rhdl->dependent_count = 0;
    rhdl->actor_count     = 0;

    return HARP_RESULT_OK;
}
void harp_teardown_rhdl(HarpRuntimeHandler *rhdl) {
    HARP_CHECK_CRITICAL(rhdl != NULL);

    rhdl->instance        = NULL;
    rhdl->dependent_count = 0;
    rhdl->actor_count     = 0;
}

HarpResult harp_rhdl_check_dependencies(HarpRuntimeHandler *rhdl, HarpRuntime *runtime) {
    HARP_CHECK_CRITICAL(rhdl != NULL && runtime != NULL);

    HarpHandlerDesc *rdesc = &rhdl->descriptor;
    HARP_CHECK_CRITICAL(rdesc != NULL);

    for(uint64_t i = 0; i < rdesc->dependency_count; ++i) {
        HarpDependencyDesc *dep = &rdesc->p_dependencies[i];

        HarpRuntimeHandler *dep_rdesc = 
            harp_registry_get_runtime(&runtime->registry, dep->name, HARP_REGISTRY_ENTRY_TYPE_HANDLER);
    
        if(dep_rdesc == NULL)
            return HARP_RESULT_NAME_NOT_FOUND;
        if(utils_dependency_matches(dep, dep_rdesc->descriptor.version) != HARP_RESULT_OK)
            return HARP_RESULT_DEPENDENCY_VERSION_MISMATCH;

        HarpHandlerBase *dep_handler = dep_rdesc->instance;
        HARP_CHECK_CRITICAL(dep_handler != NULL);

        if(!(dep_handler->status & HARP_STATUS_FLAG_VALID))
            return HARP_RESULT_DEPENDENCY_UNINITIALIZED;
        if(dep_handler->status & (HARP_STATUS_FLAG_INITIALIZING | HARP_STATUS_FLAG_TERMINATING))
            return HARP_RESULT_DEPENDENCY_CYCLE;
    }

    return HARP_RESULT_OK;
}
HarpResult harp_rhdl_check_dependents(HarpRuntimeHandler *rhdl, HarpRuntime *runtime) {
    HARP_CHECK_CRITICAL(rhdl != NULL && runtime != NULL);

    if(rhdl->dependent_count > 0)
        return HARP_RESULT_EXISTENT_DEPENDENTS;
    if(rhdl->actor_count > 0)
        return HARP_RESULT_EXISTENT_ACTORS;

    return HARP_RESULT_OK;
}

HarpResult harp_rhdl_initialize(HarpRuntimeHandler *rhdl, HarpRuntime *runtime, HarpCreatorBase *creator) {
    HARP_CHECK_CRITICAL(rhdl != NULL && runtime != NULL && creator != NULL);

    HarpHandlerDesc *rdesc = &rhdl->descriptor;
    HarpHandlerBase *handler = rhdl->instance;

    HARP_CHECK_CRITICAL(rdesc != NULL);
    HARP_CHECK_CRITICAL(handler != NULL);

    // check state
    if(handler->status & HARP_STATUS_FLAG_VALID)
        return HARP_RESULT_OK;
    if(handler->status & (HARP_STATUS_FLAG_INITIALIZING | HARP_STATUS_FLAG_TERMINATING))
        return HARP_RESULT_INVALID_STATE;

    // check dependencies
    HarpResult res = harp_rhdl_check_dependencies(rhdl, runtime);
    if(res != HARP_RESULT_OK)
        return res;

    // start initialization
    handler->status |= HARP_STATUS_FLAG_INITIALIZING;
    res = rdesc->pfn_init(runtime->core_handler, handler, creator);
    handler->status &= ~HARP_STATUS_FLAG_INITIALIZING;

    if(res == HARP_RESULT_OK) {
        handler->status |= HARP_STATUS_FLAG_VALID;

        for(uint64_t i = 0; i < rdesc->dependency_count; ++i) {
            HarpDependencyDesc *dep = &rdesc->p_dependencies[i];

            HarpRuntimeHandler *dep_rdesc =
                harp_registry_get_runtime(&runtime->registry, dep->name, HARP_REGISTRY_ENTRY_TYPE_HANDLER);

            if(dep_rdesc != NULL)
                ++dep_rdesc->dependent_count;
        }
    }

    return res;
}
HarpResult harp_rhdl_terminate(HarpRuntimeHandler *rhdl, HarpRuntime *runtime) {
    HARP_CHECK_CRITICAL(rhdl != NULL && runtime != NULL);

    HarpHandlerDesc *rdesc = &rhdl->descriptor;
    HarpHandlerBase *handler = rhdl->instance;

    HARP_CHECK_CRITICAL(rdesc != NULL);
    HARP_CHECK_CRITICAL(handler != NULL);

    // check state
    if(!(handler->status & HARP_STATUS_FLAG_VALID))
        return HARP_RESULT_OK;
    if(handler->status & (HARP_STATUS_FLAG_INITIALIZING | HARP_STATUS_FLAG_TERMINATING))
        return HARP_RESULT_INVALID_STATE;

    // check dependents
    HarpResult res = harp_rhdl_check_dependents(rhdl, runtime);
    if(res != HARP_RESULT_OK)
        return res;

    // start termination
    handler->status |= HARP_STATUS_FLAG_TERMINATING;
    res = rdesc->pfn_term(runtime->core_handler, handler);
    handler->status &= ~HARP_STATUS_FLAG_TERMINATING;

    if(res == HARP_RESULT_OK) {
        handler->status &= ~HARP_STATUS_FLAG_VALID;

        for(uint64_t i = 0; i < rdesc->dependency_count; ++i) {
            HarpDependencyDesc *dep = &rdesc->p_dependencies[i];

            HarpRuntimeHandler *dep_rdesc =
                harp_registry_get_runtime(&runtime->registry, dep->name, HARP_REGISTRY_ENTRY_TYPE_HANDLER);

            if(dep_rdesc != NULL && dep_rdesc->dependent_count > 0)
                --dep_rdesc->dependent_count;
        }
    }

    return res;
}


/* ================================================================================ */
/*  HarpRuntimeActor                                                                */
/* ================================================================================ */

HarpResult harp_setup_ract(HarpRuntimeActor *ract, HarpActorDesc *desc) {
    HARP_CHECK_CRITICAL(ract != NULL && desc != NULL);

    ract->descriptor        = *desc;
    ract->page_growth_index = 0;
    ract->actor_count       = 0;
    ract->actor_capacity    = 0;
    ract->actors            = NULL;

    if(!hmem_setup_book(&ract->inst_book, 16))
        return HARP_RESULT_FAILED;

    if(!hmem_setup_block(
        &ract->inst_block,
        &ract->inst_book,
        desc->instance_size,
        desc->instance_alignment
    )) {
        hmem_teardown_book(&ract->inst_book);
        return HARP_RESULT_FAILED;
    }

    return HARP_RESULT_OK;
}
void harp_teardown_ract(HarpRuntimeActor *ract) {
    HARP_CHECK_CRITICAL(ract != NULL);

    hmem_teardown_block(&ract->inst_block);

    hmem_page_t page;
    while(hmem_book_pop(&ract->inst_book, &page)) {
        free(page.pool);
        hmem_teardown_page(&page);
    }

    hmem_teardown_book(&ract->inst_book);

    free(ract->actors);
    ract->actors         = NULL;
    ract->actor_count    = 0;
    ract->actor_capacity = 0;
}

HarpResult harp_ract_check_dependencies(HarpRuntimeActor *ract, HarpRuntime *runtime) {
    HARP_CHECK_CRITICAL(ract != NULL && runtime != NULL);

    HarpActorDesc *rdesc = &ract->descriptor;
    HARP_CHECK_CRITICAL(rdesc != NULL);

    HarpRuntimeHandler *rhdl =
        harp_registry_get_runtime(&runtime->registry, rdesc->parent_handler.name, HARP_REGISTRY_ENTRY_TYPE_HANDLER);
    
    if(rhdl == NULL)
        return HARP_RESULT_NAME_NOT_FOUND;
    if(utils_dependency_matches(&rdesc->parent_handler, rhdl->descriptor.version) != HARP_RESULT_OK)
            return HARP_RESULT_DEPENDENCY_VERSION_MISMATCH;

    HarpHandlerBase *handler = rhdl->instance;
    HARP_CHECK_CRITICAL(handler != NULL);

    if(!(handler->status & HARP_STATUS_FLAG_VALID))
        return HARP_RESULT_DEPENDENCY_UNINITIALIZED;
    
    // return harp_rhdl_check_dependencies(rhdl, runtime);
    return HARP_RESULT_OK;
}
HarpResult harp_ract_check_dependents(HarpRuntimeActor *ract, HarpRuntime *runtime) {
    (void)ract;
    (void)runtime;

    // a handler cannot close without all of it's actor destroied
    // and a actor cannot be created without it's handler initialized
    // so the state this code check is impossible.
    // HarpRuntimeHandler *handler_rdesc =
    //     harp_registry_get(
    //         &runtime->registry,
    //         actor_desc->parent_handler,
    //         HARP_REGISTRY_ENTRY_TYPE_HANDLER
    //     );
    // 
    // if(handler_rdesc == NULL)
    //     return HARP_RESULT_NAME_NOT_FOUND;
    // 
    // // check state
    // HarpHandlerBase *handler_base = handler_rdesc->instance;
    // if(!(handler_base->status & HARP_STATUS_FLAG_VALID))
    //     return HARP_RESULT_INVALID_STATE;

    return HARP_RESULT_OK;
}

HarpResult harp_ract_alloc_actor(HarpRuntimeActor *ract, HarpRuntime *runtime, HarpActorBase **out_actor) {
    HARP_CHECK_CRITICAL(ract != NULL && out_actor != NULL);
    (void)runtime;

    *out_actor = NULL;

    void *ptr = hmem_block_alloc_single(&ract->inst_block);
    if(ptr != NULL) {
        *out_actor = ptr;
        return HARP_RESULT_OK;
    }

    size_t count = 4 << ract->page_growth_index;
    size_t size  = count * ract->descriptor.instance_size;

    void *pool = malloc(size);
    if(pool == NULL)
        return HARP_RESULT_OUT_OF_MEMORY;

    hmem_page_t page = { .pool = pool, .capacity = size };

    if(!hmem_book_push(&ract->inst_book, &page)) {
        free(pool);
        return HARP_RESULT_OUT_OF_MEMORY;
    }

    hmem_block_update(&ract->inst_block);
    ++ract->page_growth_index;

    ptr = hmem_block_alloc_single(&ract->inst_block);
    if(ptr == NULL)
        return HARP_RESULT_OUT_OF_MEMORY;

    *out_actor = ptr;
    return HARP_RESULT_OK;
}
HarpResult harp_ract_free_actor(HarpRuntimeActor *ract, HarpRuntime *runtime, HarpActorBase *actor) {
    HARP_CHECK_CRITICAL(ract != NULL && actor != NULL);
    (void)runtime;

    hmem_block_free_single(
        &ract->inst_block,
        actor
    );

    return HARP_RESULT_OK;
}

HarpResult harp_ract_create(HarpRuntimeActor *ract, HarpRuntime *runtime, HarpActorBase *actor, HarpCreatorBase *creator) {
    HARP_CHECK_CRITICAL(ract != NULL && runtime != NULL && creator != NULL && actor != NULL);

    HarpActorDesc *rdesc = &ract->descriptor;
    
    HARP_CHECK_CRITICAL(rdesc != NULL);

    // check state (technically useless since we know it as just been created, but this function doesn't know that)
    if(actor->status & HARP_STATUS_FLAG_VALID)
        return HARP_RESULT_OK;
    if(actor->status & (HARP_STATUS_FLAG_INITIALIZING | HARP_STATUS_FLAG_TERMINATING))
        return HARP_RESULT_INVALID_STATE;

    // check dependencies
    HarpResult res = harp_ract_check_dependencies(ract, runtime);
    if(res != HARP_RESULT_OK)
        return res;

    // grow actor pointer array
    if(ract->actor_count >= ract->actor_capacity) {
        uint64_t new_capacity =
            ract->actor_capacity == 0
            ? 16
            : ract->actor_capacity * 2;

        void *new_ptr = realloc(
            ract->actors,
            sizeof(HarpActorBase*) * new_capacity
        );

        if(new_ptr == NULL)
            return HARP_RESULT_OUT_OF_MEMORY;

        ract->actors = new_ptr;
        ract->actor_capacity = new_capacity;
    }

    // start creation
    HarpName *name_ptr = (HarpName *) &actor->name;
    *name_ptr = rdesc->name;

    actor->status |= HARP_STATUS_FLAG_INITIALIZING;
    res = rdesc->pfn_create(runtime->core_handler, actor, creator);
    actor->status &= ~HARP_STATUS_FLAG_INITIALIZING;

    if(res == HARP_RESULT_OK) {
        actor->status |= HARP_STATUS_FLAG_VALID;
        ract->actors[ract->actor_count++] = actor;

        HarpRuntimeHandler *dep_rdesc =
            harp_registry_get_runtime(&runtime->registry, rdesc->parent_handler.name, HARP_REGISTRY_ENTRY_TYPE_HANDLER);
        if(dep_rdesc != NULL)
            ++dep_rdesc->actor_count;
    }

    return res;
}
HarpResult harp_ract_destroy(HarpRuntimeActor *ract, HarpRuntime *runtime, HarpActorBase *actor) {
    HARP_CHECK_CRITICAL(ract != NULL && runtime != NULL && actor != NULL);

    HarpActorDesc *rdesc = &ract->descriptor;
    
    HARP_CHECK_CRITICAL(rdesc != NULL);

    // check state
    if(!(actor->status & HARP_STATUS_FLAG_VALID))
        return HARP_RESULT_OK;
    if(actor->status & (HARP_STATUS_FLAG_INITIALIZING | HARP_STATUS_FLAG_TERMINATING))
        return HARP_RESULT_INVALID_STATE;

    // check dependents
    HarpResult res = harp_ract_check_dependents(ract, runtime);
    if(res != HARP_RESULT_OK)
        return res;

    // validate actor ownership
    if(!hmem_book_contains(&ract->inst_book, actor))
        return HARP_RESULT_INVALID_ARGUMENTS;

    // start destruction
    actor->status |= HARP_STATUS_FLAG_TERMINATING;
    res = rdesc->pfn_destroy(runtime->core_handler, actor);
    actor->status &= ~HARP_STATUS_FLAG_TERMINATING;

    if(res == HARP_RESULT_OK) {
        actor->status &= ~HARP_STATUS_FLAG_VALID;

        /* remove actor from runtime list */
        for(uint64_t i = 0; i < ract->actor_count; ++i) {
            if(ract->actors[i] == actor) {

                ract->actors[i] =
                    ract->actors[ract->actor_count-1];

                --ract->actor_count;
                break;
            }
        }

        HarpRuntimeHandler *dep_rdesc =
            harp_registry_get_runtime(&runtime->registry, rdesc->parent_handler.name, HARP_REGISTRY_ENTRY_TYPE_HANDLER);
        if(dep_rdesc != NULL && dep_rdesc->actor_count > 0)
            --dep_rdesc->actor_count;
    }

    return res;
}


/* ================================================================================ */
/*  REGISTRATION                                                                    */
/* ================================================================================ */

HarpResult harp_runtime_register_handler(HarpRuntime *runtime, const HarpHandlerDesc *desc, HarpHandlerBase **out_handler) {
    HARP_CHECK_CRITICAL(runtime != NULL && desc != NULL);

    HarpHandlerDesc idesc;
    if(harp_runtime_intern_hdl_desc(runtime, desc, &idesc) != HARP_RESULT_OK)
        return HARP_RESULT_OUT_OF_MEMORY;

    HarpRuntimeHandler *rhdl    = NULL;
    HarpHandlerBase    *handler = NULL;
    if(harp_runtime_alloc_rhdl(runtime, &idesc, &rhdl, &handler) != HARP_RESULT_OK)
        return HARP_RESULT_OUT_OF_MEMORY;

    HarpResult res = harp_setup_rhdl(rhdl, &idesc, handler);
    if(res != HARP_RESULT_OK)
        return res;

    res = harp_registry_bind(&runtime->registry, idesc.name, HARP_REGISTRY_ENTRY_TYPE_HANDLER, rhdl);
    if(res != HARP_RESULT_OK)
        return res;

    if(out_handler != NULL)
        *out_handler = handler;

    return HARP_RESULT_OK;
}

HarpResult harp_runtime_register_actor(HarpRuntime *runtime, const HarpActorDesc *desc) {
    HARP_CHECK_CRITICAL(runtime != NULL && desc != NULL);

    HarpActorDesc idesc;
    if(harp_runtime_intern_act_desc(runtime, desc, &idesc) != HARP_RESULT_OK)
        return HARP_RESULT_OUT_OF_MEMORY;

    HarpRuntimeActor *ract = NULL;
    if(harp_runtime_alloc_ract(runtime, &idesc, &ract) != HARP_RESULT_OK)
        return HARP_RESULT_OUT_OF_MEMORY;

    HarpResult res = harp_setup_ract(ract, &idesc);
    if(res != HARP_RESULT_OK)
        return res;

    res = harp_registry_bind(&runtime->registry, idesc.name, HARP_REGISTRY_ENTRY_TYPE_ACTOR, ract);
    if(res != HARP_RESULT_OK) {
        harp_teardown_ract(ract);
        return res;
    }

    return HARP_RESULT_OK;
}