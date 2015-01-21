/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/*
* ABSTRACT: numbers modulo n
*/
#include <misc/auxiliary.h>
#include <omalloc/omalloc.h>

#include <misc/mylimits.h>
#include <reporter/reporter.h>

#include "si_gmp.h"
#include "coeffs.h"
#include "numbers.h"

#include "mpr_complex.h"

#include "longrat.h"
#include "rmodulon.h"

#include <string.h>

#ifdef HAVE_RINGS

/// Our Type!
static const n_coeffType ID = n_Zn;
static const n_coeffType ID2 = n_Znm;

number  nrnCopy        (number a, const coeffs r);
int     nrnSize        (number a, const coeffs r);
void    nrnDelete      (number *a, const coeffs r);
BOOLEAN nrnGreaterZero (number k, const coeffs r);
number  nrnMult        (number a, number b, const coeffs r);
number  nrnInit        (long i, const coeffs r);
long    nrnInt         (number &n, const coeffs r);
number  nrnAdd         (number a, number b, const coeffs r);
number  nrnSub         (number a, number b, const coeffs r);
void    nrnPower       (number a, int i, number * result, const coeffs r);
BOOLEAN nrnIsZero      (number a, const coeffs r);
BOOLEAN nrnIsOne       (number a, const coeffs r);
BOOLEAN nrnIsMOne      (number a, const coeffs r);
BOOLEAN nrnIsUnit      (number a, const coeffs r);
number  nrnGetUnit     (number a, const coeffs r);
number  nrnAnn         (number a, const coeffs r);
number  nrnDiv         (number a, number b, const coeffs r);
number  nrnMod         (number a,number b, const coeffs r);
number  nrnIntDiv      (number a,number b, const coeffs r);
number  nrnNeg         (number c, const coeffs r);
number  nrnInvers      (number c, const coeffs r);
BOOLEAN nrnGreater     (number a, number b, const coeffs r);
BOOLEAN nrnDivBy       (number a, number b, const coeffs r);
int     nrnDivComp     (number a, number b, const coeffs r);
BOOLEAN nrnEqual       (number a, number b, const coeffs r);
number  nrnLcm         (number a,number b, const coeffs r);
number  nrnGcd         (number a,number b, const coeffs r);
number  nrnExtGcd      (number a, number b, number *s, number *t, const coeffs r);
number  nrnXExtGcd      (number a, number b, number *s, number *t, number *u, number *v, const coeffs r);
number  nrnQuotRem      (number a, number b, number *s, const coeffs r);
nMapFunc nrnSetMap     (const coeffs src, const coeffs dst);
#if SI_INTEGER_VARIANT==2
// FIXME? TODO? // extern void    nrzWrite       (number &a, const coeffs r); // FIXME
# define  nrnWrite      nrzWrite
#else
void nrnWrite (number &a, const coeffs);
#endif
const char *  nrnRead  (const char *s, number *a, const coeffs r);
void     nrnCoeffWrite (const coeffs r, BOOLEAN details);
#ifdef LDEBUG
BOOLEAN nrnDBTest      (number a, const char *f, const int l, const coeffs r);
#endif
void    nrnSetExp(unsigned long c, const coeffs r);
void    nrnInitExp(unsigned long c, const coeffs r);
coeffs  nrnQuot1(number c, const coeffs r);

number nrnMapQ(number from, const coeffs src, const coeffs dst);


extern omBin gmp_nrz_bin;

void    nrnCoeffWrite  (const coeffs r, BOOLEAN /*details*/)
{
  long l = (long)mpz_sizeinbase(r->modBase, 10) + 2;
  char* s = (char*) omAlloc(l);
  s= mpz_get_str (s, 10, r->modBase);
  if (nCoeff_is_Ring_ModN(r)) Print("//   coeff. ring is : Z/%s\n", s);
  else if (nCoeff_is_Ring_PtoM(r)) Print("//   coeff. ring is : Z/%s^%lu\n", s, r->modExponent);
  omFreeSize((ADDRESS)s, l);
}

static BOOLEAN nrnCoeffsEqual(const coeffs r, n_coeffType n, void * parameter)
{
  /* test, if r is an instance of nInitCoeffs(n,parameter) */
  return (n==n_Zn) && (mpz_cmp_ui(r->modNumber,(long)parameter)==0);
}

static char* nrnCoeffString(const coeffs r)
{
  long l = (long)mpz_sizeinbase(r->modBase, 10) +2;
  char* b = (char*) omAlloc(l);
  b= mpz_get_str (b, 10, r->modBase);
  char* s = (char*) omAlloc(7+2+10+l);
  if (nCoeff_is_Ring_ModN(r)) sprintf(s,"integer,%s",b);
  else /*if (nCoeff_is_Ring_PtoM(r))*/ sprintf(s,"integer,%s^%lu",b,r->modExponent);
  omFreeSize(b,l);
  return s;
}

static void nrnKillChar(coeffs r)
{
  mpz_clear(r->modNumber);
  mpz_clear(r->modBase);
  omFreeBin((void *) r->modBase, gmp_nrz_bin);
  omFreeBin((void *) r->modNumber, gmp_nrz_bin);
}

coeffs nrnQuot1(number c, const coeffs r)
{
    coeffs rr;
    long ch = r->cfInt(c, r);
    mpz_t a,b;
    mpz_init_set(a, r->modNumber);
    mpz_init_set_ui(b, ch);
    mpz_ptr gcd;
    gcd = (mpz_ptr) omAlloc(sizeof(mpz_t));
    mpz_init(gcd);
    mpz_gcd(gcd, a,b);
    if(mpz_cmp_ui(gcd, 1) == 0)
        {
            WerrorS("constant in q-ideal is coprime to modulus in ground ring");
            WerrorS("Unable to create qring!");
            return NULL;
        }
    if(r->modExponent == 1)
    {
        ZnmInfo info;
        info.base = gcd;
        info.exp = (unsigned long) 1;
        rr = nInitChar(n_Zn, (void*)&info);
    }
    else
    {
        ZnmInfo info;
        info.base = r->modBase;
        int kNew = 1;
        mpz_t baseTokNew;
        mpz_init(baseTokNew);
        mpz_set(baseTokNew, r->modBase);
        while(mpz_cmp(gcd, baseTokNew) > 0)
        {
          kNew++;
          mpz_mul(baseTokNew, baseTokNew, r->modBase);
        }
        //printf("\nkNew = %i\n",kNew);
        info.exp = kNew;
        mpz_clear(baseTokNew);
        rr = nInitChar(n_Znm, (void*)&info);
    }
    return(rr);
}

/* for initializing function pointers */
BOOLEAN nrnInitChar (coeffs r, void* p)
{
  assume( (getCoeffType(r) == ID) || (getCoeffType (r) == ID2) );
  ZnmInfo * info= (ZnmInfo *) p;
  r->modBase= (mpz_ptr)nrnCopy((number)info->base, r); //this circumvents the problem
  //in bigintmat.cc where we cannot create a "legal" nrn that can be freed.
  //If we take a copy, we can do whatever we want.

  nrnInitExp (info->exp, r);

  /* next computation may yield wrong characteristic as r->modNumber
     is a GMP number */
  r->ch = mpz_get_ui(r->modNumber);

  r->is_field=FALSE;
  r->is_domain=FALSE;
  r->rep=n_rep_gmp;


  r->cfCoeffString = nrnCoeffString;

  r->cfInit        = nrnInit;
  r->cfDelete      = nrnDelete;
  r->cfCopy        = nrnCopy;
  r->cfSize        = nrnSize;
  r->cfInt         = nrnInt;
  r->cfAdd         = nrnAdd;
  r->cfSub         = nrnSub;
  r->cfMult        = nrnMult;
  r->cfDiv         = nrnDiv;
  r->cfAnn         = nrnAnn;
  r->cfIntMod      = nrnMod;
  r->cfExactDiv    = nrnDiv;
  r->cfInpNeg         = nrnNeg;
  r->cfInvers      = nrnInvers;
  r->cfDivBy       = nrnDivBy;
  r->cfDivComp     = nrnDivComp;
  r->cfGreater     = nrnGreater;
  r->cfEqual       = nrnEqual;
  r->cfIsZero      = nrnIsZero;
  r->cfIsOne       = nrnIsOne;
  r->cfIsMOne      = nrnIsMOne;
  r->cfGreaterZero = nrnGreaterZero;
  r->cfWriteLong   = nrnWrite;
  r->cfRead        = nrnRead;
  r->cfPower       = nrnPower;
  r->cfSetMap      = nrnSetMap;
  //r->cfNormalize   = ndNormalize;
  r->cfLcm         = nrnLcm;
  r->cfGcd         = nrnGcd;
  r->cfIsUnit      = nrnIsUnit;
  r->cfGetUnit     = nrnGetUnit;
  r->cfExtGcd      = nrnExtGcd;
  r->cfXExtGcd     = nrnXExtGcd;
  r->cfQuotRem     = nrnQuotRem;
  r->cfCoeffWrite  = nrnCoeffWrite;
  r->nCoeffIsEqual = nrnCoeffsEqual;
  r->cfKillChar    = nrnKillChar;
  r->cfQuot1       = nrnQuot1;
#ifdef LDEBUG
  r->cfDBTest      = nrnDBTest;
#endif
  return FALSE;
}

/*
 * create a number from int
 */
number nrnInit(long i, const coeffs r)
{
  mpz_ptr erg = (mpz_ptr) omAllocBin(gmp_nrz_bin);
  mpz_init_set_si(erg, i);
  mpz_mod(erg, erg, r->modNumber);
  return (number) erg;
}

void nrnDelete(number *a, const coeffs)
{
  if (*a == NULL) return;
  mpz_clear((mpz_ptr) *a);
  omFreeBin((void *) *a, gmp_nrz_bin);
  *a = NULL;
}

number nrnCopy(number a, const coeffs)
{
  mpz_ptr erg = (mpz_ptr) omAllocBin(gmp_nrz_bin);
  mpz_init_set(erg, (mpz_ptr) a);
  return (number) erg;
}

int nrnSize(number a, const coeffs)
{
  if (a == NULL) return 0;
  return sizeof(mpz_t);
}

/*
 * convert a number to int
 */
long nrnInt(number &n, const coeffs)
{
  return mpz_get_si((mpz_ptr) n);
}

/*
 * Multiply two numbers
 */
number nrnMult(number a, number b, const coeffs r)
{
  mpz_ptr erg = (mpz_ptr)omAllocBin(gmp_nrz_bin);
  mpz_init(erg);
  mpz_mul(erg, (mpz_ptr)a, (mpz_ptr) b);
  mpz_mod(erg, erg, r->modNumber);
  return (number) erg;
}

void nrnPower(number a, int i, number * result, const coeffs r)
{
  mpz_ptr erg = (mpz_ptr)omAllocBin(gmp_nrz_bin);
  mpz_init(erg);
  mpz_powm_ui(erg, (mpz_ptr)a, i, r->modNumber);
  *result = (number) erg;
}

number nrnAdd(number a, number b, const coeffs r)
{
  mpz_ptr erg = (mpz_ptr)omAllocBin(gmp_nrz_bin);
  mpz_init(erg);
  mpz_add(erg, (mpz_ptr)a, (mpz_ptr) b);
  mpz_mod(erg, erg, r->modNumber);
  return (number) erg;
}

number nrnSub(number a, number b, const coeffs r)
{
  mpz_ptr erg = (mpz_ptr)omAllocBin(gmp_nrz_bin);
  mpz_init(erg);
  mpz_sub(erg, (mpz_ptr)a, (mpz_ptr) b);
  mpz_mod(erg, erg, r->modNumber);
  return (number) erg;
}

number nrnNeg(number c, const coeffs r)
{
  if( !nrnIsZero(c, r) )
    // Attention: This method operates in-place.
    mpz_sub((mpz_ptr)c, r->modNumber, (mpz_ptr)c);
  return c;
}

number nrnInvers(number c, const coeffs r)
{
  mpz_ptr erg = (mpz_ptr)omAllocBin(gmp_nrz_bin);
  mpz_init(erg);
  mpz_invert(erg, (mpz_ptr)c, r->modNumber);
  return (number) erg;
}

/*
 * Give the smallest k, such that a * x = k = b * y has a solution
 * TODO: lcm(gcd,gcd) better than gcd(lcm) ?
 */
number nrnLcm(number a, number b, const coeffs r)
{
  number erg = nrnGcd(NULL, a, r);
  number tmp = nrnGcd(NULL, b, r);
  mpz_lcm((mpz_ptr)erg, (mpz_ptr)erg, (mpz_ptr)tmp);
  nrnDelete(&tmp, r);
  return (number)erg;
}

/*
 * Give the largest k, such that a = x * k, b = y * k has
 * a solution.
 */
number nrnGcd(number a, number b, const coeffs r)
{
  if ((a == NULL) && (b == NULL)) return nrnInit(0,r);
  mpz_ptr erg = (mpz_ptr)omAllocBin(gmp_nrz_bin);
  mpz_init_set(erg, r->modNumber);
  if (a != NULL) mpz_gcd(erg, erg, (mpz_ptr)a);
  if (b != NULL) mpz_gcd(erg, erg, (mpz_ptr)b);
  return (number)erg;
}

/* Not needed any more, but may have room for improvement
   number nrnGcd3(number a,number b, number c,ring r)
{
  mpz_ptr erg = (mpz_ptr) omAllocBin(gmp_nrz_bin);
  mpz_init(erg);
  if (a == NULL) a = (number)r->modNumber;
  if (b == NULL) b = (number)r->modNumber;
  if (c == NULL) c = (number)r->modNumber;
  mpz_gcd(erg, (mpz_ptr)a, (mpz_ptr)b);
  mpz_gcd(erg, erg, (mpz_ptr)c);
  mpz_gcd(erg, erg, r->modNumber);
  return (number)erg;
}
*/

/*
 * Give the largest k, such that a = x * k, b = y * k has
 * a solution and r, s, s.t. k = s*a + t*b
 * CF: careful: ExtGcd is wrong as implemented (or at least may not
 * give you what you want:
 * ExtGcd(5, 10 modulo 12):
 * the gcdext will return 5 = 1*5 + 0*10
 * however, mod 12, the gcd should be 1
 */
number nrnExtGcd(number a, number b, number *s, number *t, const coeffs r)
{
  mpz_ptr erg = (mpz_ptr)omAllocBin(gmp_nrz_bin);
  mpz_ptr bs  = (mpz_ptr)omAllocBin(gmp_nrz_bin);
  mpz_ptr bt  = (mpz_ptr)omAllocBin(gmp_nrz_bin);
  mpz_init(erg);
  mpz_init(bs);
  mpz_init(bt);
  mpz_gcdext(erg, bs, bt, (mpz_ptr)a, (mpz_ptr)b);
  mpz_mod(bs, bs, r->modNumber);
  mpz_mod(bt, bt, r->modNumber);
  *s = (number)bs;
  *t = (number)bt;
  return (number)erg;
}
/* XExtGcd  returns a unimodular matrix ((s,t)(u,v)) sth.
 * (a,b)^t ((st)(uv)) = (g,0)^t
 * Beware, the ExtGcd will not necessaairly do this.
 * Problem: if g = as+bt then (in Z/nZ) it follows NOT that
 *             1 = (a/g)s + (b/g) t
 * due to the zero divisors.
 */

//#define CF_DEB;
number nrnXExtGcd(number a, number b, number *s, number *t, number *u, number *v, const coeffs r)
{
  number xx;
#ifdef CF_DEB
  StringSetS("XExtGcd of ");
  nrnWrite(a, r);
  StringAppendS("\t");
  nrnWrite(b, r);
  StringAppendS(" modulo ");
  nrnWrite(xx = (number)r->modNumber, r);
  Print("%s\n", StringEndS());
#endif

  mpz_ptr one = (mpz_ptr)omAllocBin(gmp_nrz_bin);
  mpz_ptr erg = (mpz_ptr)omAllocBin(gmp_nrz_bin);
  mpz_ptr bs  = (mpz_ptr)omAllocBin(gmp_nrz_bin);
  mpz_ptr bt  = (mpz_ptr)omAllocBin(gmp_nrz_bin);
  mpz_ptr bu  = (mpz_ptr)omAllocBin(gmp_nrz_bin);
  mpz_ptr bv  = (mpz_ptr)omAllocBin(gmp_nrz_bin);
  mpz_init(erg);
  mpz_init(one);
  mpz_init_set(bs, (mpz_ptr) a);
  mpz_init_set(bt, (mpz_ptr) b);
  mpz_init(bu);
  mpz_init(bv);
  mpz_gcd(erg, bs, bt);

#ifdef CF_DEB
  StringSetS("1st gcd:");
  nrnWrite(xx= (number)erg, r);
#endif

  mpz_gcd(erg, erg, r->modNumber);

  mpz_div(bs, bs, erg);
  mpz_div(bt, bt, erg);

#ifdef CF_DEB
  Print("%s\n", StringEndS());
  StringSetS("xgcd: ");
#endif

  mpz_gcdext(one, bu, bv, bs, bt);
  number ui = nrnGetUnit(xx = (number) one, r);
#ifdef CF_DEB
  n_Write(xx, r);
  StringAppendS("\t");
  n_Write(ui, r);
  Print("%s\n", StringEndS());
#endif
  nrnDelete(&xx, r);
  if (!nrnIsOne(ui, r)) {
#ifdef CF_DEB
    Print("Scaling\n");
#endif
    number uii = nrnInvers(ui, r);
    nrnDelete(&ui, r);
    ui = uii;
    mpz_ptr uu = (mpz_ptr)omAllocBin(gmp_nrz_bin);
    mpz_init_set(uu, (mpz_ptr)ui);
    mpz_mul(bu, bu, uu);
    mpz_mul(bv, bv, uu);
    mpz_clear(uu);
    omFreeBin(uu, gmp_nrz_bin);
  }
  nrnDelete(&ui, r);
#ifdef CF_DEB
  StringSetS("xgcd");
  nrnWrite(xx= (number)bs, r);
  StringAppendS("*");
  nrnWrite(xx= (number)bu, r);
  StringAppendS(" + ");
  nrnWrite(xx= (number)bt, r);
  StringAppendS("*");
  nrnWrite(xx= (number)bv, r);
  Print("%s\n", StringEndS());
#endif

  mpz_mod(bs, bs, r->modNumber);
  mpz_mod(bt, bt, r->modNumber);
  mpz_mod(bu, bu, r->modNumber);
  mpz_mod(bv, bv, r->modNumber);
  *s = (number)bu;
  *t = (number)bv;
  *u = (number)bt;
  *u = nrnNeg(*u, r);
  *v = (number)bs;
  return (number)erg;
}


BOOLEAN nrnIsZero(number a, const coeffs)
{
#ifdef LDEBUG
  if (a == NULL) return FALSE;
#endif
  return 0 == mpz_cmpabs_ui((mpz_ptr)a, 0);
}

BOOLEAN nrnIsOne(number a, const coeffs)
{
#ifdef LDEBUG
  if (a == NULL) return FALSE;
#endif
  return 0 == mpz_cmp_si((mpz_ptr)a, 1);
}

BOOLEAN nrnIsMOne(number a, const coeffs r)
{
#ifdef LDEBUG
  if (a == NULL) return FALSE;
#endif
  mpz_t t; mpz_init_set(t, (mpz_ptr)a);
  mpz_add_ui(t, t, 1);
  bool erg = (0 == mpz_cmp(t, r->modNumber));
  mpz_clear(t);
  return erg;
}

BOOLEAN nrnEqual(number a, number b, const coeffs)
{
  return 0 == mpz_cmp((mpz_ptr)a, (mpz_ptr)b);
}

BOOLEAN nrnGreater(number a, number b, const coeffs)
{
  return 0 < mpz_cmp((mpz_ptr)a, (mpz_ptr)b);
}

BOOLEAN nrnGreaterZero(number k, const coeffs)
{
  return 0 < mpz_cmp_si((mpz_ptr)k, 0);
}

BOOLEAN nrnIsUnit(number a, const coeffs r)
{
  number tmp = nrnGcd(a, (number)r->modNumber, r);
  bool res = nrnIsOne(tmp, r);
  nrnDelete(&tmp, NULL);
  return res;
}

number nrnGetUnit(number k, const coeffs r)
{
  if (mpz_divisible_p(r->modNumber, (mpz_ptr)k)) return nrnInit(1,r);

  mpz_ptr unit = (mpz_ptr)nrnGcd(k, 0, r);
  mpz_tdiv_q(unit, (mpz_ptr)k, unit);
  mpz_ptr gcd = (mpz_ptr)nrnGcd((number)unit, 0, r);
  if (!nrnIsOne((number)gcd,r))
  {
    mpz_ptr ctmp;
    // tmp := unit^2
    mpz_ptr tmp = (mpz_ptr) nrnMult((number) unit,(number) unit,r);
    // gcd_new := gcd(tmp, 0)
    mpz_ptr gcd_new = (mpz_ptr) nrnGcd((number) tmp, 0, r);
    while (!nrnEqual((number) gcd_new,(number) gcd,r))
    {
      // gcd := gcd_new
      ctmp = gcd;
      gcd = gcd_new;
      gcd_new = ctmp;
      // tmp := tmp * unit
      mpz_mul(tmp, tmp, unit);
      mpz_mod(tmp, tmp, r->modNumber);
      // gcd_new := gcd(tmp, 0)
      mpz_gcd(gcd_new, tmp, r->modNumber);
    }
    // unit := unit + modNumber / gcd_new
    mpz_tdiv_q(tmp, r->modNumber, gcd_new);
    mpz_add(unit, unit, tmp);
    mpz_mod(unit, unit, r->modNumber);
    nrnDelete((number*) &gcd_new, NULL);
    nrnDelete((number*) &tmp, NULL);
  }
  nrnDelete((number*) &gcd, NULL);
  return (number)unit;
}

number nrnAnn(number k, const coeffs r)
{
  mpz_ptr tmp = (mpz_ptr) omAllocBin(gmp_nrz_bin);
  mpz_init(tmp);
  mpz_gcd(tmp, (mpz_ptr) k, r->modNumber);
  if (mpz_cmp_si(tmp, 1)==0) {
    mpz_set_si(tmp, 0);
    return (number) tmp;
  }
  mpz_divexact(tmp, r->modNumber, tmp);
  return (number) tmp;
}

BOOLEAN nrnDivBy(number a, number b, const coeffs r)
{
  if (a == NULL)
    return mpz_divisible_p(r->modNumber, (mpz_ptr)b);
  else
  { /* b divides a iff b/gcd(a, b) is a unit in the given ring: */
    number n = nrnGcd(a, b, r);
    mpz_tdiv_q((mpz_ptr)n, (mpz_ptr)b, (mpz_ptr)n);
    bool result = nrnIsUnit(n, r);
    nrnDelete(&n, NULL);
    return result;
  }
}

int nrnDivComp(number a, number b, const coeffs r)
{
  if (nrnEqual(a, b,r)) return 2;
  if (mpz_divisible_p((mpz_ptr) a, (mpz_ptr) b)) return -1;
  if (mpz_divisible_p((mpz_ptr) b, (mpz_ptr) a)) return 1;
  return 0;
}

number nrnDiv(number a, number b, const coeffs r)
{
  if (a == NULL) a = (number)r->modNumber;
  mpz_ptr erg = (mpz_ptr)omAllocBin(gmp_nrz_bin);
  mpz_init(erg);
  if (mpz_divisible_p((mpz_ptr)a, (mpz_ptr)b))
  {
    mpz_divexact(erg, (mpz_ptr)a, (mpz_ptr)b);
    return (number)erg;
  }
  else
  {
    mpz_ptr gcd = (mpz_ptr)nrnGcd(a, b, r);
    mpz_divexact(erg, (mpz_ptr)b, gcd);
    if (!nrnIsUnit((number)erg, r))
    {
      WerrorS("Division not possible, even by cancelling zero divisors.");
      WerrorS("Result is integer division without remainder.");
      mpz_tdiv_q(erg, (mpz_ptr) a, (mpz_ptr) b);
      nrnDelete((number*) &gcd, NULL);
      return (number)erg;
    }
    // a / gcd(a,b) * [b / gcd (a,b)]^(-1)
    mpz_ptr tmp = (mpz_ptr)nrnInvers((number) erg,r);
    mpz_divexact(erg, (mpz_ptr)a, gcd);
    mpz_mul(erg, erg, tmp);
    nrnDelete((number*) &gcd, NULL);
    nrnDelete((number*) &tmp, NULL);
    mpz_mod(erg, erg, r->modNumber);
    return (number)erg;
  }
}

number nrnMod(number a, number b, const coeffs r)
{
  /*
    We need to return the number rr which is uniquely determined by the
    following two properties:
      (1) 0 <= rr < |b| (with respect to '<' and '<=' performed in Z x Z)
      (2) There exists some k in the integers Z such that a = k * b + rr.
    Consider g := gcd(n, |b|). Note that then |b|/g is a unit in Z/n.
    Now, there are three cases:
      (a) g = 1
          Then |b| is a unit in Z/n, i.e. |b| (and also b) divides a.
          Thus rr = 0.
      (b) g <> 1 and g divides a
          Then a = (a/g) * (|b|/g)^(-1) * b (up to sign), i.e. again rr = 0.
      (c) g <> 1 and g does not divide a
          Then denote the division with remainder of a by g as this:
          a = s * g + t. Then t = a - s * g = a - s * (|b|/g)^(-1) * |b|
          fulfills (1) and (2), i.e. rr := t is the correct result. Hence
          in this third case, rr is the remainder of division of a by g in Z.
     Remark: according to mpz_mod: a,b are always non-negative
  */
  mpz_ptr g = (mpz_ptr)omAllocBin(gmp_nrz_bin);
  mpz_ptr rr = (mpz_ptr)omAllocBin(gmp_nrz_bin);
  mpz_init(g);
  mpz_init_set_si(rr, 0);
  mpz_gcd(g, (mpz_ptr)r->modNumber, (mpz_ptr)b); // g is now as above
  if (mpz_cmp_si(g, (long)1) != 0) mpz_mod(rr, (mpz_ptr)a, g); // the case g <> 1
  mpz_clear(g);
  omFreeBin(g, gmp_nrz_bin);
  return (number)rr;
}

number nrnIntDiv(number a, number b, const coeffs r)
{
  mpz_ptr erg = (mpz_ptr)omAllocBin(gmp_nrz_bin);
  mpz_init(erg);
  if (a == NULL) a = (number)r->modNumber;
  mpz_tdiv_q(erg, (mpz_ptr)a, (mpz_ptr)b);
  return (number)erg;
}

/* CF: note that Z/nZ has (at least) two distinct euclidean structures
 * 1st phi(a) := (a mod n) which is just the structure directly
 *     inherited from Z
 * 2nd phi(a) := gcd(a, n)
 * The 1st version is probably faster as everything just comes from Z,
 * but the 2nd version behaves nicely wrt. to quotient operations
 * and HNF and such. In agreement with nrnMod we imlement the 2nd here
 *
 * For quotrem note that if b exactly divides a, then
 *   min(v_p(a), v_p(n))  >= min(v_p(b), v_p(n))
 * so if we divide  a and b by g:= gcd(a,b,n), then   b becomes a
 * unit mod n/g.
 * Thus we 1st compute the remainder (similar to nrnMod) and then
 * the exact quotient.
 */
number nrnQuotRem(number a, number b, number  * rem, const coeffs r)
{
  mpz_t g, aa, bb;
  mpz_ptr qq = (mpz_ptr)omAllocBin(gmp_nrz_bin);
  mpz_ptr rr = (mpz_ptr)omAllocBin(gmp_nrz_bin);
  mpz_init(qq);
  mpz_init(rr);
  mpz_init(g);
  mpz_init_set(aa, (mpz_ptr)a);
  mpz_init_set(bb, (mpz_ptr)b);

  mpz_gcd(g, bb, r->modNumber);
  mpz_mod(rr, aa, g);
  mpz_sub(aa, aa, rr);
  mpz_gcd(g, aa, g);
  mpz_div(aa, aa, g);
  mpz_div(bb, bb, g);
  mpz_div(g, r->modNumber, g);
  mpz_invert(g, bb, g);
  mpz_mul(qq, aa, g);
  if (rem)
    *rem = (number)rr;
  else {
    mpz_clear(rr);
    omFreeBin(rr, gmp_nrz_bin);
  }
  mpz_clear(g);
  mpz_clear(aa);
  mpz_clear(bb);
  return (number) qq;
}

/*
 * Helper function for computing the module
 */

mpz_ptr nrnMapCoef = NULL;

number nrnMapModN(number from, const coeffs /*src*/, const coeffs dst)
{
  return nrnMult(from, (number) nrnMapCoef, dst);
}

number nrnMap2toM(number from, const coeffs /*src*/, const coeffs dst)
{
  mpz_ptr erg = (mpz_ptr)omAllocBin(gmp_nrz_bin);
  mpz_init(erg);
  mpz_mul_ui(erg, nrnMapCoef, (unsigned long)from);
  mpz_mod(erg, erg, dst->modNumber);
  return (number)erg;
}

number nrnMapZp(number from, const coeffs /*src*/, const coeffs dst)
{
  mpz_ptr erg = (mpz_ptr)omAllocBin(gmp_nrz_bin);
  mpz_init(erg);
  // TODO: use npInt(...)
  mpz_mul_si(erg, nrnMapCoef, (unsigned long)from);
  mpz_mod(erg, erg, dst->modNumber);
  return (number)erg;
}

number nrnMapGMP(number from, const coeffs /*src*/, const coeffs dst)
{
  mpz_ptr erg = (mpz_ptr)omAllocBin(gmp_nrz_bin);
  mpz_init(erg);
  mpz_mod(erg, (mpz_ptr)from, dst->modNumber);
  return (number)erg;
}

#if SI_INTEGER_VARIANT==3
number nrnMapZ(number from, const coeffs /*src*/, const coeffs dst)
{
  mpz_ptr erg = (mpz_ptr)omAllocBin(gmp_nrz_bin);
  if (n_Z_IS_SMALL(from))
    mpz_init_set_si(erg, SR_TO_INT(from));
  else
    mpz_init_set(erg, (mpz_ptr) from);
  mpz_mod(erg, erg, dst->modNumber);
  return (number)erg;
}
#elif SI_INTEGER_VARIANT==2

number nrnMapZ(number from, const coeffs src, const coeffs dst)
{
  if (SR_HDL(from) & SR_INT)
  {
    long f_i=SR_TO_INT(from);
    return nrnInit(f_i,dst);
  }
  return nrnMapGMP(from,src,dst);
}
#elif SI_INTEGER_VARIANT==1
number nrnMapZ(number from, const coeffs src, const coeffs dst)
{
  return nrnMapQ(from,src,dst);
}
#endif
#if SI_INTEGER_VARIANT!=2
void nrnWrite (number &a, const coeffs)
{
  char *s,*z;
  if (a==NULL)
  {
    StringAppendS("o");
  }
  else
  {
    int l=mpz_sizeinbase((mpz_ptr) a, 10) + 2;
    s=(char*)omAlloc(l);
    z=mpz_get_str(s,10,(mpz_ptr) a);
    StringAppendS(z);
    omFreeSize((ADDRESS)s,l);
  }
}
#endif

number nrnMapQ(number from, const coeffs src, const coeffs dst)
{
  mpz_ptr erg = (mpz_ptr)omAllocBin(gmp_nrz_bin);
  mpz_init(erg);
  nlGMP(from, (number)erg, src); // FIXME? TODO? // extern void   nlGMP(number &i, number n, const coeffs r); // to be replaced with n_MPZ(erg, from, src); // ?
  mpz_mod(erg, erg, dst->modNumber);
  return (number)erg;
}

nMapFunc nrnSetMap(const coeffs src, const coeffs dst)
{
  /* dst = nrn */
  if ((src->rep==n_rep_gmp) && nCoeff_is_Ring_Z(src))
  {
    return nrnMapZ;
  }
  if ((src->rep==n_rep_gap_gmp) /*&& nCoeff_is_Ring_Z(src)*/)
  {
    return nrnMapZ;
  }
  if (src->rep==n_rep_gap_rat) /*&& nCoeff_is_Q(src)) or Z*/
  {
    return nrnMapQ;
  }
  // Some type of Z/n ring / field
  if (nCoeff_is_Ring_ModN(src) || nCoeff_is_Ring_PtoM(src) ||
      nCoeff_is_Ring_2toM(src) || nCoeff_is_Zp(src))
  {
    if (   (!nCoeff_is_Zp(src))
        && (mpz_cmp(src->modBase, dst->modBase) == 0)
        && (src->modExponent == dst->modExponent)) return nrnMapGMP;
    else
    {
      mpz_ptr nrnMapModul = (mpz_ptr) omAllocBin(gmp_nrz_bin);
      // Computing the n of Z/n
      if (nCoeff_is_Zp(src))
      {
        mpz_init_set_si(nrnMapModul, src->ch);
      }
      else
      {
        mpz_init(nrnMapModul);
        mpz_set(nrnMapModul, src->modNumber);
      }
      // nrnMapCoef = 1 in dst       if dst is a subring of src
      // nrnMapCoef = 0 in dst / src if src is a subring of dst
      if (nrnMapCoef == NULL)
      {
        nrnMapCoef = (mpz_ptr) omAllocBin(gmp_nrz_bin);
        mpz_init(nrnMapCoef);
      }
      if (mpz_divisible_p(nrnMapModul, dst->modNumber))
      {
        mpz_set_si(nrnMapCoef, 1);
      }
      else
      if (nrnDivBy(NULL, (number) nrnMapModul,dst))
      {
        mpz_divexact(nrnMapCoef, dst->modNumber, nrnMapModul);
        mpz_ptr tmp = dst->modNumber;
        dst->modNumber = nrnMapModul;
        if (!nrnIsUnit((number) nrnMapCoef,dst))
        {
          dst->modNumber = tmp;
          nrnDelete((number*) &nrnMapModul, dst);
          return NULL;
        }
        mpz_ptr inv = (mpz_ptr) nrnInvers((number) nrnMapCoef,dst);
        dst->modNumber = tmp;
        mpz_mul(nrnMapCoef, nrnMapCoef, inv);
        mpz_mod(nrnMapCoef, nrnMapCoef, dst->modNumber);
        nrnDelete((number*) &inv, dst);
      }
      else
      {
        nrnDelete((number*) &nrnMapModul, dst);
        return NULL;
      }
      nrnDelete((number*) &nrnMapModul, dst);
      if (nCoeff_is_Ring_2toM(src))
        return nrnMap2toM;
      else if (nCoeff_is_Zp(src))
        return nrnMapZp;
      else
        return nrnMapModN;
    }
  }
  return NULL;      // default
}

/*
 * set the exponent (allocate and init tables) (TODO)
 */

void nrnSetExp(unsigned long m, coeffs r)
{
  /* clean up former stuff */
  if (r->modNumber != NULL) mpz_clear(r->modNumber);

  r->modExponent= m;
  r->modNumber = (mpz_ptr)omAllocBin(gmp_nrz_bin);
  mpz_init_set (r->modNumber, r->modBase);
  mpz_pow_ui (r->modNumber, r->modNumber, m);
}

/* We expect this ring to be Z/n^m for some m > 0 and for some n > 2 which is not a prime. */
void nrnInitExp(unsigned long m, coeffs r)
{
  nrnSetExp(m, r);
  assume (r->modNumber != NULL);
//CF: in geenral, the modulus is computed somewhere. I don't want to
//  check it's size before I construct the best ring.
//  if (mpz_cmp_ui(r->modNumber,2) <= 0)
//    WarnS("nrnInitExp failed (m in Z/m too small)");
}

#ifdef LDEBUG
BOOLEAN nrnDBTest (number a, const char *, const int, const coeffs r)
{
  if (a==NULL) return TRUE;
  if ( (mpz_cmp_si((mpz_ptr) a, 0) < 0) || (mpz_cmp((mpz_ptr) a, r->modNumber) > 0) )
  {
    return FALSE;
  }
  return TRUE;
}
#endif

/*2
* extracts a long integer from s, returns the rest    (COPY FROM longrat0.cc)
*/
static const char * nlCPEatLongC(char *s, mpz_ptr i)
{
  const char * start=s;
  if (!(*s >= '0' && *s <= '9'))
  {
    mpz_init_set_si(i, 1);
    return s;
  }
  mpz_init(i);
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

const char * nrnRead (const char *s, number *a, const coeffs r)
{
  mpz_ptr z = (mpz_ptr) omAllocBin(gmp_nrz_bin);
  {
    s = nlCPEatLongC((char *)s, z);
  }
  mpz_mod(z, z, r->modNumber);
  *a = (number) z;
  return s;
}
#endif
/* #ifdef HAVE_RINGS */
