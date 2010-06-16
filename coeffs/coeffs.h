#ifndef COEFFS_H
#define COEFFS_H
/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/* $Id$ */
/*
* ABSTRACT
*/

#include <auxiliary.h>
#include <si_gmp.h>

enum n_coeffType
{
  n_unknown=0,
  n_Zp,
  n_Q,
  n_R,
  n_GF,
  n_long_R,
  n_Zp_a,
  n_Q_a,
  n_long_C,
  // only used if HAVE_RINGS is defined
  n_Z,
  n_Zm,
  n_Zpn,
  n_Z2n
};

struct snumber;
typedef struct snumber *   number;

/* standard types */
#ifdef HAVE_RINGS
typedef unsigned long NATNUMBER;
typedef mpz_ptr int_number;
#endif

//
// the access macros:

#define n_Copy(n, r)          (r)->cfCopy(n,r)
#define n_Delete(n, r)        (r)->cfDelete(n,r)
#define n_Mult(n1, n2, r)     (r)->cfMult(n1, n2,r)
#define n_Add(n1, n2, r)      (r)->cfAdd(n1, n2,r)
#define n_IsZero(n, r)        (r)->cfIsZero(n,r)
#define n_Equal(n1, n2, r)    (r)->cfEqual(n1, n2,r)
#define n_Neg(n, r)           (r)->cfNeg(n,r)
#define n_Sub(n1, n2, r)      (r)->cfSub(n1, n2,r)
#define n_GetChar(r)          nInternalChar(r)
#define n_Init(i, r)          (r)->cfInit(i,r)
#define n_IsOne(n, r)         (r)->cfIsOne(n,r)
#define n_IsMOne(n, r)        (r)->cfIsMOne(n,r)
#define n_GreaterZero(n, r)   (r)->cfGreaterZero(n,r)
#define n_Write(n, r)         (r)->cfWrite(n,r)
#define n_Normalize(n, r)     (r)->cfNormalize(n,r)
#define n_Gcd(a, b, r)        (r)->cfGcd(a,b,r)
#define n_IntDiv(a, b, r)     (r)->cfIntDiv(a,b,r)
#define n_Div(a, b, r)        (r)->cfDiv(a,b,r)
#define n_Invers(a, r)        (r)->cfInvers(a,r)
#define n_ExactDiv(a, b, r)   (r)->cfExactDiv(a,b,r)
// #define n_Test(a,r)           (r)->cfDBTest(a,r,__FILE__,__LINE__)
#define n_InpMult(a, b, r)    (r)->cfInpMult(a,b,r)
#define n_Power(a, b, res, r) (r)->cfPower(a,b,res,r)
#define n_Size(n,r)           (r)->cfSize(n,r)
#define n_GetDenom(N,r)       (r)->cfGetDenom((N),r)
#define n_GetNumerator(N,r)   (r)->cfGetNumerator((N),r)

// and the routines w.r.t. currRing:
// (should only be used in the context of currRing, i.e. in the interpreter)
#define nCopy(n)          n_Copy(n, currRing->cf)
#define nDelete(n)        n_Delete(n, currRing->cf)
#define nMult(n1, n2)     n_Mult(n1, n2, currRing->cf)
#define nAdd(n1, n2)      n_Add(n1, n2, currRing->cf)
#define nIsZero(n)        n_IsZero(m, currRing->cf)
#define nEqual(n1, n2)    n_Equal(n1, n2, currRing->cf)
#define nNeg(n)           n_Neg(n, currRing->cf)
#define nSub(n1, n2)      n_Sub(n1, n2, currRing->cf)
#define nGetChar()        nInternalChar(currRing->cf)
#define nInit(i)          n_Init(i, currRing->cf)
#define nIsOne(n)         n_IsOne(n, currRing->cf)
#define nIsMOne(n)        n_IsMOne(n, currRing->cf)
#define nGreaterZero(n)   n_GreaterZero(n, currRing->cf)
#define nWrite(n)         n_Write(n,currRing->cf)
#define nNormalize(n)     n_Normalize(n,currRing->cf)
#define nGcd(a, b)        n_Gcd(a,b,currRing->cf)
#define nIntDiv(a, b)     n_IntDiv(a,b,currRing->cf)
#define nDiv(a, b)        n_Div(a,b,currRing->cf)
#define nInvers(a)        n_Invers(a,currRing->cf)
#define nExactDiv(a, b)   n_ExactDiv(a,b,currRing->cf)
#define nTest(a)          n_Test(a,currRing->cf)

#define nInpMult(a, b)    n_InpMult(a,b,currRing->cf)
#define nPower(a, b, res) n_Power(a,b,res,currRing->cf)
#define nSize(n)          n_Size(n,currRing->cf)
#define nGetDenom(N)      n_GetDenom((N),currRing->cf)
#define nGetNumerator(N)  n_GetNumerator((N),currRing->cf)

#define nSetMap(R)        n_SetMap(R,currRing->cf)

struct n_Procs_s;
typedef struct  n_Procs_s  n_Procs_s;
typedef struct  n_Procs_s  *coeffs;

typedef number (*numberfunc)(number a, number b, const coeffs r);

/// maps "a", which lives in src, into dst
typedef number (*nMapFunc)(number a, const coeffs src, const coeffs dst);

struct n_Procs_s
{
   coeffs next;
   unsigned int  ringtype;  /* 0 => coefficient field, 1 => coeffs from Z/2^m */

   // general properties:
   /// TRUE, if nNew/nDelete/nCopy are dummies
   BOOLEAN has_simple_Alloc;
   /// TRUE, if std should make polynomials monic (if nInvers is cheap)
   /// if false, then a gcd routine is required for a content computation
   BOOLEAN has_simple_Inverse;

   // tests for numbers.cc:
   BOOLEAN (*nCoeffIsEqual)(const coeffs r, n_coeffType n, void * parameter);

   // the union stuff

   // Zp:
   int npPrimeM;
   int npPminus1M;
   #ifdef HAVE_DIV_MOD
   unsigned short *npInvTable;
   #endif
   #if !defined(HAVE_DIV_MOD) || !defined(HAVE_MULT_MOD)
   unsigned short *npExpTable;
   unsigned short *npLogTable;
   #endif
   // Zp_a, Q_a
   // ?
   // initialisation:
   //void (*cfInitChar)(coeffs r, int parameter); // do one-time initialisations
   void (*cfKillChar)(coeffs r); //  undo all initialisations
                                // or NULL
   void (*cfSetChar)(const coeffs r); // initialisations after each ring change
                                // or NULL
   // general stuff
   numberfunc cfMult, cfSub ,cfAdd ,cfDiv, cfIntDiv, cfIntMod, cfExactDiv;
   /// init with an integer
   number  (*cfInit)(int i,const coeffs r);
   number  (*cfPar)(int i, const coeffs r);
   int     (*cfParDeg)(number n, const coeffs r);
   /// how complicated, (0) => 0, or positive
   int     (*cfSize)(number n, const coeffs r);
   /// convertion, 0 if impossible
   int     (*cfInt)(number &n, const coeffs r);
#ifdef HAVE_RINGS
   int     (*cfDivComp)(number a,number b,const coeffs r);
   BOOLEAN (*cfIsUnit)(number a,const coeffs r);
   number  (*cfGetUnit)(number a,const coeffs r);
   number  (*cfExtGcd)(number a, number b, number *s, number *t,const coeffs r);
#endif
   /// changes argument  inline: a:= -a
   number  (*cfNeg)(number a, const coeffs r);
   /// return 1/a
   number  (*cfInvers)(number a, const coeffs r);
   /// return a copy of a
   number  (*cfCopy)(number a, const coeffs r);
   number  (*cfRePart)(number a, const coeffs r);
   number  (*cfImPart)(number a, const coeffs r);
   void    (*cfWrite)(number &a, const coeffs r);
   const char *  (*cfRead)(const char * s, number * a, const coeffs r);
   void    (*cfNormalize)(number &a, const coeffs r);
   BOOLEAN (*cfGreater)(number a,number b, const coeffs r),
#ifdef HAVE_RINGS
           (*cfDivBy)(number a, number b, const coeffs r),
#endif
            /// tests
           (*cfEqual)(number a,number b, const coeffs r),
           (*cfIsZero)(number a, const coeffs r),
           (*cfIsOne)(number a, const coeffs r),
           (*cfIsMOne)(number a, const coeffs r),
           (*cfGreaterZero)(number a, const coeffs r);
   void    (*cfPower)(number a, int i, number * result, const coeffs r);
   number  (*cfGetDenom)(number &n, const coeffs r);
   number  (*cfGetNumerator)(number &n, const coeffs r);
   number  (*cfGcd)(number a, number b, const coeffs r);
   number  (*cfLcm)(number a, number b, const coeffs r);
   void    (*cfDelete)(number * a, const coeffs r);
   nMapFunc (*cfSetMap)(const coeffs src, const coeffs dst);

   /// For extensions (writes into global string buffer)
   char *  (*cfName)(number n, const coeffs r);

   /// Inline: a := b
   void    (*cfInpMult)(number &a, number b, const coeffs r);
   /// maps the bigint i (from dummy) into the coeffs dst
   number  (*cfInit_bigint)(number i, const coeffs dummy, const coeffs dst);

#ifdef LDEBUG
   /// Test: is "a" a correct number?
   BOOLEAN (*cfDBTest)(number a, const char *f,const int l,const coeffs r);
#endif

   number nNULL; /* the 0 as constant */
   int     char_flag;
   int     ref;
   n_coeffType type;
////-----------------------------------------
  char**     parameter; /* names of parameters, rInit */
  number     minpoly;  /* for Q_a/Zp_a, rInit */

#ifdef HAVE_RINGS
  int_number    ringflaga; /* Z/(ringflag^ringflagb)=Z/nrnModul*/
  unsigned long ringflagb;
  unsigned long nr2mModul;  /* Z/nr2mModul */
  int_number    nrnModul;
#endif
  int        ch;  /* characteristic, rInit */

  short      float_len; /* additional char-flags, rInit */
  short      float_len2; /* additional char-flags, rInit */

  BOOLEAN   ShortOut; /// ffields need this. 

};
//
// test properties and type
/// Returns the type of coeffs domain
static inline n_coeffType getCoeffType(const coeffs r)
{
  return r->type;
}

static inline int nInternalChar(const coeffs r)
{
  return r->ch;
}

/// one-time initialisations for new coeffs
coeffs nInitChar(n_coeffType t, void * parameter);
/// undo all initialisations
void nKillChar(coeffs r);
/// initialisations after each ring change
inline void nSetChar(coeffs r)
{
  if ((r!=NULL) && (r->cfSetChar!=NULL)) r->cfSetChar(r);
}

// nach einer heissen Diskussion
void           nNew(number * a);
#define n_New(n, r)           nNew(n)

static inline BOOLEAN n_DBTest(number a, const char *filename, const int linenumber, const coeffs r)
{
#ifdef LDEBUG
  return (r)->cfDBTest(a, filename, linenumber, r);
//  return (r)->cfDBTest(a,r,filename,linenumber); // testing error message
#else
  return TRUE;
#endif
}
/// BOOLEAN n_Test(number a, const coeffs r)
#define n_Test(a,r)  n_DBTest(a, __FILE__, __LINE__, r)

#endif

