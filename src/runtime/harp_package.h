#ifndef RUNTIME_HARP_PACKAGE_H
#define RUNTIME_HARP_PACKAGE_H

#include <harp/harp_core.h>
#include "harp_registry.h"


typedef struct HarpLoadedPackage {
    HarpName    name;
    HarpVersion version;
    void       *library_handle;
    char       *directory;       // NULL for direct .so packages with no assets
} HarpLoadedPackage;

typedef struct HarpPackageManager {
    HarpLoadedPackage *packages;
    uint64_t           count;
    uint64_t           capacity;
} HarpPackageManager;


HarpResult harp_setup_package_manager(HarpPackageManager *manager);
void harp_teardown_package_manager(HarpPackageManager *manager);

HarpResult harp_package_manager_add(HarpPackageManager *manager, HarpName name, HarpVersion version, void *library_handle, const char *directory);
HarpLoadedPackage *harp_package_manager_get(HarpPackageManager *manager, HarpName name);
HarpLoadedPackage *harp_package_manager_get_at(HarpPackageManager *manager, uint64_t index);

HarpResult harp_package_manager_load(HarpPackageManager *manager, HarpRegistry *registry, HarpCoreHandler *core_handler, const char *path);


#endif /* RUNTIME_HARP_PACKAGE_H */
