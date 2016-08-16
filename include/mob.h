#ifndef _MOB_H
#define _MOB_H 1

#include <stdint.h>
#include "oop.h"

#define MAX_MOBS 256

/* Monster definitions */
struct monster {
  object proto;

  char *name; /* "a demon", "Bahamut", "the final boss" */
  char *name2; /* "the demon", "Bahamut", "the final boss" */
  char *attack_str; /* "launches a magic missile towards you", "slashes at you with its beastly talons", "lunges forth with a great leap, clashing his broadsword against your own" */
  char *defend_str; /* "looks at you eye to eye, ready to absorb the impact of your blade", "hovers nearby, watching your every move", "leaps back in anticipation of your next attack" */
  char *desc_str;
  uint8_t health;
  uint8_t attack_dmg;
  uint8_t defense;
};

typedef struct monster monster;

struct monster_frec {
  uint8_t monster_id;
  uint8_t health;
  uint8_t attack_dmg;
  uint8_t defense;
  char name[MAX_NAMELEN];
  char name2[MAX_NAMELEN];
  char attack_str[MAX_STRLEN];
  char defend_str[MAX_STRLEN];
  char desc_str[MAX_STRLEN];
};

typedef struct monster_frec monster_frec;

int monster_init(void *self);
uint8_t monster_attack(void *self, void *target);
int monster_attack_receive(void *self, uint8_t damage);
void monster_describe(void *self);
void monster_check(monster *monster);

#endif
