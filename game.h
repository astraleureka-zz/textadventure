#ifndef _GAME_H
#define _GAME_H 1

#include <stdint.h>
#include "oop.h"

#define MAX_ROOMS 255
#define MAX_MOBS 255
#define MAX_NAMELEN 31
#define MAX_STRLEN 255

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

/* Room definitions */
struct room {
  object proto;

  monster *monster;

  struct room *north;
  struct room *south;
  struct room *east;
  struct room *west;

  char *description;
  char *name;
};

typedef struct room room;

struct room_frec {
  uint8_t room_id;
  uint8_t monster_id;
  uint8_t north_id;
  uint8_t south_id;
  uint8_t east_id;
  uint8_t west_id;
  char name[MAX_NAMELEN];
  char description[MAX_STRLEN];
};

typedef struct room_frec room_frec;

int room_init(void *self);
void room_describe(void *self);

/* Player definitions */
typedef struct {
  object proto;

  room *room_current;

  uint8_t health;
  uint8_t attack_dmg;
  uint8_t defense;
} player;

int player_init(void *self);
uint8_t player_attack(void *self, void *target);
int player_attack_receive(void *self, uint8_t damage);
void player_move(void *self, direction dir);

/* game definitions */
typedef struct {
  object proto;

  room *start;
  player *player;
} game;

int game_init(void *self);

#endif
