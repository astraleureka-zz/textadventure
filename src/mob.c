#include <stdio.h>

#include "types.h"
#include "game.h"

int monster_init(void *self) {
  return 1;
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
  printf("Enemy HP: %3d    ATK: %3d  DEF: %3d\n", monster->health, monster->strength, monster->defense);
}
