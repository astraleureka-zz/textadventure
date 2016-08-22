#ifndef _UTIL_H
#define _UTIL_H 1

#include <stddef.h>
#include <jansson.h>

#define JSON_OBJECT_VALUE(obj, key) json_object_get(obj, key)
#define JSON_OBJECT_STRING(obj, key) strdup(json_string_value(json_object_get(obj, key)))
#define JSON_OBJECT_INTEGER(obj, key) json_integer_value(json_object_get(obj, key))
#define JSON_OBJECT_REAL(obj, key) json_real_value(json_object_get(obj, key))
#define JSON_ARRAY_VALUE(obj, idx) json_array_get(obj, idx)
#define JSON_ARRAY_STRING(obj, idx) strdup(json_string_value(json_array_get(obj, idx)))
#define JSON_ARRAY_INTEGER(obj, idx) json_integer_value(json_array_get(obj, idx))
#define JSON_ARRAY_REAL(obj, idx) json_real_value(json_array_get(obj, idx))

#if JANSSON_VERSION_HEX < 0x020700
size_t json_string_length(const json_t* string);
#endif

char *util_pronoun_get(gender g, pronoun_case c);

#endif
