#ifndef RUNTIME_HARP_RUNTIME_H
#define RUNTIME_HARP_RUNTIME_H

#include <harp/harp_core.h>
#include "harp_registry.h"

#include <hmem/hmem_arena.h>
#include <hmem/hmem_block.h>
#include <hmem/hmem_book.h>


typedef struct HarpPackageRuntimeDesc HarpPackageRuntimeDesc;
typedef struct HarpHandlerRuntimeDesc HarpHandlerRuntimeDesc;
typedef struct HarpActorRuntimeDesc HarpActorRuntimeDesc;
typedef struct HarpApiRuntimeDesc HarpApiRuntimeDesc;

struct HarpPackageRuntimeDesc {
    HarpPackageDesc _base;
};
struct HarpHandlerRuntimeDesc {
    HarpHandlerDesc _base;
};
struct HarpActorRuntimeDesc {
    HarpActorDesc _base;
    hmem_book_t inst_book;
    hmem_block_t inst_block;
};
struct HarpApiRuntimeDesc {
    HarpApiDesc _base;
};

struct HarpRuntime {
    HarpHandlerBase _base;

    HarpRegistry registry;

    hmem_book_t global_book;
    hmem_arena_t global_arena;

    size_t page_size;
};


void *harp_runtime_global_alloc(HarpRuntime *runtime, size_t size, size_t alignment);


#endif /* RUNTIME_HARP_RUNTIME_H */