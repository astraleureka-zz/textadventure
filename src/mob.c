#include <stdio.h>

#include "types.h"
#include "game.h"

int monster_init(void *self) {
  return 1;
}

/*+ handles receiving an attack +*/
int monster_attack_receive(void *self,     /*+ pointer to called object +*/
                           uint8_t damage) /*+ raw amount damage received +*/
/*+ returns 1 if mob will die from this attack, otherwise 0 if mob continues to live +*/
{
  monster *monster = self;

  if (damage >= monster->health)
    return 1;

  monster->health -= damage;
  return 0;
}

/*+ outputs the description of the mob to the console +*/
void monster_describe(void *self) {
  monster *monster = self;

  printf("%s\n", monster->desc_str);
}

/*+ outputs the mob's current health and stats to the console +*/
void monster_check(monster *monster) {
  printf("Enemy HP: %3d    ATK: %3d  DEF: %3d\n", monster->health, monster->strength, monster->defense);
}
