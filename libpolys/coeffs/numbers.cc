/*****************************************
*  Computer Algebra System SINGULAR      *
*****************************************/
/* $Id$ */

/*
* ABSTRACT: interface to coefficient aritmetics
*/

#include <string.h>
#include <stdlib.h>

#include "config.h"
#include <misc/auxiliary.h>

#ifdef HAVE_FACTORY
#include <factory/factory.h>
#endif


#include "coeffs.h"
#include <coeffs/numbers.h>

#include <reporter/reporter.h>
#include <omalloc/omalloc.h>
#include <coeffs/numbers.h>
#include <coeffs/longrat.h>
#include <coeffs/modulop.h>
#include <coeffs/gnumpfl.h>
#include <coeffs/gnumpc.h>
#include <coeffs/ffields.h>
#include <coeffs/shortfl.h>

#ifdef HAVE_RINGS
#include <coeffs/rmodulo2m.h>
#include <coeffs/rmodulon.h>
#include <coeffs/rintegers.h>
#endif

#ifdef HAVE_POLYEXTENSIONS
#include <polys/ext_fields/algext.h>
#include <polys/ext_fields/transext.h>
#endif



//static int characteristic = 0;
extern int IsPrime(int p);

/*0 implementation*/
number nNULL; /* the 0 as constant */

n_Procs_s *cf_root=NULL;

void   nNew(number* d) { *d=NULL; }
void   ndDelete(number* d, const coeffs) { *d=NULL; }
void   ndInpMult(number &a, number b, const coeffs r)
{
  number n=n_Mult(a,b,r);
  n_Delete(&a,r);
  a=n;
}
void ndInpAdd(number &a, number b, const coeffs r)
{
  number n=n_Add(a,b,r);
  n_Delete(&a,r);
  a=n;
}

#ifdef LDEBUG
void   nDBDummy1(number* d,char *, int) { *d=NULL; }
BOOLEAN ndDBTest(number, const char *, const int, const coeffs)
{
  return TRUE;
}
#endif


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

void   ndNormalize(number&, const coeffs) { }

char * ndName(number, const coeffs) { return NULL; }

number ndReturn0(number, const coeffs r) { return n_Init(0,r); }

number ndGcd(number, number, const coeffs r) { return n_Init(1,r); }

number ndIntMod(number, number, const coeffs r) { return n_Init(0,r); }

number ndGetDenom(number &, const coeffs r) { return n_Init(1,r); }
number ndGetNumerator(number &a,const coeffs r) { return n_Copy(a,r); }

int ndSize(number a, const coeffs r) { return (int)n_IsZero(a,r)==FALSE; }

number ndCopy(number a, const coeffs) { return a; }
number ndCopyMap(number a, const coeffs aRing, const coeffs r)
{
  assume( getCoeffType(r) == getCoeffType(aRing) );
  if ( nCoeff_has_simple_Alloc(r) && nCoeff_has_simple_Alloc(aRing) )
    return a;
	else
    return n_Copy(a, r);
}
void ndKillChar(coeffs) {}
void ndSetChar(const coeffs) {}

number nd_Copy(number a, const coeffs r) { return n_Copy(a, r); }

number ndChineseRemainder(number *,number *,int,const coeffs r){ return n_Init(0,r); }
#ifdef HAVE_RINGS
BOOLEAN ndDivBy(number, number, const coeffs) { return TRUE; } // assume a,b !=0
int ndDivComp(number, number, const coeffs) { return 2; }
BOOLEAN ndIsUnit(number a, const coeffs r) { return !n_IsZero(a,r); }
number  ndExtGcd (number, number, number *, number *, const coeffs r) { return n_Init(1,r); }
#endif

#ifdef HAVE_FACTORY
CanonicalForm ndConvSingNFactoryN( number, BOOLEAN /*setChar*/, const coeffs)
{
  CanonicalForm term(0);
  Werror("no conversion to factory");
  return term;
}

number ndConvFactoryNSingN( const CanonicalForm, const coeffs)
{
  Werror("no conversion from factory");
  return NULL;
}
#endif

number  ndInit_bigint(number, const coeffs, const coeffs)
{
  Werror("no conversion from bigint to this field");
  return NULL;
}

/**< [in, out] a bigint number >= 0  */
/**< [out] the GMP equivalent    */
/// Converts a non-negative bigint number into a GMP number.
void ndMPZ(mpz_t result, number &n, const coeffs r)
{
  mpz_init_set_si( result, n_Int(n, r) );
}

number ndInitMPZ(mpz_t m, const coeffs r)
{ 
  return n_Init( mpz_get_si(m), r);
}


BOOLEAN ndCoeffIsEqual(const coeffs r, n_coeffType n, void *)
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
 NULL,         /* n_Zpn */
 nr2mInitChar, /* n_Z2m */
 #else
 NULL,         /* n_Z */
 NULL,         /* n_Zn */
 NULL,         /* n_Zpn */
 NULL,         /* n_Z2m */
 #endif
 NULL	/* n_CF */
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
    n->cfSize = ndSize;
    n->cfGetDenom= ndGetDenom;
    n->cfGetNumerator= ndGetNumerator;
    n->cfName =  ndName;
    n->cfImPart=ndReturn0;
    n->cfDelete= ndDelete;
    n->cfInpMult=ndInpMult;
    n->cfCopy = ndCopy;
    n->cfIntMod=ndIntMod; /* dummy !! */
    n->cfNormalize=ndNormalize;
    n->cfGcd  = ndGcd;
    n->cfLcm  = ndGcd; /* tricky, isn't it ?*/
    n->cfInit_bigint = ndInit_bigint;
    n->cfInitMPZ = ndInitMPZ;
    n->cfMPZ = ndMPZ;

    //n->cfKillChar = ndKillChar; /* dummy */
    n->cfSetChar = ndSetChar; /* dummy */
    // temp. removed to catch all the coeffs which miss to implement this!

    n->cfChineseRemainder = ndChineseRemainder;
    n->cfFarey = ndGcd;

#ifdef HAVE_RINGS
    n->cfDivComp = ndDivComp;
    n->cfDivBy = ndDivBy;
    n->cfIsUnit = ndIsUnit;
    n->cfExtGcd = ndExtGcd;
    //n->cfGetUnit = (nMapFunc)NULL;
#endif

#ifdef fACTORY
    n->convSingNFactoryN=ndConvSingNFactoryN;
    n->convFactoryNSingN=ndConvFactoryNSingN;
#endif
    
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
    if (n->cfIntDiv==NULL) n->cfIntDiv=n->cfDiv;
    
#ifdef HAVE_RINGS
    if (n->cfGetUnit==NULL) n->cfGetUnit=n->cfCopy;
#endif
   
#ifndef NDEBUG
    assume(n->nCoeffIsEqual!=NULL);
    if(n->cfKillChar==NULL) Warn("cfKillChar is NULL for coeff %d",t);
		assume(n->cfSetChar!=NULL);
    assume(n->cfMult!=NULL);
    assume(n->cfSub!=NULL);
    assume(n->cfAdd!=NULL);
    assume(n->cfDiv!=NULL);
    assume(n->cfIntDiv!=NULL);
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
    assume(n->cfNeg!=NULL);
    assume(n->cfCopy!=NULL);
    assume(n->cfRePart!=NULL);
    assume(n->cfImPart!=NULL);
    assume(n->cfWrite!=NULL);
    assume(n->cfRead!=NULL);
    assume(n->cfNormalize!=NULL);
    assume(n->cfGreater!=NULL);
    //assume(n->cfDivBy!=NULL);
    assume(n->cfEqual!=NULL);
    assume(n->cfIsZero!=NULL);
    assume(n->cfIsOne!=NULL);
    assume(n->cfIsMOne!=NULL);
    assume(n->cfGreaterZero!=NULL);
    assume(n->cfPower!=NULL);
    assume(n->cfGetDenom!=NULL);
    assume(n->cfGetNumerator!=NULL);
    assume(n->cfGcd!=NULL);
    assume(n->cfLcm!=NULL);
    assume(n->cfDelete!=NULL);
    assume(n->cfSetMap!=NULL);
    assume(n->cfName!=NULL);
    assume(n->cfInpMult!=NULL);
//    assume(n->cfInit_bigint!=NULL);
    assume(n->cfCoeffWrite != NULL);
#ifdef LDEBUG
    assume(n->cfDBTest!=NULL);
#endif
    assume(n->type==t);
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
        r->cfDelete(&(r->nNULL),r);
        if (r->cfKillChar!=NULL) r->cfKillChar(r);
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
                                          nLastCoeffs*sizeof(cfInitCharProc));
      memcpy(nInitCharTable,nInitCharTableDefault,
              (nLastCoeffs-1)*sizeof(cfInitCharProc));
    }
    else
    {
      nInitCharTable=(cfInitCharProc*)omReallocSize(nInitCharTable,
                                          (((int)nLastCoeffs)-1)*sizeof(cfInitCharProc),
                                          ((int)nLastCoeffs)*sizeof(cfInitCharProc));
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

