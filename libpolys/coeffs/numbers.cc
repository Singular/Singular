/*****************************************
*  Computer Algebra System SINGULAR      *
*****************************************/

/*
* ABSTRACT: interface to coefficient aritmetics
*/

#include <string.h>
#include <stdlib.h>

#include "misc/auxiliary.h"
#include "misc/mylimits.h"
#include "factory/factory.h"

#include "reporter/reporter.h"

#include "coeffs/coeffs.h"
#include "coeffs/numbers.h"

#include "coeffs/longrat.h"
#include "coeffs/modulop.h"
#include "coeffs/gnumpfl.h"
#include "coeffs/gnumpc.h"
#include "coeffs/ffields.h"
#include "coeffs/shortfl.h"
#include "coeffs/ntupel.h"
#include "coeffs/flintcf_Qrat.h"

#ifdef HAVE_RINGS
#include "coeffs/rmodulo2m.h"
#include "coeffs/rmodulon.h"
#include "coeffs/rintegers.h"
#endif

#ifdef HAVE_POLYEXTENSIONS
#include "polys/ext_fields/algext.h"
#include "polys/ext_fields/transext.h"
#endif


//static int characteristic = 0;
//extern int IsPrime(int p);

VAR n_Procs_s *cf_root=NULL;

static void   ndDelete(number* d, const coeffs) { *d=NULL; }
static number ndAnn(number, const coeffs cf) { WarnS("cfAnn undefined"); return n_Init(0,cf); }
static char* ndCoeffString(const coeffs r)
{
  return omStrDup(r->cfCoeffName(r));
}
static void ndCoeffWrite(const coeffs r,BOOLEAN)
{
  PrintS(r->cfCoeffName(r));
}
static char* ndCoeffName(const coeffs r)
{
  STATIC_VAR char s[20];
  snprintf(s,11,"Coeffs(%d)",r->type);
  return s;
}
static void   ndInpMult(number &a, number b, const coeffs r)
{
  number n=r->cfMult(a,b,r);
  r->cfDelete(&a,r);
  a=n;
}
static void ndInpAdd(number &a, number b, const coeffs r)
{
  number n=r->cfAdd(a,b,r);
  r->cfDelete(&a,r);
  a=n;
}

static void ndPower(number a, int i, number * res, const coeffs r)
{
  if (i==0)
  {
    *res = r->cfInit(1, r);
  }
  else if (i==1)
  {
    *res = r->cfCopy(a, r);
  }
  else if (i==2)
  {
    *res = r->cfMult(a, a, r);
  }
  else if (i<0)
  {
    number b = r->cfInvers(a, r);
    ndPower(b, -i, res, r);
    r->cfDelete(&b, r);
  }
  else
  {
    ndPower(a, i/2, res, r);
    r->cfInpMult(*res, *res, r);
    if (i&1)
    {
      r->cfInpMult(*res, a, r);
    }
  }
}
static number ndInvers(number a, const coeffs r)
{
  number one=r->cfInit(1,r);
  number res=r->cfDiv(one,a,r);
  r->cfDelete(&one,r);
  return res;
}
static number ndInvers_Ring(number a, const coeffs r)
{
  if (!r->cfIsUnit(a,r)) Print("ndInvers_Ring used with non-unit\n");
  number one=r->cfInit(1,r);
  number res=r->cfDiv(one,a,r);
  r->cfDelete(&one,r);
  return res;
}

static BOOLEAN ndIsUnit_Ring(number a, const coeffs r)
{ return r->cfIsOne(a,r)|| r->cfIsMOne(a,r); }
static BOOLEAN ndIsUnit_Field(number a, const coeffs r)
{ return !r->cfIsZero(a,r); }
static number ndGetUnit_Ring(number, const coeffs r)
{ return r->cfInit(1,r); }
static number ndRandom(siRandProc p, number, number, const coeffs cf)
{ return cf->cfInit(p(),cf); }
static number ndEucNorm(number a, const coeffs cf)
{ return cf->cfInit(cf->cfSize(a,cf),cf); }
#ifdef LDEBUG
// static void   nDBDummy1(number* d,char *, int) { *d=NULL; }
static BOOLEAN ndDBTest(number, const char *, const int, const coeffs){ return TRUE; }
#endif

static number ndFarey(number,number,const coeffs r)
{
  Werror("farey not implemented for %s (c=%d)",r->cfCoeffName(r),getCoeffType(r));
  return n_Init(0,r);
}
static number ndXExtGcd(number, number, number *, number *, number *, number *, const coeffs r)
{
  Werror("XExtGcd not implemented for %s (c=%d)",r->cfCoeffName(r),getCoeffType(r));
  return n_Init(0,r);
}
static number ndChineseRemainder(number *,number *,int,BOOLEAN,CFArray&,const coeffs r)
{
  Werror("ChineseRemainder not implemented for %s (c=%d)",r->cfCoeffName(r),getCoeffType(r));
  return r->cfInit(0,r);
}
number ndReadFd( const ssiInfo *, const coeffs r)
{
  Warn("ReadFd not implemented for %s (c=%d)",r->cfCoeffName(r),getCoeffType(r));
  return n_Init(0,r);
}

static void ndWriteFd(number, const ssiInfo *, const coeffs r)
{
  Warn("WriteFd not implemented for %s (c=%d)",r->cfCoeffName(r),getCoeffType(r));
}

static int ndParDeg(number n, const coeffs r)
{
  return (-r->cfIsZero(n,r));
}

static number ndParameter(const int, const coeffs r)
{
  return r->cfInit(1,r);
}

BOOLEAN n_IsZeroDivisor( number a, const coeffs r)
{
  BOOLEAN ret = n_IsZero(a, r);
  int c = n_GetChar(r);
  if (ret || (c==0) || (r->is_field))
    return ret; /*n_IsZero(a, r)*/
  number ch = n_Init( c, r );
  number g = n_Gcd( ch, a, r );
  ret = !n_IsOne (g, r);
  n_Delete(&ch, r);
  n_Delete(&g, r);
  return ret;
}

void   ndNormalize(number&, const coeffs) { }
static number ndReturn0(number, const coeffs r)        { return r->cfInit(0,r); }
number ndGcd(number, number, const coeffs r)    { return r->cfInit(1,r); }
static number ndIntMod(number a, number b, const coeffs R)
{
  if (R->is_field)
    return R->cfInit(0,R);
  else // implementation for a non-field:
  {
    number d=n_Div(a,b,R);
    number p=n_Mult(b,d,R);
    number r=n_Sub(a,p,R);
    n_Delete(&p,R);
    n_Delete(&d,R);
    return r;
  }
}
static number ndGetDenom(number &, const coeffs r)     { return r->cfInit(1,r); }
static number ndGetNumerator(number &a,const coeffs r) { return r->cfCopy(a,r); }
static int    ndSize(number a, const coeffs r)         { return (int)r->cfIsZero(a,r)==FALSE; }

static void ndClearContent(ICoeffsEnumerator& numberCollectionEnumerator, number& c, const coeffs r)
{
  assume(r != NULL);

  // no fractions
  assume(!(  nCoeff_is_Q(r) ));
  // all coeffs are given by integers!!!

  numberCollectionEnumerator.Reset();

  if( !numberCollectionEnumerator.MoveNext() ) // empty zero polynomial?
  {
    c = n_Init(1, r);
    return;
  }

  number &curr = numberCollectionEnumerator.Current();

#ifdef HAVE_RINGS
  /// TODO: move to a separate implementation
  if (nCoeff_is_Ring(r))
  {
    if (nCoeff_has_Units(r))
    {
      c = n_GetUnit(curr, r);

      if (!n_IsOne(c, r))
      {
        number inv = n_Invers(c, r);

        n_InpMult(curr, inv, r);

        while( numberCollectionEnumerator.MoveNext() )
        {
          number &n = numberCollectionEnumerator.Current();
          n_Normalize(n, r); // ?
          n_InpMult(n, inv, r); // TODO: either this or directly divide!!!?
        }

        n_Delete(&inv, r);
      }
    } else c = n_Init(1, r);

    return;
  }
#endif

  assume(!nCoeff_is_Ring(r));
  assume(nCoeff_is_Zp(r) || nCoeff_is_numeric(r) || nCoeff_is_GF(r) || nCoeff_is_Zp_a(r) || nCoeff_is_Q_algext(r));

  n_Normalize(curr, r); // Q: good/bad/ugly??

  if (!n_IsOne(curr, r))
  {
    number t = curr; // takes over the curr! note: not a reference!!!

    curr = n_Init(1, r); // ???

    number inv = n_Invers(t, r);

    while( numberCollectionEnumerator.MoveNext() )
    {
      number &n = numberCollectionEnumerator.Current();
      n_InpMult(n, inv, r); // TODO: either this or directly divide!!!?
//      n_Normalize(n, r); // ?
    }

    n_Delete(&inv, r);

    c = t;
  } else
    c = n_Copy(curr, r); // c == 1 and nothing else to do...
}

static void ndClearDenominators(ICoeffsEnumerator& /*numberCollectionEnumerator*/, number& d, const coeffs r)
{
  assume( r != NULL );
  assume( !(nCoeff_is_Q(r) || nCoeff_is_transExt(r) || nCoeff_is_algExt(r)) );
  assume( nCoeff_is_Ring(r) || nCoeff_is_Zp(r) || nCoeff_is_numeric(r) || nCoeff_is_GF(r) );

  d = n_Init(1, r);
}

number ndCopyMap(number a, const coeffs aRing, const coeffs r)
{
  // aRing and r need not be the same, but must be the same representation
  assume(aRing->rep==r->rep);
  if ( nCoeff_has_simple_Alloc(r) && nCoeff_has_simple_Alloc(aRing) )
    return a;
  else
    return r->cfCopy(a, r);
}

static void ndKillChar(coeffs) {}
static void ndSetChar(const coeffs) {}

static number ndCopy(number a, const coeffs) { return a; }
number nd_Copy(number a, const coeffs r) { return r->cfCopy(a, r); }

#ifdef HAVE_RINGS
static BOOLEAN ndDivBy(number, number, const coeffs) { return TRUE; } // assume a,b !=0
static int ndDivComp(number, number, const coeffs) { return 2; }
static number  ndExtGcd (number, number, number *, number *, const coeffs r) { return r->cfInit(1,r); }
#endif

CanonicalForm ndConvSingNFactoryN( number, BOOLEAN /*setChar*/, const coeffs)
{
  CanonicalForm term(0);
  WerrorS("no conversion to factory");
  return term;
}

static number ndConvFactoryNSingN( const CanonicalForm, const coeffs r)
{
  WerrorS("no conversion from factory");
  return n_Init(0,r);
}

/**< [in, out] a bigint number >= 0  */
/**< [out] the GMP equivalent    */
/// Converts a non-negative bigint number into a GMP number.
static void ndMPZ(mpz_t result, number &n, const coeffs r)
{
  mpz_init_set_si( result, r->cfInt(n, r) );
}

static number ndInitMPZ(mpz_t m, const coeffs r)
{
  return r->cfInit( mpz_get_si(m), r);
}

static const char *ndRead(const char * s, number *n, const coeffs r)
{
  *n=n_Init(1,r);
  return s;
}
static nMapFunc ndSetMap(const coeffs src, const coeffs dst)
{
  if (src==dst) return ndCopyMap;
  Werror("cfSetMap is undefined for %s",nCoeffString(dst));
  return ndCopyMap;
}

static BOOLEAN ndCoeffIsEqual(const coeffs r, n_coeffType n, void *)
{
  /* test, if r is an instance of nInitCoeffs(n,parameter) */
  /* if parameter is not needed */
  return (n==r->type);
}

number ndQuotRem (number a, number b, number * r, const coeffs R)
{
  // implementation for a field: r: 0, result: n_Div
  if(R->is_field)
  {
    *r=n_Init(0,R);
    return n_Div(a,b,R);
  }
  else
  // implementation for a non-field:
  {
    number d=n_Div(a,b,R);
    number p=n_Mult(b,d,R);
    *r=n_Sub(a,p,R);
    n_Delete(&p,R);
    return d;
  }
}
STATIC_VAR n_coeffType nLastCoeffs=n_CF;
VAR cfInitCharProc nInitCharTableDefault[]=
{ NULL,        /*n_unknown */
 npInitChar,   /* n_Zp */
 nlInitChar,   /* n_Q */
 nrInitChar,   /* n_R */
 nfInitChar,   /* n_GF */
 ngfInitChar,  /* n_long_R */
 #ifdef HAVE_POLYEXTENSIONS
 n2pInitChar, /* n_polyExt */
 naInitChar,  /* n_algExt */
 ntInitChar,  /* n_transExt */
 #else
 NULL,        /* n_polyExt */
 NULL,        /* n_algExt */
 NULL,        /* n_transExt */
 #endif
 ngcInitChar,  /* n_long_C */
 nnInitChar,   /* n_nTupel */
 #ifdef HAVE_RINGS
 nrzInitChar,  /* n_Z */
 nrnInitChar,  /* n_Zn */
 nrnInitChar,  /* n_Znm */
 nr2mInitChar, /* n_Z2m */
 #else
 NULL,         /* n_Z */
 NULL,         /* n_Zn */
 NULL,         /* n_Znm */
 NULL,         /* n_Z2m */
 #endif
 flintQrat_InitChar, /* n_FlintQrat */
 NULL         /* n_CF */
};

STATIC_VAR cfInitCharProc *nInitCharTable=nInitCharTableDefault;
/*2
* init operations for coeffs r
*/
coeffs nInitChar(n_coeffType t, void * parameter)
{
  n_Procs_s *n=cf_root;

  while((n!=NULL) && (n->nCoeffIsEqual!=NULL) && (!n->nCoeffIsEqual(n,t,parameter)))
      n=n->next;

  if (n==NULL)
  {
    n=(n_Procs_s*)omAlloc0(sizeof(n_Procs_s));
    n->next=cf_root;
    n->ref=1;
    n->type=t;

    // default entries (different from NULL) for some routines:
    n->nCoeffIsEqual = ndCoeffIsEqual;
    n->cfSize = ndSize;
    n->cfGetDenom= ndGetDenom;
    n->cfGetNumerator= ndGetNumerator;
    n->cfImPart=ndReturn0;
    n->cfDelete= ndDelete;
    n->cfAnn = ndAnn;
    n->cfCoeffString = ndCoeffString;
    n->cfCoeffWrite = ndCoeffWrite;
    n->cfCoeffName = ndCoeffName; // should alway be changed!
    n->cfInpAdd=ndInpAdd;
    n->cfInpMult=ndInpMult;
    n->cfCopy = ndCopy;
    n->cfIntMod=ndIntMod; /* dummy !! */
    n->cfNormalize=ndNormalize;
    n->cfGcd  = ndGcd;
    n->cfNormalizeHelper  = ndGcd; /* tricky, isn't it ?*/
    n->cfLcm  = ndGcd; /* tricky, isn't it ?*/
    n->cfInitMPZ = ndInitMPZ;
    n->cfMPZ = ndMPZ;
    n->cfPower = ndPower;
    n->cfQuotRem = ndQuotRem;
    n->cfInvers = ndInvers;
    n->cfRandom = ndRandom;

    n->cfKillChar = ndKillChar; /* dummy */
    n->cfSetChar = ndSetChar; /* dummy */
    // temp. removed to catch all the coeffs which miss to implement this!

    n->cfChineseRemainder = ndChineseRemainder; /* not implemented */
    n->cfFarey = ndFarey; /* not implemented */
    n->cfParDeg = ndParDeg; /* not implemented */
    n->cfReadFd = ndReadFd; /* not implemented */
    n->cfWriteFd = ndWriteFd; /* not implemented */

    n->cfParameter = ndParameter;

    n->cfClearContent = ndClearContent;
    n->cfClearDenominators = ndClearDenominators;

    n->cfEucNorm = ndEucNorm;
#ifdef HAVE_RINGS
    n->cfDivComp = ndDivComp;
    n->cfDivBy = ndDivBy;
    n->cfExtGcd = ndExtGcd;
    n->cfXExtGcd = ndXExtGcd;
    //n->cfGetUnit = ndGetUnit_Ring;// set afterwards
#endif

    // report error, if not redefined
    n->cfRead=ndRead;
    n->cfSetMap=ndSetMap;

#ifdef LDEBUG
    n->cfDBTest=ndDBTest;
#endif

    n->convSingNFactoryN=ndConvSingNFactoryN;
    n->convFactoryNSingN=ndConvFactoryNSingN;

    BOOLEAN nOK=TRUE;
    // init
    if ((t<=nLastCoeffs) && (nInitCharTable[t]!=NULL))
      nOK = (nInitCharTable[t])(n,parameter);
    else
       Werror("Sorry: the coeff type [%d] was not registered: it is missing in nInitCharTable", (int)t);
    if (nOK)
    {
      omFreeSize(n,sizeof(*n));
      return NULL;
    }
    cf_root=n;
    // post init settings:
    if (n->cfRePart==NULL) n->cfRePart=n->cfCopy;
    if (n->cfExactDiv==NULL) n->cfExactDiv=n->cfDiv;
    if (n->cfSubringGcd==NULL) n->cfSubringGcd=n->cfGcd;
    if (n->cfWriteShort==NULL) n->cfWriteShort = n->cfWriteLong;
    if (n->cfIsUnit==NULL)
    {
      if (n->is_field) n->cfIsUnit=ndIsUnit_Field;
      else             n->cfIsUnit=ndIsUnit_Ring;
    }
    #ifdef HAVE_RINGS
    if (n->cfGetUnit==NULL)
    {
      if (n->is_field) n->cfGetUnit=n->cfCopy;
      else             n->cfGetUnit=ndGetUnit_Ring;
    }
    if ((n->cfInvers==ndInvers)&&(n->is_field))
    {
      n->cfInvers=ndInvers_Ring;
    }
    #endif


    if(n->cfMult==NULL)  PrintS("cfMult missing\n");
    if(n->cfSub==NULL)  PrintS("cfSub missing\n");
    if(n->cfAdd==NULL) PrintS("cfAdd missing\n");
    if(n->cfDiv==NULL) PrintS("cfDiv missing\n");
    if(n->cfExactDiv==NULL) PrintS("cfExactDiv missing\n");
    if(n->cfInit==NULL) PrintS("cfInit missing\n");
    if(n->cfInt==NULL) PrintS("cfInt missing\n");
    if(n->cfIsUnit==NULL) PrintS("cfIsUnit missing\n");
    if(n->cfGetUnit==NULL) PrintS("cfGetUnit missing\n");
    if(n->cfInpNeg==NULL)  PrintS("cfInpNeg missing\n");
    if(n->cfXExtGcd==NULL)  PrintS("cfXExtGcd missing\n");
    if(n->cfAnn==NULL)  PrintS("cfAnn missing\n");
    if(n->cfWriteLong==NULL) PrintS("cfWriteLong missing\n");

    assume(n->iNumberOfParameters>= 0);

    assume( (n->iNumberOfParameters == 0 && n->pParameterNames == NULL) ||
            (n->iNumberOfParameters >  0 && n->pParameterNames != NULL) );


    if(n->cfGreater==NULL) PrintS("cfGreater missing\n");
    if(n->cfEqual==NULL) PrintS("cfEqual missing\n");
    if(n->cfIsZero==NULL) PrintS("cfIsZero missing\n");
    if(n->cfIsOne==NULL) PrintS("cfIsOne missing\n");
    if(n->cfIsMOne==NULL) PrintS("cfIsMOne missing\n");
    if(n->cfGreaterZero==NULL) PrintS("cfGreaterZero missing\n");
    /* error reporter:
    if(n->cfRead==ndRead) PrintS("cfRead missing\n");
    if(n->cfSetMap==ndSetMap) PrintS("cfSetMap missing\n");
    */

    assume(n->type==t);

#ifndef SING_NDEBUG
    if(n->cfWriteLong==NULL) Warn("cfWrite is NULL for coeff %d",t);
    if(n->cfWriteShort==NULL) Warn("cfWriteShort is NULL for coeff %d",t);
#endif
  }
  else
  {
    n->ref++;
  }
  return n;
}

void nKillChar(coeffs r)
{
  if (r!=NULL)
  {
    r->ref--;
    if (r->ref<=0)
    {
      n_Procs_s tmp;
      n_Procs_s* n=&tmp;
      tmp.next=cf_root;
      while((n->next!=NULL) && (n->next!=r)) n=n->next;
      if (n->next==r)
      {
        n->next=n->next->next;
        if (cf_root==r) cf_root=n->next;
        assume (r->cfKillChar!=NULL); r->cfKillChar(r);
        omFreeSize((void *)r, sizeof(n_Procs_s));
        r=NULL;
      }
      else
      {
        WarnS("cf_root list destroyed");
      }
    }
  }
}

n_coeffType nRegister(n_coeffType n, cfInitCharProc p)
{
  if (n==n_unknown)
  {
    nLastCoeffs=(n_coeffType)(int(nLastCoeffs)+1);
    if (nInitCharTable==nInitCharTableDefault)
    {
      nInitCharTable=(cfInitCharProc*)omAlloc0(
                                          ((int)nLastCoeffs+1)*sizeof(cfInitCharProc));
      memcpy(nInitCharTable,nInitCharTableDefault,
              ((int)nLastCoeffs)*sizeof(cfInitCharProc));
    }
    else
    {
      nInitCharTable=(cfInitCharProc*)omReallocSize(nInitCharTable,
                                          ((int)nLastCoeffs)*sizeof(cfInitCharProc),
                                          (((int)nLastCoeffs)+1)*sizeof(cfInitCharProc));
    }

    nInitCharTable[nLastCoeffs]=p;
    return nLastCoeffs;
  }
  else
  {
    if (nInitCharTable[n]!=NULL) Print("coeff %d already initialized\n",n);
    nInitCharTable[n]=p;
    return n;
  }
}

struct nFindCoeffByName_s;
typedef struct nFindCoeffByName_s* nFindCoeffByName_p;

struct nFindCoeffByName_s
{
  n_coeffType n;
  cfInitCfByNameProc p;
  nFindCoeffByName_p next;
};

VAR nFindCoeffByName_p nFindCoeffByName_Root=NULL;
void nRegisterCfByName(cfInitCfByNameProc p,n_coeffType n)
{
  nFindCoeffByName_p h=(nFindCoeffByName_p)omAlloc0(sizeof(*h));
  h->p=p;
  h->n=n;
  h->next=nFindCoeffByName_Root;
  nFindCoeffByName_Root=h;
}

coeffs nFindCoeffByName(char *cf_name)
{
  n_Procs_s* n=cf_root;
  // try existings coeffs:
  while(n!=NULL)
  {
    if ((n->cfCoeffName!=NULL)
    && (strcmp(cf_name,n->cfCoeffName(n))==0)) return n;
    n=n->next;
  }
  // TODO: parametrized cf, e.g. flint:Z/26[a]
  // try existing types:
  nFindCoeffByName_p p=nFindCoeffByName_Root;
  while(p!=NULL)
  {
    coeffs cf=p->p(cf_name,p->n);
    if (cf!=NULL) return cf;
    p=p->next;
  }
  return NULL;
}

void n_Print(number& a,  const coeffs r)
{
   assume(r != NULL);
   n_Test(a,r);

   StringSetS("");
   n_Write(a, r);
   { char* s = StringEndS(); Print("%s", s); omFree(s); }
}

char* nEati(char *s, int *i, int m)
{

  if (((*s) >= '0') && ((*s) <= '9'))
  {
    unsigned long ii=0L;
    do
    {
      ii *= 10;
      ii += *s++ - '0';
      if ((m!=0) && (ii > (MAX_INT_VAL / 10))) ii = ii % m;
    }
    while (((*s) >= '0') && ((*s) <= '9'));
    if ((m!=0) && (ii>=(unsigned)m)) ii=ii%m;
    *i=(int)ii;
  }
  else (*i) = 1;
  return s;
}

char* nEati(char *s, long *i, int m)
{

  if (((*s) >= '0') && ((*s) <= '9'))
  {
    unsigned long ii=0L;
    do
    {
      ii *= 10;
      ii += *s++ - '0';
      if ((m!=0) && (ii > (LONG_MAX / 10))) ii = ii % m;
    }
    while (((*s) >= '0') && ((*s) <= '9'));
    if ((m!=0) && (ii>=(long)m)) ii=ii%m;
    *i=ii;
  }
  else (*i) = 1;
  return s;
}

/// extracts a long integer from s, returns the rest
char * nEatLong(char *s, mpz_ptr i)
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

