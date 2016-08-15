#ifndef _ITEM_H
#define _ITEM_H 1

#include <stdint.h>
#include "oop.h"
#include "game.h"

#define MAX_ITEMS 255

/* Item definitions */
struct item {
  object proto;

  char *name;
  char *description;
  struct flags {
    boolean_t can_use:1;
    boolean_t can_eat:1;
    boolean_t can_equip:1;
    boolean_t can_throw:1;
    boolean_t is_unique:1;
    int :26;
    boolean_t reserved:1;
  } flags;
};

typedef struct item item;

int item_init(void *self);
void item_describe(void *self);

#endif
