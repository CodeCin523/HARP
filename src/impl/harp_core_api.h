#ifndef IMPL_HARP_API_H
#define IMPL_HARP_API_H

#include <harp/harp_core.h>


HarpResult register_api(HarpHandlerBase *handler, const HarpApiDesc* desc, HarpApiBase** out_api);
HarpResult register_handler(HarpHandlerBase *handler, const HarpHandlerDesc* desc);
HarpResult register_actor(HarpHandlerBase *handler, const HarpActorDesc* desc);

HarpResult get_api(HarpHandlerBase *handler, const HarpName name, HarpApiBase **out_api);
HarpResult get_handler(HarpHandlerBase *handler, const HarpName name, HarpHandlerBase** out_handler);
HarpResult get_api_desc(HarpHandlerBase *handler, const HarpName name, HarpApiDesc **out_desc);
HarpResult get_handler_desc(HarpHandlerBase *handler, const HarpName name, HarpHandlerDesc** out_desc);
HarpResult get_actor_desc(HarpHandlerBase *handler, const HarpName name, HarpActorDesc** out_desc);

HarpResult handler_initialize(HarpHandlerBase *handler, const HarpName name, const HarpCreatorBase* creator);
HarpResult handler_terminate(HarpHandlerBase *handler, const HarpName name);

HarpResult actor_create(HarpHandlerBase *handler, const HarpName name, const HarpCreatorBase* creator, HarpActorBase** out_actor);
HarpResult actor_destroy(HarpHandlerBase *handler, const HarpName name, HarpActorBase* actor);


#endif /* IMPL_HARP_API_H */