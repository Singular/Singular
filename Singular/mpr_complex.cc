/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/* $Id: mpr_complex.cc,v 1.24 2000-06-27 12:10:29 pohl Exp $ */

/*
* ABSTRACT - multipolynomial resultants - real floating-point numbers using gmp
*            and complex numbers based on pairs of real floating-point numbers
*
*/

// WARNING! ALWAYS use AllocL and FreeL when alloc. memory for some char* !!

#include "mod2.h"
//#ifdef HAVE_MPR
#include "tok.h"
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

int dummy=mmInit();
static const gmp_float  gmpOne= 1;
static const gmp_float gmpMOne= -1;
static gmp_float gmpRel=0;
static gmp_float diff=0;


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
  size_t bits = 1 + (size_t) ((float)digits * 3.5);
  size_t db = bits+bits;
  bits= bits>64?bits:64;
  gmp_float::setPrecision( db );
  gmp_float::setEqualBits( bits );
  gmp_output_digits= digits;
  mpf_set_default_prec( db );
  mpf_set_prec(*diff.mpfp(),32);
  mpf_set_prec(*gmpRel.mpfp(),32);
  mpf_set_d(*gmpRel.mpfp(),0.1);
  mpf_pow_ui(*gmpRel.mpfp(),*gmpRel.mpfp(),digits);
  mpf_set_prec(*gmpOne.mpfp(),db);
  mpf_set_prec(*gmpMOne.mpfp(),db);
}

size_t getGMPFloatDigits()
{
  return gmp_output_digits;
}
//<-

//-> gmp_float::*
unsigned long int gmp_float::gmp_default_prec_bits= GMP_DEFAULT_PREC_BITS;
unsigned long int gmp_float::gmp_needequal_bits= GMP_NEEDEQUAL_BITS;

void gmp_float::setFromStr( char * in )
{
  // gmp doesn't understand number which begin with "." -- it needs 0.
  // so, insert the zero
  if (*in == '.')
  {
    int len = strlen(in)+2;
    char* c_in = (char*) Alloc(len);
    *c_in = '0';
    strcpy(&(c_in[1]), in);
    
    mpf_set_str( t, c_in, 10 );
    Free((void*)c_in, len);
  }
  else
  {
    mpf_set_str( t, in, 10 );
  }
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
  if (mpf_sgn(t) != -(mpf_sgn(a.t)))
  {
    mpf_add( t, t, a.t);
    return *this;
  }
  if((mpf_sgn(a.t)==0) && (mpf_sgn(t)==0))
  {
    mpf_set_d( t, 0.0);
    return *this;
  }
  mpf_add( t, t, a.t );
  mpf_set(diff.t, t);
  mpf_set_prec(diff.t, 32);
  mpf_div(diff.t, diff.t, a.t);
  mpf_abs(diff.t, diff.t);
  if(diff < gmpRel)
    mpf_set_d( t, 0.0);
  return *this;
}
gmp_float & gmp_float::operator -= ( const gmp_float & a )
{
  if (mpf_sgn(t) != mpf_sgn(a.t))
  {
    mpf_sub( t, t, a.t);
    return *this;
  }
  if((mpf_sgn(a.t)==0) && (mpf_sgn(t)==0))
  {
    mpf_set_d( t, 0.0);
    return *this;
  }
  mpf_sub( t, t, a.t );
  mpf_set(diff.t, t);
  mpf_set_prec(diff.t, 32);
  mpf_div(diff.t, diff.t, a.t);
  mpf_abs(diff.t, diff.t);
  if(diff < gmpRel)
    mpf_set_d( t, 0.0);
  return *this;
}

// <gmp_float> == <gmp_float> ??
bool operator == ( const gmp_float & a, const gmp_float & b )
{
  if(mpf_sgn(a.t) != mpf_sgn(b.t))
    return false;
  if((mpf_sgn(a.t)==0) && (mpf_sgn(b.t)==0))
    return true;
  mpf_sub(diff.t, a.t, b.t);
  mpf_div(diff.t, diff.t, a.t);
  mpf_abs(diff.t, diff.t);
  if(diff < gmpRel)
    return true;
  else
    return false;
}
// t == 0 ?
bool gmp_float::isZero()
{
  return (mpf_sgn( t ) == 0);
}
// t == 1 ?
bool gmp_float::isOne()
{
#ifdef  VARIANTE_1
  return (mpf_cmp_ui( t , 1 ) == 0);
#else
  if (mpf_sgn(t) <= 0)
    return false;
  mpf_sub(diff.t, t, gmpOne.t);
  mpf_abs(diff.t, diff.t);
  if(diff < gmpRel)
    return true;
  else
    return false;
#endif
}
// t == -1 ?
bool gmp_float::isMOne()
{
#ifdef VARIANTE_1
  return (mpf_cmp_si( t , -1 ) == 0);
#else
  if (mpf_sgn(t) >= 0)
    return false;
  mpf_sub(diff.t, t, gmpMOne.t);
  mpf_abs(diff.t, diff.t);
  if(diff < gmpRel)
    return true;
  else
    return false;
#endif
}
bool operator > ( const gmp_float & a, const gmp_float & b )
{
  if (a.t == b.t)
    return false;
  return mpf_cmp( a.t, b.t ) > 0;
}
bool operator < ( const gmp_float & a, const gmp_float & b )
{
  if (a.t == b.t)
    return false;
  return mpf_cmp( a.t, b.t ) < 0;
}
bool operator >= ( const gmp_float & a, const gmp_float & b )
{
  if (a.t == b.t)  
    return true;  
  return mpf_cmp( a.t, b.t ) >= 0;
}
bool operator <= ( const gmp_float & a, const gmp_float & b )
{
  if (a.t == b.t)  
    return true;   
  return mpf_cmp( a.t, b.t ) <= 0;
}

// unary -
gmp_float operator - ( const gmp_float & a )
{
  gmp_float tmp;
  mpf_neg( *(tmp._mpfp()), *(a.mpfp()) );
  return tmp;
}

gmp_float abs( const gmp_float & a )
{
  gmp_float *tmp= new gmp_float();
  mpf_abs( *tmp->_mpfp(), *a.mpfp() );
  return *tmp;
}
gmp_float sqrt( const gmp_float & a )
{
  gmp_float *tmp= new gmp_float();
  mpf_sqrt( *tmp->_mpfp(), *a.mpfp() );
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

gmp_float numberFieldToFloat( number num, int k )
{
  gmp_float r;

  switch (k) 
  {
  case QTOF:
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
    break;
  case RTOF:
    r= *(gmp_float*)num;
    break;
  case CTOF:
    WerrorS("Can not map from field C to field R!");
    break;
  case ZTOF:
  default:
    WerrorS("Ground field not implemented!");
  } // switch

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
    return mstrdup("0");
  }

  if ( ((unsigned int)ABS(exponent) <= oprec)
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
      sprintf(out,"%s0.%se%s%d",csign,in+sign,exponent>=0?"+":"",(int)exponent);
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
  return gmp_complex( a.r * b.r - a.i * b.i,
                  a.r * b.i + a.i * b.r);
}
gmp_complex operator / ( const gmp_complex & a, const gmp_complex & b )
{
  gmp_float d = b.r*b.r + b.i*b.i;
  return gmp_complex( (a.r * b.r + a.i * b.i) / d,
                (a.i * b.r - a.r * b.i) / d);
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
  gmp_float d = b.r*b.r + b.i*b.i;
  r = (r * b.r + i * b.i) / d;
  i = (i * b.r - r * b.i) / d;
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
char *complexToStr( gmp_complex & c, const unsigned int oprec )
{
  char *out,*in_imag,*in_real;

  c.SmallToZero();
  if ( !c.imag().isZero() )
  {

    in_real=floatToStr( c.real(), oprec );         // get real part
    in_imag=floatToStr( abs(c.imag()), oprec );    // get imaginary part

    if (rField_is_long_C())
    {
      int len=(strlen(in_real)+strlen(in_imag)+7+strlen(currRing->parameter[0]))*sizeof(char);
      out=(char*)AllocL(len);
      memset(out,0,len);
      if (  !c.real().isZero() )  // (-23-i*5.43) or (15.1+i*5.3)
	sprintf(out,"(%s%s%s*%s)",in_real,c.imag().sign()>=0?"+":"-",currRing->parameter[0],in_imag);
      else // (-i*43) or (i*34)
      {
        if (c.imag().isOne())
	  sprintf(out,currRing->parameter[0]);
        else if (c.imag().isMOne())
	  sprintf(out,"-%s",currRing->parameter[0]);
	else
	  sprintf(out,"(%s%s*%s)",c.imag().sign()>=0?"":"-",currRing->parameter[0],in_imag);
      }
    }
    else
    {
      int len=(strlen(in_real)+strlen(in_imag)+9) * sizeof(char);
      out=(char*)AllocL( len );
      memset(out,0,len);
      if ( !c.real().isZero() ) 
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

bool complexNearZero( gmp_complex * c, int digits )
{
  gmp_float eps,epsm;

  if ( digits < 1 ) return true;

  eps=pow(10.0,(int)digits);
  //Print("eps: %s\n",floatToStr(eps,gmp_output_digits));
  eps=(gmp_float)1.0/eps;
  epsm=-eps;

  //Print("eps: %s\n",floatToStr(eps,gmp_output_digits));

  if ( c->real().sign() > 0 ) // +
    return (c->real() < eps && (c->imag() < eps && c->imag() > epsm));
  else // -
    return (c->real() > epsm && (c->imag() < eps && c->imag() > epsm));
}

void gmp_complex::SmallToZero()
{
  gmp_float ar=this->real();
  gmp_float ai=this->imag();
  if (ar.isZero() || ai.isZero()) return;
  mpf_abs(*ar.mpfp(), *ar.mpfp());
  mpf_abs(*ai.mpfp(), *ai.mpfp());
  mpf_set_prec(*ar.mpfp(), 32);
  mpf_set_prec(*ai.mpfp(), 32);
  if (ar > ai)
  {
    mpf_div(*ai.mpfp(), *ai.mpfp(), *ar.mpfp());
    if (ai < gmpRel) this->imag(0.0);
  }
  else
  {
    mpf_div(*ar.mpfp(), *ar.mpfp(), *ai.mpfp());
    if (ar < gmpRel) this->real(0.0);
  }
}

//%e

//#endif // HAVE_MPR

// local Variables: ***
// folded-file: t ***
// compile-command-1: "make installg" ***
// compile-command-2: "make install" ***
// End: ***
