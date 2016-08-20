#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include "allocator.h"
#include "types.h"
#include "game.h"
#include "combat.h"
#include "util.h"

/*+ monster class prototype +*/
object monster_proto = {
  .init        = monster_init,
  .take_action = combat_generic,
  .recv_action = monster_attack_receive,
  .describe    = monster_describe
};

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

monster** monster_load(item** items) {
  monster** monsters           = malloc(sizeof(monster *) * MAX_MOBS);
  monster_frec **monster_frecs = malloc(sizeof(monster_frec *) * MAX_MOBS);
  uint16_t i                   = 0;

  assert(NULL != monsters);
  assert(NULL != monster_frecs);

  alloc_register(monsters);
  alloc_register(monster_frecs);

  for (i = 0; i < MAX_MOBS; i++) {
    monster_frecs[i] = malloc(sizeof(monster_frec));
    monsters[i]      = malloc(sizeof(monster));
    memset(monster_frecs[i], 0, sizeof(monster_frec));
    alloc_register(monsters[i]);
  }

  util_frec_load("mobs", (void**) monster_frecs, sizeof(monster_frec));

  /* copy file records to runtime struct */
  for (i = 1; i < MAX_MOBS; i++) {
    if (! monster_frecs[i]->monster_id) continue;
    assert(monsters[i]        = NEW(monster, monster_frecs[i]->name));
    monsters[i]->name         = strdup(monster_frecs[i]->name);
    monsters[i]->name2        = strdup(monster_frecs[i]->name2);
    monsters[i]->attack_str   = strdup(monster_frecs[i]->attack_str);
    monsters[i]->defend_str   = strdup(monster_frecs[i]->defend_str);
    monsters[i]->desc_str     = strdup(monster_frecs[i]->desc_str);
    monsters[i]->health       = monster_frecs[i]->health;
    monsters[i]->skill        = monster_frecs[i]->skill;
    monsters[i]->strength     = monster_frecs[i]->strength;
    monsters[i]->defense      = monster_frecs[i]->defense;
    monsters[i]->celerity     = monster_frecs[i]->celerity;
    monsters[i]->intelligence = monster_frecs[i]->intelligence;

    switch (monster_frecs[i]->gender) {
      case 0:
        monsters[i]->gender = GENDER_NONE;
        break;
      case 1:
        monsters[i]->gender = GENDER_FEMALE;
        break;
      case 2:
        monsters[i]->gender = GENDER_MALE;
        break;
      default:
        printf("monster %d bad gender %d\n", i, monster_frecs[i]->gender);
        exit(0);
    }

    if (monster_frecs[i]->item_id) {
      if (! items[ monster_frecs[i]->item_id ]) {
        printf("mob %d has bad item_id %d\n", i, monster_frecs[i]->item_id);
        exit(0);
      }

      monsters[i]->item_held = malloc(sizeof(item));
      memcpy(monsters[i]->item_held, items[ monster_frecs[i]->item_id ], sizeof(item));
      alloc_register(monsters[i]->item_held);
    }

    alloc_register(monsters[i]->name);
    alloc_register(monsters[i]->name2);
    alloc_register(monsters[i]->attack_str);
    alloc_register(monsters[i]->defend_str);
    alloc_register(monsters[i]->desc_str);
  }

  for (i = 0; i < MAX_MOBS; i++) {
    free(monster_frecs[i]);
  }
  free(monster_frecs);

  return monsters;
}

