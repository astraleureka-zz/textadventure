#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <stdint.h>
#include "game.h"

int main(int argc, char *argv[]) {
  FILE *fh;

  if (argc < 11) {
    printf("usage: mobtool <output file> <mob_id> <health> <attack dmg> <defense> <name> <name2> <attack str> <defend str> <description>\n");
    printf("warning: this file will overwrite the output file without asking\n");
    return 1;
  }

  monster_frec *monster = malloc(sizeof(monster_frec));
  monster->monster_id   = atoi(argv[2]);
  monster->health       = atoi(argv[3]);
  monster->attack_dmg   = atoi(argv[4]);
  monster->defense      = atoi(argv[5]);
  snprintf(monster->name,       sizeof(monster->name),       "%s", argv[6]);
  snprintf(monster->name2,      sizeof(monster->name2),      "%s", argv[7]);
  snprintf(monster->attack_str, sizeof(monster->attack_str), "%s", argv[8]);
  snprintf(monster->defend_str, sizeof(monster->defend_str), "%s", argv[9]);
  snprintf(monster->desc_str,   sizeof(monster->desc_str),   "%s", argv[10]);

  fh = fopen(argv[1], "w");
  if (fh == NULL) {
    perror("fopen output file");
    return 1;
  }

  fwrite(monster, sizeof(monster_frec), 1, fh);
  fflush(fh);
  fclose(fh);

  return 0;
}

