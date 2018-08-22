/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/*
* ABSTRACT: flint: nmod_poly_t
*/
#include <ctype.h> /* isdigit*/

#include "misc/auxiliary.h"

#ifdef HAVE_FLINT

#include <flint/flint.h>
#include <flint/nmod_poly.h>
#include "factory/factory.h"

#include "coeffs/coeffs.h"

#include "coeffs/numbers.h"
#include "coeffs/longrat.h"
#include "coeffs/modulop.h"
#include "coeffs/flintcf_Zn.h"

typedef nmod_poly_struct *nmod_poly_ptr;

/*2
* extracts a long integer from s, returns the rest
*/
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
  Print("flint:Z/%d[%s]",r->ch,r->pParameterNames[0]);
}

static BOOLEAN CoeffIsEqual(const coeffs r, n_coeffType n, void * parameter)
{
  flintZn_struct *pp=(flintZn_struct*)parameter;
  return (r->type==n) &&(r->ch==pp->ch)
          &&(r->pParameterNames!=NULL)
          &&(strcmp(r->pParameterNames[0],pp->name)==0);
}
static void KillChar(coeffs cf)
{
  omFree((ADDRESS)(cf->pParameterNames[0]));
  omFreeSize(cf->pParameterNames,sizeof(char*));
}
static void SetChar(const coeffs r)
{
  // dummy
}
static number Mult(number a, number b, const coeffs c)
{
  nmod_poly_ptr res=(nmod_poly_ptr)omAlloc(sizeof(*res));
  nmod_poly_init(res,c->ch);
  nmod_poly_mul(res,(nmod_poly_ptr)a,(nmod_poly_ptr)b);
  return (number)res;
}
static number Sub(number a, number b, const coeffs c)
{
  nmod_poly_ptr res=(nmod_poly_ptr)omAlloc(sizeof(nmod_poly_t));
  nmod_poly_init(res,c->ch);
  nmod_poly_sub(res,(nmod_poly_ptr)a,(nmod_poly_ptr)b);
  return (number)res;
}
static number Add(number a, number b, const coeffs c)
{
  nmod_poly_ptr res=(nmod_poly_ptr)omAlloc(sizeof(nmod_poly_t));
  nmod_poly_init(res,c->ch);
  nmod_poly_add(res,(nmod_poly_ptr)a,(nmod_poly_ptr)b);
  return (number)res;
}
static number Div(number a, number b, const coeffs c)
{
  nmod_poly_ptr res=(nmod_poly_ptr)omAlloc(sizeof(nmod_poly_t));
  nmod_poly_init(res,c->ch);
  if(nmod_poly_is_zero((nmod_poly_ptr)b))
  {
     WerrorS(nDivBy0);
  }
  else
  {
    nmod_poly_div(res,(nmod_poly_ptr)a,(nmod_poly_ptr)b);
    nmod_poly_t mod;
    nmod_poly_init(mod,c->ch);
    nmod_poly_rem(mod,(nmod_poly_ptr)a,(nmod_poly_ptr)b);
    if (!nmod_poly_is_zero((nmod_poly_ptr)mod))
    {
      WerrorS("cannot divide");
    }
    nmod_poly_clear(mod);
  }
  return (number)res;
}
static number ExactDiv(number a, number b, const coeffs c)
{
  nmod_poly_ptr res=(nmod_poly_ptr)omAlloc(sizeof(nmod_poly_t));
  nmod_poly_init(res,c->ch);
  if(nmod_poly_is_zero((nmod_poly_ptr)b))
  {
     WerrorS(nDivBy0);
  }
  else
    nmod_poly_div(res,(nmod_poly_ptr)a,(nmod_poly_ptr)b);
  return (number)res;
}
static number IntMod(number a, number b, const coeffs c)
{
  nmod_poly_ptr res=(nmod_poly_ptr)omAlloc(sizeof(nmod_poly_t));
  nmod_poly_init(res,c->ch);
  nmod_poly_rem(res,(nmod_poly_ptr)a,(nmod_poly_ptr)b);
  return (number)res;
}
static number Init (long i, const coeffs r)
{
  nmod_poly_ptr res=(nmod_poly_ptr)omAlloc(sizeof(nmod_poly_t));
  nmod_poly_init(res,r->ch);
  i= i%r->ch;
  if (i<0) i+=r->ch;
  nmod_poly_set_coeff_ui(res,0,i);
  return (number)res;
}
static number InitMPZ (mpz_t i, const coeffs r)
{
  nmod_poly_ptr res=(nmod_poly_ptr)omAlloc(sizeof(nmod_poly_t));
  nmod_poly_init(res,r->ch);
  mpz_t tmp;
  mpz_init(tmp);
  slong ii=mpz_mod_ui(tmp,i,r->ch);
  mpz_clear(tmp);
  nmod_poly_set_coeff_ui(res,0,ii);
  return (number)res;
}
static int Size (number n, const coeffs r)
{
  return nmod_poly_degree((nmod_poly_ptr)n);
}
static long Int (number &n, const coeffs r)
{
  if (nmod_poly_degree((nmod_poly_ptr)n)==0)
  {
    slong m;
    m=nmod_poly_get_coeff_ui((nmod_poly_ptr)n,0);
    return (long)m;
  }
  return 0;
}
static void MPZ(mpz_t result, number &n, const coeffs r)
{
  mpz_init(result);
  if (nmod_poly_degree((nmod_poly_ptr)n)==0)
  {
    slong m;
    m=nmod_poly_get_coeff_ui((nmod_poly_ptr)n,0);
    mpz_set_ui(result,m);
  }
}
static number Neg(number a, const coeffs r)
{
  nmod_poly_neg((nmod_poly_ptr)a,(nmod_poly_ptr)a);
  return a;
}
static number Invers(number a, const coeffs r)
{
  if(nmod_poly_is_zero((nmod_poly_ptr)a))
  {
    WerrorS(nDivBy0);
    return NULL;
  }
  if (nmod_poly_degree((nmod_poly_ptr)a)==0)
  {
    nmod_poly_ptr res=(nmod_poly_ptr)omAlloc(sizeof(nmod_poly_t));
    nmod_poly_init(res,r->ch);
    slong c=nmod_poly_get_coeff_ui((nmod_poly_ptr)a,0);
    extern number  nvInvers (number c, const coeffs r);
    c=(slong)nvInvers((number)c,r);
    nmod_poly_set_coeff_ui((nmod_poly_ptr)a,0,c);
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
 nmod_poly_ptr res=(nmod_poly_ptr)omAlloc(sizeof(nmod_poly_t));
 nmod_poly_init(res,r->ch);
 nmod_poly_set(res,(nmod_poly_ptr)a);
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
  //nmod_poly_print_pretty((nmod_poly_ptr)a,r->pParameterNames[0]);
  if (IsOne(a,r)) StringAppendS("1");
  else if (IsZero(a,r)) StringAppendS("0");
  else
  {
    StringAppendS("(");
    BOOLEAN need_plus=FALSE;
    for(int i=nmod_poly_length((nmod_poly_ptr)a);i>=0;i--)
    {
      slong m=nmod_poly_get_coeff_ui((nmod_poly_ptr)a,i);
      if (m!=0)
      {
        if (need_plus) StringAppendS("+");
        need_plus=TRUE;
        if (i>0)
        {
          if (m!=1) StringAppend("%d*",(int)m);
          if (i>1)
            StringAppend("%s^%d",r->pParameterNames[0],i);
          else if (i==1)
            StringAppend("%s",r->pParameterNames[0]);
        }
        else StringAppend("%d",(int)m);
      }
    }
    StringAppendS(")");
  }
}
static const char* Read(const char * st, number * a, const coeffs r)
{
// we only read "monomials" (i.e. [-][digits][parameter]),
// everythings else (+,*,^,()) is left to the singular interpreter
  const char *s=st;
  *a=(number)omAlloc(sizeof(nmod_poly_t));
  nmod_poly_init((nmod_poly_ptr)(*a),r->ch);
  BOOLEAN neg=FALSE;
  if (*s=='-') { neg=TRUE; s++;}
  if (isdigit(*s))
  {
    int z;
    s=Eati((char *)s, &z);
    nmod_poly_set_coeff_ui((nmod_poly_ptr)(*a),0,z);
  }
  else if(strncmp(s,r->pParameterNames[0],strlen(r->pParameterNames[0]))==0)
  {
    nmod_poly_set_coeff_ui((nmod_poly_ptr)(*a),1,1);
    s+=strlen(r->pParameterNames[0]);
    if(isdigit(*s))
    {
      int i=1;
      s=Eati(s,&i);
      if (i!=1)
      {
        nmod_poly_set_coeff_ui((nmod_poly_ptr)(*a),1,0);
        nmod_poly_set_coeff_ui((nmod_poly_ptr)(*a),i,1);
      }
    }
  }
  if (neg)
    nmod_poly_neg((nmod_poly_ptr)(*a),(nmod_poly_ptr)(*a));
  return s;
}
static void Normalize(number &a, const coeffs r)
{
}
static BOOLEAN Greater (number a, number b, const coeffs r)
{
  if (nmod_poly_length((nmod_poly_ptr)a)>nmod_poly_length((nmod_poly_ptr)b))
    return TRUE;
   else if (nmod_poly_length((nmod_poly_ptr)a)<nmod_poly_length((nmod_poly_ptr)b))
     return FALSE;
   for(int i=nmod_poly_length((nmod_poly_ptr)a);i>=0;i--)
   {
     slong ac=nmod_poly_get_coeff_ui((nmod_poly_ptr)a,i);
     slong bc=nmod_poly_get_coeff_ui((nmod_poly_ptr)b,i);
     if (ac>bc) return TRUE;
     else if (ac<bc) return FALSE;
   }
   return FALSE;
}
static BOOLEAN Equal (number a, number b, const coeffs r)
{
  return (nmod_poly_equal((nmod_poly_ptr)a,(nmod_poly_ptr)b));
}
static BOOLEAN IsZero (number a, const coeffs r)
{
  return nmod_poly_is_zero((nmod_poly_ptr)a);
}
static BOOLEAN IsOne (number a, const coeffs r)
{
  return nmod_poly_is_one((nmod_poly_ptr)a);
}
static BOOLEAN IsMOne (number k, const coeffs r)
{
  if (nmod_poly_length((nmod_poly_ptr)k)>0) return FALSE;
  slong m=nmod_poly_get_coeff_ui((nmod_poly_ptr)k,0);
  return (m+1==r->ch);
}
static BOOLEAN GreaterZero (number k, const coeffs r)
{
  // does it have a leading sign?
  // no: 0 and 1 do not have, everything else is in (...)
  return TRUE;
}
static void Power(number a, int i, number * result, const coeffs r)
{
  nmod_poly_ptr res=(nmod_poly_ptr)omAlloc(sizeof(nmod_poly_t));
  nmod_poly_init(res,r->ch);
  *result=(number)res;
  nmod_poly_pow((nmod_poly_ptr)(*result),(nmod_poly_ptr)a,i);
}
static number Gcd(number a, number b, const coeffs r)
{
  nmod_poly_ptr res=(nmod_poly_ptr)omAlloc(sizeof(nmod_poly_t));
  nmod_poly_init(res,r->ch);
  nmod_poly_gcd(res,(nmod_poly_ptr)a,(nmod_poly_ptr)b);
  return (number)res;
}
static number ExtGcd(number a, number b, number *s, number *t,const coeffs r)
{
  nmod_poly_ptr res=(nmod_poly_ptr)omAlloc(sizeof(nmod_poly_t));
  nmod_poly_init(res,r->ch);
  nmod_poly_init((nmod_poly_ptr)*s,r->ch);
  nmod_poly_init((nmod_poly_ptr)*t,r->ch);
  nmod_poly_xgcd(res,(nmod_poly_ptr)*s,(nmod_poly_ptr)*t,(nmod_poly_ptr)a,(nmod_poly_ptr)b);
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
    nmod_poly_clear((nmod_poly_ptr)*a);
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
  nmod_poly_ptr res=(nmod_poly_ptr)omAlloc(sizeof(nmod_poly_t));
  nmod_poly_init(res,dst->ch);
  long ii;
  if (SR_HDL(i) & SR_INT)
  {
    ii=SR_TO_INT(i) % dst->ch;
  }
  else
  {
    mpz_t tmp;
    mpz_init(tmp);
    ii=mpz_mod_ui(tmp,i->z,dst->ch);
    mpz_clear(tmp);
  }
  if (ii<0) ii+=dst->ch;
  nmod_poly_set_coeff_ui(res,0,ii);
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
  return nmod_poly_degree((nmod_poly_ptr)x);
}
static number Parameter(const int i, const coeffs r)
{
  nmod_poly_ptr res=(nmod_poly_ptr)omAlloc(sizeof(nmod_poly_t));
  nmod_poly_init(res,r->ch);
  nmod_poly_set_coeff_ui(res,1,1);
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
static char * CoeffName(const coeffs r)
{
  static char CoeffName_flint_Zn[20];
  sprintf(CoeffName_flint_Zn,"flint:Z/%d[%s]",r->ch,r->pParameterNames[0]);
  return (char*)CoeffName_flint_Zn;
}
static char* CoeffString(const coeffs r)
{
  char *buf=(char*)omAlloc(12+10 /*ch*/+strlen(r->pParameterNames[0]));
  sprintf(buf,"flintZn(%d,\"%s\")",r->ch,r->pParameterNames[0]);
  return buf;
}
coeffs flintZnInitCfByName(char *s,n_coeffType n)
{
  const char start[]="flint:Z/";
  const int start_len=strlen(start);
  if (strncmp(s,start,start_len)==0)
  {
    s+=start_len;
    int p;
    char st[10];
    int l=sscanf(s,"%d[%s",&p,st);
    if (l==2)
    {
      flintZn_struct info;
      info.ch=p;
      while (st[strlen(st)-1]==']') st[strlen(st)-1]='\0';
      info.name=st;
      return nInitChar(n,(void*)&info);
    }
  }
  return NULL;
}
static void WriteFd(number a, const ssiInfo *d, const coeffs)
{
  // format: len a_len .. a_0
  nmod_poly_ptr aa=(nmod_poly_ptr)a;
  int l=nmod_poly_length(aa);
  fprintf(d->f_write,"%d ",l);
  for(int i=l; i>=0; i--)
  {
    ulong ul=nmod_poly_get_coeff_ui(aa,i);
    fprintf(d->f_write,"%lu ", ul);
  }
}

static number ReadFd(const ssiInfo *d, const coeffs r)
{
  // format: len a_len .. a_0
  nmod_poly_ptr aa=(nmod_poly_ptr)omAlloc(sizeof(nmod_poly_t));
  nmod_poly_init(aa,r->ch);
  int l=s_readint(d->f_read);
  unsigned long ul;
  for (int i=l;i>=0;i--)
  {
    unsigned long ul=s_readlong(d->f_read);
    nmod_poly_set_coeff_ui(aa,i,ul);
  }
  return (number)aa;
}
#ifdef LDEBUG
static BOOLEAN DBTest(number a, const char *f, const int l, const coeffs r)
{
  return TRUE;
}
#endif
BOOLEAN flintZn_InitChar(coeffs cf, void * infoStruct)
{
  flintZn_struct *pp=(flintZn_struct*)infoStruct;
  cf->ch=pp->ch;

  cf->cfCoeffString  = CoeffString;
  cf->cfCoeffName    = CoeffName;
  cf->cfCoeffWrite   = CoeffWrite;
  cf->nCoeffIsEqual  = CoeffIsEqual;
  cf->cfKillChar     = KillChar;
  cf->cfSetChar      = SetChar;
  cf->cfMult         = Mult;
  cf->cfSub          = Sub;
  cf->cfAdd          = Add;
  cf->cfDiv          = Div;
  cf->cfExactDiv     = ExactDiv; // ???
  cf->cfInit         = Init;
  cf->cfInitMPZ      = InitMPZ;
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
  //default: cf->cfGetDenom     = GetDenom;
  //default: cf->cfGetNumerator = GetNumerator;
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
  cf->cfWriteFd = WriteFd;
  cf->cfReadFd  = ReadFd;
#ifdef LDEBUG
  cf->cfDBTest       = DBTest;
#endif

  cf->iNumberOfParameters = 1;
  char **pn=(char**)omAlloc0(sizeof(char*));
  pn[0]=(char*)omStrDup(pp->name);
  cf->pParameterNames = (const char **)pn;
  cf->has_simple_Inverse= FALSE;
  cf->has_simple_Alloc= FALSE;
  cf->is_field=FALSE;

  return FALSE;
}
#endif
