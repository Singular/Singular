#ifndef COEFFS_H
#define COEFFS_H
/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/* $Id$ */
/*
* ABSTRACT
*/

#include <misc/auxiliary.h>
/* for assume: */
#include <reporter/reporter.h>

#include <coeffs/si_gmp.h>

#ifdef HAVE_FACTORY
class CanonicalForm;
#endif

enum n_coeffType
{
  n_unknown=0,
  n_Zp,
  n_Q,
  n_R,
  n_GF,
  n_long_R,
  n_Ext,  // used for all extensions (of Zp, of Q AND OF EXTENSIONS THEREOF)
  n_long_C,
  // only used if HAVE_RINGS is defined:
  n_Z,
  n_Zn,
  n_Zpn, // does no longer exist?
  n_Z2m,
  n_CF
};

struct snumber;
typedef struct snumber *   number;

struct snumber;
typedef struct snumber *   number;

/* standard types */
#ifdef HAVE_RINGS
typedef unsigned long NATNUMBER;
typedef mpz_ptr int_number;
#endif

struct ip_sring;
typedef struct ip_sring *         ring;

struct n_Procs_s;
typedef struct  n_Procs_s  n_Procs_s;
typedef struct  n_Procs_s  *coeffs;

typedef number (*numberfunc)(number a, number b, const coeffs r);

/// maps "a", which lives in src, into dst
typedef number (*nMapFunc)(number a, const coeffs src, const coeffs dst);

struct n_Procs_s
{
   coeffs next;
   unsigned int ringtype;  /* =0 => coefficient field,
                             !=0 => coeffs from one of the rings */

   // general properties:
   /// TRUE, if nNew/nDelete/nCopy are dummies
   BOOLEAN has_simple_Alloc;
   /// TRUE, if std should make polynomials monic (if nInvers is cheap)
   /// if false, then a gcd routine is used for a content computation
   BOOLEAN has_simple_Inverse;

   // tests for numbers.cc:
   BOOLEAN (*nCoeffIsEqual)(const coeffs r, n_coeffType n, void * parameter);

   /// output of coeff description via Print
   void (*cfCoeffWrite)(const coeffs r);

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

   /// Inplace: a *= b
   void    (*cfInpMult)(number &a, number b, const coeffs r);
   /// maps the bigint i (from dummy) into the coeffs dst
   number  (*cfInit_bigint)(number i, const coeffs dummy, const coeffs dst);

#ifdef HAVE_FACTORY
   number (*convFactoryNSingN)( const CanonicalForm n, const coeffs r);
   CanonicalForm (*convSingNFactoryN)( number n, BOOLEAN setChar, const coeffs r );
#endif


#ifdef LDEBUG
   /// Test: is "a" a correct number?
   BOOLEAN (*cfDBTest)(number a, const char *f, const int l, const coeffs r);
#endif

   number nNULL; /* the 0 as constant */
   int     char_flag;
   int     ref;
   n_coeffType type;
//-------------------------------------------

  /// For Zp_a, Q_a we need polynomials (due to polys)
  ring          algring; //< implementation of extensions needs polynomials...
  /// for Q_a/Zp_a, rInit
  ///number     minpoly;  //< no longer needed: replaced by
                          //  algring->minideal->[0]


//-------------------------------------------
  char* complex_parameter; //< the name of sqrt(-1), i.e. 'i' or 'j' etc...?

#ifdef HAVE_RINGS
  /* The following members are for representing the ring Z/n,
     where n is not a prime. We distinguish four cases:
     1.) n has at least two distinct prime factors. Then
         modBase stores n, modExponent stores 1, modNumber
         stores n, and mod2mMask is not used;
     2.) n = p^k for some odd prime p and k > 1. Then
         modBase stores p, modExponent stores k, modNumber
         stores n, and mod2mMask is not used;
     3.) n = 2^k for some k > 1; moreover, 2^k - 1 fits in
         an unsigned long. Then modBase stores 2, modExponent
         stores k, modNumber is not used, and mod2mMask stores
         2^k - 1, i.e., the bit mask '111..1' of length k.
     4.) n = 2^k for some k > 1; but 2^k - 1 does not fit in
         an unsigned long. Then modBase stores 2, modExponent
         stores k, modNumber stores n, and mod2mMask is not
         used;
     Cases 1.), 2.), and 4.) are covered by the implementation
     in the files rmodulon.h and rmodulon.cc, whereas case 3.)
     is implemented in the files rmodulo2m.h and rmodulo2m.cc. */
  int_number    modBase;
  unsigned long modExponent;
  int_number    modNumber;
  unsigned long mod2mMask;
#endif
  int        ch;  /* characteristic, rInit */

  short      float_len; /* additional char-flags, rInit */
  short      float_len2; /* additional char-flags, rInit */

  BOOLEAN   ShortOut; /// ffields need this.

// ---------------------------------------------------
  // for n_GF

  int m_nfCharQ;  ///< the number of elemts: q
  int m_nfM1;       ///< representation of -1
  int m_nfCharP;  ///< the characteristic: p
  int m_nfCharQ1; ///< q-1
  unsigned short *m_nfPlus1Table;
  int *m_nfMinPoly;
  char * m_nfParameter;
};
//
// test properties and type
/// Returns the type of coeffs domain
static inline n_coeffType getCoeffType(const coeffs r)
{
  assume(r != NULL);
  return r->type;
}

static inline int nInternalChar(const coeffs r)
{
  assume(r != NULL);
  return r->ch;
}

/// one-time initialisations for new coeffs
/// in case of an error return NULL
coeffs nInitChar(n_coeffType t, void * parameter);

/// undo all initialisations
void nKillChar(coeffs r);

/// initialisations after each ring change
static inline void nSetChar(const coeffs r)
{
  assume(r!=NULL); // r==NULL is an error
  if (r->cfSetChar!=NULL) r->cfSetChar(r);
}

void           nNew(number * a);
#define n_New(n, r)           nNew(n)


// the access methods (part 2):

/// return a copy of a
static inline number n_Copy(number n,    const coeffs r)
{   assume(r != NULL); assume(r->cfCopy!=NULL); return r->cfCopy(n, r); }

static inline void   n_Delete(number* p, const coeffs r)
{   assume(r != NULL); assume(r->cfDelete!= NULL); r->cfDelete(p, r); }

static inline BOOLEAN n_Equal(number a, number b, const coeffs r)
{ assume(r != NULL); assume(r->cfEqual!=NULL); return r->cfEqual(a, b, r); }

static inline BOOLEAN n_IsZero(number n, const coeffs r)
{ assume(r != NULL); assume(r->cfIsZero!=NULL); return r->cfIsZero(n,r); }

static inline BOOLEAN n_IsOne(number n,  const coeffs r)
{ assume(r != NULL); assume(r->cfIsOne!=NULL); return r->cfIsOne(n,r); }

static inline BOOLEAN n_IsMOne(number n, const coeffs r)
{ assume(r != NULL); assume(r->cfIsMOne!=NULL); return r->cfIsMOne(n,r); }

static inline BOOLEAN n_GreaterZero(number n, const coeffs r)
{ assume(r != NULL); assume(r->cfGreaterZero!=NULL); return r->cfGreaterZero(n,r); }
static inline BOOLEAN n_Greater(number a, number b, const coeffs r)
{ assume(r != NULL); assume(r->cfGreater!=NULL); return r->cfGreater(a,b,r); }

#ifdef HAVE_RINGS
static inline BOOLEAN n_IsUnit(number n, const coeffs r)
{ assume(r != NULL); assume(r->cfIsUnit!=NULL); return r->cfIsUnit(n,r); }

static inline number n_GetUnit(number n, const coeffs r)
{ assume(r != NULL); assume(r->cfGetUnit!=NULL); return r->cfGetUnit(n,r); }

static inline BOOLEAN n_DivBy(number a, number b, const coeffs r)
{ assume(r != NULL); assume(r->cfDivBy!=NULL); return r->cfDivBy(a,b,r); }
#endif

/// init with an integer
static inline number n_Init(int i,       const coeffs r)
{ assume(r != NULL); assume(r->cfInit!=NULL); return r->cfInit(i,r); }

/// conversion to int; 0 if not possible
static inline int n_Int(number n,        const coeffs r)
{ assume(r != NULL); assume(r->cfInt!=NULL); return r->cfInt(n,r); }

/// changes argument  inline: a:= -a
static inline number n_Neg(number n,     const coeffs r)
{ assume(r != NULL); assume(r->cfNeg!=NULL); return r->cfNeg(n,r); }

/// return 1/a
static inline number n_Invers(number a,  const coeffs r)
{ assume(r != NULL); assume(r->cfInvers!=NULL); return r->cfInvers(a,r); }

/// use for pivot strategies, (0) => 0, otherwise positive
static inline int    n_Size(number n,    const coeffs r)
{ assume(r != NULL); assume(r->cfSize!=NULL); return r->cfSize(n,r); }

/// normalize the number. i.e. go to some canonnical representation (inplace)
static inline void   n_Normalize(number& n, const coeffs r)
{ assume(r != NULL); assume(r->cfNormalize!=NULL); r->cfNormalize(n,r); }

/// Normalize and Write to the output buffer of reporter
static inline void   n_Write(number& n,  const coeffs r)
{ assume(r != NULL); assume(r->cfWrite!=NULL); r->cfWrite(n,r); }

/// Normalize and get denomerator
static inline number n_GetDenom(number& n, const coeffs r)
{ assume(r != NULL); assume(r->cfGetDenom!=NULL); return r->cfGetDenom(n, r); }

/// Normalize and get numerator
static inline number n_GetNumerator(number& n, const coeffs r)
{ assume(r != NULL); assume(r->cfGetNumerator!=NULL); return r->cfGetNumerator(n, r); }

static inline void   n_Power(number a, int b, number *res, const coeffs r)
{ assume(r != NULL); assume(r->cfPower!=NULL); r->cfPower(a,b,res,r); }

static inline number n_Mult(number a, number b, const coeffs r)
{ assume(r != NULL); assume(r->cfMult!=NULL); return r->cfMult(a, b, r); }

/// Inplace multiplication: a := a * b
static inline void n_InpMult(number &a, number b, const coeffs r)
{ assume(r != NULL); assume(r->cfInpMult!=NULL); r->cfInpMult(a,b,r); }

static inline number n_Sub(number a, number b, const coeffs r)
{ assume(r != NULL); assume(r->cfSub!=NULL); return r->cfSub(a, b, r); }

static inline number n_Add(number a, number b, const coeffs r)
{ assume(r != NULL); assume(r->cfAdd!=NULL); return r->cfAdd(a, b, r); }

static inline number n_Div(number a, number b, const coeffs r)
{ assume(r != NULL); assume(r->cfDiv!=NULL); return r->cfDiv(a,b,r); }

static inline number n_IntDiv(number a, number b, const coeffs r)
{ assume(r != NULL); assume(r->cfIntDiv!=NULL); return r->cfIntDiv(a,b,r); }

static inline number n_ExactDiv(number a, number b, const coeffs r)
{ assume(r != NULL); assume(r->cfExactDiv!=NULL); return r->cfExactDiv(a,b,r); }

static inline number n_Gcd(number a, number b, const coeffs r)
{ assume(r != NULL); assume(r->cfGcd!=NULL); return r->cfGcd(a,b,r); }

static inline number n_Lcm(number a, number b, const coeffs r)
{ assume(r != NULL); assume(r->cfLcm!=NULL); return r->cfLcm(a,b,r); }

static inline nMapFunc n_SetMap(const coeffs src, const coeffs dst)
{ assume(src != NULL && dst != NULL); assume(dst->cfSetMap!=NULL); return dst->cfSetMap(src,dst); }

static inline number n_Par(int n, const coeffs r)
{ assume(r != NULL); assume(r->cfPar!=NULL); return r->cfPar(n,r); }

static inline int n_ParDeg(number n, const coeffs r)
{ assume(r != NULL); assume(r->cfParDeg!=NULL); return r->cfParDeg(n,r); }

/// Tests whether n is a correct number: only used if LDEBUG is defined
static inline BOOLEAN n_DBTest(number n, const char *filename, const int linenumber, const coeffs r)
{
  assume(r != NULL); 
#ifdef LDEBUG
  assume(r->cfDBTest != NULL); 
  return r->cfDBTest(n, filename, linenumber, r);
#else
  return TRUE;
#endif
}

/// output the coeff description
static inline void   n_CoeffWrite(const coeffs r)
{ assume(r != NULL); assume(r->cfCoeffWrite != NULL); r->cfCoeffWrite(r); }

// Tests:
static inline BOOLEAN nCoeff_is_Ring_2toM(const coeffs r)
{ assume(r != NULL); return (r->ringtype == 1); }

static inline BOOLEAN nCoeff_is_Ring_ModN(const coeffs r)
{ assume(r != NULL); return (r->ringtype == 2); }

static inline BOOLEAN nCoeff_is_Ring_PtoM(const coeffs r)
{ assume(r != NULL); return (r->ringtype == 3); }

static inline BOOLEAN nCoeff_is_Ring_Z(const coeffs r)
{ assume(r != NULL); return (r->ringtype == 4); }

static inline BOOLEAN nCoeff_is_Ring(const coeffs r)
{ assume(r != NULL); return (r->ringtype != 0); }

/// returns TRUE, if r is not a field and r has no zero divisors (i.e is a domain)
static inline BOOLEAN nCoeff_is_Domain(const coeffs r)
{
  assume(r != NULL); 
#ifdef HAVE_RINGS
  return (r->ringtype == 4 || r->ringtype == 0);
#else
  return TRUE;
#endif
}

/// returns TRUE, if r is not a field and r has non-trivial units
static inline BOOLEAN nCoeff_has_Units(const coeffs r)
{ assume(r != NULL); return ((r->ringtype == 1) || (r->ringtype == 2) || (r->ringtype == 3)); }

static inline BOOLEAN nCoeff_is_Zp(const coeffs r)
{ assume(r != NULL); return getCoeffType(r)==n_Zp; }

static inline BOOLEAN nCoeff_is_Zp(const coeffs r, int p)
{ assume(r != NULL); return (getCoeffType(r)  && (r->ch == ABS(p))); }

static inline BOOLEAN nCoeff_is_Q(const coeffs r)
{ assume(r != NULL); return getCoeffType(r)==n_Q; }

static inline BOOLEAN nCoeff_is_numeric(const coeffs r) /* R, long R, long C */
{ assume(r != NULL);  return (getCoeffType(r)==n_R) || (getCoeffType(r)==n_long_R) || (getCoeffType(r)==n_long_C); }
// (r->ringtype == 0) && (r->ch ==  -1); ??


static inline BOOLEAN nCoeff_is_R(const coeffs r)
{ assume(r != NULL); return getCoeffType(r)==n_R; }

static inline BOOLEAN nCoeff_is_GF(const coeffs r)
{ assume(r != NULL); return getCoeffType(r)==n_GF; }

static inline BOOLEAN nCoeff_is_GF(const coeffs r, int q)
{ assume(r != NULL); return (getCoeffType(r)==n_GF) && (r->ch == q); }

static inline BOOLEAN nCoeff_is_Zp_a(const coeffs r)
{
  assume(r != NULL);
  return (r->ringtype == 0) && (getCoeffType(r)==n_Ext) && (r->ch < -1);
}

static inline BOOLEAN nCoeff_is_Zp_a(const coeffs r, int p)
{
  assume(r != NULL);
  return (r->ringtype == 0) && (getCoeffType(r)==n_Ext) && (r->ch < -1 )
                            && (-(r->ch) == ABS(p));
}

static inline BOOLEAN nCoeff_is_Q_a(const coeffs r)
{
  assume(r != NULL);
  return (r->ringtype == 0) && (getCoeffType(r)==n_Ext) && (r->ch == 0);
}

static inline BOOLEAN nCoeff_is_long_R(const coeffs r)
{ assume(r != NULL); return getCoeffType(r)==n_long_R; }

static inline BOOLEAN nCoeff_is_long_C(const coeffs r)
{ assume(r != NULL); return getCoeffType(r)==n_long_C; }

static inline BOOLEAN nCoeff_is_CF(const coeffs r)
{ assume(r != NULL); return getCoeffType(r)==n_CF; }

/// TRUE, if the computation of the inverse is fast (i.e. prefer leading coeff. 1 over content)
static inline BOOLEAN nCoeff_has_simple_inverse(const coeffs r)
{ assume(r != NULL); return r->has_simple_Inverse; }
/* Z/2^n, Z/p, GF(p,n), R, long_R, long_C*/
// /* { return (r->ch>1) || (r->ch== -1); } *//* Z/p, GF(p,n), R, long_R, long_C*/
// #ifdef HAVE_RINGS
// { return (r->ringtype > 0) || (r->ch>1) || ((r->ch== -1) && (r->float_len < 10)); } /* Z/2^n, Z/p, GF(p,n), R, long_R, long_C*/
// #else
// { return (r->ch>1) || ((r->ch== -1) && (r->float_len < 10)); } /* Z/p, GF(p,n), R, long_R, long_C*/
// #endif



/// TRUE if n_Delete/n_New are empty operations
static inline BOOLEAN nCoeff_has_simple_Alloc(const coeffs r)
{ assume(r != NULL); return r->has_simple_Alloc; }
/* Z/p, GF(p,n), R, Ring_2toM: nCopy, nNew, nDelete are dummies*/
// return (rField_is_Zp(r)
//         || rField_is_GF(r)
// #ifdef HAVE_RINGS
//             || rField_is_Ring_2toM(r)
// #endif
//             || rField_is_R(r)); }


static inline BOOLEAN nCoeff_is_Extension(const coeffs r)
{ assume(r != NULL); return (nCoeff_is_Q_a(r)) || (nCoeff_is_Zp_a(r)); } /* Z/p(a) and Q(a)*/

/// BOOLEAN n_Test(number a, const coeffs r)
#define n_Test(a,r)  n_DBTest(a, __FILE__, __LINE__, r)

// Missing wrappers for:
// cfIntMod, cfRePart, cfImPart, cfRead, cfName, cfInit_bigint
// HAVE_RINGS: cfDivComp, cfExtGcd... cfDivBy


// Deprecated:
static inline int n_GetChar(const coeffs r)
{ assume(r != NULL); return nInternalChar(r); }

#endif

