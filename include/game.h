#ifndef _GAME_H
#define _GAME_H 1

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

#endif
