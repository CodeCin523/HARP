#include "template_writer.h"

#include "templates/package_templates.h"
#include "templates/application_templates.h"

#include "utils.h"

#include <stdio.h>
#include <string.h>

#if defined(_WIN32)

    #include <direct.h>

    #define harp_mkdir(path) _mkdir(path)

#else

    #include <sys/stat.h>

    #define harp_mkdir(path) mkdir(path, 0755)

#endif

static int command_new_package(
    const char *name,
    const char *prefix
) {
    char path[1024];

    char package_upper[256];
    char package_lower[256];

    char prefix_upper[64];
    char prefix_lower[64];

    harp_to_upper(package_upper, name);
    harp_to_lower(package_lower, name);
    harp_to_upper(prefix_upper, prefix);
    harp_to_lower(prefix_lower, prefix);

    HarpTemplateArgument arguments[] = {
        {"@PACKAGE_NAME@",  name},
        {"@PACKAGE_UPPER@", package_upper},
        {"@PACKAGE_LOWER@", package_lower},
        {"@PREFIX_UPPER@",  prefix_upper},
        {"@PREFIX_LOWER@",  prefix_lower},
        {NULL, NULL}
    };

    /* --------------------------------------------------------------------- */
    /* directories                                                           */
    /* --------------------------------------------------------------------- */

    harp_mkdir(name);

    snprintf(path, sizeof(path),
        "%s/include", name);
    harp_mkdir(path);

    snprintf(path, sizeof(path),
        "%s/include/%s", name, package_lower);
    harp_mkdir(path);

    snprintf(path, sizeof(path),
        "%s/src", name);
    harp_mkdir(path);

    snprintf(path, sizeof(path),
        "%s/src/impl", name);
    harp_mkdir(path);

    snprintf(path, sizeof(path),
        "%s/cmake", name);
    harp_mkdir(path);

    snprintf(path, sizeof(path),
        "%s/tests", name);
    harp_mkdir(path);

    /* --------------------------------------------------------------------- */
    /* cmake                                                                 */
    /* --------------------------------------------------------------------- */

    snprintf(path, sizeof(path),
        "%s/CMakeLists.txt", name);
    harp_write_template(
        path,
        template_package_cmakelists,
        arguments
    );

    snprintf(path, sizeof(path),
        "%s/cmake/PackageConfig.cmake.in", name);
    harp_write_template(
        path,
        template_package_config,
        arguments
    );

    /* --------------------------------------------------------------------- */
    /* public include                                                        */
    /* --------------------------------------------------------------------- */

    snprintf(path, sizeof(path),
        "%s/include/%s/%s.h", name, package_lower, prefix_lower);
    harp_write_template(
        path,
        template_package_h,
        arguments
    );

    /* --------------------------------------------------------------------- */
    /* api implementation                                                    */
    /* --------------------------------------------------------------------- */

    snprintf(path, sizeof(path),
        "%s/src/impl/%s_hello_handler.h", name, prefix_lower);
    harp_write_template(
        path,
        template_package_impl_api_h,
        arguments
    );

    snprintf(path, sizeof(path),
"%s/src/impl/%s_hello_handler.c", name, prefix_lower);
    harp_write_template(
        path,
        template_package_impl_api_c,
        arguments
    );

    /* --------------------------------------------------------------------- */
    /* package registration                                                  */
    /* --------------------------------------------------------------------- */

    snprintf(path, sizeof(path),
        "%s/src/%s_package.c", name, prefix_lower);
    harp_write_template(
        path,
        template_package_c,
        arguments
    );

    /* --------------------------------------------------------------------- */
    /* gitignore                                                             */
    /* --------------------------------------------------------------------- */

    snprintf(path, sizeof(path),
        "%s/.gitignore", name);
    harp_write_template(
        path,
        template_gitignore,
        arguments
    );

    /* --------------------------------------------------------------------- */
    /* tests                                                                 */
    /* --------------------------------------------------------------------- */

    snprintf(path, sizeof(path),
        "%s/tests/CMakeLists.txt", name);
    harp_write_template(
        path,
        template_tests_cmakelists,
        arguments
    );

    snprintf(path, sizeof(path),
        "%s/tests/test_hello.c", name);
    harp_write_template(
        path,
        template_tests_hello,
        arguments
    );

    printf("Created package '%s'\n", name);
    return 0;
}

static int command_new_application(
    const char *name
) {
    char path[1024];

    char package_upper[256];
    char package_lower[256];

    harp_to_upper(package_upper, name);
    harp_to_lower(package_lower, name);

    HarpTemplateArgument arguments[] = {
        {"@PACKAGE_NAME@",  name},
        {"@PACKAGE_UPPER@", package_upper},
        {"@PACKAGE_LOWER@", package_lower},
        {NULL, NULL}
    };


    harp_mkdir(name);

    snprintf(path,sizeof(path),
        "%s/assets", name);
    harp_mkdir(path);

    snprintf(path,sizeof(path),
        "%s/packages", name);
    harp_mkdir(path);

    snprintf(path,sizeof(path),
        "%s/src", name);
    harp_mkdir(path);

    snprintf(path,sizeof(path),
        "%s/tests", name);
    harp_mkdir(path);

    snprintf(path, sizeof(path),
        "%s/CMakeLists.txt", name);
    harp_write_template(
        path,
        template_application_cmakelists,
        arguments
    );

    snprintf(path, sizeof(path),
        "%s/src/main.c", name);
    harp_write_template(
        path,
        template_application_main_c,
        arguments
    );

    snprintf(path, sizeof(path),
        "%s/tests/CMakeLists.txt", name);
    harp_write_template(
        path,
        template_application_tests_cmakelists,
        arguments
    );

    snprintf(path, sizeof(path),
        "%s/.gitignore", name);
    harp_write_template(
        path,
        template_gitignore,
        arguments
    );

    printf("Created application '%s'\n", name);
    return 0;
}


int command_new(
    int argc,
    char **argv
) {
    if(argc < 3) {
        fprintf(
            stderr,
            "Usage:\n"
            "  harp new package <name> <prefix>\n"
            "  harp new application <name>\n"
        );

        return 1;
    }

    if(strcmp(argv[2], "package") == 0) {

        if(argc < 5) {
            fprintf(
                stderr,
                "Usage:\n"
                "  harp new package <name> <prefix>\n"
            );

            return 1;
        }

        return command_new_package(
            argv[3],
            argv[4]
        );
    }

    if(strcmp(argv[2], "application") == 0) {

        if(argc < 4) {
            fprintf(
                stderr,
                "Usage:\n"
                "  harp new application <name>\n"
            );

            return 1;
        }

        return command_new_application(
            argv[3]
        );
    }

    fprintf(
        stderr,
        "Unknown new command '%s'\n",
        argv[2]
    );

    return 1;
}