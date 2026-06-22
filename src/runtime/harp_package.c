#include "harp_package.h"

#include <harp/utils/harp_platform.h>

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
#include <sys/stat.h>

#if defined(__APPLE__)
    #define HARP_SHARED_LIBRARY_EXT ".dylib"
#else
    #define HARP_SHARED_LIBRARY_EXT ".so"
#endif

#endif


typedef HarpResult (*HarpPackageQueryFn)(HarpPackageDesc **out_desc);

typedef struct {
    HarpPackageDesc *desc;
    void            *library_handle;
    char             directory[4096]; // empty string for direct .so packages
    uint8_t          registered;
    uint8_t          visiting;
} PendingPackage;


/* ================================================================================ */
/*  SETUP / TEARDOWN                                                                */
/* ================================================================================ */

HarpResult harp_setup_package_manager(HarpPackageManager *manager) {
    if(manager == NULL)
        return HARP_RESULT_INVALID_ARGUMENTS;

    manager->packages = NULL;
    manager->count    = 0;
    manager->capacity = 0;

    return HARP_RESULT_OK;
}
void harp_teardown_package_manager(HarpPackageManager *manager) {
    if(manager == NULL)
        return;

    for(uint64_t i = manager->count; i > 0; --i) {
        HarpLoadedPackage *pkg = &manager->packages[i - 1];

        if(pkg->library_handle != NULL) {
#if HARP_PLATFORM_WINDOWS
            FreeLibrary((HMODULE)pkg->library_handle);
#else
            dlclose(pkg->library_handle);
#endif
        }

        free(pkg->directory);
    }

    free(manager->packages);
    manager->packages = NULL;
    manager->count    = 0;
    manager->capacity = 0;
}


/* ================================================================================ */
/*  ADD && GET                                                                      */
/* ================================================================================ */

HarpResult harp_package_manager_add(
    HarpPackageManager *manager,
    HarpName            name,
    HarpVersion         version,
    void               *library_handle,
    const char         *directory
) {
    if(manager == NULL || name == NULL)
        return HARP_RESULT_INVALID_ARGUMENTS;

    if(manager->count >= manager->capacity) {
        uint64_t new_capacity = manager->capacity == 0 ? 8 : manager->capacity * 2;

        HarpLoadedPackage *new_packages = realloc(
            manager->packages,
            sizeof(HarpLoadedPackage) * new_capacity
        );

        if(new_packages == NULL)
            return HARP_RESULT_OUT_OF_MEMORY;

        manager->packages = new_packages;
        manager->capacity = new_capacity;
    }

    HarpLoadedPackage *pkg = &manager->packages[manager->count];
    pkg->name           = name;
    pkg->version        = version;
    pkg->library_handle = library_handle;
    pkg->directory      = NULL;

    if(directory != NULL) {
        pkg->directory = strdup(directory);
        if(pkg->directory == NULL)
            return HARP_RESULT_OUT_OF_MEMORY;
    }

    ++manager->count;
    return HARP_RESULT_OK;
}

HarpLoadedPackage *harp_package_manager_get(HarpPackageManager *manager, HarpName name) {
    if(manager == NULL || name == NULL)
        return NULL;

    for(uint64_t i = 0; i < manager->count; ++i) {
        if(strcmp(manager->packages[i].name, name) == 0)
            return &manager->packages[i];
    }

    return NULL;
}

HarpLoadedPackage *harp_package_manager_get_at(HarpPackageManager *manager, uint64_t index) {
    if(manager == NULL || index >= manager->count)
        return NULL;

    return &manager->packages[index];
}


/* ================================================================================ */
/*  LOADING                                                                          */
/* ================================================================================ */

static PendingPackage *find_pending(
    PendingPackage *packages,
    uint64_t        count,
    HarpName        name
) {
    for(uint64_t i = 0; i < count; ++i) {
        if(packages[i].desc != NULL && strcmp(packages[i].desc->name, name) == 0)
            return &packages[i];
    }
    return NULL;
}

static HarpResult register_recursive(
    HarpPackageManager *manager,
    HarpRegistry       *registry,
    HarpCoreHandler    *core_handler,
    PendingPackage     *packages,
    uint64_t            count,
    PendingPackage     *pkg
) {
    if(pkg->registered)
        return HARP_RESULT_OK;

    if(pkg->visiting)
        return HARP_RESULT_DEPENDENCY_CYCLE;

    pkg->visiting = 1;

    for(uint64_t i = 0; i < pkg->desc->dependency_count; ++i) {
        HarpDependencyDesc *dep     = &pkg->desc->p_dependencies[i];
        PendingPackage     *dep_pkg = find_pending(packages, count, dep->name);

        if(dep_pkg == NULL)
            return HARP_RESULT_DEPENDENCY_NOT_FOUND;

        if(dep_pkg->desc->version < dep->min_version)
            return HARP_RESULT_DEPENDENCY_VERSION_MISMATCH;

        if(dep->max_version != 0 && dep_pkg->desc->version > dep->max_version)
            return HARP_RESULT_DEPENDENCY_VERSION_MISMATCH;

        HarpResult res = register_recursive(manager, registry, core_handler, packages, count, dep_pkg);
        if(res != HARP_RESULT_OK)
            return res;
    }

    HarpResult res = pkg->desc->pfn_register(core_handler);
    if(res != HARP_RESULT_OK)
        return res;

    HarpName interned = harp_registry_name(registry, pkg->desc->name);
    if(interned == NULL)
        return HARP_RESULT_OUT_OF_MEMORY;

    const char *directory = pkg->directory[0] != '\0' ? pkg->directory : NULL;

    res = harp_package_manager_add(manager, interned, pkg->desc->version, pkg->library_handle, directory);
    if(res != HARP_RESULT_OK)
        return res;

    pkg->registered = 1;
    pkg->visiting   = 0;
    return HARP_RESULT_OK;
}

static void close_unregistered(PendingPackage *packages, uint64_t count) {
    for(uint64_t i = 0; i < count; ++i) {
        if(packages[i].registered || packages[i].library_handle == NULL)
            continue;
#if HARP_PLATFORM_WINDOWS
        FreeLibrary((HMODULE)packages[i].library_handle);
#else
        dlclose(packages[i].library_handle);
#endif
    }
}

HarpResult harp_package_manager_load(
    HarpPackageManager *manager,
    HarpRegistry       *registry,
    HarpCoreHandler    *core_handler,
    const char         *path
) {
    if(manager == NULL || registry == NULL || core_handler == NULL || path == NULL)
        return HARP_RESULT_INVALID_ARGUMENTS;

    char packages_path[4096];

    PendingPackage *packages = NULL;
    uint64_t        count    = 0;
    uint64_t        capacity = 0;

#if HARP_PLATFORM_WINDOWS

    snprintf(packages_path, sizeof(packages_path), "%s\\packages", path);

    char search_path[4096];
    snprintf(search_path, sizeof(search_path), "%s\\*", packages_path);

    WIN32_FIND_DATAA find_data;
    HANDLE handle = FindFirstFileA(search_path, &find_data);

    if(handle == INVALID_HANDLE_VALUE)
        return HARP_RESULT_NAME_NOT_FOUND;

    do {
        if(strcmp(find_data.cFileName, ".") == 0 || strcmp(find_data.cFileName, "..") == 0)
            continue;

        char lib_path[4096];
        char entry_dir[4096];
        int  has_dir = 0;

        if(find_data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
            snprintf(entry_dir, sizeof(entry_dir), "%s\\%s", packages_path, find_data.cFileName);
            snprintf(lib_path,  sizeof(lib_path),  "%s\\%s" HARP_SHARED_LIBRARY_EXT, entry_dir, find_data.cFileName);
            has_dir = 1;
        } else {
            size_t name_len = strlen(find_data.cFileName);
            size_t ext_len  = strlen(HARP_SHARED_LIBRARY_EXT);
            if(name_len <= ext_len)
                continue;
            if(strcmp(find_data.cFileName + name_len - ext_len, HARP_SHARED_LIBRARY_EXT) != 0)
                continue;
            snprintf(lib_path, sizeof(lib_path), "%s\\%s", packages_path, find_data.cFileName);
        }

        HMODULE library = LoadLibraryA(lib_path);
        if(library == NULL)
            continue;

        HarpPackageQueryFn query = (HarpPackageQueryFn)GetProcAddress(library, "harp_package_query");
        if(query == NULL) { FreeLibrary(library); continue; }

        HarpPackageDesc *desc = NULL;
        if(query(&desc) != HARP_RESULT_OK || desc == NULL) { FreeLibrary(library); continue; }

#else

    snprintf(packages_path, sizeof(packages_path), "%s/packages", path);

    DIR *dir = opendir(packages_path);
    if(dir == NULL)
        return HARP_RESULT_NAME_NOT_FOUND;

    struct dirent *entry;
    while((entry = readdir(dir)) != NULL) {
        if(strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
            continue;

        char lib_path[4096];
        char entry_dir[4096];
        int  has_dir = 0;

        uint8_t is_dir  = 0;
        uint8_t is_file = 0;

        if(entry->d_type == DT_DIR) {
            is_dir = 1;
        } else if(entry->d_type == DT_REG) {
            is_file = 1;
        } else if(entry->d_type == DT_UNKNOWN) {
            char full_path[4096];
            snprintf(full_path, sizeof(full_path), "%s/%s", packages_path, entry->d_name);
            struct stat st;
            if(stat(full_path, &st) == 0) {
                if(S_ISDIR(st.st_mode))      is_dir  = 1;
                else if(S_ISREG(st.st_mode)) is_file = 1;
            }
        }

        if(is_dir) {
            snprintf(entry_dir, sizeof(entry_dir), "%s/%s", packages_path, entry->d_name);
            snprintf(lib_path,  sizeof(lib_path),  "%s/%s" HARP_SHARED_LIBRARY_EXT, entry_dir, entry->d_name);
            has_dir = 1;
        } else if(is_file) {
            size_t name_len = strlen(entry->d_name);
            size_t ext_len  = strlen(HARP_SHARED_LIBRARY_EXT);
            if(name_len <= ext_len)
                continue;
            if(strcmp(entry->d_name + name_len - ext_len, HARP_SHARED_LIBRARY_EXT) != 0)
                continue;
            snprintf(lib_path, sizeof(lib_path), "%s/%s", packages_path, entry->d_name);
        } else {
            continue;
        }

        void *library = dlopen(lib_path, RTLD_NOW);
        if(library == NULL)
            continue;

        HarpPackageQueryFn query = (HarpPackageQueryFn)dlsym(library, "harp_package_query");
        if(query == NULL) { dlclose(library); continue; }

        HarpPackageDesc *desc = NULL;
        if(query(&desc) != HARP_RESULT_OK || desc == NULL) { dlclose(library); continue; }

#endif

        // grow pending array
        if(count >= capacity) {
            uint64_t        new_cap  = capacity == 0 ? 8 : capacity * 2;
            PendingPackage *new_pkgs = realloc(packages, sizeof(PendingPackage) * new_cap);
            if(new_pkgs == NULL) {
#if HARP_PLATFORM_WINDOWS
                FreeLibrary(library);
#else
                dlclose(library);
#endif
                continue;
            }
            packages = new_pkgs;
            capacity = new_cap;
        }

        PendingPackage *pkg = &packages[count++];
        memset(pkg, 0, sizeof(*pkg));
        pkg->desc           = desc;
        pkg->library_handle = library;

        if(has_dir)
            memcpy(pkg->directory, entry_dir, strlen(entry_dir) + 1);

#if HARP_PLATFORM_WINDOWS
    } while(FindNextFileA(handle, &find_data));

    FindClose(handle);
#else
    }

    closedir(dir);
#endif

    /* validate dependencies */

    for(uint64_t i = 0; i < count; ++i) {
        for(uint64_t j = 0; j < packages[i].desc->dependency_count; ++j) {
            HarpDependencyDesc *dep     = &packages[i].desc->p_dependencies[j];
            PendingPackage     *dep_pkg = find_pending(packages, count, dep->name);

            if(dep_pkg == NULL) {
                close_unregistered(packages, count);
                free(packages);
                return HARP_RESULT_DEPENDENCY_NOT_FOUND;
            }

            if(dep_pkg->desc->version < dep->min_version ||
               (dep->max_version != 0 && dep_pkg->desc->version > dep->max_version)) {
                close_unregistered(packages, count);
                free(packages);
                return HARP_RESULT_DEPENDENCY_VERSION_MISMATCH;
            }
        }
    }

    /* register in dependency order */

    HarpResult result = HARP_RESULT_OK;

    for(uint64_t i = 0; i < count; ++i) {
        result = register_recursive(manager, registry, core_handler, packages, count, &packages[i]);
        if(result != HARP_RESULT_OK) {
            close_unregistered(packages, count);
            break;
        }
    }

    free(packages);
    return result;
}
