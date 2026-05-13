#ifndef IMPL_HARP_API_H
#define IMPL_HARP_API_H


#include <harp/harp_core.h>
#include "registry.h"

#include <hmem/hmem_arena.h>
#include <hmem/hmem_book.h>


typedef struct HarpPackageDescImpl {
    HarpPackageDesc _base;
} HarpPackageDescImpl;
typedef struct HarpHandlerDescImpl {
    HarpHandlerDesc _base;
} HarpHandlerDescImpl;
typedef struct HarpActorDescImpl {
    HarpActorDesc _base;
} HarpActorDescImpl;
typedef struct HarpApiDescImpl {
    HarpApiDesc _base;
} HarpApiDescImpl;

struct HarpRuntime {
    HarpRegistry registry;

    hmem_book_t desc_book;
    hmem_arena_t desc_arena;

    size_t page_size;
};


HarpResult register_api(HarpCoreApi api, const HarpApiDesc* desc, HarpApiBase** out_api);
HarpResult register_handler(HarpCoreApi api, const HarpHandlerDesc* desc);
HarpResult register_actor(HarpCoreApi api, const HarpActorDesc* desc);

HarpResult get_api(HarpCoreApi api, const HarpName name, HarpApiBase **out_api);
HarpResult get_handler(HarpCoreApi api, const HarpName name, HarpHandlerBase** out_handler);
HarpResult get_api_desc(HarpCoreApi api, const HarpName name, HarpApiDesc **out_desc);
HarpResult get_handler_desc(HarpCoreApi api, const HarpName name, HarpHandlerDesc** out_desc);
HarpResult get_actor_desc(HarpCoreApi api, const HarpName name, HarpActorDesc** out_desc);

HarpResult handler_initialize(HarpCoreApi api, const HarpName name, const HarpCreatorBase* creator);
HarpResult handler_terminate(HarpCoreApi api, const HarpName name);

HarpResult actor_create(HarpCoreApi api, const HarpName name, const HarpCreatorBase* creator, HarpActorBase** out_actor);
HarpResult actor_destroy(HarpCoreApi api, const HarpName name, HarpActorBase* actor);


#endif /* IMPL_HARP_API_H */