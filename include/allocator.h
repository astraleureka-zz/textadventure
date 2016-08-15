#ifndef _ALLOCATOR_H
#define _ALLOCATOR_H 1

void alloc_do_cleanup(void);
void alloc_register_cb(void);
void alloc_register(void *ptr);

#endif
