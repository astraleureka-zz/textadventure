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
int game_init(void *self) /*+ pointer to called object +*/
/*+ returns 1 upon success, 0 upon failure +*/
{
  game *game                   = self;
  player *playerobj            = NULL;
  room **rooms;
  monster **monsters;
  item **items;

  items    = item_load();
  monsters = monster_load(items);
  rooms    = room_load(monsters, items);

  /* bootstrap the game object */
  assert(playerobj = NEW(player, "player"));
  playerobj->room_current  = rooms[1];
  game->start              = rooms[1];
  game->player             = playerobj;

  return 1;
}

/*+ main game logic function. accepts commands from the player and calls the respective object functions +*/
int process(game *game) /*+ pointer to called object +*/
{
  player *player          = game->player;
  room *room              = NULL;
  monster *monster        = NULL;
  int monster_will_defend = 0;
  int monster_will_attack = 0;

  if (player)
    room = player->room_current;
  if (room && room->monster)
    monster = room->monster;

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
      if (! monster) {
        printf("You swing your broadsword at the air. This makes you both look and feel rather foolish.\n");
        return 1;
      }
      else if (monster->health == 0) {
        printf("You poke %s with your broadsword. Unfortunately, %s lifeless corpse doesn't give much of a fight.\n", monster->name2, util_pronoun_get(monster->gender, U_PRONOUN_POSSESSIVE));
        return 1;
      }

      if (monster_will_defend) {
        monster->defense += monster_will_defend;
        printf("The %s %s.\n", monster->name2, monster->defend_str);
      }

      if (player->_(take_action)(player, monster, TRUE))
        monster_will_attack = 0;

      if (monster_will_defend)
        monster->defense -= monster_will_defend;
      if (monster_will_attack)
        monster->_(take_action)(monster, player, FALSE);

      player_check(player);
      monster_check(monster);
      break;

    case 'l':
      printf("You have somehow forgotten where you are. You rub your eyes and look around.\n");
      room->_(describe)(room);
      break;

    case 'c':
      printf("You check yourself before you wreck yourself.\n");
      player_check(player);
      if (monster)
        monster_check(monster);
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

int main(int argc, char *argv[]) {
  alloc_register_cb();
  game *gameobj = NULL;

  assert(gameobj = NEW(game, ""));

  srand(time(NULL));

  printf("Welcome to the game!\n");
  gameobj->start->_(describe)(gameobj->start);

  while (process(gameobj)) {
    /* do nothing */
  }

  return 0;
}

