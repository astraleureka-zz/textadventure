#ifndef _GAME_H
#define _GAME_H 1

#define MAX_NAMELEN 32
#define MAX_STRLEN 256

#include <stdint.h>
#include "types.h"
#include "oop.h"
#include "world.h"
#include "player.h"

/*+ core game object, holds reference to spawn point and player object +*/
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
