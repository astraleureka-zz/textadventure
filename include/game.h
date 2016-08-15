#ifndef _GAME_H
#define _GAME_H 1

#define MAX_NAMELEN 31
#define MAX_STRLEN 255
#define FALSE 0
#define TRUE !FALSE

typedef unsigned int boolean_t;

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

uint8_t damage_calculate(uint8_t atk, uint8_t def);
int game_init(void *self);

object monster_proto;
object room_proto;
object player_proto;

#endif
