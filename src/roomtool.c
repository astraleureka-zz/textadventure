#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "types.h"
#include "game.h"

int main(int argc, char *argv[]) {
  FILE *fh;

  if (argc < 9) {
    printf("usage: roomtool <output file> <room_id> <monster_id> <north_id> <south_id> <east_id> <west_id> <name> <description>\n");
    printf("warning: this file will overwrite the output file without asking\n");
    return 1;
  }

  room_frec *room   = malloc(sizeof(room_frec));
  room->room_id     = atoi(argv[2]);
  room->monster_id  = atoi(argv[3]);
  room->north_id    = atoi(argv[4]);
  room->south_id    = atoi(argv[5]);
  room->east_id     = atoi(argv[6]);
  room->west_id     = atoi(argv[7]);
  snprintf(room->name,        sizeof(room->name),        "%s", argv[8]);
  snprintf(room->description, sizeof(room->description), "%s", argv[9]);

  fh = fopen(argv[1], "w");
  if (fh == NULL) {
    perror("fopen output file");
    return 1;
  }

  fwrite(room, sizeof(room_frec), 1, fh);
  fflush(fh);
  fclose(fh);

  return 0;
}

