#ifndef _singthread_lintree_h
#define _singthread_lintree_h

#include <string>
#include <vector>
#include <cstring>
#include <Singular/ipid.h>

namespace LinTree {

class LinTree;

typedef void (*LinTreeEncodeFunc)(LinTree &lintree, leftv val);
typedef leftv (*LinTreeDecodeFunc)(LinTree &lintree);
typedef void (*LinTreeRefFunc)(LinTree &lintree, int by);

extern std::vector<LinTreeEncodeFunc> encoders;
extern std::vector<LinTreeDecodeFunc> decoders;
extern std::vector<LinTreeRefFunc> refupdaters;

void install(int typ, LinTreeEncodeFunc enc, LinTreeDecodeFunc dec,
  LinTreeRefFunc ref);

class LinTree {
private:
  std::string &memory;
  size_t cursor;
  const char * error;
  void *last_ring;
public:
  LinTree();
  LinTree(const LinTree &other);
  ~LinTree();
  LinTree(std::string &source);
  LinTree& operator=(const LinTree &other);
  void rewind() { cursor = 0; }
  void clear() { memory.clear(); cursor = 0; error = NULL; last_ring = NULL; }
  void mark_error(const char *s) {
    error = s;
  }
  int has_error() {
    return error != NULL;
  }
  const char *error_msg() {
    return error;
  }
  template<typename T>
  T get() {
    T result;
    memcpy(&result, memory.c_str() + cursor, sizeof(T));
    cursor += sizeof(T);
    return result;
  }
  template<typename T>
  T get_prev() {
    T result;
    memcpy(&result, memory.c_str() + cursor - sizeof(T), sizeof(T));
    return result;
  }
  template<typename T>
  void put(T data) {
    memory.append((const char *) &data, sizeof(T));
  }
  template<typename T>
  void skip() {
    cursor += sizeof(T);
  }
  int get_int() {
    return get<int>();
  }
  size_t get_size() {
    return get<size_t>();
  }
  void put_int(int code) {
    put(code);
  }
  void skip_int() {
    skip<int>();
  }
  const char *get_bytes(size_t n) {
    const char *result = memory.c_str() + cursor;
    cursor += n;
    return result;
  }
  const char *get_addr() {
    return memory.c_str() + cursor;
  }
  void put_bytes(char *p, size_t n) {
    memory.append(p, n);
  }
  char *reserve_bytes(size_t n) {
    size_t pos = memory.size();
    memory.reserve(n);
    for (size_t i = 0; i < n; i++) {
      memory += '\0';
    }
    return (char *)(memory.c_str() + pos);
  }
  void put_cstring(char *p) {
    size_t n = strlen(p);
    put(n);
    put_bytes(p, n+1);
  }
  const char *get_cstring() {
    size_t n = get_size();
    const char *result = memory.c_str() + cursor;
    cursor += n + 1;
    return result;
  }
  void skip_cstring() {
    size_t n = get_size();
    cursor += n + 1;
  }
  void skip_bytes(size_t n) {
    cursor += n;
  }
  std::string &to_string() {
    return memory;
  }
  void set_last_ring(void *r);
  int has_last_ring() {
    return last_ring != NULL;
  }
  void *get_last_ring() {
    return last_ring;
  }
};

std::string to_string(leftv val);
leftv from_string(std::string &str);

void init();

};

#endif
