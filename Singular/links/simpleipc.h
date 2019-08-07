#include <semaphore.h>

#ifndef _SIMPLEIPC_H
#define _SIMPLEIPC_H

#ifdef __cplusplus
extern "C"
{
#endif
#define SIPC_MAX_SEMAPHORES 256

#define USE_SEM_INIT 0

#ifdef __APPLE__
#define PORTABLE_SEMAPHORES 1
#endif

#if PORTABLE_SEMAPHORES

#include <sys/mman.h>

typedef struct {
  sem_t *guard, *sig;
  int count;
} sipc_sem_t;
#else
typedef sem_t sipc_sem_t;
#endif


THREAD_VAR extern sipc_sem_t *semaphore[SIPC_MAX_SEMAPHORES];
THREAD_VAR extern int sem_acquired[SIPC_MAX_SEMAPHORES];

int sipc_semaphore_init(int id, int count);
int sipc_semaphore_exists(int id);
int sipc_semaphore_acquire(int id);
int sipc_semaphore_try_acquire(int id);
int sipc_semaphore_get_value(int id);
int sipc_semaphore_release(int id);

int simpleipc_cmd(char *cmd, int id, int v);
#ifdef __cplusplus
}
#endif
#endif
