#pragma once

#include "lib.h"

class BitSet;

typedef Arr<BitSet *> BitMatrix;

class BitSet : public GC {
private:
  Int _bits;
  Int _words;
  Word32 *_data;
  void init(Int n);
  void resize(Int n);
  void expand(Int n);
  Int index(Word n) {
    return n >> 5;
  }
  Word bit(Word n) {
    return 1 << (n & 31);
  }

public:
  class Each {
  private:
    BitSet *_set;
    Int _i;
    Word _off;
    Word32 _mask;
    void next() {
      _i++;
      _mask <<= 1;
      if (_mask == 0) {
        _mask = 1;
        _off++;
      }
    }
    void skip() {
      while (_i < _set->_bits && (_set->_data[_off] & _mask) == 0) {
        next();
      }
    }

  public:
    Each(BitSet *set) {
      _set = set;
      _i = 0;
      _mask = 1;
      _off = 0;
      skip();
    }
    operator bool() {
      return _i < _set->_bits;
    }
    void operator++() {
      next();
      skip();
    }
    void operator++(int dummy) {
      next();
      skip();
    }
    Int operator*() {
      return _i;
    }
  };

  BitSet(Int n) {
    init(n);
    zero();
  }
  Int len() {
    return _bits;
  }
  void zero();
  BitSet(BitSet *set) {
    init(set->_bits);
    memcpy(_data, set->_data, _words * sizeof(Word32));
  }
  BitSet *clone() {
    return new BitSet(this);
  }
  void set(Int i) {
    require(i < _bits, "index out of range");
    _data[index(i)] |= bit(i);
  }
  void clear(Int i) {
    require(i < _bits, "index out of range");
    _data[index(i)] &= ~bit(i);
  }
  bool test(Int i) {
    require(i < _bits, "index out of range");
    return (_data[index(i)] & bit(i)) != 0;
  }
  Int count();
  BitSet *complement();
  BitSet *complement_in_place();
  BitSet *union_with(BitSet *that);
  BitSet *union_in_place(BitSet *that);
  BitSet *intersect_with(BitSet *that);
  BitSet *intersect_in_place(BitSet *that);
  BitSet *diff_with(BitSet *that);
  BitSet *diff_in_place(BitSet *that);
  friend BitMatrix *Transpose(BitMatrix *mat);
  friend BitMatrix *TransitiveClosure(BitMatrix *mat);
};

BitMatrix *MakeBitMatrix(Int n, Int m);
bool IsMatrix(BitMatrix *mat);
BitMatrix *Clone(BitMatrix *mat);
BitMatrix *Transpose(BitMatrix *mat);
BitMatrix *TransitiveClosure(BitMatrix *mat);
