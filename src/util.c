#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <dirent.h>
#include <errno.h>
#include <assert.h>

#include "types.h"
#include "util.h"

/*+ determines the correct pronoun given a gender and case +*/
char *util_pronoun_get(gender g,       /*+ gender enum +*/
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

/*+ generic routine to load frec data. given a path to a folder this function will open each file, read frec_size bytes, and load the frecs array as necessary +*/
boolean_t util_frec_load(char* path,       /*+ string containing relative or absolute path to frec folder +*/
                         void** frecs,     /*+ triple pointer holding a list of pointers to individual frecs +*/
                         size_t frec_size) /*+ the size of an individual frec being read +*/
/*+ returns TRUE if all frecs successfully load, FALSE otherwise +*/
{
  DIR *dh  = NULL;
  FILE *fh = NULL;
  struct dirent *d;
  char *path_tmp = malloc(MAX_STRLEN), *err = malloc(MAX_STRLEN);
  void *frec = NULL;

  assert(NULL != err);
  assert(NULL != path_tmp);

  if (strlen(path) >= MAX_STRLEN) {
    printf("util_frec_load(%s, %p, %d): path is too long (%d >= %d)\n", path, frecs, frec_size, strlen(path), MAX_STRLEN);
    goto ERROR;
  }

  dh = opendir(path);
  if (NULL == dh) {
    snprintf(err, MAX_STRLEN - 1, "util_frec_load(%s, %p, %d): opendir", path, frecs, frec_size);
    goto PERROR;
  }

  while (NULL != (d = readdir(dh))) {
    if (*d->d_name == '.') continue;

    if ((strlen(d->d_name) + strlen(path) + 1) > MAX_STRLEN) {
      printf("util_frec_load(%s, %p, %d): filename %s too long (%d >= %d)\n", path, frecs, frec_size, d->d_name, (strlen(d->d_name) + strlen(path) + 1), MAX_STRLEN);
      goto ERROR;
    }

    snprintf(path_tmp, MAX_STRLEN - 1, "%s/%s", path, d->d_name);
    fh = fopen(path_tmp, "r");
#ifdef DEBUG
    printf("util_frec_load(%s, %p, %d): fopen(%s, 'r') = %d\n", path, frecs, frec_size, path_tmp, fh);
#endif
    if (NULL == fh) {
      snprintf(err, MAX_STRLEN - 1, "util_frec_load(%s, %p, %d): fopen %s", path, frecs, frec_size, d->d_name);
      goto PERROR;

    }

    /* clean up the struct from last time */
    if (frec) free(frec);
    assert(frec = malloc(frec_size));

    if (! fread(frec, frec_size, 1, fh)) {
      printf("util_frec_load(%s, %p, %d): fread %s too short (try rebuilding frecs)\n", path, frecs, frec_size, d->d_name);
      goto ERROR;
    }

    if (((struct generic_frec*)frec)->obj_id != 0)
      memcpy(frecs[ ((struct generic_frec*)frec)->obj_id ], frec, frec_size);

    free(frec);
    frec = NULL;
    fclose(fh);
  }

  if (path_tmp) free(path_tmp);
  if (err)      free(err);
  if (frec)     free(frec);
  if (dh)       closedir(dh);
  return TRUE;

  PERROR:
  perror(err);
  ERROR:
  if (path_tmp) free(path_tmp);
  if (err)      free(err);
  if (frec)     free(frec);
  if (dh)       closedir(dh);
  return FALSE;
}

