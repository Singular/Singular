/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/* $Id: mpr_complex.cc,v 1.10 1999-07-02 15:01:43 wenk Exp $ */

/*
* ABSTRACT - multipolynomial resultants - real floating-point numbers using gmp
*            and complex numbers based on pairs of real floating-point numbers
*
*/

// WARNING! ALWAYS use AllocL and FreeL when alloc. memory for some char* !!

#include "mod2.h"
//#ifdef HAVE_MPR
#include "structs.h"
#include "febase.h"
#include "mmemory.h"
#include "numbers.h"
#include "longrat.h"
#include <math.h>
#include "mpr_complex.h"

//%s
// this was copied form longrat0.cc
// and will be used in numberToFloat.
// Make sure that it is up to date!!
#define SR_HDL(A) ((long)(A))
#define SR_INT    1
#define SR_TO_INT(SR) (((long)SR) >> 2)

#define SIGN_PLUS  1
#define SIGN_SPACE 2
#define SIGN_EMPTY 4

#define EXTRABYTES 4

size_t gmp_output_digits= DEFPREC;

//-> setGMPFloat*
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

// Sets the lenght of the mantissa to <digits> digits
void setGMPFloatDigits( size_t digits )
{
  size_t bits= 1 + (size_t) (digits / (log(2)/log(10)));
  bits= bits>64?bits:64;
  //  gmp_float::setPrecision( bits+EXTRABYTES*8 );
  gmp_float::setPrecision( bits+(bits/2) );
  gmp_float::setEqualBits( bits );
  gmp_output_digits= digits;
}

size_t getGMPFloatDigits()
{
  return gmp_output_digits;
}
//<-

//-> gmp_float::*
unsigned long int gmp_float::gmp_default_prec_bits= GMP_DEFAULT_PREC_BITS;
unsigned long int gmp_float::gmp_needequal_bits= GMP_NEEDEQUAL_BITS;

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
// number to float, number = Q, R, C
// makes a COPY of num! (Ist das gut?)
//
gmp_float numberToFloat( number num )
{
  gmp_float r;

  if ( rField_is_Q() )
  {
    if ( num != NULL )
    {
      if (SR_HDL(num) & SR_INT)
      {
        r= SR_TO_INT(num);
      }
      else
      {
        if ( num->s == 0 )
        {
          nlNormalize( num );
        }
        if (SR_HDL(num) & SR_INT)
        {
          r= SR_TO_INT(num);
        }
        else
        {
          if ( num->s != 3 )
          {
            r= &num->z;
            r/= (gmp_float)&num->n;
          }
          else
          {
            r= &num->z;
          }
        }
      }
    }
    else
    {
      r= 0.0;
    }
  }
  else if (rField_is_long_R() || rField_is_long_C())
  {
    r= *(gmp_float*)num;
  }
  else if ( rField_is_R() )
  {
    // Add some code here :-)
    WerrorS("Ground field not implemented!");
  }
  else
  {
    WerrorS("Ground field not implemented!");
  }

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
      sign ? strcpy(csign,"-") : strcpy(csign,"+");  //+123, -123
      break;
    case SIGN_SPACE:
      sign ? strcpy(csign,"-") : strcpy(csign," ");  // 123, -123
      break;
    case SIGN_EMPTY:
    default:
      sign ? strcpy(csign,"-") : strcpy(csign,"");   //123, -123
      break;
  }

  if ( strlen(in) == 0 )
  {
    *size= 2*sizeof(char);
    out= (char*)AllocL( *size );
    strcpy(out,"0");
    return out;
  }

  if ( ((unsigned int)abs(exponent) <= oprec)
       /*|| (exponent+sign >= (int)strlen(in))*/ )
  {
    if ( exponent+sign < (int)strlen(in) )
    {
      int eexponent= (exponent >= 0) ? 0 : -exponent;
      int eeexponent= (exponent >= 0) ? exponent : 0;
      *size= (strlen(in)+15+eexponent) * sizeof(char);
      out= (char*)AllocL(*size);
      memset(out,0,*size);

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
      *size= (strlen(in)+exponent+12)*sizeof(char);
      out= (char*)AllocL(*size);
      memset(out,0,*size);
      sprintf(out,"%s%s",csign,in+sign);
      memset(out+strlen(out),'0',exponent-strlen(in)+sign);
    }
    else
    {
      *size= (strlen(in)+2) * sizeof(char) + 10;
      out= (char*)AllocL(*size);
      memset(out,0,*size);
      sprintf(out,"%s%s",csign,in+sign);
    }
  }
  else
  {
//      if ( exponent > 0 )
//      {
      int c=1,d=10;
      while ( exponent / d > 0 )
      { // count digits
        d*=10;
        c++;
      }
      *size= (strlen(in)+12+c) * sizeof(char) + 10;
      out= (char*)AllocL(*size);
      memset(out,0,*size);
      sprintf(out,"%s0.%se%d",csign,in+sign,(unsigned int)exponent);
//      }
//      else
//      {
//        *size=2;
//        out= (char*)AllocL(*size);
//        strcpy(out,"0");
//      }
  }
  return out;
}

char *floatToStr( const gmp_float & r, const unsigned int oprec )
{
#if 1
  mp_exp_t exponent;
  int size,insize;
  char *nout,*out,*in;

  insize= (oprec+2) * sizeof(char) + 10;
  in= (char*)AllocL( insize );

  mpf_get_str(in,&exponent,10,oprec,*(r.mpfp()));

  if ( (exponent > 0)
  && (exponent < (int)oprec)
  && (strlen(in)-(in[0]=='-'?1:0) == oprec) )
  {
    FreeL( (ADDRESS) in );
    insize= (exponent+oprec+2) * sizeof(char) + 10;
    in= (char*)AllocL( insize );
    int newprec= exponent+oprec;
    mpf_get_str(in,&exponent,10,newprec,*(r.mpfp()));
  }
  nout= nicifyFloatStr( in, exponent, oprec, &size, SIGN_EMPTY );
  FreeL( (ADDRESS) in );
  out= (char*)AllocL( (strlen(nout)+1) * sizeof(char) );
  strcpy( out, nout );
  FreeL( (ADDRESS) nout );

  return out;
#else
  // for testing purpose...
  char *out= (char*)AllocL( (1024) * sizeof(char) );
  sprintf(out,"% .10f",(double)r);
  return out;
#endif
}
//<-

//-> gmp_complex::*
// <gmp_complex> = <gmp_complex> operator <gmp_complex>
//
gmp_complex operator + ( const gmp_complex & a, const gmp_complex & b )
{
  return gmp_complex( a.r + b.r, a.i + b.i );
}
gmp_complex operator - ( const gmp_complex & a, const gmp_complex & b )
{
  return gmp_complex( a.r - b.r, a.i - b.i );
}
gmp_complex operator * ( const gmp_complex & a, const gmp_complex & b )
{
  return gmp_complex( a.real() * b.real() - a.imag() * b.imag(),
                  a.real() * b.imag() + a.imag() * b.real());
}
gmp_complex operator / ( const gmp_complex & a, const gmp_complex & b )
{
  gmp_float ar = abs(b.real());
  gmp_float ai = abs(b.imag());
  gmp_float nr, ni, t, d;
  if (ar <= ai)
  {
    t = b.real() / b.imag();
    d = b.imag() * ((gmp_float)1 + t*t);
    nr = (a.real() * t + a.imag()) / d;
    ni = (a.imag() * t - a.real()) / d;
  }
  else
  {
    t = b.imag() / b.real();
    d = b.real() * ((gmp_float)1 + t*t);
    nr = (a.real() + a.imag() * t) / d;
    ni = (a.imag() - a.real() * t) / d;
  }
  return gmp_complex( nr, ni );
}

// <gmp_complex> operator <gmp_complex>
//
gmp_complex & gmp_complex::operator += ( const gmp_complex & b )
{
  r+=b.r;
  i+=b.i;
  return *this;
}
gmp_complex & gmp_complex::operator -= ( const gmp_complex & b )
{
  r-=b.r;
  i-=b.i;
  return *this;
}
gmp_complex & gmp_complex::operator *= ( const gmp_complex & b )
{
  gmp_float f = r * b.r - i * b.i;
  i = r * b.i + i * b.r;
  r = f;
  return *this;
}
gmp_complex & gmp_complex::operator /= ( const gmp_complex & b )
{
  gmp_float ar = abs(b.r);
  gmp_float ai = abs(b.i);
  gmp_float nr, ni, t, d;
  if (ar <= ai)
  {
    t = b.r / b.i;
    d = b.i * ((gmp_float)1 + t*t);
    nr = (r * t + i) / d;
    ni = (i * t - r) / d;
  }
  else
  {
    t = b.i / b.r;
    d = b.r * ((gmp_float)1 + t*t);
    nr = (r + i * t) / d;
    ni = (i - r * t) / d;
  }
  r = nr;
  i = ni;
  return *this;
}

// Returns square root of gmp_complex number
//
gmp_complex sqrt( const gmp_complex & x )
{
  gmp_float r = abs(x);
  gmp_float nr, ni;
  if (r == (gmp_float) 0.0)
  {
    nr = ni = r;
  }
  else if ( x.real() > (gmp_float)0)
  {
    nr = sqrt((gmp_float)0.5 * (r + x.real()));
    ni = x.imag() / nr / (gmp_float)2;
  }
  else
  {
    ni = sqrt((gmp_float)0.5 * (r - x.real()));
    if (x.imag() < (gmp_float)0)
    {
      ni = - ni;
    }
    nr = x.imag() / ni / (gmp_float)2;
  }
  gmp_complex *tmp= new gmp_complex(nr, ni);
  return *tmp;
}

// converts a gmp_complex to a string ( <real part> + I * <imaginary part> )
//
char *complexToStr( const gmp_complex & c, const unsigned int oprec )
{
  char *out,*in_imag,*in_real;

  if ( !c.imag().isZero() )
  {

      in_real=floatToStr( c.real(), oprec );         // get real part
    in_imag=floatToStr( abs(c.imag()), oprec );    // get imaginary part

    if (rField_is_long_C())
    {
      int len=(strlen(in_real)+strlen(in_imag)+5+strlen(currRing->parameter[0]))*sizeof(char);
      out=(char*)AllocL(len);
      memset(out,0,len);
      if (  !c.real().isZero() ) 
	sprintf(out,"(%s%s%s*%s)",in_real,c.imag().sign()>=0?"+":"-",currRing->parameter[0],in_imag);
      else
	sprintf(out,"(%s%s*%s)",c.imag().sign()>=0?"":"-",currRing->parameter[0],in_imag);
    }
    else
    {
      int len=(strlen(in_real)+strlen(in_imag)+8) * sizeof(char);
      out=(char*)AllocL( len );
      memset(out,0,len);
      if (  !c.real().isZero() ) 
	sprintf(out,"(%s%s%s)",in_real,c.imag().sign()>=0?"+I*":"-I*",in_imag);
      else
	sprintf(out,"(%s%s)",c.imag().sign()>=0?"I*":"-I*",in_imag);
    }
    FreeL( (ADDRESS) in_real );
    FreeL( (ADDRESS) in_imag );
  }
  else 
  {
    out= floatToStr( c.real(), oprec );
  }

  return out;
}
//<-
//%e

//#endif // HAVE_MPR

// local Variables: ***
// folded-file: t ***
// compile-command-1: "make installg" ***
// compile-command-2: "make install" ***
// End: ***
