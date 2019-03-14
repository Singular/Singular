#include "threadconf.h"
#include <list>
#include <vector>

#include <cstring>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/mman.h>

#include "thread.h"
#include "singthreads.h"

using namespace std;

pthread_t no_thread;

void ThreadError(const char *message) {
  fprintf(stderr, "FATAL ERROR: %s\n", message);
  abort(); // should not happen
}

void Semaphore::wait() {
  lock.lock();
  waiting++;
  while (count == 0)
    cond.wait();
  waiting--;
  count--;
  lock.unlock();
}

void Semaphore::post() {
  lock.lock();
  if (count++ == 0 && waiting)
    cond.signal();
  lock.unlock();
}

namespace LibThread {
  template <typename T>
  T *shared_alloc(size_t n) {
    T *p = (T *) malloc(n * sizeof(T));
    return p;
  }
  template <typename T>
  T *shared_alloc0(size_t n) {
    T *p = (T *) calloc(n, sizeof(T));
    return p;
  }
  template <typename T>
  void shared_free(T *p) {
    free(p);
  }
}
