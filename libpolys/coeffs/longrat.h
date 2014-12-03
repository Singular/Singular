#ifndef LONGRAT_H
#define LONGRAT_H
/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/*
* ABSTRACT: computation with long rational numbers
*/
#include <misc/auxiliary.h>

#include <coeffs/si_gmp.h>
#include <coeffs/coeffs.h>

struct snumber; typedef struct snumber  *number;

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
#define INT_TO_SR(INT)  ((number) (((long)INT << 2) + SR_INT))
#define SR_TO_INT(SR)   (((long)SR) >> 2)

#define MP_SMALL 1

BOOLEAN nlInitChar(coeffs, void*);

/// only used by slimgb (tgb.cc)
static inline int nlQlogSize (number n, const coeffs r)
{
  assume( nCoeff_is_Q (r) );

  long nl=n_Size(n,r);
  if (nl==0L) return 0;
  if (nl==1L)
  {
    long i = SR_TO_INT (n);
    unsigned long v;
    v = (i >= 0) ? i : -i;
    int r = 0;

    while(v >>= 1)
    {
      r++;
    }
    return r + 1;
  }
  //assume denominator is 0
  number nn=(number) n;
  return mpz_sizeinbase (nn->z, 2);
}


// number nlModP(number q, const coeffs Q, const coeffs Zp);
// void   nlNormalize(number &x, const coeffs r);
// void   nlInpGcd(number &a, number b, const coeffs r);
// void   nlDelete(number *a, const coeffs r);

#  ifdef HAVE_RINGS
// void   nlGMP(number &i, number n, const coeffs r); // to be replaced with n_MPZ(number n, number &i,const coeffs r)???
// number nlMapGMP(number from, const coeffs src, const coeffs dst);
#  endif

#endif


