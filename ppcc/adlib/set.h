#pragma once

#include "lib.h"

// Note: this is not an enum. A low bit of 0 indicates a value of either
// SLOT_EMPTY or SLOT_OCCUPIED. A low bit of 1 (== SLOT_OCCUPIED)
// indicates that the top bits contain an abbreviated hash value.

#define SLOT_EMPTY 0
#define SLOT_OCCUPIED 1
#define SLOT_DELETED 2

template <typename T>
class Set : public GC {
  template <typename U>
  friend class Set;
private:
  static const Int _minsize = 8;
  Int _count;
  Int _size;
  Int _deleted;
  CmpFunc(T, _cmp);
  HashFunc(T, _hash);
  T *_data;
  // The _state array contains either SLOT_EMPTY, SLOT_DELETED or
  // an abbreviated hash value with the lowest bit (SLOT_OCCUPIED)
  // set. Rather than storing the entire hash value, we only store
  // 7 bits of it, expecting this to still eliminate 99% of calls
  // to the compare function.
  Byte *_state;
  void resize(Int newsize) {
    _data = (T *) GC_MALLOC(newsize * sizeof(T));
    _state = (Byte *) GC_MALLOC_ATOMIC(newsize);
    memset(_state, SLOT_EMPTY, newsize);
    _size = newsize;
  }
  void uncheckedAdd(T item, bool replace = false);
  void rebuild();
  Word next(Word pos, Word hash) {
    return (pos - hash) * 5 + 1 + hash;
  }

public:
  class Each {
  private:
    Set *_set;
    Int _i;
    void skip() {
      while (_i < _set->_size && (_set->_state[_i] & SLOT_OCCUPIED) == 0)
        _i++;
    }

  public:
    Each(Set *set) {
      _set = set;
      _i = 0;
      skip();
    }
    operator bool() {
      return _i < _set->_size;
    }
    void operator++() {
      _i++;
      skip();
    }
    void operator++(int dummy) {
      _i++;
      skip();
    }
    T &operator*() {
      return _set->_data[_i];
    }
    T operator->() {
      return _set->_data[_i];
    }
  };
  Set(CmpFunc(T, cmp), HashFunc(T, hash));
  Set();
  Set(Set<T> *set, bool copy = true);
  Set(Arr<T> *, CmpFunc(T, cmp), HashFunc(T, hash));
  Set(Arr<T> *);
  Set<T> *clone() {
    return new Set(this);
  }
  Int count() {
    return _count;
  }
  Set<T> *add(T item, bool replace = false);
  Set<T> *add(Arr<T> *item, bool replace = false);
  Set<T> &operator<<(T item) {
    return *add(item);
  }
  bool remove(T item);
  bool contains(T item);
  bool at(T item) {
    return contains(item);
  }
  T *find(T item);
  T get_or_add(T item);
  Arr<T> *items();
  bool eq(Set<T> *that);
  Set<T> *union_with(Set<T> *that, bool replace = false);
  Set<T> *union_in_place(Set<T> *that, bool replace = false);
  Set<T> *intersect_with(Set<T> *that);
  Set<T> *intersect_in_place(Set<T> *that);
  Set<T> *diff_with(Set<T> *that);
  Set<T> *diff_in_place(Set<T> *that);
  template <typename A, typename F>
  A fold(A init, F foldfunc);
  template <typename U, typename F>
  Set<U> *map(Set<U> *into, F mapfunc);
  template <typename F>
  Set<T> *filter(F filterfunc);
  template <typename F>
  void iter(F iterfunc);
};

template <typename T>
void Set<T>::rebuild() {
  Int size = _size;
  Int newsize = nextPow2(_count * 2);
  if (newsize < _minsize)
    newsize = _minsize;
  T *data = _data;
  Byte *state = _state;
  _count = 0;
  _deleted = 0;
  resize(newsize);
  for (Int i = 0; i < size; i++) {
    if ((state[i] & SLOT_OCCUPIED) != 0)
      uncheckedAdd(data[i]);
  }
}

template <typename T>
Set<T>::Set(CmpFunc(T, cmp), HashFunc(T, hash)) {
  resize(_minsize);
  _count = 0;
  _deleted = 0;
  _cmp = cmp;
  _hash = hash;
}

template <typename T>
Set<T>::Set() {
  resize(_minsize);
  _count = 0;
  _deleted = 0;
  _cmp = Cmp;
  _hash = Hash;
}

template <typename T>
Set<T>::Set(Set<T> *set, bool copy) {
  _cmp = set->_cmp;
  _hash = set->_hash;
  if (copy) {
    resize(set->_size);
    memcpy(_data, set->_data, sizeof(T) * _size);
    memcpy(_state, set->_state, _size);
    _count = set->_count;
    _deleted = set->_deleted;
  } else {
    resize(_minsize);
    _count = 0;
    _deleted = 0;
  }
}

template <typename T>
Set<T>::Set(Arr<T> *arr) {
  resize(_minsize);
  _count = 0;
  _deleted = 0;
  _cmp = Cmp;
  _hash = Hash;
  add(arr);
}

template <typename T>
Set<T>::Set(Arr<T> *arr, CmpFunc(T, cmp), HashFunc(T, hash)) {
  resize(_minsize);
  _count = 0;
  _deleted = 0;
  _cmp = cmp;
  _hash = hash;
  add(arr);
}

#define INIT_HASH_LOOP(item) \
  Word mask = _size - 1; \
  Word hash = _hash(item); \
  Word pos = hash & mask; \
  Byte occ = FibHash(hash, 8) | SLOT_OCCUPIED

template <typename T>
void Set<T>::uncheckedAdd(T item, bool replace) {
  INIT_HASH_LOOP(item);
  Int freepos = -1;
  while (_state[pos] != SLOT_EMPTY) {
    if (_state[pos] == occ && _cmp(_data[pos], item) == 0) {
      if (replace)
        _data[pos] = item;
      return;
    }
    if (_state[pos] == SLOT_DELETED && freepos < 0)
      freepos = pos;
    pos = next(pos, hash) & mask;
  }
  if (freepos >= 0) {
    _deleted--;
    pos = freepos;
  }
  _data[pos] = item;
  _state[pos] = occ;
  _count++;
}

template <typename T>
T Set<T>::get_or_add(T item) {
  if ((_count + _deleted) * 3 / 2 >= _size)
    rebuild();
  INIT_HASH_LOOP(item);
  while (_state[pos] != SLOT_EMPTY) {
    if (_state[pos] == occ && _cmp(_data[pos], item) == 0) {
      return _data[pos];
    }
    pos = next(pos, hash) & mask;
  }
  if (_state[pos] == SLOT_DELETED) {
    _deleted--;
  }
  _data[pos] = item;
  _state[pos] = occ;
  _count++;
  return item;
}

template <typename T>
Set<T> *Set<T>::add(T item, bool replace) {
  if ((_count + _deleted) * 3 / 2 >= _size)
    rebuild();
  uncheckedAdd(item, replace);
  return this;
}

template <typename T>
Set<T> *Set<T>::add(Arr<T> *arr, bool replace) {
  if ((_count + _deleted) * 3 / 2 >= _size)
    rebuild();
  for (Int i = 0; i < arr->len(); i++)
    add(arr->at(i), replace);
  return this;
}

template <typename T>
bool Set<T>::remove(T item) {
  INIT_HASH_LOOP(item);
  while (_state[pos] != SLOT_EMPTY) {
    if (_state[pos] == occ && _cmp(_data[pos], item) == 0) {
      memset(_data + pos, 0, sizeof(T));
      _state[pos] = SLOT_DELETED;
      _count--;
      _deleted++;
      if ((_count + _deleted) * 3 / 2 > _size || _deleted >= _count)
        rebuild();
      return 1;
    }
    pos = next(pos, hash) & mask;
  }
  return 0;
}

template <typename T>
T *Set<T>::find(T item) {
  INIT_HASH_LOOP(item);
  while (_state[pos] != SLOT_EMPTY) {
    if (_state[pos] == occ && _cmp(_data[pos], item) == 0)
      return _data + pos;
    pos = next(pos, hash) & mask;
  }
  return NULL;
}

template <typename T>
bool Set<T>::contains(T item) {
  return find(item) != NULL;
}

template <typename T>
Arr<T> *Set<T>::items() {
  Arr<T> *result = new Arr<T>(_count);
  for (Int i = 0; i < _size; i++) {
    if (_state[i] & SLOT_OCCUPIED)
      result->add(_data[i]);
  }
  return result;
}

template <typename T>
Set<T> *Set<T>::union_in_place(Set<T> *that, bool replace) {
  for (Each it(that); it; it++) {
    add(*it, replace);
  }
  return this;
}

template <typename T>
Set<T> *Set<T>::union_with(Set<T> *that, bool replace) {
  return clone()->union_in_place(that, replace);
}

template <typename T>
Set<T> *Set<T>::diff_in_place(Set<T> *that) {
  for (Each it(that); it; it++) {
    remove(*it);
  }
  return this;
}

template <typename T>
Set<T> *Set<T>::diff_with(Set<T> *that) {
  return clone()->diff_in_place(that);
}

template <typename T>
Set<T> *Set<T>::intersect_with(Set<T> *that) {
  Set<T> *result = new Set<T>(_cmp, _hash);
  for (Each it(this); it; it++) {
    if (that->contains(*it))
      result->add(*it);
  }
  return result;
}

template <typename T>
Set<T> *Set<T>::intersect_in_place(Set<T> *that) {
  Set<T> *tmp = intersect_with(that);
  *this = *tmp;
  return this;
}

template <typename T>
bool Set<T>::eq(Set<T> *that) {
  if (_count != that->_count)
    return false;
  for (Each it(this); it; it++) {
    if (!that->contains(*it))
      return false;
  }
  return true;
}

template <typename T>
template <typename A, typename F>
A Set<T>::fold(A init, F foldfunc) {
  A result = init;
  for (Each it(this); it; it++) {
    result = foldfunc(result, *it);
  }
  return result;
}

template <typename T>
template <typename U, typename F>
Set<U> *Set<T>::map(Set<U> *into, F mapfunc) {
  for (Each it(this); it; it++) {
    into->add(mapfunc(*it));
  }
  return into;
}

template <typename T>
template <typename F>
Set<T> *Set<T>::filter(F filterfunc) {
  Set<T> *result = new Set<T>(_cmp, _hash);
  for (Each it(this); it; it++) {
    if (filterfunc(*it))
      result->add(*it);
  }
  return result;
}

template <typename T>
template <typename F>
void Set<T>::iter(F iterfunc) {
  for (Each it(this); it; it++) {
    iterfunc(*it);
  }
}

typedef Set<Str *> StrSet;

static inline Str *S(StrSet *set, const char *sep = " ") {
  return StrJoin(set->items()->sort(StrCmp), sep);
}

#undef SLOT_EMPTY
#undef SLOT_OCCUPIED
#undef SLOT_DELETED

#undef INIT_HASH_LOOP
