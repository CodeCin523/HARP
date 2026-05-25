#include "harp_runtime.h"

#include <hmem/hmem_os.h>


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