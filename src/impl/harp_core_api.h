#ifndef IMPL_HARP_API_H
#define IMPL_HARP_API_H

#include <harp/harp_core.h>


typedef struct HarpCoreApiImpl {
    HarpCoreApi core_api;
    HarpRuntime *p_runtime;
} HarpCoreApiImpl;



// HarpCoreApi's impl
HarpResult core_register_api(HarpCoreApi *api, const HarpApiDesc* desc, HarpApiBase** out_api);
HarpResult core_register_handler(HarpCoreApi *api, const HarpHandlerDesc* desc);
HarpResult core_register_actor(HarpCoreApi *api, const HarpActorDesc* desc);

HarpResult core_get_api(HarpCoreApi *api, const HarpName name, HarpApiBase **out_api);
HarpResult core_get_handler(HarpCoreApi *api, const HarpName name, HarpHandlerBase** out_handler);
HarpResult core_get_api_desc(HarpCoreApi *api, const HarpName name, HarpApiDesc **out_desc);
HarpResult core_get_handler_desc(HarpCoreApi *api, const HarpName name, HarpHandlerDesc** out_desc);
HarpResult core_get_actor_desc(HarpCoreApi *api, const HarpName name, HarpActorDesc** out_desc);

HarpResult core_handler_initialize(HarpCoreApi *api, const HarpName name, const HarpCreatorBase* creator);
HarpResult core_handler_terminate(HarpCoreApi *api, const HarpName name);

HarpResult core_actor_create(HarpCoreApi *api, const HarpName name, const HarpCreatorBase* creator, HarpActorBase** out_actor);
HarpResult core_actor_destroy(HarpCoreApi *api, const HarpName name, HarpActorBase* actor);

HarpResult core_get_executable_directory(HarpCoreApi *api, const char **out_path);
HarpResult core_get_working_directory(HarpCoreApi *api, const char **out_path);
HarpResult core_get_package_directory(HarpCoreApi *api, const HarpName name, const char **out_path);


// HarpRuntime's impl
HarpResult runtime_register_api(HarpRuntime *runtime, const HarpApiDesc* desc, HarpApiBase** out_api);
HarpResult runtime_register_handler(HarpRuntime *runtime, const HarpHandlerDesc* desc);
HarpResult runtime_register_actor(HarpRuntime *runtime, const HarpActorDesc* desc);

HarpResult runtime_get_api(HarpRuntime *runtime, const HarpName name, HarpApiBase **out_api);
HarpResult runtime_get_handler(HarpRuntime *runtime, const HarpName name, HarpHandlerBase** out_handler);
HarpResult runtime_get_api_desc(HarpRuntime *runtime, const HarpName name, HarpApiDesc **out_desc);
HarpResult runtime_get_handler_desc(HarpRuntime *runtime, const HarpName name, HarpHandlerDesc** out_desc);
HarpResult runtime_get_actor_desc(HarpRuntime *runtime, const HarpName name, HarpActorDesc** out_desc);

HarpResult runtime_handler_initialize(HarpRuntime *runtime, const HarpName name, const HarpCreatorBase* creator);
HarpResult runtime_handler_terminate(HarpRuntime *runtime, const HarpName name);

HarpResult runtime_actor_create(HarpRuntime *runtime, const HarpName name, const HarpCreatorBase* creator, HarpActorBase** out_actor);
HarpResult runtime_actor_destroy(HarpRuntime *runtime, const HarpName name, HarpActorBase* actor);

HarpResult runtime_get_executable_directory(HarpRuntime *runtime, const char **out_path);
HarpResult runtime_get_working_directory(HarpRuntime *runtime, const char **out_path);
HarpResult runtime_get_package_directory(HarpRuntime *runtime, const HarpName name, const char **out_path);


#endif /* IMPL_HARP_API_H */