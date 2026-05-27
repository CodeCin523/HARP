#ifndef RUNTIME_REGISTRY_H
#define RUNTIME_REGISTRY_H


#include <harp/harp_core.h>


#define HARP_REGISTRY_BUCKET_COUNT (128)


typedef uint8_t HarpRegistryEntryType;
enum {
    HARP_REGISTRY_ENTRY_TYPE_NONE = 0,
    HARP_REGISTRY_ENTRY_TYPE_API = 1,
    HARP_REGISTRY_ENTRY_TYPE_HANDLER = 2,
    HARP_REGISTRY_ENTRY_TYPE_ACTOR = 3
    // HARP_REGISTRY_ENTRY_TYPE_RESERVED = 4
};

typedef struct {
    HarpName name;              // Name of the registered object

    uint32_t hash;              // The full hash of the name
    HarpRegistryEntryType type; // Type tag

    void *p_desc;
    void *p_inst;
} HarpRegistryEntry;

typedef struct {
    HarpRegistryEntry *entries; // Dynamic array of entries
    uint32_t count;             // Number of entries
    uint32_t capacity;          // Allocated size
} HarpRegistryBucket;

typedef struct {
    HarpRegistryBucket buckets[HARP_REGISTRY_BUCKET_COUNT]; // Fixed-size bucket array
} HarpRegistry;


uint32_t harp_registry_hash(HarpName name);

HarpRegistryEntry *harp_registry_insert(HarpRuntime *runtime, HarpRegistry *registry, HarpName name);
HarpRegistryEntry *harp_registry_find(HarpRuntime *runtime, HarpRegistry *registry, HarpName name);

void harp_registry_remove(HarpRuntime *runtime, HarpRegistry *registry, HarpName name);


#endif /* RUNTIME_REGISTRY_H */