#include <assert.h>
#include <stdio.h>

#include <harp/harp_core.h>


int main(int argc, char **argv) {
    printf("=== HARP PATH TEST ===\n");

    /* ===================================================================== */
    /* CREATE RUNTIME                                                        */
    /* ===================================================================== */

    HarpRuntime *runtime = NULL;

    HarpRuntimeCreator creator = {
        .argv0 = argv[0]
    };

    HarpResult res =
        harp_initialize((HarpCreatorBase *)&creator, &runtime);

    assert(res == HARP_RESULT_OK);
    assert(runtime != NULL);

    printf("[OK] runtime initialize\n");

    /* ===================================================================== */
    /* GET CORE API                                                          */
    /* ===================================================================== */

    HarpApiBase *core_base = NULL;
    HarpDependencyDesc dep_desc = {
        HARP_CORE_API_NAME,
        0,
        UINT32_MAX
    };

    res =
        harp_runtime_get_api(
            runtime,
            &dep_desc,
            &core_base
        );

    assert(res == HARP_RESULT_OK);
    assert(core_base != NULL);

    HarpCoreApi *core_api = (HarpCoreApi *)core_base;

    printf("[OK] get core api\n");

    /* ===================================================================== */
    /* GET EXECUTABLE DIRECTORY                                              */
    /* ===================================================================== */

    const char *exe_dir = NULL;

    res =
        core_api->get_executable_directory(
            core_api,
            &exe_dir
        );

    assert(res == HARP_RESULT_OK);
    assert(exe_dir != NULL);

    printf("[OK] executable directory\n");
    printf("Executable Directory: %s\n", exe_dir);

    /* ===================================================================== */
    /* GET WORKING DIRECTORY                                                 */
    /* ===================================================================== */

    const char *work_dir = NULL;

    res =
        core_api->get_working_directory(
            core_api,
            &work_dir
        );

    assert(res == HARP_RESULT_OK);
    assert(work_dir != NULL);

    printf("[OK] working directory\n");
    printf("Working Directory: %s\n", work_dir);

    /* ===================================================================== */
    /* TERMINATE                                                             */
    /* ===================================================================== */

    res = harp_terminate(runtime);

    assert(res == HARP_RESULT_OK);

    printf("[OK] runtime terminate\n");

    printf("\nALL TESTS PASSED\n");

    return 0;
}