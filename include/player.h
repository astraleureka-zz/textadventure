#ifndef _PLAYER_H
#define _PLAYER_H 1

#include <stdint.h>
#include "oop.h"

/* Player definitions */
typedef struct {
  object proto;

  room *room_current;

  uint8_t health;
  uint8_t skill;
  uint8_t strength;
  uint8_t defense;
  uint8_t celerity;
  uint8_t intelligence;
} player;

int player_init(void *self);
int player_attack_receive(void *self, uint8_t damage);
void player_move(void *self, direction dir);
void player_check(player *player);

#endif
