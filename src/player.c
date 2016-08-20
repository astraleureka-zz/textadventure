#include <stdio.h>
#include <string.h>
#include <dirent.h>
#include <assert.h>
#include <stdlib.h>
#include <errno.h>
#include <stdint.h>
#include <time.h>
#include "game.h"

int player_init(void *self) {
  player *player     = self;
  player->health     = 25 + (rand() % 17);
  player->strength   = 10 + (rand() % 5);
  player->defense    = 6 + (rand() % 7);
  return 1;
}

int player_attack_receive(void *self, uint8_t damage) {
  player *player = self;

  if (damage >= player->health) {
    printf("You have died!\n");
    exit(0);
  }

  player->health -= damage;
  return 0;
}

void player_move(void *self, direction dir) {
  player *player = self;
  room *current  = player->room_current, *next = NULL;
  monster *monster = current->monster;

  if (monster && monster->health > 0) {
    printf("You try to sneak past %s, but it notices you!\n", monster->name2);
    monster->_(take_action)(monster, player, FALSE);
    return;
  }

  if (dir == NORTH && current->north) {
    printf("You move north.\n");
    next = current->north;
  }
  else if (dir == SOUTH && current->south) {
    printf("You move south.\n");
    next = current->south;
  }
  else if (dir == EAST && current->east) {
    printf("You move east.\n");
    next = current->east;
  }
  else if (dir == WEST && current->west) {
    printf("You move west.\n");
    next = current->west;
  }
  else {
    printf("You can't go in that direction.\n");
  }

  if (next) {
    next->_(describe)(next);
    player->room_current = next;
  }
}

void player_check(player *player) {
  printf(" Your HP: %3d    ATK: %3d  DEF: %3d\n", player->health, player->strength, player->defense);
}
