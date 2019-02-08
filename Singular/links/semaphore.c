#include "kernel/mod2.h"

#ifdef HAVE_SIMPLEIPC
#include <semaphore.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>

# include "simpleipc.h"

#include "Singular/cntrlc.h"
#include "reporter/si_signals.h"



// Not yet implemented: SYSV IPC Semaphores
// They are more difficult to clean up after a process crash
// but are supported more widely.

VAR sipc_sem_t *semaphore[SIPC_MAX_SEMAPHORES];
VAR int sem_acquired[SIPC_MAX_SEMAPHORES];

/* return 1 on success,
 *        0 if already initialized,
 *       -1 for errors
 */
int sipc_semaphore_init(int id, int count)
{
  char buf[100];
  sipc_sem_t *sem;
  if ((id<0) || (id >= SIPC_MAX_SEMAPHORES))
    return -1;
  // Already initialized?
  if (semaphore[id])
    return 0;
  // to make it completely safe, we should generate a name
  // from /dev/urandom.
#if USE_SEM_INIT
  // TODO: This should really use mapped memory so that processes
  // can keep using the semaphore after fork + exec.
  sem = malloc(sizeof(sem_t));
  if (!sem)
    return -1;
  if (sem_init(sem, 1, count) < 0)
  {
    free(sem);
    return -1;
  }
#else
#if PORTABLE_SEMAPHORES
#ifndef MAP_ANONYMOUS
#define MAP_ANONYMOUS MAP_ANON
#endif
  sem = mmap(NULL, getpagesize(),
    PROT_READ|PROT_WRITE, MAP_ANONYMOUS|MAP_SHARED, -1, 0);
  if (sem == MAP_FAILED)
    return -1;
  sem->count = count;
  sprintf(buf, "/%d:sem%d:g", getpid(), id);
  sem_unlink(buf);
  sem->guard = sem_open(buf, O_CREAT, 0600, count);
  sem_unlink(buf);
  sprintf(buf, "/%d:sem%d:s", getpid(), id);
  sem_unlink(buf);
  sem->sig = sem_open(buf, O_CREAT, 0600, count);
  sem_unlink(buf);
#else
  sprintf(buf, "/%d:sem%d", getpid(), id);
  sem_unlink(buf);
  sem = sem_open(buf, O_CREAT, 0600, count);
#endif
#endif
#if !PORTABLE_SEMAPHORES
  if (sem == SEM_FAILED || !sem)
    return -1;
#endif
  semaphore[id] = sem;
#if !USE_SEM_INIT
  sem_unlink(buf);
#endif
  return 1;
}

int sipc_semaphore_exists(int id)
{
  if ((id<0) || (id >= SIPC_MAX_SEMAPHORES))  return -1;
  return semaphore[id] != NULL;
}

int sipc_semaphore_acquire(int id)
{
  if ((id<0) || (id >= SIPC_MAX_SEMAPHORES) || (semaphore[id]==NULL))  return -1;
  defer_shutdown++;
#if PORTABLE_SEMAPHORES
  si_sem_wait(semaphore[id]->sig);
  si_sem_wait(semaphore[id]->guard);
  semaphore[id]->count--;
  sem_post(semaphore[id]->guard);
#else
  si_sem_wait(semaphore[id]);
#endif
  sem_acquired[id]++;
  defer_shutdown--;
  if (!defer_shutdown && do_shutdown) m2_end(1);
  return 1;
}

int sipc_semaphore_try_acquire(int id)
{
  if ((id<0) || (id >= SIPC_MAX_SEMAPHORES) || (semaphore[id]==NULL))  return -1;
  defer_shutdown++;
#if PORTABLE_SEMAPHORES
  int trywait = si_sem_trywait(semaphore[id]->sig);
#else
  int trywait = si_sem_trywait(semaphore[id]);
#endif
  if (!trywait)
  {
#if PORTABLE_SEMAPHORES
    si_sem_wait(semaphore[id]->guard);
    semaphore[id]->count--;
    sem_post(semaphore[id]->guard);
#endif
    sem_acquired[id]++;
  }
  defer_shutdown--;
  if (!defer_shutdown && do_shutdown) m2_end(1);
  return !trywait;
}

int sipc_semaphore_release(int id)
{
  if ((id<0) || (id >= SIPC_MAX_SEMAPHORES) || (semaphore[id]==NULL))  return -1;
  defer_shutdown++;
#if PORTABLE_SEMAPHORES
  sem_wait(semaphore[id]->guard);
  semaphore[id]->count++;
  sem_post(semaphore[id]->sig);
  sem_post(semaphore[id]->guard);
#else
  sem_post(semaphore[id]);
#endif
  sem_acquired[id]--;
  defer_shutdown--;
  if (!defer_shutdown && do_shutdown) m2_end(1);
  return 1;
}

int sipc_semaphore_get_value(int id)
{
  int val;
  if ((id<0) || (id >= SIPC_MAX_SEMAPHORES) || (semaphore[id]==NULL))  return -1;
#if PORTABLE_SEMAPHORES
  sem_wait(semaphore[id]->guard);
  val = semaphore[id]->count;
  sem_post(semaphore[id]->guard);
#else
  sem_getvalue(semaphore[id], &val);
#endif
  return val;
}

int simpleipc_cmd(char *cmd, int id, int v)
{
  if (strcmp(cmd,"init")==0)
    return sipc_semaphore_init(id,v);
  else if (strcmp(cmd,"exists")==0)
    return sipc_semaphore_exists(id);
  else if (strcmp(cmd,"acquire")==0)
    return  sipc_semaphore_acquire(id);
  else if (strcmp(cmd,"try_acquire")==0)
    return sipc_semaphore_try_acquire(id);
  else if (strcmp(cmd,"release")==0)
    return sipc_semaphore_release(id);
  else if (strcmp(cmd,"get_value")==0)
    return sipc_semaphore_get_value(id);
  else printf("unknown\n");
    return  -2;
}
#endif
