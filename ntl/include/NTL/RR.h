#ifndef NTL_RR__H
#define NTL_RR__H

#include <NTL/ZZ.h>
#include <NTL/xdouble.h>
#include <NTL/quad_float.h>

NTL_OPEN_NNS


class RR {

public:

ZZ x;
long e;

RR() {  e = 0; }

inline RR(INIT_VAL_TYPE, const ZZ& a);
inline RR(INIT_VAL_TYPE, int a);
inline RR(INIT_VAL_TYPE, long a);
inline RR(INIT_VAL_TYPE, unsigned int a);
inline RR(INIT_VAL_TYPE, unsigned long a);
inline RR(INIT_VAL_TYPE, float a);
inline RR(INIT_VAL_TYPE, double a);
inline RR(INIT_VAL_TYPE, const xdouble& a);
inline RR(INIT_VAL_TYPE, const quad_float& a);
inline RR(INIT_VAL_TYPE, const char *a);  // read from string
inline RR(INIT_VAL_TYPE, const RR& a);


inline RR& operator=(double a);

RR(RR& z, INIT_TRANS_TYPE) : x(z.x, INIT_TRANS), e(z.e) { } 





~RR() { }

const ZZ& mantissa() const { return x; }
long exponent() const { return e; }

static long prec;
static void SetPrecision(long p);
static long precision() { return prec; }

static long oprec;
static void SetOutputPrecision(long p);
static long OutputPrecision() { return oprec; }

#ifdef NTL_TRANSITION
private:
RR& operator=(const RR&);
RR(const RR&);
#endif

};


long IsZero(const RR& a);
long IsOne(const RR& a);
long sign(const RR& a);
void clear(RR& z);
void set(RR& z);
void swap(RR& a, RR& b);

void add(RR& z, const RR& a, const RR& b);

void add(RR& z, const RR& a, double b);
inline void add(RR& z, double a, const RR& b) { add(z, b, a); }



void sub(RR& z, const RR& a, const RR& b);

void sub(RR& z, const RR& a, double b);
void sub(RR& z, double a, const RR& b);

void negate(RR& z, const RR& a);

void abs(RR& z, const RR& a);
inline RR abs(const RR& a)
   { RR z; abs(z, a); NTL_OPT_RETURN(RR, z); }
inline RR fabs(const RR& a)
   { RR z; abs(z, a); NTL_OPT_RETURN(RR, z); }

void mul(RR& z, const RR& a, const RR& b);

void mul(RR& z, const RR& a, double b);
inline void mul(RR& z, double a, const RR& b) { mul(z, b, a); }

void sqr(RR& z, const RR& a);
inline RR sqr(const RR& a)
   { RR z; sqr(z, a); NTL_OPT_RETURN(RR, z); }

void div(RR& z, const RR& a, const RR& b);

void div(RR& z, const RR& a, double b);
void div(RR& z, double a, const RR& b);

void inv(RR& z, const RR& a);
inline RR inv(const RR& a)
   { RR z; inv(z, a); NTL_OPT_RETURN(RR, z); }

// operator notation:

inline RR operator+(const RR& a, const RR& b)
   { RR x; add(x, a, b); NTL_OPT_RETURN(RR, x); }

inline RR operator+(const RR& a, double b)
   { RR x; add(x, a, b); NTL_OPT_RETURN(RR, x); }

inline RR operator+(double a, const RR& b)
   { RR x; add(x, a, b); NTL_OPT_RETURN(RR, x); }

inline RR& operator+=(RR& x, const RR& b)
   { add(x, x, b); return x; } 

inline RR& operator+=(RR& x, double b)
   { add(x, x, b); return x; } 



inline RR operator-(const RR& a, const RR& b)
   { RR x; sub(x, a, b); NTL_OPT_RETURN(RR, x); }

inline RR operator-(const RR& a, double b)
   { RR x; sub(x, a, b); NTL_OPT_RETURN(RR, x); }

inline RR operator-(double a, const RR& b)
   { RR x; sub(x, a, b); NTL_OPT_RETURN(RR, x); }

inline RR& operator-=(RR& x, const RR& b)
   { sub(x, x, b); return x; } 

inline RR& operator-=(RR& x, double b)
   { sub(x, x, b); return x; } 



inline RR operator*(const RR& a, const RR& b)
   { RR x; mul(x, a, b); NTL_OPT_RETURN(RR, x); }

inline RR operator*(const RR& a, double b)
   { RR x; mul(x, a, b); NTL_OPT_RETURN(RR, x); }

inline RR operator*(double a, const RR& b)
   { RR x; mul(x, a, b); NTL_OPT_RETURN(RR, x); }

inline RR& operator*=(RR& x, const RR& b)
   { mul(x, x, b); return x; } 

inline RR& operator*=(RR& x, double b)
   { mul(x, x, b); return x; } 


inline RR operator/(const RR& a, const RR& b)
   { RR x; div(x, a, b); NTL_OPT_RETURN(RR, x); }

inline RR operator/(const RR& a, double b)
   { RR x; div(x, a, b); NTL_OPT_RETURN(RR, x); }

inline RR operator/(double a, const RR& b)
   { RR x; div(x, a, b); NTL_OPT_RETURN(RR, x); }

inline RR& operator/=(RR& x, const RR& b)
   { div(x, x, b); return x; } 

inline RR& operator/=(RR& x, double b)
   { div(x, x, b); return x; } 


inline RR operator-(const RR& a)
   { RR x; negate(x, a); NTL_OPT_RETURN(RR, x); }


inline RR& operator++(RR& x) { add(x, x, 1); return x; }
inline void operator++(RR& x, int) { add(x, x, 1); }
inline RR& operator--(RR& x) { sub(x, x, 1); return x; }
inline void operator--(RR& x, int) { sub(x, x, 1); }



long compare(const RR& a, const RR& b);

long compare(const RR& a, double b);
inline long compare(double a, const RR& b) { return -compare(b, a); }


long operator==(const RR& a, const RR& b);
inline long operator!=(const RR& a, const RR& b) { return !(a == b); }
inline long operator<=(const RR& a, const RR& b) { return compare(a, b) <= 0; }
inline long operator>=(const RR& a, const RR& b) { return compare(a, b) >= 0; }
inline long operator <(const RR& a, const RR& b) { return compare(a, b)  < 0; }
inline long operator >(const RR& a, const RR& b) { return compare(a, b)  > 0; }

long operator==(const RR& a, double b);
inline long operator!=(const RR& a, double b) { return !(a == b); }
inline long operator<=(const RR& a, double b) { return compare(a, b) <= 0; }
inline long operator>=(const RR& a, double b) { return compare(a, b) >= 0; }
inline long operator <(const RR& a, double b) { return compare(a, b)  < 0; }
inline long operator >(const RR& a, double b) { return compare(a, b)  > 0; }

inline long operator==(double a, const RR& b) { return (b == a); }
inline long operator!=(double a, const RR& b) { return !(a == b); }
inline long operator<=(double a, const RR& b) { return compare(a, b) <= 0; }
inline long operator>=(double a, const RR& b) { return compare(a, b) >= 0; }
inline long operator <(double a, const RR& b) { return compare(a, b)  < 0; }
inline long operator >(double a, const RR& b) { return compare(a, b)  > 0; }

void ceil(RR& z, const RR& a);
inline RR ceil(const RR& a)
   { RR z; ceil(z, a); NTL_OPT_RETURN(RR, z); }

void floor(RR& z, const RR& a);
inline RR floor(const RR& a)
   { RR z; floor(z, a); NTL_OPT_RETURN(RR, z); }

void trunc(RR& z, const RR& a);
inline RR trunc(const RR& a)
   { RR z; trunc(z, a); NTL_OPT_RETURN(RR, z); }

void round(RR& z, const RR& a);
inline RR round(const RR& a)
   { RR z; round(z, a); NTL_OPT_RETURN(RR, z); }

void RoundToPrecision(RR& z, const RR& a, long p);
inline RR RoundToPrecision(const RR& a, long p)
   { RR z; RoundToPrecision(z, a, p); NTL_OPT_RETURN(RR, z); }


void conv(RR& z, const ZZ& a);
void conv(RR& z, long a);
inline void conv(RR& z, int a) { conv(z, long(a)); }
void conv(RR& z, unsigned long a);
inline void conv(RR& z, unsigned int a) { conv(z, (unsigned long)(a)); }
void conv(RR& z, const char *s);
void conv(RR& z, double a);
inline void conv(RR& z, float a) { conv(z, double(a)); }
void conv(RR& z, const xdouble& a);
void conv(RR& z, const quad_float& a);

void conv(RR& z, const RR& a);



inline RR::RR(INIT_VAL_TYPE, int a) { e = 0; conv(*this, a); }
inline RR::RR(INIT_VAL_TYPE, long a) { e = 0; conv(*this, a); }
inline RR::RR(INIT_VAL_TYPE, unsigned int a) { e = 0; conv(*this, a); }
inline RR::RR(INIT_VAL_TYPE, unsigned long a) { e = 0; conv(*this, a); }
inline RR::RR(INIT_VAL_TYPE, float a) { e = 0; conv(*this, a); }
inline RR::RR(INIT_VAL_TYPE, double a) { e = 0; conv(*this, a); }

inline RR::RR(INIT_VAL_TYPE, const RR& a) { e = 0; conv(*this, a); }
inline RR::RR(INIT_VAL_TYPE, const ZZ& a) { e = 0; conv(*this, a); }
inline RR::RR(INIT_VAL_TYPE, const xdouble& a) { e = 0; conv(*this, a); }
inline RR::RR(INIT_VAL_TYPE, const quad_float& a) { e = 0; conv(*this, a); }
inline RR::RR(INIT_VAL_TYPE, const char *a) { e = 0; conv(*this, a); }


inline RR to_RR(int a) { return RR(INIT_VAL, a); }
inline RR to_RR(long a) { return RR(INIT_VAL, a); }
inline RR to_RR(unsigned int a) { return RR(INIT_VAL, a); }
inline RR to_RR(unsigned long a) { return RR(INIT_VAL, a); }
inline RR to_RR(float a) { return RR(INIT_VAL, a); }
inline RR to_RR(double a) { return RR(INIT_VAL, a); }
inline RR to_RR(const ZZ& a) { return RR(INIT_VAL, a); }
inline RR to_RR(const RR& a) { return RR(INIT_VAL, a); }
inline RR to_RR(const xdouble& a) { return RR(INIT_VAL, a); }
inline RR to_RR(const quad_float& a) { return RR(INIT_VAL, a); }
inline RR to_RR(const char *a) { return RR(INIT_VAL, a); }

inline RR& RR::operator=(double a) { conv(*this, a); return *this; }

void conv(ZZ& z, const RR& a);
void conv(long& z, const RR& a);
void conv(double& z, const RR& a);
void conv(xdouble& z, const RR& a);
void conv(quad_float& z, const RR& a);

inline void conv(int& z, const RR& a) 
   { long t; conv(t, a); z = int(t); }

inline void conv(float& z, const RR& a) 
   { double t; conv(t, a); z = float(t); }

inline int to_int(const RR& a) { int z; conv(z, a); return z; }
inline long to_long(const RR& a) { long z; conv(z, a); return z; }
inline float to_float(const RR& a) { float z; conv(z, a); return z; }
inline double to_double(const RR& a) { double z; conv(z, a); return z; }

inline xdouble to_xdouble(const RR& a) 
   { xdouble z; conv(z, a); return z; }
inline quad_float to_quad_float(const RR& a) 
   { quad_float z; conv(z, a); return z; }

inline ZZ to_ZZ(const RR& a)
   { ZZ z; conv(z, a); NTL_OPT_RETURN(ZZ, z); }

void CeilToZZ(ZZ& z, const RR& a);
inline ZZ CeilToZZ(const RR& a)
   { ZZ z; CeilToZZ(z, a); NTL_OPT_RETURN(ZZ, z); }

void TruncToZZ(ZZ& z, const RR& a);
inline ZZ TruncToZZ(const RR& a)
   { ZZ z; TruncToZZ(z, a); NTL_OPT_RETURN(ZZ, z); }

void RoundToZZ(ZZ& z, const RR& a);
inline ZZ RoundToZZ(const RR& a)
   { ZZ z; RoundToZZ(z, a); NTL_OPT_RETURN(ZZ, z); }

inline void FloorToZZ(ZZ& z, const RR& a) { conv(z, a); }
inline ZZ FloorToZZ(const RR& a)
   { ZZ z; conv(z, a); NTL_OPT_RETURN(ZZ, z); }

void MakeRR(RR& z, const ZZ& a,  long e);
inline RR MakeRR(const ZZ& a,  long e)
   { RR z; MakeRR(z, a, e); NTL_OPT_RETURN(RR, z); }

void random(RR& z);
inline RR random_RR() 
   { RR z; random(z); NTL_OPT_RETURN(RR, z); }


void power(RR& z, const RR& a, long e);
inline RR power(const RR& a, long e)
   { RR z; power(z, a, e); NTL_OPT_RETURN(RR, z); }

void power2(RR& z, long e);

inline RR power2_RR(long e)
   { RR z; power2(z, e); NTL_OPT_RETURN(RR, z); }

void SqrRoot(RR& x, const RR& a);
inline RR SqrRoot(const RR& a)
   { RR z; SqrRoot(z, a); NTL_OPT_RETURN(RR, z); }
inline RR sqrt(const RR& a)
   { RR z; SqrRoot(z, a); NTL_OPT_RETURN(RR, z); }

void exp(RR& res, const RR& x);
inline RR exp(const RR& a)
   { RR z; exp(z, a); NTL_OPT_RETURN(RR, z); }

void log(RR& res, const RR& x);
inline RR log(const RR& a)
   { RR z; log(z, a); NTL_OPT_RETURN(RR, z); }

void log10(RR& res, const RR& x);
inline RR log10(const RR& a)
   { RR z; log10(z, a); NTL_OPT_RETURN(RR, z); }

void expm1(RR& res, const RR& x);
inline RR expm1(const RR& a)
   { RR z; expm1(z, a); NTL_OPT_RETURN(RR, z); }

void log1p(RR& res, const RR& x);
inline RR log1p(const RR& a)
   { RR z; log1p(z, a); NTL_OPT_RETURN(RR, z); }

void pow(RR& res, const RR& x, const RR& y);
inline RR pow(const RR& x, const RR& y)
   { RR z; pow(z, x, y); NTL_OPT_RETURN(RR, z); }

void ComputePi(RR& res);
inline RR ComputePi_RR()
   { RR z; ComputePi(z); NTL_OPT_RETURN(RR, z); }

void sin(RR& res, const RR& x);
inline RR sin(const RR& a)
   { RR z; sin(z, a); NTL_OPT_RETURN(RR, z); }

void cos(RR& res, const RR& x);
inline RR cos(const RR& a)
   { RR z; cos(z, a); NTL_OPT_RETURN(RR, z); }




NTL_CLOSE_NNS

#endif
