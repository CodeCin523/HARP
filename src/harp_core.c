#include <harp/harp_core.h>
#include "impl/harp_api.h"

#define HMEM_IMPLEMENTATION
#include <hmem/hmem_arena.h>
#include <hmem/hmem_book.h>
#include <hmem/hmem_os.h>

#undef HARP_UTILS_UNDEF
#include <harp/utils/harp_api.h>
#include <harp/utils/harp_version.h>

#include <stdlib.h>


HarpResult harp_init(
    HarpRuntime **out_runtime
) {
    if (!out_runtime)
        return HARP_RESULT_MISSING_OUTPUT;

    *out_runtime = NULL;

    HarpRuntime *runtime = malloc(sizeof(*runtime));
    if (!runtime)
        return HARP_RESULT_FAILED;

    HarpRegistryEntry *entry_pool =
        malloc(sizeof(*entry_pool) *
               128 *
               HARP_REGISTRY_BUCKET_COUNT);

    if (!entry_pool)
        goto fail_runtime;

    // Initialize registry
    for (int i = 0; i < HARP_REGISTRY_BUCKET_COUNT; ++i) {
        runtime->registry.buckets[i].entries  = entry_pool;
        runtime->registry.buckets[i].capacity = 128;
        runtime->registry.buckets[i].count    = 0;

        entry_pool += 128;
    }

    // Setup descriptor memory
    runtime->page_size =
        hmem_clamp(hmem_os_page_size(), 16384, SIZE_MAX);

    if (!hmem_setup_book(&runtime->desc_book, 16))
        goto fail_registry;

    hmem_page_t page = {0};

    page.pool = hmem_os_alloc_pages(runtime->page_size);
    if (!page.pool)
        goto fail_book;

    page.capacity = runtime->page_size;

    if (!hmem_book_push(&runtime->desc_book, &page)) {
        hmem_os_free_pages(page.pool, page.capacity);
        goto fail_book;
    }

    // Setup descriptor arena
    if (!hmem_setup_arena(
            &runtime->desc_arena,
            &runtime->desc_book))
    {
        goto fail_pages;
    }

    *out_runtime = runtime;

    return HARP_RESULT_OK;

fail_pages:
    while (hmem_book_pop(&runtime->desc_book, &page)) {
        hmem_os_free_pages(page.pool, page.capacity);
        hmem_teardown_page(&page);
    }

fail_book:
    hmem_teardown_book(&runtime->desc_book);

fail_registry:
    free(runtime->registry.buckets[0].entries);

fail_runtime:
    free(runtime);

    return HARP_RESULT_FAILED;
}
HarpResult harp_exit(
    HarpRuntime *runtime
) {
    if (!runtime)
        return HARP_RESULT_INVALID_ARGUMENTS;

    // Unload actors
    // Unload handlers
    // Unload packages

    // Destroy descriptor arena
    hmem_teardown_arena(&runtime->desc_arena);

    // Release descriptor pages
    hmem_page_t page;

    while (hmem_book_pop(&runtime->desc_book, &page)) {
        hmem_os_free_pages(page.pool, page.capacity);
        hmem_teardown_page(&page);
    }

    hmem_teardown_book(&runtime->desc_book);

    // Release registry
    free(runtime->registry.buckets[0].entries);

    // Release runtime
    free(runtime);

    return HARP_RESULT_OK;
}

HarpResult harp_runtime_load_packages(
    HarpRuntime *runtime,
    const char *path
) {
    
}

HarpResult harp_runtime_get_api(
    HarpRuntime *runtime,
    const HarpName name,
    HarpApiBase **out_api
) {
    
}

HarpResult harp_runtime_get_package_count(
    HarpRuntime *runtime,
    uint64_t *out_count
) {
    
}
HarpResult harp_runtime_get_package_name(
    HarpRuntime *runtime,
    uint64_t index,
    HarpName *out_name
) {
    
}


/* wouldn't work, since the api is first element of runtime, we need manual allocation or at least overwrite
HarpResult harp_package_register(HarpApiBase *base) {
    static const HarpApiDesc core_api_desc = {
        HARP_CORE_API_VERSION,
        sizeof()
    }

    HarpCoreApi *core_api = HARP_API_AS(HarpCoreApi, base);

    core_api->register_api()
}

HarpResult harp_package_query(HarpPackageDesc** out_desc) {
    static HarpPackageDesc package_desc = {
        "Harp",
        HARP_MAKE_VERSION(1, 0, 0),
        harp_package_register,
        NULL,
        NULL,
        NULL,
        0
    };
    *out_desc = &package_desc;
    return HARP_RESULT_OK;
}*/