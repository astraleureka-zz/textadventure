#include <stdio.h>
#include <string.h>
#include <dirent.h>
#include <assert.h>
#include <stdlib.h>
#include <errno.h>
#include <stdint.h>
#include <time.h>
#include "game.h"

int room_init(void *self) {
  return 1;
}

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
