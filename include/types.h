#ifndef _TYPES_H
#define _TYPES_H 1

#define FALSE 0
#define TRUE (!FALSE)
typedef unsigned int boolean_t;

#define MAX_NAMELEN 32
#define MAX_STRLEN 256
#define MAX_ITEMS 256
#define MAX_MOBS 256
#define MAX_ROOMS 256
#define MAX_ITEMCNT 32
#define MAX_STRCNT 32

#include <stdint.h>

/*+ indicates the desired case of the pronoun when calling util_pronoun_get +*/
typedef enum {
  U_PRONOUN_SUBJECTIVE,
  U_PRONOUN_POSSESSIVE,
  U_PRONOUN_OBJECTIVE,
} pronoun_case;

/*+ used to identify movement in the console +*/
typedef enum {
  NORTH, SOUTH, EAST, WEST
} direction;

/*+ used for getting the proper pronoun when building strings +*/
typedef enum {
  GENDER_NONE,   /*+ it(s), they */
  GENDER_FEMALE, /*+ she, her +*/
  GENDER_MALE    /*+ him, his +*/
} gender;

#endif

