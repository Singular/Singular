#ifndef MPR_COMPLEX_H
#define MPR_COMPLEX_H
/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/* $Id: mpr_complex.h,v 1.19 2002-11-04 17:37:03 bricken Exp $ */

/*
* ABSTRACT - multipolynomial resultants - real floating-point numbers using gmp
*            and complex numbers based on pairs of real floating-point numbers
*
*/

//-> include & define stuff
// must have gmp version >= 2
extern "C" {
#ifdef __cplusplus
#undef __cplusplus
#include <gmp.h>
#define __cplusplus
#else
#include <gmp.h>
#endif
}
#include "numbers.h"
#include "ring.h"
#include "mpr_global.h"

#define ZTOF 1
#define QTOF 2
#define RTOF 3
#define CTOF 4

void setGMPFloatDigits( size_t digits, size_t rest );
size_t getGMPFloatDigits();

//-> class gmp_float
/**
 * @short wrapper class for GNU Multi Precision Floats
 */
class gmp_float
{
public:
  gmp_float( const int v = 0 )
  {
    mpf_init_set_d( t, (double) v );
  }
  gmp_float( const long v )
  {
    mpf_init_set_d( t, (double) v );
  }
  gmp_float( const mprfloat v ) // double
  {
    mpf_init_set_d( t, (double) v );
  }
  gmp_float( const mpf_t v )
  {
    mpf_init_set( t, v );
  }
  gmp_float( const mpz_t v ) // gnu mp Z
  {
    mpf_init( t );
    mpf_set_z( t, v );
  }
  gmp_float( const gmp_float & v ) // copy constructor
  {
    mpf_init_set( t, v.t );
  }

  ~gmp_float()
  {
    mpf_clear( t );
  }

  friend gmp_float operator + ( const gmp_float & a, const gmp_float & b );
  friend gmp_float operator - ( const gmp_float & a, const gmp_float & b );
  friend gmp_float operator * ( const gmp_float & a, const gmp_float & b );
  friend gmp_float operator / ( const gmp_float & a, const gmp_float & b );

  gmp_float & operator += ( const gmp_float & a );
  gmp_float & operator -= ( const gmp_float & a );
  inline gmp_float & operator *= ( const gmp_float & a );
  inline gmp_float & operator /= ( const gmp_float & a );

  friend bool operator == ( const gmp_float & a, const gmp_float & b );
  friend bool operator  > ( const gmp_float & a, const gmp_float & b );
  friend bool operator  < ( const gmp_float & a, const gmp_float & b );
  friend bool operator >= ( const gmp_float & a, const gmp_float & b );
  friend bool operator <= ( const gmp_float & a, const gmp_float & b );

  friend gmp_float operator - ( const gmp_float & a );

  gmp_float & operator = ( const gmp_float & a );
  gmp_float & operator = ( const mpz_t & a );
  gmp_float & operator = ( const mprfloat a );
  gmp_float & operator = ( const long a );

  inline int sign();    // t>0:+1, t==0:0, t<0:-1
  bool isZero();  // t == 0 ?
  bool isOne();   // t == 1 ?
  bool isMOne();  // t == -1 ?

  void setFromStr( char * in );

  // access
  inline const mpf_t *mpfp() const;
  inline mpf_t *_mpfp();

  inline operator double();
  inline operator double() const;

  inline operator int();
  inline operator int() const;

private:
  mpf_t t;
};

// <gmp_float> operator <gmp_float>
inline gmp_float & gmp_float::operator *= ( const gmp_float & a )
{
  mpf_mul( t, t, a.t );
  return *this;
}
inline gmp_float & gmp_float::operator /= ( const gmp_float & a )
{
  mpf_div( t, t, a.t );
  return *this;
}

// <gmp_float> = <*>
inline gmp_float & gmp_float::operator = ( const gmp_float & a )
{
  mpf_set( t, a.t );
  return *this;
}
inline gmp_float & gmp_float::operator = ( const mpz_t & a )
{
  mpf_set_z( t, a );
  return *this;
}
inline gmp_float & gmp_float::operator = ( const mprfloat a )
{
  mpf_set_d( t, (double) a );
  return *this;
}
inline gmp_float & gmp_float::operator = ( const long a )
{
  mpf_set_d( t, (double) a );
  return *this;
}

// cast to double
inline gmp_float::operator double()
{
  return mpf_get_d( t );
}
inline gmp_float::operator double() const
{
  return mpf_get_d( t );
}

// cast to int
inline gmp_float::operator int()
{
  return (int)mpf_get_d( t );
}
inline gmp_float::operator int() const
{
  return (int)mpf_get_d( t );
}

// get sign of real number ( -1: t < 0; 0: t==0; 1: t > 0 )
inline int gmp_float::sign()
{
  return mpf_sgn( t );
}

// access pointer
inline const mpf_t *gmp_float::mpfp() const
{
  return &t;
}

inline mpf_t *gmp_float::_mpfp()
{
  return &t;
}

// built-in functions of GMP
gmp_float abs( const gmp_float & );
gmp_float sqrt( const gmp_float & );
gmp_float hypot( const gmp_float &, const gmp_float & );
//gmp_float pow( const gmp_float &, int & );

// simulated functions using double functions
gmp_float sin( const gmp_float & );
gmp_float cos( const gmp_float & );
gmp_float log( const gmp_float & );
gmp_float exp( const gmp_float & );

gmp_float max( const gmp_float &, const gmp_float & );

gmp_float numberToFloat( number num );
gmp_float numberFieldToFloat( number num, int k );
char *floatToStr( const gmp_float & r, const unsigned int oprec );
//<-

//-> class gmp_complex
/**
 * @short gmp_complex numbers based on
 */
class gmp_complex
{
private:
  gmp_float r, i;

public:
  gmp_complex( const gmp_float re= 0.0, const gmp_float im= 0.0 )
  {
    r= re;
    i= im;
  }
  gmp_complex( const mprfloat re, const mprfloat im = 0.0 )
  {
    r= re;
    i= im;
  }
  gmp_complex( const long re, const long im )
  {
    r= re;
    i= im;
  }
  gmp_complex( const gmp_complex & v )
  {
    r= v.r;
    i= v.i;
  }
  ~gmp_complex() {}

  friend gmp_complex operator + ( const gmp_complex & a, const gmp_complex & b );
  friend gmp_complex operator - ( const gmp_complex & a, const gmp_complex & b );
  friend gmp_complex operator * ( const gmp_complex & a, const gmp_complex & b );
  friend gmp_complex operator / ( const gmp_complex & a, const gmp_complex & b );

  // gmp_complex <operator> real
  inline friend gmp_complex operator + ( const gmp_complex & a, const gmp_float b_d );
  inline friend gmp_complex operator - ( const gmp_complex & a, const gmp_float b_d );
  inline friend gmp_complex operator * ( const gmp_complex & a, const gmp_float b_d );
  inline friend gmp_complex operator / ( const gmp_complex & a, const gmp_float b_d );

  gmp_complex & operator += ( const gmp_complex & a );
  gmp_complex & operator -= ( const gmp_complex & a );
  gmp_complex & operator *= ( const gmp_complex & a );
  gmp_complex & operator /= ( const gmp_complex & a );

  inline friend bool operator == ( const gmp_complex & a, const gmp_complex & b );

  inline gmp_complex & operator = ( const gmp_complex & a );
  inline gmp_complex & operator = ( const gmp_float & f );

  // access to real and imaginary part
  inline gmp_float real() const { return r; }
  inline gmp_float imag() const { return i; }

  inline void real( gmp_float val ) { r = val; }
  inline void imag( gmp_float val ) { i = val; }

  void SmallToZero();
};

// <gmp_complex> = <gmp_complex> operator <gmp_float>
//
inline gmp_complex operator + ( const gmp_complex & a, const gmp_float b_d )
{
  return gmp_complex( a.r + b_d, a.i );
}
inline gmp_complex operator - ( const gmp_complex & a, const gmp_float b_d )
{
  return gmp_complex( a.r - b_d, a.i );
}
inline gmp_complex operator * ( const gmp_complex & a, const gmp_float b_d )
{
  return gmp_complex( a.r * b_d, a.i * b_d );
}
inline gmp_complex operator / ( const gmp_complex & a, const gmp_float b_d )
{
  return gmp_complex( a.r / b_d, a.i / b_d );
}

// <gmp_complex> == <gmp_complex> ?
inline bool operator == ( const gmp_complex & a, const gmp_complex & b )
{
  return ( b.real() == a.real() ) && ( b.imag() == a.imag() );
}

// <gmp_complex> = <gmp_complex>
inline gmp_complex & gmp_complex::operator = ( const gmp_complex & a )
{
  r= a.r;
  i= a.i;
  return *this;
}

// <gmp_complex> = <gmp_complex>
inline gmp_complex & gmp_complex::operator = ( const gmp_float & f )
{
  r= f;
  i= (long int)0;
  return *this;
}

// Returns absolute value of a gmp_complex number
//
inline gmp_float abs( const gmp_complex & c )
{
  return hypot(c.real(),c.imag());
}

gmp_complex sqrt( const gmp_complex & x );

inline gmp_complex numberToComplex( number num )
{
  if (rField_is_long_C()) {
    return *(gmp_complex*)num;
  } else {
    return gmp_complex( numberToFloat(num) );
  }
}

char *complexToStr( gmp_complex & c, const  unsigned int oprec );
//<-

bool complexNearZero( gmp_complex * c, int digits );

#endif /* MPR_COMPLEX_H */

// local Variables: ***
// folded-file: t ***
// compile-command-1: "make installg" ***
// compile-command-2: "make install" ***
// End: ***
