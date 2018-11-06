#ifndef __STACK_CHK_GUARD_H__
#define __STACK_CHK_GUARD_H__

typedef union {
  struct {
#ifdef __x86_64__
    unsigned long long stack_chk_guard, heap_chunk_chk_guard, heap_page_chk_guard;
#else
    unsigned long stack_chk_guard, heap_chunk_chk_guard, heap_page_chk_guard;
#endif
  } v;
  char __pad[0x1000];
} stack_chk_guard_t;

extern stack_chk_guard_t __stack_chk_guard;

#endif
