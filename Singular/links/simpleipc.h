#ifndef _SIMPLEIPC_H
#define _SIMPLEIPC_H

#ifdef __cplusplus
extern "C"
{
#endif
#define SIPC_MAX_SEMAPHORES 256

#define USE_SEM_INIT 0

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
