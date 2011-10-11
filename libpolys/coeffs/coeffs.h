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
  n_algExt,  /**< used for all algebraic extensions, i.e.,
                the top-most extension in an extension tower
                is algebraic */
  n_transExt,  /**< used for all transcendental extensions, i.e.,
                  the top-most extension in an extension tower
                  is transcendental */
  n_long_C,
  // only used if HAVE_RINGS is defined:
  n_Z,
  n_Zn,
  n_Zpn, // does no longer exist?
  n_Z2m,
  n_CF
};

extern unsigned short fftable[];

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
typedef struct  n_Procs_s  *coeffs;

typedef number (*numberfunc)(number a, number b, const coeffs r);

/// maps "a", which lives in src, into dst
typedef number (*nMapFunc)(number a, const coeffs src, const coeffs dst);

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

struct n_Procs_s
{
   coeffs next;
   unsigned int ringtype;  /* =0 => coefficient field,
                             !=0 => coeffs from one of the rings:
                              =1 => Z/2^mZ
                              =2 => Z/nZ, n not a prime
                              =3 => Z/p^mZ
                              =4 => Z */

   // general properties:
   /// TRUE, if nNew/nDelete/nCopy are dummies
   BOOLEAN has_simple_Alloc;
   /// TRUE, if std should make polynomials monic (if nInvers is cheap)
   /// if false, then a gcd routine is used for a content computation
   BOOLEAN has_simple_Inverse;

   // tests for numbers.cc:
   BOOLEAN (*nCoeffIsEqual)(const coeffs r, n_coeffType n, void * parameter);

   /// output of coeff description via Print
   void (*cfCoeffWrite)(const coeffs r, BOOLEAN details);

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

   /// init with a GMP integer
   number  (*cfInitMPZ)(mpz_t i, const coeffs r);
   
   /// how complicated, (0) => 0, or positive
   int     (*cfSize)(number n, const coeffs r);
   
   /// convertion to int, 0 if impossible
   int     (*cfInt)(number &n, const coeffs r);

   /// Converts a non-negative number n into a GMP number, 0 if impossible
   void     (*cfMPZ)(mpz_t result, number &n, const coeffs r);
   
#ifdef HAVE_RINGS
   int     (*cfDivComp)(number a,number b,const coeffs r);
   BOOLEAN (*cfIsUnit)(number a,const coeffs r);
   number  (*cfGetUnit)(number a,const coeffs r);
   number  (*cfExtGcd)(number a, number b, number *s, number *t,const coeffs r);
#endif

   /// changes argument  inline: a:= -a
   /// return -a! (no copy is returned)
   /// the result should be assigned to the original argument: e.g. a = n_Neg(a,r)
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
   number  (*cfChineseRemainder)(number *a, number *b, int rl, const coeffs r);
   number  (*cfFarey)(number a, number b, const coeffs r);
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

  /* for extension fields we need to be able to represent polynomials,
     so here is the polynomial ring: */
  ring          extRing;

  //number     minpoly;  //< no longer needed: replaced by
  //                     //< extRing->minideal->[0]


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

  BOOLEAN   CanShortOut; //< if the elements can be printed in short format
		       // this is set to FALSE if a parameter name has >2 chars
  BOOLEAN   ShortOut; //< if the elements should print in short format

// ---------------------------------------------------
  // for n_GF

  int m_nfCharQ;  ///< the number of elements: q
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
  assume(r->cfSetChar != NULL);
  r->cfSetChar(r);
}

void           nNew(number * a);
#define n_New(n, r)           nNew(n)


// the access methods (part 2):

/// return a copy of 'n'
static inline number n_Copy(number n,    const coeffs r)
{   assume(r != NULL); assume(r->cfCopy!=NULL); return r->cfCopy(n, r); }

/// delete 'p'
static inline void   n_Delete(number* p, const coeffs r)
{   assume(r != NULL); assume(r->cfDelete!= NULL); r->cfDelete(p, r); }

/// TRUE iff 'a' and 'b' represent the same number;
/// they may have different representations
static inline BOOLEAN n_Equal(number a, number b, const coeffs r)
{ assume(r != NULL); assume(r->cfEqual!=NULL); return r->cfEqual(a, b, r); }

/// TRUE iff 'n' represents the zero element
static inline BOOLEAN n_IsZero(number n, const coeffs r)
{ assume(r != NULL); assume(r->cfIsZero!=NULL); return r->cfIsZero(n,r); }

/// TRUE iff 'n' represents the one element
static inline BOOLEAN n_IsOne(number n,  const coeffs r)
{ assume(r != NULL); assume(r->cfIsOne!=NULL); return r->cfIsOne(n,r); }

/// TRUE iff 'n' represents the additive inverse of the one element, i.e. -1
static inline BOOLEAN n_IsMOne(number n, const coeffs r)
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
///
static inline BOOLEAN n_GreaterZero(number n, const coeffs r)
{
  assume(r != NULL); assume(r->cfGreaterZero!=NULL);
  return r->cfGreaterZero(n,r);
}

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
static inline BOOLEAN n_Greater(number a, number b, const coeffs r)
{ assume(r != NULL); assume(r->cfGreater!=NULL); return r->cfGreater(a,b,r); }

#ifdef HAVE_RINGS
/// TRUE iff n has a multiplicative inverse in the given coeff field/ring r
static inline BOOLEAN n_IsUnit(number n, const coeffs r)
{ assume(r != NULL); assume(r->cfIsUnit!=NULL); return r->cfIsUnit(n,r); }

static inline number n_ExtGcd(number a, number b, number *s, number *t, const coeffs r)
{ assume(r != NULL); assume(r->cfExtGcd!=NULL); return r->cfExtGcd (a,b,s,t,r); }

static inline int n_DivComp(number a, number b, const coeffs r)
{ assume(r != NULL); assume(r->cfDivComp!=NULL); return r->cfDivComp (a,b,r); }

/// in Z: 1
/// in Z/kZ (where k is not a prime): largest divisor of n (taken in Z) that
///                                   is co-prime with k
/// in Z/2^kZ: largest odd divisor of n (taken in Z)
/// other cases: not implemented
static inline number n_GetUnit(number n, const coeffs r)
{ assume(r != NULL); assume(r->cfGetUnit!=NULL); return r->cfGetUnit(n,r); }
#endif

/// a number representing i in the given coeff field/ring r
static inline number n_Init(int i,       const coeffs r)
{ assume(r != NULL); assume(r->cfInit!=NULL); return r->cfInit(i,r); }

/// conversion of a GMP integer to number
static inline number n_Init(mpz_t n,     const coeffs r)
{ assume(r != NULL); assume(r->cfInitMPZ != NULL); return r->cfInitMPZ(n,r); }

/// conversion of n to an int; 0 if not possible
/// in Z/pZ: the representing int lying in (-p/2 .. p/2]
static inline int n_Int(number &n,       const coeffs r)
{ assume(r != NULL); assume(r->cfInt!=NULL); return r->cfInt(n,r); }

/// conversion of n to a GMP integer; 0 if not possible
static inline void n_MPZ(mpz_t result, number &n,       const coeffs r)
{ assume(r != NULL); assume(r->cfMPZ!=NULL); r->cfMPZ(result, n, r); }


/// in-place negation of n
/// MUST BE USED: n = n_Neg(n) (no copy is returned)
static inline number n_Neg(number n,     const coeffs r)
{ assume(r != NULL); assume(r->cfNeg!=NULL); return r->cfNeg(n,r); }

/// return the multiplicative inverse of 'a';
/// raise an error if 'a' is not invertible
///
/// !!! Recommendation: rename to 'n_Inverse'
static inline number n_Invers(number a,  const coeffs r)
{ assume(r != NULL); assume(r->cfInvers!=NULL); return r->cfInvers(a,r); }

/// return a non-negative measure for the complexity of n;
/// return 0 only when n represents zero;
/// (used for pivot strategies in matrix computations with entries from r)
static inline int    n_Size(number n,    const coeffs r)
{ assume(r != NULL); assume(r->cfSize!=NULL); return r->cfSize(n,r); }

/// inplace-normalization of n;
/// produces some canonical representation of n;
///
/// !!! Recommendation: remove this method from the user-interface, i.e.,
/// !!!                 this should be hidden
static inline void   n_Normalize(number& n, const coeffs r)
{ assume(r != NULL); assume(r->cfNormalize!=NULL); r->cfNormalize(n,r); }

/// write to the output buffer of the currently used reporter
static inline void   n_Write(number& n,  const coeffs r)
{ assume(r != NULL); assume(r->cfWrite!=NULL); r->cfWrite(n,r); }

/// @todo: Describe me!!! --> Hans
///
/// !!! Recommendation: This method is to cryptic to be part of the user-
/// !!!                 interface. As defined here, it is merely a helper
/// !!!                 method for parsing number input strings.
static inline const char *n_Read(const char * s, number * a, const coeffs r)
{ assume(r != NULL); assume(r->cfRead!=NULL); return r->cfRead(s, a, r); }

/// return the denominator of n
/// (if elements of r are by nature not fractional, result is 1)
static inline number n_GetDenom(number& n, const coeffs r)
{ assume(r != NULL); assume(r->cfGetDenom!=NULL); return r->cfGetDenom(n, r); }

/// return the numerator of n
/// (if elements of r are by nature not fractional, result is n)
static inline number n_GetNumerator(number& n, const coeffs r)
{ assume(r != NULL); assume(r->cfGetNumerator!=NULL); return r->cfGetNumerator(n, r); }

/// fill res with the power a^b
static inline void   n_Power(number a, int b, number *res, const coeffs r)
{ assume(r != NULL); assume(r->cfPower!=NULL); r->cfPower(a,b,res,r); }

/// return the product of 'a' and 'b', i.e., a*b
static inline number n_Mult(number a, number b, const coeffs r)
{ assume(r != NULL); assume(r->cfMult!=NULL); return r->cfMult(a, b, r); }

/// multiplication of 'a' and 'b';
/// replacement of 'a' by the product a*b
static inline void n_InpMult(number &a, number b, const coeffs r)
{ assume(r != NULL); assume(r->cfInpMult!=NULL); r->cfInpMult(a,b,r); }

/// return the difference of 'a' and 'b', i.e., a-b
static inline number n_Sub(number a, number b, const coeffs r)
{ assume(r != NULL); assume(r->cfSub!=NULL); return r->cfSub(a, b, r); }

/// return the sum of 'a' and 'b', i.e., a+b
static inline number n_Add(number a, number b, const coeffs r)
{ assume(r != NULL); assume(r->cfAdd!=NULL); return r->cfAdd(a, b, r); }

/// return the quotient of 'a' and 'b', i.e., a/b;
/// raise an error if 'b' is not invertible in r
static inline number n_Div(number a, number b, const coeffs r)
{ assume(r != NULL); assume(r->cfDiv!=NULL); return r->cfDiv(a,b,r); }

/// in Z: largest c such that c*b <= a
/// in Z/nZ, Z/2^kZ: computed as in the case Z (from integers representing
///                  'a' and 'b')
/// in Z/pZ: return a/b
/// in K(a)/<p(a)>: return a/b
/// in K(t_1, ..., t_n): return a/b
/// other fields: not implemented
static inline number n_IntDiv(number a, number b, const coeffs r)
{ assume(r != NULL); assume(r->cfIntDiv!=NULL); return r->cfIntDiv(a,b,r); }

static inline number n_IntMod(number a, number b, const coeffs r)
{ assume(r != NULL); assume(r->cfIntMod!=NULL); return r->cfIntMod(a,b,r); }
/// @todo: Describe me!!!
///
/// What is the purpose of this method, especially in comparison with
/// n_Div?
/// !!! Recommendation: remove this method from the user-interface.
static inline number n_ExactDiv(number a, number b, const coeffs r)
{ assume(r != NULL); assume(r->cfExactDiv!=NULL); return r->cfExactDiv(a,b,r); }

/// in Z: return the gcd of 'a' and 'b'
/// in Z/nZ, Z/2^kZ: computed as in the case Z
/// in Z/pZ, C, R: not implemented
/// in Q: return the gcd of the numerators of 'a' and 'b'
/// in K(a)/<p(a)>: not implemented
/// in K(t_1, ..., t_n): not implemented
static inline number n_Gcd(number a, number b, const coeffs r)
{ assume(r != NULL); assume(r->cfGcd!=NULL); return r->cfGcd(a,b,r); }

/// in Z: return the lcm of 'a' and 'b'
/// in Z/nZ, Z/2^kZ: computed as in the case Z
/// in Z/pZ, C, R: not implemented
/// in Q: return the lcm of the numerators of 'a' and the denominator of 'b'
/// in K(a)/<p(a)>: not implemented
/// in K(t_1, ..., t_n): not implemented
static inline number n_Lcm(number a, number b, const coeffs r)
{ assume(r != NULL); assume(r->cfLcm!=NULL); return r->cfLcm(a,b,r); }

/// set the mapping function pointers for translating numbers from src to dst
static inline nMapFunc n_SetMap(const coeffs src, const coeffs dst)
{ assume(src != NULL && dst != NULL); assume(dst->cfSetMap!=NULL); return dst->cfSetMap(src,dst); }

/// test whether n is a correct number;
/// only used if LDEBUG is defined
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
static inline void   n_CoeffWrite(const coeffs r, BOOLEAN details = TRUE)
{ assume(r != NULL); assume(r->cfCoeffWrite != NULL); r->cfCoeffWrite(r, details); }

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

/// test whether 'a' is divisible 'b';
/// for r encoding a field: TRUE iff 'b' does not represent zero
/// in Z: TRUE iff 'b' divides 'a' (with remainder = zero)
/// in Z/nZ: TRUE iff (a = 0 and b divides n in Z) or
///                   (a != 0 and b/gcd(a, b) is co-prime with n, i.e.
///                                              a unit in Z/nZ)
/// in Z/2^kZ: TRUE iff ((a = 0 mod 2^k) and (b = 0 or b is a power of 2))
///                  or ((a, b <> 0) and (b/gcd(a, b) is odd))
static inline BOOLEAN n_DivBy(number a, number b, const coeffs r)
{
  assume(r != NULL);
#ifdef HAVE_RINGS
  if( nCoeff_is_Ring(r) )
  {
    assume(r->cfDivBy!=NULL); return r->cfDivBy(a,b,r);
  }
#endif
  return !n_IsZero(b, r);
}

static inline number n_ChineseRemainder(number *a, number *b, int rl, const coeffs r)
{
  assume(r != NULL);
  assume(getCoeffType(r)==n_Q);
  return r->cfChineseRemainder(a,b,rl,r);
}

static inline number n_Farey(number a, number b, const coeffs r)
{
  assume(r != NULL);
  assume(getCoeffType(r)==n_Q);
  return r->cfFarey(a,b,r);
}

static inline number  n_Init_bigint(number i, const coeffs dummy,
		const coeffs dst)
{
  assume(dummy != NULL && dst != NULL); assume(dst->cfInit_bigint!=NULL); 
  return dst->cfInit_bigint(i, dummy, dst);
}

static inline number  n_RePart(number i, const coeffs cf)
{
  assume(cf != NULL); assume(cf->cfRePart!=NULL); 
  return cf->cfRePart(i,cf);
}
static inline number  n_ImPart(number i, const coeffs cf)
{
  assume(cf != NULL); assume(cf->cfImPart!=NULL); 
  return cf->cfImPart(i,cf);
}

/// returns TRUE, if r is not a field and r has non-trivial units
static inline BOOLEAN nCoeff_has_Units(const coeffs r)
{ assume(r != NULL); return ((r->ringtype == 1) || (r->ringtype == 2) || (r->ringtype == 3)); }

static inline BOOLEAN nCoeff_is_Zp(const coeffs r)
{ assume(r != NULL); return getCoeffType(r)==n_Zp; }

static inline BOOLEAN nCoeff_is_Zp(const coeffs r, int p)
{ assume(r != NULL); return (getCoeffType(r)  && (r->ch == p)); }

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

/* TRUE iff r represents an algebraic or transcendental extension field */
static inline BOOLEAN nCoeff_is_Extension(const coeffs r)
{
  assume(r != NULL);
  return (getCoeffType(r)==n_algExt) || (getCoeffType(r)==n_transExt);
}

/* DO NOT USE (only kept for compatibility reasons towards the SINGULAR
   svn trunk);
   intension: should be TRUE iff the given r is an extension field above
   some Z/pZ;
   actually: TRUE iff the given r is an extension tower of arbitrary
   height above some field of characteristic p (may be Z/pZ or some
   Galois field of characteristic p) */
static inline BOOLEAN nCoeff_is_Zp_a(const coeffs r)
{
  assume(r != NULL);
  return ((r->ringtype == 0) && (r->ch != 0) && nCoeff_is_Extension(r));
}

/* DO NOT USE (only kept for compatibility reasons towards the SINGULAR
   svn trunk);
   intension: should be TRUE iff the given r is an extension field above
   Z/pZ (with p as provided);
   actually: TRUE iff the given r is an extension tower of arbitrary
   height above some field of characteristic p (may be Z/pZ or some
   Galois field of characteristic p) */
static inline BOOLEAN nCoeff_is_Zp_a(const coeffs r, int p)
{
  assume(r != NULL);
  return ((r->ringtype == 0) && (r->ch == p) && nCoeff_is_Extension(r));
}

/* DO NOT USE (only kept for compatibility reasons towards the SINGULAR
   svn trunk);
   intension: should be TRUE iff the given r is an extension field
   above Q;
   actually: TRUE iff the given r is an extension tower of arbitrary
   height above some field of characteristic 0 (may be Q, R, or C) */
static inline BOOLEAN nCoeff_is_Q_a(const coeffs r)
{
  assume(r != NULL);
  return ((r->ringtype == 0) && (r->ch == 0) && nCoeff_is_Extension(r));
}

static inline BOOLEAN nCoeff_is_long_R(const coeffs r)
{ assume(r != NULL); return getCoeffType(r)==n_long_R; }

static inline BOOLEAN nCoeff_is_long_C(const coeffs r)
{ assume(r != NULL); return getCoeffType(r)==n_long_C; }

static inline BOOLEAN nCoeff_is_CF(const coeffs r)
{ assume(r != NULL); return getCoeffType(r)==n_CF; }

/// TRUE, if the computation of the inverse is fast,
/// i.e. prefer leading coeff. 1 over content
static inline BOOLEAN nCoeff_has_simple_inverse(const coeffs r)
{ assume(r != NULL); return r->has_simple_Inverse; }

/// TRUE if n_Delete/n_New are empty operations
static inline BOOLEAN nCoeff_has_simple_Alloc(const coeffs r)
{ assume(r != NULL); return r->has_simple_Alloc; }

/// TRUE iff r represents an algebraic extension field
static inline BOOLEAN nCoeff_is_algExt(const coeffs r)
{ assume(r != NULL); return (getCoeffType(r)==n_algExt); }

/// TRUE iff r represents a transcendental extension field
static inline BOOLEAN nCoeff_is_transExt(const coeffs r)
{ assume(r != NULL); return (getCoeffType(r)==n_transExt); }

/// BOOLEAN n_Test(number a, const coeffs r)
#define n_Test(a,r)  n_DBTest(a, __FILE__, __LINE__, r)

// Missing wrappers for: (TODO: review this?)
// cfIntMod, cfRead, cfName, cfInit_bigint
// HAVE_RINGS: cfDivComp, cfExtGcd... 

// Deprecated:
static inline int n_GetChar(const coeffs r)
{ assume(r != NULL); return nInternalChar(r); }

#endif

