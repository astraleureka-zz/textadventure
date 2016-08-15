#ifndef _WORLD_H
#define _WORLD_H 1

#include <stdint.h>
#include "oop.h"
#include "game.h"
#include "mob.h"

#define MAX_ROOMS 255

/* Room definitions */
struct room {
  object proto;

  monster *monster;

  struct room *north;
  struct room *south;
  struct room *east;
  struct room *west;

  char *description;
  char *name;
};

typedef struct room room;

struct room_frec {
  uint8_t room_id;
  uint8_t monster_id;
  uint8_t north_id;
  uint8_t south_id;
  uint8_t east_id;
  uint8_t west_id;
  char name[MAX_NAMELEN];
  char description[MAX_STRLEN];
};

typedef struct room_frec room_frec;

int room_init(void *self);
void room_describe(void *self);

#endif
