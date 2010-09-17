/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/* $Id$ */
/*
* ABSTRACT: computations with GMP complex floating-point numbers
*
* ngc == number gnu complex
*/

#include <kernel/mod2.h>
#include <kernel/structs.h>
#include <kernel/febase.h>
#include <omalloc/omalloc.h>
#include <kernel/numbers.h>
#include <kernel/longrat.h>
#include <kernel/modulop.h>
#include <kernel/gnumpc.h>
#include <kernel/gnumpfl.h>
#include <kernel/mpr_complex.h>

extern size_t gmp_output_digits;


number ngcMapQ(number from)
{
  if ( from != NULL )
  {
    gmp_complex *res=new gmp_complex(numberFieldToFloat(from,QTOF));
    return (number)res;
  }
  else
    return NULL;
}
union nf
{
  float _f;
  number _n;
  nf(float f) {_f = f;}
  nf(number n) {_n = n;}
  float F() const {return _f;}
  number N() const {return _n;}
};
static number ngcMapLongR(number from)
{
  if ( from != NULL )
  {
    gmp_complex *res=new gmp_complex(*((gmp_float *)from));
    return (number)res;
  }
  else
    return NULL;
}
static number ngcMapR(number from)
{
  if ( from != NULL )
  {
    gmp_complex *res=new gmp_complex((double)nf(from).F());
    return (number)res;
  }
  else
    return NULL;
}
extern ring ngfMapRing;
static number ngcMapP(number from)
{
  if ( from != NULL)
    return ngcInit(npInt(from,ngfMapRing), currRing);
  else
    return NULL;
}

nMapFunc ngcSetMap(const ring src,const ring dst)
{
  if(rField_is_Q(src))
  {
    return ngcMapQ;
  }
  if (rField_is_long_R(src))
  {
    return ngcMapLongR;
  }
  if (rField_is_long_C(src))
  {
    return ngcCopy;
  }
  if(rField_is_R(src))
  {
    return ngcMapR;
  }
  if (rField_is_Zp(src))
  {
    ngfMapRing=src;
    return ngcMapP;
  }
  return NULL;
}

number   ngcPar(int i)
{
  gmp_complex* n= new gmp_complex( (long)0, (long)1 );
  return (number)n;
}

/*2
* n := i
*/
number ngcInit (int i, const ring r)
{
  gmp_complex* n= new gmp_complex( (long)i, (long)0 );
  return (number)n;
}

/*2
* convert number to int
*/
int ngcInt(number &i, const ring r)
{
  return (int)((gmp_complex*)i)->real();
}

int ngcSize(number n)
{
  int r = (int)((gmp_complex*)n)->real();
  if (r < 0) r = -r;
  int i = (int)((gmp_complex*)n)->imag();
  if (i < 0) i = -i;
  int oneNorm = r + i;
  /* basically return the 1-norm of n;
     only if this happens to be zero although n != 0,
     return 1;
     (this code ensures that zero has the size zero) */
  if ((oneNorm == 0.0) & (ngcIsZero(n) == FALSE)) oneNorm = 1;
  return oneNorm;
}

/*2
* delete a
*/
void ngcDelete (number * a, const ring r)
{
  if ( *a != NULL )
  {
    delete *(gmp_complex**)a;
    *a=NULL;
  }
}

/*2
* copy a to b
*/
number ngcCopy(number a)
{
  gmp_complex* b= new gmp_complex( *(gmp_complex*)a );
  return (number)b;
}
number ngc_Copy(number a, ring r)
{
  gmp_complex* b=new gmp_complex( *(gmp_complex*)a );
  return (number)b;
}

/*2
* za:= - za
*/
gmp_complex ngc_m1(-1);

number ngcNeg (number a)
{
  gmp_complex* r=(gmp_complex*)a;
  (*r) *= ngc_m1;
  return (number)a;
}

/*
* 1/a
*/
number ngcInvers(number a)
{
  gmp_complex* r = NULL;
  if (((gmp_complex*)a)->isZero())
  {
    WerrorS(nDivBy0);
  }
  else
  {
    r= new gmp_complex( (gmp_complex)1 / (*(gmp_complex*)a) );
  }
  return (number)r;
}

/*2
* u:= a + b
*/
number ngcAdd (number a, number b)
{
  gmp_complex* r= new gmp_complex( (*(gmp_complex*)a) + (*(gmp_complex*)b) );
  return (number)r;
}

/*2
* u:= a - b
*/
number ngcSub (number a, number b)
{
  gmp_complex* r= new gmp_complex( (*(gmp_complex*)a) - (*(gmp_complex*)b) );
  return (number)r;
}

/*2
* u := a * b
*/
number ngcMult (number a, number b)
{
  gmp_complex* r= new gmp_complex( (*(gmp_complex*)a) * (*(gmp_complex*)b) );
  return (number)r;
}

/*2
* u := a / b
*/
number ngcDiv (number a, number b)
{
  if (((gmp_complex*)b)->isZero())
  {
    // a/0 = error
    WerrorS(nDivBy0);
    return NULL;
  }
  gmp_complex* r= new gmp_complex( (*(gmp_complex*)a) / (*(gmp_complex*)b) );
  return (number)r;
}

/*2
* u:= x ^ exp
*/
void ngcPower ( number x, int exp, number * u )
{
  if ( exp == 0 )
  {
    gmp_complex* n = new gmp_complex(1);
    *u=(number)n;
    return;
  }
  else if ( exp == 1 )
  {
    nNew(u);
    gmp_complex* n = new gmp_complex();
    *n= *(gmp_complex*)x;
    *u=(number)n;
    return;
  }
  else if (exp == 2)
  {
    nNew(u);
    gmp_complex* n = new gmp_complex();
    *n= *(gmp_complex*)x;
    *u=(number)n;
    *(gmp_complex*)(*u) *= *(gmp_complex*)n;
    return;
  }
  if (exp&1==1)
  {
    ngcPower(x,exp-1,u);
    gmp_complex *n=new gmp_complex();
    *n=*(gmp_complex*)x;
    *(gmp_complex*)(*u) *= *(gmp_complex*)n;
    delete n;
  }
  else
  {
    number w;
    nNew(&w);
    ngcPower(x,exp/2,&w);
    ngcPower(w,2,u);
    nDelete(&w);
  }
}

BOOLEAN ngcIsZero (number a)
{
  return ( ((gmp_complex*)a)->real().isZero() && ((gmp_complex*)a)->imag().isZero());
}

number ngcRePart(number a)
{
  gmp_complex* n = new gmp_complex(((gmp_complex*)a)->real());
  return (number)n;
}

number ngcImPart(number a)
{
  gmp_complex* n = new gmp_complex(((gmp_complex*)a)->imag());
  return (number)n;
}

/*2
* za >= 0 ?
*/
BOOLEAN ngcGreaterZero (number a)
{
  if ( ! ((gmp_complex*)a)->imag().isZero() )
    return ( abs( *(gmp_complex*)a).sign() >= 0 );
  else
    return ( ((gmp_complex*)a)->real().sign() >= 0 );
}

/*2
* a > b ?
*/
BOOLEAN ngcGreater (number a, number b)
{
  gmp_complex *aa=(gmp_complex*)a;
  gmp_complex *bb=(gmp_complex*)b;
  return (*aa) > (*bb);
}

/*2
* a = b ?
*/
BOOLEAN ngcEqual (number a, number b)
{
  gmp_complex *aa=(gmp_complex*)a;
  gmp_complex *bb=(gmp_complex*)b;
  return (*aa) == (*bb);
}

/*2
* a == 1 ?
*/
BOOLEAN ngcIsOne (number a)
{
  return (((gmp_complex*)a)->real().isOne() && ((gmp_complex*)a)->imag().isZero());
  //return (((gmp_complex*)a)->real().isOne());
}

/*2
* a == -1 ?
*/
BOOLEAN ngcIsMOne (number a)
{
  return (((gmp_complex*)a)->real().isMOne() && ((gmp_complex*)a)->imag().isZero());
  //return (((gmp_complex*)a)->real().isMOne());
}

/*2
* extracts the number a from s, returns the rest
*/
const char * ngcRead (const char * s, number * a)
{
  const char *start= s;
  if ((*s >= '0') && (*s <= '9'))
  {
    gmp_float *re=NULL;
    s=ngfRead(s,(number *)&re);
    gmp_complex *aa=new gmp_complex(*re);
    *a=(number)aa;
    delete re;
  }
  else if (strncmp(s,currRing->parameter[0],strlen(currRing->parameter[0]))==0)
  {
    s+=strlen(currRing->parameter[0]);
    gmp_complex *aa=new gmp_complex((long)0,(long)1);
    *a=(number)aa;
  }
  else
  {
    *a=(number) new gmp_complex((long)1);
  }
  return s;
}

/*2
* write a floating point number
*/
void ngcWrite (number &a, const ring r)
{
  if (a==NULL)
    StringAppendS("0");
  else
  {
    char *out;
    out= complexToStr(*(gmp_complex*)a,gmp_output_digits);
    StringAppendS(out);
    //    omFreeSize((ADDRESS)out, (strlen(out)+1)* sizeof(char) );
    omFree( (ADDRESS)out );
  }
}

#ifdef LDEBUG
// not yet implemented
//BOOLEAN ngcDBTest(number a, const char *f, const int l)
//{
//  return TRUE;
//}
#endif

// local Variables: ***
// folded-file: t ***
// compile-command: "make installg" ***
// End: ***
