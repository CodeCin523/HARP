#ifndef IMPL_HARP_API_H
#define IMPL_HARP_API_H


#include <harp/harp.h>


HarpResult register_api(const HarpApiDesc* desc, HarpApiBase** out_api);
HarpResult register_handler(const HarpHandlerDesc* desc);
HarpResult register_actor(const HarpActorDesc* desc);

HarpResult get_api(const HarpName name, HarpApiBase **out_api);
HarpResult get_handler(const HarpName name, HarpHandlerBase** out_handler);
HarpResult get_api_desc(const HarpName name, HarpApiDesc **out_desc);
HarpResult get_handler_desc(const HarpName name, HarpHandlerDesc** out_desc);
HarpResult get_actor_desc(const HarpName name, HarpActorDesc** out_desc);

HarpResult handler_initialize(const HarpName name, const HarpCreatorBase* creator);
HarpResult handler_terminate(const HarpName name);

HarpResult actor_create(const HarpName name, const HarpCreatorBase* creator, HarpActorBase** out_actor);
HarpResult actor_destroy(const HarpName name, HarpActorBase* actor);


#endif /* IMPL_HARP_API_H */