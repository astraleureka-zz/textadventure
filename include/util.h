#ifndef _UTIL_H
#define _UTIL_H 1

#include <stddef.h>

/*+ a generic frec object, containing fields that are universal to all frecs +*/
struct generic_frec {
  uint8_t obj_id; /*+ the frec's object id is always the field field +*/
};

char *util_pronoun_get(gender g, pronoun_case c);
boolean_t util_frec_load(char* path, void** frecs, size_t frec_size);

#endif
