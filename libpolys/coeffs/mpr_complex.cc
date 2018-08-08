/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/

/*
* ABSTRACT - multipolynomial resultants - real floating-point numbers using gmp
*            and complex numbers based on pairs of real floating-point numbers
*
*/

// WARNING! ALWAYS use omAlloc and FreeL when alloc. memory for some char* !!


#include "misc/auxiliary.h"

#include "reporter/reporter.h"

#include "coeffs/coeffs.h"
#include "coeffs/numbers.h"

#include "coeffs/mpr_complex.h"

#include "coeffs/longrat.h"

#include <cmath>


//%s
// this was copied form longrat0.cc
// and will be used in numberToFloat.
// Make sure that it is up to date!!
#define SR_HDL(A) ((long)(A))
#define SR_TO_INT(SR) (((long)SR) >> 2)

#define SIGN_PLUS  1
#define SIGN_SPACE 2
#define SIGN_EMPTY 4

#define EXTRABYTES 4

#define DEFPREC        20         // minimum number of digits (output operations)
size_t gmp_output_digits= DEFPREC;

static gmp_float *gmpRel=NULL;
static gmp_float *diff=NULL;


/** Set size of mantissa
 *  digits - the number of output digits (basis 10)
 *  the size of mantissa consists of two parts:
 *    the "output" part a and the "rest" part b.
 *  According to the GMP-precision digits is
 *  recomputed to bits (basis 2).
 *  Two numbers a, b are equal if
 *    | a - b | < | a | * 0.1^digits .
 *  In this case we have a - b = 0 .
 *  The epsilon e is e=0.1^(digits+rest) with
 *  1+e != 1, but 1+0.1*e = 1.
 */
void setGMPFloatDigits( size_t digits, size_t rest )
{
  size_t bits = 1 + (size_t) ((float)digits * 3.5);
  size_t rb = 1 + (size_t) ((float)rest * 3.5);
  size_t db = bits+rb;
  gmp_output_digits= digits;
  mpf_set_default_prec( db );
  if (diff!=NULL) delete diff;
  diff=new gmp_float(0.0);
  mpf_set_prec(*diff->_mpfp(),32);
  if (gmpRel!=NULL) delete gmpRel;
  gmpRel=new gmp_float(0.0);
  mpf_set_prec(*gmpRel->_mpfp(),32);
  mpf_set_d(*gmpRel->_mpfp(),0.1);
  mpf_pow_ui(*gmpRel->_mpfp(),*gmpRel->_mpfp(),digits);
}

#if 1
void gmp_float::setFromStr(const char * in )
{
  BOOLEAN neg=false;
  if (*in == '-') { in++; neg=TRUE; }
  char *s;
  if ((s=strchr((char *)in,'E')) !=NULL)
  {
    *s='e';
  }

  // gmp doesn't understand number which begin with "." -- it needs 0.
  // so, insert the zero
  if (*in == '.')
  {
    int len = strlen(in)+2;
    char* c_in = (char*) omAlloc(len);
    *c_in = '0';
    strcpy(&(c_in[1]), in);

    if(mpf_set_str( t, c_in, 10 )!=0) WerrorS("syntax error in GMP float");
    omFreeSize((void*)c_in, len);
  }
  else
  {
    if(mpf_set_str( t, in, 10 )!=0) WerrorS("syntax error in GMP float");
  }
  if (neg)  mpf_neg( t, t );
}
#else
// problemns with solve_s.tst
void gmp_float::setFromStr(const char * in )
{
  BOOLEAN neg=false;
  BOOLEAN E_found=FALSE;
  if (*in == '-') { in++; neg=TRUE; }
  char *s;
  if ((s=strchr(in,'E')) !=NULL)
  {
    *s='e';
    E_found=TRUE;
  }
  // gmp doesn't understand number like 1e1, it need 1e+1
  // so, insert the +
  if (E_found ||((s=strchr(in,'e')) !=NULL))
  {
    if ((*(s+1)!='+') && (*(s+1)!='-'))
    {
      int len = strlen(in)+3;
      char* c_in = (char*) omAlloc(len);
      if (*in == '.')
      {
        *c_in = '0';
        strcpy(&(c_in[1]), in);
      }
      else
      {
        strcpy(c_in, in);
      }
      char * ss=strchr(c_in,'e');
      memmove(ss+2,s+1,strlen(s+1));
      *(ss+1)+'+';

      mpf_set_str( t, c_in, 10 );
      omFreeSize((void*)c_in, len);
    }
  }

  // gmp doesn't understand number which begin with "." -- it needs 0.
  // so, insert the zero
  else if (*in == '.')
  {
    int len = strlen(in)+2;
    char* c_in = (char*) omAlloc(len);
    *c_in = '0';
    strcpy(&(c_in[1]), in);

    mpf_set_str( t, c_in, 10 );
    omFreeSize((void*)c_in, len);
  }
  else
  {
    mpf_set_str( t, in, 10 );
  }
  if (neg)  mpf_neg( t, t );
}
#endif


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
  mpf_set(diff->t, t);
  mpf_set_prec(diff->t, 32);
  mpf_div(diff->t, diff->t, a.t);
  mpf_abs(diff->t, diff->t);
  if(mpf_cmp(diff->t, gmpRel->t) < 0)
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
  mpf_set(diff->t, t);
  mpf_set_prec(diff->t, 32);
  mpf_div(diff->t, diff->t, a.t);
  mpf_abs(diff->t, diff->t);
  if(mpf_cmp(diff->t, gmpRel->t) < 0)
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
  mpf_sub(diff->t, a.t, b.t);
  mpf_div(diff->t, diff->t, a.t);
  mpf_abs(diff->t, diff->t);
  if(mpf_cmp(diff->t, gmpRel->t) < 0)
    return true;
  else
    return false;
}
// t == 0 ?
bool gmp_float::isZero() const
{
  return (mpf_sgn( t ) == 0);
}
// t == 1 ?
bool gmp_float::isOne() const
{
#ifdef  VARIANTE_1
  return (mpf_cmp_ui( t , 1 ) == 0);
#else
  if (mpf_sgn(t) <= 0)
    return false;
  mpf_sub_ui(diff->t, t, 1);
  mpf_abs(diff->t, diff->t);
  if(mpf_cmp(diff->t, gmpRel->t) < 0)
    return true;
  else
    return false;
#endif
}
// t == -1 ?
bool gmp_float::isMOne() const
{
#ifdef VARIANTE_1
  return (mpf_cmp_si( t , -1 ) == 0);
#else
  if (mpf_sgn(t) >= 0)
    return false;
  mpf_add_ui(diff->t, t, 1);
  mpf_abs(diff->t, diff->t);
  if(mpf_cmp(diff->t, gmpRel->t) < 0)
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
  gmp_float tmp;
  mpf_abs( *(tmp._mpfp()), *a.mpfp() );
  return tmp;
}
gmp_float sqrt( const gmp_float & a )
{
  gmp_float tmp;
  mpf_sqrt( *(tmp._mpfp()), *a.mpfp() );
  return tmp;
}
gmp_float sin( const gmp_float & a )
{
  gmp_float tmp( sin((double)a) );
  return tmp;
}
gmp_float cos( const gmp_float & a )
{
  gmp_float tmp( cos((double)a) );
  return tmp;
}
gmp_float log( const gmp_float & a )
{
  gmp_float tmp( log((double)a) );
  return tmp;
}
gmp_float hypot( const gmp_float & a, const gmp_float & b )
{
#if 1
  return ( sqrt( (a*a) + (b*b) ) );
#else
  gmp_float tmp( hypot( (double)a, (double)b ) );
  return tmp;
#endif
}
gmp_float exp( const gmp_float & a )
{
  gmp_float tmp( exp((double)a) );
  return tmp;
}
gmp_float max( const gmp_float & a, const gmp_float & b )
{
  gmp_float tmp;
  a > b ? tmp= a : tmp= b;
  return tmp;
}
//
// number to float, number = Q, R, C
// makes a COPY of num! (Ist das gut?)
//
gmp_float numberToFloat( number num, const coeffs src)
{
  gmp_float r;

  if ( nCoeff_is_Q(src) )
  {
    if ( num != NULL )
    {
      if (SR_HDL(num) & SR_INT)
      {
        //n_Print(num, src);printf("\n");
        int nn = SR_TO_INT(num);
        if((long)nn == SR_TO_INT(num))
            r = SR_TO_INT(num);
        else
            r = gmp_float(SR_TO_INT(num));
        //int dd = 20;
        //gmp_printf("\nr = %.*Ff\n",dd,*r.mpfp());
        //getchar();
      }
      else
      {
        if ( num->s == 0 )
        {
          nlNormalize( num, src ); // FIXME? TODO? // extern void     nlNormalize(number &x, const coeffs r); // FIXME
        }
        if (SR_HDL(num) & SR_INT)
        {
          r= SR_TO_INT(num);
        }
        else
        {
          if ( num->s != 3 )
          {
            r= num->z;
            r/= (gmp_float)num->n;
          }
          else
          {
            r= num->z;
          }
        }
      }
    }
    else
    {
      r= 0.0;
    }
  }
  else if (nCoeff_is_long_R(src) || nCoeff_is_long_C(src))
  {
    r= *(gmp_float*)num;
  }
  else if ( nCoeff_is_R(src) )
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

gmp_float numberFieldToFloat( number num, int cf)
{
  gmp_float r;

  switch (cf)
  {
  case QTOF:
    if ( num != NULL )
    {
      if (SR_HDL(num) & SR_INT)
      {
        r = gmp_float(SR_TO_INT(num));
      }
      else
      {
        if ( num->s != 3 )
        {
          r= gmp_float(num->z);
          r/= gmp_float(num->n);
        }
        else
        {
          r= num->z;
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
    return omStrDup("0");
  }

  if ( ((unsigned int)ABS(exponent) <= oprec)
       /*|| (exponent+sign >= (int)strlen(in))*/ )
  {
    if ( exponent+sign < (int)strlen(in) )
    {
      int eexponent= (exponent >= 0) ? 0 : -exponent;
      int eeexponent= (exponent >= 0) ? exponent : 0;
      *size= (strlen(in)+15+eexponent) * sizeof(char);
      out= (char*)omAlloc(*size);
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
      out= (char*)omAlloc(*size);
      memset(out,0,*size);
      sprintf(out,"%s%s",csign,in+sign);
      memset(out+strlen(out),'0',exponent-strlen(in)+sign);
    }
    else
    {
      *size= (strlen(in)+2) * sizeof(char) + 10;
      out= (char*)omAlloc(*size);
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
      out= (char*)omAlloc(*size);
      memset(out,0,*size);
      sprintf(out,"%s0.%se%s%d",csign,in+sign,exponent>=0?"+":"",(int)exponent);
//      }
//      else
//      {
//        *size=2;
//        out= (char*)omAlloc(*size);
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
  in= (char*)omAlloc( insize );

  mpf_get_str(in,&exponent,10,oprec,*(r.mpfp()));

  //if ( (exponent > 0)
  //&& (exponent < (int)oprec)
  //&& (strlen(in)-(in[0]=='-'?1:0) == oprec) )
  //{
  //  omFree( (void *) in );
  //  insize= (exponent+oprec+2) * sizeof(char) + 10;
  //  in= (char*)omAlloc( insize );
  //  int newprec= exponent+oprec;
  //  mpf_get_str(in,&exponent,10,newprec,*(r.mpfp()));
  //}
  nout= nicifyFloatStr( in, exponent, oprec, &size, SIGN_EMPTY );
  omFree( (void *) in );
  out= (char*)omAlloc( (strlen(nout)+1) * sizeof(char) );
  strcpy( out, nout );
  omFree( (void *) nout );

  return out;
#else
  // for testing purpose...
  char *out= (char*)omAlloc( (1024) * sizeof(char) );
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
gmp_complex & gmp_complex::neg ( )
{
  i.neg();
  r.neg();
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
  gmp_complex tmp(nr, ni);
  return tmp;
}

// converts a gmp_complex to a string ( <real part> + I * <imaginary part> )
//
char *complexToStr( gmp_complex & c, const unsigned int oprec, const coeffs src )
{
  const char * complex_parameter = "I";
  int N = 1; // strlen(complex_parameter);

  if (nCoeff_is_long_C(src))
  {
    complex_parameter = n_ParameterNames(src)[0];
    N = strlen(complex_parameter);
  }

  assume( complex_parameter != NULL && N > 0);

  char *out,*in_imag,*in_real;

  c.SmallToZero();
  if ( !c.imag().isZero() )
  {

    in_real=floatToStr( c.real(), oprec );         // get real part
    in_imag=floatToStr( abs(c.imag()), oprec );    // get imaginary part

    if (nCoeff_is_long_C(src))
    {
      int len=(strlen(in_real)+strlen(in_imag)+7+N)*sizeof(char);
      out=(char*)omAlloc(len);
      memset(out,0,len);
      if (  !c.real().isZero() )  // (-23-i*5.43) or (15.1+i*5.3)
        sprintf(out,"(%s%s%s*%s)",in_real,c.imag().sign()>=0?"+":"-",complex_parameter,in_imag);
      else // (-i*43) or (i*34)
      {
        if (c.imag().isOne())
          sprintf(out,"%s", complex_parameter);
        else if (c.imag().isMOne())
          sprintf(out,"-%s", complex_parameter);
        else
          sprintf(out,"(%s%s*%s)",c.imag().sign()>=0?"":"-", complex_parameter,in_imag);
      }
    }
    else
    {
      int len=(strlen(in_real)+strlen(in_imag)+9) * sizeof(char);
      out=(char*)omAlloc( len );
      memset(out,0,len);
      if ( !c.real().isZero() )
        sprintf(out,"(%s%s%s)",in_real,c.imag().sign()>=0?"+I*":"-I*",in_imag);
      else
        sprintf(out,"(%s%s)",c.imag().sign()>=0?"I*":"-I*",in_imag);
    }
    omFree( (void *) in_real );
    omFree( (void *) in_imag );
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
  mpf_abs(*ar._mpfp(), *ar._mpfp());
  mpf_abs(*ai._mpfp(), *ai._mpfp());
  mpf_set_prec(*ar._mpfp(), 32);
  mpf_set_prec(*ai._mpfp(), 32);
  if (ar > ai)
  {
    mpf_div(*ai._mpfp(), *ai._mpfp(), *ar._mpfp());
    if (ai < *gmpRel) this->imag(0.0);
  }
  else
  {
    mpf_div(*ar._mpfp(), *ar._mpfp(), *ai._mpfp());
    if (ar < *gmpRel) this->real(0.0);
  }
}

//%e

// local Variables: ***
// folded-file: t ***
// compile-command-1: "make installg" ***
// compile-command-2: "make install" ***
// End: ***
