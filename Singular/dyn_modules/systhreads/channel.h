#ifndef _SINGULAR_LIBTHREAD_CHANNEL
#define _SINGULAR_LIBTHREAD_CHANNEL

#include "singthreads.h"
#include "thread.h"
#include <queue>

namespace LibThread {

template <typename T>
class Channel {
private:
  Lock lock;
  ConditionVariable cond;
  int waiting;
  std::queue<T> q;
public:
  Channel() : lock(), cond(&lock), waiting(0), q() {
  }
  void send(T& value) {
    lock.lock();
    q.push(value);
    if (waiting)
      cond.signal();
    lock.unlock();
  }
  void receive(T& result) {
    lock.lock();
    waiting++;
    while (q.empty())
      cond.wait();
    result = q.pop();
    waiting--;
    if (waiting)
      cond.signal();
    lock.unlock();
  }
  T receive() {
    T result;
    receive(&result);
    return result;
  }
};

}

#endif // _SINGULAR_LIBTHREAD_CHANNEL
