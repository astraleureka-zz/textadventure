#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include "allocator.h"
#include "types.h"
#include "game.h"
#include "util.h"

object room_proto = {
  .init     = room_init,
  .describe = room_describe
};

int room_init(void *self) {
  return 1;
}

/*+ describes the room and its contents to the player +*/
void room_describe(void *self) {
  room *room = self;

  printf("You look around %s. %s\n", room->name, room->description);

  if (room->monster) {
    if (room->monster->health > 0) {
      printf("You spot %s just off in the distance.\n", room->monster->name);
      room->monster->_(describe)(room->monster);
      monster_check(room->monster);
      return; /* they can't leave the room */
    }
    else {
      printf("You see %s corpse resting in the middle of the room.\n", room->monster->name);
    }
  }

  printf("You can move in these directions: ");
  if (room->north) printf("north ");
  if (room->south) printf("south ");
  if (room->east) printf("east ");
  if (room->west) printf("west ");
  printf("\n");
}

room** room_load(monster** monsters, item** items) {
  room** rooms           = malloc(sizeof(room *) * MAX_ROOMS);
  room_frec **room_frecs = malloc(sizeof(room_frec *) * MAX_ROOMS);
  uint16_t i             = 0;
  uint8_t north_id, south_id, east_id, west_id, monster_id;

  assert(NULL != rooms);
  assert(NULL != room_frecs);

  alloc_register(rooms);
  alloc_register(room_frecs);

  for (i = 0; i < MAX_ITEMS; i++) {
    room_frecs[i] = malloc(sizeof(room_frec));
    rooms[i]      = malloc(sizeof(room));
    memset(room_frecs[i], 0, sizeof(room_frec));
    alloc_register(rooms[i]);
  }

  util_frec_load("rooms", (void**) room_frecs, sizeof(room_frec));

  /* first create all records, don't check mappings yet */
  for (i = 1; i < MAX_ROOMS; i++) {
    if (! room_frecs[i]->room_id) continue;
    assert(rooms[i] = NEW(room, room_frecs[i]->name));
    rooms[i]->name        = strdup(room_frecs[i]->name);
    rooms[i]->description = strdup(room_frecs[i]->description);
    alloc_register(rooms[i]->name);
    alloc_register(rooms[i]->description);
  }

  /* once all records are in place, we can match up mappings and truly check if they are valid */
  for (i = 1; i < MAX_ROOMS; i++) {
    if (! rooms[i]) continue;
    north_id = room_frecs[i]->north_id;
    south_id = room_frecs[i]->south_id;
    east_id  = room_frecs[i]->east_id;
    west_id  = room_frecs[i]->west_id;

    if (north_id && rooms[north_id]) {
      if (room_frecs[north_id]->south_id != i) {
        printf("room %d north_id %d does not correspond room %d south_id %d\n", i, north_id, north_id, room_frecs[north_id]->south_id);
        exit(0);
      }
      rooms[i]->north = rooms[north_id];
    }
    if (south_id && rooms[south_id]) {
      if (room_frecs[south_id]->north_id != i) {
        printf("room %d south_id %d does not correspond room %d north_id %d\n", i, south_id, south_id, room_frecs[south_id]->north_id);
        exit(0);
      }
      rooms[i]->south = rooms[south_id];
    }
    if (east_id && rooms[east_id]) {
      if (room_frecs[east_id]->west_id != i) {
        printf("room %d east_id %d does not correspond room %d west_id %d\n", 1, east_id, east_id, room_frecs[east_id]->west_id);
        exit(0);
      }
      rooms[i]->east = rooms[east_id];
    }
    if (west_id && rooms[west_id]) {
      if (room_frecs[west_id]->east_id != i) {
        printf("room %d west_id %d does not correspond room %d east_id %d\n", 1, west_id, west_id, room_frecs[west_id]->east_id);
        exit(0);
      }
      rooms[i]->west = rooms[west_id];
    }

    monster_id = room_frecs[i]->monster_id;
    if (monster_id == 0) continue; /* monster id 0 means no monster */

    if (monsters[monster_id]) {
      rooms[i]->monster = malloc(sizeof(monster));
      memcpy(rooms[i]->monster, monsters[monster_id], sizeof(monster));
      alloc_register(rooms[i]->monster);
    }
    else {
      printf("map %d monster_id %d does not exist\n", i, monster_id);
      exit(0);
    }
  }

  for (i = 0; i < MAX_ROOMS; i++) {
    free(room_frecs[i]);
  }
  free(room_frecs);

  return rooms;
}

