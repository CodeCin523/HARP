#include <harp/harp_api.h>
#include "impl/harp_core.h"

#include "runtime/harp_runtime.h"

#define HMEM_IMPLEMENTATION
#include <hmem/hmem_arena.h>
#include <hmem/hmem_block.h>
#include <hmem/hmem_book.h>
#include <hmem/hmem_os.h>

#include <harp/utils/harp_helpers.h>
#include <harp/utils/harp_platform.h>
#include <harp/utils/harp_version.h>

#if HARP_PLATFORM_UNKNOWN
    #warning Unknown platform
#endif
#if HARP_ARCH_UNKNOWN || HARP_ARCH_X86 || HARP_ARCH_ARM32
    #warning Unknown or unsupported architecture
#endif

#include <stdlib.h>
#include <stdio.h>


/* ================================================================================ */
/*  INITIALIZATION                                                                  */
/* ================================================================================ */

HarpVersion harp_version(void) {
    return HARP_MAKE_VERSION(1, 3, 0);
}

HarpResult harp_initialize(
    const HarpRuntimeDesc *desc,
    HarpRuntime **out_runtime
) {
    if(out_runtime == NULL)
        return HARP_RESULT_MISSING_OUTPUT;

    *out_runtime = NULL;

    HarpRuntime *runtime = malloc(sizeof(HarpRuntime));

    if(runtime == NULL)
        return HARP_RESULT_FAILED;

    if(harp_setup_runtime(runtime, desc) != HARP_RESULT_OK)
        goto fail_runtime;


    /* Core Handler */
    HarpCoreHandlerImpl *core_handler = NULL;
    {
        static const HarpHandlerDesc CORE_HANDLER_DESC = {
            .name = HARP_CORE_HANDLER_NAME,
            .version = HARP_CORE_HANDLER_VERSION,
            .instance_size = sizeof(HarpCoreHandlerImpl),
            .instance_alignment = alignof(HarpCoreHandlerImpl),
            .pfn_init = init_core_handler,
            .pfn_term = term_core_handler,
            .p_dependencies = NULL,
            .dependency_count = 0
        };

        HarpHandlerBase *handler = NULL;
        if(harp_runtime_register_handler(runtime, &CORE_HANDLER_DESC, &handler) != HARP_RESULT_OK)
            goto fail_runtime;

        core_handler = (HarpCoreHandlerImpl *) handler;
    }
    core_handler->p_runtime = runtime;
    runtime->core_handler = (HarpCoreHandler *)core_handler;

    /* Registration */
    core_handler->interface.register_handler = core_register_handler;
    core_handler->interface.register_actor = core_register_actor;

    /* Retrieval */
    core_handler->interface.get_handler = core_get_handler;

    core_handler->interface.get_actor_count = core_get_actor_count;
    core_handler->interface.get_actor_at = core_get_actor_at;
    core_handler->interface.get_actors = core_get_actors;

    core_handler->interface.get_handler_desc = core_get_handler_desc;
    core_handler->interface.get_actor_desc = core_get_actor_desc;

    /* Handler */
    core_handler->interface.handler_initialize = core_handler_initialize;
    core_handler->interface.handler_terminate = core_handler_terminate;
    core_handler->interface.handler_set_serving = core_handler_set_serving;
    core_handler->interface.handler_set_failed = core_handler_set_failed;

    /* Actor */
    core_handler->interface.actor_create = core_actor_create;
    core_handler->interface.actor_destroy = core_actor_destroy;
    core_handler->interface.actor_set_serving = core_actor_set_serving;
    core_handler->interface.actor_set_failed = core_actor_set_failed;

    /* paths */
    core_handler->interface.get_executable_directory = core_get_executable_directory;
    core_handler->interface.get_working_directory = core_get_working_directory;
    core_handler->interface.get_package_directory = core_get_package_directory;

    /* time */
    core_handler->interface.get_uptime_s = core_get_uptime_s;
    core_handler->interface.get_uptime_ms = core_get_uptime_ms;
    core_handler->interface.get_uptime_ns = core_get_uptime_ns;

    core_handler_set_serving(&core_handler->interface, &core_handler->interface._base, 1);

    if(core_handler->interface.handler_initialize(&core_handler->interface, HARP_CORE_HANDLER_NAME, &HARP_DEFAULT_CREATOR) != HARP_RESULT_OK)
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

    HarpRuntimeHandler *rhdl = 
        harp_registry_get_runtime(&runtime->registry, HARP_CORE_HANDLER_NAME, HARP_REGISTRY_ENTRY_TYPE_HANDLER);
    HARP_CHECK_CRITICAL(rhdl != NULL);

    HarpCoreHandler *tmp = runtime->core_handler;
    runtime->core_handler = NULL;
    if(harp_rhdl_terminate(rhdl, runtime) != HARP_RESULT_OK) {
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
    return core_get_handler(
        runtime->core_handler,
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

