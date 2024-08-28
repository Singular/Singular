// https://github.com/rbehrends/vspace
#include "vspace.h"
#include "kernel/mod2.h"
#ifdef HAVE_VSPACE
#ifdef HAVE_CPP_THREADS
#include <thread>
#endif
#ifdef HAVE_POLL
#include <poll.h>
#endif
#include <cstddef>
#include "reporter/si_signals.h"

#if defined(__GNUC__) && (__GNUC__<9) &&!defined(__clang__)

namespace vspace {
namespace internals {

size_t config[4]
    = { METABLOCK_SIZE, MAX_PROCESS, SEGMENT_SIZE, MAX_SEGMENTS };

VMem VMem::vmem_global;

// offsetof() only works for POD types, so we need to construct
// a portable version of it for metapage fields.

#define metapageaddr(field) \
  ((char *) &vmem.metapage->field - (char *) vmem.metapage)

size_t VMem::filesize() {
  struct stat stat;
  fstat(fd, &stat);
  return stat.st_size;
}

Status VMem::init(int fd) {
  this->fd = fd;
  for (int i = 0; i < MAX_SEGMENTS; i++)
    segments[i] = VSeg(NULL);
  for (int i = 0; i < MAX_PROCESS; i++) {
    int channel[2];
    if (pipe(channel) < 0) {
      for (int j = 0; j < i; j++) {
        close(channels[j].fd_read);
        close(channels[j].fd_write);
      }
      return Status(ErrOS);
    }
    channels[i].fd_read = channel[0];
    channels[i].fd_write = channel[1];
  }
  lock_metapage();
  init_metapage(filesize() == 0);
  unlock_metapage();
  freelist = metapage->freelist;
  return Status(ErrNone);
}

Status VMem::init() {
  FILE *fp = tmpfile();
  Status result = init(fileno(fp));
  if (!result.ok())
    return result;
  current_process = 0;
  file_handle = fp;
  metapage->process_info[0].pid = getpid();
  return Status(ErrNone);
}

Status VMem::init(const char *path) {
  int fd = open(path, O_RDWR | O_CREAT, 0600);
  if (fd < 0)
    return Status(ErrFile);
  init(fd);
  lock_metapage();
  // TODO: enter process in meta table
  unlock_metapage();
  return Status(ErrNone);
}

void VMem::deinit() {
  if (file_handle) {
    fclose(file_handle);
    file_handle = NULL;
  } else {
    close(fd);
  }
  munmap(metapage, METABLOCK_SIZE);
  metapage = NULL;
  current_process = -1;
  freelist = NULL;
  for (int i = 0; i < MAX_SEGMENTS; i++) {
    if (segments[i].base) munmap(segments[i].base, SEGMENT_SIZE);
    segments[i] = NULL;
  }
  for (int i = 0; i < MAX_PROCESS; i++) {
    close(channels[i].fd_read);
    close(channels[i].fd_write);
  }
}

void *VMem::mmap_segment(int seg) {
  lock_metapage();
  void *map = mmap(NULL, SEGMENT_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd,
      METABLOCK_SIZE + seg * SEGMENT_SIZE);
  if (map == MAP_FAILED) {
    // This is an "impossible to proceed from here, because system state
    // is impossible to proceed from" situation, so we abort the program.
    perror("mmap");
    abort();
  }
  unlock_metapage();
  return map;
}

void VMem::add_segment() {
  int seg = metapage->segment_count++;
  ftruncate(fd, METABLOCK_SIZE + metapage->segment_count * SEGMENT_SIZE);
  void *map_addr = mmap_segment(seg);
  segments[seg] = VSeg(map_addr);
  Block *top = block_ptr(seg * SEGMENT_SIZE);
  top->next = freelist[LOG2_SEGMENT_SIZE];
  top->prev = VADDR_NULL;
  freelist[LOG2_SEGMENT_SIZE] = seg * SEGMENT_SIZE;
}

void FastLock::lock() {
#ifdef HAVE_CPP_THREADS
  while (_lock.test_and_set()) {
  }
  bool empty = _owner < 0;
  if (empty) {
    _owner = vmem.current_process;
  } else {
    int p = vmem.current_process;
    vmem.metapage->process_info[p].next = -1;
    if (_head < 0)
      _head = p;
    else
      vmem.metapage->process_info[_tail].next = p;
    _tail = p;
  }
  _lock.clear();
  if (!empty)
    wait_signal(false);
#else
  lock_file(vmem.fd, _offset);
#endif
}

void FastLock::unlock() {
#ifdef HAVE_CPP_THREADS
  while (_lock.test_and_set()) {
  }
  _owner = _head;
  if (_owner >= 0)
    _head = vmem.metapage->process_info[_head].next;
  _lock.clear();
  if (_owner >= 0)
    send_signal(_owner, 0, false);
#else
  unlock_file(vmem.fd, _offset);
#endif
}

static void lock_allocator() {
  vmem.metapage->allocator_lock.lock();
}

static void unlock_allocator() {
  vmem.metapage->allocator_lock.unlock();
}

static void print_freelists() {
  for (int i = 0; i <= LOG2_SEGMENT_SIZE; i++) {
    vaddr_t vaddr = vmem.freelist[i];
    if (vaddr != VADDR_NULL) {
      printf("%2d: %ld", i, (long)vaddr);
      vaddr_t prev = block_ptr(vaddr)->prev;
      if (prev != VADDR_NULL) {
        printf("(%ld)", (long)prev);
      }
      assert(block_ptr(vaddr)->prev == VADDR_NULL);
      for (;;) {
        vaddr_t last_vaddr = vaddr;
        Block *block = block_ptr(vaddr);
        vaddr = block->next;
        if (vaddr == VADDR_NULL)
          break;
        printf(" -> %ld", (long)vaddr);
        vaddr_t prev = block_ptr(vaddr)->prev;
        if (prev != last_vaddr) {
          printf("(%ld)", (long)prev);
        }
      }
      printf("\n");
    }
  }
  fflush(stdout);
}

void vmem_free(vaddr_t vaddr) {
  lock_allocator();
  vaddr -= offsetof(Block, data);
  vmem.ensure_is_mapped(vaddr);
  size_t segno = vmem.segment_no(vaddr);
  VSeg seg = vmem.segment(vaddr);
  segaddr_t addr = vmem.segaddr(vaddr);
  int level = seg.block_ptr(addr)->level();
  assert(!seg.is_free(addr));
  while (level < LOG2_SEGMENT_SIZE) {
    segaddr_t buddy = find_buddy(addr, level);
    Block *block = seg.block_ptr(buddy);
    // is buddy free and at the same level?
    if (!block->is_free() || block->level() != level)
      break;
    // remove buddy from freelist.
    Block *prev = vmem.block_ptr(block->prev);
    Block *next = vmem.block_ptr(block->next);
    block->data[0] = level;
    if (prev) {
      assert(prev->next == vmem.vaddr(segno, buddy));
      prev->next = block->next;
    } else {
      // head of freelist.
      assert(vmem.freelist[level] == vmem.vaddr(segno, buddy));
      vmem.freelist[level] = block->next;
    }
    if (next) {
      assert(next->prev == vmem.vaddr(segno, buddy));
      next->prev = block->prev;
    }
    // coalesce block with buddy
    level++;
    if (buddy < addr)
      addr = buddy;
  }
  // Add coalesced block to free list
  Block *block = seg.block_ptr(addr);
  block->prev = VADDR_NULL;
  block->next = vmem.freelist[level];
  block->mark_as_free(level);
  vaddr_t blockaddr = vmem.vaddr(segno, addr);
  if (block->next != VADDR_NULL)
    vmem.block_ptr(block->next)->prev = blockaddr;
  vmem.freelist[level] = blockaddr;
  unlock_allocator();
}

vaddr_t vmem_alloc(size_t size) {
  lock_allocator();
  size_t alloc_size = size + offsetof(Block, data);
  int level = find_level(alloc_size);
  int flevel = level;
  while (flevel < LOG2_SEGMENT_SIZE && vmem.freelist[flevel] == VADDR_NULL)
    flevel++;
  if (vmem.freelist[flevel] == VADDR_NULL) {
    vmem.add_segment();
  }
  vmem.ensure_is_mapped(vmem.freelist[flevel]);
  while (flevel > level) {
    // get and split a block
    vaddr_t blockaddr = vmem.freelist[flevel];
    assert((blockaddr & ((1 << flevel) - 1)) == 0);
    Block *block = vmem.block_ptr(blockaddr);
    vmem.freelist[flevel] = block->next;
    if (vmem.freelist[flevel] != VADDR_NULL)
      vmem.block_ptr(vmem.freelist[flevel])->prev = VADDR_NULL;
    vaddr_t blockaddr2 = blockaddr + (1 << (flevel - 1));
    Block *block2 = vmem.block_ptr(blockaddr2);
    flevel--;
    block2->next = vmem.freelist[flevel];
    block2->prev = blockaddr;
    block->next = blockaddr2;
    block->prev = VADDR_NULL;
    // block->prev == VADDR_NULL already.
    vmem.freelist[flevel] = blockaddr;
  }
  assert(vmem.freelist[level] != VADDR_NULL);
  Block *block = vmem.block_ptr(vmem.freelist[level]);
  vaddr_t vaddr = vmem.freelist[level];
  #if defined(__GNUC__) && (__GNUC__>11)
  vaddr_t result = vaddr + (sizeof(vaddr_t)*2);
  #else
  vaddr_t result = vaddr + offsetof(Block, data);
  #endif
  vmem.freelist[level] = block->next;
  if (block->next != VADDR_NULL)
    vmem.block_ptr(block->next)->prev = VADDR_NULL;
  block->mark_as_allocated(vaddr, level);
  unlock_allocator();
  memset(block->data, 0, size);
  return result;
}

void init_flock_struct(
    struct flock &lock_info, size_t offset, size_t len, bool lock) {
  lock_info.l_start = offset;
  lock_info.l_len = len;
  lock_info.l_pid = 0;
  lock_info.l_type = lock ? F_WRLCK : F_UNLCK;
  lock_info.l_whence = SEEK_SET;
}

void lock_file(int fd, size_t offset, size_t len) {
  struct flock lock_info;
  init_flock_struct(lock_info, offset, len, true);
  fcntl(fd, F_SETLKW, &lock_info);
}

void unlock_file(int fd, size_t offset, size_t len) {
  struct flock lock_info;
  init_flock_struct(lock_info, offset, len, false);
  fcntl(fd, F_SETLKW, &lock_info);
}

void lock_metapage() {
  lock_file(vmem.fd, 0);
}

void unlock_metapage() {
  unlock_file(vmem.fd, 0);
}

void init_metapage(bool create) {
  if (create)
    ftruncate(vmem.fd, METABLOCK_SIZE);
  vmem.metapage = (MetaPage *) mmap(
      NULL, METABLOCK_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, vmem.fd, 0);
  if (create) {
    memcpy(vmem.metapage->config_header, config, sizeof(config));
    for (int i = 0; i <= LOG2_SEGMENT_SIZE; i++) {
      vmem.metapage->freelist[i] = VADDR_NULL;
    }
    vmem.metapage->segment_count = 0;
    vmem.metapage->allocator_lock = FastLock(metapageaddr(allocator_lock));
  } else {
    assert(memcmp(vmem.metapage->config_header, config, sizeof(config)) != 0);
  }
}

static void lock_process(int processno) {
  lock_file(vmem.fd,
      metapageaddr(process_info)
          + sizeof(ProcessInfo) * vmem.current_process);
}

static void unlock_process(int processno) {
  unlock_file(vmem.fd,
      metapageaddr(process_info)
          + sizeof(ProcessInfo) * vmem.current_process);
}

static ProcessInfo &process_info(int processno) {
  return vmem.metapage->process_info[processno];
}

bool send_signal(int processno, ipc_signal_t sig, bool lock) {
  if (lock)
    lock_process(processno);
  if (process_info(processno).sigstate != Waiting) {
    unlock_process(processno);
    return false;
  }
  if (processno == vmem.current_process) {
    process_info(processno).sigstate = Accepted;
    process_info(processno).signal = sig;
  } else {
    process_info(processno).sigstate = Pending;
    process_info(processno).signal = sig;
    int fd = vmem.channels[processno].fd_write;
    char buf[1] = { 0 };
    while (write(fd, buf, 1) != 1) {
    }
  }
  if (lock)
    unlock_process(processno);
  return true;
}

ipc_signal_t check_signal(bool resume, bool lock) {
  ipc_signal_t result;
  if (lock)
    lock_process(vmem.current_process);
  SignalState sigstate = process_info(vmem.current_process).sigstate;
  switch (sigstate) {
    case Waiting:
    case Pending: {
      int fd = vmem.channels[vmem.current_process].fd_read;
      char buf[1];
      if (lock && sigstate == Waiting) {
        unlock_process(vmem.current_process);
        loop
        {
          #if defined(HAVE_POLL)  && !defined(__APPLE__)
          // fd is restricted on OsX by ulimit "file descriptors" (256)
          pollfd pfd;
          pfd.fd = fd;
          pfd.events = POLLIN;
          int rv = poll(&pfd, 1, 500000); /* msec*/
          #else
          // fd is restricted to <=1024
          fd_set set;
          FD_ZERO(&set); /* clear the set */
          FD_SET(fd, &set); /* add our file descriptor to the set */
          struct timeval timeout;
          timeout.tv_sec = 500;
          timeout.tv_usec = 0;
          int rv = si_select(fd + 1, &set, NULL, NULL, &timeout);
          #endif
          if (rv== -1) continue; /* an error occurred */
          if (rv== 0) break;  /* timeout */
          while(read(fd, buf, 1)!=1) {}
          break;
        }
        lock_process(vmem.current_process);
      } else {
        loop
        {
          #if defined(HAVE_POLL)  && !defined(__APPLE__)
          // fd is restricted on OsX by ulimit "file descriptors" (256)
          pollfd pfd;
          pfd.fd = fd;
          pfd.events = POLLIN;
          int rv = poll(&pfd, 1, 500000); /* msec*/
          #else
          // fd is restricted to <=1024
          fd_set set;
          FD_ZERO(&set); /* clear the set */
          FD_SET(fd, &set); /* add our file descriptor to the set */
          struct timeval timeout;
          timeout.tv_sec = 500;
          timeout.tv_usec = 0;
          int rv = si_select(fd + 1, &set, NULL, NULL, &timeout);
          #endif
          if (rv== -1) continue;  /* an error occurred */
          if (rv== 0) break;  /* timeout */
          while(read(fd, buf, 1)!=1) {}
          break;
        }
      }
      result = process_info(vmem.current_process).signal;
      process_info(vmem.current_process).sigstate
          = resume ? Waiting : Accepted;
      if (lock)
        unlock_process(vmem.current_process);
      break;
    }
    case Accepted:
      result = process_info(vmem.current_process).signal;
      if (resume)
        process_info(vmem.current_process).sigstate = Waiting;
      if (lock)
        unlock_process(vmem.current_process);
      break;
  }
  return result;
}

void accept_signals() {
  lock_process(vmem.current_process);
  process_info(vmem.current_process).sigstate = Waiting;
  unlock_process(vmem.current_process);
}

ipc_signal_t wait_signal(bool lock) {
  return check_signal(true, lock);
}

} // namespace internals

pid_t fork_process() {
  using namespace internals;
  lock_metapage();
  for (int p = 0; p < MAX_PROCESS; p++) {
    if (vmem.metapage->process_info[p].pid == 0) {
      pid_t pid = fork();
      if (pid < 0) {
        // error
        return -1;
      } else if (pid == 0) {
        // child process
        int parent = vmem.current_process;
        vmem.current_process = p;
        lock_metapage();
        vmem.metapage->process_info[p].pid = getpid();
        unlock_metapage();
        send_signal(parent);
      } else {
        // parent process
        unlock_metapage();
        wait_signal();
        // child has unlocked metapage, so we don't need to.
      }
      return pid;
    }
  }
  unlock_metapage();
  return -1;
}

void Semaphore::post() {
  int wakeup = -1;
  internals::ipc_signal_t sig;
  _lock.lock();
  if (_head == _tail) {
    _value++;
  } else {
    // don't increment value, as we'll pass that on to the next process.
    wakeup = _waiting[_head];
    sig = _signals[_head];
    next(_head);
  }
  _lock.unlock();
  if (wakeup >= 0) {
    internals::send_signal(wakeup, sig);
  }
}

bool Semaphore::try_wait() {
  bool result = false;
  _lock.lock();
  if (_value > 0) {
    _value--;
    result = true;
  }
  _lock.unlock();
  return result;
}

void Semaphore::wait() {
  _lock.lock();
  if (_value > 0) {
    _value--;
    _lock.unlock();
    return;
  }
  _waiting[_tail] = internals::vmem.current_process;
  _signals[_tail] = 0;
  next(_tail);
  _lock.unlock();
  internals::wait_signal();
}

bool Semaphore::start_wait(internals::ipc_signal_t sig) {
  _lock.lock();
  if (_value > 0) {
    if (internals::send_signal(internals::vmem.current_process, sig))
      _value--;
    _lock.unlock();
    return false;
  }
  _waiting[_tail] = internals::vmem.current_process;
  _signals[_tail] = sig;
  next(_tail);
  _lock.unlock();
  return true;
}

bool Semaphore::stop_wait() {
  bool result = false;
  _lock.lock();
  for (int i = _head; i != _tail; next(i)) {
    if (_waiting[i] == internals::vmem.current_process) {
      int last = i;
      next(i);
      while (i != _tail) {
        _waiting[last] = _waiting[i];
        _signals[last] = _signals[i];
        last = i;
        next(i);
      }
      _tail = last;
      result = true;
      break;
    }
  }
  _lock.unlock();
  return result;
}

void EventSet::add(Event *event) {
  event->_next = NULL;
  if (_head == NULL) {
    _head = _tail = event;
  } else {
    _tail->_next = event;
    _tail = event;
  }
}

int EventSet::wait() {
  size_t n = 0;
  for (Event *event = _head; event; event = event->_next) {
    if (!event->start_listen((int) (n++))) {
      break;
    }
  }
  internals::ipc_signal_t result = internals::check_signal();
  for (Event *event = _head; event; event = event->_next) {
    event->stop_listen();
  }
  internals::accept_signals();
  return (int) result;
}

} // namespace vspace
#else // gcc>9
#include <cstdlib>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/stat.h>


namespace vspace {
namespace internals {

size_t config[4]
    = { METABLOCK_SIZE, MAX_PROCESS, SEGMENT_SIZE, MAX_SEGMENTS };

VMem VMem::vmem_global;

// offsetof() only works for POD types, so we need to construct
// a portable version of it for metapage fields.

#define metapageaddr(field) \
  ((char *) &vmem.metapage->field - (char *) vmem.metapage)

size_t VMem::filesize() {
  struct stat stat;
  fstat(fd, &stat);
  return stat.st_size;
}

Status VMem::init(int fd) {
  this->fd = fd;
  for (int i = 0; i < MAX_SEGMENTS; i++)
    segments[i] = VSeg(NULL);
  for (int i = 0; i < MAX_PROCESS; i++) {
    int channel[2];
    if (pipe(channel) < 0) {
      for (int j = 0; j < i; j++) {
        close(channels[j].fd_read);
        close(channels[j].fd_write);
      }
      return Status(ErrOS);
    }
    channels[i].fd_read = channel[0];
    channels[i].fd_write = channel[1];
  }
  lock_metapage();
  init_metapage(filesize() == 0);
  unlock_metapage();
  freelist = metapage->freelist;
  return Status(ErrNone);
}

Status VMem::init() {
  FILE *fp = tmpfile();
  Status result = init(fileno(fp));
  if (!result.ok())
    return result;
  current_process = 0;
  file_handle = fp;
  metapage->process_info[0].pid = getpid();
  return Status(ErrNone);
}

Status VMem::init(const char *path) {
  int fd = open(path, O_RDWR | O_CREAT, 0600);
  if (fd < 0)
    return Status(ErrFile);
  init(fd);
  lock_metapage();
  // TODO: enter process in meta table
  unlock_metapage();
  return Status(ErrNone);
}

void VMem::deinit() {
  if (file_handle) {
    fclose(file_handle);
    file_handle = NULL;
  } else {
    close(fd);
  }
  munmap(metapage, METABLOCK_SIZE);
  metapage = NULL;
  current_process = -1;
  freelist = NULL;
  for (int i = 0; i < MAX_SEGMENTS; i++) {
    if (!segments[i].is_free())
      munmap(segments[i].base, SEGMENT_SIZE);
    segments[i] = VSeg(NULL);
  }
  for (int i = 0; i < MAX_PROCESS; i++) {
    close(channels[i].fd_read);
    close(channels[i].fd_write);
  }
}

void *VMem::mmap_segment(int seg) {
  lock_metapage();
  void *map = mmap(NULL, SEGMENT_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd,
      METABLOCK_SIZE + seg * SEGMENT_SIZE);
  if (map == MAP_FAILED) {
    // This is an "impossible to proceed from here, because system state
    // is impossible to proceed from" situation, so we abort the program.
    perror("mmap");
    abort();
  }
  unlock_metapage();
  return map;
}

void VMem::add_segment() {
  int seg = metapage->segment_count++;
  ftruncate(fd, METABLOCK_SIZE + metapage->segment_count * SEGMENT_SIZE);
  void *map_addr = mmap_segment(seg);
  segments[seg] = VSeg(map_addr);
  Block *top = block_ptr(seg * SEGMENT_SIZE);
  top->next = freelist[LOG2_SEGMENT_SIZE];
  top->prev = VADDR_NULL;
  freelist[LOG2_SEGMENT_SIZE] = seg * SEGMENT_SIZE;
}

void FastLock::lock() {
#ifdef HAVE_CPP_THREADS
  while (_lock.test_and_set()) {
  }
  bool empty = _owner < 0;
  if (empty) {
    _owner = vmem.current_process;
  } else {
    int p = vmem.current_process;
    vmem.metapage->process_info[p].next = -1;
    if (_head < 0)
      _head = p;
    else
      vmem.metapage->process_info[_tail].next = p;
    _tail = p;
  }
  _lock.clear();
  if (!empty)
    wait_signal(false);
#else
  lock_file(vmem.fd, _offset);
#endif
}

void FastLock::unlock() {
#ifdef HAVE_CPP_THREADS
  while (_lock.test_and_set()) {
  }
  _owner = _head;
  if (_owner >= 0)
    _head = vmem.metapage->process_info[_head].next;
  _lock.clear();
  if (_owner >= 0)
    send_signal(_owner, 0, false);
#else
  unlock_file(vmem.fd, _offset);
#endif
}

static void lock_allocator() {
  vmem.metapage->allocator_lock.lock();
}

static void unlock_allocator() {
  vmem.metapage->allocator_lock.unlock();
}

static void print_freelists() {
  for (int i = 0; i <= LOG2_SEGMENT_SIZE; i++) {
    vaddr_t vaddr = vmem.freelist[i];
    if (vaddr != VADDR_NULL) {
      std::printf("%2d: %ld", i, (long)vaddr);
      vaddr_t prev = block_ptr(vaddr)->prev;
      if (prev != VADDR_NULL) {
        std::printf("(%ld)", (long)prev);
      }
      assert(block_ptr(vaddr)->prev == VADDR_NULL);
      for (;;) {
        vaddr_t last_vaddr = vaddr;
        Block *block = block_ptr(vaddr);
        vaddr = block->next;
        if (vaddr == VADDR_NULL)
          break;
        std::printf(" -> %ld", (long)vaddr);
        vaddr_t prev = block_ptr(vaddr)->prev;
        if (prev != last_vaddr) {
          std::printf("(%ld)", (long)prev);
        }
      }
      std::printf("\n");
    }
  }
  std::fflush(stdout);
}

void vmem_free(vaddr_t vaddr) {
  lock_allocator();
  #if defined(__GNUC__) && (__GNUC__>11)
  vaddr -= (sizeof(vaddr_t)*2);
  #else
  vaddr -= offsetof(Block, data);
  #endif
  vmem.ensure_is_mapped(vaddr);
  size_t segno = vmem.segment_no(vaddr);
  VSeg seg = vmem.segment(vaddr);
  segaddr_t addr = vmem.segaddr(vaddr);
  int level = seg.block_ptr(addr)->level();
  assert(!seg.is_free(addr));
  while (level < LOG2_SEGMENT_SIZE) {
    segaddr_t buddy = find_buddy(addr, level);
    Block *block = seg.block_ptr(buddy);
    // is buddy free and at the same level?
    if (!block->is_free() || block->level() != level)
      break;
    // remove buddy from freelist.
    Block *prev = vmem.block_ptr(block->prev);
    Block *next = vmem.block_ptr(block->next);
    block->data[0] = level;
    if (prev) {
      assert(prev->next == vmem.vaddr(segno, buddy));
      prev->next = block->next;
    } else {
      // head of freelist.
      assert(vmem.freelist[level] == vmem.vaddr(segno, buddy));
      vmem.freelist[level] = block->next;
    }
    if (next) {
      assert(next->prev == vmem.vaddr(segno, buddy));
      next->prev = block->prev;
    }
    // coalesce block with buddy
    level++;
    if (buddy < addr)
      addr = buddy;
  }
  // Add coalesced block to free list
  Block *block = seg.block_ptr(addr);
  block->prev = VADDR_NULL;
  block->next = vmem.freelist[level];
  block->mark_as_free(level);
  vaddr_t blockaddr = vmem.vaddr(segno, addr);
  if (block->next != VADDR_NULL)
    vmem.block_ptr(block->next)->prev = blockaddr;
  vmem.freelist[level] = blockaddr;
  unlock_allocator();
}

vaddr_t vmem_alloc(size_t size) {
  lock_allocator();
  #if defined(__GNUC__) && (__GNUC__>11)
  size_t alloc_size = size +  (sizeof(vaddr_t)*2);
  #else
  size_t alloc_size = size + offsetof(Block, data);
  #endif
  int level = find_level(alloc_size);
  int flevel = level;
  while (flevel < LOG2_SEGMENT_SIZE && vmem.freelist[flevel] == VADDR_NULL)
    flevel++;
  if (vmem.freelist[flevel] == VADDR_NULL) {
    vmem.add_segment();
  }
  vmem.ensure_is_mapped(vmem.freelist[flevel]);
  while (flevel > level) {
    // get and split a block
    vaddr_t blockaddr = vmem.freelist[flevel];
    assert((blockaddr & ((1 << flevel) - 1)) == 0);
    Block *block = vmem.block_ptr(blockaddr);
    vmem.freelist[flevel] = block->next;
    if (vmem.freelist[flevel] != VADDR_NULL)
      vmem.block_ptr(vmem.freelist[flevel])->prev = VADDR_NULL;
    vaddr_t blockaddr2 = blockaddr + (1 << (flevel - 1));
    Block *block2 = vmem.block_ptr(blockaddr2);
    flevel--;
    block2->next = vmem.freelist[flevel];
    block2->prev = blockaddr;
    block->next = blockaddr2;
    block->prev = VADDR_NULL;
    // block->prev == VADDR_NULL already.
    vmem.freelist[flevel] = blockaddr;
  }
  assert(vmem.freelist[level] != VADDR_NULL);
  Block *block = vmem.block_ptr(vmem.freelist[level]);
  vaddr_t vaddr = vmem.freelist[level];
  #if defined(__GNUC__) && (__GNUC__>11)
  vaddr_t result = vaddr + (sizeof(vaddr_t)*2);
  #else
  vaddr_t result = vaddr + offsetof(Block, data);
  #endif
  vmem.freelist[level] = block->next;
  if (block->next != VADDR_NULL)
    vmem.block_ptr(block->next)->prev = VADDR_NULL;
  block->mark_as_allocated(vaddr, level);
  unlock_allocator();
  memset(block->data, 0, size);
  return result;
}

void init_flock_struct(
    struct flock &lock_info, size_t offset, size_t len, bool lock) {
  lock_info.l_start = offset;
  lock_info.l_len = len;
  lock_info.l_pid = 0;
  lock_info.l_type = lock ? F_WRLCK : F_UNLCK;
  lock_info.l_whence = SEEK_SET;
}

void lock_file(int fd, size_t offset, size_t len) {
  struct flock lock_info;
  init_flock_struct(lock_info, offset, len, true);
  fcntl(fd, F_SETLKW, &lock_info);
}

void unlock_file(int fd, size_t offset, size_t len) {
  struct flock lock_info;
  init_flock_struct(lock_info, offset, len, false);
  fcntl(fd, F_SETLKW, &lock_info);
}

void lock_metapage() {
  lock_file(vmem.fd, 0);
}

void unlock_metapage() {
  unlock_file(vmem.fd, 0);
}

void init_metapage(bool create) {
  if (create)
    ftruncate(vmem.fd, METABLOCK_SIZE);
  vmem.metapage = (MetaPage *) mmap(
      NULL, METABLOCK_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, vmem.fd, 0);
  if (create) {
    std::memcpy(vmem.metapage->config_header, config, sizeof(config));
    for (int i = 0; i <= LOG2_SEGMENT_SIZE; i++) {
      vmem.metapage->freelist[i] = VADDR_NULL;
    }
    vmem.metapage->segment_count = 0;
    vmem.metapage->allocator_lock = FastLock(metapageaddr(allocator_lock));
  } else {
    assert(std::memcmp(vmem.metapage->config_header, config,
        sizeof(config)) != 0);
  }
}

static void lock_process(int processno) {
  lock_file(vmem.fd,
      metapageaddr(process_info)
          + sizeof(ProcessInfo) * vmem.current_process);
}

static void unlock_process(int processno) {
  unlock_file(vmem.fd,
      metapageaddr(process_info)
          + sizeof(ProcessInfo) * vmem.current_process);
}

static ProcessInfo &process_info(int processno) {
  return vmem.metapage->process_info[processno];
}

bool send_signal(int processno, ipc_signal_t sig, bool lock) {
  if (lock)
    lock_process(processno);
  if (process_info(processno).sigstate != Waiting) {
    unlock_process(processno);
    return false;
  }
  if (processno == vmem.current_process) {
    process_info(processno).sigstate = Accepted;
    process_info(processno).signal = sig;
  } else {
    process_info(processno).sigstate = Pending;
    process_info(processno).signal = sig;
    int fd = vmem.channels[processno].fd_write;
    char buf[1] = { 0 };
    while (write(fd, buf, 1) != 1) {
    }
  }
  if (lock)
    unlock_process(processno);
  return true;
}

ipc_signal_t check_signal(bool resume, bool lock) {
  ipc_signal_t result;
  if (lock)
    lock_process(vmem.current_process);
  SignalState sigstate = process_info(vmem.current_process).sigstate;
  switch (sigstate) {
    case Waiting:
    case Pending: {
      int fd = vmem.channels[vmem.current_process].fd_read;
      char buf[1];
      if (lock && sigstate == Waiting) {
        unlock_process(vmem.current_process);
        loop
        {
          #if defined(HAVE_POLL)  && !defined(__APPLE__)
          // fd is restricted on OsX by ulimit "file descriptors" (256)
          pollfd pfd;
          pfd.fd = fd;
          pfd.events = POLLIN;
          int rv = poll(&pfd, 1, 500000); /* msec*/
          #else
          // fd is restricted to <=1024
          fd_set set;
          FD_ZERO(&set); /* clear the set */
          FD_SET(fd, &set); /* add our file descriptor to the set */
          struct timeval timeout;
          timeout.tv_sec = 500;
          timeout.tv_usec = 0;
          int rv = si_select(fd + 1, &set, NULL, NULL, &timeout);
          #endif
          if (rv== -1) continue;  /* an error occurred */
          if (rv== 0) break;  /* timeout */
          while(read(fd, buf, 1)!=1) {}
          break;
        }
        lock_process(vmem.current_process);
      } else {
        loop
        {
          #if defined(HAVE_POLL)  && !defined(__APPLE__)
          // fd is restricted on OsX by ulimit "file descriptors" (256)
          pollfd pfd;
          pfd.fd = fd;
          pfd.events = POLLIN;
          int rv = poll(&pfd, 1, 500000); /* msec*/
          #else
          // fd is restricted to <=1024
          fd_set set;
          FD_ZERO(&set); /* clear the set */
          FD_SET(fd, &set); /* add our file descriptor to the set */
          struct timeval timeout;
          timeout.tv_sec = 500;
          timeout.tv_usec = 0;
          int rv = si_select(fd + 1, &set, NULL, NULL, &timeout);
          #endif
          if (rv== -1) continue; /* an error occurred */
          if (rv== 0) break;/* timeout */
          while(read(fd, buf, 1)!=1) {}
          break;
        }
      }
      result = process_info(vmem.current_process).signal;
      process_info(vmem.current_process).sigstate
          = resume ? Waiting : Accepted;
      if (lock)
        unlock_process(vmem.current_process);
      break;
    }
    case Accepted:
      result = process_info(vmem.current_process).signal;
      if (resume)
        process_info(vmem.current_process).sigstate = Waiting;
      if (lock)
        unlock_process(vmem.current_process);
      break;
  }
  return result;
}

void accept_signals() {
  lock_process(vmem.current_process);
  process_info(vmem.current_process).sigstate = Waiting;
  unlock_process(vmem.current_process);
}

ipc_signal_t wait_signal(bool lock) {
  return check_signal(true, lock);
}

} // namespace internals

pid_t fork_process() {
  using namespace internals;
  lock_metapage();
  for (int p = 0; p < MAX_PROCESS; p++) {
    if (vmem.metapage->process_info[p].pid == 0) {
      pid_t pid = fork();
      if (pid < 0) {
        // error
        return -1;
      } else if (pid == 0) {
        // child process
        int parent = vmem.current_process;
        vmem.current_process = p;
        lock_metapage();
        vmem.metapage->process_info[p].pid = getpid();
        unlock_metapage();
        send_signal(parent);
      } else {
        // parent process
        unlock_metapage();
        wait_signal();
        // child has unlocked metapage, so we don't need to.
      }
      return pid;
    }
  }
  unlock_metapage();
  return -1;
}

void Semaphore::post() {
  int wakeup = -1;
  internals::ipc_signal_t sig;
  _lock.lock();
  if (_head == _tail) {
    _value++;
  } else {
    // don't increment value, as we'll pass that on to the next process.
    wakeup = _waiting[_head];
    sig = _signals[_head];
    next(_head);
  }
  _lock.unlock();
  if (wakeup >= 0) {
    internals::send_signal(wakeup, sig);
  }
}

bool Semaphore::try_wait() {
  bool result = false;
  _lock.lock();
  if (_value > 0) {
    _value--;
    result = true;
  }
  _lock.unlock();
  return result;
}

void Semaphore::wait() {
  _lock.lock();
  if (_value > 0) {
    _value--;
    _lock.unlock();
    return;
  }
  _waiting[_tail] = internals::vmem.current_process;
  _signals[_tail] = 0;
  next(_tail);
  _lock.unlock();
  internals::wait_signal();
}

bool Semaphore::start_wait(internals::ipc_signal_t sig) {
  _lock.lock();
  if (_value > 0) {
    if (internals::send_signal(internals::vmem.current_process, sig))
      _value--;
    _lock.unlock();
    return false;
  }
  _waiting[_tail] = internals::vmem.current_process;
  _signals[_tail] = sig;
  next(_tail);
  _lock.unlock();
  return true;
}

bool Semaphore::stop_wait() {
  bool result = false;
  _lock.lock();
  for (int i = _head; i != _tail; next(i)) {
    if (_waiting[i] == internals::vmem.current_process) {
      int last = i;
      next(i);
      while (i != _tail) {
        _waiting[last] = _waiting[i];
        _signals[last] = _signals[i];
        last = i;
        next(i);
      }
      _tail = last;
      result = true;
      break;
    }
  }
  _lock.unlock();
  return result;
}

void EventSet::add(Event *event) {
  event->_next = NULL;
  if (_head == NULL) {
    _head = _tail = event;
  } else {
    _tail->_next = event;
    _tail = event;
  }
}

int EventSet::wait() {
  size_t n = 0;
  for (Event *event = _head; event; event = event->_next) {
    if (!event->start_listen((int) (n++))) {
      break;
    }
  }
  internals::ipc_signal_t result = internals::check_signal();
  for (Event *event = _head; event; event = event->_next) {
    event->stop_listen();
  }
  internals::accept_signals();
  return (int) result;
}

} // namespace vspace
#endif
#endif
