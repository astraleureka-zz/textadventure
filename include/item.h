#ifndef _ITEM_H
#define _ITEM_H 1

#include "types.h"
#include "game.h"

/* Item definitions */
struct item_flags {
  boolean_t can_use:1;
  boolean_t can_eat:1;
  boolean_t can_equip:1;
  boolean_t can_throw:1;
  boolean_t is_unique:1;
  int :26;
  boolean_t reserved:1;
};

typedef struct item_flags item_flags;

struct item {
  object proto;

  item_flags flags;
  uint8_t health;
  uint8_t skill;
  uint8_t strength;
  uint8_t defense;
  uint8_t celerity;
  uint8_t intelligence;
  uint8_t reserved[7];
  char *name;
  char *description;
};

typedef struct item item;

struct item_frec {
  uint8_t item_id;

  item_flags flags;
  uint8_t health;
  uint8_t skill;
  uint8_t strength;
  uint8_t defense;
  uint8_t celerity;
  uint8_t intelligence;
  uint8_t reserved[7];
  char name[MAX_NAMELEN];
  char description[MAX_STRLEN];
};

typedef struct item_frec item_frec;

int item_init(void *self);
void item_describe(void *self);
item** item_load(void);

#endif
