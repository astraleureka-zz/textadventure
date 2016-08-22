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

/*+ loads data from json structure into mob structure +*/
boolean_t mob_json_unpack(void* mob_ptr,    /*+ target mob structure +*/
                          json_t* json_mob) /*+ source json structure +*/
/*+ returns TRUE if load was successful, FALSE otherwise +*/
{
  json_t* json_array = NULL, *json_tmp = NULL;
  mob_t* mob = mob_ptr;
  uint8_t idx;

  if (! mob || ! json_mob) return FALSE;

  mob->name  = JSON_OBJECT_STRING(json_mob, "name");
  mob->name2 = JSON_OBJECT_STRING(json_mob, "name2");

  /* attack strings */
  json_array            = JSON_OBJECT_VALUE(json_mob, "attack_strings");
  mob->attack_strs  = malloc(json_array_size(json_array) * sizeof(char*));
  json_array_foreach(json_array, idx, json_tmp) {
    mob->attack_strs[idx] = malloc((json_string_length(json_tmp) + 1) * sizeof(char));
    strcpy(mob->attack_strs[idx], json_string_value(json_tmp));
    alloc_register(mob->attack_strs[idx]);
  }
  mob->attack_str_count = json_array_size(json_array);

  /* defense strings */
  json_array            = JSON_OBJECT_VALUE(json_mob, "defend_strings");
  mob->defend_strs  = malloc(json_array_size(json_array) * sizeof(char*));
  json_array_foreach(json_array, idx, json_tmp) {
    mob->defend_strs[idx] = malloc((json_string_length(json_tmp) + 1) * sizeof(char));
    strcpy(mob->defend_strs[idx], json_string_value(json_tmp));
    alloc_register(mob->defend_strs[idx]);
  }
  mob->defend_str_count = json_array_size(json_array);

  /* description strings */
  json_array            = JSON_OBJECT_VALUE(json_mob, "describe_strings");
  mob->desc_strs    = malloc(json_array_size(json_array) * sizeof(char*));
  json_array_foreach(json_array, idx, json_tmp) {
    mob->desc_strs[idx] = malloc((json_string_length(json_tmp) + 1) * sizeof(char));
    strcpy(mob->desc_strs[idx], json_string_value(json_tmp));
    alloc_register(mob->desc_strs[idx]);
  }
  mob->desc_str_count = json_array_size(json_array);

  mob->health       = JSON_OBJECT_INTEGER(json_mob, "health");
  mob->skill        = JSON_OBJECT_INTEGER(json_mob, "skill");
  mob->strength     = JSON_OBJECT_INTEGER(json_mob, "strength");
  mob->defense      = JSON_OBJECT_INTEGER(json_mob, "defense");
  mob->celerity     = JSON_OBJECT_INTEGER(json_mob, "celerity");
  mob->intelligence = JSON_OBJECT_INTEGER(json_mob, "intelligence");

  switch (JSON_OBJECT_INTEGER(json_mob, "gender")) {
    case 0:
      mob->gender = GENDER_NONE;
      break;
    case 1:
      mob->gender = GENDER_FEMALE;
      break;
    case 2:
      mob->gender = GENDER_MALE;
      break;
    default:
      printf("mob %d bad gender %d\n", mob->id, JSON_OBJECT_INTEGER(json_mob, "gender"));
      return FALSE;
  }

/* for item array
    json_array            = JSON_OBJECT_VALUE(json_mob, "describe_strings");
    mob->desc_strs    = malloc(json_array_size(json_array) * sizeof(char*));
    json_array_foreach(json_array, idx, json_tmp) {
      mob->desc_strs[idx] = malloc((json_string_length(json_tmp) + 1) * sizeof(char));
      strcpy(mob->desc_strs[idx], json_string_value(json_tmp));
      alloc_register(mob->desc_strs[idx]);
    }
*/
  alloc_register(mob->name);
  alloc_register(mob->name2);
  alloc_register(mob->attack_strs);
  alloc_register(mob->defend_strs);
  alloc_register(mob->desc_strs);

  return TRUE;
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
