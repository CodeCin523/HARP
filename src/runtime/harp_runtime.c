#include "harp_runtime.h"

#include <hmem/hmem_os.h>
#include <hmem/utils/hmem_align.h>

#include <unistd.h>
#include <limits.h>
#include <string.h>
#include <stdlib.h>
#include <stdalign.h>


static char *runtime_strdup(HarpRuntime *runtime, const char *str) {
    if(runtime == NULL || str == NULL)
        return NULL;

    size_t len = strlen(str) + 1;

    char *dst =
        harp_alloc_global(
            runtime,
            len,
            alignof(char)
        );

    if(dst == NULL)
        return NULL;

    memcpy(dst, str, len);
    return dst;
}

static HarpResult runtime_setup_paths(
    HarpRuntime *runtime,
    const HarpRuntimeCreator *creator
) {
    if(runtime == NULL || creator == NULL)
        return HARP_RESULT_INVALID_ARGUMENTS;

    runtime->executable_directory = NULL;
    runtime->working_directory = NULL;

    /* ===================================================================== */
    /* WORKING DIRECTORY                                                     */
    /* ===================================================================== */

    {
        char cwd[PATH_MAX];

        if(getcwd(cwd, sizeof(cwd)) == NULL)
            return HARP_RESULT_FAILED;

        runtime->working_directory =
            runtime_strdup(runtime, cwd);

        if(runtime->working_directory == NULL)
            return HARP_RESULT_OUT_OF_MEMORY;
    }

    /* ===================================================================== */
    /* EXECUTABLE DIRECTORY                                                  */
    /* ===================================================================== */

    {
        if(creator->argv0 == NULL)
            return HARP_RESULT_INVALID_ARGUMENTS;

        char resolved[PATH_MAX];

        if(realpath(creator->argv0, resolved) == NULL)
            return HARP_RESULT_FAILED;

        char *last_sep = strrchr(resolved, '/');

#ifdef _WIN32
        char *last_backslash = strrchr(resolved, '\\');

        if(last_backslash != NULL &&
           (last_sep == NULL || last_backslash > last_sep))
            last_sep = last_backslash;
#endif

        if(last_sep == NULL)
            return HARP_RESULT_FAILED;

        *last_sep = '\0';

        runtime->executable_directory =
            runtime_strdup(runtime, resolved);

        if(runtime->executable_directory == NULL)
            return HARP_RESULT_OUT_OF_MEMORY;
    }

    return HARP_RESULT_OK;
}


HarpResult harp_setup_runtime(HarpRuntime *runtime, HarpRuntimeCreator *creator) {
    if (runtime == NULL || creator == NULL)
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

    // paths
    if(runtime_setup_paths(runtime, creator) != HARP_RESULT_OK)
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

void *harp_alloc_global(HarpRuntime *runtime, size_t size, size_t alignment) {
    if(runtime == NULL || size == 0 || alignment == 0)
        return NULL;

    // First attempt
    void *ptr = hmem_arena_alloc(&runtime->global_arena, size, alignment);
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

HarpActorBase *harp_alloc_actor(HarpRuntime *runtime, HarpActorRuntimeDesc *rdesc) {
    if(runtime == NULL || rdesc == NULL)
        return NULL;

    void *ptr = hmem_block_alloc_single(&rdesc->inst_block);
    if(ptr != NULL)
        return ptr;

    size_t count = 4 << rdesc->page_growth_index;
    size_t size = count * rdesc->_base.instance_size;

    void *pool = malloc(size);

    if(pool == NULL)
        return NULL;

    hmem_page_t page = {
        .pool = pool,
        .capacity = size
    };

    if(!hmem_book_push(&rdesc->inst_book, &page)) {
        free(pool);
        return NULL;
    }

    hmem_block_update(&rdesc->inst_block);

    ++rdesc->page_growth_index;
    return hmem_block_alloc_single(&rdesc->inst_block);
}