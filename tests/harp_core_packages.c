#include <assert.h>
#include <stdio.h>

#include <harp/harp_core.h>
#include <harp/utils/harp_api.h>

#include <test_package/test_api.h>


int main(
    int argc,
    char **argv
) {
    printf("=== HARP PACKAGE TEST ===\n");

    HarpRuntimeCreator creator = {
        .argv0 = argv[0]
    };

    HarpRuntime *runtime = NULL;

    HarpResult res =
        harp_initialize(
            (const HarpCreatorBase *)&creator,
            &runtime
        );

    assert(res == HARP_RESULT_OK);

    printf("[OK] runtime initialize\n");

    /* get core api */

    HarpApiBase *core_base = NULL;
    HarpDependencyDesc dep_desc = {
        .name = HARP_CORE_API_NAME,
        .min_version = 0,
        .max_version = UINT32_MAX
    };

    res =
        harp_runtime_get_api(
            runtime,
            &dep_desc,
            &core_base
        );

    assert(res == HARP_RESULT_OK);
    assert(core_base != NULL);

    HarpCoreApi *core =
        HARP_API_AS(HarpCoreApi, core_base);

    printf("[OK] get core api\n");

    /* load packages */

    res =
        harp_runtime_load_packages(
            runtime
        );

    assert(res == HARP_RESULT_OK);

    printf("[OK] package loading\n");

    /* get package directory */

    const char *package_dir = NULL;

    res =
        core->get_package_directory(
            core,
            "test_package",
            &package_dir
        );

    assert(res == HARP_RESULT_OK);
    assert(package_dir != NULL);

    printf("[OK] get package directory\n");
    printf("package path: %s\n", package_dir);

    /* get test api */

    HarpApiBase *api_base = NULL;
    dep_desc.name = TEST_API_NAME;

    res =
        harp_runtime_get_api(
            runtime,
            &dep_desc,
            &api_base
        );

    assert(res == HARP_RESULT_OK);
    assert(api_base != NULL);

    printf("[OK] get package api\n");

    TestApi *api =
        HARP_API_AS(TestApi, api_base);

    int value =
        api->add(api, 2, 3);

    assert(value == 5);

    printf("[OK] api function call\n");

    /* get handler */

    HarpHandlerBase *handler = NULL;
    dep_desc.name = "test_handler";

    res =
        harp_runtime_get_handler(
            runtime,
            &dep_desc,
            &handler
        );

    assert(res == HARP_RESULT_OK);
    assert(handler != NULL);

    printf("[OK] get package handler\n");

    /* initialize handler */

    HarpCreatorBase empty_creator = {0};

    res =
        core->handler_initialize(
            core,
            "test_handler",
            &empty_creator
        );

    assert(res == HARP_RESULT_OK);

    printf("[OK] handler initialize\n");

    harp_terminate(runtime);

    printf("[OK] runtime terminate\n");

    printf("\nALL TESTS PASSED\n");

    return 0;
}