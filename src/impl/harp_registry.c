#include "harp_registry.h"

#include <string.h>


// Find Current Index
static uint32_t bucket_find_ci(HarpRegistryBucket *bucket, HarpName name) {
    uint32_t low = 0, high = bucket->count, mid, cmp;

    while(low < high) {
        mid = low + (high - low) / 2;
        cmp = strcmp(name, bucket->entries[mid].name);
        if(cmp == 0)
            return mid;
        else if(cmp < 0)
            low = mid + 1;
        else
            high = mid;
    }

    return UINT32_MAX;
}
// Find Insert Index
static uint32_t bucket_find_ii(HarpRegistryBucket *bucket, HarpName name) {
    uint32_t low = 0, high = bucket->count, mid, cmp;

    while(low < high) {
        mid = low + (high - low) / 2;
        cmp = strcmp(name, bucket->entries[mid].name);
        
        if(cmp < 0)
            low = mid + 1;
        else
            high = mid;
    }

    return low;
}


uint32_t harp_registry_hash(HarpName name) {
    uint32_t hash = 2166136261u; // FNV offset basis
    for (const char *p = name; *p; ++p) {
        hash ^= (uint8_t)(*p);
        hash *= 16777619u; // FNV prime
    }
    return hash;
}

HarpRegistryEntry *harp_registry_insert(HarpRuntime *runtime, HarpRegistry *registry, HarpName name) {

}
HarpRegistryEntry *harp_registry_find(HarpRuntime *runtime, HarpRegistry *registry, HarpName name) {
    // CHECK

    uint32_t bucket_idx = harp_registry_hash(name) % HARP_REGISTRY_BUCKET_COUNT;
    uint32_t entry_idx = bucket_find_ci(&registry->buckets[bucket_idx], name);
    if(entry_idx == UINT32_MAX)
        return NULL;
    return &registry->buckets[bucket_idx].entries[entry_idx];
}
HarpResult harp_registry_remove(HarpRuntime *runtime, HarpRegistry *registry, HarpName name) {

}