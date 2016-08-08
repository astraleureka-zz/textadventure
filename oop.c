#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <stdlib.h>
#include "oop.h"

void object_destroy(void *self) {
  object *obj = self;

  if (obj)
    free(obj);
}

void object_describe(void *self) {
  printf("cannot describe a root object\n");
}

int object_init(void *self) {
  return 1;
}

uint8_t object_take_action(void *self, void *target) {
  printf("cannot take action as a root object\n");
  return 0;
}

int object_recv_action(void *self, uint8_t parameter) {
  printf("cannot recv action as a root object\n");
  return 0;
}

void object_move_action(void *self, direction dir) {
  printf("cannot move action as a root object\n");
}

void *object_new(size_t size, object proto, char *class) {
  if (! proto.init)
    proto.init = object_init;

  if (! proto.destroy)
    proto.destroy = object_destroy;

  if (! proto.describe)
    proto.describe = object_describe;

  if (! proto.take_action)
    proto.take_action = object_take_action;

  if (! proto.recv_action)
    proto.recv_action = object_recv_action;

  if (! proto.move_action)
    proto.move_action = object_move_action;

  object *new = calloc(1, size);
  *new = proto; /* assign base object */

  new->class = strdup(class);

  if (! new->init(new)) {
    printf("object_new(%u, object) failed\n", size);

    new->destroy(new);
    return NULL;
  }

  return new;
}
