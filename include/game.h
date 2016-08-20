#ifndef _GAME_H
#define _GAME_H 1

#define MAX_NAMELEN 32
#define MAX_STRLEN 256

#include <stdint.h>
#include "oop.h"
#include "allocator.h"
#include "item.h"
#include "world.h"
#include "mob.h"
#include "player.h"

/* game definitions */
typedef struct {
  object proto;

  room *start;
  player *player;
} game;

int game_init(void *self);

object monster_proto;
object room_proto;
object player_proto;

#endif
