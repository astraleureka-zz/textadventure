#include <stdio.h>
#include <stdlib.h>

#include "types.h"
#include "game.h"

/*+ Calculate the amount of damage taken, given specified strength and defense parameters. +*/
uint8_t combat_damage_calculate(uint8_t str, /*+ Strength (of actor) +*/
                                uint8_t def) /*+ Defense (of target) +*/
/*+ returns the raw damage value +*/
{
  uint8_t str_base = (str + ((rand() % str) * 3 / 2));
  uint8_t def_base = ((def / 2) + ((rand() % def) / 2));
  if (def_base == 0) {
    printf("Critical hit!\n");
    return str_base;
  }
  uint8_t damage   = (str_base / def_base);
#ifdef DEBUG
  printf("combat_damage_calculate(%d, %d): %d, %d == %d\n", str, def, str_base, def_base, damage);
#endif
  return damage;
}


/*+++++++++++
  This function handles all combat interaction between an actor and target object

  void* actor Pointer to the actor object (expected: player or mob)

  void* target Pointer to the target object (expected: player or mob)

  boolean_t player_action Identifies whether the actor is the player or a mob. TRUE means actor is of type player.
  +++++++++++*/

/*+ handles all combat interaction between an actor and target object +*/
boolean_t combat_generic(void* actor,             /*+ pointer to actor object (usually player or mob) +*/
                         void* target,            /*+ pointer to target object (usually player or mob) +*/
                         boolean_t player_action) /*+ flag to identify whether actor is player or not. if TRUE, actor's type is player* +*/
/*+ returns TRUE when combat resulted in target's death +*/
{
  uint8_t str, def, dmg;
  player_t* pl;
  mob_t* mob;

  if (TRUE == player_action) {
    pl  = actor;
    mob = target;
    str = pl->strength;
    def = mob->defense;
  }

  else {
    pl  = target;
    mob = actor;
    str = mob->strength;
    def = pl->defense;
  }

  dmg = combat_damage_calculate(str, def);

  if (TRUE == player_action) {
    printf("You attack %s! It takes %d damage.\n", mob->name2, dmg);

    if (mob->_(recv_action)(mob, dmg)) {
      printf("You have slain %s!\n", mob->name2);
      mob->health = 0;
      return TRUE;
    }
  }

  else {
    printf("%s You take %d damage.\n", mob_string_get(mob, MOB_STR_ATTACK), dmg);

    if (pl->_(recv_action)(pl, dmg)) {
      printf("You have died!\n");
      exit(0);
    }
  }

  return FALSE;
}
