#include <harp/harp_core.h>
#include "impl/harp_core_api.h"

#include "runtime/harp_runtime.h"

#define HMEM_IMPLEMENTATION
#include <hmem/hmem_arena.h>
#include <hmem/hmem_block.h>
#include <hmem/hmem_book.h>
#include <hmem/hmem_os.h>

#undef HARP_UTILS_UNDEF
#include <harp/utils/harp_api.h>
#include <harp/utils/harp_version.h>

#include <stdlib.h>
#include <string.h>


HarpResult harp_initialize(
    HarpRuntime **out_runtime
) {
    if(out_runtime == NULL)
        return HARP_RESULT_MISSING_OUTPUT;

    *out_runtime = NULL;

    HarpRuntime *runtime = malloc(sizeof(HarpRuntime));

    if(runtime == NULL)
        return HARP_RESULT_FAILED;

    if(harp_setup_runtime(runtime) != HARP_RESULT_OK)
        goto fail_runtime;

    // Register core API
    HarpApiDesc core_api_desc = {
        .name = HARP_CORE_API_NAME,
        .version = HARP_CORE_API_VERSION,
        .instance_size = sizeof(HarpCoreApi),
        .instance_alignment = alignof(HarpCoreApi)
    };

    HarpApiBase *core_api_base = NULL;

    if(register_api((HarpHandlerBase *)runtime, &core_api_desc, &core_api_base) != HARP_RESULT_OK)
        goto fail_setup;

    core_api_base->p_handler = (HarpHandlerBase *)runtime;

    HarpCoreApi *core_api = (HarpCoreApi *)core_api_base;

    core_api->register_api = register_api;
    core_api->register_handler = register_handler;
    core_api->register_actor = register_actor;

    core_api->get_api = get_api;
    core_api->get_handler = get_handler;
    core_api->get_api_desc = get_api_desc;
    core_api->get_handler_desc = get_handler_desc;
    core_api->get_actor_desc = get_actor_desc;

    core_api->handler_initialize = handler_initialize;
    core_api->handler_terminate = handler_terminate;

    runtime->core_api = core_api;
    core_api_base->available = 1;

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
    if(runtime == NULL) return HARP_RESULT_INVALID_ARGUMENTS;

    harp_teardown_runtime(runtime);

    free(runtime);
    return HARP_RESULT_OK;
}

HarpResult harp_runtime_load_directory(
    HarpRuntime *runtime,
    const char *path
) {
    
}

HarpResult harp_runtime_get_api(
    HarpRuntime *runtime,
    const HarpName name,
    HarpApiBase **out_api
) {
    
}
HarpResult harp_runtime_get_handler(
    HarpRuntime *runtime,
    const HarpName name,
    HarpHandlerBase **out_handler
) {

}

/* HarpResult harp_runtime_get_package_count(
    HarpRuntime *runtime,
    uint64_t *out_count
) {
    
}
HarpResult harp_runtime_get_package_name(
    HarpRuntime *runtime,
    uint64_t index,
    HarpName *out_name
) {
    
} */


/* wouldn't work, since the api is first element of runtime, we need manual allocation or at least overwrite
HarpResult harp_package_register(HarpApiBase *base) {
    static const HarpApiDesc core_api_desc = {
        HARP_CORE_API_VERSION,
        sizeof()
    }

    HarpCoreApi *core_api = HARP_API_AS(HarpCoreApi, base);

    core_api->register_api()
}

HarpResult harp_package_query(HarpPackageDesc** out_desc) {
    static HarpPackageDesc package_desc = {
        "Harp",
        HARP_MAKE_VERSION(1, 0, 0),
        harp_package_register,
        NULL,
        NULL,
        NULL,
        0
    };
    *out_desc = &package_desc;
    return HARP_RESULT_OK;
}*/