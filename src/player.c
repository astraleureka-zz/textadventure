#include <stdio.h>
#include <stdlib.h>

#include "types.h"
#include "game.h"

/*+ initializes the player object.
  XXX - perhaps we should prompt the player to choose a class or other starting stats instead of randomly generating? +*/
int player_init(void *self) {
  player *player     = self;
  player->health     = 25 + (rand() % 17);
  player->strength   = 10 + (rand() % 5);
  player->defense    = 6 + (rand() % 7);
  return 1;
}

/*+ handles receiving an attack +*/
int player_attack_receive(void *self,     /*+ pointer to called object +*/
                          uint8_t damage) /*+ raw amount damage received +*/
/*+ always returns 0. if the player dies, the game exits +*/
{
  player *player = self;

  if (damage >= player->health) {
    printf("You have died!\n");
    exit(0);
  }

  player->health -= damage;
  return 0;
}

/*+ handles moving the player from one room to another. if the player is currently engaged in combat, they will fail to move. they will also fail to move in a direction that has no link. +*/
void player_move(void *self,    /*+ pointer to called object +*/
                 direction dir) /*+ enum of cardinal directions +*/
{
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

/*+ outputs the player's current health and stats to the console +*/
void player_check(player *player) {
  printf(" Your HP: %3d    ATK: %3d  DEF: %3d\n", player->health, player->strength, player->defense);
}
