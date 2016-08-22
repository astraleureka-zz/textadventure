#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <errno.h>
#include <jansson.h>

#include "allocator.h"
#include "types.h"
#include "game.h"
#include "util.h"

object room_proto = {
  .init     = room_init,
  .describe = room_describe
};

int room_init(void* self) {
  return 1;
}

/*+ describes the room and its contents to the player +*/
void room_describe(void* self) {
  room_t* room = self;

  printf("You look around %s. %s\n", room->name, room->description);

  if (room->mob) {
    if (room->mob->health > 0) {
      printf("You spot %s just off in the distance.\n", room->mob->name);
      room->mob->_(describe)(room->mob);
      mob_check(room->mob);
      return; /* they can't leave the room */
    }
    else {
      printf("You see %s corpse resting in the middle of the room.\n", room->mob->name);
    }
  }

  printf("You can move in these directions: ");
  if (room->north) printf("north ");
  if (room->south) printf("south ");
  if (room->east) printf("east ");
  if (room->west) printf("west ");
  printf("\n");
}

/*+ loads data from json structure into mob structure +*/
boolean_t room_json_unpack(void* room_ptr,   /*+ target room structure +*/
                          json_t* json_room) /*+ source json structure +*/
/*+ returns TRUE if load was successful, FALSE otherwise +*/
{
  room_t* room = room_ptr;
  if (! room || ! json_room) return FALSE;

  room->name         = JSON_OBJECT_STRING(json_room, "name");
  room->description  = JSON_OBJECT_STRING(json_room, "description");
  room->north_id     = JSON_OBJECT_INTEGER(json_room, "north");
  room->south_id     = JSON_OBJECT_INTEGER(json_room, "south");
  room->east_id      = JSON_OBJECT_INTEGER(json_room, "east");
  room->west_id      = JSON_OBJECT_INTEGER(json_room, "west");
  room->item_id      = JSON_OBJECT_INTEGER(json_room, "item_id");
  room->mob_id       = JSON_OBJECT_INTEGER(json_room, "mob_id");

  alloc_register(room->name);
  alloc_register(room->description);

  return TRUE;
}

/*+ creates pointers between rooms and their respective objects +*/
boolean_t room_linkage_create(room_t** rooms, /*+ list of rooms to link +*/
                              mob_t** mobs,   /*+ list of mob objects +*/
                              item_t** items) /*+ list of item objects +*/
/*+ returns TRUE if linkage creation succeeded, otherwise FALSE +*/
{
  uint16_t i;
  uint8_t mob_id, north_id, south_id, east_id, west_id;

  if (! rooms || ! mobs || ! items) return FALSE;

  for (i = 0; i < MAX_ROOMS; i++) {
    if (! rooms[i]) continue;
    mob_id = rooms[i]->mob_id;

    if (mobs[mob_id]) {
      rooms[i]->mob = malloc(sizeof(mob_t));
      mob_copy(rooms[i]->mob, mobs[mob_id]);
      alloc_register(rooms[i]->mob);
    }
    else if (mob_id > 0) {
#ifdef DEBUG
      fprintf(stderr, "%s: mob_id %d does not exist\n", rooms[i]->_(class), mob_id);
#endif
      return FALSE;
    }

    north_id = rooms[i]->north_id;
    south_id = rooms[i]->south_id;
    east_id  = rooms[i]->east_id;
    west_id  = rooms[i]->west_id;

    if (north_id && rooms[north_id]) {
      if (rooms[north_id]->south_id != i) {
#ifdef DEBUG
        fprintf(stderr, "room %d north_id %d does not correspond room %d south_id %d\n", i, north_id, north_id, rooms[north_id]->south_id);
#endif
        return FALSE;
      }
      rooms[i]->north = rooms[north_id];
    }
    if (south_id && rooms[south_id]) {
      if (rooms[south_id]->north_id != i) {
#ifdef DEBUG
        fprintf(stderr, "room %d south_id %d does not correspond room %d north_id %d\n", i, south_id, south_id, rooms[south_id]->north_id);
#endif
        return FALSE;
      }
      rooms[i]->south = rooms[south_id];
    }
    if (east_id && rooms[east_id]) {
      if (rooms[east_id]->west_id != i) {
#ifdef DEBUG
        fprintf(stderr, "room %d east_id %d does not correspond room %d west_id %d\n", 1, east_id, east_id, rooms[east_id]->west_id);
#endif
        return FALSE;
      }
      rooms[i]->east = rooms[east_id];
    }
    if (west_id && rooms[west_id]) {
      if (rooms[west_id]->east_id != i) {
#ifdef DEBUG
        fprintf(stderr, "room %d west_id %d does not correspond room %d east_id %d\n", 1, west_id, west_id, rooms[west_id]->east_id);
#endif
        return FALSE;
      }
      rooms[i]->west = rooms[west_id];
    }
  }

  return TRUE;
}

