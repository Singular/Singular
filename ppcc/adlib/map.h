#pragma once

#include "lib.h"

// Note: this is not an enum. A low bit of 0 indicates a value of either
// SLOT_EMPTY or SLOT_OCCUPIED. A low bit of 1 (== SLOT_OCCUPIED)
// indicates that the top bits contain an abbreviated hash value.

#define SLOT_EMPTY 0
#define SLOT_OCCUPIED 1
#define SLOT_DELETED 2

template <typename K, typename V>
struct Assoc : public GC {
  K key;
  V value;
};

template <typename K, typename V>
class Map : public GC {
  template <typename K2, typename V2>
  friend class Map;
private:
  static const Int _minsize = 8;
  Int _count;
  Int _size;
  Int _deleted;
  CmpFunc(K, _cmp);
  HashFunc(K, _hash);
  K *_keys;
  V *_values;
  // The _state array contains either SLOT_EMPTY, SLOT_DELETED or
  // an abbreviated hash value with the lowest bit (SLOT_OCCUPIED)
  // set. Rather than storing the entire hash value, we only store
  // 7 bits of it, expecting this to still eliminate 99% of calls
  // to the compare function.
  Byte *_state;
  void resize(Int newsize) {
    _keys = (K *) GC_MALLOC(newsize * sizeof(K));
    _values = (V *) GC_MALLOC(newsize * sizeof(V));
    _state = (Byte *) GC_MALLOC_ATOMIC(newsize);
    memset(_state, SLOT_EMPTY, newsize);
    _size = newsize;
  }
  void uncheckedAdd(K key, V value, bool replace = false);
  void rebuild();
  Word next(Word pos, Word hash) {
    return (pos - hash) * 5 + 1 + hash;
  }

public:
  class Each {
  private:
    Map *_map;
    Int _i;
    void skip() {
      while (_i < _map->_size && (_map->_state[_i] & SLOT_OCCUPIED) == 0)
        _i++;
    }

  public:
    Each(Map *map) {
      _map = map;
      _i = 0;
      skip();
    }
    operator bool() {
      return _i < _map->_size;
    }
    void operator++() {
      _i++;
      skip();
    }
    void operator++(int dummy) {
      _i++;
      skip();
    }
    K &key() {
      return _map->_keys[_i];
    }
    V &value() {
      return _map->_values[_i];
    }
    Assoc<K, V> pair() {
      Assoc<K, V> assoc;
      assoc.key = _map->_keys[_i];
      assoc.value = _map->_values[_i];
      return assoc;
    }
  };
  Map(CmpFunc(K, cmp), HashFunc(K, hash));
  Map();
  Map(Map<K, V> *map, bool copy = true);
  Map(Arr<K> *keys, Arr<V> *values);
  Map(Arr<K> *keys, Arr<V> *values, CmpFunc(K, cmp), HashFunc(K, hash));
  Map<K, V> *clone() {
    return new Map(this);
  }
  Int count() {
    return _count;
  }
  Map<K, V> *add(K key, V value, bool replace = false);
  Map<K, V> *add(Assoc<K, V> assoc, bool replace = false) {
    add(assoc.key, assoc.value, replace);
    return this;
  }
  Map<K, V> *add(Arr<Assoc<K, V> > *arr, bool replace = false);
  Map<K, V> *add(Arr<K> *keys, Arr<V> *values, bool replace = false);
  bool remove(K key);
  bool contains(K key);
  bool find(K key, V &value);
  V get(K key, V if_absent);
  V get(K key);
  V at(K key);
  Arr<K> *keys();
  Arr<V> *values();
  Arr<Assoc<K, V> > *pairs();
  bool eq(Map<K, V> *that);
  Map<K, V> *union_with(Map<K, V> *that, bool replace = false);
  Map<K, V> *union_in_place(Map<K, V> *that, bool replace = false);
  Map<K, V> *intersect_with(Map<K, V> *that);
  Map<K, V> *intersect_in_place(Map<K, V> *that);
  Map<K, V> *diff_with(Map<K, V> *that);
  Map<K, V> *diff_in_place(Map<K, V> *that);
  template <typename A, typename F>
  A fold(A init, F foldfunc);
  template <typename V2, typename F>
  Map<K, V2> *map_values(Map<K, V2> *into, F mapfunc);
  template <typename K2, typename V2, typename F, typename G>
  Map<K2, V2> *map_pairs(Map<K2, V2> *into, F mapfunc, G mapfunc2);
  template <typename F>
  Map<K, V> *filter(F filterfunc);
  template <typename F>
  void iter(F iterfunc);
};

template <typename K, typename V>
void Map<K, V>::rebuild() {
  Int size = _size;
  Int newsize = nextPow2(_count * 2);
  if (newsize < _minsize)
    newsize = _minsize;
  K *keys = _keys;
  V *values = _values;
  Byte *state = _state;
  _count = 0;
  _deleted = 0;
  resize(newsize);
  for (Int i = 0; i < size; i++) {
    if (state[i] & SLOT_OCCUPIED)
      uncheckedAdd(keys[i], values[i]);
  }
}

template <typename K, typename V>
Map<K, V>::Map(CmpFunc(K, cmp), HashFunc(K, hash)) {
  resize(_minsize);
  _count = 0;
  _deleted = 0;
  _cmp = cmp;
  _hash = hash;
}

template <typename K, typename V>
Map<K, V>::Map() {
  resize(_minsize);
  _count = 0;
  _deleted = 0;
  _cmp = Cmp;
  _hash = Hash;
}

template <typename K, typename V>
Map<K, V>::Map(Arr<K> *keys, Arr<V> *values) {
  resize(_minsize);
  _count = 0;
  _deleted = 0;
  _cmp = Cmp;
  _hash = Hash;
  add(keys, values);
}

template <typename K, typename V>
Map<K, V>::Map(
    Arr<K> *keys, Arr<V> *values, CmpFunc(K, cmp), HashFunc(K, hash)) {
  resize(_minsize);
  _count = 0;
  _deleted = 0;
  _cmp = cmp;
  _hash = hash;
  add(keys, values);
}

template <typename K, typename V>
Map<K, V>::Map(Map<K, V> *map, bool copy) {
  _cmp = map->_cmp;
  _hash = map->_hash;
  if (copy) {
    resize(map->_size);
    memcpy(_keys, map->_keys, sizeof(K) * _size);
    memcpy(_values, map->_values, sizeof(V) * _size);
    memcpy(_state, map->_state, _size);
    _count = map->_count;
    _deleted = map->_deleted;
  } else {
    resize(_minsize);
    _count = 0;
    _deleted = 0;
  }
}

#define INIT_HASH_LOOP(key) \
  Word mask = _size - 1; \
  Word hash = _hash(key); \
  Word pos = hash & mask; \
  Byte occ = FibHash(hash, 8) | SLOT_OCCUPIED

template <typename K, typename V>
void Map<K, V>::uncheckedAdd(K key, V value, bool replace) {
  INIT_HASH_LOOP(key);
  Int freepos = -1;
  while (_state[pos] != SLOT_EMPTY) {
    if (_state[pos] == occ && _cmp(_keys[pos], key) == 0) {
      if (replace) {
        _keys[pos] = key;
        _values[pos] = value;
      }
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
  _keys[pos] = key;
  _values[pos] = value;
  _state[pos] = occ;
  _count++;
}

template <typename K, typename V>
Map<K, V> *Map<K, V>::add(K key, V value, bool replace) {
  if ((_count + _deleted) * 3 / 2 >= _size)
    rebuild();
  uncheckedAdd(key, value, replace);
  return this;
}

template <typename K, typename V>
Map<K, V> *Map<K, V>::add(Arr<Assoc<K, V> > *arr, bool replace) {
  for (Int i = 0; i < arr->len(); i++)
    add(arr->at(i));
  return this;
}

template <typename K, typename V>
Map<K, V> *Map<K, V>::add(Arr<K> *keys, Arr<V> *values, bool replace) {
  require(keys->len() == values->len(), "mismatched array sizes");
  for (Int i = 0; i < keys->len(); i++) {
    add(keys->at(i), values->at(i));
  }
  return this;
}

template <typename K, typename V>
bool Map<K, V>::remove(K key) {
  INIT_HASH_LOOP(key);
  while (_state[pos] != SLOT_EMPTY) {
    if (_state[pos] == occ && _cmp(_keys[pos], key) == 0) {
      memset(_keys + pos, 0, sizeof(K));
      memset(_values + pos, 0, sizeof(V));
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

template <typename K, typename V>
bool Map<K, V>::find(K key, V &value) {
  INIT_HASH_LOOP(key);
  while (_state[pos] != SLOT_EMPTY) {
    if (_state[pos] == occ && _cmp(_keys[pos], key) == 0) {
      value = _values[pos];
      return true;
    }
    pos = next(pos, hash) & mask;
  }
  return false;
}

template <typename K, typename V>
V Map<K, V>::get(K key, V if_absent) {
  V result;
  if (find(key, result))
    return result;
  else
    return if_absent;
}

template <typename K, typename V>
V Map<K, V>::get(K key) {
  V result;
  if (find(key, result))
    return result;
  memset(result, 0, sizeof(V));
  return result;
}

template <typename K, typename V>
V Map<K, V>::at(K key) {
  V result;
  require(find(key, result), "key not found");
  return result;
}

template <typename K, typename V>
bool Map<K, V>::contains(K key) {
  V value;
  return find(key, value);
}

template <typename K, typename V>
Arr<K> *Map<K, V>::keys() {
  Arr<K> *result = new Arr<K>(_count);
  for (Int i = 0; i < _size; i++) {
    if (_state[i] == SLOT_OCCUPIED)
      result->add(_keys[i]);
  }
  return result;
}

template <typename K, typename V>
Arr<V> *Map<K, V>::values() {
  Arr<V> *result = new Arr<V>(_count);
  for (Int i = 0; i < _size; i++) {
    if (_state[i] == SLOT_OCCUPIED)
      result->add(_values[i]);
  }
  return result;
}

template <typename K, typename V>
Arr<Assoc<K, V> > *Map<K, V>::pairs() {
  Arr<Assoc<K, V> > *result = new Arr<Assoc<K, V> >(_count);
  for (Int i = 0; i < _size; i++) {
    if (_state[i] == SLOT_OCCUPIED) {
      Assoc<K, V> m;
      m.key = _keys[i];
      m.value = _values[i];
      result->add(m);
    }
  }
  return result;
}

template <typename K, typename V>
Map<K, V> *Map<K, V>::union_in_place(Map<K, V> *that, bool replace) {
  for (Each it(that); it; it++) {
    add(it.key(), it.value(), replace);
  }
  return this;
}

template <typename K, typename V>
Map<K, V> *Map<K, V>::union_with(Map<K, V> *that, bool replace) {
  return clone()->union_in_place(that, replace);
}

template <typename K, typename V>
Map<K, V> *Map<K, V>::diff_in_place(Map<K, V> *that) {
  for (Each it(that); it; it++) {
    remove(it.key());
  }
  return this;
}

template <typename K, typename V>
Map<K, V> *Map<K, V>::diff_with(Map<K, V> *that) {
  return clone()->diff_in_place(that);
}

template <typename K, typename V>
Map<K, V> *Map<K, V>::intersect_with(Map<K, V> *that) {
  Map<K, V> *result = new Map<K, V>(_cmp, _hash);
  for (Each it(this); it; it++) {
    if (that->contains(it.key()))
      result->add(it.key(), it.value());
  }
  return result;
}

template <typename K, typename V>
Map<K, V> *Map<K, V>::intersect_in_place(Map<K, V> *that) {
  Map<K, V> *tmp = intersect_with(that);
  *this = *tmp;
  return this;
}

template <typename K, typename V>
bool Map<K, V>::eq(Map<K, V> *that) {
  // FIXME: compare values
  if (_count != that->_count)
    return false;
  for (Each it(this); it; it++) {
    if (!that->contains(it.key()))
      return false;
  }
  return true;
}

template <typename K, typename V>
template <typename A, typename F>
A Map<K, V>::fold(A init, F foldfunc) {
  A result = init;
  for (Each it(this); it; it++) {
    result = foldfunc(result, it.key(), it.value());
  }
  return result;
}

template <typename K, typename V>
template <typename V2, typename F>
Map<K, V2> *Map<K, V>::map_values(Map<K, V2> *into, F mapfunc) {
  for (Each it(this); it; it++) {
    into->add(it.key(), mapfunc(it.value()));
  }
  return into;
}

template <typename K, typename V>
template <typename K2, typename V2, typename F, typename G>
Map<K2, V2> *Map<K, V>::map_pairs(Map<K2, V2> *into, F mapfunc, G mapfunc2) {
  for (Each it(this); it; it++) {
    into->add(mapfunc(it.key()), mapfunc2(it.value()));
  }
  return into;
}

template <typename K, typename V>
template <typename F>
Map<K, V> *Map<K, V>::filter(F filterfunc) {
  Map<K, V> *result = new Map<K, V>(_cmp, _hash);
  for (Each it(this); it; it++) {
    if (filterfunc(it.key(), it.value()))
      result->add(it.key(), it.value());
  }
  return result;
}

template <typename K, typename V>
template <typename F>
void Map<K, V>::iter(F iterfunc) {
  for (Each it(this); it; it++) {
    iterfunc(*it);
  }
}

typedef Map<Str *, Str *> Dict;

#undef SLOT_EMPTY
#undef SLOT_OCCUPIED
#undef SLOT_DELETED

#undef INIT_HASH_LOOP
