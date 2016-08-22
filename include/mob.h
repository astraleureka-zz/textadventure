#ifndef _MOB_H
#define _MOB_H 1

#include <jansson.h>

#include "types.h"
#include "item.h"

/* mob definitions */
struct mob {
  object proto;

  gender gender;

  uint8_t id;
  uint8_t health;
  uint8_t skill;
  uint8_t strength;
  uint8_t defense;
  uint8_t celerity;
  uint8_t intelligence;

  uint8_t attack_str_count;
  uint8_t defend_str_count;
  uint8_t desc_str_count;

  item_t** inventory;

  char* name;
  char* name2;
  char** attack_strs;
  char** defend_strs;
  char** desc_strs;
};

typedef struct mob mob_t;

typedef enum {
  MOB_STR_ATTACK,
  MOB_STR_DEFEND,
  MOB_STR_DESCRIBE,
} mob_string_type;

extern object mob_proto;

int mob_init(void* self);
int mob_attack_receive(void* self, uint8_t damage);
void mob_describe(void* self);
char* mob_string_get(mob_t* mob, mob_string_type t);
void mob_check(mob_t* mob);
boolean_t mob_json_unpack(mob_t* mob, json_t* json_mob);
void mob_copy(mob_t* copy, mob_t* mob);

#endif
