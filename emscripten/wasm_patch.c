#ifdef __cplusplus
extern "C" {
#endif

#include <stdlib.h>

int sem_unlink(void* name) { return 0; }
int sem_open(void* name, int oflag, int mode) { return 0; }
int sem_getvalue(void* sem, int* sval) { return 0; }
int pthread_getaffinity_np(int thread, int cpusetsize, void* cpuset) { return 0; }
int pthread_setaffinity_np(int thread, int cpusetsize, void* cpuset) { return 0; }

__attribute__((constructor)) void fix_env() {
    setenv("TMPDIR", "/tmp", 1);
    setenv("SINGULAR_DEFAULT_DIR", "/", 1);
    setenv("SINGULAR_EXECUTABLE", "/Singular", 1);
    setenv("SINGULAR_BIN_DIR", "/", 1);
}

#ifdef __cplusplus
}
#endif