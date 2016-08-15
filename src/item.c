#include <stdio.h>
#include <string.h>
#include <dirent.h>
#include <assert.h>
#include <stdlib.h>
#include <errno.h>
#include <stdint.h>
#include <time.h>
#include "game.h"

int item_init(void *self) {
  return 1;
}

void item_describe(void *self) {
  item *item = self;
}
