#include "command_version.c"
#include "command_new.c"

#include "templates/package_templates.c"
#include "templates/application_templates.c"

#include "template_writer.c"
#include "utils.c"

#include <stdio.h>
#include <string.h>

static void print_help(void) {
    printf(
        "HARP Command Line Tool\n"
        "\n"
        "Commands:\n"
        "  harp version\n"
        "  harp new package <name> <prefix>\n"
        "  harp new application <name>\n"
    );
}

int main(
    int argc,
    char **argv
) {
    if(argc < 2) {
        print_help();
        return 1;
    }

    if(strcmp(argv[1], "version") == 0)
        return command_version();

    if(strcmp(argv[1], "new") == 0)
        return command_new(argc, argv);

    print_help();
    return 1;
}