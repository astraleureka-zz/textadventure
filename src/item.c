#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include "allocator.h"
#include "types.h"
#include "game.h"
#include "util.h"

object item_proto = {
  .init     = item_init,
  .describe = item_describe
};

int item_init(void *self) {
  return 1;
}

void item_describe(void *self) {
  item *item = self;
}

item** item_load(void) {
  item** items           = malloc(sizeof(item *) * MAX_ITEMS);
  item_frec **item_frecs = malloc(sizeof(item_frec *) * MAX_ITEMS);
  uint16_t i             = 0;

  assert(NULL != items);
  assert(NULL != item_frecs);

  alloc_register(items);
  alloc_register(item_frecs);

  for (i = 0; i < MAX_ITEMS; i++) {
    item_frecs[i] = malloc(sizeof(item_frec));
    items[i]      = malloc(sizeof(item));
    memset(item_frecs[i], 0, sizeof(item_frec));
    alloc_register(items[i]);
  }

  util_frec_load("items", (void**) item_frecs, sizeof(item_frec));

  /* copy file records to runtime struct */
  for (i = 1; i < MAX_ITEMS; i++) {
    if (! item_frecs[i]->item_id) continue;
    assert(items[i]        = NEW(item, item_frecs[i]->name));
    items[i]->name         = strdup(item_frecs[i]->name);
    items[i]->description  = strdup(item_frecs[i]->description);
    items[i]->health       = item_frecs[i]->health;
    items[i]->skill        = item_frecs[i]->skill;
    items[i]->strength     = item_frecs[i]->strength;
    items[i]->defense      = item_frecs[i]->defense;
    items[i]->celerity     = item_frecs[i]->celerity;
    items[i]->intelligence = item_frecs[i]->intelligence;
    memcpy(&items[i]->flags, &item_frecs[i]->flags, sizeof(item_flags)); /* has no member pointers */

    alloc_register(items[i]->name);
    alloc_register(items[i]->description);
  }

  for (i = 0; i < MAX_ITEMS; i++) {
    free(item_frecs[i]);
  }
  free(item_frecs);

  return items;
}


