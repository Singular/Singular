#ifdef __cplusplus
extern "C" {
#endif

#include <stdlib.h>
#include <semaphore.h>
#include <pthread.h>
#include <stddef.h>

struct cpu_set_t;

int sem_unlink(const char *name) { 
    return -1; 
}

sem_t *sem_open(const char *name, int oflag, ...) { 
    return SEM_FAILED; 
}

int sem_getvalue(sem_t *sem, int *sval) { 
    return -1; 
}

int pthread_getaffinity_np(pthread_t thread, size_t cpusetsize, struct cpu_set_t *cpuset) { 
    return 0; 
}

int pthread_setaffinity_np(pthread_t thread, size_t cpusetsize, const struct cpu_set_t *cpuset) { 
    return 0; 
}

__attribute__((constructor)) void fix_env() {
    setenv("TMPDIR", "/tmp", 1);
    setenv("SINGULAR_DEFAULT_DIR", "/", 1);
    setenv("SINGULAR_EXECUTABLE", "/Singular", 1);
    setenv("SINGULAR_BIN_DIR", "/", 1);
}

#ifdef __cplusplus
}
#endif