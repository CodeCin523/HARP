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
    /* GET CORE HANDLER (NEW MODEL)                                          */
    /* ===================================================================== */

    HarpHandlerBase *core_base = NULL;

    HarpDependencyDesc dep_desc = {
        .name = HARP_CORE_HANDLER_NAME,
        .min_version = 0,
        .max_version = UINT32_MAX
    };

    res =
        harp_runtime_get_handler(
            runtime,
            &dep_desc,
            &core_base
        );

    assert(res == HARP_RESULT_OK);
    assert(core_base != NULL);

    HarpCoreHandler *core = (HarpCoreHandler *)core_base;

    printf("[OK] get core handler\n");

    /* ===================================================================== */
    /* GET EXECUTABLE DIRECTORY                                              */
    /* ===================================================================== */

    const char *exe_dir = NULL;

    res =
        core->get_executable_directory(
            core,
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
        core->get_working_directory(
            core,
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