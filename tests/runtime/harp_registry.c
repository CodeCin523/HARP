#include <assert.h>
#include <stdint.h>
#include <stdio.h>

#include <runtime/harp_registry.h>


static void test_name_store() {
    HarpRegistry r = {0};
    assert(harp_setup_registry(&r) == HARP_RESULT_OK);

    HarpName a = harp_registry_name(&r, "foo");
    assert(a != NULL);

    HarpName b = harp_registry_name(&r, "foo");
    assert(b != NULL);

    // same interned string
    assert(a == b);

    HarpName c = harp_registry_name(&r, "bar");
    assert(c != NULL);
    assert(c != a);

    printf("[OK] name store\n");

    harp_teardown_registry(&r);
}

static void test_bind_get() {
    HarpRegistry r = {0};
    assert(harp_setup_registry(&r) == HARP_RESULT_OK);

    int value = 42;

    HarpName name = harp_registry_name(&r, "foo");
    assert(name != NULL);

    assert(
        harp_registry_bind(
            &r,
            name,
            HARP_REGISTRY_ENTRY_TYPE_HANDLER,
            &value
        ) == HARP_RESULT_OK
    );

    void *ptr =
        harp_registry_get_runtime(
            &r,
            "foo",
            HARP_REGISTRY_ENTRY_TYPE_HANDLER
        );

    assert(ptr == &value);

    printf("[OK] bind + get\n");

    harp_teardown_registry(&r);
}

static void test_bind_missing_name() {
    HarpRegistry r = {0};
    assert(harp_setup_registry(&r) == HARP_RESULT_OK);

    int value = 42;

    assert(
        harp_registry_bind(
            &r,
            "foo",
            HARP_REGISTRY_ENTRY_TYPE_HANDLER,
            &value
        ) == HARP_RESULT_NAME_NOT_FOUND
    );

    printf("[OK] bind missing name rejected\n");

    harp_teardown_registry(&r);
}

static void test_duplicate_bind() {
    HarpRegistry r = {0};
    assert(harp_setup_registry(&r) == HARP_RESULT_OK);

    int a = 1;
    int b = 2;

    HarpName name = harp_registry_name(&r, "foo");
    assert(name != NULL);

    assert(
        harp_registry_bind(
            &r,
            name,
            HARP_REGISTRY_ENTRY_TYPE_ACTOR,
            &a
        ) == HARP_RESULT_OK
    );

    // second bind should fail
    assert(
        harp_registry_bind(
            &r,
            name,
            HARP_REGISTRY_ENTRY_TYPE_HANDLER,
            &b
        ) == HARP_RESULT_NAME_TYPE_MISMATCH
    );

    printf("[OK] duplicate bind rejected\n");

    harp_teardown_registry(&r);
}

static void test_unbind() {
    HarpRegistry r = {0};
    assert(harp_setup_registry(&r) == HARP_RESULT_OK);

    int value = 123;

    HarpName name = harp_registry_name(&r, "foo");
    assert(name != NULL);

    assert(
        harp_registry_bind(
            &r,
            name,
            HARP_REGISTRY_ENTRY_TYPE_HANDLER,
            &value
        ) == HARP_RESULT_OK
    );

    assert(
        harp_registry_unbind(
            &r,
            name,
            HARP_REGISTRY_ENTRY_TYPE_HANDLER
        ) == HARP_RESULT_OK
    );

    void *ptr =
        harp_registry_get_runtime(
            &r,
            name,
            HARP_REGISTRY_ENTRY_TYPE_HANDLER
        );

    assert(ptr == NULL);

    printf("[OK] unbind\n");

    harp_teardown_registry(&r);
}

static void test_wrong_type() {
    HarpRegistry r = {0};
    assert(harp_setup_registry(&r) == HARP_RESULT_OK);

    int value = 999;

    HarpName name = harp_registry_name(&r, "foo");
    assert(name != NULL);

    assert(
        harp_registry_bind(
            &r,
            name,
            HARP_REGISTRY_ENTRY_TYPE_ACTOR,
            &value
        ) == HARP_RESULT_OK
    );

    void *ptr =
        harp_registry_get_runtime(
            &r,
            name,
            HARP_REGISTRY_ENTRY_TYPE_HANDLER
        );

    assert(ptr == NULL);

    printf("[OK] wrong type rejected\n");

    harp_teardown_registry(&r);
}

static void test_many_entries() {
    HarpRegistry r = {0};
    assert(harp_setup_registry(&r) == HARP_RESULT_OK);

    int a = 1;
    int b = 2;
    int c = 3;
    int d = 4;

    HarpName na = harp_registry_name(&r, "a");
    HarpName nb = harp_registry_name(&r, "b");
    HarpName nc = harp_registry_name(&r, "c");
    HarpName nd = harp_registry_name(&r, "d");

    assert(na != NULL);
    assert(nb != NULL);
    assert(nc != NULL);
    assert(nd != NULL);

    assert(
        harp_registry_bind(
            &r,
            na,
            HARP_REGISTRY_ENTRY_TYPE_HANDLER,
            &a
        ) == HARP_RESULT_OK
    );

    assert(
        harp_registry_bind(
            &r,
            nb,
            HARP_REGISTRY_ENTRY_TYPE_HANDLER,
            &b
        ) == HARP_RESULT_OK
    );

    assert(
        harp_registry_bind(
            &r,
            nc,
            HARP_REGISTRY_ENTRY_TYPE_ACTOR,
            &c
        ) == HARP_RESULT_OK
    );

    assert(
        harp_registry_bind(
            &r,
            nd,
            HARP_REGISTRY_ENTRY_TYPE_HANDLER,
            &d
        ) == HARP_RESULT_OK
    );

    assert(
        harp_registry_get_runtime(
            &r,
            "a",
            HARP_REGISTRY_ENTRY_TYPE_HANDLER
        ) == &a
    );

    assert(
        harp_registry_get_runtime(
            &r,
            "b",
            HARP_REGISTRY_ENTRY_TYPE_HANDLER
        ) == &b
    );

    assert(
        harp_registry_get_runtime(
            &r,
            "c",
            HARP_REGISTRY_ENTRY_TYPE_ACTOR
        ) == &c
    );

    assert(
        harp_registry_get_runtime(
            &r,
            "d",
            HARP_REGISTRY_ENTRY_TYPE_HANDLER
        ) == &d
    );

    assert(
        harp_registry_get_runtime(
            &r,
            "z",
            HARP_REGISTRY_ENTRY_TYPE_HANDLER
        ) == NULL
    );

    printf("[OK] many entries\n");

    harp_teardown_registry(&r);
}


int main() {
    test_name_store();
    test_bind_get();
    test_bind_missing_name();
    test_duplicate_bind();
    test_unbind();
    test_wrong_type();
    test_many_entries();

    printf("\nALL TESTS PASSED\n");

    return 0;
}