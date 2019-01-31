#include "singthreads.h"
#include "channel.h"
#include "syncvar.h"

int main() {
  LibThread::SyncVar<int> svar();
  LibThread::Channel<int> ch();
  Semaphore sem(1);
  return 0;
}

