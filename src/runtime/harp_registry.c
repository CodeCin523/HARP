#include "harp_registry.h"

#include <stdint.h>
#include <string.h>


// Find Current Index
static uint32_t bucket_find_ci(HarpRegistryBucket *bucket, HarpName name, uint32_t hash) {
    uint32_t low = 0, high = bucket->count, mid, cmp;
    HarpRegistryEntry *entry;

    while(low < high) {
        mid = low + (high - low) / 2;

        entry = &bucket->entries[mid];
        cmp = (hash < entry->hash) ? -1 :
                (hash > entry->hash) ?  1 :
                strcmp(name, entry->name);

        if(cmp == 0)
            return mid;
        else if(cmp > 0)
            low = mid + 1;
        else
            high = mid;
    }

    return UINT32_MAX;
}
// Find Insert Index
static uint32_t bucket_find_ii(HarpRegistryBucket *bucket, HarpName name, uint32_t hash) {
    uint32_t low = 0, high = bucket->count, mid, cmp;
    HarpRegistryEntry *entry;

    while(low < high) {
        mid = low + (high - low) / 2;

        entry = &bucket->entries[mid];
        cmp = (hash < entry->hash) ? -1 :
                (hash > entry->hash) ?  1 :
                strcmp(name, entry->name);
        
        if(cmp > 0)
            low = mid + 1;
        else
            high = mid;
    }

    return low;
}


uint32_t harp_registry_hash(HarpName name) {
    uint32_t hash = 2166136261u;

    while (*name) {
        hash ^= (unsigned char)(*name++);
        hash *= 16777619u;
    }

    // extra mixing step (important)
    hash ^= hash >> 16;
    hash *= 0x7feb352d;
    hash ^= hash >> 15;
    hash *= 0x846ca68b;
    hash ^= hash >> 16;

    return hash;
}

HarpRegistryEntry *harp_registry_insert(HarpRuntime *runtime, HarpRegistry *registry, HarpName name) {
    if (!registry || !name)
        return NULL;

    uint32_t hash = harp_registry_hash(name);
    HarpRegistryBucket *bucket = &registry->buckets[hash  & (HARP_REGISTRY_BUCKET_COUNT - 1)];

    if(bucket_find_ci(bucket, name, hash) != UINT32_MAX)
        return NULL;
    if(bucket->count+1 >= bucket->capacity)
        return NULL;

    uint32_t entry_idx = bucket_find_ii(bucket, name, hash);
    if(entry_idx != bucket->count) {
        memmove(&bucket->entries[entry_idx + 1],
                &bucket->entries[entry_idx],
                (bucket->count - entry_idx) * sizeof(HarpRegistryEntry));
    }
    ++bucket->count;

    bucket->entries[entry_idx].name = name;
    bucket->entries[entry_idx].hash = hash;

    return &bucket->entries[entry_idx];
}
HarpRegistryEntry *harp_registry_find(HarpRuntime *runtime, HarpRegistry *registry, HarpName name) {
    if (!registry || !name)
        return NULL;

    uint32_t hash = harp_registry_hash(name);
    HarpRegistryBucket *bucket = &registry->buckets[hash  & (HARP_REGISTRY_BUCKET_COUNT - 1)];

    uint32_t entry_idx = bucket_find_ci(bucket, name, hash);
    if(entry_idx == UINT32_MAX)
        return NULL;
    return &bucket->entries[entry_idx];
}
void harp_registry_remove(HarpRuntime *runtime, HarpRegistry *registry, HarpName name) {
    if (!registry || !name)
        return;

    uint32_t hash = harp_registry_hash(name);
    HarpRegistryBucket *bucket = &registry->buckets[hash  & (HARP_REGISTRY_BUCKET_COUNT - 1)];

    uint32_t entry_idx = bucket_find_ci(bucket, name, hash);
    if(entry_idx == UINT32_MAX)
        return;

    if(--bucket->count != entry_idx) {
        memmove(&bucket->entries[entry_idx],
                &bucket->entries[entry_idx + 1],
                (bucket->count - entry_idx) * sizeof(HarpRegistryEntry));
    }

    // bucket->entries[entry_idx].name = NULL;
    // bucket->entries[entry_idx].ptr = NULL;
    // bucket->entries[entry_idx].hash = 0;
    // bucket->entries[entry_idx].type = HARP_REGISTRY_ENTRY_TYPE_NONE;
}