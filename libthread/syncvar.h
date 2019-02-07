#ifndef _SINGULAR_LIBTHREAD_SYNCVAR
#define _SINGULAR_LIBTHREAD_SYNCVAR

#include "singthreads.h"
#include "thread.h"
#include <queue>

namespace LibThread {

template <typename T>
class SyncVar {
private:
  Lock lock;
  ConditionVariable cond;
  int waiting;
  bool init;
  T value;
public:
  SyncVar() : lock(), cond(&lock), waiting(0), init(false) {
  }
  void write(T& value) {
    lock.lock();
    if (!init) {
      this->value = value;
      this->init = true;
    }
    if (waiting)
      cond.signal();
    lock.unlock();
  }
  void read(T& result) {
    lock.lock();
    waiting++;
    while (!init)
      cond.wait();
    waiting--;
    if (waiting)
      cond.signal();
    lock.unlock();
  }
  bool try_read(T& result) {
    bool success;
    lock.lock();
    success = init;
    if (success) {
      result = value;
    }
    lock.unlock();
    return success;
  }
};

}

#endif // _SINGULAR_LIBTHREAD_SYNCVAR
