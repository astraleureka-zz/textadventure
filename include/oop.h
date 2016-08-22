#ifndef _OOP_H
#define _OOP_H 1

#include <stddef.h>

/*+ root prototype for all objects +*/
typedef struct {
  char* class;                                                                 /*+ string to identify the object root type +*/
  int (*init)(void* self);                                                     /*+ func ptr - initializer +*/
  void (*destroy)(void* self);                                                 /*+ func ptr - destructor +*/
  void (*describe)(void* self);                                                /*+ func ptr - outputs a description of the object to the console +*/
  boolean_t (*take_action)(void* self, void* target, boolean_t player_action); /*+ func ptr - takes an action upon a specified target (generally applies to player/mob only) +*/
  int (*recv_action)(void* self, uint8_t parameter);                           /*+ func ptr - receive an action (i.e. take damage after applying inventory bonuses/etc) +*/
  void (*move_action)(void* self, direction dir);                              /*+ func ptr - move object from one room to another (generally applies to player only) +*/
} object;

typedef struct {
  object proto;
} object_generic;

int object_init(void* self);
void object_destroy(void* self);
void object_describe(void* self);
boolean_t take_action(void* self, void* target, boolean_t player_action);
int recv_action(void* self, uint8_t parameter);
void move_action(void* self, direction dir);
void* object_new(size_t size, object proto, char* class);

#define NEW(T, N) object_new(sizeof(struct T), T##_proto, N)
#define _(N) proto.N

#endif
