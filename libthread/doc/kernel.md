# Creating threads

Functions:

        #include "libthread/singthreads.h"

        typedef void *(*ThreadFunc)(ThreadState *thread, void *arg);
        ThreadState *createThread(ThreadFunc func, void *arg);
        void *joinThread(ThreadState *thread);

Example:

        #include "libthread/singthreads.h"

        using namespace LibThread;

        void *thread_func(ThreadState *thread, void *arg) {
          ...;
          return result;
        }

        void example() {
          ThreadState *thread = createThread(func, NULL):
          void *result = joinThread(thread);
          // Note: do not delete thread
        }

# Channels

Classes and functions:

        #include "libthread/singthreads.h"
        #include "libthread/channel.h"

        template <typename T>
        class Channel {
        public:
          Channel();
          void send(T& value);
          T receive();
          void receive(T& result);
        }

Example:

        #include "libthread/singthreads.h"
        #include "libthread/channel.h"

        using namespace LibThread;

        void example() {
          Channel<int> *ch = new Channel<int>();
          ch->send(1);
          ch->send(2);
          int value = ch->receive();
          ch->receive(value); // avoids copying
          delete ch;
        }

# Synchronization variables

Classes and functions:

        #include "libthread/singthreads.h"
        #include "libthread/syncvar.h"

        template <typename T>
        class SyncVar {
        public:
          SyncVar();
          void write(T& value);
          void read(T& result);
          bool try_read(T& result);
        }

Example:

        #include "libthread/singthreads.h"
        #include "libthread/channel.h"

        using namespace LibThread;

        void example() {
          SyncVar<int> *svar = new SyncVar<int>();
          svar->write(1);
          int value;
          svar->read(value);
          delete svar;
        }

# Mutexes and condition variables

Classes and functions:

        #include "libthread/thread.h"

        class Lock {
        public:
          Lock();
          void lock();
          void unlock();
        }

        class ConditionVariable {
        public:
          ConditionVariable(Lock *lock);
          void signal();
          void wait();
          void broadcast();
        }

Example:

        #include "libthread/thread.h"

        void example() {
          Lock *lock = new Lock();
          ConditionVariable *cond = new ConditionVariable(lock);
          lock.lock();
          cond.signal();
          lock.unlock();
          delete cond;
          delete lock;
        }

# Semaphores

Classes and functions:

        #include "libthread/thread.h"

        class Semaphore {
        public:
          Semaphore();
          Semaphore(int initial_value);
          void post();
          void wait();
        }

Example:

        #include "libthread/thread.h"

        void example() {
          Semaphore *sem = new Semaphore(1);
          sem->wait(); // -> 0
          sem->post(); // -> 1
          sem->wait(); // -> 0
          delete sem;
        }

