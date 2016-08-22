#ifndef _UTIL_H
#define _UTIL_H 1

#include <stddef.h>
#include <jansson.h>

#include "oop.h"

#define JSON_OBJECT_VALUE(obj, key) json_object_get(obj, key)
#define JSON_OBJECT_STRING(obj, key) strdup(json_string_value(json_object_get(obj, key)))
#define JSON_OBJECT_INTEGER(obj, key) (int)json_integer_value(json_object_get(obj, key))
#define JSON_OBJECT_REAL(obj, key) json_real_value(json_object_get(obj, key))
#define JSON_OBJECT_BOOLEAN(obj, key) (boolean_t)json_boolean_value(json_object_get(obj, key))
#define JSON_ARRAY_VALUE(obj, idx) json_array_get(obj, idx)
#define JSON_ARRAY_STRING(obj, idx) strdup(json_string_value(json_array_get(obj, idx)))
#define JSON_ARRAY_INTEGER(obj, idx) (int)json_integer_value(json_array_get(obj, idx))
#define JSON_ARRAY_REAL(obj, idx) json_real_value(json_array_get(obj, idx))
#define JSON_ARRAY_BOOLEAN(obj, key) (boolean_t)json_boolean_value(json_array_get(obj, key))

#if JANSSON_VERSION_HEX < 0x020700
size_t json_string_length(const json_t* string);
# define json_boolean_value(j) json_is_true(j)
#endif

char *util_pronoun_get(gender g, pronoun_case c);
boolean_t util_load_json_asset(char* path, void **target, size_t record_size, size_t record_count, object proto, size_t proto_size, boolean_t (*unpacker_cb)(void*, json_t*));

#endif
