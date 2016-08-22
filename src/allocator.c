#ifdef DEBUG
# include <stdio.h>
# include <execinfo.h>
# include <signal.h>
void __backtrace_and_die();
void __backtrace();
#endif

#include <assert.h>
#include <stdlib.h>

#include "allocator.h"
#include "types.h"

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
#ifdef DEBUG
  signal(SIGSEGV, __backtrace_and_die);
#endif
  atexit(alloc_do_cleanup);
}

/*+ register a pointer to be automatically cleaned up at program exit +*/
void alloc_register(void *ptr) /*+ pointer to newly-alloc'd memory +*/
{
#ifdef DEBUG
  fprintf(stderr, "===[ %p ]===\n", ptr);
  __backtrace(FALSE);
  fprintf(stderr, "\n");
#endif

  assert(NULL != ptr);
  void* tmp;
  tmp = realloc(alloc_cleanup, sizeof(void *) * (alloc_cleanup_count + 1));
  assert(NULL != tmp);
  alloc_cleanup = tmp;
  alloc_cleanup[alloc_cleanup_count++] = ptr;
}

#ifdef DEBUG
void __backtrace_and_die() {
  __backtrace();
  exit(0);
}

void __backtrace() {
  void* trace[255];
  const int calls = backtrace(trace, sizeof(trace) / sizeof(void*));
  backtrace_symbols_fd(trace, calls, 1);
}
#endif
