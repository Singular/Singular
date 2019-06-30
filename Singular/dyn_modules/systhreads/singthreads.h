#ifndef _SINGULAR_LIBTHREAD
#define _SINGULAR_LIBTHREAD

#include <cstdlib>
#include <new>
#include <string>
#include <vector>

namespace LibThread {

class ThreadState;
typedef void *(*ThreadFunc)(ThreadState *, void *);

ThreadState *createThread(ThreadFunc body, void **arg, const char **error);
void *joinThread(ThreadState *thread);

template <typename T>
T *shared_alloc(std::size_t n);
template <typename T>
T *shared_alloc0(std::size_t n);
template <typename T>
T shared_free(T *p);

#ifdef USE_SHARED_ALLOCATOR

template <class T>
struct SharedAllocator {
  typedef T value_type;
  SharedAllocator() {
    // do nothing
  }
  template <class U> SharedAllocator(const SharedAllocator<U>&) noexcept {
    // do nothing
  }
  T *allocate(std::size_t n) {
    T *p = shared_alloc0<T>(n);
    if (p) return p;
    throw std::bad_alloc();
  }
  void deallocate(T *p, std::size_t n) noexcept {
    shared_free(p);
  }
};

typedef std::basic_string<char, std::char_traits<char>, SharedAllocator<char> >
  SharedString;

template <typename T>
class SharedVector : public std::vector<T, SharedAllocator<T> > {
public:
  SharedVector() : std::vector<T, SharedAllocator<T>>() { }
  SharedVector(std::size_t n, const T& value = T()) : std::vector<T, SharedAllocator<T> >(n, value) { }
  SharedVector(SharedVector &other) : std::vector<T, SharedAllocator<T> >(&other) { }
};

#endif

}

#endif // _SINGULAR_LIBTHREAD
