#include <stdio.h>
#include <string.h>
#include <dirent.h>
#include <assert.h>
#include <stdlib.h>
#include <errno.h>
#include <stdint.h>
#include <time.h>
#include "game.h"
#include "oop.h"

/* Used for both monster and player attacks */
uint8_t damage_calculate(uint8_t atk, uint8_t def) {
  uint8_t atk_base = (atk + ((rand() % atk) * 3 / 2));
  uint8_t def_base = ((def / 2) + ((rand() % def) / 2));
  if (def_base == 0) {
    printf("Critical hit!\n");
    return atk_base;
  }
  uint8_t damage   = (atk_base / def_base);
#ifdef DEBUG
  printf("damage_calculate(%d, %d): %d, %d == %d\n", atk, def, atk_base, def_base, damage);
#endif
  return damage;
}

int monster_init(void *self) {
  return 1;
}

uint8_t monster_attack(void *self, void *target) {
  monster *monster = self;
  player *player   = target;

  uint8_t damage = damage_calculate(monster->attack_dmg, player->defense);
  printf("%s %s! You take %d damage.\n", monster->name2, monster->attack_str, damage);

  return player->_(recv_action)(player, damage);
}

int monster_attack_receive(void *self, uint8_t damage) {
  monster *monster = self;

  if (damage >= monster->health)
    return 1;

  monster->health -= damage;
  return 0;
}

void monster_describe(void *self) {
  monster *monster = self;

  printf("%s\n", monster->desc_str);
}

void monster_check(monster *monster) {
  printf("Enemy HP: %3d    ATK: %3d  DEF: %3d\n", monster->health, monster->attack_dmg, monster->defense);
}

object monster_proto = {
  .init        = monster_init,
  .take_action = monster_attack,
  .recv_action = monster_attack_receive,
  .describe    = monster_describe
};

int room_init(void *self) {
  return 1;
}

void room_describe(void *self) {
  room *room = self;

  printf("You look around %s. %s\n", room->name, room->description);

  if (room->monster) {
    if (room->monster->health > 0) {
      printf("You spot %s just off in the distance.\n", room->monster->name);
      room->monster->_(describe)(room->monster);
      monster_check(room->monster);
      return; /* they can't leave the room */
    }
    else {
      printf("You see %s corpse resting in the middle of the room.\n", room->monster->name);
    }
  }

  printf("You can move in these directions: ");
  if (room->north) printf("north ");
  if (room->south) printf("south ");
  if (room->east) printf("east ");
  if (room->west) printf("west ");
  printf("\n");
}

object room_proto = {
  .init     = room_init,
  .describe = room_describe
};

int player_init(void *self) {
  player *player     = self;
  player->health     = 25 + (rand() % 17);
  player->attack_dmg = 10 + (rand() % 5);
  player->defense    = 6 + (rand() % 7);
  return 1;
}

uint8_t player_attack(void *self, void *target) {
  player *player   = self;
  monster *monster = target;

  uint8_t damage = damage_calculate(player->attack_dmg, monster->defense);
  printf("You attack %s! It takes %d damage.\n", monster->name2, damage);

  if (monster->_(recv_action)(monster, damage)) {
    printf("You have slain %s!\n", monster->name2);
    monster->health = 0;

    return 1;
  }

  return 0;
}

int player_attack_receive(void *self, uint8_t damage) {
  player *player = self;

  if (damage >= player->health) {
    printf("You have died!\n");
    exit(0);
  }

  player->health -= damage;
  return 0;
}

void player_move(void *self, direction dir) {
  player *player = self;
  room *current  = player->room_current, *next = NULL;
  monster *monster = current->monster;

  if (monster && monster->health > 0) {
    printf("You try to sneak past %s, but it notices you!\n", monster->name2);
    monster_attack(monster, player);
    return;
  }

  if (dir == NORTH && current->north) {
    printf("You move north.\n");
    next = current->north;
  }
  else if (dir == SOUTH && current->south) {
    printf("You move south.\n");
    next = current->south;
  }
  else if (dir == EAST && current->east) {
    printf("You move east.\n");
    next = current->east;
  }
  else if (dir == WEST && current->west) {
    printf("You move west.\n");
    next = current->west;
  }
  else {
    printf("You can't go in that direction.\n");
  }

  if (next) {
    next->_(describe)(next);
    player->room_current = next;
  }
}

void player_check(player *player) {
  printf(" Your HP: %3d    ATK: %3d  DEF: %3d\n", player->health, player->attack_dmg, player->defense);
}

object player_proto = {
  .init = player_init,
  .take_action = player_attack,
  .recv_action = player_attack_receive,
  .move_action = player_move
};

int game_init(void *self) {
  game *game                   = self;
  room **rooms                 = malloc(sizeof(room *) * MAX_ROOMS);
  monster **monsters           = malloc(sizeof(monster *) * MAX_MOBS);
  room_frec **room_frecs       = malloc(sizeof(room_frec *) * MAX_ROOMS);
  monster_frec **monster_frecs = malloc(sizeof(monster_frec *) * MAX_MOBS);
  char *path_tmp               = malloc(256);
  room_frec room_frec_read;
  monster_frec monster_frec_read;
  DIR *room_dh, *mob_dh;
  FILE *fh;
  struct dirent *d_entry;
  uint8_t i, north_id, south_id, east_id, west_id, monster_id;

  /* initialize the memory for the both file record structures and the runtime structures */
  /* we explicitly start from 0 here even though 0 is reserved, since we check against it */
  for (i = 0; i < MAX_MOBS; i++) {
    monster_frecs[i] = calloc(1, sizeof(monster_frec));
    monsters[i]      = calloc(1, sizeof(monster));
  }

  for (i = 0; i < MAX_ROOMS; i++) {
    room_frecs[i] = calloc(1, sizeof(room_frec));
    rooms[i]      = calloc(i, sizeof(room));
  }

  /* scan the mobs dir and read all entries */
  mob_dh = opendir("mobs");
  if (mob_dh == NULL) {
    perror("opendir mobs");
    exit(0);
  }

  while ((d_entry = readdir(mob_dh)) != NULL) {
    if (! strcmp(d_entry->d_name, ".") || ! strcmp(d_entry->d_name, "..")) continue;
    if (strlen(d_entry->d_name) > 249) {
      printf("filename too long \n");
      continue;
    }
    sprintf(path_tmp, "mobs/%s", d_entry->d_name);
    fh = fopen(path_tmp, "r");
    if (fh == NULL) {
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
    monsters[i]             = NEW(monster, monster_frecs[i]->name);
    monsters[i]->name       = strdup(monster_frecs[i]->name);
    monsters[i]->name2      = strdup(monster_frecs[i]->name2);
    monsters[i]->attack_str = strdup(monster_frecs[i]->attack_str);
    monsters[i]->defend_str = strdup(monster_frecs[i]->defend_str);
    monsters[i]->desc_str   = strdup(monster_frecs[i]->desc_str);
    monsters[i]->health     = monster_frecs[i]->health;
    monsters[i]->attack_dmg = monster_frecs[i]->attack_dmg;
    monsters[i]->defense    = monster_frecs[i]->defense;
  }

  /* similar process for rooms, read all entries */
  room_dh = opendir("rooms");
  if (room_dh == NULL) {
    perror("opendir room_dh");
    exit(0);
  }

  while ((d_entry = readdir(room_dh)) != NULL) {
    if (! strcmp(d_entry->d_name, ".") || ! strcmp(d_entry->d_name, "..")) continue;
    if (strlen(d_entry->d_name) > 249) {
      printf("filename too long \n");
      continue;
    }
    sprintf(path_tmp, "rooms/%s", d_entry->d_name);
    fh = fopen(path_tmp, "r");
    if (fh == NULL) {
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
    rooms[i] = NEW(room, room_frecs[i]->name);
    rooms[i]->name        = strdup(room_frecs[i]->name);
    rooms[i]->description = strdup(room_frecs[i]->description);
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

    if (monsters[monster_id])
      rooms[i]->monster = monsters[monster_id];
    else {
      printf("map %d monster_id %d does not exist\n", i, monster_id);
      exit(0);
    }
  }

  /* clean up the frecs allocations */
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
  player *playerobj       = NEW(player, "player");
  playerobj->room_current = rooms[1];
  game->start             = rooms[1];
  game->player            = playerobj;

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
      if (monster && monster->health > 0) {
        if (rand() % 8 != 3) {
          if (monster->health > 0) {
            monster_will_attack = 1;
          }
        }
        else
          monster_will_defend = (monster->defense % 4);
      }
      else { /* no monster */
        printf("You swing your broadsword at the air. This makes you both look and feel rather foolish.\n");
        return 1;
      }

      if (monster_will_defend) {
        monster->defense += monster_will_defend;
        printf("The %s %s.\n", monster->name2, monster->defend_str);
      }

      if (player->_(take_action)(player, monster))
        monster_will_attack = 0;

      if (monster_will_defend)
        monster->defense -= monster_will_defend;
      if (monster_will_attack)
        monster->_(take_action)(monster, player);

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
  game *gameobj = NEW(game, "");

  srand(time(NULL));

  printf("Welcome to the game!\n");
  gameobj->start->_(describe)(gameobj->start);

  while (process(gameobj)) {
    /* do nothing */
  }

  return 0;
}

