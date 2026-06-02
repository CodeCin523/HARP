#include "template_writer.h"

#include <stdio.h>
#include <string.h>

static const char *find_replacement(
    const char *token,
    const HarpTemplateArgument *arguments
) {
    while(arguments->key != NULL) {

        if(strcmp(
            token,
            arguments->key
        ) == 0)
            return arguments->value;

        ++arguments;
    }

    return NULL;
}

int harp_write_template(
    const char *path,
    const char *template_text,
    const HarpTemplateArgument *arguments
) {
    FILE *file =
        fopen(path, "w");

    if(file == NULL)
        return 0;

    while(*template_text) {

        if(*template_text == '@') {

            const char *end =
                strchr(
                    template_text + 1,
                    '@'
                );

            if(end != NULL) {

                char token[128];

                size_t length =
                    (size_t)(
                        end -
                        template_text +
                        1
                    );

                memcpy(
                    token,
                    template_text,
                    length
                );

                token[length] = 0;

                const char *replacement =
                    find_replacement(
                        token,
                        arguments
                    );

                if(replacement != NULL) {
                    fputs(
                        replacement,
                        file
                    );

                    template_text =
                        end + 1;

                    continue;
                }
            }
        }

        fputc(
            *template_text,
            file
        );

        ++template_text;
    }

    fclose(file);

    return 1;
}