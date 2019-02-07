#pragma once

typedef unsigned char Byte;
typedef const char *CStr;

#define CmpFunc(T, func) int (*func)(T, T)
#define HashFunc(T, func) Word (*func)(T)
#define TestFunc(T, func) int (*func)(T)
#define MapFunc(T, func) T (*func)(T)
#define FoldFunc(T, A, func) A (*func)(A, T)

#define NUMOF(a) (sizeof(a) / sizeof(a[0]))

#define NOWHERE ((Int) -1)

static inline Int nextPow2(Int n) {
  Int result = 1;
  while (result < n)
    result *= 2;
  return result;
}

template <typename T>
static inline T Min(T a, T b) {
  return a < b ? a : b;
}

template <typename T>
static inline T Max(T a, T b) {
  return a > b ? a : b;
}

int Cmp(Int a, Int b);
int Cmp(Word a, Word b);

template <typename R, typename A, typename C>
class Lambda {
  R (*fn)(A, C);
  C context;
public:
  Lambda(R (*fn)(A, C), C context) {
    this->fn = fn;
    this->context = context;
  }
  R operator()(A arg) {
    return fn(arg, context);
  }
};

template <typename R, typename A, typename C>
static inline Lambda<R, A, C> F(R(*fn)(A, C), C context) {
  return Lambda<R, A, C>(fn, context);
}