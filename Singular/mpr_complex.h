#ifndef MPR_COMPLEX_H
#define MPR_COMPLEX_H
/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/* $Id: mpr_complex.h,v 1.2 1999-05-17 11:31:45 Singular Exp $ */

/* 
* ABSTRACT - multipolynomial resultants - real floating-point numbers using gmp
*            and complex numbers based on pairs of real floating-point numbers
*   
*/

//-> include & define stuff
// must have gmp version >= 2
extern "C" { 
#include <gmp.h>
}
#include "numbers.h"
#include "mpr_global.h"

#define DEFPREC        20         // minimum number of digits (output operations)

#define GMP_DEFAULT_PREC_BITS 512 // size of mantissa of floating-point number
#define GMP_NEEDEQUAL_BITS    512-64 // a == b for the first gmp_equalupto_bits bits
//<-

void setGMPFloatPrecBytes( unsigned long int bytes );
unsigned long int getGMPFloatPrecBytes();
void setGMPFloatDigits( size_t digits );
size_t getGMPFloatDigits();

//-> class gmp_float
/**
 * @short wrapper class for GNU Multi Precision Floats
 */
class gmp_float
{
public:
  gmp_float( const mprfloat v = 0.0 );
  gmp_float( const int v );
  gmp_float( const long v );
  gmp_float( const mpf_t v );
  gmp_float( const mpz_t v );
  gmp_float( const gmp_float & v );

  ~gmp_float();

  friend gmp_float operator + ( const gmp_float & a, const gmp_float & b );
  friend gmp_float operator - ( const gmp_float & a, const gmp_float & b );
  friend gmp_float operator * ( const gmp_float & a, const gmp_float & b );
  friend gmp_float operator / ( const gmp_float & a, const gmp_float & b );

  gmp_float & operator += ( const gmp_float & a );
  gmp_float & operator -= ( const gmp_float & a );
  gmp_float & operator *= ( const gmp_float & a );
  gmp_float & operator /= ( const gmp_float & a );

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

  int sign();    // t>0:+1, t==0:0, t<0:-1
  bool isZero();  // t == 0 ?
  bool isOne();   // t == 1 ?
  bool isMOne();  // t == -1 ?

  bool setFromStr( char * in );
  
  // access 
  inline const mpf_t *mpfp() const;

  operator double();
  operator double() const;

  operator int();
  operator int() const;

public:
  static void setPrecision( const unsigned long int prec ) 
    { gmp_default_prec_bits= prec; }
  static void setEqualBits( const unsigned long int prec )
    { gmp_needequal_bits= prec; }

  static const unsigned long int getPrecision() 
    { return gmp_default_prec_bits; }
  static const unsigned long int getEqualBits() 
    { return gmp_needequal_bits; }

private:
  static unsigned long int gmp_default_prec_bits;
  static unsigned long int gmp_needequal_bits;

  mpf_t t;
};

// built-in functions of GMP
gmp_float abs( const gmp_float & );
gmp_float sqrt( const gmp_float & );
gmp_float hypot( const gmp_float &, const gmp_float & );

// simulated functions using double functions
gmp_float sin( const gmp_float & );
gmp_float cos( const gmp_float & );
gmp_float log( const gmp_float & );
gmp_float exp( const gmp_float & );

gmp_float max( const gmp_float &, const gmp_float & );

gmp_float numberToFloat( number num );
char *floatToStr( const gmp_float & r, const unsigned int oprec );

typedef gmp_float mprfloat_g;
//<-

//-> class complex
/**
 * @short complex numbers based on 
 */
class complex
{
private:
  mprfloat_g r, i;

public:
  complex( const mprfloat_g re= 0.0, const mprfloat_g im= 0.0 );
  complex( const mprfloat re, const mprfloat im = 0.0 );
  complex( const long re, const long im );
  complex( const complex & v );

  ~complex();

  friend complex operator + ( const complex & a, const complex & b );
  friend complex operator - ( const complex & a, const complex & b );
  friend complex operator * ( const complex & a, const complex & b );
  friend complex operator / ( const complex & a, const complex & b );

  friend complex operator + ( const complex & a, const mprfloat_g b_d );
  friend complex operator - ( const complex & a, const mprfloat_g b_d );
  friend complex operator * ( const complex & a, const mprfloat_g b_d );
  friend complex operator / ( const complex & a, const mprfloat_g b_d );

  complex & operator += ( const complex & a );
  complex & operator -= ( const complex & a );
  complex & operator *= ( const complex & a );
  complex & operator /= ( const complex & a );

  friend bool operator == ( const complex & a, const complex & b );

  complex & operator = ( const complex & a );

  // access to real and imaginary part
  inline mprfloat_g real() const { return r; }
  inline mprfloat_g imag() const { return i; }

  inline void real( mprfloat_g val ) { r = val; }
  inline void imag( mprfloat_g val ) { i = val; }
};

mprfloat_g abs( const complex & c );
complex sqrt( const complex & x );

inline complex numberToComplex( number num )
{
  return complex( numberToFloat(num) );
}
char *complexToStr( const complex & c, const  unsigned int oprec );
//<-

#endif MPR_COMPLEX_H

// local Variables: ***
// folded-file: t ***
// compile-command-1: "make installg" ***
// compile-command-2: "make install" ***
// End: *** 
