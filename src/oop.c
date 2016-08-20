#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <stdlib.h>

#include "types.h"
#include "allocator.h"
#include "oop.h"

/*+ default function for object creation +*/
void object_describe(void *self) {
  printf("cannot describe a root object\n");
}

/*+ default function for object creation +*/
int object_init(void *self) {
  return 1;
}

/*+ default function for object creation +*/
boolean_t object_take_action(void *self, void *target, boolean_t player_action) {
  printf("cannot take action as a root object\n");
  return 0;
}

/*+ default function for object creation +*/
int object_recv_action(void *self, uint8_t parameter) {
  printf("cannot recv action as a root object\n");
  return 0;
}

/*+ default function for object creation +*/
void object_move_action(void *self, direction dir) {
  printf("cannot move action as a root object\n");
}

/*+ creates a new object pointer from the specified prototype object.  +*/
void *object_new(size_t size,  /*+ size of the prototype object +*/
                 object proto, /*+ the actual prototype object, contains function pointers to object functions +*/
                 char *class)  /*+ the name of the object's class, as a string +*/
{
  object *new;

  if (! proto.init)
    proto.init = object_init;

  if (! proto.describe)
    proto.describe = object_describe;

  if (! proto.take_action)
    proto.take_action = object_take_action;

  if (! proto.recv_action)
    proto.recv_action = object_recv_action;

  if (! proto.move_action)
    proto.move_action = object_move_action;

  assert(new = calloc(1, size));
  *new = proto; /* assign base object */

  new->class = strdup(class);

  /*+ after creating the new object, we call its initializer. if the initializer returns FALSE, the object is considered to be uninitialized and we discard it +*/
  if (! new->init(new)) {
    printf("object_new(%u, object) failed\n", size);
    free(new->class);
    free(new);
    return NULL;
  }

  alloc_register(new);
  alloc_register(new->class);
  return new;
}
