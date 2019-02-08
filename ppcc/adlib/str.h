#pragma once

class Str;

typedef Arr<Str *> StrArr;

class Str : public GC {
private:
  Int _len;
  Int _cap;
  char *_data;
  void resize(Int newcap) {
    char *newdata = (char *) GC_MALLOC_ATOMIC(newcap + 1);
    memcpy(newdata, _data, _len);
    _data = newdata;
    _cap = newcap;
  }
  void init(Int cap) {
    _len = 0;
    _cap = cap;
    // atomic memory is not zeroed by default.
    _data = (char *) GC_MALLOC_ATOMIC(cap + 1);
    _data[0] = '\0';
  }
  void init(const char *s, Int len) {
    _len = _cap = len;
    // atomic memory is not zeroed by default.
    _data = (char *) GC_MALLOC_ATOMIC(_cap + 1);
    memcpy(_data, s, len);
    _data[len] = '\0';
  }

public:
  class Each {
  private:
    Str *_str;
    Int _i;

  public:
    Each(Str *str) {
      _str = str;
      _i = 0;
    }
    operator bool() {
      return _i < _str->_len;
    }
    void operator++() {
      _i++;
    }
    void operator++(int dummy) {
      _i++;
    }
    char &operator*() {
      return _str->_data[_i];
    }
    char operator->() {
      return _str->_data[_i];
    }
  };
  Str(const char *s) {
    init(s, strlen(s));
  }
  Str(const char *s, Int n) {
    init(s, n);
  }
  Str(Int size) {
    init(size);
  }
  Str() {
    init(sizeof(Int) * 2 - 1);
  }
  Str(const Str *str) {
    init(str->_data, str->_len);
  }
  Str *clone() {
    return new Str(this);
  }
  Str *expand(Int newlen) {
    if (newlen > _cap)
      resize(nextPow2(newlen));
    return this;
  }
  Str *shrink(bool fit = true) {
    if (_len != _cap)
      resize(_len);
    return this;
  }
  Str *add(char ch) {
    expand(_len + 1);
    _data[_len++] = ch;
    _data[_len] = 0;
    return this;
  }
  Str *add(const char *s, Int n) {
    expand(_len + n);
    memcpy(_data + _len, s, n);
    _len += n;
    _data[_len] = 0;
    return this;
  }
  Str *add(Str *other) {
    return add(other->_data, other->_len);
  }
  Str *add(const char *s) {
    return add(s, strlen(s));
  }
  Str *remove(Int start, Int count);
  Str *remove(Int at);
  Str *set_len(Int len);
  Str *substr(Int start, Int count);
  Str *range_incl(Int start, Int end) {
    return substr(start, end - start + 1);
  }
  Str *range_excl(Int start, Int end) {
    return substr(start, end - start);
  }
  Str *chomp();
  Int find(Str *str, Int from = 0);
  Int find(char ch, Int from = 0);
  Int find(const char *s, Int from = 0);
  Int find(const char *s, Int n, Int from);
  Int rfind(Str *str);
  Int rfind(char ch);
  Int rfind(const char *s);
  Int rfind(const char *s, Int n);
  Str *replace_count(Int n, Str *pattern, Str *replacement);
  Str *replace_all(Str *pattern, Str *replacement) {
    return replace_count(_len, pattern, replacement);
  }
  StrArr *split(Str *sep);
  StrArr *split(char ch);
  StrArr *split(const char *s);
  StrArr *split(const char *s, Int n);
  StrArr *splitLines();
  bool starts_with(Str *str);
  bool starts_with(const char *s);
  bool starts_with(const char *s, Int n);
  bool ends_with(Str *str);
  bool ends_with(const char *s);
  bool ends_with(const char *s, Int n);
  bool eq(Str *str);
  bool eq(const char *str);
  bool eq(const char *str, Int n);
  Int len() {
    return _len;
  }
  Int count() {
    return _len;
  }
  char &ch(Int i) {
    require(0 <= i && i <= _len, "string index out of range");
    return _data[i];
  }
  unsigned char &byte(Int i) {
    require(0 <= i && i <= _len, "string index out of range");
    return ((unsigned char *)_data)[i];
  }
  char &at(Int i) {
    require(0 <= i && i <= _len, "string index out of range");
    return _data[i];
  }
  char &operator[](Int i) {
    require(0 <= i && i <= _len, "string index out of range");
    return _data[i];
  }
  char *c_str() {
    return _data;
  }
  unsigned char *u_str() {
    return (unsigned char *) _data;
  }
};

Str *StrJoin(StrArr *arr, Str *sep);
Str *StrJoin(StrArr *arr, char ch);
Str *StrJoin(StrArr *arr, const char *sep);
Str *StrJoin(StrArr *arr, const char *sep, Int n);

#define StrArrLit(a) (new StrArr(NUMOF(a), a, CStrToStr))

int Cmp(Str *str1, Str *str2);
int StrCmp(Str *str1, Str *str2);
Str *ToStr(Int x);
Str *ToStr(Int x);
Str *CStrToStr(const char *s);

static inline Str *S(const char *str) {
  return new Str(str);
}

static inline Str *S(Int i) {
  return ToStr(i);
}

static inline Str *S(Word w) {
  return ToStr(w);
}

static inline Str *S(StrArr *arr, const char *sep = " ") {
  return StrJoin(arr, sep);
}

static inline StrArr *A() {
  return new StrArr();
}
static inline StrArr *A(CStr s) {
  return new StrArr(1, S(s));
}

static inline StrArr *A(CStr s1, CStr s2) {
  return new StrArr(2, S(s1), S(s2));
}

static inline StrArr *A(CStr s1, CStr s2, CStr s3) {
  return new StrArr(3, S(s1), S(s2), S(s3));
}

static inline StrArr *A(CStr s1, CStr s2, CStr s3, CStr s4) {
  return new StrArr(4, S(s1), S(s2), S(s3), S(s4));
}
