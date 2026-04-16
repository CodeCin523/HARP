#ifndef IMPL_HARP_REGISTRY_H
#define IMPL_HARP_REGISTRY_H


#include <harp/harp_core.h>


#define HARP_REGISTRY_BUCKET_COUNT (128)


typedef uint8_t HarpRegistryEntryType;
enum {
    HARP_REGISTRY_ENTRY_TYPE_API = 0,
    HARP_REGISTRY_ENTRY_TYPE_HANDLER = 1,
    HARP_REGISTRY_ENTRY_TYPE_ACTOR = 2
};

typedef struct {
    char *name;                // Name of the registered object
    void *ptr;                 // Pointer to the object (API/handler/actor)
    HarpRegistryEntryType type;// Type tag
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
HarpResult harp_registry_remove(HarpRuntime *runtime, HarpRegistry *registry, HarpName name);


#endif /* IMPL_HARP_REGISTRY_H */