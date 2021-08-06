/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/**
  * ABSTRACT: numbers in an algebraic extension field K[a] / < f(a) >
  *           Assuming that we have a coeffs object cf, then these numbers
  *           are polynomials in the polynomial ring K[a] represented by
  *           cf->extRing.
  *           IMPORTANT ASSUMPTIONS:
  *           1.) So far we assume that cf->extRing is a valid polynomial
  *               ring in exactly one variable, i.e., K[a], where K is allowed
  0*               to be any field (representable in SINGULAR and which may
  *               itself be some extension field, thus allowing for extension
  *               towers).
  *           2.) Moreover, this implementation assumes that
  *               cf->extRing->qideal is not NULL but an ideal with at
  *               least one non-zero generator which may be accessed by
  *               cf->extRing->qideal->m[0] and which represents the minimal
  *               polynomial f(a) of the extension variable 'a' in K[a].
  *           3.) As soon as an std method for polynomial rings becomes
  *               availabe, all reduction steps modulo f(a) should be replaced
  *               by a call to std. Moreover, in this situation one can finally
  *               move from K[a] / < f(a) > to
  *                  K[a_1, ..., a_s] / I, with I some zero-dimensional ideal
  *                                        in K[a_1, ..., a_s] given by a lex
  *                                        Gröbner basis.
  *               The code in algext.h and algext.cc is then capable of
  *               computing in K[a_1, ..., a_s] / I.
  **/

#include "misc/auxiliary.h"

#include "reporter/reporter.h"

#include "coeffs/coeffs.h"
#include "coeffs/numbers.h"
#include "coeffs/longrat.h"

#include "polys/monomials/ring.h"
#include "polys/monomials/p_polys.h"
#include "polys/simpleideals.h"
#include "polys/PolyEnumerator.h"

#include "factory/factory.h"
#include "polys/clapconv.h"
#include "polys/clapsing.h"
#include "polys/prCopy.h"

#include "polys/ext_fields/algext.h"
#define TRANSEXT_PRIVATES 1
#include "polys/ext_fields/transext.h"

#ifdef LDEBUG
#define naTest(a) naDBTest(a,__FILE__,__LINE__,cf)
BOOLEAN  naDBTest(number a, const char *f, const int l, const coeffs r);
#else
#define naTest(a) do {} while (0)
#endif

/* polynomial ring in which our numbers live */
#define naRing cf->extRing

/* coeffs object in which the coefficients of our numbers live;
 * methods attached to naCoeffs may be used to compute with the
 * coefficients of our numbers, e.g., use naCoeffs->nAdd to add
 * coefficients of our numbers */
#define naCoeffs cf->extRing->cf

/* minimal polynomial */
#define naMinpoly naRing->qideal->m[0]

/// forward declarations
BOOLEAN  naGreaterZero(number a, const coeffs cf);
BOOLEAN  naGreater(number a, number b, const coeffs cf);
BOOLEAN  naEqual(number a, number b, const coeffs cf);
BOOLEAN  naIsOne(number a, const coeffs cf);
BOOLEAN  naIsMOne(number a, const coeffs cf);
number   naInit(long i, const coeffs cf);
number   naNeg(number a, const coeffs cf);
number   naInvers(number a, const coeffs cf);
number   naAdd(number a, number b, const coeffs cf);
number   naSub(number a, number b, const coeffs cf);
number   naMult(number a, number b, const coeffs cf);
number   naDiv(number a, number b, const coeffs cf);
void     naPower(number a, int exp, number *b, const coeffs cf);
number   naCopy(number a, const coeffs cf);
void     naWriteLong(number a, const coeffs cf);
void     naWriteShort(number a, const coeffs cf);
number   naGetDenom(number &a, const coeffs cf);
number   naGetNumerator(number &a, const coeffs cf);
number   naGcd(number a, number b, const coeffs cf);
void     naDelete(number *a, const coeffs cf);
void     naCoeffWrite(const coeffs cf, BOOLEAN details);
//number   naIntDiv(number a, number b, const coeffs cf);
const char * naRead(const char *s, number *a, const coeffs cf);

static BOOLEAN naCoeffIsEqual(const coeffs cf, n_coeffType n, void * param);


/// returns NULL if p == NULL, otherwise makes p monic by dividing
///   by its leading coefficient (only done if this is not already 1);
///   this assumes that we are over a ground field so that division
///   is well-defined;
///   modifies p
// void      p_Monic(poly p, const ring r);

///   assumes that p and q are univariate polynomials in r,
///   mentioning the same variable;
///   assumes a global monomial ordering in r;
///   assumes that not both p and q are NULL;
///   returns the gcd of p and q;
///   leaves p and q unmodified
// poly      p_Gcd(const poly p, const poly q, const ring r);

/* returns NULL if p == NULL, otherwise makes p monic by dividing
   by its leading coefficient (only done if this is not already 1);
   this assumes that we are over a ground field so that division
   is well-defined;
   modifies p */
static inline void p_Monic(poly p, const ring r)
{
  if (p == NULL) return;
  number n = n_Init(1, r->cf);
  if (p->next==NULL) { p_SetCoeff(p,n,r); return; }
  poly pp = p;
  number lc = p_GetCoeff(p, r);
  if (n_IsOne(lc, r->cf)) return;
  number lcInverse = n_Invers(lc, r->cf);
  p_SetCoeff(p, n, r);   // destroys old leading coefficient!
  pIter(p);
  while (p != NULL)
  {
    number n = n_Mult(p_GetCoeff(p, r), lcInverse, r->cf);
    n_Normalize(n,r->cf);
    p_SetCoeff(p, n, r);   // destroys old leading coefficient!
    pIter(p);
  }
  n_Delete(&lcInverse, r->cf);
  p = pp;
}

/// see p_Gcd;
///   additional assumption: deg(p) >= deg(q);
///   must destroy p and q (unless one of them is returned)
static inline poly p_GcdHelper(poly &p, poly &q, const ring r)
{
  while (q != NULL)
  {
    p_PolyDiv(p, q, FALSE, r);
    // swap p and q:
    poly& t = q;
    q = p;
    p = t;

  }
  return p;
}

/* assumes that p and q are univariate polynomials in r,
   mentioning the same variable;
   assumes a global monomial ordering in r;
   assumes that not both p and q are NULL;
   returns the gcd of p and q;
   leaves p and q unmodified */
static inline poly      p_Gcd(const poly p, const poly q, const ring r)
{
  assume((p != NULL) || (q != NULL));

  poly a = p; poly b = q;
  if (p_Deg(a, r) < p_Deg(b, r)) { a = q; b = p; }
  a = p_Copy(a, r); b = p_Copy(b, r);

  /* We have to make p monic before we return it, so that if the
     gcd is a unit in the ground field, we will actually return 1. */
  a = p_GcdHelper(a, b, r);
  p_Monic(a, r);
  return a;
}

/* see p_ExtGcd;
   additional assumption: deg(p) >= deg(q);
   must destroy p and q (unless one of them is returned) */
static inline poly p_ExtGcdHelper(poly &p, poly &pFactor, poly &q, poly &qFactor,
                                  ring r)
{
  if (q == NULL)
  {
    qFactor = NULL;
    pFactor = p_ISet(1, r);
    p_SetCoeff(pFactor, n_Invers(p_GetCoeff(p, r), r->cf), r);
    p_Monic(p, r);
    return p;
  }
  else
  {
    poly pDivQ = p_PolyDiv(p, q, TRUE, r);
    poly ppFactor = NULL; poly qqFactor = NULL;
    poly theGcd = p_ExtGcdHelper(q, qqFactor, p, ppFactor, r);
    pFactor = ppFactor;
    qFactor = p_Add_q(qqFactor,
                      p_Neg(p_Mult_q(pDivQ, p_Copy(ppFactor, r), r), r),
                      r);
    return theGcd;
  }
}


/* assumes that p and q are univariate polynomials in r,
   mentioning the same variable;
   assumes a global monomial ordering in r;
   assumes that not both p and q are NULL;
   returns the gcd of p and q;
   moreover, afterwards pFactor and qFactor contain appropriate
   factors such that gcd(p, q) = p * pFactor + q * qFactor;
   leaves p and q unmodified */
poly p_ExtGcd(poly p, poly &pFactor, poly q, poly &qFactor, ring r)
{
  assume((p != NULL) || (q != NULL));
  poly a = p; poly b = q; BOOLEAN aCorrespondsToP = TRUE;
  if (p_Deg(a, r) < p_Deg(b, r))
    { a = q; b = p; aCorrespondsToP = FALSE; }
  a = p_Copy(a, r); b = p_Copy(b, r);
  poly aFactor = NULL; poly bFactor = NULL;
  poly theGcd = p_ExtGcdHelper(a, aFactor, b, bFactor, r);
  if (aCorrespondsToP) { pFactor = aFactor; qFactor = bFactor; }
  else                 { pFactor = bFactor; qFactor = aFactor; }
  return theGcd;
}



#ifdef LDEBUG
BOOLEAN naDBTest(number a, const char *f, const int l, const coeffs cf)
{
  if (a == NULL) return TRUE;
  p_Test((poly)a, naRing);
  if (getCoeffType(cf)==n_algExt)
  {
    if((((poly)a)!=naMinpoly)
    && p_Totaldegree((poly)a, naRing) >= p_Totaldegree(naMinpoly, naRing)
    && (p_Totaldegree((poly)a, naRing)> 1)) // allow to output par(1)
    {
      dReportError("deg >= deg(minpoly) in %s:%d\n",f,l);
      return FALSE;
    }
  }
  return TRUE;
}
#endif

void heuristicReduce(poly &p, poly reducer, const coeffs cf);
void definiteReduce(poly &p, poly reducer, const coeffs cf);

/* returns the bottom field in this field extension tower; if the tower
   is flat, i.e., if there is no extension, then r itself is returned;
   as a side-effect, the counter 'height' is filled with the height of
   the extension tower (in case the tower is flat, 'height' is zero) */
static coeffs nCoeff_bottom(const coeffs r, int &height)
{
  assume(r != NULL);
  coeffs cf = r;
  height = 0;
  while (nCoeff_is_Extension(cf))
  {
    assume(cf->extRing != NULL); assume(cf->extRing->cf != NULL);
    cf = cf->extRing->cf;
    height++;
  }
  return cf;
}

BOOLEAN naIsZero(number a, const coeffs cf)
{
  naTest(a);
  return (a == NULL);
}

void naDelete(number * a, const coeffs cf)
{
  if (*a == NULL) return;
  if (((poly)*a)==naMinpoly) { *a=NULL;return;}
  poly aAsPoly = (poly)(*a);
  p_Delete(&aAsPoly, naRing);
  *a = NULL;
}

BOOLEAN naEqual(number a, number b, const coeffs cf)
{
  naTest(a); naTest(b);
  /// simple tests
  if (a == NULL) return (b == NULL);
  if (b == NULL) return (a == NULL);
  return p_EqualPolys((poly)a,(poly)b,naRing);
}

number naCopy(number a, const coeffs cf)
{
  naTest(a);
  if (a == NULL) return NULL;
  if (((poly)a)==naMinpoly) return a;
  return (number)p_Copy((poly)a, naRing);
}

number naGetNumerator(number &a, const coeffs cf)
{
  return naCopy(a, cf);
}

number naGetDenom(number &a, const coeffs cf)
{
  naTest(a);
  return naInit(1, cf);
}

BOOLEAN naIsOne(number a, const coeffs cf)
{
  naTest(a);
  poly aAsPoly = (poly)a;
  if ((a==NULL) || (!p_IsConstant(aAsPoly, naRing))) return FALSE;
  return n_IsOne(p_GetCoeff(aAsPoly, naRing), naCoeffs);
}

BOOLEAN naIsMOne(number a, const coeffs cf)
{
  naTest(a);
  poly aAsPoly = (poly)a;
  if ((a==NULL) || (!p_IsConstant(aAsPoly, naRing))) return FALSE;
  return n_IsMOne(p_GetCoeff(aAsPoly, naRing), naCoeffs);
}

/// this is in-place, modifies a
number naNeg(number a, const coeffs cf)
{
  naTest(a);
  if (a != NULL) a = (number)p_Neg((poly)a, naRing);
  return a;
}

number naInit(long i, const coeffs cf)
{
  if (i == 0) return NULL;
  else        return (number)p_ISet(i, naRing);
}

static number naInitMPZ(mpz_t m, const coeffs r)
{
  number n=n_InitMPZ(m,r->extRing->cf);
  return (number)p_NSet(n,r->extRing);
}

long naInt(number &a, const coeffs cf)
{
  naTest(a);
  poly aAsPoly = (poly)a;
  if(aAsPoly == NULL)
    return 0;
  if (!p_IsConstant(aAsPoly, naRing))
    return 0;
  assume( aAsPoly != NULL );
  return n_Int(p_GetCoeff(aAsPoly, naRing), naCoeffs);
}

/* TRUE iff (a != 0 and (b == 0 or deg(a) > deg(b) or (deg(a)==deg(b) && lc(a)>lc(b))) */
BOOLEAN naGreater(number a, number b, const coeffs cf)
{
  naTest(a); naTest(b);
  if (naIsZero(a, cf))
  {
    if (naIsZero(b, cf)) return FALSE;
    return !n_GreaterZero(pGetCoeff((poly)b),naCoeffs);
  }
  if (naIsZero(b, cf))
  {
    return n_GreaterZero(pGetCoeff((poly)a),naCoeffs);
  }
  int aDeg = p_Totaldegree((poly)a, naRing);
  int bDeg = p_Totaldegree((poly)b, naRing);
  if (aDeg>bDeg) return TRUE;
  if (aDeg<bDeg) return FALSE;
  return n_Greater(pGetCoeff((poly)a),pGetCoeff((poly)b),naCoeffs);
}

/* TRUE iff a != 0 and (LC(a) > 0 or deg(a) > 0) */
BOOLEAN naGreaterZero(number a, const coeffs cf)
{
  naTest(a);
  if (a == NULL)                                            return FALSE;
  if (n_GreaterZero(p_GetCoeff((poly)a, naRing), naCoeffs)) return TRUE;
  if (p_Totaldegree((poly)a, naRing) > 0)                   return TRUE;
  return FALSE;
}

void naCoeffWrite(const coeffs cf, BOOLEAN details)
{
  assume( cf != NULL );

  const ring A = cf->extRing;

  assume( A != NULL );
  assume( A->cf != NULL );

  n_CoeffWrite(A->cf, details);

//  rWrite(A);

  const int P = rVar(A);
  assume( P > 0 );

  PrintS("[");

  for (int nop=0; nop < P; nop ++)
  {
    Print("%s", rRingVar(nop, A));
    if (nop!=P-1) PrintS(", ");
  }

  PrintS("]/(");

  const ideal I = A->qideal;

  assume( I != NULL );
  assume( IDELEMS(I) == 1 );


  if ( details )
  {
    p_Write0( I->m[0], A);
    PrintS(")");
  }
  else
    PrintS("...)");

/*
  char *x = rRingVar(0, A);

  Print("//   Coefficients live in the extension field K[%s]/<f(%s)>\n", x, x);
  Print("//   with the minimal polynomial f(%s) = %s\n", x,
        p_String(A->qideal->m[0], A));
  PrintS("//   and K: ");
*/
}

number naAdd(number a, number b, const coeffs cf)
{
  naTest(a); naTest(b);
  if (a == NULL) return naCopy(b, cf);
  if (b == NULL) return naCopy(a, cf);
  poly aPlusB = p_Add_q(p_Copy((poly)a, naRing),
                        p_Copy((poly)b, naRing), naRing);
  //definiteReduce(aPlusB, naMinpoly, cf);
  return (number)aPlusB;
}

number naSub(number a, number b, const coeffs cf)
{
  naTest(a); naTest(b);
  if (b == NULL) return naCopy(a, cf);
  poly minusB = p_Neg(p_Copy((poly)b, naRing), naRing);
  if (a == NULL) return (number)minusB;
  poly aMinusB = p_Add_q(p_Copy((poly)a, naRing), minusB, naRing);
  //definiteReduce(aMinusB, naMinpoly, cf);
  return (number)aMinusB;
}

number naMult(number a, number b, const coeffs cf)
{
  naTest(a); naTest(b);
  if ((a == NULL)||(b == NULL)) return NULL;
  poly aTimesB = pp_Mult_qq((poly)a, (poly)b, naRing);
  definiteReduce(aTimesB, naMinpoly, cf);
  p_Normalize(aTimesB,naRing);
  return (number)aTimesB;
}

number naDiv(number a, number b, const coeffs cf)
{
  naTest(a); naTest(b);
  if (b == NULL) WerrorS(nDivBy0);
  if (a == NULL) return NULL;
  poly bInverse = (poly)naInvers(b, cf);
  if(bInverse != NULL) // b is non-zero divisor!
  {
    poly aDivB = p_Mult_q(p_Copy((poly)a, naRing), bInverse, naRing);
    definiteReduce(aDivB, naMinpoly, cf);
    p_Normalize(aDivB,naRing);
    return (number)aDivB;
  }
  return NULL;
}

/* 0^0 = 0;
   for |exp| <= 7 compute power by a simple multiplication loop;
   for |exp| >= 8 compute power along binary presentation of |exp|, e.g.
      p^13 = p^1 * p^4 * p^8, where we utilise that
      p^(2^(k+1)) = p^(2^k) * p^(2^k);
   intermediate reduction modulo the minimal polynomial is controlled by
   the in-place method heuristicReduce(poly, poly, coeffs); see there.
*/
void naPower(number a, int exp, number *b, const coeffs cf)
{
  naTest(a);

  /* special cases first */
  if (a == NULL)
  {
    if (exp >= 0) *b = NULL;
    else          WerrorS(nDivBy0);
    return;
  }
  else if (exp ==  0) { *b = naInit(1, cf); return; }
  else if (exp ==  1) { *b = naCopy(a, cf); return; }
  else if (exp == -1) { *b = naInvers(a, cf); return; }

  int expAbs = exp; if (expAbs < 0) expAbs = -expAbs;

  /* now compute a^expAbs */
  poly pow; poly aAsPoly = (poly)a;
  if (expAbs <= 7)
  {
    pow = p_Copy(aAsPoly, naRing);
    for (int i = 2; i <= expAbs; i++)
    {
      pow = p_Mult_q(pow, p_Copy(aAsPoly, naRing), naRing);
      heuristicReduce(pow, naMinpoly, cf);
    }
    definiteReduce(pow, naMinpoly, cf);
  }
  else
  {
    pow = p_ISet(1, naRing);
    poly factor = p_Copy(aAsPoly, naRing);
    while (expAbs != 0)
    {
      if (expAbs & 1)
      {
        pow = p_Mult_q(pow, p_Copy(factor, naRing), naRing);
        heuristicReduce(pow, naMinpoly, cf);
      }
      expAbs = expAbs / 2;
      if (expAbs != 0)
      {
        factor = p_Mult_q(factor, p_Copy(factor, naRing), naRing);
        heuristicReduce(factor, naMinpoly, cf);
      }
    }
    p_Delete(&factor, naRing);
    definiteReduce(pow, naMinpoly, cf);
  }

  /* invert if original exponent was negative */
  number n = (number)pow;
  if (exp < 0)
  {
    number m = naInvers(n, cf);
    naDelete(&n, cf);
    n = m;
  }
  *b = n;
}

/* may reduce p modulo the reducer by calling definiteReduce;
   the decision is made based on the following heuristic
   (which should also only be changed here in this method):
      if (deg(p) > 10*deg(reducer) then perform reduction;
   modifies p */
void heuristicReduce(poly &p, poly reducer, const coeffs cf)
{
  #ifdef LDEBUG
  p_Test((poly)p, naRing);
  p_Test((poly)reducer, naRing);
  #endif
  if (p_Totaldegree(p, naRing) > 10 * p_Totaldegree(reducer, naRing))
    definiteReduce(p, reducer, cf);
}

void naWriteLong(number a, const coeffs cf)
{
  naTest(a);
  if (a == NULL)
    StringAppendS("0");
  else
  {
    poly aAsPoly = (poly)a;
    /* basically, just write aAsPoly using p_Write,
       but use brackets around the output, if a is not
       a constant living in naCoeffs = cf->extRing->cf */
    BOOLEAN useBrackets = !(p_IsConstant(aAsPoly, naRing));
    if (useBrackets) StringAppendS("(");
    p_String0Long(aAsPoly, naRing, naRing);
    if (useBrackets) StringAppendS(")");
  }
}

void naWriteShort(number a, const coeffs cf)
{
  naTest(a);
  if (a == NULL)
    StringAppendS("0");
  else
  {
    poly aAsPoly = (poly)a;
    /* basically, just write aAsPoly using p_Write,
       but use brackets around the output, if a is not
       a constant living in naCoeffs = cf->extRing->cf */
    BOOLEAN useBrackets = !(p_IsConstant(aAsPoly, naRing));
    if (useBrackets) StringAppendS("(");
    p_String0Short(aAsPoly, naRing, naRing);
    if (useBrackets) StringAppendS(")");
  }
}

const char * naRead(const char *s, number *a, const coeffs cf)
{
  poly aAsPoly;
  const char * result = p_Read(s, aAsPoly, naRing);
  if (aAsPoly!=NULL) definiteReduce(aAsPoly, naMinpoly, cf);
  *a = (number)aAsPoly;
  return result;
}

#if 0
/* implemented by the rule lcm(a, b) = a * b / gcd(a, b) */
number naLcm(number a, number b, const coeffs cf)
{
  naTest(a); naTest(b);
  if (a == NULL) return NULL;
  if (b == NULL) return NULL;
  number theProduct = (number)pp_Mult_qq((poly)a, (poly)b, naRing);
  /* note that theProduct needs not be reduced w.r.t. naMinpoly;
     but the final division will take care of the necessary reduction */
  number theGcd = naGcd(a, b, cf);
  return naDiv(theProduct, theGcd, cf);
}
#endif
number napNormalizeHelper(number b, const coeffs cf)
{
  number h=n_Init(1,naRing->cf);
  poly bb=(poly)b;
  number d;
  while(bb!=NULL)
  {
    d=n_NormalizeHelper(h,pGetCoeff(bb), naRing->cf);
    n_Delete(&h,naRing->cf);
    h=d;
    pIter(bb);
  }
  return h;
}
number naLcmContent(number a, number b, const coeffs cf)
{
  if (nCoeff_is_Zp(naRing->cf)) return naCopy(a,cf);
#if 0
  else {
    number g = ndGcd(a, b, cf);
    return g;
  }
#else
  {
    a=(number)p_Copy((poly)a,naRing);
    number t=napNormalizeHelper(b,cf);
    if(!n_IsOne(t,naRing->cf))
    {
      number bt, rr;
      poly xx=(poly)a;
      while (xx!=NULL)
      {
        bt = n_SubringGcd(t, pGetCoeff(xx), naRing->cf);
        rr = n_Mult(t, pGetCoeff(xx), naRing->cf);
        n_Delete(&pGetCoeff(xx),naRing->cf);
        pGetCoeff(xx) = n_Div(rr, bt, naRing->cf);
        n_Normalize(pGetCoeff(xx),naRing->cf);
        n_Delete(&bt,naRing->cf);
        n_Delete(&rr,naRing->cf);
        pIter(xx);
      }
    }
    n_Delete(&t,naRing->cf);
    return (number) a;
  }
#endif
}

/* expects *param to be castable to AlgExtInfo */
static BOOLEAN naCoeffIsEqual(const coeffs cf, n_coeffType n, void * param)
{
  if (n_algExt != n) return FALSE;
  AlgExtInfo *e = (AlgExtInfo *)param;
  /* for extension coefficient fields we expect the underlying
     polynomial rings to be IDENTICAL, i.e. the SAME OBJECT;
     this expectation is based on the assumption that we have properly
     registered cf and perform reference counting rather than creating
     multiple copies of the same coefficient field/domain/ring */
  if (naRing == e->r)
    return TRUE;
  /* (Note that then also the minimal ideals will necessarily be
     the same, as they are attached to the ring.) */

  // NOTE: Q(a)[x] && Q(a)[y] should better share the _same_ Q(a)...
  if( rEqual(naRing, e->r, TRUE) ) // also checks the equality of qideals
  {
    const ideal mi = naRing->qideal;
    assume( IDELEMS(mi) == 1 );
    const ideal ii = e->r->qideal;
    assume( IDELEMS(ii) == 1 );

    // TODO: the following should be extended for 2 *equal* rings...
    assume( p_EqualPolys(mi->m[0], ii->m[0], naRing, e->r) );

    rDelete(e->r);

    return TRUE;
  }

  return FALSE;

}

int naSize(number a, const coeffs cf)
{
  if (a == NULL) return 0;
  poly aAsPoly = (poly)a;
  int theDegree = 0; int noOfTerms = 0;
  while (aAsPoly != NULL)
  {
    noOfTerms++;
    int d = p_GetExp(aAsPoly, 1, naRing);
    if (d > theDegree) theDegree = d;
    pIter(aAsPoly);
  }
  return (theDegree +1) * noOfTerms;
}

/* performs polynomial division and overrides p by the remainder
   of division of p by the reducer;
   modifies p */
void definiteReduce(poly &p, poly reducer, const coeffs cf)
{
  #ifdef LDEBUG
  p_Test((poly)p, naRing);
  p_Test((poly)reducer, naRing);
  #endif
  if ((p!=NULL) && (p_GetExp(p,1,naRing)>=p_GetExp(reducer,1,naRing)))
  {
    p_PolyDiv(p, reducer, FALSE, naRing);
  }
}

void  naNormalize(number &a, const coeffs cf)
{
  poly aa=(poly)a;
  if (aa!=naMinpoly)
    definiteReduce(aa,naMinpoly,cf);
  a=(number)aa;
}

number naConvFactoryNSingN( const CanonicalForm n, const coeffs cf)
{
  if (n.isZero()) return NULL;
  poly p=convFactoryPSingP(n,naRing);
  return (number)p;
}
CanonicalForm naConvSingNFactoryN( number n, BOOLEAN /*setChar*/, const coeffs cf )
{
  naTest(n);
  if (n==NULL) return CanonicalForm(0);

  return convSingPFactoryP((poly)n,naRing);
}

/* IMPORTANT NOTE: Since an algebraic field extension is again a field,
                   the gcd of two elements is not very interesting. (It
                   is actually any unit in the field, i.e., any non-
                   zero element.) Note that the below method does not operate
                   in this strong sense but rather computes the gcd of
                   two given elements in the underlying polynomial ring. */
number naGcd(number a, number b, const coeffs cf)
{
  if (a==NULL)  return naCopy(b,cf);
  if (b==NULL)  return naCopy(a,cf);

  poly ax=(poly)a;
  poly bx=(poly)b;
  if (pNext(ax)!=NULL)
    return (number)p_Copy(ax, naRing);
  else
  {
    if(nCoeff_is_Zp(naRing->cf))
      return naInit(1,cf);
    else
    {
      number x = n_Copy(pGetCoeff((poly)a),naRing->cf);
      if (n_IsOne(x,naRing->cf))
        return (number)p_NSet(x,naRing);
      while (pNext(ax)!=NULL)
      {
        pIter(ax);
        number y = n_SubringGcd(x, pGetCoeff(ax), naRing->cf);
        n_Delete(&x,naRing->cf);
        x = y;
        if (n_IsOne(x,naRing->cf))
          return (number)p_NSet(x,naRing);
      }
      do
      {
        number y = n_SubringGcd(x, pGetCoeff(bx), naRing->cf);
        n_Delete(&x,naRing->cf);
        x = y;
        if (n_IsOne(x,naRing->cf))
          return (number)p_NSet(x,naRing);
        pIter(bx);
      }
      while (bx!=NULL);
      return (number)p_NSet(x,naRing);
    }
  }
#if 0
  naTest(a); naTest(b);
  const ring R = naRing;
  return (number) singclap_gcd_r((poly)a, (poly)b, R);
#endif
//  return (number)p_Gcd((poly)a, (poly)b, naRing);
}

number naInvers(number a, const coeffs cf)
{
  naTest(a);
  if (a == NULL) WerrorS(nDivBy0);

  poly aFactor = NULL; poly mFactor = NULL; poly theGcd = NULL;
// singclap_extgcd!
  const BOOLEAN ret = singclap_extgcd ((poly)a, naMinpoly, theGcd, aFactor, mFactor, naRing);

  assume( !ret );

//  if( ret ) theGcd = p_ExtGcd((poly)a, aFactor, naMinpoly, mFactor, naRing);

  naTest((number)theGcd); naTest((number)aFactor); naTest((number)mFactor);
  p_Delete(&mFactor, naRing);

  //  /* the gcd must be 1 since naMinpoly is irreducible and a != NULL: */
  //  assume(naIsOne((number)theGcd, cf));

  if( !naIsOne((number)theGcd, cf) )
  {
    WerrorS("zero divisor found - your minpoly is not irreducible");
    p_Delete(&aFactor, naRing); aFactor = NULL;
  }
  p_Delete(&theGcd, naRing);

  return (number)(aFactor);
}

/* assumes that src = Q or Z, dst = Q(a) */
number naMap00(number a, const coeffs src, const coeffs dst)
{
  if (n_IsZero(a, src)) return NULL;
  assume(src->rep == dst->extRing->cf->rep);
  poly result = p_One(dst->extRing);
  p_SetCoeff(result, n_Copy(a, src), dst->extRing);
  return (number)result;
}

/* assumes that src = Z, dst = K(a) */
number naMapZ0(number a, const coeffs src, const coeffs dst)
{
  if (n_IsZero(a, src)) return NULL;
  poly result = p_One(dst->extRing);
  nMapFunc nMap=n_SetMap(src,dst->extRing->cf);
  p_SetCoeff(result, nMap(a, src, dst->extRing->cf), dst->extRing);
  if (n_IsZero(pGetCoeff(result),dst->extRing->cf))
    p_Delete(&result,dst->extRing);
  return (number)result;
}

/* assumes that src = Z/p, dst = Q(a) */
number naMapP0(number a, const coeffs src, const coeffs dst)
{
  if (n_IsZero(a, src)) return NULL;
  /* mapping via intermediate int: */
  int n = n_Int(a, src);
  number q = n_Init(n, dst->extRing->cf);
  poly result = p_One(dst->extRing);
  p_SetCoeff(result, q, dst->extRing);
  return (number)result;
}

#if 0
/* assumes that either src = Q(a), dst = Q(a), or
                       src = Z/p(a), dst = Z/p(a)     */
number naCopyMap(number a, const coeffs src, const coeffs dst)
{
  return naCopy(a, dst);
}
#endif

number naCopyTrans2AlgExt(number a, const coeffs src, const coeffs dst)
{
  assume (nCoeff_is_transExt (src));
  assume (nCoeff_is_algExt (dst));
  fraction fa=(fraction)a;
  poly p, q;
  if (rSamePolyRep(src->extRing, dst->extRing))
  {
    p = p_Copy(NUM(fa),src->extRing);
    if (!DENIS1(fa))
    {
      q = p_Copy(DEN(fa),src->extRing);
      assume (q != NULL);
    }
  }
  else
  {
    assume ((strcmp(rRingVar(0,src->extRing),rRingVar(0,dst->extRing))==0) && (rVar (src->extRing) == rVar (dst->extRing)));

    nMapFunc nMap= n_SetMap (src->extRing->cf, dst->extRing->cf);

    assume (nMap != NULL);
    p= p_PermPoly (NUM (fa), NULL, src->extRing, dst->extRing,nMap, NULL,rVar (src->extRing));
    if (!DENIS1(fa))
    {
      q= p_PermPoly (DEN (fa), NULL, src->extRing, dst->extRing,nMap, NULL,rVar (src->extRing));
      assume (q != NULL);
    }
  }
  definiteReduce(p, dst->extRing->qideal->m[0], dst);
  p_Test (p, dst->extRing);
  if (!DENIS1(fa))
  {
    definiteReduce(q, dst->extRing->qideal->m[0], dst);
    p_Test (q, dst->extRing);
    if (q != NULL)
    {
      number t= naDiv ((number)p,(number)q, dst);
      p_Delete (&p, dst->extRing);
      p_Delete (&q, dst->extRing);
      return t;
    }
    WerrorS ("mapping denominator to zero");
  }
  return (number) p;
}

/* assumes that src = Q, dst = Z/p(a) */
number naMap0P(number a, const coeffs src, const coeffs dst)
{
  if (n_IsZero(a, src)) return NULL;
  // int p = rChar(dst->extRing);

  number q = nlModP(a, src, dst->extRing->cf); // FIXME? TODO? // extern number nlModP(number q, const coeffs Q, const coeffs Zp); // Map q \in QQ \to pZ

  poly result = p_NSet(q, dst->extRing);

  return (number)result;
}

/* assumes that src = Z/p, dst = Z/p(a) */
number naMapPP(number a, const coeffs src, const coeffs dst)
{
  if (n_IsZero(a, src)) return NULL;
  assume(src == dst->extRing->cf);
  poly result = p_One(dst->extRing);
  p_SetCoeff(result, n_Copy(a, src), dst->extRing);
  return (number)result;
}

/* assumes that src = Z/u, dst = Z/p(a), where u != p */
number naMapUP(number a, const coeffs src, const coeffs dst)
{
  if (n_IsZero(a, src)) return NULL;
  /* mapping via intermediate int: */
  int n = n_Int(a, src);
  number q = n_Init(n, dst->extRing->cf);
  poly result = p_One(dst->extRing);
  p_SetCoeff(result, q, dst->extRing);
  return (number)result;
}

number naGenMap(number a, const coeffs cf, const coeffs dst)
{
  if (a==NULL) return NULL;

  const ring rSrc = cf->extRing;
  const ring rDst = dst->extRing;

  const nMapFunc nMap=n_SetMap(rSrc->cf,rDst->cf);
  poly f = (poly)a;
  poly g = prMapR(f, nMap, rSrc, rDst);

  n_Test((number)g, dst);
  return (number)g;
}

number naGenTrans2AlgExt(number a, const coeffs cf, const coeffs dst)
{
  if (a==NULL) return NULL;

  const ring rSrc = cf->extRing;
  const ring rDst = dst->extRing;

  const nMapFunc nMap=n_SetMap(rSrc->cf,rDst->cf);
  fraction f = (fraction)a;
  poly g = prMapR(NUM(f), nMap, rSrc, rDst);

  number result=NULL;
  poly h = NULL;

  if (!DENIS1(f))
     h = prMapR(DEN(f), nMap, rSrc, rDst);

  if (h!=NULL)
  {
    result=naDiv((number)g,(number)h,dst);
    p_Delete(&g,dst->extRing);
    p_Delete(&h,dst->extRing);
  }
  else
    result=(number)g;

  n_Test((number)result, dst);
  return (number)result;
}

nMapFunc naSetMap(const coeffs src, const coeffs dst)
{
  /* dst is expected to be an algebraic field extension */
  assume(getCoeffType(dst) == n_algExt);

  int h = 0; /* the height of the extension tower given by dst */
  coeffs bDst = nCoeff_bottom(dst, h); /* the bottom field in the tower dst */
  coeffs bSrc = nCoeff_bottom(src, h); /* the bottom field in the tower src */

  /* for the time being, we only provide maps if h = 1 or 0 */
  if (h==0)
  {
    if ((src->rep==n_rep_gap_rat) && nCoeff_is_Q(bDst))
      return naMap00;                            /// Q or Z     -->  Q(a)
    if ((src->rep==n_rep_gap_gmp) && nCoeff_is_Q(bDst))
      return naMapZ0;                            /// Z     -->  Q(a)
    if (nCoeff_is_Zp(src) && nCoeff_is_Q(bDst))
      return naMapP0;                            /// Z/p   -->  Q(a)
    if (nCoeff_is_Q_or_BI(src) && nCoeff_is_Zp(bDst))
      return naMap0P;                            /// Q      --> Z/p(a)
    if ((src->rep==n_rep_gap_gmp) && nCoeff_is_Zp(bDst))
      return naMapZ0;                            /// Z     -->  Z/p(a)
    if (nCoeff_is_Zp(src) && nCoeff_is_Zp(bDst))
    {
      if (src->ch == dst->ch) return naMapPP;    /// Z/p    --> Z/p(a)
      else return naMapUP;                       /// Z/u    --> Z/p(a)
    }
  }
  if (h != 1) return NULL;
  if ((!nCoeff_is_Zp(bDst)) && (!nCoeff_is_Q(bDst))) return NULL;
  if ((!nCoeff_is_Zp(bSrc)) && (!nCoeff_is_Q_or_BI(bSrc))) return NULL;

  nMapFunc nMap=n_SetMap(src->extRing->cf,dst->extRing->cf);
  if (rSamePolyRep(src->extRing, dst->extRing) && (strcmp(rRingVar(0, src->extRing), rRingVar(0, dst->extRing)) == 0))
  {
    if (src->type==n_algExt)
       return ndCopyMap; // naCopyMap;         /// K(a)   --> K(a)
    else
       return naCopyTrans2AlgExt;
  }
  else if ((nMap!=NULL) && (strcmp(rRingVar(0,src->extRing),rRingVar(0,dst->extRing))==0) && (rVar (src->extRing) == rVar (dst->extRing)))
  {
    if (src->type==n_algExt)
       return naGenMap; // naCopyMap;         /// K(a)   --> K'(a)
    else
       return naGenTrans2AlgExt;
  }

  return NULL;                                           /// default
}

int naParDeg(number a, const coeffs cf)
{
  if (a == NULL) return -1;
  poly aa=(poly)a;
  return cf->extRing->pFDeg(aa,cf->extRing);
}

/// return the specified parameter as a number in the given alg. field
number naParameter(const int iParameter, const coeffs cf)
{
  assume(getCoeffType(cf) == n_algExt);

  const ring R = cf->extRing;
  assume( R != NULL );
  assume( 0 < iParameter && iParameter <= rVar(R) );

  poly p = p_One(R); p_SetExp(p, iParameter, 1, R); p_Setm(p, R);

  return (number) p;
}


/// if m == var(i)/1 => return i,
int naIsParam(number m, const coeffs cf)
{
  assume((getCoeffType(cf) == n_algExt)||(getCoeffType(cf) == n_polyExt));

  const ring R = cf->extRing;
  assume( R != NULL );

  return p_Var( (poly)m, R );
}


static void naClearContent(ICoeffsEnumerator& numberCollectionEnumerator, number& c, const coeffs cf)
{
  assume(cf != NULL);
  assume(getCoeffType(cf) == n_algExt);
  assume(nCoeff_is_Q_algext(cf)); // only over (Q[a]/m(a)), while the default impl. is used over Zp[a]/m(a) !

  const ring   R = cf->extRing;
  assume(R != NULL);
  const coeffs Q = R->cf;
  assume(Q != NULL);
  assume(nCoeff_is_Q(Q));

  numberCollectionEnumerator.Reset();

  if( !numberCollectionEnumerator.MoveNext() ) // empty zero polynomial?
  {
    c = n_Init(1, cf);
    return;
  }

  naTest(numberCollectionEnumerator.Current());

  // part 1, find a small candidate for gcd
  int s1; int s=2147483647; // max. int

  const BOOLEAN lc_is_pos=naGreaterZero(numberCollectionEnumerator.Current(),cf);

  int normalcount = 0;

  poly cand1, cand;

  do
  {
    number& n = numberCollectionEnumerator.Current();
    naNormalize(n, cf); ++normalcount;

    naTest(n);

    cand1 = (poly)n;

    s1 = p_Deg(cand1, R); // naSize?
    if (s>s1)
    {
      cand = cand1;
      s = s1;
    }
  } while (numberCollectionEnumerator.MoveNext() );

//  assume( nlGreaterZero(cand,cf) ); // cand may be a negative integer!

  cand = p_Copy(cand, R);
  // part 2: compute gcd(cand,all coeffs)

  numberCollectionEnumerator.Reset();

  int length = 0;
  while (numberCollectionEnumerator.MoveNext() )
  {
    number& n = numberCollectionEnumerator.Current();
    ++length;

    if( (--normalcount) <= 0)
      naNormalize(n, cf);

    naTest(n);

//    p_InpGcd(cand, (poly)n, R);

    { // R->cf is QQ
      poly tmp=gcd_over_Q(cand,(poly)n,R);
      p_Delete(&cand,R);
      cand=tmp;
    }

//    cand1 = p_Gcd(cand,(poly)n, R); p_Delete(&cand, R); cand = cand1;

    assume( naGreaterZero((number)cand, cf) ); // ???
/*
    if(p_IsConstant(cand,R))
    {
      c = cand;

      if(!lc_is_pos)
      {
        // make the leading coeff positive
        c = nlNeg(c, cf);
        numberCollectionEnumerator.Reset();

        while (numberCollectionEnumerator.MoveNext() )
        {
          number& nn = numberCollectionEnumerator.Current();
          nn = nlNeg(nn, cf);
        }
      }
      return;
    }
*/

  }


  // part3: all coeffs = all coeffs / cand
  if (!lc_is_pos)
    cand = p_Neg(cand, R);

  c = (number)cand; naTest(c);

  poly cInverse = (poly)naInvers(c, cf);
  assume(cInverse != NULL); // c is non-zero divisor!?


  numberCollectionEnumerator.Reset();


  while (numberCollectionEnumerator.MoveNext() )
  {
    number& n = numberCollectionEnumerator.Current();

    assume( length > 0 );

    if( --length > 0 )
    {
      assume( cInverse != NULL );
      n = (number) p_Mult_q(p_Copy(cInverse, R), (poly)n, R);
    }
    else
    {
      n = (number) p_Mult_q(cInverse, (poly)n, R);
      cInverse = NULL;
      assume(length == 0);
    }

    definiteReduce((poly &)n, naMinpoly, cf);
  }

  assume(length == 0);
  assume(cInverse == NULL); //   p_Delete(&cInverse, R);

  // Quick and dirty fix for constant content clearing... !?
  CRecursivePolyCoeffsEnumerator<NAConverter> itr(numberCollectionEnumerator); // recursively treat the numbers as polys!

  number cc;

  n_ClearContent(itr, cc, Q); // TODO: get rid of (-LC) normalization!?

  // over alg. ext. of Q // takes over the input number
  c = (number) __p_Mult_nn( (poly)c, cc, R);
//      p_Mult_q(p_NSet(cc, R), , R);

  n_Delete(&cc, Q);

  // TODO: the above is not enough! need GCD's of polynomial coeffs...!
/*
  // old and wrong part of p_Content
    if (rField_is_Q_a(r) && !CLEARENUMERATORS) // should not be used anymore if CLEARENUMERATORS is 1
    {
      // we only need special handling for alg. ext.
      if (getCoeffType(r->cf)==n_algExt)
      {
        number hzz = n_Init(1, r->cf->extRing->cf);
        p=ph;
        while (p!=NULL)
        { // each monom: coeff in Q_a
          poly c_n_n=(poly)pGetCoeff(p);
          poly c_n=c_n_n;
          while (c_n!=NULL)
          { // each monom: coeff in Q
            d=n_NormalizeHelper(hzz,pGetCoeff(c_n),r->cf->extRing->cf);
            n_Delete(&hzz,r->cf->extRing->cf);
            hzz=d;
            pIter(c_n);
          }
          pIter(p);
        }
        // hzz contains the 1/lcm of all denominators in c_n_n
        h=n_Invers(hzz,r->cf->extRing->cf);
        n_Delete(&hzz,r->cf->extRing->cf);
        n_Normalize(h,r->cf->extRing->cf);
        if(!n_IsOne(h,r->cf->extRing->cf))
        {
          p=ph;
          while (p!=NULL)
          { // each monom: coeff in Q_a
            poly c_n=(poly)pGetCoeff(p);
            while (c_n!=NULL)
            { // each monom: coeff in Q
              d=n_Mult(h,pGetCoeff(c_n),r->cf->extRing->cf);
              n_Normalize(d,r->cf->extRing->cf);
              n_Delete(&pGetCoeff(c_n),r->cf->extRing->cf);
              pGetCoeff(c_n)=d;
              pIter(c_n);
            }
            pIter(p);
          }
        }
        n_Delete(&h,r->cf->extRing->cf);
      }
    }
*/


//  c = n_Init(1, cf); assume(FALSE); // TODO: NOT YET IMPLEMENTED!!!
}


void naClearDenominators(ICoeffsEnumerator& numberCollectionEnumerator, number& c, const coeffs cf)
{
  assume(cf != NULL);
  assume((getCoeffType(cf) == n_algExt)||(getCoeffType(cf) == n_polyExt));
  assume(nCoeff_is_Q_algext(cf)); // only over (Q[a]/m(a)), while the default impl. is used over Zp[a]/m(a) !

  assume(cf->extRing != NULL);
  const coeffs Q = cf->extRing->cf;
  assume(Q != NULL);
  assume(nCoeff_is_Q(Q));
  number n;
  CRecursivePolyCoeffsEnumerator<NAConverter> itr(numberCollectionEnumerator); // recursively treat the numbers as polys!
  n_ClearDenominators(itr, n, Q); // this should probably be fine...
  c = (number)p_NSet(n, cf->extRing); // over alg. ext. of Q // takes over the input number
}

void naKillChar(coeffs cf)
{
  rDecRefCnt(cf->extRing);
  if(cf->extRing->ref<0)
    rDelete(cf->extRing);
}

char* naCoeffName(const coeffs r) // currently also for tranext.
{
  const char* const* p=n_ParameterNames(r);
  int l=0;
  int i;
  for(i=0; i<n_NumberOfParameters(r);i++)
  {
    l+=(strlen(p[i])+1);
  }
  STATIC_VAR char s[200];
  s[0]='\0';
  snprintf(s,10+1,"%d",r->ch); /* Fp(a) or Q(a) */
  char tt[2];
  tt[0]=',';
  tt[1]='\0';
  for(i=0; i<n_NumberOfParameters(r);i++)
  {
    strcat(s,tt);
    strcat(s,p[i]);
  }
  return s;
}

number  naChineseRemainder(number *x, number *q,int rl, BOOLEAN /*sym*/,CFArray &inv_cache,const coeffs cf)
{
  poly *P=(poly*)omAlloc(rl*sizeof(poly*));
  number *X=(number *)omAlloc(rl*sizeof(number));
  int i;
  for(i=0;i<rl;i++) P[i]=p_Copy((poly)(x[i]),cf->extRing);
  poly result=p_ChineseRemainder(P,X,q,rl,inv_cache,cf->extRing);
  omFreeSize(X,rl*sizeof(number));
  omFreeSize(P,rl*sizeof(poly*));
  return ((number)result);
}

number  naFarey(number p, number n, const coeffs cf)
{
  // n is really a bigint
  poly result=p_Farey(p_Copy((poly)p,cf->extRing),n,cf->extRing);
  return ((number)result);
}


BOOLEAN naInitChar(coeffs cf, void * infoStruct)
{
  assume( infoStruct != NULL );

  AlgExtInfo *e = (AlgExtInfo *)infoStruct;
  /// first check whether cf->extRing != NULL and delete old ring???

  assume(e->r                     != NULL);      // extRing;
  assume(e->r->cf                 != NULL);      // extRing->cf;

  assume((e->r->qideal            != NULL) &&    // minideal has one
         (IDELEMS(e->r->qideal)   == 1)    &&    // non-zero generator
         (e->r->qideal->m[0]      != NULL)    ); // at m[0];

  assume( cf != NULL );
  assume(getCoeffType(cf) == n_algExt);                     // coeff type;

  rIncRefCnt(e->r); // increase the ref.counter for the ground poly. ring!
  const ring R = e->r; // no copy!
  cf->extRing  = R;

  /* propagate characteristic up so that it becomes
     directly accessible in cf: */
  cf->ch = R->cf->ch;

  cf->is_field=TRUE;
  cf->is_domain=TRUE;
  cf->rep=n_rep_poly;

  #ifdef LDEBUG
  p_Test((poly)naMinpoly, naRing);
  #endif

  cf->cfCoeffName    = naCoeffName;

  cf->cfGreaterZero  = naGreaterZero;
  cf->cfGreater      = naGreater;
  cf->cfEqual        = naEqual;
  cf->cfIsZero       = naIsZero;
  cf->cfIsOne        = naIsOne;
  cf->cfIsMOne       = naIsMOne;
  cf->cfInit         = naInit;
  cf->cfInitMPZ      = naInitMPZ;
  cf->cfFarey        = naFarey;
  cf->cfChineseRemainder= naChineseRemainder;
  cf->cfInt          = naInt;
  cf->cfInpNeg       = naNeg;
  cf->cfAdd          = naAdd;
  cf->cfSub          = naSub;
  cf->cfMult         = naMult;
  cf->cfDiv          = naDiv;
  cf->cfExactDiv     = naDiv;
  cf->cfPower        = naPower;
  cf->cfCopy         = naCopy;

  cf->cfWriteLong        = naWriteLong;

  if( rCanShortOut(naRing) )
    cf->cfWriteShort = naWriteShort;
  else
    cf->cfWriteShort = naWriteLong;

  cf->cfRead         = naRead;
  cf->cfDelete       = naDelete;
  cf->cfSetMap       = naSetMap;
  cf->cfGetDenom     = naGetDenom;
  cf->cfGetNumerator = naGetNumerator;
  cf->cfRePart       = naCopy;
  cf->cfCoeffWrite   = naCoeffWrite;
  cf->cfNormalize    = naNormalize;
  cf->cfKillChar     = naKillChar;
#ifdef LDEBUG
  cf->cfDBTest       = naDBTest;
#endif
  cf->cfGcd          = naGcd;
  cf->cfNormalizeHelper          = naLcmContent;
  cf->cfSize         = naSize;
  cf->nCoeffIsEqual  = naCoeffIsEqual;
  cf->cfInvers       = naInvers;
  cf->convFactoryNSingN=naConvFactoryNSingN;
  cf->convSingNFactoryN=naConvSingNFactoryN;
  cf->cfParDeg = naParDeg;

  cf->iNumberOfParameters = rVar(R);
  cf->pParameterNames = (const char**)R->names;
  cf->cfParameter = naParameter;
  cf->has_simple_Inverse= R->cf->has_simple_Inverse;
  /* cf->has_simple_Alloc= FALSE; */

  if( nCoeff_is_Q(R->cf) )
  {
    cf->cfClearContent = naClearContent;
    cf->cfClearDenominators = naClearDenominators;
  }

  return FALSE;
}

template class CRecursivePolyCoeffsEnumerator<NAConverter>;

template class IAccessor<snumber*>;

/* --------------------------------------------------------------------*/
/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/**
  * ABSTRACT: numbers as polys in the ring K[a]
  *           Assuming that we have a coeffs object cf, then these numbers
  *           are polynomials in the polynomial ring K[a] represented by
  *           cf->extRing.
  *           IMPORTANT ASSUMPTIONS:
  *           1.) So far we assume that cf->extRing is a valid polynomial
  *               ring
  **/

#ifdef LDEBUG
#define n2pTest(a) n2pDBTest(a,__FILE__,__LINE__,cf)
BOOLEAN  n2pDBTest(number a, const char *f, const int l, const coeffs r);
#else
#define n2pTest(a) do {} while (0)
#endif

/* polynomial ring in which our numbers live */
#define n2pRing cf->extRing

/* coeffs object in which the coefficients of our numbers live;
 * methods attached to n2pCoeffs may be used to compute with the
 * coefficients of our numbers, e.g., use n2pCoeffs->nAdd to add
 * coefficients of our numbers */
#define n2pCoeffs cf->extRing->cf

#ifdef LDEBUG
BOOLEAN n2pDBTest(number a, const char *f, const int l, const coeffs cf)
{
  if (a == NULL) return TRUE;
  return p_Test((poly)a, n2pRing);
}
#endif

void  n2pNormalize(number &a, const coeffs cf)
{
  poly aa=(poly)a;
  p_Normalize(aa,n2pRing);
}

/* TRUE iff (a != 0 and (b == 0 or deg(a) > deg(b) or (deg(a)==deg(b) && lc(a)>lc(b))) */
number n2pMult(number a, number b, const coeffs cf)
{
  n2pTest(a); n2pTest(b);
  if ((a == NULL)||(b == NULL)) return NULL;
  poly aTimesB = pp_Mult_qq((poly)a, (poly)b, n2pRing);
  return (number)aTimesB;
}

number n2pDiv(number a, number b, const coeffs cf)
{
  n2pTest(a); n2pTest(b);
  if (b == NULL) WerrorS(nDivBy0);
  if (a == NULL) return NULL;
  poly p=singclap_pdivide((poly)a,(poly)b,n2pRing);
  return (number)p;
}

void n2pPower(number a, int exp, number *b, const coeffs cf)
{
  n2pTest(a);

  *b= (number)p_Power((poly)a,exp,n2pRing);
}

const char * n2pRead(const char *s, number *a, const coeffs cf)
{
  poly aAsPoly;
  const char * result = p_Read(s, aAsPoly, n2pRing);
  *a = (number)aAsPoly;
  return result;
}

/* expects *param to be castable to AlgExtInfo */
static BOOLEAN n2pCoeffIsEqual(const coeffs cf, n_coeffType n, void * param)
{
  if (n_polyExt != n) return FALSE;
  AlgExtInfo *e = (AlgExtInfo *)param;
  /* for extension coefficient fields we expect the underlying
     polynomial rings to be IDENTICAL, i.e. the SAME OBJECT;
     this expectation is based on the assumption that we have properly
     registered cf and perform reference counting rather than creating
     multiple copies of the same coefficient field/domain/ring */
  if (n2pRing == e->r)
    return TRUE;
  // NOTE: Q(a)[x] && Q(a)[y] should better share the _same_ Q(a)...
  if( rEqual(n2pRing, e->r, TRUE) ) // also checks the equality of qideals
  {
    rDelete(e->r);
    return TRUE;
  }
  return FALSE;
}

char* n2pCoeffName(const coeffs cf)
{
  const char* const* p=n_ParameterNames(cf);
  int l=0;
  int i;
  for(i=0; i<rVar(n2pRing);i++)
  {
    l+=(strlen(p[i])+1);
  }
  char *cf_s=nCoeffName(n2pRing->cf);
  STATIC_VAR char s[200];
  s[0]='\0';
  snprintf(s,strlen(cf_s)+2,"%s",cf_s);
  char tt[2];
  tt[0]='[';
  tt[1]='\0';
  strcat(s,tt);
  tt[0]=',';
  for(i=0; i<rVar(n2pRing);i++)
  {
    strcat(s,p[i]);
    if (i+1!=rVar(n2pRing)) strcat(s,tt);
    else { tt[0]=']'; strcat(s,tt); }
  }
  return s;
}

void n2pCoeffWrite(const coeffs cf, BOOLEAN details)
{
  assume( cf != NULL );

  const ring A = cf->extRing;

  assume( A != NULL );
  PrintS("// polynomial ring as coefficient ring :\n");
  rWrite(A);
  PrintLn();
}

number  n2pInvers(number a, const coeffs cf)
{
  poly aa=(poly)a;
  if(p_IsConstant(aa, n2pRing))
  {
    poly p=p_Init(n2pRing);
    p_SetCoeff0(p,n_Invers(pGetCoeff(aa),n2pCoeffs),n2pRing);
    return (number)p;
  }
  else
  {
    WerrorS("not invertible");
    return NULL;
  }
}

BOOLEAN n2pInitChar(coeffs cf, void * infoStruct)
{
  assume( infoStruct != NULL );

  AlgExtInfo *e = (AlgExtInfo *)infoStruct;
  /// first check whether cf->extRing != NULL and delete old ring???

  assume(e->r                     != NULL);      // extRing;
  assume(e->r->cf                 != NULL);      // extRing->cf;

  assume( cf != NULL );

  rIncRefCnt(e->r); // increase the ref.counter for the ground poly. ring!
  const ring R = e->r; // no copy!
  cf->extRing  = R;

  /* propagate characteristic up so that it becomes
     directly accessible in cf: */
  cf->ch = R->cf->ch;
  cf->is_field=FALSE;
  cf->is_domain=TRUE;

  cf->cfCoeffName    = n2pCoeffName;

  cf->cfGreaterZero  = naGreaterZero;
  cf->cfGreater      = naGreater;
  cf->cfEqual        = naEqual;
  cf->cfIsZero       = naIsZero;
  cf->cfIsOne        = naIsOne;
  cf->cfIsMOne       = naIsMOne;
  cf->cfInit         = naInit;
  cf->cfInitMPZ      = naInitMPZ;
  cf->cfFarey        = naFarey;
  cf->cfChineseRemainder= naChineseRemainder;
  cf->cfInt          = naInt;
  cf->cfInpNeg       = naNeg;
  cf->cfAdd          = naAdd;
  cf->cfSub          = naSub;
  cf->cfMult         = n2pMult;
  cf->cfDiv          = n2pDiv;
  cf->cfPower        = n2pPower;
  cf->cfCopy         = naCopy;

  cf->cfWriteLong        = naWriteLong;

  if( rCanShortOut(n2pRing) )
    cf->cfWriteShort = naWriteShort;
  else
    cf->cfWriteShort = naWriteLong;

  cf->cfRead         = n2pRead;
  cf->cfDelete       = naDelete;
  cf->cfSetMap       = naSetMap;
  cf->cfGetDenom     = naGetDenom;
  cf->cfGetNumerator = naGetNumerator;
  cf->cfRePart       = naCopy;
  cf->cfCoeffWrite   = n2pCoeffWrite;
  cf->cfNormalize    = n2pNormalize;
  cf->cfKillChar     = naKillChar;
#ifdef LDEBUG
  cf->cfDBTest       = naDBTest;
#endif
  cf->cfGcd          = naGcd;
  cf->cfNormalizeHelper          = naLcmContent;
  cf->cfSize         = naSize;
  cf->nCoeffIsEqual  = n2pCoeffIsEqual;
  cf->cfInvers       = n2pInvers;
  cf->convFactoryNSingN=naConvFactoryNSingN;
  cf->convSingNFactoryN=naConvSingNFactoryN;
  cf->cfParDeg = naParDeg;

  cf->iNumberOfParameters = rVar(R);
  cf->pParameterNames = (const char**)R->names;
  cf->cfParameter = naParameter;
  cf->has_simple_Inverse=FALSE;
  /* cf->has_simple_Alloc= FALSE; */

  if( nCoeff_is_Q(R->cf) )
  {
    cf->cfClearContent = naClearContent;
    cf->cfClearDenominators = naClearDenominators;
  }

  return FALSE;
}
