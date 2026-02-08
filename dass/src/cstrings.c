#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "cstrings.h"

cstrings_t* appendToCstrings_t(cstrings_t *cstr, char *str) {
    if (cstr == NULL) {
        cstr = malloc(sizeof(cstrings_t));
        cstr->length = 0;
        cstr->max_size = 256;
        cstr->contents = malloc(cstr->max_size * sizeof(char));
    }

    uint32_t length_of_str = strlen(str);
    uint32_t new_max_size = cstr->max_size;
    while (length_of_str + cstr->length >= new_max_size) {
        new_max_size *= 2;
    }

    if (new_max_size >= cstr->max_size) {
        cstr->max_size = new_max_size;
        char *old_cstrings_t_content = cstr->contents;
        cstr->contents = malloc(new_max_size * sizeof(char));
        strcpy(cstr->contents, old_cstrings_t_content);
    }

    strcpy(&cstr->contents[cstr->length], str);
    cstr->length += length_of_str;

    return cstr;
}

char* getCstrings_tContent(cstrings_t *cstr) {
    if (cstr == NULL) {
        return NULL;
    }

    char *contents = malloc((cstr->length + 1) * sizeof(char));

    strcpy(contents, cstr->contents);
    contents[cstr->length] = '\0';

    return contents;
}

char charAtCstrings_t(cstrings_t *cstr, uint32_t pos) {
    if (cstr == NULL) {
        return '\0';
    }

    if (cstr->length < pos) {
        return '\0';
    }

    return cstr->contents[pos];
}

cstrings_t * concatCstrings_t(cstrings_t * cstrA, cstrings_t * cstrB) {
    if (cstrA == NULL) {
        return cstrB;
    }

    if (cstrB == NULL) {
        return cstrA;
    }

    uint8_t max_max_size = cstrA->max_size;
    if (max_max_size < cstrB->max_size) {
        max_max_size = cstrB->max_size;
    }

    int total_length = cstrA->length + cstrB->length;
    if (max_max_size <= total_length) {
        max_max_size *= 2;
    }

    cstrings_t *cstr = malloc(sizeof(cstrings_t));
    cstr->length = total_length;
    cstr->max_size = max_max_size;
    cstr->contents = malloc(cstr->max_size * sizeof(char));

    strcpy(cstr->contents, cstrA->contents);
    strcpy(&cstr->contents[cstrA->length], cstrB->contents);

    return cstr;
}

