#ifndef _THREAD_H
#define _THREAD_H

#include <limits.h>
#include <pthread.h>
#include <stdint.h>
#include <alloca.h>
#include <cstddef>
#include <exception>

typedef pthread_t Thread;

void ThreadError(const char *message);

class ConditionVariable;

class Lock {
private:
  pthread_mutex_t mutex;
  friend class ConditionVariable;
  Thread owner;
  int locked;
  bool recursive;
  void resume_lock(int l) {
    owner = pthread_self();
    locked = l;
  }
  int break_lock() {
    extern pthread_t no_thread;
    int result = locked;
    owner = no_thread;
    locked = 0;
    return result;
  }
public:
  Lock(bool rec = false) {
    extern pthread_t no_thread;
    pthread_mutex_init(&mutex, NULL);
    locked = 0;
    recursive = rec;
    owner = no_thread;
  }
  ~Lock() {
    pthread_mutex_destroy(&mutex);
  }
  void lock() {
    Thread self = pthread_self();
    if (owner == self) {
      if (locked && !recursive)
	ThreadError("locking mutex twice");
    }
    else
      pthread_mutex_lock(&mutex);
    owner = self;
    locked++;
  }
  void unlock() {
    extern pthread_t no_thread;
    Thread self = pthread_self();
    if (owner != self)
      ThreadError("unlocking unowned lock");
    locked--;
    if (locked == 0) {
      owner = no_thread;
      pthread_mutex_unlock(&mutex);
    }
  }
  bool is_locked() {
    return locked != 0 && owner == pthread_self();
  }
};

class ConditionVariable {
  friend class Lock;
private:
  pthread_cond_t condition;
  Lock *lock;
  int waiting;
  friend class Semaphore;
  ConditionVariable() { }
public:
  ConditionVariable(Lock *lock_init) : waiting(0), lock(lock_init) {
    pthread_cond_init(&condition, NULL);
  }
  ~ConditionVariable() {
    pthread_cond_destroy(&condition);
  }
  void wait() {
    if (!lock->is_locked())
      ThreadError("waited on condition without locked mutex");
    waiting++;
    int l = lock->break_lock();
    pthread_cond_wait(&condition, &lock->mutex);
    waiting--;
    lock->resume_lock(l);
  }
  void signal() {
    if (!lock->is_locked())
      ThreadError("signaled condition without locked mutex");
    if (waiting)
      pthread_cond_signal(&condition);
  }
  void broadcast() {
    if (!lock->is_locked())
      ThreadError("signaled condition without locked mutex");
    if (waiting)
      pthread_cond_broadcast(&condition);
  }
};

class Semaphore {
private:
  Lock lock;
  ConditionVariable cond;
  unsigned count;
  unsigned waiting;
public:
  Semaphore() : lock(), cond(&lock), count(0), waiting(0) {
  }
  Semaphore(unsigned count0) : lock(), cond(&lock), count(count0), waiting(0) {
  }
  void wait();
  void post();
};

#endif // _THREAD_H
