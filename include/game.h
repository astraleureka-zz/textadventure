#ifndef _GAME_H
#define _GAME_H 1

#include <stdint.h>
#include "types.h"
#include "oop.h"
#include "world.h"
#include "player.h"

/*+ core game object, holds reference to spawn point and player object +*/
struct game {
  object proto;

  room_t* start;
  player_t* player;
};

typedef struct game game_t;

int game_init(void* self);

#endif
