#include <assert.h>
#include <stdlib.h>

#include "allocator.h"

static void **alloc_cleanup;       /*+ list of alloc'd pointers +*/
static size_t alloc_cleanup_count; /*+ count of alloc'd pointers +*/

/*+ performs the actual cleanup function, frees all long-running memory at program exit +*/
void alloc_do_cleanup(void) {
  size_t i;

  for (i = 0; i < alloc_cleanup_count; i++) {
    free(alloc_cleanup[i]);
  }

  free(alloc_cleanup);
}

/*+ registers the atexit callback. should be called early as possible in program init +*/
void alloc_register_cb(void) {
  atexit(alloc_do_cleanup);
}

/*+ register a pointer to be automatically cleaned up at program exit +*/
void alloc_register(void *ptr) /*+ pointer to newly-alloc'd memory +*/
{
  assert(NULL != ptr);
  void *tmp;
  tmp = realloc(alloc_cleanup, sizeof(void *) * (alloc_cleanup_count + 1));
  assert(NULL != tmp);
  alloc_cleanup = tmp;
  alloc_cleanup[alloc_cleanup_count++] = ptr;
}
