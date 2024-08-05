/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/*
* ABSTRACT: flint: fmpq_poly
*/
#include <ctype.h> /* isdigit*/

#include "misc/auxiliary.h"

#ifdef HAVE_FLINT

#include <flint/flint.h>
#include <flint/fmpz.h>
#include <flint/fmpq.h>
#include <flint/fmpq_poly.h>
#include "factory/factory.h"

#include "coeffs/coeffs.h"

#include "coeffs/numbers.h"
#include "coeffs/longrat.h"

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

static BOOLEAN CoeffIsEqual(const coeffs r, n_coeffType n, void *)
{
  return (r->type==n);
}
static void SetChar(const coeffs)
{
  // dummy
}
static number Mult(number a, number b, const coeffs)
{
  fmpq_poly_ptr res=(fmpq_poly_ptr)omAlloc(sizeof(fmpq_poly_t));
  fmpq_poly_init(res);
  fmpq_poly_mul(res,(fmpq_poly_ptr)a,(fmpq_poly_ptr)b);
  return (number)res;
}
static number Sub(number a, number b, const coeffs)
{
  fmpq_poly_ptr res=(fmpq_poly_ptr)omAlloc(sizeof(fmpq_poly_t));
  fmpq_poly_init(res);
  fmpq_poly_sub(res,(fmpq_poly_ptr)a,(fmpq_poly_ptr)b);
  return (number)res;
}
static number Add(number a, number b, const coeffs)
{
  fmpq_poly_ptr res=(fmpq_poly_ptr)omAlloc(sizeof(fmpq_poly_t));
  fmpq_poly_init(res);
  fmpq_poly_add(res,(fmpq_poly_ptr)a,(fmpq_poly_ptr)b);
  return (number)res;
}
static number Div(number a, number b, const coeffs)
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
static number ExactDiv(number a, number b, const coeffs)
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
#if 0
static number IntMod(number a, number b, const coeffs)
{
  fmpq_poly_ptr res=(fmpq_poly_ptr)omAlloc(sizeof(fmpq_poly_t));
  fmpq_poly_init(res);
  fmpq_poly_rem(res,(fmpq_poly_ptr)a,(fmpq_poly_ptr)b);
  return (number)res;
}
#endif
static number Init (long i, const coeffs)
{
  fmpq_poly_ptr res=(fmpq_poly_ptr)omAlloc(sizeof(fmpq_poly_t));
  fmpq_poly_init(res);
  fmpq_poly_set_si(res,i);
  return (number)res;
}
static number InitMPZ (mpz_t i, const coeffs)
{
  fmpq_poly_ptr res=(fmpq_poly_ptr)omAlloc(sizeof(fmpq_poly_t));
  fmpq_poly_init(res);
  fmpq_poly_fit_length(res, 1);
  fmpz_set_mpz(res->coeffs, i);
  fmpz_one(res->den);
  _fmpq_poly_set_length(res, 1);
  _fmpq_poly_normalise(res);
  return (number)res;
}
static int Size (number n, const coeffs)
{
  return fmpq_poly_degree((fmpq_poly_ptr)n);
}
static long Int (number &n, const coeffs)
{
  if (fmpq_poly_degree((fmpq_poly_ptr)n)==0)
  {
    fmpq_t m;
    fmpq_init(m);
    fmpq_poly_get_coeff_fmpq(m,(fmpq_poly_ptr)n,0);
    long nl=fmpz_get_si(fmpq_numref(m));
    if (fmpz_cmp_si(fmpq_numref(m),nl)!=0) nl=0;
    long dl=fmpz_get_si(fmpq_denref(m));
    if ((dl!=1)||(fmpz_cmp_si(fmpq_denref(m),dl)!=0)) nl=0;
    fmpq_clear(m);
    return nl;
  }
  return 0;
}
static void MPZ(mpz_t result, number &n, const coeffs)
{
  mpz_init(result);
  if (fmpq_poly_degree((fmpq_poly_ptr)n)==0)
  {
#if __FLINT_RELEASE >= 20503
    fmpq_t m;
    fmpq_init(m);
    fmpq_poly_get_coeff_fmpq(m,(fmpq_poly_ptr)n,0);
    mpz_t den;
    mpz_init(den);
    fmpq_get_mpz_frac(result,den,m);
    int dl=(int)mpz_get_si(den);
    if ((dl!=1)||(mpz_cmp_si(den,(long)dl)!=0)) mpz_set_ui(result,0);
    mpz_clear(den);
    fmpq_clear(m);
#else
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
#endif
  }
}
static number Neg(number a, const coeffs)
{
  fmpq_poly_neg((fmpq_poly_ptr)a,(fmpq_poly_ptr)a);
  return a;
}
static number Invers(number a, const coeffs)
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
static number Copy(number a, const coeffs)
{
 fmpq_poly_ptr res=(fmpq_poly_ptr)omAlloc(sizeof(fmpq_poly_t));
 fmpq_poly_init(res);
 fmpq_poly_set(res,(fmpq_poly_ptr)a);
 return (number)res;
}
//static number RePart(number a, const coeffs)
//{
//}
//static number ImPart(number a, const coeffs)
//{
//}
static BOOLEAN IsOne (number a, const coeffs);
static BOOLEAN IsZero (number a, const coeffs);
//static void WriteLong(number &a, const coeffs)
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
#if __FLINT_RELEASE >= 20503
  fmpq_t m;
  fmpq_init(m);
  BOOLEAN need_plus=FALSE;
  for(int i=fmpq_poly_length((fmpq_poly_ptr)a);i>=0;i--)
  {
    fmpq_poly_get_coeff_fmpq(m,(fmpq_poly_ptr)a,i);
    if (!fmpq_is_zero(m))
    {
      if (need_plus && (fmpq_cmp_ui(m,0)>0))
        StringAppendS("+");
      need_plus=TRUE;
      int l=fmpz_sizeinbase(fmpq_numref(m),10);
      l=si_max(l,(int)fmpz_sizeinbase(fmpq_denref(m),10));
      l+=2;
      char *s=(char*)omAlloc(l);
      char *z=fmpz_get_str(s,10,fmpq_numref(m));
      if ((i==0)
      ||(fmpz_cmp_si(fmpq_numref(m),1)!=0)
      ||(fmpz_cmp_si(fmpq_denref(m),1)!=0))
      {
        StringAppendS(z);
        if (fmpz_cmp_si(fmpq_denref(m),1)!=0)
        {
          StringAppendS("/");
          z=fmpz_get_str(s,10,fmpq_denref(m));
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
  fmpq_clear(m);
#else
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
    if (mpz_sgn1(num)!=0)
    {
      if (need_plus && (mpz_sgn1(num)>0))
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
#endif
  StringAppendS(")");
  }
}
static const char* Read(const char * st, number * a, const coeffs r)
{
// we only read "monomials" (i.e. [-][digits][parameter]),
// everything else (+,*,^,()) is left to the singular interpreter
  char *s=(char *)st;
  *a=(number)omAlloc(sizeof(fmpq_poly_t));
  fmpq_poly_init((fmpq_poly_ptr)(*a));
  BOOLEAN neg=FALSE;
  if (*s=='-') { neg=TRUE; s++;}
  if (isdigit(*s))
  {
    mpz_t z;
    mpz_init(z);
    fmpz_t z1;
    fmpz_init(z1);
    s=nlEatLong((char *)s, z);
    fmpz_set_mpz(z1,z);
    fmpq_poly_set_fmpz((fmpq_poly_ptr)(*a),z1);
    if (*s == '/')
    {
      s++;
      s=nlEatLong((char *)s, z);
      fmpz_set_mpz(z1,z);
      fmpq_poly_scalar_div_fmpz((fmpq_poly_ptr)(*a),(fmpq_poly_ptr)(*a),z1);
    }
    fmpz_clear(z1);
    mpz_clear(z);
  }
  else if(strncmp(s,r->pParameterNames[0],strlen(r->pParameterNames[0]))==0)
  {
    fmpq_poly_set_coeff_si((fmpq_poly_ptr)(*a),1,1);
    s+=strlen(r->pParameterNames[0]);
    if(isdigit(*s))
    {
      int i=1;
      s=nEati(s,&i,0);
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
static void Normalize(number &a, const coeffs)
{
  fmpq_poly_canonicalise((fmpq_poly_ptr)a);
}
static BOOLEAN Greater (number a, number b, const coeffs)
{
  return (fmpq_poly_cmp((fmpq_poly_ptr)a,(fmpq_poly_ptr)b)>0);
}
static BOOLEAN Equal (number a, number b, const coeffs)
{
  return (fmpq_poly_equal((fmpq_poly_ptr)a,(fmpq_poly_ptr)b));
}
static BOOLEAN IsZero (number a, const coeffs)
{
  return fmpq_poly_is_zero((fmpq_poly_ptr)a);
}
static BOOLEAN IsOne (number a, const coeffs)
{
  return fmpq_poly_is_one((fmpq_poly_ptr)a);
}
static BOOLEAN IsMOne (number k, const coeffs)
{
  if (fmpq_poly_length((fmpq_poly_ptr)k)>0) return FALSE;
  fmpq_poly_canonicalise((fmpq_poly_ptr)k);
  fmpq_t m;
  fmpq_init(m);
  fmpq_poly_get_coeff_fmpq(m,(fmpq_poly_ptr)k,0);
  BOOLEAN result=TRUE;
  if (fmpz_cmp_si(fmpq_numref(m),(long)-1)!=0) result=FALSE;
  else
  {
    int dl=(int)fmpz_get_si(fmpq_denref(m));
    if ((dl!=1)||(fmpz_cmp_si(fmpq_denref(m),(long)dl)!=0)) result=FALSE;
  }
  fmpq_clear(m);
  return (result);
}
static BOOLEAN GreaterZero (number, const coeffs)
{
  // does it have a leading sign?
  // no: 0 and 1 do not have, everything else is in (...)
  return TRUE;
}
static void Power(number a, int i, number * result, const coeffs)
{
  fmpq_poly_ptr res=(fmpq_poly_ptr)omAlloc(sizeof(fmpq_poly_t));
  fmpq_poly_init(res);
  *result=(number)res;
  fmpq_poly_pow((fmpq_poly_ptr)(*result),(fmpq_poly_ptr)a,i);
}
static number GetDenom(number &n, const coeffs)
{
 fmpq_poly_ptr res=(fmpq_poly_ptr)omAlloc(sizeof(fmpq_poly_t));
 fmpq_poly_init(res);
 fmpz_ptr den=fmpq_poly_denref((fmpq_poly_ptr)n);
 fmpq_poly_set_fmpz(res,den);
 return (number)res;
}
static number GetNumerator(number &n, const coeffs)
{
 fmpq_poly_ptr res=(fmpq_poly_ptr)omAlloc(sizeof(fmpq_poly_t));
 fmpq_poly_init(res);
 fmpq_poly_set(res,(fmpq_poly_ptr)n);
 fmpz_ptr den=fmpq_poly_denref(res);
 fmpq_poly_scalar_mul_fmpz(res,res,den);
 return (number)res;
}
static number Gcd(number a, number b, const coeffs)
{
  fmpq_poly_ptr res=(fmpq_poly_ptr)omAlloc(sizeof(fmpq_poly_t));
  fmpq_poly_init(res);
  fmpq_poly_gcd(res,(fmpq_poly_ptr)a,(fmpq_poly_ptr)b);
  return (number)res;
}
static number ExtGcd(number a, number b, number *s, number *t,const coeffs)
{
  fmpq_poly_ptr res=(fmpq_poly_ptr)omAlloc(sizeof(fmpq_poly_t));
  fmpq_poly_init(res);
  fmpq_poly_init((fmpq_poly_ptr)*s);
  fmpq_poly_init((fmpq_poly_ptr)*t);
  fmpq_poly_xgcd(res,(fmpq_poly_ptr)*s,(fmpq_poly_ptr)*t,(fmpq_poly_ptr)a,(fmpq_poly_ptr)b);
  return (number)res;
}
static number Lcm(number, number, const coeffs)
{
  WerrorS("not yet: Lcm");
  return NULL;
}
static void Delete(number * a, const coeffs)
{
  if ((*a)!=NULL)
  {
    fmpq_poly_clear((fmpq_poly_ptr)*a);
    omFree(*a);
    *a=NULL;
  }
}
static nMapFunc SetMap(const coeffs, const coeffs)
{
  WerrorS("not yet: SetMap");
  return NULL;
}
//static void InpMult(number &a, number b, const coeffs)
//{
//}
//static void InpAdd(number &a, number b, const coeffs)
//{
//}
#if 0
static number Init_bigint(number i, const coeffs, const coeffs)
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
#endif
static number Farey(number, number, const coeffs)
{
  WerrorS("not yet: Farey");
  return NULL;
}
static number ChineseRemainder(number *, number *,int , BOOLEAN ,CFArray &,const coeffs)
{
  WerrorS("not yet: ChineseRemainder");
  return NULL;
}
static int ParDeg(number x,const coeffs)
{
  return fmpq_poly_degree((fmpq_poly_ptr)x);
}
static number Parameter(const int, const coeffs)
{
  fmpq_poly_ptr res=(fmpq_poly_ptr)omAlloc(sizeof(fmpq_poly_t));
  fmpq_poly_init(res);
  fmpq_poly_set_coeff_si(res,1,1);
  return (number)res;
}
static void WriteFd(number a, const ssiInfo *d, const coeffs)
{
  // format: len a_len(num den) .. a_0
  fmpq_poly_ptr aa=(fmpq_poly_ptr)a;
  int l=fmpq_poly_length(aa);
  fprintf(d->f_write,"%d ",l);
#if __FLINT_RELEASE >= 20503
  fmpq_t m;
  fmpq_init(m);
  mpz_t num,den;
  mpz_init(num);
  mpz_init(den);
  for(int i=l; i>=0; i--)
  {
    fmpq_poly_get_coeff_fmpq(m,(fmpq_poly_ptr)a,i);
    fmpq_get_mpz_frac(num,den,m);
    mpz_out_str (d->f_write,SSI_BASE, num);
    fputc(' ',d->f_write);
    mpz_out_str (d->f_write,SSI_BASE, den);
    fputc(' ',d->f_write);
  }
  mpz_clear(den);
  mpz_clear(num);
  fmpq_clear(m);
#else
  mpq_t m;
  mpq_init(m);
  mpz_t num,den;
  mpz_init(num);
  mpz_init(den);
  for(int i=l; i>=0; i--)
  {
    fmpq_poly_get_coeff_mpq(m,(fmpq_poly_ptr)a,i);
    mpq_get_num(num,m);
    mpq_get_den(den,m);
    mpz_out_str (d->f_write,SSI_BASE, num);
    fputc(' ',d->f_write);
    mpz_out_str (d->f_write,SSI_BASE, den);
    fputc(' ',d->f_write);
  }
  mpz_clear(den);
  mpz_clear(num);
  mpq_clear(m);
#endif
}
static number ReadFd(const ssiInfo *d, const coeffs)
{
  // format: len a_len .. a_0
  fmpq_poly_ptr aa=(fmpq_poly_ptr)omAlloc(sizeof(fmpq_poly_t));
  fmpq_poly_init(aa);
  int l=s_readint(d->f_read);
  mpz_t tmp;
  mpz_init(tmp);
  fmpq_t m;
  fmpq_init(m);
  fmpz_t num,den;
  fmpz_init(num);
  fmpz_init(den);
  for (int i=l;i>=0;i--)
  {
    s_readmpz_base (d->f_read,tmp, SSI_BASE);
    fmpz_set_mpz(num, tmp);
    s_readmpz_base (d->f_read,tmp, SSI_BASE);
    fmpz_set_mpz(den, tmp);
    fmpq_set_fmpz_frac(m,num,den);
    fmpq_poly_set_coeff_fmpq(aa,i,m);
  }
  mpz_clear(tmp);
  fmpz_clear(den);
  fmpz_clear(num);
  fmpq_clear(m);
  return (number)aa;
}
// cfClearContent
// cfClearDenominators
static number ConvFactoryNSingN( const CanonicalForm, const coeffs)
{
  WerrorS("not yet: ConvFactoryNSingN");
  return NULL;
}
static CanonicalForm ConvSingNFactoryN( number, BOOLEAN, const coeffs )
{
  WerrorS("not yet: ConvSingNFactoryN");
  return CanonicalForm(0);
}
char * CoeffName(const coeffs r)
{
  STATIC_VAR char CoeffName_flint_Q[20];
  sprintf(CoeffName_flint_Q,"flintQp[%s]",r->pParameterNames[0]);
  return (char*)CoeffName_flint_Q;

}
coeffs flintQInitCfByName(char *s,n_coeffType n)
{
  const char start[]="flintQp[";
  const int start_len=strlen(start);
  if (strncmp(s,start,start_len)==0)
  {
    s+=start_len;
    char st[10];
    int l=sscanf(s,"%s",st);
    if (l==1)
    {
      while (st[strlen(st)-1]==']') st[strlen(st)-1]='\0';
      return nInitChar(n,(void*)st);
    }
  }
  return NULL;
}
#ifdef LDEBUG
static BOOLEAN DBTest(number, const char *, const int, const coeffs)
{
  return TRUE;
}
#endif
static void KillChar(coeffs cf)
{
  omFree((ADDRESS)(cf->pParameterNames[0]));
  omFreeSize(cf->pParameterNames,sizeof(char*));
}
BOOLEAN flintQ_InitChar(coeffs cf, void * infoStruct)
{
  char *pp=(char*)infoStruct;
  cf->cfCoeffName    = CoeffName;
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
  cf->cfWriteFd  = WriteFd;
  cf->cfReadFd = ReadFd;
#ifdef LDEBUG
  cf->cfDBTest       = DBTest;
#endif

  cf->iNumberOfParameters = 1;
  char **pn=(char**)omAlloc0(sizeof(char*));
  pn[0]=omStrDup(pp);
  cf->pParameterNames = (const char **)pn;
  cf->has_simple_Inverse= FALSE;
  cf->has_simple_Alloc= FALSE;
  cf->is_field=FALSE;

  return FALSE;
}
#endif
