/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/* $Id: gnumpc.cc,v 1.1 1999-05-11 15:42:38 Singular Exp $ */
/*
* ABSTRACT: computations with GMP complex floating-point numbers
*
* ngc == number gnu complex
*/

#include "mod2.h"
#include "tok.h"
#include "febase.h"
#include "mmemory.h"
#include "numbers.h"
#include "modulop.h"
#include "longrat.h"

#include "gnumpc.h"
#include "gnumpfl.h"
#include "mpr_complex.h"

extern size_t gmp_output_digits;

//static int ngcPrimeM;
//static number ngcMapP(number from)
//{
//number to;
//int save=npPrimeM;
//npPrimeM=ngcPrimeM;
//to = ngcInit(npInt(from));
//npPrimeM=save;
//return to;
//}
//static number ngcMapQ(number from)
//{
//gmp_float *res= new gmp_complex();
//*res= numberToFloat(from);
//return (number)res;
//}

BOOLEAN ngcSetMap(int c, char ** par, int nop, number minpol)
{
//  if (c == 0)
//  {                      /* Q -> R      */
//    nMap = ngcMapQ;
//    return TRUE;
//  }
//  if (c>1)
//  {
//    if (par==NULL)
//    {                    /* Z/p -> R    */
//      nMap = ngcMapP;
//      return TRUE;
//    }
//    else
//    {                    /* GF(q) -> R  */
//      return FALSE;
//    }
//  }
//  else if (c<0)
//     return FALSE;       /* Z/p(a) -> R */
//  else if (c==1)
//     return FALSE;       /* Q(a) -> R   */
  return FALSE;
}

number   ngcPar(int i)
{
  complex* n= new complex( (long)0, (long)1 );
  return (number)n;
}

void ngcNew (number * r)
{
  *r= NULL;
}

/*2
* n := i
*/
number ngcInit (int i)
{
  complex* n= NULL;
  if ( i != 0 )
  {
    n= new complex( (long)i, (long)0 );
  }
  return (number)n;
}

/*2
* convert number to int
*/
int ngcInt(number &i)
{
  if ( i == NULL ) return 0;
  return (int)((complex*)i)->real();
}

/*2
* delete a
*/
#ifdef LDEBUG
void ngcDBDelete (number * a,char *f, int l)
#else
void ngcDelete (number * a)
#endif
{
  if ( *a != NULL ) {
    delete *(complex**)a;
    *a=NULL;
  }
}

/*2
* copy a to b
*/
number ngcCopy(number a)
{
  complex* b= NULL;
  if ( a !=  NULL )
  {
    b= new complex( *(complex*)a );
  }
  return (number)b;
}

/*2
* za:= - za
*/
number ngcNeg (number a)
{
  if ( a == NULL ) return NULL;
  number m1=nInit(-1);
  a=ngcMult(m1,a);
  return (number)a;
}

/*
* 1/a
*/
number ngcInvers(number a)
{
  complex* r= NULL;
  if ( (a==NULL) /*|| ((complex*)a)->isZero()*/ )
  {
    WerrorS("div. 1/0");
  }
  else
  {
    r= new complex( (complex)1 / (*(complex*)a) );
  }
  return (number)r;
}

/*2
* u:= a + b
*/
number ngcAdd (number a, number b)
{
  complex* r= NULL;
  if ( a==NULL && b==NULL )
  {
    return NULL;
  }
  else if ( a == NULL )
  {
    r= new complex( *(complex*)b );
  }
  else if ( b == NULL )
  {
    r= new complex( *(complex*)a );
  }
  else
  {
    r= new complex( (*(complex*)a) + (*(complex*)b) );
  }
  return (number)r;
}

/*2
* u:= a - b
*/
number ngcSub (number a, number b)
{
  complex* r= NULL;
  if ( a==NULL && b==NULL )
  {
    return NULL;
  }
  else if ( a == NULL )
  {
    r= new complex( (*(complex*)b) );
    r= (complex *)ngcNeg((number)r);
  }
  else if ( b == NULL )
  {
    r= new complex( *(complex*)a );
  }
  else
  {
    r= new complex( (*(complex*)a) - (*(complex*)b) );
  }
  return (number)r;
}

/*2
* u := a * b
*/
number ngcMult (number a, number b)
{
  complex* r= NULL;
  if ( a==NULL || b==NULL )
  {
    return NULL;
  }
  else
  {
    r= new complex( (*(complex*)a) * (*(complex*)b) );
  }
  return (number)r;
}

/*2
* u := a / b
*/
number ngcDiv (number a, number b)
{
  if ( b==NULL /*|| ((gmp_float*)b)->isZero()*/ )
  {
    // a/0 = error
    WerrorS("div. 1/0");
    return NULL;
  }
  else if ( a==NULL )
  {
    // 0/b = 0
    return NULL;
  }
  complex* r= new complex( (*(complex*)a) / (*(complex*)b) );
  return (number)r;
}

/*2
* u:= x ^ exp
*/
void ngcPower ( number x, int exp, number * u )
{
  if ( exp == 0 )
  {
    *(complex*)u= 1.0;
    return;
  }
  if ( exp == 1 )
  {
    if ( x == NULL )
    {
      *(complex*)u= 0.0;
    }
    else
    {
      *(complex*)u= *(complex*)x;
    }
    return;
  }
  ngcPower(x,exp-1,u);
  *(complex*)u*= *(complex*)x;
}

BOOLEAN ngcIsZero (number a)
{
  if ( a == NULL ) return TRUE;
  return ( ((complex*)a)->real().isZero() && ((complex*)a)->imag().isZero());
}

/*2
* za >= 0 ?
*/
BOOLEAN ngcGreaterZero (number a)
{
  return TRUE;
}

/*2
* a > b ?
*/
BOOLEAN ngcGreater (number a, number b)
{
  if ( a==NULL )
  {
    return (((complex*)b)->real().sign() < 0);
  }
  if ( b==NULL )
  {
    return (((complex*)a)->real().sign() < 0);
  }
  return FALSE;
}

/*2
* a = b ?
*/
BOOLEAN ngcEqual (number a, number b)
{
  if ( a == NULL && b == NULL )
  {
    return TRUE;
  }
  else if ( a == NULL || b == NULL )
  {
    return FALSE;
  }
  return ( (*(complex*)a) == (*(complex*)b) );
}

/*2
* a == 1 ?
*/
BOOLEAN ngcIsOne (number a)
{
  if ( a == NULL ) return FALSE;
  return (((complex*)a)->real().isOne() && ((complex*)a)->imag().isZero());
}

/*2
* a == -1 ?
*/
BOOLEAN ngcIsMOne (number a)
{
  if ( a == NULL ) return FALSE;
  return (((complex*)a)->real().isMOne() && ((complex*)a)->imag().isZero());
}

/*2
* extracts the number a from s, returns the rest
*/
char * ngcRead (char * s, number * a)
{
  char *start= s;
  if ((*s >= '0') && (*s <= '9'))
  {
    gmp_float *re=NULL;
    s=ngfRead(s,(number *)&re);
    complex *aa=new complex(*re);
    *a=(number)aa;
    delete re;
  }
  else if (strncmp(s,currRing->parameter[0],strlen(currRing->parameter[0]))==0)
  {
    s+=strlen(currRing->parameter[0]);
    complex *aa=new complex((long)0,(long)1);
    *a=(number)aa;
  }
  return s;
}

/*2
* write a floating point number
*/
void ngcWrite (number &a)
{
  if (a==NULL)
    StringAppend("0");
  else
  {
    char *out;
    out= complexToStr(*(complex*)a,gmp_output_digits);
    StringAppend(out);
    Free((ADDRESS)out, (strlen(out)+1)* sizeof(char) );
  }
}

#ifdef LDEBUG
BOOLEAN ngcDBTest(number a, char *f, int l)
{
  return TRUE;
}
#endif

// local Variables: ***
// folded-file: t ***
// compile-command: "make installg" ***
// End: ***
