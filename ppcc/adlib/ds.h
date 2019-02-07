#pragma once

#include <stddef.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdint.h>

typedef size_t Word;
typedef uint32_t Word32;
typedef uint64_t Word64;
typedef ssize_t Int;
typedef char char;
typedef unsigned char Byte;
typedef void *Obj;

template <typename T>
typedef int (*CmpFunc)(T, T);
template <typename T>
typedef Word (*HashFunc)(T);
template <typename T>
typedef T (*MapFunc)(T);
template <typename T>
typedef int (*TestFunc)(T);
template <typename T>
typedef T (*FoldFunc)(T, T);

typedef struct {
  Word len;
  Word cap;
  char data[1];
} StrData, *Str;

template <typename T>
typedef struct {
  Word len;
  Word cap;
  T data[1];
} ArrData, *Arr;

template <typename T>
typedef struct {
  Word count;
  CmpFunc<T> cmp;
  HashFunc<T> hash;
  Arr<Arr<T>> buckets;
} SetData, *Set;

Str NewStr(Word cap);
Str ExpandStr(Str &str, Word newcap);
Str ShrinkStr(Str &str, int fully = 1);
Str MakeStr(const char *str);
Str NewStrFromBytes(const char *str, Word n);
Str CloneStr(Str str);
void StrAppend(Str &str, char ch);
void StrAppend(Str &str, const char *s);
void StrAppend(Str &str, const char *s, Word n);
void StrAppend(Str &str, Str str2);

Arr<Str> StrSplit(Str str, Str seps);
Arr<Str> StrSplitLines(Str str);
Arr<Str> StrSplitChar(Str str, char sep);
Str StrJoin(Arr<Str> arr, const char *sep);
int StrEndsWith(Str str, const char *suffix);
int StrStartsWith(Str str, const char *prefix);

Arr NewArr(Word cap);
Arr MakeArr(Word n, Obj *items);
Arr MakeArr1(Obj item);
Arr MakeArr2(Obj item1, Obj item2);
Arr MakeArr3(Obj item1, Obj item2, Obj item3);
Arr MakeArr4(Obj item1, Obj item2, Obj item3, Obj item4);
Arr ExpandArr(Arr *arr, Word newcap);
Arr ShrinkArr(Arr *arr, int fully);
Arr CloneArr(Arr arr);
void ArrPush(Arr *aref, Obj item);
Obj ArrPop(Arr *aref);
void ArrAppend(Arr *aref, Arr arr);
Arr ArrMap(Arr arr, MapFunc fn);
Arr ArrFilter(Arr arr, TestFunc fn);
Arr ArrFilterMap(Arr arr, MapFunc mapfn);
Obj ArrFold(Arr arr, Obj init, FoldFunc fn);

void SetAdd(Set set, Obj item);
void SetRemove(Set set, Obj item);
Obj SetFind(Set set, Obj item);

Word HashBytes(char *addr, Word n);
Word HashStr(Str str);
Word HashCStr(char *str);

template <typename T>
static inline T ArrLast(Arr<T> arr) {
  assert(arr->len > 0);
  return arr->data[arr->len-1];
}

template <typename T>
static inline T ArrGet(Arr<T> arr, Word index) {
  assert(index < arr->len);
  return arr->data[index];
}

template <typename T>
static inline T& ArrItem(Arr<T> arr, Word index) {
  assert(index < arr->len);
  return arr->data[index];
}

#define FOREACH(i, a) \
        for (size_t i = 0, len_ = a->len; i < len_; i++)
#define FOREACH_REV(i, a) \
        for (size_t i = a->len; i-- > 0;)

#define NEW(t) ((t *)memset(GC_malloc(sizeof(t)), 0, sizeof(t)))
#define NEW_ATOMIC(t) ((t *)memset(GC_malloc_atomic(sizeof(t)), 0, sizeof(t)))
