#ifndef LONGRAT_H
#define LONGRAT_H
/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/*
* ABSTRACT: computation with long rational numbers
*/
#include "misc/auxiliary.h"

#include "coeffs/si_gmp.h"
#include "coeffs/coeffs.h"
#include "factory/si_log2.h"

number   nlGetDenom(number &n, const coeffs r); /*for SAGE,, better: n_GetDenom */
number   nlGetNumerator(number &n, const coeffs r); /*for SAGE, better: n_GetNumerator*/

/*-----------------------------------------------------------------*/
/**
**  'SR_INT' is the type of those integers small enough to fit into  29  bits.
**  Therefor the value range of this small integers is: $-2^{28}...2^{28}-1$.
**
**  Small integers are represented by an immediate integer handle, containing
**  the value instead of pointing  to  it,  which  has  the  following  form:
**
**      +-------+-------+-------+-------+- - - -+-------+-------+-------+
**      | guard | sign  | bit   | bit   |       | bit   | tag   | tag   |
**      | bit   | bit   | 27    | 26    |       | 0     | 0     | 1     |
**      +-------+-------+-------+-------+- - - -+-------+-------+-------+
**
**  Immediate integers handles carry the tag 'SR_INT', i.e. the last bit is 1.
**  This distuingishes immediate integers from other handles which  point  to
**  structures aligned on 4 byte boundaries and therefor have last bit  zero.
**  (The second bit is reserved as tag to allow extensions of  this  scheme.)
**  Using immediates as pointers and dereferencing them gives address errors.
**
**  To aid overflow check the most significant two bits must always be equal,
**  that is to say that the sign bit of immediate integers has a  guard  bit.
**
**  The macros 'INT_TO_SR' and 'SR_TO_INT' should be used to convert  between
**  a small integer value and its representation as immediate integer handle.
**
**  Large integers and rationals are represented by z and n
**  where n may be undefined (if s==3)
**  NULL represents only deleted values
*/

struct snumber
{
  mpz_t z; //< Zaehler
  mpz_t n; //< Nenner
#if defined(LDEBUG)
  int debug;
#endif

  /**
   * parameter s in number:
   * 0 (or FALSE): not normalised rational
   * 1 (or TRUE):  normalised rational
   * 3          :  integer with n==NULL
   **/
  BOOLEAN s; //< integer parameter
};

#define SR_HDL(A) ((long)(A))

#define SR_INT    1L
#define INT_TO_SR(INT)  ((number) (((unsigned long)INT << 2) + SR_INT))
#define SR_TO_INT(SR)   (((long)SR) >> 2)

#define MP_SMALL 1

BOOLEAN nlInitChar(coeffs, void*);

/// only used by slimgb (tgb.cc)
static  FORCE_INLINE int nlQlogSize (number n, const coeffs r)
{
  assume( nCoeff_is_Q (r) );

  if(SR_HDL(n)&SR_INT)
  {
    if (SR_HDL(n)==SR_INT) return 0;
    long i = SR_TO_INT (n);
    unsigned long v;
    v = ABS(i);
    return SI_LOG2_LONG(v) + 1;
  }
  //assume denominator is 0
  number nn=(number) n;
  return mpz_sizeinbase (nn->z, 2);
}


static FORCE_INLINE BOOLEAN nlIsInteger(number q, const coeffs r)
{
  assume( nCoeff_is_Q (r) );
  n_Test(q, r);

  if (SR_HDL(q) & SR_INT)
    return TRUE; // immediate int

  return ( q->s == 3 );
}

void nlMPZ(mpz_t m, number &n, const coeffs r);
number nlModP(number q, const coeffs Q, const coeffs Zp);
void   nlNormalize(number &x, const coeffs r);
void   nlInpGcd(number &a, number b, const coeffs r);
void   nlDelete(number *a, const coeffs r);  /*for SAGE,, better: n_Delete */



/// create a rational i/j (implicitly) over Q
/// NOTE: make sure to use correct Q in debug mode
number   nlInit2 (int i, int j, const coeffs r);

/// create a rational i/j (implicitly) over Q
/// NOTE: make sure to use correct Q in debug mode
number   nlInit2gmp (mpz_t i, mpz_t j, const coeffs r);

// FIXME: TODO:  why only if HAVE_RINGS? bug?
#  ifdef HAVE_RINGS
void   nlGMP(number &i, mpz_t n, const coeffs r); // to be replaced with n_MPZ(number n, number &i,const coeffs r)???
number nlMapGMP(number from, const coeffs src, const coeffs dst);
#  endif
// for ring similiar to Q/Z (char 0 required):
number   nlChineseRemainderSym(number *x, number *q,int rl, BOOLEAN sym, CFArray &inv_cache,const coeffs CF);


#endif


