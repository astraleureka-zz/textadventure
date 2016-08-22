#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <errno.h>
#include <jansson.h>

#include "allocator.h"
#include "types.h"
#include "game.h"
#include "combat.h"
#include "util.h"

/*+ mob class prototype +*/
object mob_proto = {
  .init        = mob_init,
  .take_action = combat_generic,
  .recv_action = mob_attack_receive,
  .describe    = mob_describe
};

int mob_init(void* self) {
  return 1;
}

/*+ handles receiving an attack +*/
int mob_attack_receive(void* self,     /*+ pointer to called object +*/
                       uint8_t damage) /*+ raw amount damage received +*/
/*+ returns 1 if mob will die from this attack, otherwise 0 if mob continues to live +*/
{
  mob_t* mob = self;

  if (damage >= mob->health)
    return 1;

  mob->health -= damage;
  return 0;
}

/*+ outputs the description of the mob to the console +*/
void mob_describe(void* self) {
  mob_t* mob = self;

  printf("%s\n", mob_string_get(mob, MOB_STR_DESCRIBE));
}

/*+ obtain a mob's string given a specified type +*/
char* mob_string_get(mob_t* mob,        /*+ pointer to called object +*/
                     mob_string_type t) /*+ string type enum +*/
/*+ returns a randomly chosen string of the specified type, or NULL on error +*/
{
  char **strings;
  uint8_t count;

  if (! mob) return NULL;

  switch (t) {
    case MOB_STR_ATTACK:
      strings = mob->attack_strs;
      count   = mob->attack_str_count;
      break;
    case MOB_STR_DEFEND:
      strings = mob->defend_strs;
      count   = mob->defend_str_count;
      break;
    case MOB_STR_DESCRIBE:
      strings = mob->desc_strs;
      count   = mob->desc_str_count;
      break;
    default:
      return NULL;
  }

  return strings[rand() % count];
}

/*+ outputs the mob's current health and stats to the console +*/
void mob_check(mob_t* mob) {
  printf("Enemy HP: %3d    ATK: %3d  DEF: %3d\n", mob->health, mob->strength, mob->defense);
}

/*+ loads all mob files in mob/ +*/
boolean_t mob_load(mob_t** mobs,   /*+ pointer to empty mob list +*/
                   item_t** items) /*+ pointer to item list used to create item copies inside mob structs +*/
/*+ returns TRUE on success, FALSE on failure +*/
{
  DIR* dh;
  struct dirent* d;
  char* path_tmp = malloc(MAX_STRLEN);
  uint16_t i     = 0;
  /* following vars used per loop */
  size_t idx;
  json_t* json_mob, *json_tmp, *json_array;
  json_error_t json_error;
  uint8_t mob_id;

  assert(NULL != mobs);
  assert(NULL != path_tmp);

  memset(mobs, 0, sizeof(mob_t*) * MAX_MOBS);

  dh = opendir("mobs");
  if (NULL == dh) {
    perror("opendir mobs");
    goto ERROR;
  }

  while (NULL != (d = readdir(dh))) {
    if (*d->d_name == '.') continue; /* skip any lines starting with . */

    if ((strlen(d->d_name) + 5) > MAX_STRLEN) { /* 5 = "mobs/" */
      printf("mobs/%s - path too long\n", d->d_name);
      goto ERROR;
    }

    snprintf(path_tmp, MAX_STRLEN - 1, "mobs/%s", d->d_name);
    if (NULL == (json_mob = json_load_file(path_tmp, 0, &json_error))) {
      printf("%s - error line %d: %s\n", path_tmp, json_error.line, json_error.text);
      goto ERROR;
    }

    if (! json_is_object(json_mob)) {
      printf("%s - json is not an object\n", path_tmp);
      goto ERROR;
    }

    mob_id = JSON_OBJECT_INTEGER(json_mob, "id");
    if (mobs[mob_id]) { /* XXX should probably have a dedicated bit for checking whether it exists or not */
      printf("%s - duplicate mob id %d (already assigned to %s)\n", path_tmp, mob_id, mobs[mob_id]->_(class));
      goto ERROR;
    }

    assert(mobs[mob_id]        = NEW(mob, path_tmp));
    mobs[mob_id]->name         = JSON_OBJECT_STRING(json_mob, "name");
    mobs[mob_id]->name2        = JSON_OBJECT_STRING(json_mob, "name2");

    /* attack strings */
    json_array            = JSON_OBJECT_VALUE(json_mob, "attack_strings");
    mobs[mob_id]->attack_strs  = malloc(json_array_size(json_array) * sizeof(char*));
    json_array_foreach(json_array, idx, json_tmp) {
      mobs[mob_id]->attack_strs[idx] = malloc((json_string_length(json_tmp) + 1) * sizeof(char));
      strcpy(mobs[mob_id]->attack_strs[idx], json_string_value(json_tmp));
      alloc_register(mobs[mob_id]->attack_strs[idx]);
    }
    mobs[mob_id]->attack_str_count = json_array_size(json_array);

    /* defense strings */
    json_array            = JSON_OBJECT_VALUE(json_mob, "defend_strings");
    mobs[mob_id]->defend_strs  = malloc(json_array_size(json_array) * sizeof(char*));
    json_array_foreach(json_array, idx, json_tmp) {
      mobs[mob_id]->defend_strs[idx] = malloc((json_string_length(json_tmp) + 1) * sizeof(char));
      strcpy(mobs[mob_id]->defend_strs[idx], json_string_value(json_tmp));
      alloc_register(mobs[mob_id]->defend_strs[idx]);
    }
    mobs[mob_id]->defend_str_count = json_array_size(json_array);

    /* description strings */
    json_array            = JSON_OBJECT_VALUE(json_mob, "describe_strings");
    mobs[mob_id]->desc_strs    = malloc(json_array_size(json_array) * sizeof(char*));
    json_array_foreach(json_array, idx, json_tmp) {
      mobs[mob_id]->desc_strs[idx] = malloc((json_string_length(json_tmp) + 1) * sizeof(char));
      strcpy(mobs[mob_id]->desc_strs[idx], json_string_value(json_tmp));
      alloc_register(mobs[mob_id]->desc_strs[idx]);
    }
    mobs[mob_id]->desc_str_count = json_array_size(json_array);

    mobs[mob_id]->health       = JSON_OBJECT_INTEGER(json_mob, "health");
    mobs[mob_id]->skill        = JSON_OBJECT_INTEGER(json_mob, "skill");
    mobs[mob_id]->strength     = JSON_OBJECT_INTEGER(json_mob, "strength");
    mobs[mob_id]->defense      = JSON_OBJECT_INTEGER(json_mob, "defense");
    mobs[mob_id]->celerity     = JSON_OBJECT_INTEGER(json_mob, "celerity");
    mobs[mob_id]->intelligence = JSON_OBJECT_INTEGER(json_mob, "intelligence");

    switch (JSON_OBJECT_INTEGER(json_mob, "gender")) {
      case 0:
        mobs[mob_id]->gender = GENDER_NONE;
        break;
      case 1:
        mobs[mob_id]->gender = GENDER_FEMALE;
        break;
      case 2:
        mobs[mob_id]->gender = GENDER_MALE;
        break;
      default:
        printf("mob %d bad gender %d\n", i, JSON_OBJECT_INTEGER(json_mob, "gender"));
        goto ERROR;
    }

/* for item array
    json_array            = JSON_OBJECT_VALUE(json_mob, "describe_strings");
    mobs[mob_id]->desc_strs    = malloc(json_array_size(json_array) * sizeof(char*));
    json_array_foreach(json_array, idx, json_tmp) {
      mobs[mob_id]->desc_strs[idx] = malloc((json_string_length(json_tmp) + 1) * sizeof(char));
      strcpy(mobs[mob_id]->desc_strs[idx], json_string_value(json_tmp));
      alloc_register(mobs[mob_id]->desc_strs[idx]);
    }
*/
    alloc_register(mobs[mob_id]->name);
    alloc_register(mobs[mob_id]->name2);
    alloc_register(mobs[mob_id]->attack_strs);
    alloc_register(mobs[mob_id]->defend_strs);
    alloc_register(mobs[mob_id]->desc_strs);
    json_decref(json_mob);
  }

  if (path_tmp) free(path_tmp);
  if (dh) closedir(dh);
  return TRUE;

  ERROR:
  if (path_tmp) free(path_tmp);
  if (dh) closedir(dh);
  if (json_mob) json_decref(json_mob);
  return FALSE;
}

/*+ performs a deep copy of a mob structure +*/
void mob_copy(mob_t* copy, /*+ pointer to dest mob struct +*/
              mob_t* mob)  /*+ pointer to source mob struct +*/
{
  uint8_t i;

  if (! mob || ! copy) return;

  /* perform a regular copy of the data */
  memcpy(copy, mob, sizeof(mob_t));
  /* then we go and update all pointers that we want to duplicate */
  /* XXX inventory copy */
  copy->name  = strdup(mob->name);
  copy->name2 = strdup(mob->name2);

  /* XXX how should we handle the lifetime of the object? */
  alloc_register(copy->name);
  alloc_register(copy->name2);

  for (i = 0; i < mob->attack_str_count; i++) {
    copy->attack_strs[i] = strdup(mob->attack_strs[i]);
    alloc_register(copy->attack_strs[i]);
  }

  for (i = 0; i < mob->defend_str_count; i++) {
    copy->defend_strs[i] = strdup(mob->defend_strs[i]);
    alloc_register(copy->defend_strs[i]);
  }

  for (i = 0; i < mob->desc_str_count; i++) {
    copy->desc_strs[i] = strdup(mob->desc_strs[i]);
    alloc_register(copy->desc_strs[i]);
  }
}
