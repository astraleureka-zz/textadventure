#ifndef _WORLD_H
#define _WORLD_H 1

#include "types.h"
#include "mob.h"
#include "item.h"

/* Room definitions */
struct room {
  object proto;

  mob_t* mob;
  item_t* item;

  uint8_t north_id, south_id, east_id, west_id, mob_id, item_id;

  struct room* north;
  struct room* south;
  struct room* east;
  struct room* west;

  char* description;
  char* name;
};

typedef struct room room_t;

extern object room_proto;

int room_init(void* self);
void room_describe(void* self);
boolean_t room_json_unpack(room_t* room, json_t* json_room);
boolean_t room_linkage_create(room_t** rooms, mob_t** mobs, item_t** items);

#endif
