#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <dirent.h>
#include <errno.h>
#include <assert.h>
#include <jansson.h>

#include "types.h"
#include "util.h"

/*+ determines the correct pronoun given a gender and case +*/
char* util_pronoun_get(gender g,       /*+ gender enum +*/
                       pronoun_case c) /*+ case type enum +*/
/*+ returns a string containing the pronoun */
{
  switch (g) {
    case GENDER_NONE:
      switch (c) {
        case U_PRONOUN_POSSESSIVE:
          return "its";
        case U_PRONOUN_SUBJECTIVE:
        case U_PRONOUN_OBJECTIVE:
          return "it";
        default:
          return "<BAD CASE>";
      }

    case GENDER_FEMALE:
      switch (c) {
        case U_PRONOUN_SUBJECTIVE:
          return "she";
        case U_PRONOUN_POSSESSIVE:
        case U_PRONOUN_OBJECTIVE:
          return "her";
        default:
          return "<BAD CASE>";
      }

    case GENDER_MALE:
      switch (c) {
        case U_PRONOUN_SUBJECTIVE:
          return "he";
        case U_PRONOUN_POSSESSIVE:
          return "his";
        case U_PRONOUN_OBJECTIVE:
          return "him";
        default:
          return "<BAD CASE>";
      }

    default:
      return "<BAD GENDER>";
  }

  /* UNREACHABLE */
}

/*+ generic routine to iterate over an asset folder and load each file +*/
boolean_t util_load_json_asset(char* path,                               /*+ path to the folder to load +*/
                               void **target,                            /*+ list to load asset pointers to +*/
                               size_t record_size,                       /*+ size of each asset struct +*/
                               size_t record_count,                      /*+ count of asset structs +*/
                               object proto,                             /*+ function prototype of asset object that will be created +*/
                               size_t proto_size,                        /*+ size of the prototype struct +*/
                               boolean_t (*unpacker_cb)(void*, json_t*)) /*+ pointer to function that will unpack a json_t structure into its runtime object +*/
/*+ returns TRUE when all assets were loaded successfully, FALSE otherwise +*/
{
  DIR* dh;
  struct dirent* d;
  char* path_tmp   = malloc(MAX_STRLEN), *error = malloc(MAX_STRLEN);
  uint8_t path_len = strlen(path);
  /* per loop variables */
  object_generic* obj_gen = NULL;
  json_t* json_obj = NULL;
  json_error_t json_error;
  uint8_t obj_id;

  assert(NULL != target);
  assert(NULL != path_tmp);

  memset(target, 0, record_size * record_count);

  dh = opendir(path);
  if (NULL == dh) {
    snprintf(error, MAX_STRLEN - 1, "opendir %s", path);
    goto PERROR;
  }

  while (NULL != (d = readdir(dh))) {
    if (*d->d_name == '.') continue; /* skip any lines starting with . */

    if ((strlen(d->d_name) + path_len) > MAX_STRLEN) {
      printf("%s/%s - path too long\n", path, d->d_name);
      goto ERROR;
    }

    snprintf(path_tmp, MAX_STRLEN - 1, "%s/%s", path, d->d_name);
    if (NULL == (json_obj = json_load_file(path_tmp, 0, &json_error))) {
      printf("%s - error line %d: %s\n", path_tmp, json_error.line, json_error.text);
      goto ERROR;
    }

    if (! json_is_object(json_obj)) {
      printf("%s - json is not an object\n", path_tmp);
      goto ERROR;
    }

    obj_id = JSON_OBJECT_INTEGER(json_obj, "id");
    if (target[obj_id]) {
      obj_gen = (object_generic*)target[obj_id];
      printf("%s - duplicate object id %d (already assigned to %s)\n", path_tmp, obj_id, obj_gen->proto.class);
      goto ERROR;
    }

    assert(target[obj_id] = object_new(proto_size, proto, path_tmp));
    if (! unpacker_cb(target[obj_id], json_obj)) {
      printf("%s - unpacker callback failed, check file syntax?\n", path_tmp);
      goto ERROR;
    }

    json_decref(json_obj);
  }

  if (path_tmp) free(path_tmp);
  if (error) free(error);
  if (dh) closedir(dh);
  return TRUE;

  PERROR:
  perror(error);
  ERROR:
  if (path_tmp) free(path_tmp);
  if (error) free(error);
  if (dh) closedir(dh);
  return FALSE;
}

/*+ wrapper function to safely retrieve a string from a json_t object +*/
inline char* util_json_object_string(json_t* obj, /*+ json node pointer +*/
                                     char* key)   /*+ key to retrieve +*/
/*+ returns the string corresponding to key, or "(null)" if the key doesn't exist +*/
{
  json_t* tmp = json_object_get(obj, key);
  if (json_is_string(tmp)) return strdup(json_string_value(tmp));
  return strdup("(null)");
}

/*+ wrapper function to safely retrieve a string from a json_t array +*/
inline char* util_json_array_string(json_t* obj, /*+ json node pointer +*/
                                    int idx)     /*+ key to retrieve +*/
/*+ returns the string corresponding to idx, or "(null)" if the key doesn't exist +*/
{
  json_t* tmp = json_array_get(obj, idx);
  if (json_is_string(tmp)) return strdup(json_string_value(tmp));
  return strdup("(null)");
}

#if JANSSON_VERSION_HEX < 0x020700
inline size_t json_string_length(const json_t* string) {
  if (json_is_string(string)) return strlen(json_string_value(string));
  return 0;
}
#endif

