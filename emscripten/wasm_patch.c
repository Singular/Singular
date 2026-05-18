#ifdef __cplusplus
extern "C" {
#endif

#include <stdlib.h>
#include <semaphore.h>
#include <pthread.h>
#include <stddef.h>

struct cpu_set_t;

static sem_t fake_sem;

int sem_unlink(const char *name) { 
    return 0; 
}

sem_t *sem_open(const char *name, int oflag, ...) { 
    return &fake_sem; /* Pretend it opened successfully */
}

int sem_getvalue(sem_t *sem, int *sval) { 
    if (sval) *sval = 1; /* Fake that 1 core/resource is available */
    return 0; 
}

int sem_close(sem_t *sem) {
    return 0; 
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
    setenv("SINGULARPATH", "/LIB", 1);
}

#ifdef __cplusplus
}
#endif