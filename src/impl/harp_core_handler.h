#ifndef HARP_CORE_HANDLER_H
#define HARP_CORE_HANDLER_H

#include <harp/harp_core.h>


typedef struct HarpCoreHandlerImpl {
    HarpCoreHandler interface;
    HarpRuntime *p_runtime;
} HarpCoreHandlerImpl;


// HarpCoreApi's impl
HarpResult handler_register_handler(const HarpCoreHandler *h, const HarpHandlerDesc* desc);
HarpResult handler_register_actor(const HarpCoreHandler *h, const HarpActorDesc* desc);

HarpResult handler_get_handler(const HarpCoreHandler *h, const HarpDependencyDesc *dependency, HarpHandlerBase** out_handler);

HarpResult handler_get_actor_count(const HarpCoreHandler *h, const HarpName name, uint64_t *out_count);
HarpResult handler_get_actor_at(const HarpCoreHandler *h, const HarpName name, uint64_t index, HarpActorBase **out_actor);
HarpResult handler_get_actors(const HarpCoreHandler *h, const HarpName name, uint64_t *inout_count, HarpActorBase **out_actors);

HarpResult handler_get_handler_desc(const HarpCoreHandler *h, const HarpName name, HarpHandlerDesc** out_desc);
HarpResult handler_get_actor_desc(const HarpCoreHandler *h, const HarpName name, HarpActorDesc** out_desc);

HarpResult handler_handler_initialize(const HarpCoreHandler *h, const HarpName name, const HarpCreatorBase* creator);
HarpResult handler_handler_terminate(const HarpCoreHandler *h, const HarpName name);

HarpResult handler_actor_create(const HarpCoreHandler *h, const HarpName name, const HarpCreatorBase* creator, HarpActorBase** out_actor);
HarpResult handler_actor_destroy(const HarpCoreHandler *h, const HarpName name, HarpActorBase* actor);

HarpResult handler_get_executable_directory(const HarpCoreHandler *h, const char **out_path);
HarpResult handler_get_working_directory(const HarpCoreHandler *h, const char **out_path);
HarpResult handler_get_package_directory(const HarpCoreHandler *h, const HarpName name, const char **out_path);


// HarpRuntime's impl
HarpResult runtime_register_handler(HarpRuntime *runtime, const HarpHandlerDesc* desc);
HarpResult runtime_register_actor(HarpRuntime *runtime, const HarpActorDesc* desc);

HarpResult runtime_get_handler(HarpRuntime *runtime, const HarpDependencyDesc *dependency, HarpHandlerBase** out_handler);

HarpResult runtime_get_actor_count(HarpRuntime *runtime, const HarpName name, uint64_t *out_count);
HarpResult runtime_get_actor_at(HarpRuntime *runtime, const HarpName name, uint64_t index, HarpActorBase **out_actor);
HarpResult runtime_get_actors(HarpRuntime *runtime, const HarpName name, uint64_t *inout_count, HarpActorBase **out_actors);

HarpResult runtime_get_handler_desc(HarpRuntime *runtime, const HarpName name, HarpHandlerDesc** out_desc);
HarpResult runtime_get_actor_desc(HarpRuntime *runtime, const HarpName name, HarpActorDesc** out_desc);

HarpResult runtime_handler_initialize(HarpRuntime *runtime, const HarpName name, const HarpCreatorBase* creator);
HarpResult runtime_handler_terminate(HarpRuntime *runtime, const HarpName name);

HarpResult runtime_actor_create(HarpRuntime *runtime, const HarpName name, const HarpCreatorBase* creator, HarpActorBase** out_actor);
HarpResult runtime_actor_destroy(HarpRuntime *runtime, const HarpName name, HarpActorBase* actor);

HarpResult runtime_get_executable_directory(HarpRuntime *runtime, const char **out_path);
HarpResult runtime_get_working_directory(HarpRuntime *runtime, const char **out_path);
HarpResult runtime_get_package_directory(HarpRuntime *runtime, const HarpName name, const char **out_path);


#endif /* HARP_CORE_HANDLER_H */