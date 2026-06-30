#include "harp_registry.h"

#include <hmem/utils/hmem_align.h>
#include <hmem/hmem_os.h>

#include <stdlib.h>
#include <string.h>
#include <stdalign.h>


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

// Hash a Name
static uint32_t name_hash(HarpName name) {
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

// Allocate in the Registry
static HarpName harp_registry_alloc_name(HarpRegistry *registry, HarpName name) {
    if(registry == NULL || name == NULL)
        return NULL;

    size_t name_len = strlen(name) + 1;
    char *rname =
        hmem_arena_alloc(
            &registry->name_arena,
            name_len,
            alignof(char)
        );
    
    if(rname == NULL) {
        hmem_page_t page = {0};
        page.pool = hmem_os_alloc_pages(registry->page_size);

        if(page.pool == NULL)
            return NULL;

        page.capacity = registry->page_size;

        if(!hmem_book_push(&registry->name_book, &page)) {
            hmem_os_free_pages(page.pool, page.capacity);
            return NULL;
        }

        rname = hmem_arena_alloc(
            &registry->name_arena,
            name_len,
            alignof(char)
        );
        if(rname == NULL)
            return NULL;
    }

    memcpy(rname, name, name_len);
    return rname;
}


HarpResult harp_setup_registry(HarpRegistry *registry) {
    if(registry == NULL)
        return HARP_RESULT_INVALID_ARGUMENTS;

    // Buckets
    int bucket_i = 0;

    for (; bucket_i < HARP_REGISTRY_BUCKET_COUNT; ++bucket_i) {
        registry->buckets[bucket_i].entries =
            malloc(sizeof(HarpRegistryEntry) * 8);

        if (!registry->buckets[bucket_i].entries)
            goto fail_buckets;

        registry->buckets[bucket_i].capacity = 8;
        registry->buckets[bucket_i].count = 0;
    }

    // Arena
    registry->page_size =
        hmem_clamp(hmem_os_page_size(), 1024, SIZE_MAX);

    if(!hmem_setup_book(&registry->name_book, 16))
        goto fail_buckets;

    hmem_page_t page = {0};
    page.pool = hmem_os_alloc_pages(registry->page_size);
    if(page.pool == NULL)
        goto fail_book;

    page.capacity = registry->page_size;

    if(!hmem_book_push(&registry->name_book, &page)) {
        hmem_os_free_pages(page.pool, page.capacity);
        goto fail_book;
    }

    if(!hmem_setup_arena(&registry->name_arena, &registry->name_book))
        goto fail_pages;

    return HARP_RESULT_OK;
fail_pages:
    while (hmem_book_pop(&registry->name_book, &page)) {
        hmem_os_free_pages(page.pool, page.capacity);
        hmem_teardown_page(&page);
    }

fail_book:
    hmem_teardown_book(&registry->name_book);    

fail_buckets:
    // rollback registry buckets
    for (int i = 0; i < bucket_i; ++i) {
        free(registry->buckets[i].entries);
    }

    return HARP_RESULT_FAILED;
}
void harp_teardown_registry(HarpRegistry *registry) {
    if(registry == NULL) return;

    hmem_teardown_arena(&registry->name_arena);

    hmem_page_t page;
    while (hmem_book_pop(&registry->name_book, &page)) {
        hmem_os_free_pages(page.pool, page.capacity);
        hmem_teardown_page(&page);
    }

    hmem_teardown_book(&registry->name_book);

    for (int i = 0; i < HARP_REGISTRY_BUCKET_COUNT; ++i) {
        free(registry->buckets[i].entries);
    }
}

HarpName harp_registry_name(HarpRegistry *registry, HarpName name) {
    if(registry == NULL || name == NULL)
        return NULL;

    // name information
    uint32_t hash = name_hash(name);
    HarpRegistryBucket *bucket = &registry->buckets[hash & (HARP_REGISTRY_BUCKET_COUNT - 1)];

    // check entry
    uint32_t entry_idx = bucket_find_ci(bucket, name, hash);
    if(entry_idx != UINT32_MAX)
        return bucket->entries[entry_idx].name;

    // if need resize
    if(bucket->count+1 >= bucket->capacity) {
        size_t tmp_capacity = bucket->capacity * 2;

        void *tmp = realloc(
            bucket->entries,
            sizeof(HarpRegistryEntry) * tmp_capacity
        );
        if(tmp == NULL)
            return NULL; // no more memory

        bucket->capacity = tmp_capacity;
        bucket->entries = tmp;
    }

    // copy the name in the registry memory
    const char *rname = harp_registry_alloc_name(registry, name);
    if(rname == NULL)
        return NULL; // no more memory
    name = rname;

    // add new entry
    entry_idx = bucket_find_ii(bucket, name, hash);
    if(entry_idx != bucket->count) {
        memmove(&bucket->entries[entry_idx + 1],
                &bucket->entries[entry_idx],
                (bucket->count - entry_idx) * sizeof(HarpRegistryEntry));
    }

    ++bucket->count;
    bucket->entries[entry_idx].name = name;
    bucket->entries[entry_idx].hash = hash;
    bucket->entries[entry_idx].type = HARP_REGISTRY_ENTRY_TYPE_RESERVED;
    bucket->entries[entry_idx].runtime = NULL;
    return name;
}

HarpResult harp_registry_bind(HarpRegistry *registry, HarpName name, HarpRegistryEntryType type, void *runtime) {
    if(registry == NULL || name == NULL || runtime == NULL || type == HARP_REGISTRY_ENTRY_TYPE_INVALID)
        return HARP_RESULT_INVALID_ARGUMENTS;

    // entry information
    uint32_t hash = name_hash(name);
    HarpRegistryBucket *bucket = &registry->buckets[hash & (HARP_REGISTRY_BUCKET_COUNT - 1)];

    uint32_t entry_idx = bucket_find_ci(bucket, name, hash);
    if(entry_idx == UINT32_MAX) {
        return HARP_RESULT_NAME_NOT_FOUND;
        /* name = harp_registry_name(registry, name);
        if(name == NULL)
            return HARP_RESULT_OUT_OF_MEMORY;
        // I could technically cast the name to the Entry, but decided to find_ci instead...
        entry_idx = bucket_find_ci(bucket, name, hash); */
    }
    HarpRegistryEntry *entry = &bucket->entries[entry_idx];

    // check conflicting types
    if(entry->type != HARP_REGISTRY_ENTRY_TYPE_RESERVED && entry->type != type)
        return HARP_RESULT_NAME_TYPE_MISMATCH;

    // update the entry
    entry->type = type;
    entry->runtime = runtime;

    return HARP_RESULT_OK;
}
HarpResult harp_registry_unbind(HarpRegistry *registry, HarpName name, HarpRegistryEntryType type) {
    if(registry == NULL || name == NULL || type == HARP_REGISTRY_ENTRY_TYPE_INVALID)
        return HARP_RESULT_INVALID_ARGUMENTS;

    // entry information
    uint32_t hash = name_hash(name);
    HarpRegistryBucket *bucket = &registry->buckets[hash & (HARP_REGISTRY_BUCKET_COUNT - 1)];

    uint32_t entry_idx = bucket_find_ci(bucket, name, hash);
    if(entry_idx == UINT32_MAX)
        return HARP_RESULT_NAME_NOT_FOUND;
    HarpRegistryEntry *entry = &bucket->entries[entry_idx];

    // check type
    if(entry->type != type)
        return HARP_RESULT_NAME_TYPE_MISMATCH;

    // update entry
    entry->type = HARP_REGISTRY_ENTRY_TYPE_RESERVED;
    entry->runtime = NULL;

    return HARP_RESULT_OK;
}

HarpRegistryEntryType harp_registry_get_type(HarpRegistry *registry, HarpName name) {
    if(registry == NULL || name == NULL)
        return HARP_REGISTRY_ENTRY_TYPE_INVALID;

    // entry information
    uint32_t hash = name_hash(name);
    HarpRegistryBucket *bucket = &registry->buckets[hash & (HARP_REGISTRY_BUCKET_COUNT - 1)];

    uint32_t entry_idx = bucket_find_ci(bucket, name, hash);
    if(entry_idx == UINT32_MAX)
        return HARP_REGISTRY_ENTRY_TYPE_INVALID;
    HarpRegistryEntry *entry = &bucket->entries[entry_idx];

    return entry->type;
}
void *harp_registry_get_runtime(HarpRegistry *registry, HarpName name, HarpRegistryEntryType type) {
    if(registry == NULL || name == NULL || type == HARP_REGISTRY_ENTRY_TYPE_INVALID)
        return NULL;

    // entry information
    uint32_t hash = name_hash(name);
    HarpRegistryBucket *bucket = &registry->buckets[hash & (HARP_REGISTRY_BUCKET_COUNT - 1)];

    uint32_t entry_idx = bucket_find_ci(bucket, name, hash);
    if(entry_idx == UINT32_MAX)
        return NULL;
    HarpRegistryEntry *entry = &bucket->entries[entry_idx];

    // check type
    if(entry->type != type)
        return NULL;

    return entry->runtime;
}
