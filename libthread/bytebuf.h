#ifndef _SINGULAR_LIBTHREAD_BYTEBUFFER_H
#define _SINGULAR_LIBTHREAD_BYTEBUFFER_H

#include <cstddef>
#include <cstdlib>
#include <cstring>

namespace LibThread {

using namespace std;

char *allocate_space (size_t n);
void free_space(size_t n, char *p);

class ByteBuf {
private:
  size_t count;
  size_t cap;
  size_t pos;
  char *buf;
public:
  ByteBuf() : pos(0), count(0), cap(0) { }
  ByteBuf(const ByteBuf &other) :
    count(other.count), cap(other.cap), pos(0)
  {
    buf = allocate_space(cap);
    memcpy(buf, other.buf, count);
  }
  ByteBuf &operator=(const ByteBuf &other) {
    count = other.count;
    cap = other.cap;
    pos = 0;
    buf = allocate_space(cap);
    memcpy(buf, other.buf, count);
  }
  ~ByteBuf() {
    free_space(cap, buf);
  }
  size_t size() { return count; }
  void write_bytes(char *p, size_t n) {
    if (pos + n >= cap) {
      char *newbuf = allocate_space(2 * cap);
      memcpy(newbuf, buf, count);
      free_space(cap, buf);
      cap *= 2;
    }
    memcpy(buf+pos, p, n);
    pos += n;
    if (pos >= count)
      count = pos;
  }
  void read_bytes(char *p, size_t n) {
    memcpy(p, buf+pos, n);
    pos += n;
  }
  template <typename T>
  void write(T &value) {
    write_bytes((char *)&value, sizeof(T));
  }
  template <typename T>
  void read(T &value) {
    read_bytes((char *)&value, sizeof(T));
  }
  void seek(size_t p) {
    pos = p;
  }
};

}

#endif // _SINGULAR_LIBTHREAD_BYTEBUFFER_H
