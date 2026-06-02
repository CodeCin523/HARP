#ifndef TEMPLATE_WRITER_H
#define TEMPLATE_WRITER_H

typedef struct {
    const char *key;
    const char *value;
} HarpTemplateArgument;

int harp_write_template(
    const char *path,
    const char *template_text,
    const HarpTemplateArgument *arguments
);

#endif