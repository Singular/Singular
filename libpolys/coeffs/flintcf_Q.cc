/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/*
* ABSTRACT: flint: fmpq_poly
*/
#include <ctype.h> /* isdigit*/

#ifdef HAVE_CONFIG_H
#include "libpolysconfig.h"
#endif /* HAVE_CONFIG_H */
#include <misc/auxiliary.h>

#ifdef SINGULAR_4_1
#ifdef HAVE_FLINT

#include <flint/flint.h>
#include <flint/fmpq_poly.h>
#include <factory/factory.h>

#include <omalloc/omalloc.h>
#include <coeffs/coeffs.h>

#include <coeffs/numbers.h>
#include <coeffs/longrat.h>

typedef fmpq_poly_struct *fmpq_poly_ptr;
typedef fmpz *fmpz_ptr;
/*2
* extracts a long integer from s, returns the rest
*/
static char * nlEatLong(char *s, mpz_ptr i)
{
  const char * start=s;

  while (*s >= '0' && *s <= '9') s++;
  if (*s=='\0')
  {
    mpz_set_str(i,start,10);
  }
  else
  {
    char c=*s;
    *s='\0';
    mpz_set_str(i,start,10);
    *s=c;
  }
  return s;
}

static const char* Eati(const char *s, int *i)
{

  if (((*s) >= '0') && ((*s) <= '9'))
  {
    unsigned long ii=0L;
    do
    {
      ii *= 10;
      ii += *s++ - '0';
    }
    while (((*s) >= '0') && ((*s) <= '9'));
    *i=(int)ii;
  }
  else (*i) = 1;
  return s;
}



static void CoeffWrite(const coeffs r, BOOLEAN details)
{
  Print("// flint fmpq_poly\n");
}
BOOLEAN CoeffIsEqual(const coeffs r, n_coeffType n, void * parameter)
{
  return (r->type==n);
}
static void KillChar(coeffs r)
{
  // not yet
}
static void SetChar(const coeffs r)
{
  // dummy
}
static number Mult(number a, number b, const coeffs c)
{
  fmpq_poly_ptr res=(fmpq_poly_ptr)omAlloc(sizeof(fmpq_poly_t));
  fmpq_poly_init(res);
  fmpq_poly_mul(res,(fmpq_poly_ptr)a,(fmpq_poly_ptr)b);
  return (number)res;
}
static number Sub(number a, number b, const coeffs c)
{
  fmpq_poly_ptr res=(fmpq_poly_ptr)omAlloc(sizeof(fmpq_poly_t));
  fmpq_poly_init(res);
  fmpq_poly_sub(res,(fmpq_poly_ptr)a,(fmpq_poly_ptr)b);
  return (number)res;
}
static number Add(number a, number b, const coeffs c)
{
  fmpq_poly_ptr res=(fmpq_poly_ptr)omAlloc(sizeof(fmpq_poly_t));
  fmpq_poly_init(res);
  fmpq_poly_add(res,(fmpq_poly_ptr)a,(fmpq_poly_ptr)b);
  return (number)res;
}
static number Div(number a, number b, const coeffs c)
{
  fmpq_poly_ptr res=(fmpq_poly_ptr)omAlloc(sizeof(fmpq_poly_t));
  fmpq_poly_init(res);
  if(fmpq_poly_is_zero((fmpq_poly_ptr)b))
  {
     WerrorS(nDivBy0);
  }
  else
  {
    fmpq_poly_div(res,(fmpq_poly_ptr)a,(fmpq_poly_ptr)b);
    fmpq_poly_t mod;
    fmpq_poly_init(mod);
    fmpq_poly_rem(mod,(fmpq_poly_ptr)a,(fmpq_poly_ptr)b);
    if (!fmpq_poly_is_zero((fmpq_poly_ptr)mod))
    {
      WerrorS("cannot divide");
    }
    fmpq_poly_clear(mod);
  }
  return (number)res;
}
static number ExactDiv(number a, number b, const coeffs c)
{
  fmpq_poly_ptr res=(fmpq_poly_ptr)omAlloc(sizeof(fmpq_poly_t));
  fmpq_poly_init(res);
  if(fmpq_poly_is_zero((fmpq_poly_ptr)b))
  {
     WerrorS(nDivBy0);
  }
  else
    fmpq_poly_div(res,(fmpq_poly_ptr)a,(fmpq_poly_ptr)b);
  return (number)res;
}
static number IntMod(number a, number b, const coeffs c)
{
  fmpq_poly_ptr res=(fmpq_poly_ptr)omAlloc(sizeof(fmpq_poly_t));
  fmpq_poly_init(res);
  fmpq_poly_rem(res,(fmpq_poly_ptr)a,(fmpq_poly_ptr)b);
  return (number)res;
}
static number Init (long i, const coeffs r)
{
  fmpq_poly_ptr res=(fmpq_poly_ptr)omAlloc(sizeof(fmpq_poly_t));
  fmpq_poly_init(res);
  fmpq_poly_set_si(res,i);
  return (number)res;
}
static number InitMPZ (mpz_t i, const coeffs r)
{
  fmpq_poly_ptr res=(fmpq_poly_ptr)omAlloc(sizeof(fmpq_poly_t));
  fmpq_poly_init(res);
  fmpq_poly_set_mpz(res,i);
  return (number)res;
}
static int Size (number n, const coeffs r)
{
  return fmpq_poly_degree((fmpq_poly_ptr)n);
}
static long Int (number &n, const coeffs r)
{
  if (fmpq_poly_degree((fmpq_poly_ptr)n)==0)
  {
    mpq_t m;
    mpq_init(m);
    fmpq_poly_get_coeff_mpq(m,(fmpq_poly_ptr)n,0);
    mpz_t num,den;
    mpz_init(num);
    mpz_init(den);
    mpq_get_num(num,m);
    mpq_get_den(den,m);
    long nl=mpz_get_si(num);
    if (mpz_cmp_si(num,nl)!=0) nl=0;
    long dl=mpz_get_si(den);
    if ((dl!=1)||(mpz_cmp_si(den,dl)!=0)) nl=0;
    mpz_clear(num);
    mpz_clear(den);
    mpq_clear(m);
    return nl;
  }
  return 0;
}
static void MPZ(mpz_t result, number &n, const coeffs r)
{
  mpz_init(result);
  if (fmpq_poly_degree((fmpq_poly_ptr)n)==0)
  {
    mpq_t m;
    mpq_init(m);
    fmpq_poly_get_coeff_mpq(m,(fmpq_poly_ptr)n,0);
    mpz_t den;
    mpz_init(den);
    mpq_get_num(result,m);
    mpq_get_den(den,m);
    int dl=(int)mpz_get_si(den);
    if ((dl!=1)||(mpz_cmp_si(den,(long)dl)!=0)) mpz_set_ui(result,0);
    mpz_clear(den);
    mpq_clear(m);
  }
}
static number Neg(number a, const coeffs r)
{
  fmpq_poly_neg((fmpq_poly_ptr)a,(fmpq_poly_ptr)a);
  return a;
}
static number Invers(number a, const coeffs r)
{
  if(fmpq_poly_is_zero((fmpq_poly_ptr)a))
  {
    WerrorS(nDivBy0);
    return NULL;
  }
  if (fmpq_poly_degree((fmpq_poly_ptr)a)==0)
  {
    fmpq_poly_ptr res=(fmpq_poly_ptr)omAlloc(sizeof(fmpq_poly_t));
    fmpq_poly_init(res);
    fmpq_poly_inv(res,(fmpq_poly_ptr)a);
    return (number)res;
  }
  else
  {
    WerrorS("not invertable");
    return NULL;
  }
}
static number Copy(number a, const coeffs r)
{
 fmpq_poly_ptr res=(fmpq_poly_ptr)omAlloc(sizeof(fmpq_poly_t));
 fmpq_poly_init(res);
 fmpq_poly_set(res,(fmpq_poly_ptr)a);
 return (number)res;
}
//static number RePart(number a, const coeffs r)
//{
//}
//static number ImPart(number a, const coeffs r)
//{
//}
static BOOLEAN IsOne (number a, const coeffs r);
static BOOLEAN IsZero (number a, const coeffs r);
//static void WriteLong(number &a, const coeffs r)
//{
//}
static void WriteShort(number a, const coeffs r)
{
  //fmpq_poly_print_pretty((fmpq_poly_ptr)a,r->pParameterNames[0]);
  if (IsOne(a,r)) StringAppendS("1");
  else if (IsZero(a,r)) StringAppendS("0");
  else
  {
  StringAppendS("(");
  mpq_t m;
  mpq_init(m);
  mpz_t num,den;
  mpz_init(num);
  mpz_init(den);
  BOOLEAN need_plus=FALSE;
  for(int i=fmpq_poly_length((fmpq_poly_ptr)a);i>=0;i--)
  {
    fmpq_poly_get_coeff_mpq(m,(fmpq_poly_ptr)a,i);
    mpq_get_num(num,m);
    mpq_get_den(den,m);
    if (mpz_cmp_ui(num,0)!=0)
    {
      if (need_plus && (mpz_cmp_si(num,0)>0))
        StringAppendS("+");
      need_plus=TRUE;
      int l=mpz_sizeinbase(num,10);
      l=si_max(l,(int)mpz_sizeinbase(den,10));
      l+=2;
      char *s=(char*)omAlloc(l);
      char *z=mpz_get_str(s,10,num);
      if ((i==0)
      ||(mpz_cmp_si(num,1)!=0)
      ||(mpz_cmp_si(den,1)!=0))
      {
        StringAppendS(z);
        if (mpz_cmp_si(den,1)!=0)
        {
          StringAppendS("/");
          z=mpz_get_str(s,10,den);
          StringAppendS(z);
        }
        if (i!=0) StringAppendS("*");
      }
      if (i>1)
        StringAppend("%s^%d",r->pParameterNames[0],i);
      else if (i==1)
        StringAppend("%s",r->pParameterNames[0]);
    }
  }
  mpz_clear(den);
  mpz_clear(num);
  mpq_clear(m);
  StringAppendS(")");
  }
}
static const char* Read(const char * st, number * a, const coeffs r)
{
// we only read "monomials" (i.e. [-][digits][parameter]),
// everythings else (+,*,^,()) is left to the singular interpreter
  const char *s=st;
  *a=(number)omAlloc(sizeof(fmpq_poly_t));
  fmpq_poly_init((fmpq_poly_ptr)(*a));
  BOOLEAN neg=FALSE;
  if (*s=='-') { neg=TRUE; s++;}
  if (isdigit(*s))
  {
    mpz_t z;
    mpz_init(z);
    s=nlEatLong((char *)s, z);
    fmpq_poly_set_mpz((fmpq_poly_ptr)(*a),z);
    if (*s == '/')
    {
      s++;
      s=nlEatLong((char *)s, z);
      fmpq_poly_scalar_div_mpz((fmpq_poly_ptr)(*a),(fmpq_poly_ptr)(*a),z);
    }
    mpz_clear(z);
  }
  else if(strncmp(s,r->pParameterNames[0],strlen(r->pParameterNames[0]))==0)
  {
    fmpq_poly_set_coeff_si((fmpq_poly_ptr)(*a),1,1);
    s+=strlen(r->pParameterNames[0]);
    if(isdigit(*s))
    {
      int i=1;
      s=Eati(s,&i);
      if (i!=1)
      {
        fmpq_poly_set_coeff_si((fmpq_poly_ptr)(*a),1,0);
        fmpq_poly_set_coeff_si((fmpq_poly_ptr)(*a),i,1);
      }
    }
  }
  if (neg)
    fmpq_poly_neg((fmpq_poly_ptr)(*a),(fmpq_poly_ptr)(*a));
  return s;
}
static void Normalize(number &a, const coeffs r)
{
  fmpq_poly_canonicalise((fmpq_poly_ptr)a);
}
static BOOLEAN Greater (number a, number b, const coeffs r)
{
  return (fmpq_poly_cmp((fmpq_poly_ptr)a,(fmpq_poly_ptr)b)>0);
}
static BOOLEAN Equal (number a, number b, const coeffs r)
{
  return (fmpq_poly_equal((fmpq_poly_ptr)a,(fmpq_poly_ptr)b));
}
static BOOLEAN IsZero (number a, const coeffs r)
{
  return fmpq_poly_is_zero((fmpq_poly_ptr)a);
}
static BOOLEAN IsOne (number a, const coeffs r)
{
  return fmpq_poly_is_one((fmpq_poly_ptr)a);
}
static BOOLEAN IsMOne (number k, const coeffs r)
{
  if (fmpq_poly_length((fmpq_poly_ptr)k)>0) return FALSE;
  fmpq_poly_canonicalise((fmpq_poly_ptr)k);
  mpq_t m;
  mpq_init(m);
  fmpq_poly_get_coeff_mpq(m,(fmpq_poly_ptr)k,0);
  mpz_t num,den;
  mpz_init(num);
  mpq_get_num(num,m);
  BOOLEAN result=TRUE;
  if (mpz_cmp_si(num,(long)-1)!=0) result=FALSE;
  else
  {
    mpz_init(den);
    mpq_get_den(den,m);
    int dl=(int)mpz_get_si(den);
    if ((dl!=1)||(mpz_cmp_si(den,(long)dl)!=0)) result=FALSE;
    mpz_clear(den);
  }
  mpz_clear(num);
  mpq_clear(m);
  return (result);
}
static BOOLEAN GreaterZero (number k, const coeffs r)
{
  // does it have a leading sign?
  // no: 0 and 1 do not have, everything else is in (...)
  return TRUE;
}
static void Power(number a, int i, number * result, const coeffs r)
{
  fmpq_poly_ptr res=(fmpq_poly_ptr)omAlloc(sizeof(fmpq_poly_t));
  fmpq_poly_init(res);
  *result=(number)res;
  fmpq_poly_pow((fmpq_poly_ptr)(*result),(fmpq_poly_ptr)a,i);
}
static number GetDenom(number &n, const coeffs r)
{
 fmpq_poly_ptr res=(fmpq_poly_ptr)omAlloc(sizeof(fmpq_poly_t));
 fmpq_poly_init(res);
 fmpz_ptr den=fmpq_poly_denref(res);
 fmpq_poly_set_fmpz(res,den);
 return (number)res;
}
static number GetNumerator(number &n, const coeffs r)
{
 fmpq_poly_ptr res=(fmpq_poly_ptr)omAlloc(sizeof(fmpq_poly_t));
 fmpq_poly_init(res);
 fmpq_poly_set(res,(fmpq_poly_ptr)n);
 fmpz_ptr den=fmpq_poly_denref(res);
 fmpq_poly_scalar_mul_fmpz(res,res,den);
 return (number)res;
}
static number Gcd(number a, number b, const coeffs r)
{
  fmpq_poly_ptr res=(fmpq_poly_ptr)omAlloc(sizeof(fmpq_poly_t));
  fmpq_poly_init(res);
  fmpq_poly_gcd(res,(fmpq_poly_ptr)a,(fmpq_poly_ptr)b);
  return (number)res;
}
static number ExtGcd(number a, number b, number *s, number *t,const coeffs r)
{
  fmpq_poly_ptr res=(fmpq_poly_ptr)omAlloc(sizeof(fmpq_poly_t));
  fmpq_poly_init(res);
  fmpq_poly_init((fmpq_poly_ptr)*s);
  fmpq_poly_init((fmpq_poly_ptr)*t);
  fmpq_poly_xgcd(res,(fmpq_poly_ptr)*s,(fmpq_poly_ptr)*t,(fmpq_poly_ptr)a,(fmpq_poly_ptr)b);
  return (number)res;
}
static number Lcm(number a, number b, const coeffs r)
{
  WerrorS("not yet: Lcm");
}
static void Delete(number * a, const coeffs r)
{
  if ((*a)!=NULL)
  {
    fmpq_poly_clear((fmpq_poly_ptr)*a);
    omFree(*a);
    *a=NULL;
  }
}
static nMapFunc SetMap(const coeffs src, const coeffs dst)
{
  WerrorS("not yet: SetMap");
  return NULL;
}
//static void InpMult(number &a, number b, const coeffs r)
//{
//}
//static void InpAdd(number &a, number b, const coeffs r)
//{
//}
static number Init_bigint(number i, const coeffs dummy, const coeffs dst)
{
  fmpq_poly_ptr res=(fmpq_poly_ptr)omAlloc(sizeof(fmpq_poly_t));
  fmpq_poly_init(res);
  if (SR_HDL(i) & SR_INT)
  {
    fmpq_poly_set_si(res,SR_TO_INT(i));
  }
  else
    fmpq_poly_set_mpz(res,i->z);
  return (number)res;
}
static number Farey(number p, number n, const coeffs)
{
  WerrorS("not yet: Farey");
}
static number ChineseRemainder(number *x, number *q,int rl, BOOLEAN sym,CFArray &inv_cache,const coeffs)
{
  WerrorS("not yet: ChineseRemainder");
}
static int ParDeg(number x,const coeffs r)
{
  return fmpq_poly_degree((fmpq_poly_ptr)x);
}
static number Parameter(const int i, const coeffs r)
{
  fmpq_poly_ptr res=(fmpq_poly_ptr)omAlloc(sizeof(fmpq_poly_t));
  fmpq_poly_init(res);
  fmpq_poly_set_coeff_si(res,1,1);
  return (number)res;
}
// cfClearContent
// cfClearDenominators
static number ConvFactoryNSingN( const CanonicalForm n, const coeffs r)
{
}
static CanonicalForm ConvSingNFactoryN( number n, BOOLEAN setChar, const coeffs r )
{
  WerrorS("not yet: ConvSingNFactoryN");
}
char * CoeffName(const coeffs r)
{
  return (char*)"flint:Q[a]";
}
static char* CoeffString(const coeffs r)
{
  return omStrDup(CoeffName(r));
}
#ifdef LDEBUG
static BOOLEAN DBTest(number a, const char *f, const int l, const coeffs r)
{
  return TRUE;
}
#endif
BOOLEAN flintQ_InitChar(coeffs cf, void * infoStruct)
{
  cf->cfCoeffString  = CoeffString;
  cf->cfCoeffName    = CoeffName;
  cf->cfCoeffWrite   = CoeffWrite;
  cf->nCoeffIsEqual  = CoeffIsEqual;
  cf->cfKillChar     = KillChar;
  cf->cfSetChar      = SetChar;
  cf->ch=0; //char 0
  cf->cfMult         = Mult;
  cf->cfSub          = Sub;
  cf->cfAdd          = Add;
  cf->cfDiv          = Div;
  cf->cfExactDiv     = ExactDiv; // ???
  cf->cfInit         =Init;
  cf->cfInitMPZ      =InitMPZ;
  cf->cfSize         = Size;
  cf->cfInt          = Int;
  cf->cfMPZ          = MPZ;
  cf->cfInpNeg       = Neg;
  cf->cfInvers       = Invers;
  cf->cfCopy         = Copy;
  cf->cfRePart       = Copy;
  // default: cf->cfImPart       = ndReturn0;
  cf->cfWriteLong    = WriteShort; //WriteLong;
  cf->cfWriteShort = WriteShort;
  cf->cfRead         = Read;
  cf->cfNormalize    = Normalize;

  //cf->cfDivComp=
  //cf->cfIsUnit=
  //cf->cfGetUnit=
  //cf->cfDivBy=

  cf->cfGreater=Greater;
  cf->cfEqual  =Equal;
  cf->cfIsZero =IsZero;
  cf->cfIsOne  =IsOne;
  cf->cfIsMOne =IsMOne;
  cf->cfGreaterZero=GreaterZero;

  cf->cfPower        = Power;
  cf->cfGetDenom     = GetDenom;
  cf->cfGetNumerator = GetNumerator;
  cf->cfGcd          = Gcd;
  cf->cfExtGcd         = ExtGcd;
  cf->cfLcm          = Lcm;
  cf->cfDelete       = Delete;
  cf->cfSetMap       = SetMap;
  // default: cf->cfInpMult
  // default: cf->cfInpAdd
  cf->cfFarey        =Farey;
  cf->cfChineseRemainder=ChineseRemainder;
  cf->cfParDeg = ParDeg;
  cf->cfParameter = Parameter;
  //  cf->cfClearContent = ClearContent;
  //  cf->cfClearDenominators = ClearDenominators;
  cf->convFactoryNSingN=ConvFactoryNSingN;
  cf->convSingNFactoryN=ConvSingNFactoryN;
#ifdef LDEBUG
  cf->cfDBTest       = DBTest;
#endif

  cf->iNumberOfParameters = 1;
  char **pn=(char**)omAlloc0(sizeof(char*));
  pn[0]=(char*)omStrDup("a");
  cf->pParameterNames = (const char **)pn;
  cf->has_simple_Inverse= FALSE;
  cf->has_simple_Alloc= FALSE;
  cf->is_field=FALSE;

  return FALSE;
}
#endif
#endif
