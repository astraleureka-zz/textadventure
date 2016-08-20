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
object monster_proto = {
  .init        = monster_init,
  .take_action = combat_generic,
  .recv_action = monster_attack_receive,
  .describe    = monster_describe
};

object room_proto = {
  .init     = room_init,
  .describe = room_describe
};

object player_proto = {
  .init = player_init,
  .take_action = combat_generic,
  .recv_action = player_attack_receive,
  .move_action = player_move
};

object item_proto = {
  .init     = item_init,
  .describe = item_describe
};

int game_init(void *self) {
  game *game                   = self;
  room **rooms                 = malloc(sizeof(room *) * MAX_ROOMS);
  monster **monsters           = malloc(sizeof(monster *) * MAX_MOBS);
  item **items                 = malloc(sizeof(item *) * MAX_ITEMS);
  room_frec **room_frecs       = malloc(sizeof(room_frec *) * MAX_ROOMS);
  monster_frec **monster_frecs = malloc(sizeof(monster_frec *) * MAX_MOBS);
  item_frec **item_frecs       = malloc(sizeof(item_frec *) * MAX_ITEMS);
  char *path_tmp               = malloc(256);
  player *playerobj            = NULL;
  room_frec room_frec_read;
  monster_frec monster_frec_read;
  item_frec item_frec_read;
  DIR *room_dh, *mob_dh, *item_dh;
  FILE *fh;
  struct dirent *d_entry;
  uint16_t i;
  uint8_t north_id, south_id, east_id, west_id, monster_id;

  /* register the persistent mallocs with our allocation cleanup handler */
  alloc_register(items);
  alloc_register(rooms);
  alloc_register(monsters);

  /* initialize the memory for the both file record structures and the runtime structures */
  /* we explicitly start from 0 here even though 0 is reserved, since we check against it */
  for (i = 0; i < MAX_MOBS; i++) {
    monster_frecs[i] = calloc(1, sizeof(monster_frec));
    monsters[i]      = calloc(1, sizeof(monster));
    assert(NULL != monster_frecs[i]);
    alloc_register(monsters[i]);
  }

  for (i = 0; i < MAX_ROOMS; i++) {
    room_frecs[i] = calloc(1, sizeof(room_frec));
    rooms[i]      = calloc(i, sizeof(room));
    assert(NULL != room_frecs[i]);
    alloc_register(rooms[i]);
  }

  for (i = 0; i < MAX_ITEMS; i++) {
    item_frecs[i] = calloc(1, sizeof(item_frec));
    items[i]      = calloc(i, sizeof(item));
    assert(NULL != item_frecs[i]);
    alloc_register(items[i]);
  }

  /* copy item records to runtime structs */
  item_dh = opendir("items");
  if (NULL == item_dh) {
    perror("opendir items");
    exit(0);
  }

  while (NULL != (d_entry = readdir(item_dh))) {
    if (*d_entry->d_name == '.') continue;
    if (strlen(d_entry->d_name) > 249) {
      printf("filename too long \n");
      continue;
    }
    sprintf(path_tmp, "items/%s", d_entry->d_name);
    fh = fopen(path_tmp, "r");
#ifdef DEBUG
    printf("fopen(%s, 'r') = %d\n", path_tmp, fh);
#endif
    if (NULL == fh) {
      sprintf(path_tmp, "fopen %s", path_tmp);
      perror(path_tmp);
      exit(0);
    }
    if (! fread(&item_frec_read, sizeof(item_frec), 1, fh)) {
      printf("fread %s too short, expected %d\n", path_tmp, sizeof(item_frec));
      exit(0);
    }

    if (item_frec_read.item_id == 0) continue; /* item id 0 means no item assigned */

    *item_frecs[item_frec_read.item_id] = item_frec_read;

    fclose(fh);
  }

  closedir(item_dh);

  /* copy file records to runtime struct */
  for (i = 1; i < MAX_ITEMS; i++) {
    if (! item_frecs[i]) continue;
    assert(items[i]        = NEW(item, item_frecs[i]->name));
    items[i]->name         = strdup(item_frecs[i]->name);
    items[i]->description  = strdup(item_frecs[i]->description);
    items[i]->health       = item_frecs[i]->health;
    items[i]->skill        = item_frecs[i]->skill;
    items[i]->strength     = item_frecs[i]->strength;
    items[i]->defense      = item_frecs[i]->defense;
    items[i]->celerity     = item_frecs[i]->celerity;
    items[i]->intelligence = item_frecs[i]->intelligence;
    memcpy(&items[i]->flags, &item_frecs[i]->flags, sizeof(item_flags)); /* has no member pointers */

    alloc_register(items[i]->name);
    alloc_register(items[i]->description);
  }

  /* scan the mobs dir and read all entries */
  mob_dh = opendir("mobs");
  if (NULL == mob_dh) {
    perror("opendir mobs");
    exit(0);
  }

  while (NULL != (d_entry = readdir(mob_dh))) {
    if (*d_entry->d_name == '.') continue;
    if (strlen(d_entry->d_name) > 249) {
      printf("filename too long \n");
      continue;
    }
    sprintf(path_tmp, "mobs/%s", d_entry->d_name);
    fh = fopen(path_tmp, "r");
#ifdef DEBUG
    printf("fopen(%s, 'r') = %d\n", path_tmp, fh);
#endif
    if (NULL == fh) {
      sprintf(path_tmp, "fopen %s", path_tmp);
      perror(path_tmp);
      exit(0);
    }
    if (! fread(&monster_frec_read, sizeof(monster_frec), 1, fh)) {
      printf("fread %s too short, expected %d\n", path_tmp, sizeof(monster_frec));
      exit(0);
    }

    if (monster_frec_read.monster_id == 0) continue; /* monster id 0 means no monster assigned */

    *monster_frecs[monster_frec_read.monster_id] = monster_frec_read;

    fclose(fh);
  }

  closedir(mob_dh);

  /* copy file records to runtime struct */
  for (i = 1; i < MAX_MOBS; i++) {
    if (! monster_frecs[i]) continue;
    assert(monsters[i]        = NEW(monster, monster_frecs[i]->name));
    monsters[i]->name         = strdup(monster_frecs[i]->name);
    monsters[i]->name2        = strdup(monster_frecs[i]->name2);
    monsters[i]->attack_str   = strdup(monster_frecs[i]->attack_str);
    monsters[i]->defend_str   = strdup(monster_frecs[i]->defend_str);
    monsters[i]->desc_str     = strdup(monster_frecs[i]->desc_str);
    monsters[i]->health       = monster_frecs[i]->health;
    monsters[i]->skill        = monster_frecs[i]->skill;
    monsters[i]->strength     = monster_frecs[i]->strength;
    monsters[i]->defense      = monster_frecs[i]->defense;
    monsters[i]->celerity     = monster_frecs[i]->celerity;
    monsters[i]->intelligence = monster_frecs[i]->intelligence;

    switch (monster_frecs[i]->gender) {
      case 0:
        monsters[i]->gender = GENDER_NONE;
        break;
      case 1:
        monsters[i]->gender = GENDER_FEMALE;
        break;
      case 2:
        monsters[i]->gender = GENDER_MALE;
        break;
      default:
        printf("monster %d bad gender %d\n", i, monster_frecs[i]->gender);
        exit(0);
    }

    if (monster_frecs[i]->item_id) {
      if (! items[ monster_frecs[i]->item_id ]) {
        printf("mob %d has bad item_id %d\n", i, monster_frecs[i]->item_id);
        exit(0);
      }

      monsters[i]->item_held = malloc(sizeof(item));
      memcpy(monsters[i]->item_held, items[ monster_frecs[i]->item_id ], sizeof(item));
      alloc_register(monsters[i]->item_held);
    }

    alloc_register(monsters[i]->name);
    alloc_register(monsters[i]->name2);
    alloc_register(monsters[i]->attack_str);
    alloc_register(monsters[i]->defend_str);
    alloc_register(monsters[i]->desc_str);
  }

  /* similar process for rooms, read all entries */
  room_dh = opendir("rooms");
  if (NULL == room_dh) {
    perror("opendir room_dh");
    exit(0);
  }

  while (NULL != (d_entry = readdir(room_dh))) {
    if (*d_entry->d_name == '.') continue;
    if (strlen(d_entry->d_name) > 249) {
      printf("filename too long \n");
      continue;
    }
    sprintf(path_tmp, "rooms/%s", d_entry->d_name);
    fh = fopen(path_tmp, "r");
#ifdef DEBUG
    printf("fopen(%s, 'r') = %d\n", path_tmp, fh);
#endif
    if (NULL == fh) {
      sprintf(path_tmp, "fopen %s", path_tmp);
      perror(path_tmp);
      exit(0);
    }
    if (! fread(&room_frec_read, sizeof(room_frec), 1, fh)) {
      printf("fread %s too short, expected %d\n", path_tmp, sizeof(room_frec));
      exit(0);
    }

    if (room_frec_read.room_id == 0) continue; /* 0 means no room, reserved */

    *room_frecs[room_frec_read.room_id] = room_frec_read;

    fclose(fh);
  }

  closedir(room_dh);

  /* first create all records, don't check mappings yet */
  for (i = 1; i < MAX_ROOMS; i++) {
    if (! room_frecs[i]) continue;
    assert(rooms[i] = NEW(room, room_frecs[i]->name));
    rooms[i]->name        = strdup(room_frecs[i]->name);
    rooms[i]->description = strdup(room_frecs[i]->description);
    alloc_register(rooms[i]->name);
    alloc_register(rooms[i]->description);
  }

  /* once all records are in place, we can match up mappings and truly check if they are valid */
  for (i = 1; i < MAX_ROOMS; i++) {
    if (! rooms[i]) continue;
    north_id = room_frecs[i]->north_id;
    south_id = room_frecs[i]->south_id;
    east_id  = room_frecs[i]->east_id;
    west_id  = room_frecs[i]->west_id;

    if (north_id && rooms[north_id]) {
      if (room_frecs[north_id]->south_id != i) {
        printf("room %d north_id %d does not correspond room %d south_id %d\n", i, north_id, north_id, room_frecs[north_id]->south_id);
        exit(0);
      }
      rooms[i]->north = rooms[north_id];
    }
    if (south_id && rooms[south_id]) {
      if (room_frecs[south_id]->north_id != i) {
        printf("room %d south_id %d does not correspond room %d north_id %d\n", i, south_id, south_id, room_frecs[south_id]->north_id);
        exit(0);
      }
      rooms[i]->south = rooms[south_id];
    }
    if (east_id && rooms[east_id]) {
      if (room_frecs[east_id]->west_id != i) {
        printf("room %d east_id %d does not correspond room %d west_id %d\n", 1, east_id, east_id, room_frecs[east_id]->west_id);
        exit(0);
      }
      rooms[i]->east = rooms[east_id];
    }
    if (west_id && rooms[west_id]) {
      if (room_frecs[west_id]->east_id != i) {
        printf("room %d west_id %d does not correspond room %d east_id %d\n", 1, west_id, west_id, room_frecs[west_id]->east_id);
        exit(0);
      }
      rooms[i]->west = rooms[west_id];
    }

    monster_id = room_frecs[i]->monster_id;
    if (monster_id == 0) continue; /* monster id 0 means no monster */

    if (monsters[monster_id]) {
      rooms[i]->monster = malloc(sizeof(monster));
      memcpy(rooms[i]->monster, monsters[monster_id], sizeof(monster));
      alloc_register(rooms[i]->monster);
    }
    else {
      printf("map %d monster_id %d does not exist\n", i, monster_id);
      exit(0);
    }
  }

  /* clean up the frecs allocations */
  for (i = 0; i < MAX_ITEMS; i++) {
    free(item_frecs[i]);
  }

  free(item_frecs);

  for (i = 0; i < MAX_ROOMS; i++) {
    free(room_frecs[i]);
  }

  free(room_frecs);

  for (i = 0; i < MAX_MOBS; i++) {
    free(monster_frecs[i]);
  }

  free(monster_frecs);

  free(path_tmp);

  /* finally bootstrap the game object */
  assert(playerobj = NEW(player, "player"));
  playerobj->room_current  = rooms[1];
  game->start              = rooms[1];
  game->player             = playerobj;

  return 1;
}

object game_proto = {
  .init = game_init
};

int process(game *game) {
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

