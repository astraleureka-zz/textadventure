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

#if JANSSON_VERSION_HEX < 0x020700
inline size_t json_string_length(const json_t* string) {
  return strlen(json_string_value(string));
}
#endif

