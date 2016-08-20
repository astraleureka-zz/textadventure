#ifndef _WORLD_H
#define _WORLD_H 1

#include "types.h"
#include "mob.h"
#include "item.h"

/* Room definitions */
struct room {
  object proto;

  monster *monster;
  item *item;

  struct room *north;
  struct room *south;
  struct room *east;
  struct room *west;

  char *description;
  char *name;
};

typedef struct room room;

struct room_frec {
  uint8_t room_id;              /* 1 */
  uint8_t monster_id;           /* 2 */
  uint8_t item_id;              /* 3 */
  uint8_t north_id;             /* 4 */
  uint8_t south_id;             /* 5 */
  uint8_t east_id;              /* 6 */
  uint8_t west_id;              /* 7 */
  uint8_t reserved[8];          /* 16 */
  char name[MAX_NAMELEN];       /* 48 */
  char description[MAX_STRLEN]; /* 304 */
};

typedef struct room_frec room_frec;

int room_init(void *self);
void room_describe(void *self);
room** room_load(monster** monsters, item** items);

#endif
