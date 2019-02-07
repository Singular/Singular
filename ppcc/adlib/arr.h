#pragma once

template <typename T>
class Arr : public GC {
  template <typename U>
  friend class Arr;
private:
  Int _len;
  Int _cap;
  T *_data;
  void resize(Int newcap) {
    T *_newdata = (T *) GC_MALLOC(newcap * sizeof(T));
    memcpy(_newdata, _data, _len * sizeof(T));
    _data = _newdata;
    _cap = newcap;
  }
  void init(Int cap) {
    _len = 0;
    _cap = cap;
    _data = (T *) GC_MALLOC(cap * sizeof(T));
  }
  void init(const T *p, Int cap) {
    _len = _cap = cap;
    _data = (T *) GC_MALLOC(cap * sizeof(T));
    memcpy(_data, p, _len * sizeof(T));
  }

public:
  class Each {
  private:
    Arr *_arr;
    Int _i;

  public:
    Each(Arr *arr) {
      _arr = arr;
      _i = 0;
    }
    Int index() {
      return _i;
    }
    operator bool() {
      return _i < _arr->_len;
    }
    void operator++() {
      _i++;
    }
    void operator++(int dummy) {
      _i++;
    }
    T &operator*() {
      return _arr->_data[_i];
    }
    T operator->() {
      return _arr->_data[_i];
    }
  };
  Arr(Int n, const T *data) {
    init(data, n);
  }
  Arr(const T *data, T sentinel) {
    Int n = 0;
    while (data[n] != sentinel)
      n++;
    init(data, n);
  }
  template <typename D, typename F>
  Arr(const D *data, D sentinel, F initfunc);
  template <typename D, typename F>
  Arr(Int n, const D *data, F initfunc);
  Arr(Int size) {
    init(size);
  }
  Arr(Int size, T arg1) {
    init(size);
    add(arg1);
  }
  Arr(Int size, T arg1, T arg2) {
    init(size);
    add(arg1);
    add(arg2);
  }
  Arr(Int size, T arg1, T arg2, T arg3) {
    init(size);
    add(arg1);
    add(arg2);
    add(arg3);
  }
  Arr(Int size, T arg1, T arg2, T arg3, T arg4) {
    init(size);
    add(arg1);
    add(arg2);
    add(arg3);
    add(arg4);
  }
  Arr() {
    init(2);
  }
  Arr(const Arr *arr) {
    init(arr->_data, arr->_len);
  }
  Arr *clone() {
    return new Arr<T>(this);
  }
  Arr<T> *expand(Int newlen) {
    if (newlen > _cap)
      resize(nextPow2(newlen));
    return this;
  }
  Arr<T> *shrink(bool fit = true) {
    if (_len != _cap) {
      if (fit || _len * 3 / 2 + 1 < _cap)
        resize(_len);
    }
    return this;
  }
  Arr<T> *add(T item) {
    expand(_len + 1);
    _data[_len++] = item;
    return this;
  }
  Arr<T> *add(const T *p, Int n) {
    expand(_len + n);
    memcpy(_data + _len, p, n * sizeof(T));
    _len += n;
    return this;
  }
  Arr<T> *add(Arr<T> *other) {
    return add(other->_data, other->_len);
  }
  Arr<T> *pop(Int n = 1) {
    if (n > _len)
      n = _len;
    memset(_data + _len - n, 0, n * sizeof(T));
    _len -= n;
    return this;
  }
  Arr<T> *remove(Int at);
  Arr<T> *remove(Int start, Int count);
  Arr<T> *set_len(Int len);
  Arr<T> *fill(Int start, Int count, T value);
  Arr<T> *subarr(Int start, Int count);
  Arr<T> *range_incl(Int start, Int end) {
    return subarr(start, end - start + 1);
  }
  Arr<T> *range_excl(Int start, Int end) {
    return subarr(start, end - start);
  }
  T shift();
  T first() {
    require(_len > 0, "last element of empty array");
    return _data[0];
  }
  T last() {
    require(_len > 0, "last element of empty array");
    return _data[_len - 1];
  }
  Int len() {
    return _len;
  }
  Int count() {
    return _len;
  }
  bool eq(Arr<T> *that);
  T &item(Int i) {
    require(0 <= i && i < _len, "index out of range");
    return _data[i];
  }
  T &at(Int i) {
    require(0 <= i && i < _len, "index out of range");
    return _data[i];
  }
  T *c_mem() {
    return _data;
  }
  template <typename U, typename F>
  Arr<U> *map(F mapfunc);
  template <typename F>
  Arr<T> *map_in_place(F mapfunc);
  template <typename U, typename F>
  Arr<U> *mapi(F mapfunc);
  template <typename F>
  Arr<T> *mapi_in_place(F mapfunc);
  template <typename F>
  Arr<T> *filter(F filterfunc);
  template <typename F>
  Arr<T> *filter_in_place(F filterfunc);
  template <typename A, typename F>
  A fold(A init, F foldfunc);
  template <typename F>
  Arr<T> *sort(F cmpfunc);
  template <typename F>
  Arr<T> *sort_in_place(F cmpfunc);
  template <typename F>
  void iter(F iterfunc);
  template <typename F>
  void iteri(F iterfunc);
};

template <typename T>
template <typename D, typename F>
Arr<T>::Arr(Int n, const D *data, F initfunc) {
  init(n);
  for (Int i = 0; i < n; i++) {
    add(initfunc(data[i]));
  }
}
template <typename T>
template <typename D, typename F>
Arr<T>::Arr(const D *data, D sentinel, F initfunc) {
  Int n = 0;
  while (data[n] != sentinel)
    n++;
  init(n);
  for (Int i = 0; i < n; i++) {
    add(initfunc(data[i]));
  }
}

template <typename T>
template <typename F>
Arr<T> *Arr<T>::mapi_in_place(F mapfunc) {
  for (Int i = 0; i < _len; i++) {
    _data[i] = mapfunc(i, _data[i]);
  }
  return this;
}

template <typename T>
template <typename U, typename F>
Arr<U> *Arr<T>::mapi(F mapfunc) {
  Arr<U> *result = new Arr<U>(_cap);
  result->_len = _len;
  for (Int i = 0; i < _len; i++) {
    result->_data[i] = mapfunc(i, _data[i]);
  }
  return result;
}

template <typename T>
template <typename F>
Arr<T> *Arr<T>::map_in_place(F mapfunc) {
  for (Int i = 0; i < _len; i++) {
    _data[i] = mapfunc(_data[i]);
  }
  return this;
}

template <typename T>
template <typename U, typename F>
Arr<U> *Arr<T>::map(F mapfunc) {
  Arr<U> *result = new Arr<U>(_cap);
  result->_len = _len;
  for (Int i = 0; i < _len; i++) {
    result->_data[i] = mapfunc(_data[i]);
  }
  return result;
}

template <typename T>
template <typename F>
Arr<T> *Arr<T>::filter_in_place(F filterfunc) {
  Int p = 0;
  for (Int i = 0; i < _len; i++) {
    if (filterfunc(_data[i]))
      _data[p++] = _data[i];
  }
  pop(_len - p);
  shrink(false);
  return this;
}

template <typename T>
template <typename F>
Arr<T> *Arr<T>::filter(F filterfunc) {
  return clone()->filter_in_place(filterfunc);
}

template <typename T>
template <typename F>
void Arr<T>::iter(F iterfunc) {
  for (Int i = 0; i < _len; i++) {
    iterfunc(_data[i]);
  }
}

template <typename T>
template <typename F>
void Arr<T>::iteri(F iterfunc) {
  for (Int i = 0; i < _len; i++) {
    iterfunc(i, _data[i]);
  }
}

template <typename T>
template <typename F>
Arr<T> *Arr<T>::sort_in_place(F cmpfunc) {
  T *in = (T *) GC_MALLOC(sizeof(T) * _len);
  T *out = (T *) GC_MALLOC(sizeof(T) * _len);
  memcpy(in, _data, sizeof(T) * _len);
  Int step = 1;
  while (step < _len) {
    for (Int i = 0; i < _len; i += step * 2) {
      Int p = i, l = i, r = i + step, lmax = l + step, rmax = r + step;
      if (rmax > _len)
        rmax = _len;
      if (lmax > _len)
        lmax = _len;
      while (l < lmax && r < rmax) {
        int c = cmpfunc(in[l], in[r]);
        if (c < 0) {
          out[p++] = in[l++];
        } else {
          out[p++] = in[r++];
        }
      }
      while (l < lmax) {
        out[p++] = in[l++];
      }
      while (r < rmax) {
        out[p++] = in[r++];
      }
    }
    T *tmp = in;
    in = out;
    out = tmp;
    step += step;
  }
  _data = in;
  _cap = _len;
  return this;
}

template <typename T>
template <typename F>
Arr<T> *Arr<T>::sort(F cmpfunc) {
  return clone()->sort_in_place(cmpfunc);
}

template <typename T>
template <typename A, typename F>
A Arr<T>::fold(A init, F foldfunc) {
  A result = init;
  for (Int i = 0; i < _len; i++) {
    result = foldfunc(result, _data[i]);
  }
  return result;
}

template <typename T>
bool Arr<T>::eq(Arr<T> *that) {
  if (_len != that->_len)
    return false;
  for (Int i = 0; i < _len; i++)
    if (!_data[i]->eq(that->_data[i]))
      return false;
  return true;
}

template <typename T>
Arr<T> *Arr<T>::remove(Int start, Int count) {
  require(start >= 0 && count >= 0 && start + count <= _len,
    "index out of range");
  Int end = start + count;
  if (count <= 0) return 0;
  memmove(_data + start, _data + end, sizeof(T) * (_len - end));
  memset(_data + _len - count, 0, sizeof(T) * count);
  _len -= count;
  return shrink(false);
}

template <typename T>
Arr<T> *Arr<T>::remove(Int at) {
  return remove(at, 1);
}

template <typename T>
Arr<T> *Arr<T>::set_len(Int len) {
  if (len > _len) {
    expand(len);
    memset(_data + _len, 0, sizeof(T) * (len - _len));
    _len = len;
  } else if (len < _len) {
    memset(_data + len, 0, sizeof(T) * (_len - len));
    _len = len;
    return shrink(false);
  }
  return this;
}

template <typename T>
Arr<T> *Arr<T>::fill(Int start, Int count, T value) {
  require(start >= 0 && count >= 0 && start + count <= _len,
    "index out of range");
  Int end = start + count;
  for (Int i = start; i < end; i++) {
    _data[i] = value;
  }
  return this;
}

template <typename T>
Arr<T> *Arr<T>::subarr(Int start, Int count) {
  require(start >= 0 && start + count <= _len, "index out of range");
  if (count <= 0)
    return new Arr<T>();
  return new Arr<T>(count, _data + start);
}

template <typename T>
T Arr<T>::shift() {
  require(_len > 0, "empty array");
  T result = _data[0];
  remove(0, 1);
  return result;
}
