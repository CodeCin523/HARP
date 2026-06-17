#include <harp/harp_core.h>
#include "impl/harp_core_handler.h"
#include "impl/harp_extended_handler.h"

#include "runtime/harp_runtime.h"
#include "runtime/harp_registry.h"

#define HMEM_IMPLEMENTATION
#include <hmem/hmem_arena.h>
#include <hmem/hmem_block.h>
#include <hmem/hmem_book.h>
#include <hmem/hmem_os.h>

#include <harp/utils/harp_api.h>
#include <harp/utils/harp_platform.h>
#include <harp/utils/harp_version.h>

#if HARP_PLATFORM_UNKNOWN
    #warning Unknown platform
#endif
#if HARP_ARCH_UNKNOWN || HARP_ARCH_X86 || HARP_ARCH_ARM32
    #warning Unknown or unsupported architecture
#endif

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#if HARP_PLATFORM_WINDOWS

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


static HarpResult init_core_handler(HarpCoreHandler *core_handler, HarpHandlerBase *base, HarpCreatorBase *creator) {
    return HARP_RESULT_OK;
}
static HarpResult term_core_handler(HarpCoreHandler *core_handler, HarpHandlerBase *base) {
    HarpCoreHandlerImpl *impl = (HarpCoreHandlerImpl *)base;
    if(impl->p_runtime->core_handler != NULL)
        return HARP_RESULT_INVALID_STATE;

    return HARP_RESULT_OK;
}


/* ================================================================================ */
/*  INITIALIZATION                                                                  */
/* ================================================================================ */

HarpVersion harp_version(void) {
    return HARP_MAKE_VERSION(1, 1, 0);
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

    
    /* Core Handler */
    HarpHandlerDesc core_handler_desc = {
        .name = HARP_CORE_HANDLER_NAME,
        .version = HARP_CORE_HANDLER_VERSION,
        .instance_size = sizeof(HarpCoreHandlerImpl),
        .instance_alignment = alignof(HarpCoreHandlerImpl),
        .pfn_init = init_core_handler,
        .pfn_term = term_core_handler,
        .p_dependencies = NULL,
        .dependency_count = 0
    };
    HarpHandlerBase *core_handler_base = NULL;

    if(runtime_register_handler(runtime, &core_handler_desc) != HARP_RESULT_OK)
        goto fail_setup;
    if(runtime_get_handler(
        runtime,
        &(HarpDependencyDesc) {HARP_CORE_HANDLER_NAME, HARP_CORE_HANDLER_VERSION, UINT32_MAX},
        &core_handler_base
    ) != HARP_RESULT_OK)
        goto fail_setup;

    HarpCoreHandlerImpl *core_handler = (HarpCoreHandlerImpl *)core_handler_base;
    core_handler->p_runtime = runtime;
    runtime->core_handler = (HarpCoreHandler *)core_handler;

    /* registration */
    core_handler->interface.register_handler = handler_register_handler;
    core_handler->interface.register_actor = handler_register_actor;

    /* retrieval */
    core_handler->interface.get_handler = handler_get_handler;

    core_handler->interface.get_actor_count = handler_get_actor_count;
    core_handler->interface.get_actor_at = handler_get_actor_at;
    core_handler->interface.get_actors = handler_get_actors;

    core_handler->interface.get_handler_desc = handler_get_handler_desc;
    core_handler->interface.get_actor_desc = handler_get_actor_desc;

    /* lifecycle */
    core_handler->interface.handler_initialize = handler_handler_initialize;
    core_handler->interface.handler_terminate = handler_handler_terminate;
    core_handler->interface.actor_create = handler_actor_create;
    core_handler->interface.actor_destroy = handler_actor_destroy;

    /* paths */
    core_handler->interface.get_executable_directory = handler_get_executable_directory;
    core_handler->interface.get_working_directory = handler_get_working_directory;
    core_handler->interface.get_package_directory = handler_get_package_directory;

    /* status switch */
    core_handler->interface.handler_set_serving = handler_handler_set_serving;
    core_handler->interface.handler_set_failed = handler_handler_set_failed;
    core_handler->interface.actor_set_serving = handler_actor_set_serving;
    core_handler->interface.actor_set_failed = handler_actor_set_failed;

    handler_handler_set_serving(&core_handler->interface, core_handler_base, 1);

    if(runtime_handler_initialize(
        runtime,
        HARP_CORE_HANDLER_NAME,
        &(HarpCreatorBase) {.kind = 0, .flags = HARP_CREATOR_FLAG_DEFAULT_CREATOR}
    ) != HARP_RESULT_OK)
        goto fail_setup;


    /* Extended Handler */
    HarpHandlerDesc extended_handler_desc = {
        .name = HARP_EXTENDED_HANDLER_NAME,
        .version = HARP_EXTENDED_HANDLER_VERSION,
        .instance_size = sizeof(HarpExtendedHandlerImpl),
        .instance_alignment = alignof(HarpExtendedHandlerImpl),
        .pfn_init = init_extended,
        .pfn_term = term_extended,
        .p_dependencies = NULL,
        .dependency_count = 0
    };

    HarpHandlerBase *extended_handler_base = NULL;

    if(runtime_register_handler(runtime, &extended_handler_desc) != HARP_RESULT_OK)
        goto fail_setup;

    if(runtime_get_handler(
        runtime,
        &(HarpDependencyDesc){HARP_EXTENDED_HANDLER_NAME, HARP_EXTENDED_HANDLER_VERSION, UINT32_MAX},
        &extended_handler_base
    ) != HARP_RESULT_OK)
        goto fail_setup;
    HarpExtendedHandlerImpl *extended_handler = (HarpExtendedHandlerImpl *)extended_handler_base;
    runtime->extended_handler = (HarpExtendedHandler *)extended_handler;

    /* setup interface */
    extended_handler->interface.get_uptime_s = extended_get_uptime_s;
    extended_handler->interface.get_uptime_ms = extended_get_uptime_ms;
    extended_handler->interface.get_uptime_ns = extended_get_uptime_ns;

    handler_handler_set_serving(&core_handler->interface, extended_handler_base, 1);

    if(runtime_handler_initialize(
        runtime,
        HARP_EXTENDED_HANDLER_NAME,
        &(HarpCreatorBase){
            .kind = 0,
            .flags = HARP_CREATOR_FLAG_DEFAULT_CREATOR
        }
    ) != HARP_RESULT_OK)
        goto fail_setup;

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

    HarpCoreHandler *tmp = runtime->core_handler;
    runtime->core_handler = NULL;
    if(runtime_handler_terminate(runtime, HARP_CORE_HANDLER_NAME) != HARP_RESULT_OK) {
        runtime->core_handler = tmp;
        return HARP_RESULT_INVALID_STATE;
    }

    harp_teardown_runtime(runtime);

    free(runtime);

    return HARP_RESULT_OK;
}


/* ================================================================================ */
/*  RUNTIME                                                                         */
/* ================================================================================ */

HarpResult harp_runtime_get_handler(
    HarpRuntime *runtime,
    const HarpDependencyDesc *dependency,
    HarpHandlerBase **out_handler
) {
    return runtime_get_handler(
        runtime,
        dependency,
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
            runtime->core_handler
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

#if HARP_PLATFORM_WINDOWS

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

#if HARP_PLATFORM_WINDOWS

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

#if HARP_PLATFORM_WINDOWS
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

#if HARP_PLATFORM_WINDOWS
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

        HarpRuntimePackage *rdesc =
            harp_alloc_global(
                runtime,
                sizeof(HarpRuntimePackage),
                alignof(HarpRuntimePackage)
            );

        if(rdesc == NULL)
            continue;

        memset(rdesc, 0, sizeof(*rdesc));

        rdesc->descriptor.name =
            harp_registry_name(
                &runtime->registry,
                desc->name
            );

        rdesc->descriptor.version =
            desc->version;

        rdesc->descriptor.pfn_register =
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

            rdesc->descriptor.p_dependencies = deps;
            rdesc->descriptor.dependency_count =
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
                rdesc->descriptor.name,
                HARP_REGISTRY_ENTRY_TYPE_PACKAGE,
                rdesc
            );

        if(bind_res != HARP_RESULT_OK)
            continue;

#if HARP_PLATFORM_WINDOWS

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