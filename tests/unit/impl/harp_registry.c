#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <impl/harp_registry.h>

static void setup_registry(HarpRegistry *r) {
    for (uint32_t i = 0; i < HARP_REGISTRY_BUCKET_COUNT; i++) {
        r->buckets[i].capacity = 8;
        r->buckets[i].count = 0;
        r->buckets[i].entries =
            calloc(8, sizeof(HarpRegistryEntry));
        assert(r->buckets[i].entries);
    }
}

static void cleanup_registry(HarpRegistry *r) {
    for (uint32_t i = 0; i < HARP_REGISTRY_BUCKET_COUNT; i++) {
        free(r->buckets[i].entries);
        r->buckets[i].entries = NULL;
    }
}



static void test_insert_find() {
    HarpRegistry r = {0};
    setup_registry(&r);

    HarpRegistryEntry *a = harp_registry_insert(NULL, &r, "foo");
    assert(a != NULL);

    HarpRegistryEntry *b = harp_registry_find(NULL, &r, "foo");
    assert(b == a);

    HarpRegistryEntry *c = harp_registry_find(NULL, &r, "bar");
    assert(c == NULL);

    printf("[OK] insert + find\n");

    cleanup_registry(&r);
}

static void test_duplicate_insert() {
    HarpRegistry r = {0};
    setup_registry(&r);

    HarpRegistryEntry *a = harp_registry_insert(NULL, &r, "foo");
    assert(a != NULL);

    HarpRegistryEntry *b = harp_registry_insert(NULL, &r, "foo");
    assert(b == NULL); // duplicate rejected

    printf("[OK] duplicate insert rejected\n");

    cleanup_registry(&r);
}

static void test_remove_existing() {
    HarpRegistry r = {0};
    setup_registry(&r);

    harp_registry_insert(NULL, &r, "foo");

    harp_registry_remove(NULL, &r, "foo");

    HarpRegistryEntry *f = harp_registry_find(NULL, &r, "foo");
    assert(f == NULL);

    printf("[OK] remove existing\n");

    cleanup_registry(&r);
}

static void test_remove_missing() {
    HarpRegistry r = {0};
    setup_registry(&r);

    // should not crash
    harp_registry_remove(NULL, &r, "does_not_exist");

    printf("[OK] remove missing safe\n");

    cleanup_registry(&r);
}

static void test_multiple_entries() {
    HarpRegistry r = {0};
    setup_registry(&r);

    harp_registry_insert(NULL, &r, "a");
    harp_registry_insert(NULL, &r, "b");
    harp_registry_insert(NULL, &r, "c");
    harp_registry_insert(NULL, &r, "d");

    assert(harp_registry_find(NULL, &r, "a"));
    assert(harp_registry_find(NULL, &r, "b"));
    assert(harp_registry_find(NULL, &r, "c"));
    assert(harp_registry_find(NULL, &r, "d"));

    assert(!harp_registry_find(NULL, &r, "z"));

    printf("[OK] multiple inserts + finds\n");

    cleanup_registry(&r);
}

int main() {
    test_insert_find();
    test_duplicate_insert();
    test_remove_existing();
    test_remove_missing();
    test_multiple_entries();

    printf("\nALL TESTS PASSED\n");
    return 0;
}