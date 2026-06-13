#include <assert.h>
#include <stdio.h>

#include <harp/harp_core.h>
#include <test_package/test_api.h>

int main(int argc, char **argv) {
    printf("=== HARP PACKAGE TEST ===\n");

    /* ===================================================================== */
    /* INIT RUNTIME                                                          */
    /* ===================================================================== */

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
    assert(runtime != NULL);

    printf("[OK] runtime initialize\n");

    /* ===================================================================== */
    /* LOAD PACKAGES                                                        */
    /* ===================================================================== */

    res = harp_runtime_load_packages(runtime);
    assert(res == HARP_RESULT_OK);

    printf("[OK] package loading\n");

    /* ===================================================================== */
    /* GET CORE HANDLER                                                     */
    /* ===================================================================== */

    HarpDependencyDesc core_dep = {
        .name = HARP_CORE_HANDLER_NAME,
        .min_version = 0,
        .max_version = UINT32_MAX
    };

    HarpHandlerBase *core_base = NULL;

    res =
        harp_runtime_get_handler(
            runtime,
            &core_dep,
            &core_base
        );

    assert(res == HARP_RESULT_OK);
    assert(core_base != NULL);

    HarpCoreHandler *core = (HarpCoreHandler *)core_base;

    printf("[OK] get core handler\n");

    /* ===================================================================== */
    /* GET PACKAGE DIRECTORY                                                */
    /* ===================================================================== */
    const char *package_dir = NULL;

    res = core->get_package_directory(
        core,
        "test_package",
        &package_dir
    );

    assert(res == HARP_RESULT_OK);
    assert(package_dir != NULL);

    printf("[OK] get package directory: %s\n", package_dir);

    /* ===================================================================== */
    /* GET PACKAGE HANDLER (test_api handler)                               */
    /* ===================================================================== */

    HarpDependencyDesc dep = {
        .name = TEST_API_NAME,
        .min_version = 0,
        .max_version = UINT32_MAX
    };

    HarpHandlerBase *api_base = NULL;

    res =
        harp_runtime_get_handler(
            runtime,
            &dep,
            &api_base
        );

    assert(res == HARP_RESULT_OK);
    assert(api_base != NULL);

    TestApi *api = (TestApi *)api_base;

    printf("[OK] get test handler\n");

    /* ===================================================================== */
    /* INITIALIZE HANDLER                                                   */
    /* ===================================================================== */

    HarpCreatorBase empty_creator = {0};

    res =
        core->handler_initialize(
            core,
            TEST_API_NAME,
            &empty_creator
        );

    assert(res == HARP_RESULT_OK);

    printf("[OK] handler initialize\n");

    /* ===================================================================== */
    /* CALL FUNCTION                                                        */
    /* ===================================================================== */

    int value = api->add(api, 2, 3);

    assert(value == 5);

    printf("[OK] api function call\n");

    /* ===================================================================== */
    /* TERMINATE                                                            */
    /* ===================================================================== */

    res = harp_terminate(runtime);
    assert(res == HARP_RESULT_OK);

    printf("[OK] runtime terminate\n");

    printf("\nALL TESTS PASSED\n");

    return 0;
}