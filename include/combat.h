#ifndef _COMBAT_H
#define _COMBAT_H 1

#include "oop.h"
#include "allocator.h"
#include "item.h"
#include "world.h"
#include "mob.h"
#include "player.h"

boolean_t combat_generic(void *actor, void *target, boolean_t player_action);

#endif
