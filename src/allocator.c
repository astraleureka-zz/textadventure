#include <assert.h>
#include <stdlib.h>
#include "allocator.h"

static void **alloc_cleanup;
static size_t alloc_cleanup_count;

void alloc_do_cleanup(void) {
  size_t i;

  for (i = 0; i < alloc_cleanup_count; i++) {
    free(alloc_cleanup[i]);
  }

  free(alloc_cleanup);
}

void alloc_register_cb(void) {
  atexit(alloc_do_cleanup);
}

void alloc_register(void *ptr) {
  alloc_cleanup = realloc(alloc_cleanup, sizeof(void *) * (alloc_cleanup_count + 1));
  alloc_cleanup[alloc_cleanup_count++] = ptr;
}
