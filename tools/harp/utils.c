#include "utils.h"

#include <ctype.h>

void harp_to_upper(
    char *dst,
    const char *src
) {
    while(*src) {
        *dst++ =
            (char)toupper(
                (unsigned char)*src++
            );
    }

    *dst = 0;
}

void harp_to_lower(
    char *dst,
    const char *src
) {
    while(*src) {
        *dst++ =
            (char)tolower(
                (unsigned char)*src++
            );
    }

    *dst = 0;
}