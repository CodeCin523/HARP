#ifndef RUNTIME_HARP_RUNTIME_H
#define RUNTIME_HARP_RUNTIME_H

#include <harp/harp.h>
#include "harp_registry.h"
#include "harp_package.h"

#include <hmem/hmem_arena.h>
#include <hmem/hmem_block.h>
#include <hmem/hmem_book.h>


typedef struct HarpRuntimeHandler HarpRuntimeHandler;
typedef struct HarpRuntimeActor HarpRuntimeActor;


struct HarpRuntime {
    HarpHandlerBase _base;

    HarpRegistry registry;
    HarpPackageManager package_manager;

    HarpCoreHandler *core_handler;

    char *executable_directory;
    char *working_directory;

    hmem_book_t global_book;
    hmem_arena_t global_arena;

    size_t page_size;
};

struct HarpRuntimeHandler {
    HarpHandlerDesc descriptor;
    HarpHandlerBase *instance;

    uint32_t dependent_count;
    uint32_t actor_count;
};
struct HarpRuntimeActor {
    HarpActorDesc descriptor;
    hmem_book_t inst_book;
    hmem_block_t inst_block;

    uint32_t page_growth_index;

    uint64_t actor_count;
    uint64_t actor_capacity;
    HarpActorBase **actors;
};


/* HarpRuntime */
HarpResult harp_setup_runtime(HarpRuntime *runtime, const HarpRuntimeDesc *desc);
void harp_teardown_runtime(HarpRuntime *runtime);

void *harp_runtime_alloc_global(HarpRuntime *runtime, size_t size, size_t alignment);

HarpResult harp_runtime_alloc_rhdl(HarpRuntime *runtime, HarpHandlerDesc *desc, HarpRuntimeHandler **out_rhdl, HarpHandlerBase **out_handler);
HarpResult harp_runtime_alloc_ract(HarpRuntime *runtime, HarpActorDesc *desc, HarpRuntimeActor **out_ract);

HarpResult harp_runtime_intern_hdl_desc(HarpRuntime *runtime, const HarpHandlerDesc *desc, HarpHandlerDesc *out_desc);
HarpResult harp_runtime_intern_act_desc(HarpRuntime *runtime, const HarpActorDesc *desc, HarpActorDesc *out_desc);


HarpResult harp_runtime_register_handler(HarpRuntime *runtime, const HarpHandlerDesc *desc, HarpHandlerBase **out_handler);
HarpResult harp_runtime_register_actor(HarpRuntime *runtime, const HarpActorDesc *desc);

/* HarpRuntimeHandler */
HarpResult harp_setup_rhdl(HarpRuntimeHandler *rhdl, HarpHandlerDesc *desc, HarpHandlerBase *handler);
void harp_teardown_rhdl(HarpRuntimeHandler *rhdl);

HarpResult harp_rhdl_check_dependencies(HarpRuntimeHandler *rhdl, HarpRuntime *runtime);
HarpResult harp_rhdl_check_dependents(HarpRuntimeHandler *rhdl, HarpRuntime *runtime);

HarpResult harp_rhdl_initialize(HarpRuntimeHandler *rhdl, HarpRuntime *runtime, HarpCreatorBase *creator);
HarpResult harp_rhdl_terminate(HarpRuntimeHandler *rhdl, HarpRuntime *runtime);


/* HarpRuntimeActor */
HarpResult harp_setup_ract(HarpRuntimeActor *ract, HarpActorDesc *desc);
void harp_teardown_ract(HarpRuntimeActor *ract);

HarpResult harp_ract_check_dependencies(HarpRuntimeActor *ract, HarpRuntime *runtime);
HarpResult harp_ract_check_dependents(HarpRuntimeActor *ract, HarpRuntime *runtime);

HarpResult harp_ract_alloc_actor(HarpRuntimeActor *ract, HarpRuntime *runtime, HarpActorBase **out_actor);
HarpResult harp_ract_free_actor(HarpRuntimeActor *ract, HarpRuntime *runtime, HarpActorBase *actor);

HarpResult harp_ract_create(HarpRuntimeActor *ract, HarpRuntime *runtime, HarpActorBase *actor, HarpCreatorBase *creator);
HarpResult harp_ract_destroy(HarpRuntimeActor *ract, HarpRuntime *runtime, HarpActorBase *actor);


#endif /* RUNTIME_HARP_RUNTIME_H */