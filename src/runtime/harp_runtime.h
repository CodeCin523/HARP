#ifndef RUNTIME_HARP_RUNTIME_H
#define RUNTIME_HARP_RUNTIME_H

#include <harp/harp.h>
#include "harp_registry.h"

#include <hmem/hmem_arena.h>
#include <hmem/hmem_block.h>
#include <hmem/hmem_book.h>


typedef enum HarpRuntimeState HarpRuntimeState; 

typedef struct HarpPackageRuntimeDesc HarpPackageRuntimeDesc;
typedef struct HarpHandlerRuntimeDesc HarpHandlerRuntimeDesc;
typedef struct HarpActorRuntimeDesc HarpActorRuntimeDesc;
typedef struct HarpApiRuntimeDesc HarpApiRuntimeDesc;

enum HarpRuntimeState {
    HARP_RUNTIME_STATE_UNINITIALIZED,
    HARP_RUNTIME_STATE_INITIALIZING,
    HARP_RUNTIME_STATE_INITIALIZED,
    HARP_RUNTIME_STATE_TERMINATING,
};

struct HarpPackageRuntimeDesc {
    HarpPackageDesc _base;

    char *directory;
};
struct HarpHandlerRuntimeDesc {
    HarpHandlerDesc _base;
    HarpHandlerBase *instance;

    uint32_t dependent_count;
    uint32_t actor_count;

    HarpRuntimeState state;
};
struct HarpActorRuntimeDesc {
    HarpActorDesc _base;
    hmem_book_t inst_book;
    hmem_block_t inst_block;
    
    uint32_t growth_index;
};
struct HarpApiRuntimeDesc {
    HarpApiDesc _base;
    HarpApiBase *instance;
};

struct HarpRuntime {
    HarpHandlerBase _base;

    HarpRegistry registry;

    HarpCoreApi *core_api;

    char *executable_directory;
    char *working_directory;

    hmem_book_t global_book;
    hmem_arena_t global_arena;

    size_t page_size;
};


HarpResult harp_setup_runtime(HarpRuntime *runtime, HarpRuntimeCreator *creator);
void harp_teardown_runtime(HarpRuntime *runtime);

void *harp_alloc_global(HarpRuntime *runtime, size_t size, size_t alignment);

HarpActorBase *harp_alloc_actor(HarpRuntime *runtime, HarpActorRuntimeDesc *rdesc);


#endif /* RUNTIME_HARP_RUNTIME_H */