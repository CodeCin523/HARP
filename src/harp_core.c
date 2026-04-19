#include <harp/harp_core.h>
#include "impl/harp_api.h"

#undef HARP_UTILS_UNDEF
#include <harp/utils/harp_api.h>
#include <harp/utils/harp_version.h>

#include <stdlib.h>

HarpResult harp_init(
    HarpRuntime **out_runtime
) {
    HarpRuntime *runtime = malloc(sizeof(HarpRuntime));
    if(!runtime)
        return HARP_RESULT_FAILED;

    HarpRegistryEntry *entry_pool = malloc(sizeof(HarpRegistryEntry) * 128 * HARP_REGISTRY_BUCKET_COUNT);
    if(!entry_pool)
        return HARP_RESULT_FAILED;

    for(int i = 0; i < HARP_REGISTRY_BUCKET_COUNT; ++i) {
        runtime->registry.buckets[i].entries = entry_pool;
        runtime->registry.buckets[i].capacity = 128;
        runtime->registry.buckets[i].count = 0;

        entry_pool += 128;
    }

    runtime->api_core.base.version = HARP_CORE_API_VERSION;

    runtime->api_core.register_api = register_api;
    runtime->api_core.register_handler = register_handler;
    runtime->api_core.register_actor = register_actor;

    runtime->api_core.get_api = get_api;
    runtime->api_core.get_handler = get_handler;
    runtime->api_core.get_api_desc = get_api_desc;
    runtime->api_core.get_handler_desc = get_handler_desc;
    runtime->api_core.get_actor_desc = get_actor_desc;

    runtime->api_core.handler_initialize = handler_initialize;
    runtime->api_core.handler_terminate = handler_terminate;

    runtime->api_core.actor_create = actor_create;
    runtime->api_core.actor_destroy = actor_destroy;

    HarpRegistryEntry *core_api_entry = harp_registry_insert(runtime, &runtime->registry, HARP_CORE_API_NAME);
    if(!core_api_entry)
        return HARP_RESULT_FAILED;
    core_api_entry->ptr = &runtime->api_core;
    core_api_entry->type = HARP_REGISTRY_ENTRY_TYPE_API;

    HarpRegistryEntry *extended_api_entry = harp_registry_insert(runtime, &runtime->registry, HARP_EXTENDED_API_NAME);
    if(!core_api_entry)
        return HARP_RESULT_FAILED;
    core_api_entry->ptr = &runtime->api_extended;
    core_api_entry->type = HARP_REGISTRY_ENTRY_TYPE_API;

    return HARP_RESULT_OK;
}
HarpResult harp_exit(
    HarpRuntime *runtime
) {

}

HarpResult harp_runtime_load_packages(
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

HarpResult harp_runtime_get_package_count(
    HarpRuntime *runtime,
    uint64_t *out_count
) {
    
}
HarpResult harp_runtime_get_package_name(
    HarpRuntime *runtime,
    uint64_t index,
    HarpName *out_name
) {
    
}


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