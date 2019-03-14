#include <string.h>
#include <stdlib.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifdef PSINGULAR
typedef struct InitList {
  struct InitList *next;
  void (*func)();
} InitList;

static InitList *inits;

void pSingular_initialize_thread() {
  InitList *list = inits;
  while (list) {
    list->func();
    list = list->next;
  }
}

void pSingular_register_init(void (*f)()) {
  InitList *next = (InitList *) malloc(sizeof(InitList));
  next->next = inits;
  next->func = f;
  inits = next;
  (*f)();
}

void pSingular_init_var(void *s, void *t, long n) {
  memcpy(s, t, n);
}

void *pSingular_alloc_var(long n) {
  return malloc(n);
}

#endif
#ifdef __cplusplus
}
#endif
