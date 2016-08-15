#include <stdio.h>
#include <string.h>
#include <dirent.h>
#include <assert.h>
#include <stdlib.h>
#include <errno.h>
#include <stdint.h>
#include <time.h>
#include "game.h"

int monster_init(void *self) {
  return 1;
}

uint8_t monster_attack(void *self, void *target) {
  monster *monster = self;
  player *player   = target;

  uint8_t damage = damage_calculate(monster->attack_dmg, player->defense);
  printf("%s %s! You take %d damage.\n", monster->name2, monster->attack_str, damage);

  return player->_(recv_action)(player, damage);
}

int monster_attack_receive(void *self, uint8_t damage) {
  monster *monster = self;

  if (damage >= monster->health)
    return 1;

  monster->health -= damage;
  return 0;
}

void monster_describe(void *self) {
  monster *monster = self;

  printf("%s\n", monster->desc_str);
}

void monster_check(monster *monster) {
  printf("Enemy HP: %3d    ATK: %3d  DEF: %3d\n", monster->health, monster->attack_dmg, monster->defense);
}
