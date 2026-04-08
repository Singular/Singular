#ifdef __cplusplus
extern "C" {
#endif

#include <stdlib.h>

int sem_unlink() { return 0; }
void* sem_open() { return 0; }
int sem_getvalue() { return 0; }
int pthread_getaffinity_np() { return 0; }
int pthread_setaffinity_np() { return 0; }

__attribute__((constructor)) void fix_env() {
    setenv("TMPDIR", "/tmp", 1);
    setenv("SINGULAR_DEFAULT_DIR", "/", 1);
    setenv("SINGULAR_EXECUTABLE", "/Singular", 1);
    setenv("SINGULAR_BIN_DIR", "/", 1);
}

#ifdef __cplusplus
}
#endif
