
#ifndef NTL_vec_GF2__H
#define NTL_vec_GF2__H

#include <NTL/WordVector.h>
#include <NTL/GF2.h>

NTL_OPEN_NNS

class vec_GF2;

class subscript_GF2 {
public:
   vec_GF2& v;
   long i;

   subscript_GF2(vec_GF2& vv, long ii) : v(vv), i(ii) { }

   inline const subscript_GF2& operator=(const subscript_GF2&) const;
   inline const subscript_GF2& operator=(GF2) const;
   inline const subscript_GF2& operator=(long) const;

   inline operator GF2() const;
};

class const_subscript_GF2 {
public:
   const vec_GF2& v;
   long i;

   const_subscript_GF2(const vec_GF2& vv, long ii) : v(vv), i(ii) { }

   inline operator GF2() const;

private:
   void operator=(const const_subscript_GF2&); // disabled

};
   

class vec_GF2 {

public:

// these should be private, but they are not

   WordVector rep;

   long _len;  // length (in bits)
   long _maxlen;  // (MaxLength << 1) | (fixed)

   // invariants: rep.length() "tracks" length() ( = _len)
   //             All bits in positions >= length are zero.

   // Note:       rep.MaxLength() may exceed the value
   //             indicated by MaxLength().
   

//the following are "really" public


   vec_GF2() : _len(0), _maxlen(0) {}
   vec_GF2(INIT_SIZE_TYPE, long n) : _len(0), _maxlen(0) { SetLength(n); }
   vec_GF2(const vec_GF2& a) : _len(0), _maxlen(0) { *this = a; }

   vec_GF2& operator=(const vec_GF2& a);

   ~vec_GF2() {}

   void kill();

   void SetLength(long n);
   void SetMaxLength(long n);
   void FixLength(long n);

   long length() const { return _len; }
   long MaxLength() const { return _maxlen >> 1; }  
   long allocated() const { return rep.MaxLength() * NTL_BITS_PER_LONG; }
   long fixed() const { return _maxlen & 1; }


   vec_GF2(vec_GF2& x, INIT_TRANS_TYPE) : 
      rep(x.rep, INIT_TRANS), _len(x._len), _maxlen(x._maxlen) { }

   GF2 get(long i) const;
   void put(long i, GF2 a);
   void put(long i, long a) { put(i, to_GF2(a)); }

   subscript_GF2 operator[](long i) 
      { return subscript_GF2(*this, i); }

   subscript_GF2 operator()(long i) 
      { return subscript_GF2(*this, i-1); }

   const_subscript_GF2 operator[](long i) const 
      { return const_subscript_GF2(*this, i); }

   const_subscript_GF2 operator()(long i) const 
      { return const_subscript_GF2(*this, i-1); }

};

inline subscript_GF2::operator GF2() const
{
   return v.get(i);
}

inline const_subscript_GF2::operator GF2() const
{
   return v.get(i);
}

inline const subscript_GF2& 
   subscript_GF2::operator=(const subscript_GF2& a) const
   { v.put(i, a.v.get(a.i));  return *this; }

inline const subscript_GF2& 
   subscript_GF2::operator=(GF2 a) const
   { v.put(i, a);  return *this; }

inline const subscript_GF2& 
   subscript_GF2::operator=(long a) const
   { v.put(i, a);  return *this; }

inline const subscript_GF2& operator+=(const subscript_GF2& x, GF2 b)
   { x = x + b; return x; }  

inline const subscript_GF2& operator+=(const subscript_GF2& x, long b)
   { x = x + b; return x; }  

inline const subscript_GF2& operator-=(const subscript_GF2& x, GF2 b)
   { x = x - b; return x; }  

inline const subscript_GF2& operator-=(const subscript_GF2& x, long b)
   { x = x - b; return x; }  

inline const subscript_GF2& operator*=(const subscript_GF2& x, GF2 b)
   { x = x * b; return x; }  

inline const subscript_GF2& operator*=(const subscript_GF2& x, long b)
   { x = x * b; return x; }  

inline const subscript_GF2& operator/=(const subscript_GF2& x, GF2 b)
   { x = x / b; return x; }  

inline const subscript_GF2& operator/=(const subscript_GF2& x, long b)
   { x = x / b; return x; }  

inline const subscript_GF2& operator++(const subscript_GF2& x) 
   { x = x + 1; return x; }

inline void operator++(const subscript_GF2& x, int) 
   { x = x + 1; }

inline const subscript_GF2& operator--(const subscript_GF2& x) 
   { x = x - 1; return x; }

inline void operator--(const subscript_GF2& x, int) 
   { x = x - 1; }

void swap(vec_GF2& x, vec_GF2& y);
void append(vec_GF2& v, GF2 a);
void append(vec_GF2& v, const vec_GF2& a);

long operator==(const vec_GF2& a, const vec_GF2& b);
inline long operator!=(const vec_GF2& a, const vec_GF2& b)
   { return !(a == b); }

NTL_SNS ostream& operator<<(NTL_SNS ostream& s, const vec_GF2& a);
NTL_SNS istream& operator>>(NTL_SNS istream& s, vec_GF2& a);

void shift(vec_GF2& x, const vec_GF2& a, long n);
// x = a shifted n places, i.e., if l = a.length(),
//    x.length() = l, x[i] = a[i-n] for 0 <= i-n < l,
//    and x[i] = 0 for all other i such that 0 <= i < l.

inline vec_GF2 shift(const vec_GF2& a, long n)
   { vec_GF2 x; shift(x, a, n); NTL_OPT_RETURN(vec_GF2, x); }

void reverse(vec_GF2& x, const vec_GF2& a);

inline vec_GF2 reverse(const vec_GF2& a)
   { vec_GF2 x; reverse(x, a); NTL_OPT_RETURN(vec_GF2, x); }

void random(vec_GF2& x, long n);
inline vec_GF2 random_vec_GF2(long n)
   { vec_GF2 x; random(x, n); NTL_OPT_RETURN(vec_GF2, x); }

long weight(const vec_GF2& a);

void mul(vec_GF2& x, const vec_GF2& a, GF2 b);
inline void mul(vec_GF2& x, GF2 a, const vec_GF2& b)
   { mul(x, b, a); }

inline void mul(vec_GF2& x, const vec_GF2& a, long b)
   { mul(x, a, to_GF2(b)); }
inline void mul(vec_GF2& x, long a, const vec_GF2& b)
   { mul(x, b, a); }

void add(vec_GF2& x, const vec_GF2& a, const vec_GF2& b);

inline void sub(vec_GF2& x, const vec_GF2& a, const vec_GF2& b)
   { add(x, a, b); }

void clear(vec_GF2& x);

inline void negate(vec_GF2& x, const vec_GF2& a)
   { x = a; }

inline void InnerProduct(GF2& x, const vec_GF2& a, const vec_GF2& b)
   { x = to_GF2(InnerProduct(a.rep, b.rep)); }

long IsZero(const vec_GF2& a);

vec_GF2 operator+(const vec_GF2& a, const vec_GF2& b);

vec_GF2 operator-(const vec_GF2& a, const vec_GF2& b);

inline vec_GF2 operator-(const vec_GF2& a)
   { return a; }

inline vec_GF2 operator*(const vec_GF2& a, GF2 b)
   { vec_GF2 x; mul(x, a, b); NTL_OPT_RETURN(vec_GF2, x); }

inline vec_GF2 operator*(const vec_GF2& a, long b)
   { vec_GF2 x; mul(x, a, b); NTL_OPT_RETURN(vec_GF2, x); }

inline vec_GF2 operator*(GF2 a, const vec_GF2& b)
   { vec_GF2 x; mul(x, a, b); NTL_OPT_RETURN(vec_GF2, x); }

inline vec_GF2 operator*(long a, const vec_GF2& b)
   { vec_GF2 x; mul(x, a, b); NTL_OPT_RETURN(vec_GF2, x); }


inline GF2 operator*(const vec_GF2& a, const vec_GF2& b)
   { return to_GF2(InnerProduct(a.rep, b.rep)); }

// assignment operator notation:

inline vec_GF2& operator+=(vec_GF2& x, const vec_GF2& a)
{ 
   add(x, x, a);
   return x;
}

inline vec_GF2& operator-=(vec_GF2& x, const vec_GF2& a)
{ 
   sub(x, x, a);
   return x;
}

inline vec_GF2& operator*=(vec_GF2& x, GF2 a)
{ 
   mul(x, x, a);
   return x;
}

inline vec_GF2& operator*=(vec_GF2& x, long a)
{ 
   mul(x, x, a);
   return x;
}

void VectorCopy(vec_GF2& x, const vec_GF2& a, long n);
inline vec_GF2 VectorCopy(const vec_GF2& a, long n)
   { vec_GF2 x; VectorCopy(x, a, n); NTL_OPT_RETURN(vec_GF2, x); }

NTL_CLOSE_NNS


#endif


