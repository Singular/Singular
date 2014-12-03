/*****************************************
*  Computer Algebra System SINGULAR      *
*****************************************/

/*
* ABSTRACT: interface to coefficient aritmetics
*/

#include <string.h>
#include <stdlib.h>

#include <misc/auxiliary.h>
#include <omalloc/omalloc.h>
#include <factory/factory.h>

#include <reporter/reporter.h>

#include <coeffs/coeffs.h>
#include <coeffs/numbers.h>

#include <coeffs/longrat.h>
#include <coeffs/modulop.h>
#include <coeffs/gnumpfl.h>
#include <coeffs/gnumpc.h>
#include <coeffs/ffields.h>
#include <coeffs/shortfl.h>

#ifdef HAVE_RINGS
# include <coeffs/rmodulo2m.h>
# include <coeffs/rmodulon.h>
# include <coeffs/rintegers.h>
#endif

#ifdef HAVE_POLYEXTENSIONS
#include <polys/ext_fields/algext.h>
#include <polys/ext_fields/transext.h>
#endif


#ifdef HAVE_NUMSTATS
struct SNumberStatistic number_stats;
#endif /* HAVE_NUMSTATS */ 

//static int characteristic = 0;
//extern int IsPrime(int p);

n_Procs_s *cf_root=NULL;

void   nNew(number* d) { *d=NULL; }


static void   ndDelete(number* d, const coeffs) { *d=NULL; }
static number ndAnn(number, const coeffs) { return NULL;}
static char* ndCoeffString(const coeffs r)
{
  char *s=(char *)omAlloc(11);snprintf(s,11,"Coeffs(%d)",r->type);
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
  if (i==0) {
    *res = r->cfInit(1, r);
  } else if (i==1) {
    *res = r->cfCopy(a, r);
  } else if (i==2) {
    *res = r->cfMult(a, a, r);
  } else if (i<0) {
    number b = r->cfInvers(a, r);
    ndPower(b, -i, res, r);
    r->cfDelete(&b, r);
  } else {
    ndPower(a, i/2, res, r);
    r->cfInpMult(*res, *res, r);
    if (i&1) {
      r->cfInpMult(*res, a, r);
    }
  }
}

#ifdef LDEBUG
// static void   nDBDummy1(number* d,char *, int) { *d=NULL; }
static BOOLEAN ndDBTest(number, const char *, const int, const coeffs){ return TRUE; }
#endif

static number ndFarey(number,number,const coeffs r)
{
  Werror("farey not implemented for %s (c=%d)",r->cfCoeffString(r),getCoeffType(r));
  return NULL;
}
static number ndChineseRemainder(number *,number *,int,BOOLEAN,const coeffs r)
{
  Werror("ChineseRemainder not implemented for %s (c=%d)",r->cfCoeffString(r),getCoeffType(r));
  return r->cfInit(0,r);
}

static int ndParDeg(number n, const coeffs r)
{
  return (-r->cfIsZero(n,r));
}

static number ndParameter(const int, const coeffs r)
{
  Werror("ndParameter: n_Parameter is not implemented/relevant for (coeff_type = %d)",getCoeffType(r));
  return NULL;
}

BOOLEAN n_IsZeroDivisor( number a, const coeffs r)
{
  int c = n_GetChar(r);
  BOOLEAN ret = n_IsZero(a, r);
  if( (c != 0) && !ret )
  {
    number ch = n_Init( c, r );
    number g = n_Gcd( ch, a, r );
    ret = !n_IsOne (g, r);
    n_Delete(&ch, r);
    n_Delete(&g, r);
  }
  return ret;
}

static void   ndNormalize(number&, const coeffs) { }
static number ndReturn0(number, const coeffs r)        { return r->cfInit(0,r); }
static number ndGcd(number, number, const coeffs r)    { return r->cfInit(1,r); }
static number ndIntMod(number, number, const coeffs r) { return r->cfInit(0,r); }
static number ndGetDenom(number &, const coeffs r)     { return r->cfInit(1,r); }
static number ndGetNumerator(number &a,const coeffs r) { return r->cfCopy(a,r); }
static int    ndSize(number a, const coeffs r)         { return (int)r->cfIsZero(a,r)==FALSE; }
static char * ndCoeffName(const coeffs r)              { return r->cfCoeffString(r); }

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

static number ndCopy(number a, const coeffs) { return a; }
number ndCopyMap(number a, const coeffs aRing, const coeffs r)
{
  assume( getCoeffType(r) == getCoeffType(aRing) );
  if ( nCoeff_has_simple_Alloc(r) && nCoeff_has_simple_Alloc(aRing) )
    return a;
  else
    return r->cfCopy(a, r);
}

static void ndKillChar(coeffs) {}
static void ndSetChar(const coeffs) {}

number nd_Copy(number a, const coeffs r) { return r->cfCopy(a, r); }

#ifdef HAVE_RINGS
static BOOLEAN ndDivBy(number, number, const coeffs) { return TRUE; } // assume a,b !=0
static int ndDivComp(number, number, const coeffs) { return 2; }
static BOOLEAN ndIsUnit(number a, const coeffs r) { return !r->cfIsZero(a,r); }
static number  ndExtGcd (number, number, number *, number *, const coeffs r) { return r->cfInit(1,r); }
#endif

static CanonicalForm ndConvSingNFactoryN( number, BOOLEAN /*setChar*/, const coeffs)
{
  CanonicalForm term(0);
  Werror("no conversion to factory");
  return term;
}

static number ndConvFactoryNSingN( const CanonicalForm, const coeffs)
{
  Werror("no conversion from factory");
  return NULL;
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


static BOOLEAN ndCoeffIsEqual(const coeffs r, n_coeffType n, void *)
{
  /* test, if r is an instance of nInitCoeffs(n,parameter) */
  /* if paramater is not needed */
  return (n==r->type);
}

static n_coeffType nLastCoeffs=n_CF;
cfInitCharProc nInitCharTableDefault[]=
{ NULL,        /*n_unknown */
 npInitChar,   /* n_Zp */
 nlInitChar,   /* n_Q */
 nrInitChar,   /* n_R */
 nfInitChar,   /* n_GF */
 ngfInitChar,  /* n_long_R */
 #ifdef HAVE_POLYEXTENSIONS
 naInitChar,  /* n_algExt */
 ntInitChar,  /* n_transExt */
 #else
 NULL,        /* n_algExt */
 NULL,        /* n_transExt */
 #endif
 ngcInitChar,  /* n_long_C */
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
 NULL         /* n_CF */
};

static cfInitCharProc *nInitCharTable=nInitCharTableDefault;
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
    n->cfCoeffString = ndCoeffString; // should alway be changed!
    n->cfInpMult=ndInpMult;
    n->cfInpAdd=ndInpAdd;
    n->cfCopy = ndCopy;
    n->cfIntMod=ndIntMod; /* dummy !! */
    n->cfNormalize=ndNormalize;
    n->cfGcd  = ndGcd;
    n->cfNormalizeHelper  = ndGcd; /* tricky, isn't it ?*/
    n->cfLcm  = ndGcd; /* tricky, isn't it ?*/
    n->cfInitMPZ = ndInitMPZ;
    n->cfMPZ = ndMPZ;
    n->cfPower = ndPower;
    n->cfCoeffName = ndCoeffName;

    n->cfKillChar = ndKillChar; /* dummy */
    n->cfSetChar = ndSetChar; /* dummy */
    // temp. removed to catch all the coeffs which miss to implement this!

    n->cfChineseRemainder = ndChineseRemainder;
    n->cfFarey = ndFarey;
    n->cfParDeg = ndParDeg;

    n->cfParameter = ndParameter;

    n->cfClearContent = ndClearContent;
    n->cfClearDenominators = ndClearDenominators;

#ifdef HAVE_RINGS
    n->cfDivComp = ndDivComp;
    n->cfDivBy = ndDivBy;
    n->cfIsUnit = ndIsUnit;
    n->cfExtGcd = ndExtGcd;
    //n->cfGetUnit = (nMapFunc)NULL;
#endif

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

#ifdef HAVE_RINGS
    if (n->cfGetUnit==NULL) n->cfGetUnit=n->cfCopy;
#endif

    if(n->cfWriteShort==NULL)
      n->cfWriteShort = n->cfWriteLong;

    assume(n->nCoeffIsEqual!=NULL);
    assume(n->cfSetChar!=NULL);
    assume(n->cfCoeffString!=ndCoeffString);
    assume(n->cfMult!=NULL);
    assume(n->cfSub!=NULL);
    assume(n->cfAdd!=NULL);
    assume(n->cfDiv!=NULL);
    assume(n->cfIntMod!=NULL);
    assume(n->cfExactDiv!=NULL);
    assume(n->cfInit!=NULL);
    assume(n->cfInitMPZ!=NULL);
    assume(n->cfSize!=NULL);
    assume(n->cfInt!=NULL);
    assume(n->cfMPZ!=NULL);
    //assume(n->n->cfDivComp!=NULL);
    //assume(n->cfIsUnit!=NULL);
    //assume(n->cfGetUnit!=NULL);
    //assume(n->cfExtGcd!=NULL);
    assume(n->cfInpNeg!=NULL);
    assume(n->cfCopy!=NULL);

    assume(n->cfWriteLong!=NULL);
    assume(n->cfWriteShort!=NULL);

    assume(n->iNumberOfParameters>= 0);

    assume( (n->iNumberOfParameters == 0 && n->pParameterNames == NULL) ||
            (n->iNumberOfParameters >  0 && n->pParameterNames != NULL) );

    assume(n->cfParameter!=NULL);
    assume(n->cfParDeg!=NULL);

    assume(n->cfRead!=NULL);
    assume(n->cfNormalize!=NULL);
    assume(n->cfGreater!=NULL);
    //assume(n->cfDivBy!=NULL);
    assume(n->cfEqual!=NULL);
    assume(n->cfIsZero!=NULL);
    assume(n->cfIsOne!=NULL);
    assume(n->cfIsMOne!=NULL);
    assume(n->cfGreaterZero!=NULL);
    assume(n->cfGetDenom!=NULL);
    assume(n->cfGetNumerator!=NULL);
    assume(n->cfGcd!=NULL);
    assume(n->cfNormalizeHelper!=NULL);
    assume(n->cfDelete!=NULL);
    assume(n->cfSetMap!=NULL);
    assume(n->cfInpMult!=NULL);
//    assume(n->cfInit_bigint!=NULL);
    assume(n->cfCoeffWrite != NULL);

    assume(n->cfClearContent != NULL);
    assume(n->cfClearDenominators != NULL);

    assume(n->type==t);

#ifndef SING_NDEBUG
    if(n->cfKillChar==NULL) Warn("cfKillChar is NULL for coeff %d",t);
    if(n->cfWriteLong==NULL) Warn("cfWrite is NULL for coeff %d",t);
    if(n->cfWriteShort==NULL) Warn("cfWriteShort is NULL for coeff %d",t);
    if(n->cfCoeffString==ndCoeffString) Warn("cfCoeffString is undefined for coeff %d",t);
#endif

   if( n->nNULL == NULL )
     n->nNULL = n->cfInit(0, n); // may still remain NULL
  }
  else
  {
    n->ref++;
  }
  return n;
}

void nKillChar(coeffs r)
{
  STATISTIC(nKillChar);
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
        n_Delete(&(r->nNULL),r);
        assume (r->cfKillChar!=NULL); r->cfKillChar(r); // STATISTIC(nKillChar);
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


void n_Print(number& a,  const coeffs r)
{
   assume(r != NULL);
   n_Test(a,r);

   StringSetS("");
   n_Write(a, r);
   { char* s = StringEndS(); Print("%s", s); omFree(s); }
}


number n_convFactoryNSingN( const CanonicalForm n, const coeffs r)
{ STATISTIC(n_convFactoryNSingN); assume(r != NULL); assume(r->convFactoryNSingN != NULL); return r->convFactoryNSingN(n, r); }



CanonicalForm n_convSingNFactoryN( number n, BOOLEAN setChar, const coeffs r )
{ STATISTIC(n_convSingNFactoryN); assume(r != NULL); assume(r->convSingNFactoryN != NULL); return r->convSingNFactoryN(n, setChar, r); }
