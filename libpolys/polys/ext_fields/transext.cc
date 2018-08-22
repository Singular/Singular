/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/*
* ABSTRACT: numbers in a rational function field K(t_1, .., t_s) with
*           transcendental variables t_1, ..., t_s, where s >= 1.
*           Denoting the implemented coeffs object by cf, then these numbers
*           are represented as quotients of polynomials living in the
*           polynomial ring K[t_1, .., t_s] represented by cf->extRing.
*
*           An element of K(t_1, .., t_s) may have numerous representations,
*           due to the possibility of common polynomial factors in the
*           numerator and denominator. This problem is handled by a
*           cancellation heuristic: Each number "knows" its complexity
*           which is 0 if and only if common factors have definitely been
*           cancelled, and some positive integer otherwise.
*           Each arithmetic operation of two numbers with complexities c1
*           and c2 will result in a number of complexity c1 + c2 + some
*           penalty (specific for each arithmetic operation; see constants
*           in the *.h file). Whenever the resulting complexity exceeds a
*           certain threshold (see constant in the *.h file), then the
*           cancellation heuristic will call 'factory' to compute the gcd
*           and cancel it out in the given number.
*           For the special case of K = Q (i.e., when computing over the
*           rationals), this definite cancellation procedure will also take
*           care of nested fractions: If there are fractional coefficients
*           in the numerator or denominator of a number, then this number
*           is being replaced by a quotient of two polynomials over Z, or
*           - if the denominator is a constant - by a polynomial over Q.
*
*           TODO: the description above needs a major update!!!
*/
#define TRANSEXT_PRIVATES

#include "misc/auxiliary.h"

#include "factory/factory.h"

#include "reporter/reporter.h"

#include "coeffs/coeffs.h"
#include "coeffs/numbers.h"

#include "coeffs/longrat.h"

#include "polys/monomials/ring.h"
#include "polys/monomials/p_polys.h"
#include "polys/simpleideals.h"

#include "polys/clapsing.h"
#include "polys/clapconv.h"

#include "polys/prCopy.h"
#include "transext.h"
#include "algext.h"

#include "polys/PolyEnumerator.h"


/* constants for controlling the complexity of numbers */
#define ADD_COMPLEXITY 1   /**< complexity increase due to + and - */
#define MULT_COMPLEXITY 2   /**< complexity increase due to * and / */
#define DIFF_COMPLEXITY 2   /**< complexity increase due to diff */
#define BOUND_COMPLEXITY 10   /**< maximum complexity of a number */

/// TRUE iff num. represents 1
#define NUMIS1(f) (p_IsOne(NUM(f), cf->extRing))

#define COM(f) (f)->complexity


#ifdef LDEBUG
static BOOLEAN  ntDBTest(number a, const char *f, const int l, const coeffs r);
#endif

#define ntTest(a) n_Test(a, cf)

/* polynomial ring in which the numerators and denominators of our
   numbers live */
#define ntRing cf->extRing

/* coeffs object in which the coefficients of our numbers live;
 * methods attached to ntCoeffs may be used to compute with the
 * coefficients of our numbers, e.g., use ntCoeffs->nAdd to add
 * coefficients of our numbers */
#define ntCoeffs cf->extRing->cf


omBin fractionObjectBin = omGetSpecBin(sizeof(fractionObject));

/// forward declarations
static void heuristicGcdCancellation(number a, const coeffs cf);
static void definiteGcdCancellation(number a, const coeffs cf,
                             BOOLEAN simpleTestsHaveAlreadyBeenPerformed);

/* test routine, usualy disabled *
 * if want to activate it, activate also the calls to check_N *
 *
void check_normalized(number t,const coeffs cf, const char *f, int l)
{
  if (IS0(t)) return;
  if(rField_is_Q(ntRing))
  {
    poly pp=NUM(t);
    while(pp!=NULL)
    {
      if (((SR_HDL(pGetCoeff(pp)) & SR_INT)==0)&&(SR_HDL(pGetCoeff(pp))!=NULL))
      {
        if (pGetCoeff(pp)->s==0)
        {
          Print("NUM not normalized in %s:%d\n",f,l);
          p_Normalize(pp,ntRing);
        }
        else if (pGetCoeff(pp)->s==1)
          Print("NUM is rational in %s:%d\n",f,l);
      }
      pIter(pp);
    }
    pp=DEN(t);
    while(pp!=NULL)
    {
      if (((SR_HDL(pGetCoeff(pp)) & SR_INT)==0)&&(SR_HDL(pGetCoeff(pp))!=NULL))
      {
        if (pGetCoeff(pp)->s==0)
        {
          Print("NUM not normalized in %s:%d\n",f,l);
          p_Normalize(pp,ntRing);
        }
        else if (pGetCoeff(pp)->s==1)
          Print("DEN is rational in %s:%d\n",f,l);
      }
      pIter(pp);
    }
  }
}
#define check_N(A,B) check_normalized(A,B,__FILE__,__LINE__)
*/

#ifdef LDEBUG
static BOOLEAN ntDBTest(number a, const char *f, const int l, const coeffs cf)
{
  assume(getCoeffType(cf) == n_transExt);

  if (IS0(a)) return TRUE;

  const fraction t = (fraction)a;

  //check_N(a,cf);
  const poly num = NUM(t);
  assume(num != NULL);   ///< t != 0 ==> numerator(t) != 0

  p_Test(num, ntRing);

  if (getCoeffType(ntCoeffs)==n_Q)
    for( poly p = num; p != NULL; pIter(p) )
      if (! nlIsInteger( p_GetCoeff(p, ntRing), ntCoeffs) )
      {
        Print("ERROR in %s:%d: non-integer Q coeff in num. poly\n",f,l);
        Print("TERM: ");  p_wrp(p, ntRing); PrintLn();
        return FALSE;
      }

  const poly den = DEN(t);

  if (den != NULL) // !DENIS1(f)
  {
    p_Test(den, ntRing);

    if (getCoeffType(ntCoeffs)==n_Q)
      for( poly p = den; p != NULL; pIter(p) )
        if (! nlIsInteger( p_GetCoeff(p, ntRing), ntCoeffs) )
        {
          Print("ERROR in %s:%d: non-integer Q coeff in den. poly\n",f,l);
          Print("TERM: "); p_wrp(p, ntRing);  PrintLn();
          return FALSE;
        }

    if (getCoeffType(ntCoeffs)==n_Zp)
    {
      if( p_IsConstant(den, ntRing) )
      {
        Print("ERROR in %s:%d: constant den. poly / Zp\n",f,l);
        PrintS("NUM: ");  p_Write(num, ntRing);
        PrintS("DEN: ");  p_Write(den, ntRing);
        return FALSE;
      }

      if( !n_IsOne(pGetCoeff(den), ntCoeffs) )
      {
        Print("ERROR in %s:%d: non-monic den. poly / Zp\n",f,l);
        PrintS("NUM: ");  p_Write(num, ntRing);
        PrintS("DEN: ");  p_Write(den, ntRing);
        return FALSE;
      }
    }

    if (COM(t)==0)
    {
      poly gcd = singclap_gcd_r( num, den, ntRing );
      if(gcd!=NULL)
      {
        if((gcd!=NULL) && !p_IsOne(gcd, ntRing) )
        {
          Print("ERROR in %s:%d: 1 != GCD between num. & den. poly\n",f,l);
          PrintS("GCD: ");  p_Write(gcd, ntRing);
          PrintS("NUM: ");  p_Write(num, ntRing);
          PrintS("DEN: ");  p_Write(den, ntRing);
          return FALSE;
        }
        p_Delete( &gcd, ntRing );
      }
    }
    return TRUE;

    if(p_IsConstant(den, ntRing) && (n_IsOne(pGetCoeff(den), ntCoeffs)))
    {
      Print("?/1 in %s:%d\n",f,l);
      return FALSE;
    }
    if( !n_GreaterZero(pGetCoeff(den), ntCoeffs) )
    {
      Print("negative sign of DEN. of a fraction in %s:%d\n",f,l);
      return FALSE;
    }
    // test that den is over integers!?
  }
  else
  {
    return TRUE;

    // num != NULL // den == NULL
//    if( COM(t) != 0 )
//    {
//      Print("?//NULL with non-zero complexity: %d in %s:%d\n", COM(t), f, l);
//      return FALSE;
//    }
    // test that nume is over integers!?
  }
  if (getCoeffType(ntCoeffs)==n_Q)
  {
    poly p=num; // !=NULL
    do
    {
      number n=pGetCoeff(p);
      n_Test(n,ntCoeffs);
      if ((!(SR_HDL(n) & SR_INT))&&(n->s==0))
      /* not normalized, just do for the following test*/
      {
        n_Normalize(pGetCoeff(p),ntCoeffs);
        n=pGetCoeff(p);
      }
      if (!(SR_HDL(n) & SR_INT))
      {
        if (n->s<2)
          Print("rational coeff in num: %s:%d\n",f,l);
      }
      pIter(p);
    } while(p!=NULL);
    p=den;
    while(p!=NULL)
    {
      number n=pGetCoeff(p);
      if (!(SR_HDL(n) & SR_INT))
      {
        if (n->s!=3)
          Print("rational coeff in den.:%s:%d\n",f,l);
      }
      pIter(p);
    }
  }
  return TRUE;
}
#endif

poly gcd_over_Q ( poly f, poly g, const ring r)
{
  poly res;
  f=p_Copy(f,r);
  p_Cleardenom(f, r);
  g=p_Copy(g,r);
  p_Cleardenom(g, r);
  res=singclap_gcd_r(f,g,r);
  p_Delete(&f, r);
  p_Delete(&g, r);
  return res;
}

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

static BOOLEAN ntIsZero(number a, const coeffs cf)
{
  //check_N(a,cf);
  ntTest(a); // !!!
  return (IS0(a));
}

static void ntDelete(number * a, const coeffs cf)
{
  //check_N(*a,cf);
  ntTest(*a); // !!!

  fraction f = (fraction)(*a);
  if (IS0(f)) return;
  p_Delete(&NUM(f), ntRing);
  if (!DENIS1(f)) p_Delete(&DEN(f), ntRing);
  omFreeBin((ADDRESS)f, fractionObjectBin);
  *a = NULL;
}

static BOOLEAN ntEqual(number a, number b, const coeffs cf)
{
  //check_N(a,cf);
  //check_N(b,cf);
  ntTest(a);
  ntTest(b);

  /// simple tests
  if (a == b) return TRUE;
  if ((IS0(a)) && (!IS0(b))) return FALSE;
  if ((IS0(b)) && (!IS0(a))) return FALSE;

  /// cheap test if gcd's have been cancelled in both numbers
  fraction fa = (fraction)a;
  fraction fb = (fraction)b;
  if ((COM(fa) == 1) && (COM(fb) == 1))
  {
    poly f = p_Add_q(p_Copy(NUM(fa), ntRing),
                     p_Neg(p_Copy(NUM(fb), ntRing), ntRing),
                     ntRing);
    if (f != NULL) { p_Delete(&f, ntRing); return FALSE; }
    if (DENIS1(fa) && DENIS1(fb))  return TRUE;
    if (DENIS1(fa) && !DENIS1(fb)) return FALSE;
    if (!DENIS1(fa) && DENIS1(fb)) return FALSE;
    f = p_Add_q(p_Copy(DEN(fa), ntRing),
                p_Neg(p_Copy(DEN(fb), ntRing), ntRing),
                ntRing);
    if (f != NULL) { p_Delete(&f, ntRing); return FALSE; }
    return TRUE;
  }

  /* default: the more expensive multiplication test
              a/b = c/d  <==>  a*d = b*c */
  poly f = p_Copy(NUM(fa), ntRing);
  if (!DENIS1(fb)) f = p_Mult_q(f, p_Copy(DEN(fb), ntRing), ntRing);
  poly g = p_Copy(NUM(fb), ntRing);
  if (!DENIS1(fa)) g = p_Mult_q(g, p_Copy(DEN(fa), ntRing), ntRing);
  poly h = p_Add_q(f, p_Neg(g, ntRing), ntRing);
  if (h == NULL) return TRUE;
  else
  {
    p_Delete(&h, ntRing);
    return FALSE;
  }
}

static number ntCopy(number a, const coeffs cf)
{
  //check_N(a,cf);
  ntTest(a); // !!!
  if (IS0(a)) return NULL;
  fraction f = (fraction)a;
  poly g = NUM(f);
  poly h = NULL;
  h =DEN(f);
  fraction result = (fraction)omAlloc0Bin(fractionObjectBin);
  NUM(result) = p_Copy(g,cf->extRing);
  DEN(result) = p_Copy(h,cf->extRing);
  COM(result) = COM(f);
  ntTest((number)result);
  return (number)result;
}

/* assumes that cf represents the rationals, i.e. Q, and will only
   be called in that case;
   assumes furthermore that f != NULL and that the denominator of f != 1;
   generally speaking, this method removes denominators in the rational
   coefficients of the numerator and denominator of 'a';
   more concretely, the following normalizations will be performed,
   where t^alpha denotes a monomial in the transcendental variables t_k
   (1) if 'a' is of the form
          (sum_alpha a_alpha/b_alpha * t^alpha)
          -------------------------------------
            (sum_beta c_beta/d_beta * t^beta)
       with integers a_alpha, b_alpha, c_beta, d_beta, then both the
       numerator and the denominator will be multiplied by the LCM of
       the b_alpha's and the d_beta's (if this LCM is != 1),
   (2) if 'a' is - e.g. after having performed step (1) - of the form
          (sum_alpha a_alpha * t^alpha)
          -----------------------------
            (sum_beta c_beta * t^beta)
       with integers a_alpha, c_beta, and with a non-constant denominator,
       then both the numerator and the denominator will be divided by the
       GCD of the a_alpha's and the c_beta's (if this GCD is != 1),
   this procedure does not alter COM(f) (this has to be done by the
   calling procedure);
   modifies f */
static void handleNestedFractionsOverQ(fraction f, const coeffs cf)
{
  assume(nCoeff_is_Q(ntCoeffs));
  assume(!IS0(f));
  assume(!DENIS1(f));

  { /* step (1); see documentation of this procedure above */
    number lcmOfDenominators = n_Init(1, ntCoeffs);
    number c; number tmp;
    poly p = NUM(f);
    /* careful when using n_NormalizeHelper!!! It computes the lcm of the numerator
       of the 1st argument and the denominator of the 2nd!!! */
    while (p != NULL)
    {
      c = p_GetCoeff(p, ntRing);
      tmp = n_NormalizeHelper(lcmOfDenominators, c, ntCoeffs);
      n_Delete(&lcmOfDenominators, ntCoeffs);
      lcmOfDenominators = tmp;
      pIter(p);
    }
    p = DEN(f);
    while (p != NULL)
    {
      c = p_GetCoeff(p, ntRing);
      tmp = n_NormalizeHelper(lcmOfDenominators, c, ntCoeffs);
      n_Delete(&lcmOfDenominators, ntCoeffs);
      lcmOfDenominators = tmp;
      pIter(p);
    }
    if (!n_IsOne(lcmOfDenominators, ntCoeffs))
    { /* multiply NUM(f) and DEN(f) with lcmOfDenominators */
      NUM(f) = __p_Mult_nn(NUM(f), lcmOfDenominators, ntRing);
      p_Normalize(NUM(f), ntRing);
      DEN(f) = __p_Mult_nn(DEN(f), lcmOfDenominators, ntRing);
      p_Normalize(DEN(f), ntRing);
    }
    n_Delete(&lcmOfDenominators, ntCoeffs);
    if (DEN(f)!=NULL)
    { /* step (2); see documentation of this procedure above */
      p = NUM(f);
      number gcdOfCoefficients = n_Copy(p_GetCoeff(p, ntRing), ntCoeffs);
      pIter(p);
      while ((p != NULL) && (!n_IsOne(gcdOfCoefficients, ntCoeffs)))
      {
        c = p_GetCoeff(p, ntRing);
        tmp = n_Gcd(c, gcdOfCoefficients, ntCoeffs);
        n_Delete(&gcdOfCoefficients, ntCoeffs);
        gcdOfCoefficients = tmp;
        pIter(p);
      }
      p = DEN(f);
      while ((p != NULL) && (!n_IsOne(gcdOfCoefficients, ntCoeffs)))
      {
        c = p_GetCoeff(p, ntRing);
        tmp = n_Gcd(c, gcdOfCoefficients, ntCoeffs);
        n_Delete(&gcdOfCoefficients, ntCoeffs);
        gcdOfCoefficients = tmp;
        pIter(p);
      }
      if (!n_IsOne(gcdOfCoefficients, ntCoeffs))
      { /* divide NUM(f) and DEN(f) by gcdOfCoefficients */
        number inverseOfGcdOfCoefficients = n_Invers(gcdOfCoefficients,
                                                     ntCoeffs);
        NUM(f) = __p_Mult_nn(NUM(f), inverseOfGcdOfCoefficients, ntRing);
        p_Normalize(NUM(f), ntRing);
        DEN(f) = __p_Mult_nn(DEN(f), inverseOfGcdOfCoefficients, ntRing);
        p_Normalize(DEN(f), ntRing);
        n_Delete(&inverseOfGcdOfCoefficients, ntCoeffs);
      }
      n_Delete(&gcdOfCoefficients, ntCoeffs);
    }
  }

  /* Now, due to the above computations, DEN(f) may have become the
     1-polynomial which needs to be represented by NULL: */
  if ((DEN(f) != NULL) &&
      p_IsConstant(DEN(f), ntRing) &&
      n_IsOne(p_GetCoeff(DEN(f), ntRing), ntCoeffs))
  {
    p_Delete(&DEN(f), ntRing); DEN(f) = NULL;
  }

  if( DEN(f) != NULL )
    if( !n_GreaterZero(pGetCoeff(DEN(f)), ntCoeffs) )
    {
      NUM(f) = p_Neg(NUM(f), ntRing);
      DEN(f) = p_Neg(DEN(f), ntRing);
    }
  COM(f)=BOUND_COMPLEXITY+1;
  ntTest((number)f); // TODO!
}

/// TODO: normalization of a!?
static number ntGetNumerator(number &a, const coeffs cf)
{
  //check_N(a,cf);
  ntTest(a);
  if (IS0(a)) return NULL;

  definiteGcdCancellation(a, cf, FALSE);

  fraction f = (fraction)a;
  fraction result = (fraction)omAlloc0Bin(fractionObjectBin);

  const BOOLEAN denis1= DENIS1 (f);

  if (getCoeffType (ntCoeffs) == n_Q && !denis1)
    handleNestedFractionsOverQ (f, cf);

  if (getCoeffType (ntCoeffs) == n_Q && denis1)
  {
    assume( DEN (f) == NULL );

    number g;
    // TODO/NOTE: the following should not be necessary (due to
    // Hannes!) as NUM (f) should be over Z!!!
    CPolyCoeffsEnumerator itr(NUM(f));


    n_ClearDenominators(itr, g, ntCoeffs);

    if( !n_GreaterZero(g, ntCoeffs) )
    {
      NUM (f) = p_Neg(NUM (f), ntRing);
      g = n_InpNeg(g, ntCoeffs);
    }

    // g should be a positive integer now!
    assume( n_GreaterZero(g, ntCoeffs) );

    if( !n_IsOne(g, ntCoeffs) )
    {
      DEN (f) = p_NSet(g, ntRing);
      COM (f) ++;
      assume( DEN (f) != NULL );
    }
    else
      n_Delete(&g, ntCoeffs);

    ntTest(a);
  }

  // Call ntNormalize instead of above?!?

  NUM (result) = p_Copy (NUM (f), ntRing); // ???
  //DEN (result) = NULL; // done by ..Alloc0..
  //COM (result) = 0; // done by ..Alloc0..

  ntTest((number)result);
  //check_N((number)result,cf);
  return (number)result;
}

/// TODO: normalization of a!?
static number ntGetDenom(number &a, const coeffs cf)
{
  //check_N(a,cf);
  ntTest(a);

  fraction result = (fraction)omAlloc0Bin(fractionObjectBin);
  //DEN (result)= NULL; // done by ..Alloc0..
  //COM (result)= 0; // done by ..Alloc0..

  if (IS0(a))
  {
    NUM (result) = p_One(ntRing);
    return (number)result;
  }

  definiteGcdCancellation(a, cf, FALSE);

  fraction f = (fraction)a;

  assume( !IS0(f) );

  const BOOLEAN denis1 = DENIS1 (f);

  if( denis1 && (getCoeffType (ntCoeffs) != n_Q) ) // */1 or 0
  {
    NUM (result)= p_One(ntRing);
    ntTest((number)result);
    return (number)result;
  }

  if (!denis1) // */* / Q
  {
    assume( DEN (f) != NULL );

    if (getCoeffType (ntCoeffs) == n_Q)
      handleNestedFractionsOverQ (f, cf);

    ntTest(a);

    if( DEN (f) != NULL ) // is it ?? // 1 now???
    {
      assume( !p_IsOne(DEN (f), ntRing) );

      NUM (result) = p_Copy (DEN (f), ntRing);
      ntTest((number)result);
      return (number)result;
    }
//    NUM (result) = p_One(ntRing); // NOTE: just in order to be sure...
  }

  // */1 / Q
  assume( getCoeffType (ntCoeffs) == n_Q );
  assume( DEN (f) == NULL );

  number g;
//    poly num= p_Copy (NUM (f), ntRing); // ???


  // TODO/NOTE: the following should not be necessary (due to
  // Hannes!) as NUM (f) should be over Z!!!
  CPolyCoeffsEnumerator itr(NUM(f));

  n_ClearDenominators(itr, g, ntCoeffs); // may return -1 :(((

  if( !n_GreaterZero(g, ntCoeffs) )
  {
//     NUM (f) = p_Neg(NUM (f), ntRing); // Ugly :(((
//     g = n_InpNeg(g, ntCoeffs);
    NUM (f) = p_Neg(NUM (f), ntRing); // Ugly :(((
    g = n_InpNeg(g, ntCoeffs);
  }

  // g should be a positive integer now!
  assume( n_GreaterZero(g, ntCoeffs) );

  if( !n_IsOne(g, ntCoeffs) )
  {
    assume( n_GreaterZero(g, ntCoeffs) );
    assume( !n_IsOne(g, ntCoeffs) );

    DEN (f) = p_NSet(g, ntRing); // update COM(f)???
    assume( DEN (f) != NULL );
    COM (f) ++;

    NUM (result)= p_Copy (DEN (f), ntRing);
  }
  else
  { // common denom == 1?
    NUM (result)= p_NSet(g, ntRing); // p_Copy (DEN (f), ntRing);
//  n_Delete(&g, ntCoeffs);
  }

//    if (!p_IsConstant (num, ntRing) && pNext(num) != NULL)
//    else
//      g= p_GetAllDenom (num, ntRing);
//    result= (fraction) ntSetMap (ntCoeffs, cf) (g, ntCoeffs, cf);

  ntTest((number)result);
  //check_N((number)result,cf);
  return (number)result;
}

static BOOLEAN ntIsOne(number a, const coeffs cf)
{
  //check_N(a,cf);
  ntTest(a); // !!!
  definiteGcdCancellation(a, cf, FALSE);
  fraction f = (fraction)a;
  return (f!=NULL) && DENIS1(f) && NUMIS1(f);
}

static BOOLEAN ntIsMOne(number a, const coeffs cf)
{
  //check_N(a,cf);
  ntTest(a);
  definiteGcdCancellation(a, cf, FALSE);
  fraction f = (fraction)a;
  if ((f==NULL) || (!DENIS1(f))) return FALSE;
  poly g = NUM(f);
  if (!p_IsConstant(g, ntRing)) return FALSE;
  return n_IsMOne(p_GetCoeff(g, ntRing), ntCoeffs);
}

/// this is in-place, modifies a
static number ntNeg(number a, const coeffs cf)
{
  //check_N(a,cf);
  ntTest(a);
  if (!IS0(a))
  {
    fraction f = (fraction)a;
    NUM(f) = p_Neg(NUM(f), ntRing);
  }
  ntTest(a);
  return a;
}

number ntInit(long i, const coeffs cf)
{
  if (i != 0)
  {
    poly p=p_ISet(i, ntRing);
    if (p!=NULL)
    {
      fraction result = (fraction)omAlloc0Bin(fractionObjectBin);
      NUM(result) = p;
      //DEN(result) = NULL; // done by omAlloc0Bin
      //COM(result) = 0; // done by omAlloc0Bin
      ntTest((number)result);
      //check_N((number)result,cf);
      return (number)result;
    }
  }
  return NULL;
}


// takes over p!
number ntInit(poly p, const coeffs cf)
{
  if (p == NULL) return NULL;

  p_Test( p, ntRing);
  fraction f = (fraction)omAlloc0Bin(fractionObjectBin);

  if (nCoeff_is_Q(ntCoeffs))
  {
    number g;
    // the following is necessary because
    // NUM (f) should be over Z,
    // while p may be over Q
    CPolyCoeffsEnumerator itr(p);

    n_ClearDenominators(itr, g, ntCoeffs);

    if( !n_GreaterZero(g, ntCoeffs) )
    {
      p = p_Neg(p, ntRing);
      g = n_InpNeg(g, ntCoeffs);
    }

    // g should be a positive integer now!
    assume( n_GreaterZero(g, ntCoeffs) );

    if( !n_IsOne(g, ntCoeffs) )
    {
      DEN (f) = p_NSet(g, ntRing);
      p_Normalize(DEN(f), ntRing);
      assume( DEN (f) != NULL );
    }
    else
    {
      //DEN(f) = NULL; // done by omAlloc0
      n_Delete(&g, ntCoeffs);
    }
  }

  p_Normalize(p, ntRing);
  NUM(f) = p;
  //COM(f) = 0; // done by omAlloc0

  //check_N((number)f,cf);
  ntTest((number)f);
  return (number)f;
}

static long ntInt(number &a, const coeffs cf)
{
  //check_N(a,cf);
  ntTest(a);
  if (IS0(a)) return 0;
  definiteGcdCancellation(a, cf, FALSE);
  fraction f = (fraction)a;
  if (!DENIS1(f)) return 0;

  const poly aAsPoly = NUM(f);

  if(aAsPoly == NULL)
    return 0;

  if (!p_IsConstant(aAsPoly, ntRing))
    return 0;

  assume( aAsPoly != NULL );

  return n_Int(p_GetCoeff(aAsPoly, ntRing), ntCoeffs);
}

/* return FALSE, if a is a constant <=0 */
static BOOLEAN ntGreaterZero(number a, const coeffs cf)
{
  //check_N(a,cf);
  ntTest(a);
  if (IS0(a)) return FALSE;
  fraction f = (fraction)a;
  poly g = NUM(f);
  return (!p_LmIsConstant(g,ntRing)|| n_GreaterZero(pGetCoeff(g), ntCoeffs));
}

static BOOLEAN ntGreater(number a, number b, const coeffs cf)
{
  //check_N(a,cf);
  //check_N(b,cf);
  ntTest(a);
  ntTest(b);
  if (IS0(a))
  {
    if (IS0(b)) return FALSE;
    fraction fb = (fraction)b;
    return (!n_GreaterZero(pGetCoeff(NUM(fb)), ntCoeffs));
  }
  if (IS0(b))
  {
    fraction fa = (fraction)a;
    return (n_GreaterZero(pGetCoeff(NUM(fa)), ntCoeffs));
  }
  // now: a!=0, b!=0
  fraction fa = (fraction)a;
  number aNumCoeff = p_GetCoeff(NUM(fa), ntRing);
  int aNumDeg = p_Totaldegree(NUM(fa), ntRing);
  number aDenCoeff = NULL; int aDenDeg = 0;
  if (DEN(fa)!=NULL)
  {
    aDenCoeff=p_GetCoeff(DEN(fa),ntRing);
    aDenDeg = p_Totaldegree(DEN(fa), ntRing);
  }
  fraction fb = (fraction)b;
  number bNumCoeff = p_GetCoeff(NUM(fb), ntRing);
  int bNumDeg = p_Totaldegree(NUM(fb), ntRing);
  number bDenCoeff = NULL; int bDenDeg = 0;
  if (DEN(fb)!=NULL)
  {
    bDenCoeff=p_GetCoeff(DEN(fb),ntRing);
    bDenDeg = p_Totaldegree(DEN(fb), ntRing);
  }
  if (aNumDeg-aDenDeg > bNumDeg-bDenDeg) return TRUE;
  if (aNumDeg-aDenDeg < bNumDeg-bDenDeg) return FALSE;
  number aa;
  number bb;
  if (bDenCoeff==NULL) aa=n_Copy(aNumCoeff,ntCoeffs);
  else                 aa=n_Mult(aNumCoeff,bDenCoeff,ntCoeffs);
  if (aDenCoeff==NULL) bb=n_Copy(bNumCoeff,ntCoeffs);
  else                 bb=n_Mult(bNumCoeff,aDenCoeff,ntCoeffs);
  BOOLEAN rr= n_Greater(aa, bb, ntCoeffs);
  n_Delete(&aa,ntCoeffs);
  n_Delete(&bb,ntCoeffs);
  return rr;
}

static void ntCoeffWrite(const coeffs cf, BOOLEAN details)
{
  assume( cf != NULL );

  const ring A = cf->extRing;

  assume( A != NULL );
  assume( A->cf != NULL );

  n_CoeffWrite(A->cf, details);

//  rWrite(A);

  const int P = rVar(A);
  assume( P > 0 );

  PrintS("(");

  for (int nop=0; nop < P; nop ++)
  {
    Print("%s", rRingVar(nop, A));
    if (nop!=P-1) PrintS(", ");
  }

  PrintS(")");

  assume( A->qideal == NULL );

/*
  PrintS("//   Coefficients live in the rational function field\n");
  Print("//   K(");
  for (int i = 0; i < rVar(ntRing); i++)
  {
    if (i > 0) PrintS(" ");
    Print("%s", rRingVar(i, ntRing));
  }
  PrintS(") with\n");
  PrintS("//   K: "); n_CoeffWrite(cf->extRing->cf);
*/
}

number ntDiff(number a, number d, const coeffs cf)
{
  //check_N(a,cf);
  //check_N(d,cf);
  ntTest(a);
  ntTest(d);

  if (IS0(d))
  {
    WerrorS("ringvar expected");
    return NULL;
  }
  fraction t = (fraction) d;
  if (!DENIS1(t))
  {
    WerrorS("expected differentiation by a variable");
    return NULL;
  }
  int k=p_Var(NUM(t),ntRing);
  if (k==0)
  {
    WerrorS("expected differentiation by a variable");
    return NULL;
  }

  if (IS0(a)) return ntCopy(a, cf);

  fraction fa = (fraction)a;
  fraction result = (fraction)omAlloc0Bin(fractionObjectBin);
  if (DENIS1(fa))
  {
     NUM(result) = p_Diff(NUM(fa),k,ntRing);
     //DEN(result) = NULL; // done by ..Alloc0..
     if (NUM(result)==NULL)
     {
       omFreeBin((ADDRESS)result, fractionObjectBin);
       return(NULL);
     }
     COM(result) = COM(fa)+DIFF_COMPLEXITY;
     //check_N((number)result,cf);
     ntTest((number)result);
     return (number)result;
  }

  poly fg = p_Mult_q(p_Copy(DEN(fa),ntRing),p_Diff(NUM(fa),k,ntRing),ntRing);
  poly gf = p_Mult_q(p_Copy(NUM(fa),ntRing),p_Diff(DEN(fa),k,ntRing),ntRing);
  NUM(result) = p_Sub(fg,gf,ntRing);
  if (NUM(result)==NULL) return(NULL);
  DEN(result) = pp_Mult_qq(DEN(fa), DEN(fa), ntRing);
  COM(result) = COM(fa) + COM(fa) + DIFF_COMPLEXITY;
  heuristicGcdCancellation((number)result, cf);

  //check_N((number)result,cf);
  ntTest((number)result);
  return (number)result;
}

static number ntAdd(number a, number b, const coeffs cf)
{
  //check_N(a,cf);
  //check_N(b,cf);
  ntTest(a);
  ntTest(b);
  if (IS0(a)) return ntCopy(b, cf);
  if (IS0(b)) return ntCopy(a, cf);

  fraction fa = (fraction)a;
  fraction fb = (fraction)b;

  poly g = p_Copy(NUM(fa), ntRing);
  if (!DENIS1(fb)) g = p_Mult_q(g, p_Copy(DEN(fb), ntRing), ntRing);
  poly h = p_Copy(NUM(fb), ntRing);
  if (!DENIS1(fa)) h = p_Mult_q(h, p_Copy(DEN(fa), ntRing), ntRing);
  g = p_Add_q(g, h, ntRing);

  if (g == NULL) return NULL;

  poly f;
  if      (DENIS1(fa) && DENIS1(fb))  f = NULL;
  else if (!DENIS1(fa) && DENIS1(fb)) f = p_Copy(DEN(fa), ntRing);
  else if (DENIS1(fa) && !DENIS1(fb)) f = p_Copy(DEN(fb), ntRing);
  else /* both denom's are != 1 */    f = p_Mult_q(p_Copy(DEN(fa), ntRing),
                                                   p_Copy(DEN(fb), ntRing),
                                                   ntRing);

  fraction result = (fraction)omAllocBin(fractionObjectBin);
  NUM(result) = g;
  DEN(result) = f;
  COM(result) = COM(fa) + COM(fb) + ADD_COMPLEXITY;
  heuristicGcdCancellation((number)result, cf);

//  ntTest((number)result);

  //check_N((number)result,cf);
  ntTest((number)result);
  return (number)result;
}

static number ntSub(number a, number b, const coeffs cf)
{
  //check_N(a,cf);
  //check_N(b,cf);
  ntTest(a);
  ntTest(b);
  if (IS0(a)) return ntNeg(ntCopy(b, cf), cf);
  if (IS0(b)) return ntCopy(a, cf);

  fraction fa = (fraction)a;
  fraction fb = (fraction)b;

  poly g = p_Copy(NUM(fa), ntRing);
  if (!DENIS1(fb)) g = p_Mult_q(g, p_Copy(DEN(fb), ntRing), ntRing);
  poly h = p_Copy(NUM(fb), ntRing);
  if (!DENIS1(fa)) h = p_Mult_q(h, p_Copy(DEN(fa), ntRing), ntRing);
  g = p_Add_q(g, p_Neg(h, ntRing), ntRing);

  if (g == NULL) return NULL;

  poly f;
  if      (DENIS1(fa) && DENIS1(fb))  f = NULL;
  else if (!DENIS1(fa) && DENIS1(fb)) f = p_Copy(DEN(fa), ntRing);
  else if (DENIS1(fa) && !DENIS1(fb)) f = p_Copy(DEN(fb), ntRing);
  else /* both den's are != 1 */      f = p_Mult_q(p_Copy(DEN(fa), ntRing),
                                                   p_Copy(DEN(fb), ntRing),
                                                   ntRing);

  fraction result = (fraction)omAllocBin(fractionObjectBin);
  NUM(result) = g;
  DEN(result) = f;
  COM(result) = COM(fa) + COM(fb) + ADD_COMPLEXITY;
  heuristicGcdCancellation((number)result, cf);
//  ntTest((number)result);
  //check_N((number)result,cf);
  ntTest((number)result);
  return (number)result;
}

static number ntMult(number a, number b, const coeffs cf)
{
  //check_N(a,cf);
  //check_N(b,cf);
  ntTest(a); // !!!?
  ntTest(b); // !!!?

  if (IS0(a) || IS0(b)) return NULL;

  fraction fa = (fraction)a;
  fraction fb = (fraction)b;

  const poly g = pp_Mult_qq(NUM(fa), NUM(fb), ntRing);

  if (g == NULL) return NULL; // may happen due to zero divisors???

  fraction result = (fraction)omAllocBin(fractionObjectBin);

  NUM(result) = g;

  const poly da = DEN(fa);
  const poly db = DEN(fb);


  //check_N((number)result,cf);
  if (db == NULL)
  {
    // b = ? // NULL

    if(da == NULL)
    { // both fa && fb are ?? // NULL!
      DEN(result) = NULL;
      COM(result) = 0;
      p_Normalize(g,ntRing);
    }
    else
    {
      DEN(result) = p_Copy(da, ntRing);
      COM(result) = COM(fa) + MULT_COMPLEXITY;
      heuristicGcdCancellation((number)result, cf);
      //check_N((number)result,cf);
    }
  }
  else
  { // b = ?? / ??
    if (da == NULL)
    { // a == ? // NULL
      DEN(result) = p_Copy(db, ntRing);
      COM(result) = COM(fb) + MULT_COMPLEXITY;
      heuristicGcdCancellation((number)result, cf);
      //check_N((number)result,cf);
    }
    else /* both den's are != 1 */
    {
      DEN(result) = pp_Mult_qq(da, db, ntRing);
      COM(result) = COM(fa) + COM(fb) + MULT_COMPLEXITY;
      heuristicGcdCancellation((number)result, cf);
      //check_N((number)result,cf);
    }
  }

//  ntTest((number)result);

  //check_N((number)result,cf);
  ntTest((number)result);
  return (number)result;
}

static void ntNormalizeDen(fraction result, const ring R)
{
  if ((nCoeff_has_simple_inverse(R->cf))
  && (result!=NULL)
  && (DEN(result)!=NULL))
  {
    poly n=DEN(result);
    if (!n_IsOne(pGetCoeff(n),R->cf))
    {
      number inv=n_Invers(pGetCoeff(n),R->cf);
      DEN(result)=__p_Mult_nn(n,inv,R);
      NUM(result)=__p_Mult_nn(NUM(result),inv,R);
      n_Delete(&inv,R->cf);
      if (p_IsOne(DEN(result), R))
      {
        n=DEN(result);
        DEN(result)=NULL;
        COM(result) = 0;
        p_Delete(&n,R);
      }
    }
  }
}

static number ntDiv(number a, number b, const coeffs cf)
{
  //check_N(a,cf);
  //check_N(b,cf);
  ntTest(a);
  ntTest(b);
  if (IS0(a)) return NULL;
  if (IS0(b)) WerrorS(nDivBy0);

  fraction fa = (fraction)a;
  fraction fb = (fraction)b;

  poly g = p_Copy(NUM(fa), ntRing);
  if (!DENIS1(fb)) g = p_Mult_q(g, p_Copy(DEN(fb), ntRing), ntRing);

  if (g == NULL) return NULL;   /* may happen due to zero divisors */

  poly f = p_Copy(NUM(fb), ntRing);
  if (!DENIS1(fa)) f = p_Mult_q(f, p_Copy(DEN(fa), ntRing), ntRing);

  fraction result = (fraction)omAlloc0Bin(fractionObjectBin);
  NUM(result) = g;
  if (!n_GreaterZero(pGetCoeff(f),ntCoeffs))
  {
    g=p_Neg(g,ntRing);
    f=p_Neg(f,ntRing);
    NUM(result) = g;
  }
  if (!p_IsConstant(f,ntRing) || !n_IsOne(pGetCoeff(f),ntCoeffs))
  {
    DEN(result) = f;
  }
  else
  {
    p_Delete(&f, ntRing);
  }
  COM(result) = COM(fa) + COM(fb) + MULT_COMPLEXITY;
//  definiteGcdCancellation((number)result, cf,FALSE);
  heuristicGcdCancellation((number)result, cf);
//  ntTest((number)result);
  //check_N((number)result,cf);
  ntNormalizeDen(result,ntRing);
  ntTest((number)result);
  return (number)result;
}

static number ntInvers(number a, const coeffs cf)
{
  //check_N(a,cf);
  ntTest(a);
  if (IS0(a))
  {
    WerrorS(nDivBy0);
    return NULL;
  }
  fraction f = (fraction)a;
  assume( f != NULL );

  fraction result = (fraction)omAlloc0Bin(fractionObjectBin);

  assume( NUM(f) != NULL );
  const poly den = DEN(f);

  if (den == NULL)
    NUM(result) = p_One(ntRing);
  else
    NUM(result) = p_Copy(den, ntRing);

  if( !NUMIS1(f) )
  {
    poly num_f=NUM(f);
    BOOLEAN neg= !n_GreaterZero(pGetCoeff(num_f),ntCoeffs);
    if (neg)
    {
      num_f=p_Neg(p_Copy(num_f, ntRing), ntRing);
      NUM(result)=p_Neg(NUM(result), ntRing);
    }
    else
    {
      num_f=p_Copy(num_f, ntRing);
    }
    DEN(result) = num_f;
    COM(result) = COM(f);
    if (neg)
    {
      if (p_IsOne(num_f, ntRing))
      {
        DEN(result)=NULL;
        //COM(result) = 0;
        p_Delete(&num_f,ntRing);
      }
    }
  }
  //else// Alloc0
  //{
  //  DEN(result) = NULL;
  //  COM(result) = 0;
  //}
  ntNormalizeDen(result,ntRing);
  ntTest((number)result); // !!!!
  //check_N((number)result,cf);
  return (number)result;
}

/* 0^0 = 0;
   for |exp| <= 7 compute power by a simple multiplication loop;
   for |exp| >= 8 compute power along binary presentation of |exp|, e.g.
      p^13 = p^1 * p^4 * p^8, where we utilise that
      p^(2^(k+1)) = p^(2^k) * p^(2^k);
   intermediate cancellation is controlled by the in-place method
   heuristicGcdCancellation; see there.
*/
static void ntPower(number a, int exp, number *b, const coeffs cf)
{
  ntTest(a);

  /* special cases first */
  if (IS0(a))
  {
    if (exp >= 0) *b = NULL;
    else          WerrorS(nDivBy0);
  }
  else if (exp ==  0) { *b = ntInit(1, cf); return;}
  else if (exp ==  1) { *b = ntCopy(a, cf); return;}
  else if (exp == -1) { *b = ntInvers(a, cf); return;}

  int expAbs = exp; if (expAbs < 0) expAbs = -expAbs;

  /* now compute a^expAbs */
  number pow; number t;
  if (expAbs <= 7)
  {
    pow = ntCopy(a, cf);
    for (int i = 2; i <= expAbs; i++)
    {
      t = ntMult(pow, a, cf);
      ntDelete(&pow, cf);
      pow = t;
      heuristicGcdCancellation(pow, cf);
    }
  }
  else
  {
    pow = ntInit(1, cf);
    number factor = ntCopy(a, cf);
    while (expAbs != 0)
    {
      if (expAbs & 1)
      {
        t = ntMult(pow, factor, cf);
        ntDelete(&pow, cf);
        pow = t;
        heuristicGcdCancellation(pow, cf);
      }
      expAbs = expAbs / 2;
      if (expAbs != 0)
      {
        t = ntMult(factor, factor, cf);
        ntDelete(&factor, cf);
        factor = t;
        heuristicGcdCancellation(factor, cf);
      }
    }
    ntDelete(&factor, cf);
  }

  /* invert if original exponent was negative */
  if (exp < 0)
  {
    t = ntInvers(pow, cf);
    ntDelete(&pow, cf);
    pow = t;
  }
  *b = pow;
  ntTest(*b);
  //check_N(*b,cf);
}

/* modifies a */
/* this is an intermediate simplification routine - not a comple "normalize" */
static void heuristicGcdCancellation(number a, const coeffs cf)
{
  if (IS0(a)) return;

  fraction f = (fraction)a;
  p_Normalize(NUM(f),ntRing);
  if (DENIS1(f) || NUMIS1(f)) { COM(f) = 0; return; }

  assume( DEN(f) != NULL );
  p_Normalize(DEN(f),ntRing);

  /* check whether NUM(f) = DEN(f), and - if so - replace 'a' by 1 */
  if (p_EqualPolys(NUM(f), DEN(f), ntRing))
  { /* numerator and denominator are both != 1 */
    p_Delete(&NUM(f), ntRing); NUM(f) = p_ISet(1, ntRing);
    p_Delete(&DEN(f), ntRing); DEN(f) = NULL;
    COM(f) = 0;
  }
  else
  {
    if (COM(f) > BOUND_COMPLEXITY)
      definiteGcdCancellation(a, cf, TRUE);

    // TODO: check if it is enough to put the following into definiteGcdCancellation?!
    if( DEN(f) != NULL )
    {
      if( !n_GreaterZero(pGetCoeff(DEN(f)), ntCoeffs) )
      {
        NUM(f) = p_Neg(NUM(f), ntRing);
        DEN(f) = p_Neg(DEN(f), ntRing);
      }
      if (ntCoeffs->has_simple_Inverse)
      {
        if (!n_IsOne(pGetCoeff(DEN(f)),ntCoeffs))
        {
          number inv=n_Invers(pGetCoeff(DEN(f)),ntCoeffs);
          DEN(f)=__p_Mult_nn(DEN(f),inv,ntRing);
          NUM(f)=__p_Mult_nn(NUM(f),inv,ntRing);
        }
        if(p_LmIsConstant(DEN(f),ntRing))
        {
          p_Delete(&DEN(f),ntRing);
          COM(f)=0;
        }
      }
      if ((DEN(f)!=NULL)
      && (pNext(DEN(f))==NULL))
      {
        poly den_f=DEN(f);
        poly h=NUM(f);
        loop
        {
          if (h==NULL)
          {
            h=NUM(f);
            do
            {
              p_ExpVectorDiff(h,h,den_f,ntRing);
              pIter(h);
            } while(h!=NULL);
            p_ExpVectorDiff(den_f,den_f,den_f,ntRing);
            break;
          }
          int i=0;
          do
          {
            i++;
            if (p_GetExp(den_f,i,ntRing) > p_GetExp(h,i,ntRing)) return;
          } while(i<ntRing->N);
          pIter(h);
        }
      }
    }
  }
  if ((DEN(f)!=NULL)
  && (pNext(DEN(f))==NULL)
  && (p_LmIsConstantComp(DEN(f),ntRing))
  && (n_IsOne(pGetCoeff(DEN(f)),ntCoeffs)))
  {
     p_Delete(&DEN(f),ntRing);
     COM(f)=0;
  }
}

/// modifies a
static void definiteGcdCancellation(number a, const coeffs cf,
                             BOOLEAN simpleTestsHaveAlreadyBeenPerformed)
{
//  ntTest(a); // !!!!

  fraction f = (fraction)a;

  if (IS0(a)) return;
  if (COM(f)==0) return;
  if (DENIS1(f) || NUMIS1(f)) { COM(f) = 0; ntTest(a); return; }
  if (!simpleTestsHaveAlreadyBeenPerformed)
  {

    /* check whether NUM(f) = DEN(f), and - if so - replace 'a' by 1 */
    if (p_EqualPolys(NUM(f), DEN(f), ntRing))
    { /* numerator and denominator are both != 1 */
      p_Delete(&NUM(f), ntRing); NUM(f) = p_ISet(1, ntRing);
      p_Delete(&DEN(f), ntRing); DEN(f) = NULL;
      COM(f) = 0;
      ntTest(a);
      return;
    }
  }
  /*if (rField_is_Q(ntRing))
  {
    number c=n_Copy(pGetCoeff(NUM(f)),ntCoeffs);
    poly p=pNext(NUM(f));
    while((p!=NULL)&&(!n_IsOne(c,ntCoeffs)))
    {
      number cc=n_Gcd(c,pGetCoeff(p),ntCoeffs);
      n_Delete(&c,ntCoeffs);
      c=cc;
      pIter(p);
    };
    p=DEN(f);
    while((p!=NULL)&&(!n_IsOne(c,ntCoeffs)))
    {
      number cc=n_Gcd(c,pGetCoeff(p),ntCoeffs);
      n_Delete(&c,ntCoeffs);
      c=cc;
      pIter(p);
    };
    if(!n_IsOne(c,ntCoeffs))
    {
      p=NUM(f);
      do
      {
        number cc=n_Div(pGetCoeff(p),c,ntCoeffs);
        n_Normalize(cc,ntCoeffs);
        p_SetCoeff(p,cc,ntRing);
        pIter(p);
      } while(p!=NULL);
      p=DEN(f);
      do
      {
        number cc=n_Div(pGetCoeff(p),c,ntCoeffs);
        n_Normalize(cc,ntCoeffs);
        p_SetCoeff(p,cc,ntRing);
        pIter(p);
      } while(p!=NULL);
      n_Delete(&c,ntCoeffs);
      if(pNext(DEN(f))==NULL)
      {
        if (p_IsOne(DEN(f),ntRing))
        {
          p_LmDelete(&DEN(f),ntRing);
          COM(f)=0;
          return;
        }
        else
        {
          return;
        }
      }
    }
  }*/

  /* here we assume: NUM(f), DEN(f) !=NULL, in Z_a reqp. Z/p_a */
  //StringSetS("");ntWriteLong(a,cf);
  poly pGcd = singclap_gcd_and_divide(NUM(f), DEN(f), ntRing);
  //PrintS("gcd= ");p_wrp(pGcd,ntRing);PrintLn();
  if (p_IsConstant(pGcd, ntRing)
  && n_IsOne(p_GetCoeff(pGcd, ntRing), ntCoeffs)
  )
  { /* gcd = 1; nothing to cancel;
       Suppose the given rational function field is over Q. Although the
       gcd is 1, we may have produced fractional coefficients in NUM(f),
       DEN(f), or both, due to previous arithmetics. The next call will
       remove those nested fractions, in case there are any. */
    if (nCoeff_is_Zp(ntCoeffs))
    {
      number d=p_GetCoeff (DEN(f),ntRing);
      BOOLEAN d_not_1=FALSE;
      if (!n_IsOne(d,ntCoeffs))
      {
        NUM (f) = p_Div_nn (NUM (f), d, ntRing);
        d_not_1=TRUE;
      }
      if (p_IsConstant (DEN (f), ntRing))
      {
        p_Delete(&DEN (f), ntRing);
        DEN (f) = NULL;
      }
      else if (d_not_1)
      {
        DEN (f) = p_Div_nn (DEN (f), d, ntRing);
      }
    } else if (nCoeff_is_Q(ntCoeffs)) handleNestedFractionsOverQ(f, cf);
  }
  else
  { /* We divide both NUM(f) and DEN(f) by the gcd which is known
       to be != 1. */
    if (p_IsConstant(DEN(f), ntRing) &&
      n_IsOne(p_GetCoeff(DEN(f), ntRing), ntCoeffs))
    {
      /* DEN(f) = 1 needs to be represented by NULL! */
      p_Delete(&DEN(f), ntRing);
      DEN(f) = NULL;
    }
    else
    {
      if (nCoeff_is_Zp(ntCoeffs))
      {
        NUM (f) = p_Div_nn (NUM (f), p_GetCoeff (DEN(f),ntRing), ntRing);
        if (p_IsConstant (DEN (f), ntRing))
        {
          p_Delete(&DEN (f), ntRing);
          DEN (f) = NULL;
        }
        else
        {
          p_Norm (DEN (f),ntRing);
        }
      }
    }
  }
  p_Delete(&pGcd, ntRing);
//  StringAppendS(" -> ");ntWriteLong(a,cf);StringAppendS("\n");{ char* s = StringEndS(); Print("%s", s); omFree(s); }
  COM(f) = 0;

  if( DEN(f) != NULL )
  {
    if( !n_GreaterZero(pGetCoeff(DEN(f)), ntCoeffs) )
    {
      NUM(f) = p_Neg(NUM(f), ntRing);
      DEN(f) = p_Neg(DEN(f), ntRing);
      if (p_IsConstant(DEN(f), ntRing) &&
        n_IsOne(p_GetCoeff(DEN(f), ntRing), ntCoeffs))
      {
        /* DEN(f) = 1 needs to be represented by NULL! */
        p_Delete(&DEN(f), ntRing);
        DEN (f) = NULL;
      }
    }
  }
  ntTest(a); // !!!!
}

static void ntWriteLong(number a, const coeffs cf)
{
  ntTest(a);
  if (IS0(a))
    StringAppendS("0");
  else
  {
    fraction f = (fraction)a;
    // stole logic from napWrite from kernel/longtrans.cc of legacy singular
    BOOLEAN omitBrackets = p_IsConstant(NUM(f), ntRing);
    if (!omitBrackets) StringAppendS("(");
    p_String0Long(NUM(f), ntRing, ntRing);
    if (!omitBrackets) StringAppendS(")");
    if (!DENIS1(f))
    {
      StringAppendS("/");
      omitBrackets = p_IsConstant(DEN(f), ntRing);
      if (!omitBrackets) StringAppendS("(");
      p_String0Long(DEN(f), ntRing, ntRing);
      if (!omitBrackets) StringAppendS(")");
    }
  }
  ntTest(a); // !!!!
}

static void ntWriteShort(number a, const coeffs cf)
{
  ntTest(a);
  if (IS0(a))
    StringAppendS("0");
  else
  {
    fraction f = (fraction)a;
    // stole logic from napWrite from kernel/longtrans.cc of legacy singular
    BOOLEAN omitBrackets = p_IsConstant(NUM(f), ntRing);
    if (!omitBrackets) StringAppendS("(");
    p_String0Short(NUM(f), ntRing, ntRing);
    if (!omitBrackets) StringAppendS(")");
    if (!DENIS1(f))
    {
      StringAppendS("/");
      omitBrackets = p_IsConstant(DEN(f), ntRing);
      if (!omitBrackets) StringAppendS("(");
      p_String0Short(DEN(f), ntRing, ntRing);
      if (!omitBrackets) StringAppendS(")");
    }
  }
  ntTest(a);
}

static const char * ntRead(const char *s, number *a, const coeffs cf)
{
  poly p;
  const char * result = p_Read(s, p, ntRing);
  if (p == NULL) *a = NULL;
  else *a = ntInit(p, cf);
  ntTest(*a);
  return result;
}

static void ntNormalize (number &a, const coeffs cf)
{
  if ( /*(*/ a!=NULL /*)*/ )
  {
    //PrintS("num=");p_wrp(NUM(a),ntRing);
    //PrintS(" den=");p_wrp(DEN(a),ntRing);PrintLn();
    if (COM((fraction)a)>0) definiteGcdCancellation(a, cf, FALSE);
    if ((DEN((fraction)a)!=NULL)
    &&(!n_GreaterZero(pGetCoeff(DEN((fraction)a)),ntCoeffs)))
    {
      NUM((fraction)a)=p_Neg(NUM((fraction)a),ntRing);
      DEN((fraction)a)=p_Neg(DEN((fraction)a),ntRing);
    }
  }
  ntNormalizeDen((fraction)a,ntRing);
  ntTest(a); // !!!!
}

/* expects *param to be castable to TransExtInfo */
static BOOLEAN ntCoeffIsEqual(const coeffs cf, n_coeffType n, void * param)
{
  if (n_transExt != n) return FALSE;
  TransExtInfo *e = (TransExtInfo *)param;
  /* for rational function fields we expect the underlying
     polynomial rings to be IDENTICAL, i.e. the SAME OBJECT;
     this expectation is based on the assumption that we have properly
     registered cf and perform reference counting rather than creating
     multiple copies of the same coefficient field/domain/ring */
  if (ntRing == e->r)
    return TRUE;

  // NOTE: Q(a)[x] && Q(a)[y] should better share the _same_ Q(a)...
  if( rEqual(ntRing, e->r, TRUE) )
  {
    rDelete(e->r);
    return TRUE;
  }

  return FALSE;
}

static number ntNormalizeHelper(number a, number b, const coeffs cf)
{
  ntTest(a);
  ntTest(b);
  fraction fb = (fraction)b;
  if ((b==NULL)||(DEN(fb)==NULL)) return ntCopy(a,cf);
  fraction fa = (fraction)a;

  poly pGcd;
  if (nCoeff_is_Q(ntCoeffs))
  {
    poly pa = NUM(fa);
    poly pb = DEN(fb);
    if (p_IsConstant(pa,ntRing) && p_IsConstant(pb,ntRing))
    {
      pGcd = p_Copy(pa,ntRing);
      p_SetCoeff (pGcd, n_Gcd (pGetCoeff(pGcd), pGetCoeff(pb), ntCoeffs), ntRing);
    }
    else
    {
      number contentpa, contentpb, tmp;

      contentpb= n_Copy(p_GetCoeff(pb, ntRing),ntCoeffs);
      pIter(pb);
      while (pb != NULL)
      {
        tmp = n_SubringGcd(contentpb, p_GetCoeff(pb, ntRing) , ntCoeffs);
        n_Delete(&contentpb, ntCoeffs);
        contentpb = tmp;
        pIter(pb);
      }

      contentpa= n_Copy(p_GetCoeff(pa, ntRing),ntCoeffs);
      pIter(pa);
      while (pa != NULL)
      {
        tmp = n_SubringGcd(contentpa, p_GetCoeff(pa, ntRing), ntCoeffs);
        n_Delete(&contentpa, ntCoeffs);
        contentpa = tmp;
        pIter(pa);
      }

      tmp= n_SubringGcd (contentpb, contentpa, ntCoeffs);
      n_Delete(&contentpa, ntCoeffs);
      n_Delete(&contentpb, ntCoeffs);
      contentpa= tmp;

      pGcd = gcd_over_Q(NUM(fa), DEN(fb), ntRing);
      pGcd= __p_Mult_nn (pGcd, contentpa, ntRing);
      n_Delete(&contentpa, ntCoeffs);
    }
  }
  else
    pGcd = singclap_gcd_r(NUM(fa), DEN(fb), ntRing);

  /* Note that, over Q, singclap_gcd will remove the denominators in all
     rational coefficients of pa and pb, before starting to compute
     the gcd. Thus, we do not need to ensure that the coefficients of
     pa and pb live in Z; they may well be elements of Q\Z. */

  if (p_IsConstant(pGcd, ntRing) &&
      n_IsOne(p_GetCoeff(pGcd, ntRing), ntCoeffs))
  { /* gcd = 1; return pa*pb*/
    p_Delete(&pGcd,ntRing);
    fraction result = (fraction)omAlloc0Bin(fractionObjectBin);
    NUM(result) = pp_Mult_qq(NUM(fa),DEN(fb),ntRing);

    ntTest((number)result); // !!!!

    return (number)result;
  }

  /* return pa*pb/gcd */
    poly newNum = singclap_pdivide(NUM(fa), pGcd, ntRing);
    p_Delete(&pGcd,ntRing);
    fraction result = (fraction)omAlloc0Bin(fractionObjectBin);
    NUM(result) = p_Mult_q(p_Copy(DEN(fb),ntRing),newNum,ntRing);
    ntTest((number)result); // !!!!
    return (number)result;
}

static number ntGcd(number a, number b, const coeffs cf)
{
  ntTest(a);
  ntTest(b);
  if (a==NULL) return ntCopy(b,cf);
  if (b==NULL) return ntCopy(a,cf);
  fraction fa = (fraction)a;
  fraction fb = (fraction)b;


  poly pGcd;
  if (nCoeff_is_Q(ntCoeffs))
  {
    poly pa = NUM(fa);
    poly pb = NUM(fb);
    if (p_IsConstant(pa,ntRing) && p_IsConstant(pb,ntRing))
    {
      pGcd = p_Copy(pa,ntRing);
      p_SetCoeff (pGcd, n_SubringGcd (pGetCoeff(pGcd), pGetCoeff(pb), ntCoeffs), ntRing);
    }
    else
    {
      number contentpa, contentpb, tmp;

      contentpb= n_Copy(p_GetCoeff(pb, ntRing),ntCoeffs);
      pIter(pb);
      while (pb != NULL)
      {
        tmp = n_SubringGcd(contentpb, p_GetCoeff(pb, ntRing) , ntCoeffs);
        n_Delete(&contentpb, ntCoeffs);
        contentpb = tmp;
        pIter(pb);
      }

      contentpa= n_Copy(p_GetCoeff(pa, ntRing),ntCoeffs);
      pIter(pa);
      while (pa != NULL)
      {
        tmp = n_SubringGcd(contentpa, p_GetCoeff(pa, ntRing), ntCoeffs);
        n_Delete(&contentpa, ntCoeffs);
        contentpa = tmp;
        pIter(pa);
      }

      tmp= n_SubringGcd (contentpb, contentpa, ntCoeffs);
      n_Delete(&contentpa, ntCoeffs);
      n_Delete(&contentpb, ntCoeffs);
      contentpa= tmp;

      pGcd = gcd_over_Q(NUM(fa), NUM(fb), ntRing);
      pGcd= __p_Mult_nn (pGcd, contentpa, ntRing);
      n_Delete(&contentpa, ntCoeffs);
    }
  }
  else
    pGcd = singclap_gcd_r(NUM(fa), NUM(fb), ntRing);
  /* Note that, over Q, singclap_gcd will remove the denominators in all
     rational coefficients of pa and pb, before starting to compute
     the gcd. Thus, we do not need to ensure that the coefficients of
     pa and pb live in Z; they may well be elements of Q\Z. */

  fraction result = (fraction)omAlloc0Bin(fractionObjectBin);
  NUM(result) = pGcd;
  ntTest((number)result); // !!!!
  return (number)result;
}
//number ntGcd_dummy(number a, number b, const coeffs cf)
//{
//  extern char my_yylinebuf[80];
//  Print("ntGcd in >>%s<<\n",my_yylinebuf);
//  return ntGcd(a,b,cf);
//}

static int ntSize(number a, const coeffs cf)
{
  ntTest(a);
  if (IS0(a)) return 0;
  fraction f = (fraction)a;
  poly p = NUM(f);
  unsigned long noOfTerms = 0;
  unsigned long numDegree = 0;
  if (p!=NULL)
  {
    numDegree = p_Totaldegree(p,ntRing);
    noOfTerms = pLength(p);
  }
  unsigned long denDegree = 0;
  if (!DENIS1(f))
  {
    denDegree =  p_Totaldegree(DEN(f),ntRing);
    noOfTerms += pLength(DEN(f));
  }
  ntTest(a); // !!!!
  // avoid int overflow:
  unsigned long t= ((numDegree + denDegree)*(numDegree + denDegree) + 1) * noOfTerms; // must be >0
  if (t>INT_MAX) return INT_MAX;
  else return (int)t;
}

/* assumes that src = Q or Z, dst = Q(t_1, ..., t_s) */
static number ntMap00(number a, const coeffs src, const coeffs dst)
{
  n_Test(a, src);

  if (n_IsZero(a, src)) return NULL;
  assume(src->rep == dst->extRing->cf->rep);
  if ((SR_HDL(a) & SR_INT) || (a->s==3))
  {
    number res=ntInit(p_NSet(n_Copy(a, src), dst->extRing), dst);
    n_Test(res, dst);
    return res;
  }
  number nn=n_GetDenom(a,src);
  number zz=n_GetNumerator(a,src);
  number res=ntInit(p_NSet(zz,dst->extRing), dst);
  fraction ff=(fraction)res;
  if (n_IsOne(nn,src)) DEN(ff)=NULL;
  else                 DEN(ff)=p_NSet(nn,dst->extRing);

  n_Test((number)ff,dst);
  //check_N((number)ff,dst);
  return (number)ff;
}

static number ntMapZ0(number a, const coeffs src, const coeffs dst)
{
  n_Test(a, src);
  if (n_IsZero(a, src)) return NULL;
  nMapFunc nMap=n_SetMap(src,dst->extRing->cf);
  poly p=p_NSet(nMap(a, src,dst->extRing->cf), dst->extRing);
  if (n_IsZero(pGetCoeff(p),dst->extRing->cf))
    p_Delete(&p,dst->extRing);
  number res=ntInit(p, dst);
  n_Test(res,dst);
  return res;
}

/* assumes that src = Z/p, dst = Q(t_1, ..., t_s) */
static number ntMapP0(number a, const coeffs src, const coeffs dst)
{
  n_Test(a, src);
  if (n_IsZero(a, src)) return NULL;
  /* mapping via intermediate int: */
  int n = n_Int(a, src);
  number q = n_Init(n, dst->extRing->cf);
  if (n_IsZero(q, dst->extRing->cf))
  {
    n_Delete(&q, dst->extRing->cf);
    return NULL;
  }
  return ntInit(p_NSet(q, dst->extRing), dst);
}

 /* assumes that either src = K(t_1, ..., t_s), dst = K(t_1, ..., t_s) */
static number ntCopyMap(number a, const coeffs cf, const coeffs dst)
{
  ntTest(a);
  if (IS0(a)) return NULL;

  const ring rSrc = cf->extRing;
  const ring rDst = dst->extRing;

  if( rSrc == rDst )
    return ntCopy(a, dst); // USUALLY WRONG!

  fraction f = (fraction)a;
  poly g = prCopyR(NUM(f), rSrc, rDst);

  poly h = NULL;

  if (!DENIS1(f))
     h = prCopyR(DEN(f), rSrc, rDst);

  fraction result = (fraction)omAllocBin(fractionObjectBin);

  NUM(result) = g;
  DEN(result) = h;
  COM(result) = COM(f);
  //check_N((number)result,dst);
  n_Test((number)result, dst);
  return (number)result;
}

static number ntGenMap(number a, const coeffs cf, const coeffs dst)
{
  ntTest(a);
  if (IS0(a)) return NULL;

  const ring rSrc = cf->extRing;
  const ring rDst = dst->extRing;

  const nMapFunc nMap=n_SetMap(rSrc->cf,rDst->cf);
  fraction f = (fraction)a;
  poly g = prMapR(NUM(f), nMap, rSrc, rDst);
  /* g may contain summands with coeff 0 */
  poly hh=g;
  poly prev=NULL;
  while(hh!=NULL)
  {
    if (n_IsZero(pGetCoeff(hh),rDst->cf))
    {
      if (prev==NULL)
      {
        g=p_LmFreeAndNext(g,rDst);
        hh=g;
      }
      else
      {
        prev->next=p_LmFreeAndNext(prev->next,rDst);
        hh=prev->next;
      }
    }
    else
    {
      prev=hh;
      pIter(hh);
    }
  }
  if (g==NULL) return NULL;

  poly h = NULL;

  if (!DENIS1(f))
  {
     h = prMapR(DEN(f), nMap, rSrc, rDst);
     /* h may contain summands with coeff 0 */
    hh=h;
    prev=NULL;
    while(hh!=NULL)
    {
      if (n_IsZero(pGetCoeff(hh),rDst->cf))
      {
        if (prev==NULL)
        {
          h=p_LmFreeAndNext(h,rDst);
          hh=h;
        }
        else
        {
          prev->next=p_LmFreeAndNext(prev->next,rDst);
          hh=prev->next;
        }
      }
      else
      {
        prev=hh;
        pIter(hh);
      }
    }
    if (h==NULL) WerrorS("mapping to */0");
  }

  fraction result = (fraction)omAllocBin(fractionObjectBin);

  NUM(result) = g;
  DEN(result) = h;
  COM(result) = COM(f);
  //check_N((number)result,dst);
  n_Test((number)result, dst);
  return (number)result;
}

static number ntCopyAlg(number a, const coeffs cf, const coeffs dst)
{
  n_Test(a, cf) ;
  if (n_IsZero(a, cf)) return NULL;
  return ntInit(prCopyR((poly)a, cf->extRing, dst->extRing),dst);
}

static number ntGenAlg(number a, const coeffs cf, const coeffs dst)
{
  n_Test(a, cf) ;
  if (n_IsZero(a, cf)) return NULL;

  const nMapFunc nMap=n_SetMap(cf->extRing->cf,dst->extRing->cf);
  return ntInit(prMapR((poly)a, nMap, cf->extRing, dst->extRing),dst);
}

/* assumes that src = Q, dst = Z/p(t_1, ..., t_s) */
static number ntMap0P(number a, const coeffs src, const coeffs dst)
{
  n_Test(a, src) ;
  if (n_IsZero(a, src)) return NULL;
  // int p = rChar(dst->extRing);

  number q = nlModP(a, src, dst->extRing->cf); // FIXME? TODO? // extern number nlModP(number q, const coeffs Q, const coeffs Zp); // Map q \in QQ \to Zp

  if (n_IsZero(q, dst->extRing->cf))
  {
    n_Delete(&q, dst->extRing->cf);
    return NULL;
  }

  poly g = p_NSet(q, dst->extRing);

  fraction f = (fraction)omAlloc0Bin(fractionObjectBin);
  NUM(f) = g; // DEN(f) = NULL; COM(f) = 0;
  n_Test((number)f, dst);
  //check_N((number)f,dst);
  return (number)f;
}

/* assumes that src = Z/p, dst = Z/p(t_1, ..., t_s) */
static number ntMapPP(number a, const coeffs src, const coeffs dst)
{
  n_Test(a, src) ;
  if (n_IsZero(a, src)) return NULL;
  assume(src == dst->extRing->cf);
  poly p = p_One(dst->extRing);
  p_SetCoeff(p, n_Copy(a, src), dst->extRing);
  fraction f = (fraction)omAlloc0Bin(fractionObjectBin);
  NUM(f) = p; // DEN(f) = NULL; COM(f) = 0;
  n_Test((number)f, dst);
  //check_N((number)f,dst);
  return (number)f;
}

/* assumes that src = Z/u, dst = Z/p(t_1, ..., t_s), where u != p */
static number ntMapUP(number a, const coeffs src, const coeffs dst)
{
  n_Test(a, src) ;
  if (n_IsZero(a, src)) return NULL;
  /* mapping via intermediate int: */
  int n = n_Int(a, src);
  number q = n_Init(n, dst->extRing->cf);
  poly p;
  if (n_IsZero(q, dst->extRing->cf))
  {
    n_Delete(&q, dst->extRing->cf);
    return NULL;
  }
  p = p_One(dst->extRing);
  p_SetCoeff(p, q, dst->extRing);
  fraction f = (fraction)omAlloc0Bin(fractionObjectBin);
  NUM(f) = p; // DEN(f) = NULL; COM(f) = 0;
  n_Test((number)f, dst);
  //check_N((number)f,dst);
  return (number)f;
}

nMapFunc ntSetMap(const coeffs src, const coeffs dst)
{
  /* dst is expected to be a rational function field */
  assume(getCoeffType(dst) == n_transExt);

  if( src == dst ) return ndCopyMap;

  int h = 0; /* the height of the extension tower given by dst */
  coeffs bDst = nCoeff_bottom(dst, h); /* the bottom field in the tower dst */
  coeffs bSrc = nCoeff_bottom(src, h); /* the bottom field in the tower src */

  /* for the time being, we only provide maps if h = 1 and if b is Q or
     some field Z/pZ: */
  if (h==0)
  {
    if ((src->rep==n_rep_gap_rat) && nCoeff_is_Q(bDst))
      return ntMap00;                                 /// Q or Z   -->  Q(T)
    if (src->rep==n_rep_gap_gmp)
      return ntMapZ0;                                 /// Z   -->  K(T)
    if (nCoeff_is_Zp(src) && nCoeff_is_Q(bDst))
      return ntMapP0;                                 /// Z/p     -->  Q(T)
    if (nCoeff_is_Q_or_BI(src) && nCoeff_is_Zp(bDst))
      return ntMap0P;                                 /// Q       --> Z/p(T)
    if (nCoeff_is_Zp(src) && nCoeff_is_Zp(bDst))
    {
      if (src->ch == dst->ch) return ntMapPP;         /// Z/p     --> Z/p(T)
      else return ntMapUP;                            /// Z/u     --> Z/p(T)
    }
    if (nCoeff_is_Zn(src) && nCoeff_is_Zn(bDst))
    {
      if (mpz_cmp(src->modNumber,bDst->modNumber)==0) return ntMapPP;         /// Z/p     --> Z/p(T)
    }
  }
  if (h != 1) return NULL;
  //if ((!nCoeff_is_Zp(bDst)) && (!nCoeff_is_Q(bDst))) return NULL;

  /* Let T denote the sequence of transcendental extension variables, i.e.,
     K[t_1, ..., t_s] =: K[T];
     Let moreover, for any such sequence T, T' denote any subsequence of T
     of the form t_1, ..., t_w with w <= s. */

  if (rVar(src->extRing) > rVar(dst->extRing))
     return NULL;

  for (int i = 0; i < rVar(src->extRing); i++)
    if (strcmp(rRingVar(i, src->extRing), rRingVar(i, dst->extRing)) != 0)
       return NULL;

  if (src->type==n_transExt)
  {
     if (src->extRing->cf==dst->extRing->cf)
       return ntCopyMap;          /// K(T')   --> K(T)
     else
       return ntGenMap;          /// K(T')   --> K'(T)
  }
  else
  {
     if (src->extRing->cf==dst->extRing->cf)
       return ntCopyAlg;          /// K(T')   --> K(T)
     else
       return ntGenAlg;          /// K(T')   --> K'(T)
  }

  return NULL;                                 /// default
}
#if 0
nMapFunc ntSetMap_T(const coeffs src, const coeffs dst)
{
  nMapFunc n=ntSetMap(src,dst);
  if (n==ntCopyAlg) printf("n=ntCopyAlg\n");
  else if (n==ntCopyMap) printf("n=ntCopyMap\n");
  else if (n==ntMapUP) printf("n=ntMapUP\n");
  else if (n==ntMap0P) printf("n=ntMap0P\n");
  else if (n==ntMapP0) printf("n=ntMapP0\n");
  else if (n==ntMap00) printf("n=ntMap00\n");
  else if (n==NULL) printf("n=NULL\n");
  else printf("n=?\n");
  return n;
}
#endif

static void ntKillChar(coeffs cf)
{
  if ((--cf->extRing->ref) == 0)
    rDelete(cf->extRing);
}
static number ntConvFactoryNSingN( const CanonicalForm n, const coeffs cf)
{
  if (n.isZero()) return NULL;
  poly p=convFactoryPSingP(n,ntRing);
  p_Normalize(p,ntRing);
  fraction result = (fraction)omAlloc0Bin(fractionObjectBin);
  NUM(result) = p;
  //DEN(result) = NULL; // done by omAlloc0Bin
  //COM(result) = 0; // done by omAlloc0Bin
  ntTest((number)result);
  return (number)result;
}
static CanonicalForm ntConvSingNFactoryN( number n, BOOLEAN /*setChar*/, const coeffs cf )
{
  ntTest(n);
  if (IS0(n)) return CanonicalForm(0);

  fraction f = (fraction)n;
  return convSingPFactoryP(NUM(f),ntRing);
}

static int ntParDeg(number a, const coeffs cf)
{
  ntTest(a);
  if (IS0(a)) return -1;
  fraction fa = (fraction)a;
  return cf->extRing->pFDeg(NUM(fa),cf->extRing);
}

/// return the specified parameter as a number in the given trans.ext.
static number ntParameter(const int iParameter, const coeffs cf)
{
  assume(getCoeffType(cf) == n_transExt);

  const ring R = cf->extRing;
  assume( R != NULL );
  assume( 0 < iParameter && iParameter <= rVar(R) );

  poly p = p_One(R); p_SetExp(p, iParameter, 1, R); p_Setm(p, R);
  p_Test(p,R);

  fraction f = (fraction)omAlloc0Bin(fractionObjectBin);
  NUM(f) = p;
  //DEN(f) = NULL;
  //COM(f) = 0;

  ntTest((number)f);

  return (number)f;
}

/// if m == var(i)/1 => return i,
int ntIsParam(number m, const coeffs cf)
{
  ntTest(m);
  assume(getCoeffType(cf) == n_transExt);

  const ring R = cf->extRing;
  assume( R != NULL );

  fraction f = (fraction)m;

  if( DEN(f) != NULL )
    return 0;

  return p_Var( NUM(f), R );
}

struct NTNumConverter
{
  static inline poly convert(const number& n)
  {
    // suitable for trans. ext. numbers that are fractions of polys
    return NUM((fraction)n); // return the numerator
  }
};


static void ntClearContent(ICoeffsEnumerator& numberCollectionEnumerator, number& c, const coeffs cf)
{
  assume(cf != NULL);
  assume(getCoeffType(cf) == n_transExt);
  // all coeffs are given by fractions of polynomails over integers!!!
  // without denominators!!!

  const ring   R = cf->extRing;
  assume(R != NULL);
  const coeffs Q = R->cf;
  assume(Q != NULL);
  assume(nCoeff_is_Q(Q));


  numberCollectionEnumerator.Reset();

  if( !numberCollectionEnumerator.MoveNext() ) // empty zero polynomial?
  {
    c = ntInit(1, cf);
    return;
  }

  // all coeffs are given by integers after returning from this routine

  // part 1, collect product of all denominators /gcds
  poly cand = NULL;

  do
  {
    number &n = numberCollectionEnumerator.Current();

    ntNormalize(n, cf);

    fraction f = (fraction)n;

    assume( f != NULL );

    const poly den = DEN(f);

    assume( den == NULL ); // ?? / 1 ?

    const poly num = NUM(f);

    if( cand == NULL )
      cand = p_Copy(num, R);
    else
    {
      poly tmp = singclap_gcd_r(cand, num, R); // gcd(cand, num)
      p_Delete(&cand,R);
      cand=tmp;
    }

    if( p_IsConstant(cand, R) )
      break;
  }
  while( numberCollectionEnumerator.MoveNext() ) ;


  // part2: all coeffs = all coeffs * cand
  if( cand != NULL )
  {
  if( !p_IsConstant(cand, R) )
  {
    c = ntInit(cand, cf);
    numberCollectionEnumerator.Reset();
    while (numberCollectionEnumerator.MoveNext() )
    {
      number &n = numberCollectionEnumerator.Current();
      const number t = ntDiv(n, c, cf); // TODO: rewrite!?
      ntDelete(&n, cf);
      n = t;
    }
  } // else NUM (result) = p_One(R);
  else { p_Delete(&cand, R); cand = NULL; }
  }

  // Quick and dirty fix for constant content clearing: consider numerators???
  CRecursivePolyCoeffsEnumerator<NTNumConverter> itr(numberCollectionEnumerator); // recursively treat the NUM(numbers) as polys!
  number cc;

  n_ClearContent(itr, cc, Q);
  number g = ntInit(p_NSet(cc, R), cf);

  if( cand != NULL )
  {
    number gg = ntMult(g, c, cf);
    ntDelete(&g, cf);
    ntDelete(&c, cf); c = gg;
  } else
    c = g;
  ntTest(c);
}

static void ntClearDenominators(ICoeffsEnumerator& numberCollectionEnumerator, number& c, const coeffs cf)
{
  assume(cf != NULL);
  assume(getCoeffType(cf) == n_transExt); // both over Q(a) and Zp(a)!
  // all coeffs are given by fractions of polynomails over integers!!!

  numberCollectionEnumerator.Reset();

  if( !numberCollectionEnumerator.MoveNext() ) // empty zero polynomial?
  {
    c = ntInit(1, cf);
    return;
  }

  // all coeffs are given by integers after returning from this routine

  // part 1, collect product of all denominators /gcds
  poly cand = NULL;

  const ring R = cf->extRing;
  assume(R != NULL);

  const coeffs Q = R->cf;
  assume(Q != NULL);
//  assume(nCoeff_is_Q(Q));

  do
  {
    number &n = numberCollectionEnumerator.Current();

    ntNormalize(n, cf);

    fraction f = (fraction)ntGetDenom (n, cf);

    assume( f != NULL );

    const poly den = NUM(f);

    if( den == NULL ) // ?? / 1 ?
      continue;

    if( cand == NULL )
      cand = p_Copy(den, R);
    else
    {
      // cand === LCM( cand, den )!!!!
      // NOTE: maybe it's better to make the product and clearcontent afterwards!?
      // TODO: move the following to factory?
      poly gcd = singclap_gcd_r(cand, den, R); // gcd(cand, den) is monic no mater leading coeffs! :((((
      if (nCoeff_is_Q (Q))
      {
        number LcGcd= n_SubringGcd (p_GetCoeff (cand, R), p_GetCoeff(den, R), Q);
        gcd = __p_Mult_nn(gcd, LcGcd, R);
        n_Delete(&LcGcd,Q);
      }
//      assume( n_IsOne(pGetCoeff(gcd), Q) ); // TODO: this may be wrong...
      cand = p_Mult_q(cand, p_Copy(den, R), R); // cand *= den
      const poly t = singclap_pdivide( cand, gcd, R ); // cand' * den / gcd(cand', den)
      p_Delete(&cand, R);
      p_Delete(&gcd, R);
      cand = t;
    }
  }
  while( numberCollectionEnumerator.MoveNext() );

  if( cand == NULL )
  {
    c = ntInit(1, cf);
    return;
  }

  c = ntInit(cand, cf);

  numberCollectionEnumerator.Reset();

  number d = NULL;

  while (numberCollectionEnumerator.MoveNext() )
  {
    number &n = numberCollectionEnumerator.Current();
    number t = ntMult(n, c, cf); // TODO: rewrite!?
    ntDelete(&n, cf);

    ntNormalize(t, cf); // TODO: needed?
    n = t;

    fraction f = (fraction)t;
    assume( f != NULL );

    const poly den = DEN(f);

    if( den != NULL ) // ?? / ?? ?
    {
      assume( p_IsConstant(den, R) );
      assume( pNext(den) == NULL );

      if( d == NULL )
        d = n_Copy(pGetCoeff(den), Q);
      else
      {
        number g = n_NormalizeHelper(d, pGetCoeff(den), Q);
        n_Delete(&d, Q); d = g;
      }
    }
  }

  if( d != NULL )
  {
    numberCollectionEnumerator.Reset();
    while (numberCollectionEnumerator.MoveNext() )
    {
      number &n = numberCollectionEnumerator.Current();
      fraction f = (fraction)n;

      assume( f != NULL );

      const poly den = DEN(f);

      if( den == NULL ) // ?? / 1 ?
        NUM(f) = __p_Mult_nn(NUM(f), d, R);
      else
      {
        assume( p_IsConstant(den, R) );
        assume( pNext(den) == NULL );

        number ddd = n_Div(d, pGetCoeff(den), Q); // but be an integer now!!!
        NUM(f) = __p_Mult_nn(NUM(f), ddd, R);
        n_Delete(&ddd, Q);

        p_Delete(&DEN(f), R);
        DEN(f) = NULL; // TODO: check if this is needed!?
      }

      assume( DEN(f) == NULL );
    }

    NUM((fraction)c) = __p_Mult_nn(NUM((fraction)c), d, R);
    n_Delete(&d, Q);
  }


  ntTest(c);
}

static number ntChineseRemainder(number *x, number *q,int rl, BOOLEAN /*sym*/,CFArray &inv_cache,const coeffs cf)
{
  fraction result = (fraction)omAlloc0Bin(fractionObjectBin);

  poly *P=(poly*)omAlloc(rl*sizeof(poly*));
  number *X=(number *)omAlloc(rl*sizeof(number));

  int i;

  for(i=0;i<rl;i++) P[i]=p_Copy(NUM((fraction)(x[i])),cf->extRing);
  NUM(result)=p_ChineseRemainder(P,X,q,rl,inv_cache,cf->extRing);

  for(i=0;i<rl;i++)
  {
    P[i]=p_Copy(DEN((fraction)(x[i])),cf->extRing);
    if (P[i]==NULL) P[i]=p_One(cf->extRing);
  }
  DEN(result)=p_ChineseRemainder(P,X,q,rl,inv_cache,cf->extRing);

  omFreeSize(X,rl*sizeof(number));
  omFreeSize(P,rl*sizeof(poly*));
  if (p_IsConstant(DEN(result), ntRing)
  && n_IsOne(pGetCoeff(DEN(result)), ntCoeffs))
  {
    p_Delete(&DEN(result),ntRing);
  }
  ntTest((number)result);
  return ((number)result);
}

static number ntFarey(number p, number n, const coeffs cf)
{
  // n is really a bigint
  fraction result = (fraction)omAlloc0Bin(fractionObjectBin);
  NUM(result)=p_Farey(p_Copy(NUM((fraction)p),cf->extRing),n,cf->extRing);
  DEN(result)=p_Farey(p_Copy(DEN((fraction)p),cf->extRing),n,cf->extRing);
  ntTest((number)result);
  return ((number)result);
}

BOOLEAN ntInitChar(coeffs cf, void * infoStruct)
{

  assume( infoStruct != NULL );

  TransExtInfo *e = (TransExtInfo *)infoStruct;

  assume( e->r                != NULL);      // extRing;
  assume( e->r->cf            != NULL);      // extRing->cf;
  assume( e->r->qideal == NULL );

  assume( cf != NULL );
  assume(getCoeffType(cf) == n_transExt);                // coeff type;

  ring R = e->r;
  assume(R != NULL);

  R->ref ++; // increase the ref.counter for the ground poly. ring!

  cf->extRing           = R;
  /* propagate characteristic up so that it becomes
     directly accessible in cf: */
  cf->ch = R->cf->ch;

  cf->is_field=TRUE;
  cf->is_domain=TRUE;
  cf->rep=n_rep_rat_fct;

  cf->factoryVarOffset = R->cf->factoryVarOffset + rVar(R);

  cf->cfCoeffString = naCoeffString; // FIXME? TODO? // extern char* naCoeffString(const coeffs r);
  cf->cfCoeffName =  naCoeffName; // FIXME? TODO? // extern char* naCoeffString(const coeffs r);

  cf->cfGreaterZero  = ntGreaterZero;
  cf->cfGreater      = ntGreater;
  cf->cfEqual        = ntEqual;
  cf->cfIsZero       = ntIsZero;
  cf->cfIsOne        = ntIsOne;
  cf->cfIsMOne       = ntIsMOne;
  cf->cfInit         = ntInit;
  cf->cfFarey        = ntFarey;
  cf->cfChineseRemainder = ntChineseRemainder;
  cf->cfInt          = ntInt;
  cf->cfAdd          = ntAdd;
  cf->cfInpNeg       = ntNeg;
  cf->cfSub          = ntSub;
  cf->cfMult         = ntMult;
  cf->cfDiv          = ntDiv;
  cf->cfExactDiv     = ntDiv;
  cf->cfPower        = ntPower;
  cf->cfCopy         = ntCopy;
  cf->cfWriteLong    = ntWriteLong;
  cf->cfRead         = ntRead;
  cf->cfNormalize    = ntNormalize;
  cf->cfDelete       = ntDelete;
  cf->cfSetMap       = ntSetMap;
  cf->cfGetDenom     = ntGetDenom;
  cf->cfGetNumerator = ntGetNumerator;
  //cf->cfRePart       = ntCopy;
  //cf->cfImPart       = ntImPart;
  cf->cfCoeffWrite   = ntCoeffWrite;
#ifdef LDEBUG
  cf->cfDBTest       = ntDBTest;
#endif
  //cf->cfGcd          = ntGcd_dummy;
  cf->cfSubringGcd   = ntGcd;
  cf->cfNormalizeHelper = ntNormalizeHelper;
  cf->cfSize         = ntSize;
  cf->nCoeffIsEqual  = ntCoeffIsEqual;
  cf->cfInvers       = ntInvers;
  cf->cfKillChar     = ntKillChar;

  if( rCanShortOut(ntRing) )
    cf->cfWriteShort = ntWriteShort;
  else
    cf->cfWriteShort = ntWriteLong;

  cf->convFactoryNSingN =ntConvFactoryNSingN;
  cf->convSingNFactoryN =ntConvSingNFactoryN;
  cf->cfParDeg = ntParDeg;

  cf->iNumberOfParameters = rVar(R);
  cf->pParameterNames = (const char**)R->names;
  cf->cfParameter = ntParameter;
  cf->has_simple_Inverse= FALSE;
  /* cf->has_simple_Alloc= FALSE; */


  if( nCoeff_is_Q(R->cf) )
    cf->cfClearContent = ntClearContent;

  cf->cfClearDenominators = ntClearDenominators;

  return FALSE;
}

template class CRecursivePolyCoeffsEnumerator<NTNumConverter>;
template class IEnumerator<snumber*>;
