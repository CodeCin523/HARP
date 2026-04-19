#include "harp_api.h"

HarpResult register_api(HarpCoreApi api, const HarpApiDesc* desc, HarpApiBase** out_api) {
    HarpRuntime *runtime = api.p_runtime;

    void *ptr = memcc_dfstack_push(&runtime->dfstack_instance_data, desc->instance_size);
    if(ptr == NULL)
        return HARP_RESULT_FAILED;
    HarpRegistryEntry *entry = harp_registry_insert(runtime, &runtime->registry, desc->name);
    if(entry == NULL)
        return HARP_RESULT_FAILED;

    entry->ptr = ptr;
    entry->type = HARP_REGISTRY_ENTRY_TYPE_API;

}
HarpResult register_handler(HarpCoreApi api, const HarpHandlerDesc* desc) {

}
HarpResult register_actor(HarpCoreApi api, const HarpActorDesc* desc) {

}

HarpResult get_api(HarpCoreApi api, const HarpName name, HarpApiBase **out_api) {

}
HarpResult get_handler(HarpCoreApi api, const HarpName name, HarpHandlerBase** out_handler) {

}
HarpResult get_api_desc(HarpCoreApi api, const HarpName name, HarpApiDesc **out_desc) {

}
HarpResult get_handler_desc(HarpCoreApi api, const HarpName name, HarpHandlerDesc** out_desc) {

}
HarpResult get_actor_desc(HarpCoreApi api, const HarpName name, HarpActorDesc** out_desc) {

}

HarpResult handler_initialize(HarpCoreApi api, const HarpName name, const HarpCreatorBase* creator) {

}
HarpResult handler_terminate(HarpCoreApi api, const HarpName name) {

}

HarpResult actor_create(HarpCoreApi api, const HarpName name, const HarpCreatorBase* creator, HarpActorBase** out_actor) {

}
HarpResult actor_destroy(HarpCoreApi api, const HarpName name, HarpActorBase* actor) {

}