#include "harp_runtime.h"

#include <hmem/hmem_os.h>
#include <hmem/utils/hmem_align.h>

#include <stdlib.h>


HarpResult harp_setup_runtime(HarpRuntime *runtime) {
    if (runtime == NULL)
        return HARP_RESULT_INVALID_ARGUMENTS;

    // registry
    harp_setup_registry(&runtime->registry);

    // Descriptor setup
    runtime->page_size =
        hmem_clamp(hmem_os_page_size(), 16384, SIZE_MAX);

    if (!hmem_setup_book(&runtime->global_book, 16))
        goto fail_registry;

    hmem_page_t page = {0};

    page.pool = hmem_os_alloc_pages(runtime->page_size);
    if (!page.pool)
        goto fail_book;

    page.capacity = runtime->page_size;

    if (!hmem_book_push(&runtime->global_book, &page)) {
        hmem_os_free_pages(page.pool, page.capacity);
        goto fail_book;
    }

    if (!hmem_setup_arena(&runtime->global_arena, &runtime->global_book))
        goto fail_pages;

    runtime->core_api = NULL;

    return HARP_RESULT_OK;
fail_pages:
    while (hmem_book_pop(&runtime->global_book, &page)) {
        hmem_os_free_pages(page.pool, page.capacity);
        hmem_teardown_page(&page);
    }

fail_book:
    hmem_teardown_book(&runtime->global_book);

fail_registry:
    // the registry clear itself correctly
    // harp_teardown_registry(&runtime->registry);

    return HARP_RESULT_FAILED;
}
void harp_teardown_runtime(HarpRuntime *runtime) {
    if (!runtime) return;

    hmem_teardown_arena(&runtime->global_arena);

    hmem_page_t page;
    while (hmem_book_pop(&runtime->global_book, &page)) {
        hmem_os_free_pages(page.pool, page.capacity);
        hmem_teardown_page(&page);
    }

    hmem_teardown_book(&runtime->global_book);

    harp_teardown_registry(&runtime->registry);
}

void *harp_runtime_global_alloc(HarpRuntime *runtime, size_t size, size_t alignment) {
    if(runtime == NULL || size == 0 || alignment == 0)
        return NULL;

    // First attempt
    void *ptr = hmem_arena_alloc(
        &runtime->global_arena, size, alignment);
    if(ptr != NULL)
        return ptr;

    // Arena exhausted: allocate a new page
    hmem_page_t page = {0};
    page.pool = hmem_os_alloc_pages(runtime->page_size);

    if(page.pool == NULL)
        return NULL;

    page.capacity = runtime->page_size;

    // Push page into book
    if(!hmem_book_push(&runtime->global_book, &page)) {
        hmem_os_free_pages(page.pool, page.capacity);
        return NULL;
    }

    // Retry allocation after growth
    return hmem_arena_alloc(
        &runtime->global_arena, size, alignment);
}