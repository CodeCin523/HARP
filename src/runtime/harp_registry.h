#ifndef RUNTIME_REGISTRY_H
#define RUNTIME_REGISTRY_H

#include <harp/harp_api.h>

#include <hmem/hmem_book.h>
#include <hmem/hmem_arena.h>


#define HARP_REGISTRY_BUCKET_COUNT (128)


typedef uint8_t HarpRegistryEntryType;
enum {
    HARP_REGISTRY_ENTRY_TYPE_INVALID = 0,
    HARP_REGISTRY_ENTRY_TYPE_RESERVED = 1,
    HARP_REGISTRY_ENTRY_TYPE_HANDLER = 2,
    HARP_REGISTRY_ENTRY_TYPE_ACTOR = 3,
    HARP_REGISTRY_ENTRY_TYPE_PACKAGE = 4,
};

typedef struct {
    HarpName name;              // Name of the registered object
    uint32_t hash;              // The full hash of the name

    HarpRegistryEntryType type; // Type tag

    void *runtime;
} HarpRegistryEntry;

typedef struct {
    HarpRegistryEntry *entries; // Dynamic array of entries
    uint32_t count;             // Number of entries
    uint32_t capacity;          // Allocated size
} HarpRegistryBucket;

typedef struct {
    HarpRegistryBucket buckets[HARP_REGISTRY_BUCKET_COUNT]; // Fixed-size bucket array

    hmem_book_t name_book;
    hmem_arena_t name_arena;

    size_t page_size;
} HarpRegistry;


HarpResult harp_setup_registry(HarpRegistry *registry);
void harp_teardown_registry(HarpRegistry *registry);

HarpName harp_registry_name(HarpRegistry *registry, HarpName name);

HarpResult harp_registry_bind(HarpRegistry *registry, HarpName name, HarpRegistryEntryType type, void *runtime);
HarpResult harp_registry_unbind(HarpRegistry *registry, HarpName name, HarpRegistryEntryType type);

HarpRegistryEntryType harp_registry_get_type(HarpRegistry *registry, HarpName name);
void *harp_registry_get_runtime(HarpRegistry *registry, HarpName name, HarpRegistryEntryType type);


#endif /* RUNTIME_REGISTRY_H */