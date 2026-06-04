#include <harp/harp_core.h>
#include "impl/harp_core_api.h"

#include "runtime/harp_runtime.h"
#include "runtime/harp_registry.h"

#define HMEM_IMPLEMENTATION
#include <hmem/hmem_arena.h>
#include <hmem/hmem_block.h>
#include <hmem/hmem_book.h>
#include <hmem/hmem_os.h>

#undef HARP_UTILS_UNDEF
#include <harp/utils/harp_api.h>
#include <harp/utils/harp_version.h>

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#if defined(_WIN32)

#include <windows.h>

#define HARP_SHARED_LIBRARY_EXT ".dll"

#else

#include <dirent.h>
#include <dlfcn.h>
#include <limits.h>

#if defined(__APPLE__)
    #define HARP_SHARED_LIBRARY_EXT ".dylib"
#else
    #define HARP_SHARED_LIBRARY_EXT ".so"
#endif

#endif


typedef HarpResult (*HarpPackageQueryFn)(
    HarpPackageDesc **out_desc
);


typedef struct HarpPendingPackage {
    HarpPackageDesc *desc;

    char *directory;
    char *library_path;

    void *library_handle;

    uint8_t registered;
} HarpPendingPackage;


/* ================================================================================ */
/*  INITIALIZATION                                                                  */
/* ================================================================================ */

HarpVersion harp_version(void) {
    return HARP_MAKE_VERSION(1, 0, 1);
}

HarpResult harp_initialize(
    const HarpCreatorBase *creator,
    HarpRuntime **out_runtime
) {
    if(out_runtime == NULL)
        return HARP_RESULT_MISSING_OUTPUT;

    *out_runtime = NULL;

    HarpRuntimeCreator *runtime_creator = (HarpRuntimeCreator *)creator;
    HarpRuntime *runtime = malloc(sizeof(HarpRuntime));

    if(runtime == NULL)
        return HARP_RESULT_FAILED;

    if(harp_setup_runtime(runtime, runtime_creator) != HARP_RESULT_OK)
        goto fail_runtime;

    /* register core api */
    HarpApiDesc core_api_desc = {
        .name = HARP_CORE_API_NAME,
        .version = HARP_CORE_API_VERSION,
        .instance_size = sizeof(HarpCoreApiImpl),
        .instance_alignment = alignof(HarpCoreApiImpl)
    };
    HarpApiBase *core_api_base = NULL;

    if(runtime_register_api(
        runtime,
        &core_api_desc,
        &core_api_base
    ) != HARP_RESULT_OK)
        goto fail_setup;

    HarpCoreApiImpl *core_api = (HarpCoreApiImpl *)core_api_base;
    core_api->p_runtime = runtime;

    /* registration */
    core_api->core_api.register_api = core_register_api;
    core_api->core_api.register_handler = core_register_handler;
    core_api->core_api.register_actor = core_register_actor;

    /* retrieval */
    core_api->core_api.get_api = core_get_api;
    core_api->core_api.get_handler = core_get_handler;

    core_api->core_api.get_api_desc = core_get_api_desc;
    core_api->core_api.get_handler_desc = core_get_handler_desc;
    core_api->core_api.get_actor_desc = core_get_actor_desc;

    /* lifecycle */
    core_api->core_api.handler_initialize = core_handler_initialize;
    core_api->core_api.handler_terminate = core_handler_terminate;
    core_api->core_api.actor_create = core_actor_create;
    core_api->core_api.actor_destroy = core_actor_destroy;

    /* paths */
    core_api->core_api.get_executable_directory = core_get_executable_directory;
    core_api->core_api.get_working_directory = core_get_working_directory;
    core_api->core_api.get_package_directory = core_get_package_directory;

    core_api_base->available = 1;

    runtime->core_api = (HarpCoreApi *)core_api;

    *out_runtime = runtime;

    return HARP_RESULT_OK;

fail_setup:
    harp_teardown_runtime(runtime);

fail_runtime:
    free(runtime);

    return HARP_RESULT_FAILED;
}

HarpResult harp_terminate(
    HarpRuntime *runtime
) {
    if(runtime == NULL)
        return HARP_RESULT_INVALID_ARGUMENTS;

    harp_teardown_runtime(runtime);

    free(runtime);

    return HARP_RESULT_OK;
}


/* ================================================================================ */
/*  RUNTIME                                                                         */
/* ================================================================================ */

HarpResult harp_runtime_get_api(
    HarpRuntime *runtime,
    const HarpName name,
    HarpApiBase **out_api
) {
    return runtime_get_api(
        runtime,
        name,
        out_api
    );
}

HarpResult harp_runtime_get_handler(
    HarpRuntime *runtime,
    const HarpName name,
    HarpHandlerBase **out_handler
) {
    return runtime_get_handler(
        runtime,
        name,
        out_handler
    );
}


/* ================================================================================ */
/*  PACKAGE HELPERS                                                                 */
/* ================================================================================ */

static HarpPendingPackage *harp_find_pending_package(
    HarpPendingPackage *packages,
    uint64_t count,
    const HarpName name
) {
    if(packages == NULL || name == NULL)
        return NULL;

    for(uint64_t i = 0; i < count; ++i) {

        if(packages[i].desc == NULL)
            continue;

        if(strcmp(packages[i].desc->name, name) == 0)
            return &packages[i];
    }

    return NULL;
}

static HarpResult harp_register_package_recursive(
    HarpRuntime *runtime,
    HarpPendingPackage *packages,
    uint64_t package_count,
    HarpPendingPackage *pkg
) {
    if(runtime == NULL ||
       packages == NULL ||
       pkg == NULL)
        return HARP_RESULT_INVALID_ARGUMENTS;

    if(pkg->registered)
        return HARP_RESULT_OK;

    /* register dependencies first */

    for(uint64_t i = 0;
        i < pkg->desc->dependency_count;
        ++i) {

        HarpDependencyDesc *dep =
            &pkg->desc->p_dependencies[i];

        HarpPendingPackage *dep_pkg =
            harp_find_pending_package(
                packages,
                package_count,
                dep->name
            );

        if(dep_pkg == NULL)
            return HARP_RESULT_DEPENDENCY_NOT_FOUND;

        if(dep_pkg->desc->version <
           dep->min_version)
            return HARP_RESULT_DEPENDENCY_VERSION_MISMATCH;

        if(dep->max_version != 0 &&
           dep_pkg->desc->version >
           dep->max_version)
            return HARP_RESULT_DEPENDENCY_VERSION_MISMATCH;

        HarpResult dep_res =
            harp_register_package_recursive(
                runtime,
                packages,
                package_count,
                dep_pkg
            );

        if(dep_res != HARP_RESULT_OK)
            return dep_res;
    }

    /* register package */

    HarpResult res =
        pkg->desc->pfn_register(
            runtime->core_api
        );

    if(res != HARP_RESULT_OK)
        return res;

    pkg->registered = 1;

    return HARP_RESULT_OK;
}


/* ================================================================================ */
/*  PACKAGE LOADING                                                                 */
/* ================================================================================ */

HarpResult harp_runtime_load_packages(
    HarpRuntime *runtime
) {
    if(runtime == NULL)
        return HARP_RESULT_INVALID_ARGUMENTS;

    return harp_runtime_load_packages_from(
        runtime,
        runtime->working_directory
    );
}

HarpResult harp_runtime_load_packages_from(
    HarpRuntime *runtime,
    const char *path
) {
    if(runtime == NULL || path == NULL)
        return HARP_RESULT_INVALID_ARGUMENTS;

    char packages_path[4096];

#if defined(_WIN32)

    snprintf(
        packages_path,
        sizeof(packages_path),
        "%s\\packages",
        path
    );

    WIN32_FIND_DATAA find_data;

    char search_path[4096];

    snprintf(
        search_path,
        sizeof(search_path),
        "%s\\*",
        packages_path
    );

    HANDLE handle =
        FindFirstFileA(
            search_path,
            &find_data
        );

    if(handle == INVALID_HANDLE_VALUE)
        return HARP_RESULT_NAME_NOT_FOUND;

#else

    snprintf(
        packages_path,
        sizeof(packages_path),
        "%s/packages",
        path
    );

    DIR *dir =
        opendir(packages_path);

    if(dir == NULL)
        return HARP_RESULT_NAME_NOT_FOUND;

#endif

    HarpPendingPackage *packages = NULL;
    uint64_t package_count = 0;

#if defined(_WIN32)

    do {

        if(!(find_data.dwFileAttributes &
             FILE_ATTRIBUTE_DIRECTORY))
            continue;

        if(strcmp(find_data.cFileName, ".") == 0 ||
           strcmp(find_data.cFileName, "..") == 0)
            continue;

        char package_dir[4096];

        snprintf(
            package_dir,
            sizeof(package_dir),
            "%s\\%s",
            packages_path,
            find_data.cFileName
        );

        char lib_path[4096];

        snprintf(
            lib_path,
            sizeof(lib_path),
            "%s\\%s%s",
            package_dir,
            find_data.cFileName,
            HARP_SHARED_LIBRARY_EXT
        );

        HMODULE library =
            LoadLibraryA(lib_path);

        if(library == NULL)
            continue;

        HarpPackageQueryFn query =
            (HarpPackageQueryFn)GetProcAddress(
                library,
                "harp_package_query"
            );

        if(query == NULL) {
            FreeLibrary(library);
            continue;
        }

#else

    struct dirent *entry = NULL;

    while((entry = readdir(dir)) != NULL) {

        if(entry->d_type != DT_DIR)
            continue;

        if(strcmp(entry->d_name, ".") == 0 ||
           strcmp(entry->d_name, "..") == 0)
            continue;

        char package_dir[4096];

        snprintf(
            package_dir,
            sizeof(package_dir),
            "%s/%s",
            packages_path,
            entry->d_name
        );

        char lib_path[4096];

        snprintf(
            lib_path,
            sizeof(lib_path),
            "%s/%s%s",
            package_dir,
            entry->d_name,
            HARP_SHARED_LIBRARY_EXT
        );

        void *library =
            dlopen(
                lib_path,
                RTLD_NOW
            );

        if(library == NULL)
            continue;

        HarpPackageQueryFn query =
            (HarpPackageQueryFn)dlsym(
                library,
                "harp_package_query"
            );

        if(query == NULL) {
            dlclose(library);
            continue;
        }

#endif

        HarpPackageDesc *desc = NULL;

        if(query(&desc) != HARP_RESULT_OK ||
           desc == NULL) {

#if defined(_WIN32)
            FreeLibrary(library);
#else
            dlclose(library);
#endif

            continue;
        }

        HarpPendingPackage *new_packages =
            realloc(
                packages,
                sizeof(HarpPendingPackage) *
                (package_count + 1)
            );

        if(new_packages == NULL) {

#if defined(_WIN32)
            FreeLibrary(library);
#else
            dlclose(library);
#endif

            continue;
        }

        packages = new_packages;

        HarpPendingPackage *pkg =
            &packages[package_count++];

        memset(pkg, 0, sizeof(*pkg));

        pkg->desc = desc;
        pkg->library_handle = library;

        /* create runtime desc */

        HarpPackageRuntimeDesc *rdesc =
            harp_alloc_global(
                runtime,
                sizeof(HarpPackageRuntimeDesc),
                alignof(HarpPackageRuntimeDesc)
            );

        if(rdesc == NULL)
            continue;

        memset(rdesc, 0, sizeof(*rdesc));

        rdesc->_base.name =
            harp_registry_name(
                &runtime->registry,
                desc->name
            );

        rdesc->_base.version =
            desc->version;

        rdesc->_base.pfn_register =
            desc->pfn_register;

        /* copy dependencies */

        if(desc->dependency_count > 0) {

            HarpDependencyDesc *deps =
                harp_alloc_global(
                    runtime,
                    sizeof(HarpDependencyDesc) *
                    desc->dependency_count,
                    alignof(HarpDependencyDesc)
                );

            if(deps == NULL)
                continue;

            memcpy(
                deps,
                desc->p_dependencies,
                sizeof(HarpDependencyDesc) *
                desc->dependency_count
            );

            for(uint64_t i = 0;
                i < desc->dependency_count;
                ++i) {

                deps[i].name =
                    harp_registry_name(
                        &runtime->registry,
                        deps[i].name
                    );
            }

            rdesc->_base.p_dependencies = deps;
            rdesc->_base.dependency_count =
                desc->dependency_count;
        }

        /* copy package directory */

        size_t dir_len =
            strlen(package_dir) + 1;

        char *dir_copy =
            harp_alloc_global(
                runtime,
                dir_len,
                alignof(char)
            );

        if(dir_copy == NULL)
            continue;

        memcpy(
            dir_copy,
            package_dir,
            dir_len
        );

        rdesc->directory = dir_copy;

        /* bind package */

        HarpResult bind_res =
            harp_registry_bind(
                &runtime->registry,
                rdesc->_base.name,
                HARP_REGISTRY_ENTRY_TYPE_PACKAGE,
                rdesc
            );

        if(bind_res != HARP_RESULT_OK)
            continue;

#if defined(_WIN32)

    } while(FindNextFileA(handle, &find_data));

    FindClose(handle);

#else

    }

    closedir(dir);

#endif

    /* validate dependencies */

    for(uint64_t i = 0; i < package_count; ++i) {

        HarpPendingPackage *pkg =
            &packages[i];

        for(uint64_t j = 0;
            j < pkg->desc->dependency_count;
            ++j) {

            HarpDependencyDesc *dep =
                &pkg->desc->p_dependencies[j];

            HarpPendingPackage *dep_pkg =
                harp_find_pending_package(
                    packages,
                    package_count,
                    dep->name
                );

            if(dep_pkg == NULL) {
                free(packages);
                return HARP_RESULT_DEPENDENCY_NOT_FOUND;
            }

            if(dep_pkg->desc->version <
               dep->min_version) {
                free(packages);
                return HARP_RESULT_DEPENDENCY_VERSION_MISMATCH;
            }

            if(dep->max_version != 0 &&
               dep_pkg->desc->version >
               dep->max_version) {
                free(packages);
                return HARP_RESULT_DEPENDENCY_VERSION_MISMATCH;
            }
        }
    }

    /* dependency ordered registration */

    for(uint64_t i = 0; i < package_count; ++i) {

        HarpResult res =
            harp_register_package_recursive(
                runtime,
                packages,
                package_count,
                &packages[i]
            );

        if(res != HARP_RESULT_OK) {
            free(packages);
            return res;
        }
    }

    free(packages);

    return HARP_RESULT_OK;
}