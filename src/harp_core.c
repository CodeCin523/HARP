#include <harp/harp_core.h>
#include "impl/harp_core_handler.h"

#include "runtime/harp_runtime.h"
#include "runtime/harp_registry.h"

#define HMEM_IMPLEMENTATION
#include <hmem/hmem_arena.h>
#include <hmem/hmem_block.h>
#include <hmem/hmem_book.h>
#include <hmem/hmem_os.h>

#include <harp/utils/harp_api.h>
#include <harp/utils/harp_platform.h>
#include <harp/utils/harp_version.h>

#if HARP_PLATFORM_UNKNOWN
    #warning Unknown platform
#endif
#if HARP_ARCH_UNKNOWN || HARP_ARCH_X86 || HARP_ARCH_ARM32
    #warning Unknown or unsupported architecture
#endif

#include <stdlib.h>
#include <string.h>
#include <stdio.h>


static HarpResult init_core_handler(HarpCoreHandler *core_handler, HarpHandlerBase *base, HarpCreatorBase *creator) {
    (void)core_handler;
    (void)creator;

    HarpCoreHandlerImpl *impl = (HarpCoreHandlerImpl *)base;

#if HARP_PLATFORM_WINDOWS
    QueryPerformanceFrequency(&impl->frequency);
    QueryPerformanceCounter(&impl->start_time);
#elif HARP_PLATFORM_LINUX
    clock_gettime(CLOCK_MONOTONIC, &impl->start_time);
#endif

    return HARP_RESULT_OK;
}
static HarpResult term_core_handler(HarpCoreHandler *core_handler, HarpHandlerBase *base) {
    HarpCoreHandlerImpl *impl = (HarpCoreHandlerImpl *)base;
    if(impl->p_runtime->core_handler != NULL)
        return HARP_RESULT_INVALID_STATE;

    return HARP_RESULT_OK;
}


/* ================================================================================ */
/*  INITIALIZATION                                                                  */
/* ================================================================================ */

HarpVersion harp_version(void) {
    return HARP_MAKE_VERSION(1, 2, 0);
}

HarpResult harp_initialize(
    const HarpCreatorBase *creator,
    HarpRuntime **out_runtime
) {
    if(out_runtime == NULL)
        return HARP_RESULT_MISSING_OUTPUT;

    *out_runtime = NULL;

    HarpRuntimeCreator *runtime_creator = (HarpRuntimeCreator *)creator;
    HarpRuntime *runtime = malloc(sizeof(HarpRuntime));

    if(runtime == NULL)
        return HARP_RESULT_FAILED;

    if(harp_setup_runtime(runtime, runtime_creator) != HARP_RESULT_OK)
        goto fail_runtime;

    
    /* Core Handler */
    HarpHandlerDesc core_handler_desc = {
        .name = HARP_CORE_HANDLER_NAME,
        .version = HARP_CORE_HANDLER_VERSION,
        .instance_size = sizeof(HarpCoreHandlerImpl),
        .instance_alignment = alignof(HarpCoreHandlerImpl),
        .pfn_init = init_core_handler,
        .pfn_term = term_core_handler,
        .p_dependencies = NULL,
        .dependency_count = 0
    };
    HarpHandlerBase *core_handler_base = NULL;

    if(runtime_register_handler(runtime, &core_handler_desc) != HARP_RESULT_OK)
        goto fail_setup;
    if(runtime_get_handler(
        runtime,
        &(HarpDependencyDesc) {HARP_CORE_HANDLER_NAME, HARP_CORE_HANDLER_VERSION, UINT32_MAX},
        &core_handler_base
    ) != HARP_RESULT_OK)
        goto fail_setup;

    HarpCoreHandlerImpl *core_handler = (HarpCoreHandlerImpl *)core_handler_base;
    core_handler->p_runtime = runtime;
    runtime->core_handler = (HarpCoreHandler *)core_handler;

    /* registration */
    core_handler->interface.register_handler = handler_register_handler;
    core_handler->interface.register_actor = handler_register_actor;

    /* retrieval */
    core_handler->interface.get_handler = handler_get_handler;

    core_handler->interface.get_actor_count = handler_get_actor_count;
    core_handler->interface.get_actor_at = handler_get_actor_at;
    core_handler->interface.get_actors = handler_get_actors;

    core_handler->interface.get_handler_desc = handler_get_handler_desc;
    core_handler->interface.get_actor_desc = handler_get_actor_desc;

    /* lifecycle */
    core_handler->interface.handler_initialize = handler_handler_initialize;
    core_handler->interface.handler_terminate = handler_handler_terminate;
    core_handler->interface.actor_create = handler_actor_create;
    core_handler->interface.actor_destroy = handler_actor_destroy;

    /* paths */
    core_handler->interface.get_executable_directory = handler_get_executable_directory;
    core_handler->interface.get_working_directory = handler_get_working_directory;
    core_handler->interface.get_package_directory = handler_get_package_directory;

    /* status switch */
    core_handler->interface.handler_set_serving = handler_handler_set_serving;
    core_handler->interface.handler_set_failed = handler_handler_set_failed;
    core_handler->interface.actor_set_serving = handler_actor_set_serving;
    core_handler->interface.actor_set_failed = handler_actor_set_failed;

    /* time */
    core_handler->interface.get_uptime_s = handler_get_uptime_s;
    core_handler->interface.get_uptime_ms = handler_get_uptime_ms;
    core_handler->interface.get_uptime_ns = handler_get_uptime_ns;

    handler_handler_set_serving(&core_handler->interface, core_handler_base, 1);

    if(runtime_handler_initialize(
        runtime,
        HARP_CORE_HANDLER_NAME,
        &(HarpCreatorBase) {.kind = 0, .flags = HARP_CREATOR_FLAG_DEFAULT_CREATOR}
    ) != HARP_RESULT_OK)
        goto fail_setup;

    *out_runtime = runtime;

    return HARP_RESULT_OK;

fail_setup:
    harp_teardown_runtime(runtime);

fail_runtime:
    free(runtime);

    return HARP_RESULT_FAILED;
}

HarpResult harp_terminate(
    HarpRuntime *runtime
) {
    if(runtime == NULL)
        return HARP_RESULT_INVALID_ARGUMENTS;

    HarpCoreHandler *tmp = runtime->core_handler;
    runtime->core_handler = NULL;
    if(runtime_handler_terminate(runtime, HARP_CORE_HANDLER_NAME) != HARP_RESULT_OK) {
        runtime->core_handler = tmp;
        return HARP_RESULT_INVALID_STATE;
    }

    harp_teardown_runtime(runtime);

    free(runtime);

    return HARP_RESULT_OK;
}


/* ================================================================================ */
/*  RUNTIME                                                                         */
/* ================================================================================ */

HarpResult harp_runtime_get_handler(
    HarpRuntime *runtime,
    const HarpDependencyDesc *dependency,
    HarpHandlerBase **out_handler
) {
    return runtime_get_handler(
        runtime,
        dependency,
        out_handler
    );
}


/* ================================================================================ */
/*  PACKAGE LOADING                                                                 */
/* ================================================================================ */

HarpResult harp_runtime_load_packages(
    HarpRuntime *runtime
) {
    if(runtime == NULL)
        return HARP_RESULT_INVALID_ARGUMENTS;

    return harp_runtime_load_packages_from(
        runtime,
        runtime->working_directory
    );
}

HarpResult harp_runtime_load_packages_from(
    HarpRuntime *runtime,
    const char *path
) {
    if(runtime == NULL || path == NULL)
        return HARP_RESULT_INVALID_ARGUMENTS;

    return harp_package_manager_load(
        &runtime->package_manager,
        &runtime->registry,
        runtime->core_handler,
        path
    );
}

