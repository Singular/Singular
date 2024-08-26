/*! \file coeffs/coeffs.h Coefficient rings, fields and other domains suitable for Singular polynomials

  The main interface for Singular coefficients: \ref coeffs is the main handler for Singular numbers
*/
/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/

#ifndef COEFFS_H
#define COEFFS_H

#include "misc/auxiliary.h"
#include "omalloc/omalloc.h"

#include "misc/sirandom.h"
/* for assume: */
#include "reporter/reporter.h"
#include "reporter/s_buff.h"
#include "factory/factory.h"

#include "coeffs/si_gmp.h"
#include "coeffs/Enumerator.h"

class CanonicalForm;

enum n_coeffType
{
  n_unknown=0,
  n_Zp, /**< \F{p < 2^31} */
  n_Q,  /**< rational (GMP) numbers */
  n_R,  /**< single prescision (6,6) real numbers */
  n_GF, /**< \GF{p^n < 2^16} */
  n_long_R, /**< real floating point (GMP) numbers */
  n_polyExt, /**< used to represent polys as coefficients */
  n_algExt,  /**< used for all algebraic extensions, i.e.,
                the top-most extension in an extension tower
                is algebraic */
  n_transExt,  /**< used for all transcendental extensions, i.e.,
                  the top-most extension in an extension tower
                  is transcendental */
  n_long_C, /**< complex floating point (GMP) numbers */
  n_nTupel, /**< n-tupel of cf: ZZ/p1,... ZZ/pn, R, long_R */
  n_Z, /**< only used if HAVE_RINGS is defined  */
  n_Zn, /**< only used if HAVE_RINGS is defined */
  n_Znm, /**< only used if HAVE_RINGS is defined */
  n_Z2m, /**< only used if HAVE_RINGS is defined */
  n_FlintQrat, /**< rational function field over Q */
  n_CF, /**< ? */
  n_Nemo_AnticNumberField, /*17 */
  n_Nemo_QQField,          /*18 */
  n_Nemo_ZZRing,           /*19*/
  n_Nemo_FqPolyRepField,   /*20 */
  n_Nemo_fqPolyRepField,   /*21 */
  n_Nemo_Field,            /*22 */
  n_Nemo_Ring              /*23 */
};

extern const unsigned short fftable[];

struct snumber;
typedef struct snumber *   number;

/* standard types */
//struct ip_sring;
//typedef struct ip_sring *         ring; /* already needed in s_buff.h*/

/// @class coeffs coeffs.h coeffs/coeffs.h
///
/// The main handler for Singular numbers which are suitable for Singular polynomials.
///
/// With it one may implement a ring, a field, a domain etc.
///
struct n_Procs_s;
typedef struct  n_Procs_s  *coeffs;
typedef struct  n_Procs_s  const * const_coeffs;

typedef number (*numberfunc)(number a, number b, const coeffs r);

/// maps "a", which lives in src, into dst
typedef number (*nMapFunc)(number a, const coeffs src, const coeffs dst);


/// Abstract interface for an enumerator of number coefficients for an
/// object, e.g. a polynomial
typedef IEnumerator<number> ICoeffsEnumerator;

/// goes over coeffs given by the ICoeffsEnumerator and changes them.
/// Additionally returns a number;
typedef void (*nCoeffsEnumeratorFunc)(ICoeffsEnumerator& numberCollectionEnumerator, number& output, const coeffs r);

EXTERN_VAR omBin rnumber_bin;

#define FREE_RNUMBER(x) omFreeBin((void *)x, rnumber_bin)
#define ALLOC_RNUMBER() (number)omAllocBin(rnumber_bin)
#define ALLOC0_RNUMBER() (number)omAlloc0Bin(rnumber_bin)


/// Creation data needed for finite fields
typedef struct
{
  int GFChar;
  int GFDegree;
  const char* GFPar_name;
} GFInfo;

typedef struct
{
  short      float_len; /**< additional char-flags, rInit */
  short      float_len2; /**< additional char-flags, rInit */
  const char* par_name; /**< parameter name */
} LongComplexInfo;


enum n_coeffRep
{
  n_rep_unknown=0,
  n_rep_int,      /**< (int), see modulop.h */
  n_rep_gap_rat,  /**< (number), see longrat.h */
  n_rep_gap_gmp,  /**< (), see rinteger.h, new impl. */
  n_rep_poly,     /**< (poly), see algext.h */
  n_rep_rat_fct,  /**< (fraction), see transext.h */
  n_rep_gmp,      /**< (mpz_ptr), see rmodulon,h */
  n_rep_float,    /**< (float), see shortfl.h */
  n_rep_gmp_float,  /**< (gmp_float), see  */
  n_rep_gmp_complex,/**< (gmp_complex), see gnumpc.h */
  n_rep_gf        /**< (int), see ffields.h */
};

struct n_Procs_s
{
   // administration of coeffs:
   coeffs next;
   int     ref;
   n_coeffRep rep;
   n_coeffType type;
   /// how many variables of factory are already used by this coeff
   int     factoryVarOffset;

   // general properties:
   /// TRUE, if nDelete/nCopy are dummies
   BOOLEAN has_simple_Alloc;
   /// TRUE, if std should make polynomials monic (if nInvers is cheap)
   /// if false, then a gcd routine is used for a content computation
   BOOLEAN has_simple_Inverse;

   /// TRUE, if cf is a field
   BOOLEAN is_field;
   /// TRUE, if cf is a domain
   BOOLEAN is_domain;

   // tests for numbers.cc:
   BOOLEAN (*nCoeffIsEqual)(const coeffs r, n_coeffType n, void * parameter);

   /// output of coeff description via Print
   void (*cfCoeffWrite)(const coeffs r, BOOLEAN details);

   /// string output of coeff description
   char* (*cfCoeffString)(const coeffs r);

   /// default name of cf, should substitute cfCoeffWrite, cfCoeffString
   char* (*cfCoeffName)(const coeffs r);

   // ?
   // initialisation:
   //void (*cfInitChar)(coeffs r, int parameter); // do one-time initialisations
   void (*cfKillChar)(coeffs r); //  undo all initialisations
                                // or NULL
   void (*cfSetChar)(const coeffs r); // initialisations after each ring change
                                // or NULL
   // general stuff
   //   if the ring has a meaningful Euclidean structure, hopefully
   //   supported by cfQuotRem, then
   //     IntMod, Div should give the same result
   //     Div(a,b) = QuotRem(a,b, &IntMod(a,b))
   //   if the ring is not Euclidean or a field, then IntMod should return 0
   //   and Div the exact quotient. It is assumed that the function is
   //   ONLY called on Euclidean rings or in the case of an exact division.
   //
   //   cfDiv does an exact division, but has to handle illegal input
   //   cfExactDiv does an exact division, but no error checking
   //   (I'm not sure I understand and even less that this makes sense)
   numberfunc cfMult, cfSub ,cfAdd ,cfDiv, cfIntMod, cfExactDiv;

   /// init with an integer
   number  (*cfInit)(long i,const coeffs r);

   /// init with a GMP integer
   number  (*cfInitMPZ)(mpz_t i, const coeffs r);

   /// how complicated, (0) => 0, or positive
   int     (*cfSize)(number n, const coeffs r);

   /// conversion to long, 0 if impossible
   long    (*cfInt)(number &n, const coeffs r);

   /// Converts a (integer) number n into a GMP number, 0 if impossible
   void     (*cfMPZ)(mpz_t result, number &n, const coeffs r);

   /// changes argument  inline: a:= -a
   /// return -a! (no copy is returned)
   /// the result should be assigned to the original argument: e.g. a = n_InpNeg(a,r)
   number  (*cfInpNeg)(number a, const coeffs r);
   /// return 1/a
   number  (*cfInvers)(number a, const coeffs r);
   /// return a copy of a
   number  (*cfCopy)(number a, const coeffs r);
   number  (*cfRePart)(number a, const coeffs r);
   number  (*cfImPart)(number a, const coeffs r);

   /// print a given number (long format)
   void    (*cfWriteLong)(number a, const coeffs r);

   /// print a given number in a shorter way, if possible
   /// e.g. in K(a): a2 instead of a^2
   void    (*cfWriteShort)(number a, const coeffs r);

   // it is legal, but not always useful to have cfRead(s, a, r)
   //   just return s again.
   // Useful application (read constants which are not an projection
   // from int/bigint:
   // Let ring r = R,x,dp;
   // where R is a coeffs having "special" "named" elements (ie.
   // the primitive element in some algebraic extension).
   // If there is no interpreter variable of the same name, it is
   // difficult to create non-trivial elements in R.
   // Hence one can use the string to allow creation of R-elts using the
   // unbound name of the special element.
   const char *  (*cfRead)(const char * s, number * a, const coeffs r);

   void    (*cfNormalize)(number &a, const coeffs r);

   BOOLEAN (*cfGreater)(number a,number b, const coeffs r),
            /// tests
           (*cfEqual)(number a,number b, const coeffs r),
           (*cfIsZero)(number a, const coeffs r),
           (*cfIsOne)(number a, const coeffs r),
       // IsMOne is used for printing of polynomials:
       // -1 is only printed for constant monomials
           (*cfIsMOne)(number a, const coeffs r),
       //GreaterZero is used for printing of polynomials:
       //  a "+" is only printed in front of a coefficient
       //  if the element is >0. It is assumed that any element
       //  failing this will start printing with a leading "-"
           (*cfGreaterZero)(number a, const coeffs r);

   void    (*cfPower)(number a, int i, number * result, const coeffs r);
   number  (*cfGetDenom)(number &n, const coeffs r);
   number  (*cfGetNumerator)(number &n, const coeffs r);
   //CF: a Euclidean ring is a commutative, unitary ring with an Euclidean
   //  function f s.th. for all a,b in R, b ne 0, we can find q, r s.th.
   //  a = qb+r and either r=0 or f(r) < f(b)
   //  Note that neither q nor r nor f(r) are unique.
   number  (*cfGcd)(number a, number b, const coeffs r);
   number  (*cfSubringGcd)(number a, number b, const coeffs r);
   number  (*cfExtGcd)(number a, number b, number *s, number *t,const coeffs r);
   //given a and b in a Euclidean setting, return s,t,u,v sth.
   //  sa + tb = gcd
   //  ua + vb = 0
   //  sv + tu = 1
   //  ie. the 2x2 matrix (s t | u v) is unimodular and maps (a,b) to (g, 0)
   //CF: note, in general, this cannot be derived from ExtGcd due to
   //    zero divisors
   number  (*cfXExtGcd)(number a, number b, number *s, number *t, number *u, number *v, const coeffs r);
   //in a Euclidean ring, return the Euclidean norm as a bigint (of type number)
   number  (*cfEucNorm)(number a, const coeffs r);
   //in a principal ideal ring (with zero divisors): the annihilator
   // NULL otherwise
   number  (*cfAnn)(number a, const coeffs r);
   //find a "canonical representative of a modulo the units of r
   //return NULL if a is already normalized
   //otherwise, the factor.
   //(for Z: make positive, for z/nZ make the gcd with n
   //apparently it is GetUnit!
   //in a Euclidean ring, return the quotient and compute the remainder
   //rem can be NULL
   number  (*cfQuotRem)(number a, number b, number *rem, const coeffs r);
   number  (*cfLcm)(number a, number b, const coeffs r);
   number  (*cfNormalizeHelper)(number a, number b, const coeffs r);
   void    (*cfDelete)(number * a, const coeffs r);

   //CF: tries to find a canonical map from src -> dst
   nMapFunc (*cfSetMap)(const coeffs src, const coeffs dst);

   void    (*cfWriteFd)(number a, const ssiInfo *f, const coeffs r);
   number  (*cfReadFd)( const ssiInfo *f, const coeffs r);

   /// Inplace: a *= b
   void    (*cfInpMult)(number &a, number b, const coeffs r);

   /// Inplace: a += b
   void    (*cfInpAdd)(number &a, number b, const coeffs r);

   /// rational reconstruction: "best" rational a/b with a/b = p mod n
   //  or a = bp mod n
   //  CF: no idea what this would be in general
   //     it seems to be extended to operate coefficient wise in extensions.
   //     I presume then n in coeffs_BIGINT while p in coeffs
   number  (*cfFarey)(number p, number n, const coeffs);

   /// chinese remainder
   /// returns X with X mod q[i]=x[i], i=0..rl-1
   //CF: by the looks of it: q[i] in Z (coeffs_BIGINT)
   //    strange things happen in naChineseRemainder for example.
   number  (*cfChineseRemainder)(number *x, number *q,int rl, BOOLEAN sym,CFArray &inv_cache,const coeffs);

   /// degree for coefficients: -1 for 0, 0 for "constants", ...
   int (*cfParDeg)(number x,const coeffs r);

   /// create i^th parameter or NULL if not possible
   number  (*cfParameter)(const int i, const coeffs r);

   /// a function returning random elements
   number (*cfRandom)(siRandProc p, number p1, number p2, const coeffs cf);

   /// function pointer behind n_ClearContent
   nCoeffsEnumeratorFunc cfClearContent;

   /// function pointer behind n_ClearDenominators
   nCoeffsEnumeratorFunc cfClearDenominators;

   /// conversion to CanonicalForm(factory) to number
   number (*convFactoryNSingN)( const CanonicalForm n, const coeffs r);
   CanonicalForm (*convSingNFactoryN)( number n, BOOLEAN setChar, const coeffs r );

   /// Number of Parameters in the coeffs (default 0)
   int iNumberOfParameters;

   /// array containing the names of Parameters (default NULL)
   char const **  pParameterNames;
   // NOTE that it replaces the following:
// char* complex_parameter; //< the name of sqrt(-1) in n_long_C , i.e. 'i' or 'j' etc...?
// char * m_nfParameter; //< the name of parameter in n_GF

   /////////////////////////////////////////////
   // the union stuff

   //-------------------------------------------

  /* for extension fields we need to be able to represent polynomials,
     so here is the polynomial ring: */
  ring          extRing;

  //number     minpoly;  //< no longer needed: replaced by
  //                     //< extRing->qideal->[0]


  int        ch;  /* characteristic, set by the local *InitChar methods;
                     In field extensions or extensions towers, the
                     characteristic can be accessed from any of the
                     intermediate extension fields, i.e., in this case
                     it is redundant along the chain of field extensions;
                     CONTRARY to SINGULAR as it was, we do NO LONGER use
                     negative values for ch;
                     for rings, ch will also be set and is - per def -
                     the smallest number of 1's that sum up to zero;
                     however, in this case ch may not fit in an int,
                     thus ch may contain a faulty value */

  short      float_len; /* additional char-flags, rInit */
  short      float_len2; /* additional char-flags, rInit */

//  BOOLEAN   CanShortOut; //< if the elements can be printed in short format
//                       // this is set to FALSE if a parameter name has >2 chars
//  BOOLEAN   ShortOut; //< if the elements should print in short format

// ---------------------------------------------------
  // for n_GF

  int m_nfCharQ;  ///< the number of elements: q
  int m_nfM1;       ///< representation of -1
  int m_nfCharP;  ///< the characteristic: p
  int m_nfCharQ1; ///< q-1
  unsigned short *m_nfPlus1Table;
  int *m_nfMinPoly;

// ---------------------------------------------------
// for Zp:
  unsigned short *npInvTable;
  unsigned short *npExpTable;
  unsigned short *npLogTable;
   //   int npPrimeM; // NOTE: npPrimeM is deprecated, please use ch instead!
  int npPminus1M; ///< characteristic - 1
//-------------------------------------------
   int     (*cfDivComp)(number a,number b,const coeffs r);
   BOOLEAN (*cfIsUnit)(number a,const coeffs r);
   number  (*cfGetUnit)(number a,const coeffs r);
   /// test if b divides a
   /// cfDivBy(zero,b,r) is true, if b is a zero divisor
   BOOLEAN (*cfDivBy)(number a, number b, const coeffs r);
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
  mpz_ptr    modBase;
  unsigned long modExponent;
  mpz_ptr    modNumber;
  unsigned long mod2mMask;
  //returns coeffs with updated ch, modNumber and modExp
  coeffs (*cfQuot1)(number c, const coeffs r);

  /*CF: for blackbox rings, contains data needed to define the ring.
   * contents depends on the actual example.*/
  void * data;
#ifdef LDEBUG
   // must be last entry:
   /// Test: is "a" a correct number?
   // DB as in debug, not data base.
   BOOLEAN (*cfDBTest)(number a, const char *f, const int l, const coeffs r);
#endif
};

// test properties and type
/// Returns the type of coeffs domain
static FORCE_INLINE n_coeffType getCoeffType(const coeffs r)
{ assume(r != NULL); return r->type; }

/// one-time initialisations for new coeffs
/// in case of an error return NULL
coeffs nInitChar(n_coeffType t, void * parameter);

/// "copy" coeffs, i.e. increment ref
static FORCE_INLINE coeffs nCopyCoeff(const coeffs r)
{ assume(r!=NULL); r->ref++; return r;}

/// undo all initialisations
void nKillChar(coeffs r);

/// initialisations after each ring change
static FORCE_INLINE void nSetChar(const coeffs r)
{ assume(r!=NULL); assume(r->cfSetChar != NULL); r->cfSetChar(r); }

/// Return the characteristic of the coeff. domain.
static FORCE_INLINE int n_GetChar(const coeffs r)
{ assume(r != NULL); return r->ch; }


// the access methods (part 2):

/// return a copy of 'n'
static FORCE_INLINE number n_Copy(number n,    const coeffs r)
{ assume(r != NULL); assume(r->cfCopy!=NULL); return r->cfCopy(n, r); }

/// delete 'p'
static FORCE_INLINE void   n_Delete(number* p, const coeffs r)
{ assume(r != NULL); assume(r->cfDelete!= NULL); r->cfDelete(p, r); }

/// TRUE iff 'a' and 'b' represent the same number;
/// they may have different representations
static FORCE_INLINE BOOLEAN n_Equal(number a, number b, const coeffs r)
{ assume(r != NULL); assume(r->cfEqual!=NULL); return r->cfEqual(a, b, r); }

/// TRUE iff 'n' represents the zero element
static FORCE_INLINE BOOLEAN n_IsZero(number n, const coeffs r)
{ assume(r != NULL); assume(r->cfIsZero!=NULL); return r->cfIsZero(n,r); }

/// TRUE iff 'n' represents the one element
static FORCE_INLINE BOOLEAN n_IsOne(number n,  const coeffs r)
{ assume(r != NULL); assume(r->cfIsOne!=NULL); return r->cfIsOne(n,r); }

/// TRUE iff 'n' represents the additive inverse of the one element, i.e. -1
static FORCE_INLINE BOOLEAN n_IsMOne(number n, const coeffs r)
{ assume(r != NULL); assume(r->cfIsMOne!=NULL); return r->cfIsMOne(n,r); }

/// ordered fields: TRUE iff 'n' is positive;
/// in Z/pZ: TRUE iff 0 < m <= roundedBelow(p/2), where m is the long
///          representing n
/// in C:    TRUE iff (Im(n) != 0 and Im(n) >= 0) or
///                   (Im(n) == 0 and Re(n) >= 0)
/// in K(a)/<p(a)>: TRUE iff (n != 0 and (LC(n) > 0 or deg(n) > 0))
/// in K(t_1, ..., t_n): TRUE iff (LC(numerator(n) is a constant and > 0)
///                            or (LC(numerator(n) is not a constant)
/// in Z/2^kZ: TRUE iff 0 < n <= 2^(k-1)
/// in Z/mZ: TRUE iff the internal mpz is greater than zero
/// in Z: TRUE iff n > 0
///
/// !!! Recommendation: remove implementations for unordered fields
/// !!!                 and raise errors instead, in these cases
/// !!! Do not follow this recommendation: while writing polys,
/// !!! between 2 monomials will be an additional + iff !n_GreaterZero(next coeff)
///     Then change definition to include n_GreaterZero => printing does NOT
///     start with -
///
static FORCE_INLINE BOOLEAN n_GreaterZero(number n, const coeffs r)
{ assume(r != NULL); assume(r->cfGreaterZero!=NULL); return r->cfGreaterZero(n,r); }

/// ordered fields: TRUE iff 'a' is larger than 'b';
/// in Z/pZ: TRUE iff la > lb, where la and lb are the long's representing
//                             a and b, respectively
/// in C:    TRUE iff (Im(a) > Im(b))
/// in K(a)/<p(a)>: TRUE iff (a != 0 and (b == 0 or deg(a) > deg(b))
/// in K(t_1, ..., t_n): TRUE only if one or both numerator polynomials are
///                      zero or if their degrees are equal. In this case,
///                      TRUE if LC(numerator(a)) > LC(numerator(b))
/// in Z/2^kZ: TRUE if n_DivBy(a, b)
/// in Z/mZ: TRUE iff the internal mpz's fulfill the relation '>'
/// in Z: TRUE iff a > b
///
/// !!! Recommendation: remove implementations for unordered fields
/// !!!                 and raise errors instead, in these cases
static FORCE_INLINE BOOLEAN n_Greater(number a, number b, const coeffs r)
{ assume(r != NULL); assume(r->cfGreater!=NULL); return r->cfGreater(a,b,r); }

/// TRUE iff n has a multiplicative inverse in the given coeff field/ring r
static FORCE_INLINE BOOLEAN n_IsUnit(number n, const coeffs r)
{ assume(r != NULL); assume(r->cfIsUnit!=NULL); return r->cfIsUnit(n,r); }

static FORCE_INLINE coeffs n_CoeffRingQuot1(number c, const coeffs r)
{ assume(r != NULL); assume(r->cfQuot1 != NULL); return r->cfQuot1(c, r); }

static FORCE_INLINE int n_DivComp(number a, number b, const coeffs r)
{ assume(r != NULL); assume(r->cfDivComp!=NULL); return r->cfDivComp (a,b,r); }

/// in Z: 1
/// in Z/kZ (where k is not a prime): largest divisor of n (taken in Z) that
///                                   is co-prime with k
/// in Z/2^kZ: largest odd divisor of n (taken in Z)
/// other cases: not implemented
// CF: should imply that n/GetUnit(n) is normalized in Z/kZ
//   it would make more sense to return the inverse...
static FORCE_INLINE number n_GetUnit(number n, const coeffs r)
{ assume(r != NULL); assume(r->cfGetUnit!=NULL); return r->cfGetUnit(n,r); }

/// a number representing i in the given coeff field/ring r
static FORCE_INLINE number n_Init(long i,       const coeffs r)
{ assume(r != NULL); assume(r->cfInit!=NULL); return r->cfInit(i,r); }

/// conversion of a GMP integer to number
static FORCE_INLINE number n_InitMPZ(mpz_t n,     const coeffs r)
{ assume(r != NULL); assume(r->cfInitMPZ != NULL); return r->cfInitMPZ(n,r); }

/// conversion of n to an int; 0 if not possible
/// in Z/pZ: the representing int lying in (-p/2 .. p/2]
static FORCE_INLINE long n_Int(number &n,       const coeffs r)
{ assume(r != NULL); assume(r->cfInt!=NULL); return r->cfInt(n,r); }

/// conversion of n to a GMP integer; 0 if not possible
static FORCE_INLINE void n_MPZ(mpz_t result, number &n,       const coeffs r)
{ assume(r != NULL); assume(r->cfMPZ!=NULL); r->cfMPZ(result, n, r); }


/// in-place negation of n
/// MUST BE USED: n = n_InpNeg(n) (no copy is returned)
static FORCE_INLINE number n_InpNeg(number n,     const coeffs r)
{ assume(r != NULL); assume(r->cfInpNeg!=NULL); return r->cfInpNeg(n,r); }

/// return the multiplicative inverse of 'a';
/// raise an error if 'a' is not invertible
///
/// !!! Recommendation: rename to 'n_Inverse'
static FORCE_INLINE number n_Invers(number a,  const coeffs r)
{ assume(r != NULL); assume(r->cfInvers!=NULL); return r->cfInvers(a,r); }

/// return a non-negative measure for the complexity of n;
/// return 0 only when n represents zero;
/// (used for pivot strategies in matrix computations with entries from r)
static FORCE_INLINE int    n_Size(number n,    const coeffs r)
{ assume(r != NULL); assume(r->cfSize!=NULL); return r->cfSize(n,r); }

/// inplace-normalization of n;
/// produces some canonical representation of n;
///
/// !!! Recommendation: remove this method from the user-interface, i.e.,
/// !!!                 this should be hidden
static FORCE_INLINE void   n_Normalize(number& n, const coeffs r)
{ assume(r != NULL); assume(r->cfNormalize!=NULL); r->cfNormalize(n,r); }

/// write to the output buffer of the currently used reporter
//CF: the "&" should be removed, as one wants to write constants as well
static FORCE_INLINE void   n_WriteLong(number n,  const coeffs r)
{ assume(r != NULL); assume(r->cfWriteLong!=NULL); r->cfWriteLong(n,r); }

/// write to the output buffer of the currently used reporter
/// in a shortest possible way, e.g. in K(a): a2 instead of a^2
static FORCE_INLINE void   n_WriteShort(number n,  const coeffs r)
{ assume(r != NULL); assume(r->cfWriteShort!=NULL); r->cfWriteShort(n,r); }

static FORCE_INLINE void   n_Write(number n,  const coeffs r, const BOOLEAN bShortOut = TRUE)
{ if (bShortOut) n_WriteShort(n, r); else n_WriteLong(n, r); }


/// !!! Recommendation: This method is too cryptic to be part of the user-
/// !!!                 interface. As defined here, it is merely a helper
/// !!!                 method for parsing number input strings.
static FORCE_INLINE const char *n_Read(const char * s, number * a, const coeffs r)
{ assume(r != NULL); assume(r->cfRead!=NULL); return r->cfRead(s, a, r); }

/// return the denominator of n
/// (if elements of r are by nature not fractional, result is 1)
static FORCE_INLINE number n_GetDenom(number& n, const coeffs r)
{ assume(r != NULL); assume(r->cfGetDenom!=NULL); return r->cfGetDenom(n, r); }

/// return the numerator of n
/// (if elements of r are by nature not fractional, result is n)
static FORCE_INLINE number n_GetNumerator(number& n, const coeffs r)
{ assume(r != NULL); assume(r->cfGetNumerator!=NULL); return r->cfGetNumerator(n, r); }

/// return the quotient of 'a' and 'b', i.e., a/b;
/// raises an error if 'b' is not invertible in r
/// exception in Z: raises an error if 'a' is not divisible by 'b'
/// always: n_Div(a,b,r)*b+n_IntMod(a,b,r)==a
static FORCE_INLINE number n_Div(number a, number b, const coeffs r)
{ assume(r != NULL); assume(r->cfDiv!=NULL); return r->cfDiv(a,b,r); }

/// assume that there is a canonical subring in cf and we know
/// that division is possible for these a and b in the subring,
/// n_ExactDiv performs it, may skip additional tests.
/// Can always be substituted by n_Div at the cost of larger  computing time.
static FORCE_INLINE number n_ExactDiv(number a, number b, const coeffs r)
{ assume(r != NULL); assume(r->cfExactDiv!=NULL); return r->cfExactDiv(a,b,r); }

/// for r a field, return n_Init(0,r)
/// always: n_Div(a,b,r)*b+n_IntMod(a,b,r)==a
/// n_IntMod(a,b,r) >=0
static FORCE_INLINE number n_IntMod(number a, number b, const coeffs r)
{ assume(r != NULL); return r->cfIntMod(a,b,r); }

/// fill res with the power a^b
static FORCE_INLINE void   n_Power(number a, int b, number *res, const coeffs r)
{ assume(r != NULL); assume(r->cfPower!=NULL); r->cfPower(a,b,res,r); }

/// return the product of 'a' and 'b', i.e., a*b
static FORCE_INLINE number n_Mult(number a, number b, const coeffs r)
{ assume(r != NULL); assume(r->cfMult!=NULL); return r->cfMult(a, b, r); }

/// multiplication of 'a' and 'b';
/// replacement of 'a' by the product a*b
static FORCE_INLINE void n_InpMult(number &a, number b, const coeffs r)
{ assume(r != NULL); assume(r->cfInpMult!=NULL); r->cfInpMult(a,b,r); }

/// addition of 'a' and 'b';
/// replacement of 'a' by the sum a+b
static FORCE_INLINE void n_InpAdd(number &a, number b, const coeffs r)
{ assume(r != NULL); assume(r->cfInpAdd!=NULL); r->cfInpAdd(a,b,r); }

/// return the sum of 'a' and 'b', i.e., a+b
static FORCE_INLINE number n_Add(number a, number b, const coeffs r)
{ assume(r != NULL); assume(r->cfAdd!=NULL); return r->cfAdd(a, b, r); }


/// return the difference of 'a' and 'b', i.e., a-b
static FORCE_INLINE number n_Sub(number a, number b, const coeffs r)
{ assume(r != NULL); assume(r->cfSub!=NULL); return r->cfSub(a, b, r); }

/// in Z: return the gcd of 'a' and 'b'
/// in Z/nZ, Z/2^kZ: computed as in the case Z
/// in Z/pZ, C, R: not implemented
/// in Q: return the gcd of the numerators of 'a' and 'b'
/// in K(a)/<p(a)>: not implemented
/// in K(t_1, ..., t_n): not implemented
static FORCE_INLINE number n_Gcd(number a, number b, const coeffs r)
{ assume(r != NULL); assume(r->cfGcd!=NULL); return r->cfGcd(a,b,r); }
static FORCE_INLINE number n_SubringGcd(number a, number b, const coeffs r)
{ assume(r != NULL); assume(r->cfSubringGcd!=NULL); return r->cfSubringGcd(a,b,r); }

/// beware that ExtGCD is only relevant for a few chosen coeff. domains
/// and may perform something unexpected in some cases...
static FORCE_INLINE number n_ExtGcd(number a, number b, number *s, number *t, const coeffs r)
{ assume(r != NULL); assume(r->cfExtGcd!=NULL); return r->cfExtGcd (a,b,s,t,r); }
static FORCE_INLINE number n_XExtGcd(number a, number b, number *s, number *t, number *u, number *v, const coeffs r)
{ assume(r != NULL); assume(r->cfXExtGcd!=NULL); return r->cfXExtGcd (a,b,s,t,u,v,r); }
static FORCE_INLINE number  n_EucNorm(number a, const coeffs r)
{ assume(r != NULL); assume(r->cfEucNorm!=NULL); return r->cfEucNorm (a,r); }
/// if r is a ring with zero divisors, return an annihilator!=0 of b
/// otherwise return NULL
static FORCE_INLINE number  n_Ann(number a, const coeffs r)
{ assume(r != NULL); return r->cfAnn (a,r); }
static FORCE_INLINE number  n_QuotRem(number a, number b, number *q, const coeffs r)
{ assume(r != NULL); assume(r->cfQuotRem!=NULL); return r->cfQuotRem (a,b,q,r); }


/// in Z: return the lcm of 'a' and 'b'
/// in Z/nZ, Z/2^kZ: computed as in the case Z
/// in Z/pZ, C, R: not implemented
/// in K(a)/<p(a)>: not implemented
/// in K(t_1, ..., t_n): not implemented
static FORCE_INLINE number n_Lcm(number a, number b, const coeffs r)
{ assume(r != NULL); assume(r->cfLcm!=NULL); return r->cfLcm(a,b,r); }

/// assume that r is a quotient field (otherwise, return 1)
/// for arguments (a1/a2,b1/b2) return (lcm(a1,b2)/1)
static FORCE_INLINE number n_NormalizeHelper(number a, number b, const coeffs r)
{ assume(r != NULL); assume(r->cfNormalizeHelper!=NULL); return r->cfNormalizeHelper(a,b,r); }

number ndCopyMap(number a, const coeffs src, const coeffs dst);
/// set the mapping function pointers for translating numbers from src to dst
static FORCE_INLINE nMapFunc n_SetMap(const coeffs src, const coeffs dst)
{ assume(src != NULL && dst != NULL); assume(dst->cfSetMap!=NULL);
  if (src==dst) return ndCopyMap;
  return dst->cfSetMap(src,dst);
}

#ifdef LDEBUG
/// test whether n is a correct number;
/// only used if LDEBUG is defined
static FORCE_INLINE BOOLEAN n_DBTest(number n, const char *filename, const int linenumber, const coeffs r)
{ assume(r != NULL); assume(r->cfDBTest != NULL); return r->cfDBTest(n, filename, linenumber, r); }
/// BOOLEAN n_Test(number a, const coeffs r)
#define n_Test(a,r)  n_DBTest(a, __FILE__, __LINE__, r)
#else
#define n_Test(a,r)  1
#endif


/// output the coeff description
static FORCE_INLINE void   n_CoeffWrite(const coeffs r, BOOLEAN details = TRUE)
{ assume(r != NULL); assume(r->cfCoeffWrite != NULL); r->cfCoeffWrite(r, details); }

// Tests:
static FORCE_INLINE BOOLEAN nCoeff_is_Ring_2toM(const coeffs r)
{ assume(r != NULL); return (getCoeffType(r)==n_Z2m); }

static FORCE_INLINE BOOLEAN nCoeff_is_Ring_PtoM(const coeffs r)
{ assume(r != NULL); return (getCoeffType(r)==n_Znm); }

static FORCE_INLINE BOOLEAN nCoeff_is_Ring(const coeffs r)
{ assume(r != NULL); return (r->is_field==0); }

/// returns TRUE, if r is a field or r has no zero divisors (i.e is a domain)
static FORCE_INLINE BOOLEAN nCoeff_is_Domain(const coeffs r)
{
  assume(r != NULL);
  return (r->is_domain);
}

/// test whether 'a' is divisible 'b';
/// for r encoding a field: TRUE iff 'b' does not represent zero
/// in Z: TRUE iff 'b' divides 'a' (with remainder = zero)
/// in Z/nZ: TRUE iff (a = 0 and b divides n in Z) or
///                   (a != 0 and b/gcd(a, b) is co-prime with n, i.e.
///                                              a unit in Z/nZ)
/// in Z/2^kZ: TRUE iff ((a = 0 mod 2^k) and (b = 0 or b is a power of 2))
///                  or ((a, b <> 0) and (b/gcd(a, b) is odd))
static FORCE_INLINE BOOLEAN n_DivBy(number a, number b, const coeffs r)
{ assume(r != NULL);
  if( nCoeff_is_Ring(r) )
  {
    assume(r->cfDivBy!=NULL); return r->cfDivBy(a,b,r);
  }
  return !n_IsZero(b, r);
}

static FORCE_INLINE number n_ChineseRemainderSym(number *a, number *b, int rl, BOOLEAN sym,CFArray &inv_cache,const coeffs r)
{ assume(r != NULL); assume(r->cfChineseRemainder != NULL); return r->cfChineseRemainder(a,b,rl,sym,inv_cache,r); }

static FORCE_INLINE number n_Farey(number a, number b, const coeffs r)
{ assume(r != NULL); assume(r->cfFarey != NULL); return r->cfFarey(a,b,r); }

static FORCE_INLINE int n_ParDeg(number n, const coeffs r)
{ assume(r != NULL); assume(r->cfParDeg != NULL); return r->cfParDeg(n,r); }

/// Returns the number of parameters
static FORCE_INLINE int n_NumberOfParameters(const coeffs r)
{ assume(r != NULL); return r->iNumberOfParameters; }

/// Returns a (const!) pointer to (const char*) names of parameters
static FORCE_INLINE char const * * n_ParameterNames(const coeffs r)
{ assume(r != NULL); return r->pParameterNames; }

/// return the (iParameter^th) parameter as a NEW number
/// NOTE: parameter numbering: 1..n_NumberOfParameters(...)
static FORCE_INLINE number n_Param(const int iParameter, const coeffs r)
{ assume(r != NULL);
  assume((iParameter >= 1) || (iParameter <= n_NumberOfParameters(r)));
  assume(r->cfParameter != NULL);
  return r->cfParameter(iParameter, r);
}

static FORCE_INLINE number  n_RePart(number i, const coeffs cf)
{ assume(cf != NULL); assume(cf->cfRePart!=NULL); return cf->cfRePart(i,cf); }

static FORCE_INLINE number  n_ImPart(number i, const coeffs cf)
{ assume(cf != NULL); assume(cf->cfImPart!=NULL); return cf->cfImPart(i,cf); }

/// returns TRUE, if r is not a field and r has non-trivial units
static FORCE_INLINE BOOLEAN nCoeff_has_Units(const coeffs r)
{ assume(r != NULL); return ((getCoeffType(r)==n_Zn) || (getCoeffType(r)==n_Z2m) || (getCoeffType(r)==n_Znm)); }

static FORCE_INLINE BOOLEAN nCoeff_is_Zp(const coeffs r)
{ assume(r != NULL); return getCoeffType(r)==n_Zp; }

static FORCE_INLINE BOOLEAN nCoeff_is_Zp(const coeffs r, int p)
{ assume(r != NULL); return ((getCoeffType(r)==n_Zp) && (r->ch == p)); }

static FORCE_INLINE BOOLEAN nCoeff_is_Q(const coeffs r)
{
  assume(r != NULL);
  #if SI_INTEGER_VARIANT==1
  return getCoeffType(r)==n_Q && (r->is_field);
  #else
  return getCoeffType(r)==n_Q;
  #endif
}

static FORCE_INLINE BOOLEAN nCoeff_is_Z(const coeffs r)
{
  assume(r != NULL);
  #if SI_INTEGER_VARIANT==1
  return ((getCoeffType(r)==n_Q) && (!r->is_field));
  #else
  return getCoeffType(r)==n_Z;
  #endif
}

static FORCE_INLINE BOOLEAN nCoeff_is_Zn(const coeffs r)
{ assume(r != NULL); return getCoeffType(r)==n_Zn; }

static FORCE_INLINE BOOLEAN nCoeff_is_Q_or_BI(const coeffs r)
{ assume(r != NULL); return getCoeffType(r)==n_Q; }

static FORCE_INLINE BOOLEAN nCoeff_is_numeric(const coeffs r) /* R, long R, long C */
{ assume(r != NULL);  return (getCoeffType(r)==n_R) || (getCoeffType(r)==n_long_R) || (getCoeffType(r)==n_long_C); }
// (r->ringtype == 0) && (r->ch ==  -1); ??

static FORCE_INLINE BOOLEAN nCoeff_is_R(const coeffs r)
{ assume(r != NULL); return getCoeffType(r)==n_R; }

static FORCE_INLINE BOOLEAN nCoeff_is_GF(const coeffs r)
{ assume(r != NULL); return getCoeffType(r)==n_GF; }

static FORCE_INLINE BOOLEAN nCoeff_is_GF(const coeffs r, int q)
{ assume(r != NULL); return (getCoeffType(r)==n_GF) && (r->ch == q); }

/* TRUE iff r represents an algebraic or transcendental extension field */
static FORCE_INLINE BOOLEAN nCoeff_is_Extension(const coeffs r)
{
  assume(r != NULL);
  return (getCoeffType(r)==n_algExt) || (getCoeffType(r)==n_transExt);
}

/* DO NOT USE (only kept for compatibility reasons towards the SINGULAR
   svn trunk);
   intention: should be TRUE iff the given r is an extension field above
   some Z/pZ;
   actually: TRUE iff the given r is an extension tower of arbitrary
   height above some field of characteristic p (may be Z/pZ or some
   Galois field of characteristic p) */
static FORCE_INLINE BOOLEAN nCoeff_is_Zp_a(const coeffs r)
{
  assume(r != NULL);
  return ((!nCoeff_is_Ring(r)) && (n_GetChar(r) != 0) && nCoeff_is_Extension(r));
}

/* DO NOT USE (only kept for compatibility reasons towards the SINGULAR
   svn trunk);
   intention: should be TRUE iff the given r is an extension field above
   Z/pZ (with p as provided);
   actually: TRUE iff the given r is an extension tower of arbitrary
   height above some field of characteristic p (may be Z/pZ or some
   Galois field of characteristic p) */
static FORCE_INLINE BOOLEAN nCoeff_is_Zp_a(const coeffs r, int p)
{
  assume(r != NULL);
  assume(p != 0);
  return ((!nCoeff_is_Ring(r)) && (n_GetChar(r) == p) && nCoeff_is_Extension(r));
}

/* DO NOT USE (only kept for compatibility reasons towards the SINGULAR
   svn trunk);
   intention: should be TRUE iff the given r is an extension field
   above Q;
   actually: TRUE iff the given r is an extension tower of arbitrary
   height above some field of characteristic 0 (may be Q, R, or C) */
static FORCE_INLINE BOOLEAN nCoeff_is_Q_a(const coeffs r)
{
  assume(r != NULL);
  return ((n_GetChar(r) == 0) && nCoeff_is_Extension(r));
}

static FORCE_INLINE BOOLEAN nCoeff_is_long_R(const coeffs r)
{ assume(r != NULL); return getCoeffType(r)==n_long_R; }

static FORCE_INLINE BOOLEAN nCoeff_is_long_C(const coeffs r)
{ assume(r != NULL); return getCoeffType(r)==n_long_C; }

static FORCE_INLINE BOOLEAN nCoeff_is_CF(const coeffs r)
{ assume(r != NULL); return getCoeffType(r)==n_CF; }

/// TRUE, if the computation of the inverse is fast,
/// i.e. prefer leading coeff. 1 over content
static FORCE_INLINE BOOLEAN nCoeff_has_simple_inverse(const coeffs r)
{ assume(r != NULL); return r->has_simple_Inverse; }

/// TRUE if n_Delete is empty operation
static FORCE_INLINE BOOLEAN nCoeff_has_simple_Alloc(const coeffs r)
{ assume(r != NULL); return r->has_simple_Alloc; }

/// TRUE iff r represents an algebraic extension field
static FORCE_INLINE BOOLEAN nCoeff_is_algExt(const coeffs r)
{ assume(r != NULL); return (getCoeffType(r)==n_algExt); }

/// is it an alg. ext. of Q?
static FORCE_INLINE BOOLEAN nCoeff_is_Q_algExt(const coeffs r)
{ assume(r != NULL); return ((n_GetChar(r) == 0) && nCoeff_is_algExt(r)); }

/// TRUE iff r represents a transcendental extension field
static FORCE_INLINE BOOLEAN nCoeff_is_transExt(const coeffs r)
{ assume(r != NULL); return (getCoeffType(r)==n_transExt); }

/// is it an trans. ext. of Q?
static FORCE_INLINE BOOLEAN nCoeff_is_Q_transExt(const coeffs r)
{ assume(r != NULL); return ((n_GetChar(r) == 0) && nCoeff_is_transExt(r)); }

/// Computes the content and (inplace) divides it out on a collection
/// of numbers
/// number @em c is the content (i.e. the GCD of all the coeffs, which
/// we divide out inplace)
/// NOTE: it assumes all coefficient numbers to be integer!!!
/// NOTE/TODO: see also the description by Hans
/// TODO: rename into n_ClearIntegerContent
static FORCE_INLINE void n_ClearContent(ICoeffsEnumerator& numberCollectionEnumerator, number& c, const coeffs r)
{ assume(r != NULL); r->cfClearContent(numberCollectionEnumerator, c, r); }

/// (inplace) Clears denominators on a collection of numbers
/// number @em d is the LCM of all the coefficient denominators (i.e. the number
/// with which all the number coeffs. were multiplied)
/// NOTE/TODO: see also the description by Hans
static FORCE_INLINE void n_ClearDenominators(ICoeffsEnumerator& numberCollectionEnumerator, number& d, const coeffs r)
{ assume(r != NULL); r->cfClearDenominators(numberCollectionEnumerator, d, r); }

// convenience helpers (no number returned - but the input enumeration
// is to be changed
// TODO: do we need separate hooks for these as our existing code does
// *different things* there: compare p_Cleardenom (which calls
// *p_Content) and p_Cleardenom_n (which doesn't)!!!

static FORCE_INLINE void n_ClearContent(ICoeffsEnumerator& numberCollectionEnumerator, const coeffs r)
{ number c; n_ClearContent(numberCollectionEnumerator, c, r); n_Delete(&c, r); }

static FORCE_INLINE void n_ClearDenominators(ICoeffsEnumerator& numberCollectionEnumerator, const coeffs r)
{ assume(r != NULL); number d; n_ClearDenominators(numberCollectionEnumerator, d, r); n_Delete(&d, r); }


/// print a number (BEWARE of string buffers!)
/// mostly for debugging
void   n_Print(number& a,  const coeffs r);



/// TODO: make it a virtual method of coeffs, together with:
/// Decompose & Compose, rParameter & rPar
static FORCE_INLINE char * nCoeffString(const coeffs cf)
{ assume( cf != NULL ); return cf->cfCoeffString(cf); }


static FORCE_INLINE char * nCoeffName (const coeffs cf)
{ assume( cf != NULL ); return cf->cfCoeffName(cf); }

static FORCE_INLINE number n_Random(siRandProc p, number p1, number p2, const coeffs cf)
{ assume( cf != NULL ); assume( cf->cfRandom != NULL );  return cf->cfRandom(p, p1, p2, cf); }

/// io via ssi:
static FORCE_INLINE void n_WriteFd(number a, const ssiInfo *f, const coeffs r)
{ assume(r != NULL); assume(r->cfWriteFd != NULL); return r->cfWriteFd(a, f, r); }

/// io via ssi:
static FORCE_INLINE number n_ReadFd( const ssiInfo *f, const coeffs r)
{ assume(r != NULL); assume(r->cfReadFd != NULL); return r->cfReadFd(f, r); }


static FORCE_INLINE number n_convFactoryNSingN( const CanonicalForm n, const coeffs r)
{ assume(r != NULL); assume(r->convFactoryNSingN != NULL); return r->convFactoryNSingN(n, r); }

static FORCE_INLINE CanonicalForm n_convSingNFactoryN( number n, BOOLEAN setChar, const coeffs r )
{ assume(r != NULL); assume(r->convSingNFactoryN != NULL); return r->convSingNFactoryN(n, setChar, r); }


// TODO: remove the following functions...
// the following 2 inline functions are just convenience shortcuts for Frank's code:
static FORCE_INLINE void number2mpz(number n, coeffs c, mpz_t m){ n_MPZ(m, n, c); }
static FORCE_INLINE number mpz2number(mpz_t m, coeffs c){ return n_InitMPZ(m, c); }
#endif

