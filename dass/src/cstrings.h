#ifndef CSTRINGS_H_
#define CSTRINGS_H_

#include <stdint.h>

typedef struct {
    char *contents;
    uint32_t length;
    uint32_t max_size;
} cstrings_t;


cstrings_t* appendToCstrings_t(cstrings_t *cstr, char *str);
char* getCstrings_tContent(cstrings_t *cstr);
char charAtCstrings_t(cstrings_t *cstr, uint32_t pos);
cstrings_t * concatCstrings_t(cstrings_t * cstrA, cstrings_t * cstrB);

#endif /* CSTRINGS_H_ */
