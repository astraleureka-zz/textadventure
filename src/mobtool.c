#include <stdio.h>
#include <stdlib.h>

#include "types.h"
#include "game.h"

int main(int argc, char *argv[]) {
  FILE *fh;

  if (argc < 16) {
    printf("usage: mobtool <output file> <mob_id> <gender 0=n,1=f,2=m> <health> <skill pts> <strength> <defense> <celerity> <intelligence> <item id or 0> <name> <name2> <attack str> <defend str> <description>\n");
    printf("warning: this file will overwrite the output file without asking\n");
    return 1;
  }

  monster_frec *monster = malloc(sizeof(monster_frec));
  monster->monster_id   = atoi(argv[2]);
  monster->gender       = atoi(argv[3]);
  monster->health       = atoi(argv[4]);
  monster->skill        = atoi(argv[5]);
  monster->strength     = atoi(argv[6]);
  monster->defense      = atoi(argv[7]);
  monster->celerity     = atoi(argv[8]);
  monster->intelligence = atoi(argv[9]);
  monster->item_id      = atoi(argv[10]);
  snprintf(monster->name,       sizeof(monster->name),       "%s", argv[11]);
  snprintf(monster->name2,      sizeof(monster->name2),      "%s", argv[12]);
  snprintf(monster->attack_str, sizeof(monster->attack_str), "%s", argv[13]);
  snprintf(monster->defend_str, sizeof(monster->defend_str), "%s", argv[14]);
  snprintf(monster->desc_str,   sizeof(monster->desc_str),   "%s", argv[15]);

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

