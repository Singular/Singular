
#ifndef NTL_GF2__H
#define NTL_GF2__H

#include <NTL/ZZ.h>

NTL_OPEN_NNS

class GF2 {
public:

long _GF2__rep;


GF2() { _GF2__rep = 0; }

GF2(const GF2& a) : _GF2__rep(a._GF2__rep) { }

~GF2() { }

GF2& operator=(const GF2& a) { _GF2__rep = a._GF2__rep; return *this; }

GF2& operator=(long a) { _GF2__rep = a & 1; return *this; }

static long modulus() { return 2; }
static GF2 zero() { return GF2(); }

};

inline void conv(GF2& x, long a) { x._GF2__rep = a & 1; }
inline GF2 to_GF2(long a) 
   { GF2 x; conv(x, a); return x; }

inline void conv(GF2& x, const ZZ& a) { x._GF2__rep = IsOdd(a); }
inline GF2 to_GF2(const ZZ& a) 
   { GF2 x; conv(x, a); return x; }

inline long rep(GF2 a) { return a._GF2__rep; }


inline void clear(GF2& x) { x._GF2__rep = 0; }
inline void set(GF2& x) { x._GF2__rep = 1; }

inline void swap(GF2& x, GF2& y)
   { long t; t = x._GF2__rep; x._GF2__rep = y._GF2__rep; y._GF2__rep = t; }

inline void add(GF2& x, GF2 a, GF2 b)
   { x._GF2__rep = a._GF2__rep ^ b._GF2__rep; }

inline void sub(GF2& x, GF2 a, GF2 b)
   { x._GF2__rep = a._GF2__rep ^ b._GF2__rep; }

inline void negate(GF2& x, GF2 a)
   { x._GF2__rep = a._GF2__rep; }

inline void add(GF2& x, GF2 a, long b)
   { x._GF2__rep = a._GF2__rep ^ (b & 1); }

inline void add(GF2& x, long a, GF2 b)
   { x._GF2__rep = (a & 1) ^ b._GF2__rep; }

inline void sub(GF2& x, GF2 a, long b)
   { x._GF2__rep = a._GF2__rep ^ (b & 1); }

inline void sub(GF2& x, long a, GF2 b)
   { x._GF2__rep = (a & 1) ^ b._GF2__rep; }

inline GF2 operator+(GF2 a, GF2 b)
    { GF2 x; add(x, a, b); return x; }

inline GF2 operator+(GF2 a, long b)
    { GF2 x; add(x, a, b); return x; }

inline GF2 operator+(long a, GF2 b)
    { GF2 x; add(x, a, b); return x; }

inline GF2 operator-(GF2 a, GF2 b)
    { GF2 x; sub(x, a, b); return x; }

inline GF2 operator-(GF2 a, long b)
    { GF2 x; sub(x, a, b); return x; }

inline GF2 operator-(long a, GF2 b)
    { GF2 x; sub(x, a, b); return x; }

inline GF2 operator-(GF2 a)
   { GF2 x; negate(x, a); return x; }

inline GF2& operator+=(GF2& x, GF2 b)
   { add(x, x, b); return x; }

inline GF2& operator+=(GF2& x, long b)
   { add(x, x, b); return x; }

inline GF2& operator-=(GF2& x, GF2 b)
   { sub(x, x, b); return x; }

inline GF2& operator-=(GF2& x, long b)
   { sub(x, x, b); return x; }

inline GF2& operator++(GF2& x) { add(x, x, 1); return x; }
inline void operator++(GF2& x, int) { add(x, x, 1); }
inline GF2& operator--(GF2& x) { sub(x, x, 1); return x; }
inline void operator--(GF2& x, int) { sub(x, x, 1); }


inline void mul(GF2& x, GF2 a, GF2 b)
   { x._GF2__rep = a._GF2__rep & b._GF2__rep; }

inline void mul(GF2& x, GF2 a, long b)
   { x._GF2__rep = a._GF2__rep & b; }

inline void mul(GF2& x, long a, GF2 b)
   { x._GF2__rep = a & b._GF2__rep; }

inline void sqr(GF2& x, GF2 a)
   { x = a; }

inline GF2 sqr(GF2 a)
   { return a; }

inline GF2 operator*(GF2 a, GF2 b)
    { GF2 x; mul(x, a, b); return x; }

inline GF2 operator*(GF2 a, long b)
    { GF2 x; mul(x, a, b); return x; }

inline GF2 operator*(long a, GF2 b)
    { GF2 x; mul(x, a, b); return x; }


inline GF2& operator*=(GF2& x, GF2 b)
   { mul(x, x, b); return x; }

inline GF2& operator*=(GF2& x, long b)
   { mul(x, x, b); return x; }


void div(GF2& x, GF2 a, GF2 b);
void div(GF2& x, long a, GF2 b);
void div(GF2& x, GF2 a, long b);

void inv(GF2& x, GF2 a);

inline GF2 inv(GF2 a)
   { GF2 x; inv(x, a); return x; }

inline GF2 operator/(GF2 a, GF2 b)
    { GF2 x; div(x, a, b); return x; }

inline GF2 operator/(GF2 a, long b)
    { GF2 x; div(x, a, b); return x; }

inline GF2 operator/(long a, GF2 b)
    { GF2 x; div(x, a, b); return x; }


inline GF2& operator/=(GF2& x, GF2 b)
   { div(x, x, b); return x; }

inline GF2& operator/=(GF2& x, long b)
   { div(x, x, b); return x; }


void power(GF2& x, GF2 a, long e);
inline GF2 power(GF2 a, long e)
   { GF2 x; power(x, a, e); return x; }


inline long IsZero(GF2 a)
   { return a._GF2__rep == 0; }

inline long IsOne(GF2 a)
   { return a._GF2__rep == 1; }

inline long operator==(GF2 a, GF2 b)
   { return a._GF2__rep == b._GF2__rep; }

inline long operator!=(GF2 a, GF2 b)
   { return !(a == b); }

inline long operator==(GF2 a, long b) { return a._GF2__rep == (b & 1); }
inline long operator==(long a, GF2 b) { return (a & 1) == b._GF2__rep; }

inline long operator!=(GF2 a, long b) { return !(a == b); }
inline long operator!=(long a, GF2 b) { return !(a == b); }

inline void random(GF2& x)
   { x._GF2__rep = RandomBnd(2); }

inline GF2 random_GF2()
   { GF2 x; random(x); return x; }

NTL_SNS ostream& operator<<(NTL_SNS ostream& s, GF2 a);

NTL_SNS istream& operator>>(NTL_SNS istream& s, GF2& x);

NTL_CLOSE_NNS

#endif

