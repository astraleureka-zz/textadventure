#include <stdio.h>
#include <string.h>
#include <dirent.h>
#include <assert.h>
#include <stdlib.h>
#include <errno.h>
#include <stdint.h>
#include <time.h>

#include "allocator.h"
#include "types.h"
#include "game.h"
#include "util.h"
#include "world.h"
#include "mob.h"
#include "player.h"
#include "combat.h"
#include "oop.h"

/* object prototype / function pointer definitions */
/*+ player class prototype +*/
object player_proto = {
  .init = player_init,
  .take_action = combat_generic,
  .recv_action = player_attack_receive,
  .move_action = player_move
};

/*+ game class prototype +*/
object game_proto = {
  .init = game_init
};

/*+ initializes the core game structures, loads the on-disk copies of each runtime structure and creates linkage between objects based on object id +*/
int game_init(void* self) /*+ pointer to called object +*/
/*+ returns 1 upon success, 0 upon failure +*/
{
  char* error;
  item_t** items      = malloc(sizeof(item_t*) * MAX_ITEMS);
  mob_t** mobs        = malloc(sizeof(mob_t*) * MAX_MOBS);
  room_t** rooms      = malloc(sizeof(room_t*) * MAX_ROOMS);
  game_t* game        = self;
  player_t* playerobj = NULL;

  alloc_register(items);
  alloc_register(mobs);
  alloc_register(rooms);

  /* explicitly cast the list pointers to void** to silence an irrelevant warning about incompatible types */
  if (! util_load_json_asset("items", (void**) items, sizeof(item_t*), MAX_ITEMS, item_proto, sizeof(item_t), item_json_unpack)) {
    error = "problems when loading items";
    goto ERROR;
  }

  if (! util_load_json_asset("mobs", (void**) mobs, sizeof(mob_t*), MAX_MOBS, mob_proto, sizeof(mob_t), mob_json_unpack)) {
    error = "problems when loading mobs";
    goto ERROR;
  }

  if (! util_load_json_asset("rooms", (void**) rooms, sizeof(room_t*), MAX_ROOMS, room_proto, sizeof(room_t), room_json_unpack)) {
    error = "problems when loading world";
    goto ERROR;
  }

  if (! room_linkage_create(rooms, mobs, items)) {
    error = "problems when linking rooms together";
    goto ERROR;
  }

  /* bootstrap the game object */
  assert(playerobj = NEW(player, "player"));
  playerobj->room_current = rooms[1];
  game->start             = rooms[1];
  game->player            = playerobj;

  return 1;

  ERROR:
  printf("FATAL - %s\n", error);
  exit(0);
}

/*+ main game logic function. accepts commands from the player and calls the respective object functions +*/
int process(game_t* game) /*+ pointer to called object +*/
{
  player_t* player    = game->player;
  room_t* room        = NULL;
  mob_t* mob            = NULL;
  int mob_will_defend = 0;
  int mob_will_attack = 0;

  if (player)
    room = player->room_current;
  if (room && room->mob)
    mob = room->mob;

  printf("\n> ");

  char ch = getchar();
  if (ch == -1) {
    printf("Really? You're quitting already?\n");
    return 0;
  }

  getchar(); /* eat LF */

  switch(ch) {
    case 'n':
      player->_(move_action)(player, NORTH);
      break;

    case 's':
      player->_(move_action)(player, SOUTH);
      break;

    case 'e':
      player->_(move_action)(player, EAST);
      break;

    case 'w':
      player->_(move_action)(player, WEST);
      break;

    case 'a':
      if (! mob) {
        printf("You swing your broadsword at the air. This makes you both look and feel rather foolish.\n");
        return 1;
      }
      else if (mob->health == 0) {
        printf("You poke %s with your broadsword. Unfortunately, %s lifeless corpse doesn't give much of a fight.\n", mob->name2, util_pronoun_get(mob->gender, U_PRONOUN_POSSESSIVE));
        return 1;
      }

      if (mob_will_defend) {
        mob->defense += mob_will_defend;
        printf("%s\n", mob_string_get(mob, MOB_STR_DEFEND));
      }

      if (player->_(take_action)(player, mob, TRUE))
        mob_will_attack = 0;

      if (mob_will_defend)
        mob->defense -= mob_will_defend;
      if (mob_will_attack)
        mob->_(take_action)(mob, player, FALSE);

      player_check(player);
      mob_check(mob);
      break;

    case 'l':
      printf("You have somehow forgotten where you are. You rub your eyes and look around.\n");
      room->_(describe)(room);
      break;

    case 'c':
      printf("You check yourself before you wreck yourself.\n");
      player_check(player);
      if (mob)
        mob_check(mob);
      break;

    case 'h':
      printf("'n', 's', 'e' or 'w' - move north, south, east or west.\n");
      printf("'a' - attack\n");
      printf("'l' - look around\n");
      printf("'c' - check stats\n");
      printf("'h' - this help message\n");
      break;

    default:
      printf("What? Speak up!\n");
  }

  return 1;
}

int main(int argc, char* argv[]) {
  alloc_register_cb();
  game_t* gameobj = NULL;

  assert(gameobj = NEW(game, ""));

  srand(time(NULL));

  printf("Welcome to the game!\n");
  gameobj->start->_(describe)(gameobj->start);

  while (process(gameobj)) {
    /* do nothing */
  }

  return 0;
}


