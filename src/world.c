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

/*+ loads all room files in rooms/ +*/
boolean_t room_load(room_t** rooms, /*+ pointer to empty room list +*/
                    mob_t** mobs,   /*+ pointer to mob list, used to create mob copies per room +*/
                    item_t** items) /*+ pointer to item list, used to create item copies per room +*/
/*+ returns TRUE on success, FALSE on failure +*/
{
  DIR* dh;
  struct dirent* d;
  char* path_tmp     = malloc(MAX_STRLEN);
  uint16_t i         = 0;
  /* per loop vars */
  uint8_t north_id, south_id, east_id, west_id, mob_id, item_id;
  size_t idx;
  json_t* json_room;
  json_error_t json_error;
  uint8_t room_id;

  assert(NULL != rooms);
  assert(NULL != path_tmp);

  memset(rooms, 0, sizeof(room_t*) * MAX_ROOMS);

  dh = opendir("rooms");
  if (NULL == dh) {
    perror("opendir rooms");
    goto ERROR;
  }

  while (NULL != (d = readdir(dh))) {
    if (*d->d_name == '.') continue; /* skip any lines starting with . */

    if ((strlen(d->d_name) + 6) > MAX_STRLEN) { /* 6 = "rooms/" */
      printf("rooms/%s - path too long\n", d->d_name);
      goto ERROR;
    }

    snprintf(path_tmp, MAX_STRLEN - 1, "rooms/%s", d->d_name);
    if (NULL == (json_room = json_load_file(path_tmp, 0, &json_error))) {
      printf("%s - error line %d: %s\n", path_tmp, json_error.line, json_error.text);
      goto ERROR;
    }

    if (! json_is_object(json_room)) {
      printf("%s - json is not an object\n", path_tmp);
      goto ERROR;
    }

    room_id = JSON_OBJECT_INTEGER(json_room, "id");
    if (rooms[room_id]) {
      printf("%s - duplicate room id %d (already assigned to %s)\n", path_tmp, room_id, rooms[room_id]->_(class));
      goto ERROR;
    }

    assert(rooms[room_id]        = NEW(room, path_tmp));
    rooms[room_id]->name         = JSON_OBJECT_STRING(json_room, "name");
    rooms[room_id]->description  = JSON_OBJECT_STRING(json_room, "description");
    rooms[room_id]->north_id     = JSON_OBJECT_INTEGER(json_room, "north");
    rooms[room_id]->south_id     = JSON_OBJECT_INTEGER(json_room, "south");
    rooms[room_id]->east_id      = JSON_OBJECT_INTEGER(json_room, "east");
    rooms[room_id]->west_id      = JSON_OBJECT_INTEGER(json_room, "west");

    mob_id                       = JSON_OBJECT_INTEGER(json_room, "mob_id");
    if (mobs[mob_id]) {
      rooms[room_id]->mob = malloc(sizeof(mob_t));
      mob_copy(rooms[room_id]->mob, mobs[mob_id]);
      alloc_register(rooms[room_id]->mob);
    }
    else if (mob_id > 0) {
      printf("%s: mob_id %d does not exist\n", path_tmp, mob_id);
      goto ERROR;
    }

    alloc_register(rooms[room_id]->name);
    alloc_register(rooms[room_id]->description);
    json_decref(json_room);
  }

  for (i = 1; i < MAX_ROOMS; i++) {
    if (! rooms[i]) continue;
    north_id = rooms[i]->north_id;
    south_id = rooms[i]->south_id;
    east_id  = rooms[i]->east_id;
    west_id  = rooms[i]->west_id;

    if (north_id && rooms[north_id]) {
      if (rooms[north_id]->south_id != i) {
        printf("room %d north_id %d does not correspond room %d south_id %d\n", i, north_id, north_id, rooms[north_id]->south_id);
        exit(0);
      }
      rooms[i]->north = rooms[north_id];
    }
    if (south_id && rooms[south_id]) {
      if (rooms[south_id]->north_id != i) {
        printf("room %d south_id %d does not correspond room %d north_id %d\n", i, south_id, south_id, rooms[south_id]->north_id);
        exit(0);
      }
      rooms[i]->south = rooms[south_id];
    }
    if (east_id && rooms[east_id]) {
      if (rooms[east_id]->west_id != i) {
        printf("room %d east_id %d does not correspond room %d west_id %d\n", 1, east_id, east_id, rooms[east_id]->west_id);
        exit(0);
      }
      rooms[i]->east = rooms[east_id];
    }
    if (west_id && rooms[west_id]) {
      if (rooms[west_id]->east_id != i) {
        printf("room %d west_id %d does not correspond room %d east_id %d\n", 1, west_id, west_id, rooms[west_id]->east_id);
        exit(0);
      }
      rooms[i]->west = rooms[west_id];
    }
  }

  if (path_tmp) free(path_tmp);
  if (dh) closedir(dh);
  return TRUE;

  ERROR:
  if (path_tmp) free(path_tmp);
  if (dh) closedir(dh);
  if (json_room) json_decref(json_room);
  return FALSE;
}

