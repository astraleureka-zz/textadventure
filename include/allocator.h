#ifndef _ALLOCATOR_H
#define _ALLOCATOR_H 1

#ifdef DEBUG
void __backtrace_and_die();
void __backtrace();
#endif

void alloc_register_cb(void);
void alloc_register(void* ptr);

#endif
