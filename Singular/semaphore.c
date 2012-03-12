#include <semaphore.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <Singular/mod2.h>

#ifdef HAVE_SIMPLEIPC
#include "simpleipc.h"

// Not yet implemented: SYSV IPC Semaphores
// They are more difficult to clean up after a process crash
// but are supported more widely.

static sem_t *semaphore[SIPC_MAX_SEMAPHORES];

/* return 1 on success,
 *        0 if already initialized,
 *       -1 for errors
 */
int sipc_semaphore_init(int id, int count)
{
  char buf[100];
  sem_t *sem;
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
  sprintf(buf, "/%d:sem%d", getpid(), id);
  sem_unlink(buf);
  sem = sem_open(buf, O_CREAT, 0600, count);
#endif
  if (sem == SEM_FAILED || !sem)
    return -1;
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
  sem_wait(semaphore[id]);
  return 1;
}

int sipc_semaphore_try_acquire(int id)
{
  if ((id<0) || (id >= SIPC_MAX_SEMAPHORES) || (semaphore[id]==NULL))  return -1;
  return !sem_trywait(semaphore[id]);
}

int sipc_semaphore_release(int id)
{
  if ((id<0) || (id >= SIPC_MAX_SEMAPHORES) || (semaphore[id]==NULL))  return -1;
  sem_post(semaphore[id]);
  return 1;
}

int sipc_semaphore_get_value(int id)
{
  int val;
  if ((id<0) || (id >= SIPC_MAX_SEMAPHORES) || (semaphore[id]==NULL))  return -1;
  sem_getvalue(semaphore[id], &val);
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
