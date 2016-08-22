#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include "allocator.h"
#include "types.h"
#include "game.h"
#include "util.h"

object item_proto = {
  .init     = item_init,
  .describe = item_describe
};

int item_init(void* self) {
  return 1;
}

void item_describe(void* self) {
  item_t* item = self;
}

/*+ loads data from json structure into item structure +*/
boolean_t item_json_unpack(void* item_ptr,    /*+ target item structure +*/
                           json_t* json_item) /*+ source json structure +*/
/*+ returns TRUE upon success, or FALSE upon failure +*/
{
  item_t* item       = item_ptr;
  if (! item || ! json_item) return FALSE;

  item->name         = JSON_OBJECT_STRING(json_item, "name");
  item->description  = JSON_OBJECT_STRING(json_item, "description");
  item->health       = JSON_OBJECT_INTEGER(json_item, "health");
  item->skill        = JSON_OBJECT_INTEGER(json_item, "skill");
  item->strength     = JSON_OBJECT_INTEGER(json_item, "strength");
  item->defense      = JSON_OBJECT_INTEGER(json_item, "defense");
  item->celerity     = JSON_OBJECT_INTEGER(json_item, "celerity");
  item->intelligence = JSON_OBJECT_INTEGER(json_item, "intelligence");

  item->flags.can_use   = JSON_OBJECT_BOOLEAN(json_item, "can_use");
  item->flags.can_eat   = JSON_OBJECT_BOOLEAN(json_item, "can_eat");
  item->flags.can_equip = JSON_OBJECT_BOOLEAN(json_item, "can_equip");
  item->flags.can_throw = JSON_OBJECT_BOOLEAN(json_item, "can_throw");
  item->flags.is_unique = JSON_OBJECT_BOOLEAN(json_item, "is_unique");

  alloc_register(item->name);
  alloc_register(item->description);

  if (item->flags.can_eat && item->flags.can_equip) {
    printf("%s: flag conflict: cannot set both can_eat and can_equip\n", item->_(class));
    return FALSE;
  }

  return TRUE;
}


