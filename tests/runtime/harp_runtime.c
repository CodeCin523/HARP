#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdalign.h>

#include <runtime/harp_runtime.h>


static HarpRuntimeCreator runtime_creator = {
    .argv0 = "./test_harp_runtime"
};


static void test_setup_teardown() {
    HarpRuntime runtime = {0};

    HarpResult result =
        harp_setup_runtime(
            &runtime,
            &runtime_creator
        );

    assert(result == HARP_RESULT_OK);

    // Registry buckets initialized
    for(uint32_t i = 0; i < HARP_REGISTRY_BUCKET_COUNT; ++i) {
        assert(runtime.registry.buckets[i].entries != NULL);
        assert(runtime.registry.buckets[i].capacity == 8);
        assert(runtime.registry.buckets[i].count == 0);
    }

    // Arena/book initialized
    assert(runtime.global_book.pages != NULL);
    assert(runtime.global_book.current > 0);

    assert(runtime.page_size >= 16384);

    // Paths initialized
    assert(runtime.executable_directory != NULL);
    assert(runtime.working_directory != NULL);

    printf("[OK] runtime setup\n");

    printf("Executable Directory: %s\n",
           runtime.executable_directory);

    printf("Working Directory: %s\n",
           runtime.working_directory);

    harp_teardown_runtime(&runtime);

    printf("[OK] runtime teardown\n");
}


static void test_global_alloc_single() {
    HarpRuntime runtime = {0};

    assert(
        harp_setup_runtime(
            &runtime,
            &runtime_creator
        ) == HARP_RESULT_OK
    );

    void *ptr =
        harp_alloc_global(
            &runtime,
            64,
            alignof(max_align_t)
        );

    assert(ptr != NULL);

    printf("[OK] single global allocation\n");

    harp_teardown_runtime(&runtime);
}


static void test_global_alloc_multiple() {
    HarpRuntime runtime = {0};

    assert(
        harp_setup_runtime(
            &runtime,
            &runtime_creator
        ) == HARP_RESULT_OK
    );

    void *a =
        harp_alloc_global(
            &runtime,
            32,
            8
        );

    void *b =
        harp_alloc_global(
            &runtime,
            64,
            16
        );

    void *c =
        harp_alloc_global(
            &runtime,
            128,
            32
        );

    assert(a != NULL);
    assert(b != NULL);
    assert(c != NULL);

    assert(a != b);
    assert(a != c);
    assert(b != c);

    printf("[OK] multiple global allocations\n");

    harp_teardown_runtime(&runtime);
}


static void test_global_alloc_alignment() {
    HarpRuntime runtime = {0};

    assert(
        harp_setup_runtime(
            &runtime,
            &runtime_creator
        ) == HARP_RESULT_OK
    );

    void *ptr16 =
        harp_alloc_global(
            &runtime,
            64,
            16
        );

    void *ptr32 =
        harp_alloc_global(
            &runtime,
            64,
            32
        );

    assert(ptr16 != NULL);
    assert(ptr32 != NULL);

    assert(
        ((uintptr_t)ptr16 % 16) == 0
    );

    assert(
        ((uintptr_t)ptr32 % 32) == 0
    );

    printf("[OK] allocation alignment\n");

    harp_teardown_runtime(&runtime);
}


static void test_global_alloc_growth() {
    HarpRuntime runtime = {0};

    assert(
        harp_setup_runtime(
            &runtime,
            &runtime_creator
        ) == HARP_RESULT_OK
    );

    uint8_t initial_pages =
        runtime.global_book.current;

    // Force arena growth
    size_t alloc_size =
        runtime.page_size / 4;

    for(int i = 0; i < 16; ++i) {
        void *ptr =
            harp_alloc_global(
                &runtime,
                alloc_size,
                alignof(max_align_t)
            );

        assert(ptr != NULL);
    }

    assert(
        runtime.global_book.current >
        initial_pages
    );

    printf("[OK] arena growth\n");

    harp_teardown_runtime(&runtime);
}


static void test_invalid_arguments() {
    // setup_runtime
    assert(
        harp_setup_runtime(
            NULL,
            &runtime_creator
        ) == HARP_RESULT_INVALID_ARGUMENTS
    );

    assert(
        harp_setup_runtime(
            &(HarpRuntime){0},
            NULL
        ) == HARP_RESULT_INVALID_ARGUMENTS
    );

    // alloc
    assert(
        harp_alloc_global(
            NULL,
            64,
            8
        ) == NULL
    );

    HarpRuntime runtime = {0};

    assert(
        harp_setup_runtime(
            &runtime,
            &runtime_creator
        ) == HARP_RESULT_OK
    );

    assert(
        harp_alloc_global(
            &runtime,
            0,
            8
        ) == NULL
    );

    assert(
        harp_alloc_global(
            &runtime,
            64,
            0
        ) == NULL
    );

    printf("[OK] invalid arguments\n");

    harp_teardown_runtime(&runtime);
}


int main() {
    test_setup_teardown();

    test_global_alloc_single();
    test_global_alloc_multiple();
    test_global_alloc_alignment();
    test_global_alloc_growth();

    test_invalid_arguments();

    printf("\nALL TESTS PASSED\n");

    return 0;
}