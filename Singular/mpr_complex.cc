/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/* $Id: mpr_complex.cc,v 1.1 1999-04-29 11:38:50 Singular Exp $ */

/*
* ABSTRACT - multipolynomial resultants - real floating-point numbers using gmp
*            and complex numbers based on pairs of real floating-point numbers
*
*/

#include "mod2.h"
//#ifdef HAVE_MPR
#include "structs.h"
#include "febase.h"
#include "mmemory.h"
#include "numbers.h"
#include "longrat.h"
#include <math.h>
#include "mpr_complex.h"

// this was copied form longrat0.cc
#define SR_HDL(A) ((long)(A))
#define SR_INT    1
#define SR_TO_INT(SR) (((long)SR) >> 2)

#define SIGN_PLUS  1
#define SIGN_SPACE 2
#define SIGN_EMPTY 4

#define EXTRABYTES 4

size_t gmp_output_digits= DEFPREC;

const gmp_float  gmpOne= 1;
const gmp_float gmpMOne= -1;
const gmp_float gmpZero= 0;


/** Set size of mantissa to <bytes> bytes and guess the number of
 * output digits.
 *
 * Set internal gmp floating point precision to (bytes+EXTRABYTES)*8 bits and
 * set external precision (i.e. any number smaller than this is treated as ZERO)
 * to bytes*8 bits.
 * The precision in bytes is the size of the mantissa!
 * Guesses the number of digits <-> precision
 */
void setGMPFloatPrecBytes( unsigned long int bytes )
{
  unsigned long int bits= bytes * 8;
  gmp_float::setPrecision( (bytes+EXTRABYTES)*8 );
  gmp_float::setEqualBits( bits );
  // guess the maximal number of digits for this precision
  gmp_output_digits=  -10 + (size_t)
    ( (((bits>64?bits:64)+2*mp_bits_per_limb-1)/mp_bits_per_limb)
      * mp_bits_per_limb * (log(2)/log(10)));
}

unsigned long int getGMPFloatPrecBytes()
{
  return gmp_float::getEqualBits()/8;
}

void setGMPFloatDigits( size_t digits )
{
  size_t bits= 1 + (size_t) (digits / (log(2)/log(10)));
  bits= bits>64?bits:64;
  gmp_float::setPrecision( bits+EXTRABYTES*8 );
  gmp_float::setEqualBits( bits );
  gmp_output_digits= digits;
}

size_t getGMPFloatDigits()
{
  return gmp_output_digits;
}

//------------------------------------- gmp_float ----------------------------------------------

//-> gmp_float::*
unsigned long int gmp_float::gmp_default_prec_bits= GMP_DEFAULT_PREC_BITS;
unsigned long int gmp_float::gmp_needequal_bits= GMP_NEEDEQUAL_BITS;

gmp_float::gmp_float( const int v )
{
  mpf_init2( t, gmp_default_prec_bits );
  mpf_set_si( t, (signed long int) v );
}
gmp_float::gmp_float( const long v )
{
  mpf_init2( t, gmp_default_prec_bits );
  mpf_set_si( t, (signed long int) v );
}
gmp_float::gmp_float( const mprfloat v )
{
  mpf_init2( t, gmp_default_prec_bits );
  mpf_set_d( t, (double) v );
}
gmp_float::gmp_float( const mpf_t v )
{
  mpf_init2( t, gmp_default_prec_bits );
  mpf_set( t, v );
}
gmp_float::gmp_float( const mpz_t v )
{
  mpf_init2( t, gmp_default_prec_bits );
  mpf_set_z( t, v );
}
gmp_float::gmp_float( const gmp_float & v )
{
  //mpf_init2( t, mpf_get_prec( v.t ) );
  mpf_init2( t, gmp_default_prec_bits );
  mpf_set( t, v.t );
}

gmp_float::~gmp_float()
{
  mpf_clear( t );
}

// <gmp_float> = <gmp_float> operator <gmp_float>
gmp_float operator + ( const gmp_float & a, const gmp_float & b )
{
  gmp_float tmp( a );
  tmp += b;
  return tmp;
}
gmp_float operator - ( const gmp_float & a, const gmp_float & b )
{
  gmp_float tmp( a );
  tmp -= b;
  return tmp;
}
gmp_float operator * ( const gmp_float & a, const gmp_float & b )
{
  gmp_float tmp( a );
  tmp *= b;
  return tmp;
}
gmp_float operator / ( const gmp_float & a, const gmp_float & b )
{
  gmp_float tmp( a );
  tmp /= b;
  return tmp;
}

// <gmp_float> operator <gmp_float>
gmp_float & gmp_float::operator += ( const gmp_float & a )
{
  mpf_add( t, t, a.t );
  return *this;
}
gmp_float & gmp_float::operator -= ( const gmp_float & a )
{
  mpf_sub( t, t, a.t );
  return *this;
}
gmp_float & gmp_float::operator *= ( const gmp_float & a )
{
  mpf_mul( t, t, a.t );
  return *this;
}
gmp_float & gmp_float::operator /= ( const gmp_float & a )
{
  mpf_div( t, t, a.t );
  return *this;
}

// <gmp_float> == <gmp_float> ?? up to the first gmp_float::gmp_needequal_bits bits
bool operator == ( const gmp_float & a, const gmp_float & b )
{
  //return mpf_cmp( a.t, b.t ) == 0;
  return mpf_eq( a.t , b.t , gmp_float::gmp_needequal_bits );
}
bool operator > ( const gmp_float & a, const gmp_float & b )
{
  return mpf_cmp( a.t, b.t ) > 0;
}
bool operator < ( const gmp_float & a, const gmp_float & b )
{
  return mpf_cmp( a.t, b.t ) < 0;
}
bool operator >= ( const gmp_float & a, const gmp_float & b )
{
  return mpf_cmp( a.t, b.t ) >= 0;
}
bool operator <= ( const gmp_float & a, const gmp_float & b )
{
  return mpf_cmp( a.t, b.t ) <= 0;
}

// unary -
gmp_float operator - ( const gmp_float & a )
{
  gmp_float tmp;
  mpf_neg( *(tmp.mpfp()), *(a.mpfp()) );
  return tmp;
}

// <gmp_float> = <*>
gmp_float & gmp_float::operator = ( const gmp_float & a )
{
  mpf_set( t, a.t );
  return *this;
}
gmp_float & gmp_float::operator = ( const mpz_t & a )
{
  mpf_set_z( t, a );
  return *this;
}
gmp_float & gmp_float::operator = ( const mprfloat a )
{
  mpf_set_d( t, (double) a );
  return *this;
}
gmp_float & gmp_float::operator = ( const long a )
{
  mpf_set_si( t, (signed long int) a );
  return *this;
}

// cast to double
gmp_float::operator double()
{
  return mpf_get_d( t );
}
gmp_float::operator double() const
{
  return mpf_get_d( t );
}

// cast to int
gmp_float::operator int()
{
  return (int)mpf_get_d( t );
}
gmp_float::operator int() const
{
  return (int)mpf_get_d( t );
}

// get sign of real number ( -1: t < 0; 0: t==0; 1: t > 0 )
int gmp_float::sign()
{
  return mpf_sgn( t );
}
// t == 0 ?
bool gmp_float::isZero()
{
#ifdef  VARIANTE_1
  return (mpf_sgn( t ) == 0);
#else
  return  mpf_eq( t , gmpZero.t , gmp_float::gmp_needequal_bits );
#endif
}
// t == 1 ?
bool gmp_float::isOne()
{
#ifdef  VARIANTE_1
  return (mpf_cmp_ui( t , 1 ) == 0);
#else
  return mpf_eq( t , gmpOne.t , gmp_float::gmp_needequal_bits );
#endif
}
// t == -1 ?
bool gmp_float::isMOne()
{
#ifdef VARIANTE_1
  return (mpf_cmp_si( t , -1 ) == 0);
#else
  return mpf_eq( t , gmpMOne.t , gmp_float::gmp_needequal_bits );
#endif
}

bool gmp_float::setFromStr( char * in )
{
  return ( mpf_set_str( t, in, 10 ) == 0 );
}

// access pointer
const mpf_t *gmp_float::mpfp() const
{
  return &t;
}

gmp_float abs( const gmp_float & a )
{
  gmp_float *tmp= new gmp_float();
  mpf_abs( *tmp->mpfp(), *a.mpfp() );
  return *tmp;
}
gmp_float sqrt( const gmp_float & a )
{
  gmp_float *tmp= new gmp_float();
  mpf_sqrt( *tmp->mpfp(), *a.mpfp() );
  return *tmp;
}
gmp_float sin( const gmp_float & a )
{
  gmp_float *tmp= new gmp_float( sin((double)a) );
  return *tmp;
}
gmp_float cos( const gmp_float & a )
{
  gmp_float *tmp= new gmp_float( cos((double)a) );
  return *tmp;
}
gmp_float log( const gmp_float & a )
{
  gmp_float *tmp= new gmp_float( log((double)a) );
  return *tmp;
}
gmp_float hypot( const gmp_float & a, const gmp_float & b )
{
#if 1
  gmp_float *tmp= new gmp_float();
  *tmp= sqrt( (a*a) + (b*b) );
  return *tmp;
#else
  gmp_float *tmp= new gmp_float( hypot( (double)a, (double)b ) );
  return *tmp;
#endif
}
gmp_float exp( const gmp_float & a )
{
  gmp_float *tmp= new gmp_float( exp((double)a) );
  return *tmp;
}
gmp_float max( const gmp_float & a, const gmp_float & b )
{
  gmp_float *tmp= new gmp_float();
  a > b ? *tmp= a : *tmp= b;
  return *tmp;
}
//
// WARNING:
// supports only Q to float !!!
//
gmp_float numberToFloat( number num )
{
  gmp_float r;

  //Print("numberToFloat: ");nPrint(num);

  if ( num != NULL ) {
    if (SR_HDL(num) & SR_INT) {
      r= SR_TO_INT(num);
    } else {
      if ( num->s == 0 ) {
        nlNormalize( num );
      }
      if (SR_HDL(num) & SR_INT) {
        r= SR_TO_INT(num);
      } else {
        if ( num->s != 3 ) {
          r= &num->z;
          r/= (mprfloat_g)&num->n;
        } else {
          r= &num->z;
        }
      }
    }
  } else {
    r= 0.0;
  }

  //Print(" --> %s ",floatToStr(r,10));PrintLn();

  return r;
}

// Do some strange things with the mantissa string and the exponent
// to get some nice output string.
char *nicifyFloatStr( char * in, mp_exp_t exponent, size_t oprec, int *size, int thesign )
{
  char *out;

  int sign= (in[0] == '-') ? 1 : 0;
  char csign[2];

  switch (thesign)
  {
    case SIGN_PLUS:
      sign ? strcpy(csign,"-") : strcpy(csign,"+");
      break;
    case SIGN_SPACE:
      sign ? strcpy(csign,"-") : strcpy(csign," ");
      break;
    case SIGN_EMPTY:
    default:
      sign ? strcpy(csign,"-") : strcpy(csign,"");
      break;
  }

  if ( strlen(in) == 0 )
  {
    out= (char*)Alloc0( 2*sizeof(char) );
    *size= 2*sizeof(char);
    strcpy(out,"0");
    return out;
  }

  if ( ((unsigned int)abs(exponent) <= oprec)
       /*|| (exponent+sign >= (int)strlen(in))*/ )
  {
#ifdef mprDEBUG_ALL
    Print(" no exponent %d %d\n",abs(exponent),oprec);
#endif
    if ( exponent+sign < (int)strlen(in) )
    {
      int eexponent= (exponent >= 0) ? 0 : -exponent;
      int eeexponent= (exponent >= 0) ? exponent : 0;
      *size= (strlen(in)+5+eexponent) * sizeof(char);
      out= (char*)Alloc0(*size);

      strcpy(out,csign);
      strncat(out,in+sign,eeexponent);

      if (exponent == 0)
        strcat(out,"0.");
      else if ( exponent > 0 )
        strcat(out,".");
      else
      {
        strcat(out,"0.");
        memset(out+strlen(out),'0',eexponent);
      }
      strcat(out,in+sign+eeexponent);
    }
    else if ( exponent+sign > (int)strlen(in) )
    {
      *size= (strlen(in)+exponent+2)*sizeof(char);
      out= (char*)Alloc0(*size);
      sprintf(out,"%s%s",csign,in+sign);
      memset(out+strlen(out),'0',exponent-strlen(in)+sign);
    }
    else
    {
      *size= (strlen(in)+2) * sizeof(char);
      out= (char*)Alloc0(*size);
      sprintf(out,"%s%s",csign,in+sign);
    }
  }
  else
  {
#ifdef mprDEBUG_ALL
    Print(" exponent %d %d\n",exponent,oprec);
#endif
    if ( exponent > 0 )
    {
      int c=1,d=10;
      while ( exponent / d > 0 )
      { // count digits
        d*=10;
        c++;
      }
      *size= (strlen(in)+12+c) * sizeof(char);
      out= (char*)Alloc0(*size);
      sprintf(out,"%s0.%se%d",csign,in+sign,(unsigned int)exponent);
    }
    else
    {
      *size=2;
      out= (char*)Alloc0(*size);
      strcpy(out,"0");
    }
  }
  return out;
}

char *floatToStr( const gmp_float & r, const size_t oprec )
{
#if 1
  mp_exp_t exponent;
  int size,insize;
  char *nout,*out,*in;

  insize= (oprec+2) * sizeof(char);
  in= (char*)Alloc0( insize );

  mpf_get_str(in,&exponent,10,oprec,*(r.mpfp()));
  if ( (exponent > 0) && (exponent < (int)oprec) && (strlen(in)-(in[0]=='-'?1:0) == oprec) ) {
    in= (char*)ReAlloc( in, insize, (exponent+oprec+2) * sizeof(char) );
    insize= (exponent+oprec+2) * sizeof(char);
    int newprec= exponent+oprec;
    mpf_get_str(in,&exponent,10,newprec,*(r.mpfp()));
  }
  nout= nicifyFloatStr( in, exponent, oprec, &size, SIGN_EMPTY );
  Free( (ADDRESS) in, insize );
  out= (char*)Alloc0( (strlen(nout)+1) * sizeof(char) );
  strcpy( out, nout );
  Free( (ADDRESS) nout, size );
  return out;
#else
  char *out= (char*)Alloc0( (1024) * sizeof(char) );
  sprintf(out,"% .10f",(double)r);
  return out;
#endif
}
//<-

//------------------------------------- complex ------------------------------------------------

//-> complex::*
// constructors
//
complex::complex( const mprfloat_g re, const mprfloat_g im )
{
  r= re;
  i= im;
}
complex::complex( const mprfloat re, const mprfloat im )
{
  r= re;
  i= im;
}
complex::complex( const long re, const long im )
{
  r= re;
  i= im;
}
complex::complex( const complex & v )
{
  r= v.r;
  i= v.i;
}
complex::~complex()
{
}

// <complex> = <complex> operator <complex>
//
complex operator + ( const complex & a, const complex & b )
{
  return complex( a.r + b.r, a.i + b.i );
}
complex operator - ( const complex & a, const complex & b )
{
  return complex( a.r - b.r, a.i - b.i );
}
complex operator * ( const complex & a, const complex & b )
{
  return complex( a.real() * b.real() - a.imag() * b.imag(),
                  a.real() * b.imag() + a.imag() * b.real());
}
complex operator / ( const complex & a, const complex & b )
{
  mprfloat_g ar = abs(b.real());
  mprfloat_g ai = abs(b.imag());
  mprfloat_g nr, ni, t, d;
  if (ar <= ai) {
    t = b.real() / b.imag();
    d = b.imag() * ((mprfloat_g)1 + t*t);
    nr = (a.real() * t + a.imag()) / d;
    ni = (a.imag() * t - a.real()) / d;
  } else {
    t = b.imag() / b.real();
    d = b.real() * ((mprfloat_g)1 + t*t);
    nr = (a.real() + a.imag() * t) / d;
    ni = (a.imag() - a.real() * t) / d;
  }
  return complex( nr, ni );
}

// <complex> = <complex> operator <mprfloat_g>
//
complex operator + ( const complex & a, const mprfloat_g b_d )
{
  return complex( a.r + b_d, a.i );
}
complex operator - ( const complex & a, const mprfloat_g b_d )
{
  return complex( a.r - b_d, a.i );
}
complex operator * ( const complex & a, const mprfloat_g b_d )
{
  return complex( a.r * b_d, a.i * b_d );
}
complex operator / ( const complex & a, const mprfloat_g b_d )
{
  return complex( a.r / b_d, a.i / b_d );
}

// <complex> operator <complex>
//
complex & complex::operator += ( const complex & b )
{
  r+=b.r;
  i+=b.i;
  return *this;
}
complex & complex::operator -= ( const complex & b )
{
  r-=b.r;
  i-=b.i;
  return *this;
}
complex & complex::operator *= ( const complex & b )
{
  mprfloat_g f = r * b.r - i * b.i;
  i = r * b.i + i * b.r;
  r = f;
  return *this;
}
complex & complex::operator /= ( const complex & b )
{
  mprfloat_g ar = abs(b.r);
  mprfloat_g ai = abs(b.i);
  mprfloat_g nr, ni, t, d;
  if (ar <= ai) {
    t = b.r / b.i;
    d = b.i * ((mprfloat_g)1 + t*t);
    nr = (r * t + i) / d;
    ni = (i * t - r) / d;
  } else {
    t = b.i / b.r;
    d = b.r * ((mprfloat_g)1 + t*t);
    nr = (r + i * t) / d;
    ni = (i - r * t) / d;
  }
  r = nr;
  i = ni;
  return *this;
}

// <complex> == <complex> ?
bool operator == ( const complex & a, const complex & b )
{
  return ( b.real() == a.real() ) && ( b.imag() == a.imag() );
}

// <complex> = <complex>
complex & complex::operator = ( const complex & a )
{
  r= a.r;
  i= a.i;
  return *this;
}

inline mprfloat_g complex::real() const
{
  return r;
}
inline mprfloat_g complex::imag() const
{
  return i;
}

inline void complex::real( const mprfloat_g val )
{
  r= val;
}
inline void complex::imag( const mprfloat_g val )
{
  i= val;
}

// Returns absolute value of a complex number
//
mprfloat_g abs( const complex & c )
{
  return hypot(c.real(),c.imag());
}

// Returns square root of complex number
//
complex sqrt( const complex & x )
{
  mprfloat_g r = abs(x);
  mprfloat_g nr, ni;
  if (r == 0.0) {
    nr = ni = r;
  } else if ( x.real() > 0) {
    nr = sqrt((mprfloat_g)0.5 * (r + x.real()));
    ni = x.imag() / nr / (mprfloat_g)2;
  } else {
    ni = sqrt((mprfloat_g)0.5 * (r - x.real()));
    if (x.imag() < 0) {
      ni = - ni;
    }
    nr = x.imag() / ni / (mprfloat_g)2;
  }
  complex *tmp= new complex(nr, ni);
  return *tmp;
}

// converts a number to a complex
//
complex numberToComplex( number num )
{
  return complex( numberToFloat(num) );
}

char *complexToStr( const complex & c, const size_t oprec )
{
  char *out,*in_imag,*in_real;

  if ( !c.imag().isZero() ) {
    in_real=floatToStr( c.real(), oprec );         // get real part
    in_imag=floatToStr( abs(c.imag()), oprec );    // get imaginary part
    out= (char*)Alloc0( (strlen(in_real)+strlen(in_imag)+6) * sizeof(char) );
    sprintf(out,"%s%s%s",in_real,c.imag().sign() >= 0 ? " + i ":" - i ",in_imag);
    Free( in_real, (strlen(in_real)+1)*sizeof(char) );
    Free( in_imag, (strlen(in_imag)+1)*sizeof(char) );
  } else {
    out= floatToStr( c.real(), oprec );
  }
  return out;
}
//<-

//#endif // HAVE_MPR

// local Variables: ***
// folded-file: t ***
// compile-command-1: "make installg" ***
// compile-command-2: "make install" ***
// End: ***
