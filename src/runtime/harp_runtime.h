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


struct HarpRuntimeHandler {
    HarpHandlerDesc descriptor;
    HarpHandlerBase *instance;

    uint32_t dependent_count;
    uint32_t actor_count;
};
struct HarpRuntimeActor {
    HarpActorDesc descriptor;
    hmem_book_t inst_book;
    hmem_block_t inst_block; // memory leaks

    uint32_t page_growth_index;

    uint64_t actor_count;
    uint64_t actor_capacity;
    HarpActorBase **actors; // memory leaks
};

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


HarpResult harp_setup_runtime(HarpRuntime *runtime, HarpRuntimeCreator *creator);
void harp_teardown_runtime(HarpRuntime *runtime);

void *harp_alloc_global(HarpRuntime *runtime, size_t size, size_t alignment);


HarpResult harp_setup_runtime_actor(HarpRuntimeActor *actor, uint64_t instance_size, uint64_t instance_alignment);
void harp_teardown_runtime_actor(HarpRuntimeActor *actor);

HarpActorBase *harp_alloc_actor(HarpRuntime *runtime, HarpRuntimeActor *ract);


#endif /* RUNTIME_HARP_RUNTIME_H */