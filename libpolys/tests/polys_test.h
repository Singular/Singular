#include "common.h"
using namespace std;

// the following headers are private...


#include "polys/monomials/ring.h"
#include "polys/monomials/p_polys.h"

#include "polys/simpleideals.h"

#if 0 //ifdef HAVE_FACTORY
#include "polys/clapsing.h"
#include "factory/factory.h"
#endif


#define TRANSEXT_PRIVATES

#include "polys/ext_fields/algext.h"
#include "polys/ext_fields/transext.h"


class MyGlobalPrintingFixture : public GlobalPrintingFixture
{
  public:
    virtual bool setUpWorld()
    {

      GlobalPrintingFixture::setUpWorld();


      //TS_ASSERT_EQUALS( nRegister( n_Zp, npInitChar), n_Zp );
      //TS_ASSERT_EQUALS( nRegister( n_GF, nfInitChar), n_GF );
      //TS_ASSERT_EQUALS( nRegister( n_R, nrInitChar), n_R );
      //TS_ASSERT_EQUALS( nRegister( n_Q, nlInitChar), n_Q );
      //TS_ASSERT_EQUALS( nRegister( n_R, nrInitChar), n_R );

#ifdef HAVE_RINGS
      //TS_ASSERT_EQUALS( nRegister( n_Z, nrzInitChar), n_Z ); // these are UNusable at the moment!
#endif

      return true;
    }
};


//
// We can rely on this file being included exactly once
// and declare this global variable in the header file.
//
static MyGlobalPrintingFixture globalPrintingFixture;


namespace
{

  void PrintRing(const ring r)
  {
    rWrite(r); PrintLn();
  #ifdef  RDEBUG
    rDebugPrint(r); PrintLn();
  #endif
  }

  static inline std::string _2S(poly a, const ring r)
  {
    p_Test(a,r);

    StringSetS("");
    p_Write(a, r);

    std::stringstream ss;
    {
      char* s = StringEndS();  ss << s; omFree(s);
    }

    return ss.str();
  }

  static inline void PrintSized(/*const*/ poly a, const ring r, BOOLEAN eoln = TRUE)
  {
    std::clog << _2S(a, r) << ", of size: " << p_Size(a, r);

    if( eoln )
      std::clog << std::endl;
  }

static inline void Delete(poly &p, const ring r)
{
  if( p != NULL )
    p_Delete(&p, r);

  p = NULL;
}

void TestSum(const ring r, const int N)
{
  TS_ASSERT_DIFFERS( r    , NULLp);
  TS_ASSERT_DIFFERS( r->cf, NULLp);


  clog << ( _2S("TEST: sum[0..") + _2S(N) + "]: ");
  clog << endl;

  assume( N > 0 ); // just for now...

  const int ssss = (N * (N+1)) / 2;

  poly sum1 = p_ISet(ssss, r);
  clog<< "poly(N*(N+1)/2) (int: " << ssss << "): "; PrintSized(sum1, r);

  poly s=NULL, ss=NULL, i=NULL, res=NULL;

  s = p_ISet(N  , r);
  i = p_ISet(N+1, r);

  i = p_Mult_q(s, i, r); s = NULL;

  clog<< "poly(N)*poly(N+1): (int: "<< N*(N+1) << "): "; PrintSized(i, r);

  number t = n_Init(2, r->cf);
  clog<< "number(2): "; PrintSized(t, r->cf);

  if( !n_IsZero( t, r->cf) )
  {
    if( i != NULL )
    {
      number ii = p_GetCoeff(i, r);
      clog<< "number(poly(N)*poly(N+1)): "; PrintSized(ii, r->cf);

#ifdef HAVE_RINGS
      TS_ASSERT( n_DivBy(ii, t, r->cf) );
#endif
       res = p_Div_nn(i, t, r); i = NULL;
    }



    clog<< "(poly(N)*poly(N+1))/number(2): "; PrintSized(res, r);
    poly d = p_Sub(p_Copy(res, r), p_Copy(sum1, r), r);

    if( d != NULL )
      TS_ASSERT( n_IsZeroDivisor(p_GetCoeff(d, r), r->cf) );

    Delete(d, r);

    if( n_GetChar(r->cf) == 0 )
    {
      TS_ASSERT( p_EqualPolys(sum1, res, r) );
      TS_ASSERT( p_EqualPolys(res, sum1, r) );
    }
  } else
    TS_ASSERT_EQUALS( n_GetChar(r->cf), 2);

  n_Delete(&t, r->cf);


  s = NULL;
  ss = NULL;
  for( int k = N; k >= 0; k-- )
  {
    i = p_ISet(k, r);
    s = p_Add_q(s, i, r); // s += i

    i = p_Neg( p_ISet(k, r), r );
    ss = p_Add_q(ss, i, r); // ss -= i
  }

  clog<< "ss(-sum): "; PrintSized(ss, r);

  ss = p_Neg(ss, r); // ss = -ss

  clog<< "real sum    : "; PrintSized(s, r);
  clog<< "real sum(--): "; PrintSized(ss, r);

  TS_ASSERT( p_EqualPolys(s, ss, r) );
  TS_ASSERT( p_EqualPolys(ss, s, r) );

//   TODO(somebody, fix the delete method!);

  Delete(sum1, r);
  Delete(res, r);

  Delete(s, r);
  Delete(ss, r);

  clog << ( " >>> TEST DONE!" );
  clog << endl;

}

void Test(const ring r)
{
  if( r == NULL )
      TS_FAIL("Could not get needed ring");
  else
  {
    TestSum( r, 10 );
    TestSum( r, 100 );
    TestSum( r, 101 );
    TestSum( r, 1001 );
    TestSum( r, 9000 );
  }
}

}

class PolysTestSuite : public CxxTest::TestSuite
{
private:
  /* replaces p by p + c * var(i)^exp (in-place);
     expects exp >= 0 */
  void plusTerm(poly &p, int c, int i, int exp, const ring r)
  {
    poly t = p_ISet(c, r);
    if (exp > 0) { p_SetExp(t, i, exp, r); p_Setm(t, r); }
    p = p_Add_q(p, t, r);
  }
  /* assumes that r is over Q;
     replaces p by p + c1 / c2 * var(i)^exp (in-place);
     expects exp >= 0, and c2 != 0 */
  void plusTermOverQ(poly &p, int c1, int c2, int i, int exp, const ring r)
  {
    number c1AsN = n_Init(c1, r->cf);
    number c2AsN = n_Init(c2, r->cf);
    number c =  n_Div(c1AsN, c2AsN, r->cf);
    poly t = p_ISet(1, r); p_SetCoeff(t, c, r);
    if (exp > 0) { p_SetExp(t, i, exp, r); p_Setm(t, r); }
    p = p_Add_q(p, t, r);
  }
  /* assumes r to represent Q(x)[y], for some variable names x and y;
     replaces p by p + (c1 * 1000000 + c2) * x^xExp * y^yExp (in-place);
     expects xExp, yExp >= 0 */
  void specialPlusTerm(poly &p, int c1, int c2, int sign,
                       int xExp, int yExp, const ring r)
  {
    poly c1p = p_ISet(c1, r->cf->extRing);
    poly c2p = p_ISet(c2, r->cf->extRing);
    poly c3p = p_ISet(1000000, r->cf->extRing);
    poly xterm = p_Mult_q(c1p, c3p, r->cf->extRing);
    xterm = p_Add_q(xterm, c2p, r->cf->extRing);
    if (sign == -1) xterm = p_Neg(xterm, r->cf->extRing);
    if (xExp > 0)
    {
      p_SetExp(xterm, 1, xExp, r->cf->extRing);
      p_Setm(xterm, r->cf->extRing);
    }
    number xtermAsN = toFractionNumber(xterm, r->cf);
    poly yterm = p_ISet(1, r);
    if (yExp > 0)
    {
      p_SetExp(yterm, 1, yExp, r);
      p_Setm(yterm, r);
    }
    p_SetCoeff(yterm, xtermAsN, r);
    p = p_Add_q(p, yterm, r);
  }
  /* defines a special test polynomial in Q(x)[y], for some variable
     names x and y, by excessively using specialPlusTerm */
  void specialPoly(poly &p, const ring r)
  {
    p = NULL;
    specialPlusTerm(p, 0, 1, 1, 0, 17, r);
    specialPlusTerm(p, 0, 1, -1, 15, 16, r);
    specialPlusTerm(p, 0, 2, -1, 14, 16, r);
    specialPlusTerm(p, 0, 3, -1, 13, 16, r);
    specialPlusTerm(p, 0, 4, -1, 12, 16, r);
    specialPlusTerm(p, 0, 5, -1, 11, 16, r);
    specialPlusTerm(p, 0, 6, -1, 10, 16, r);
    specialPlusTerm(p, 0, 7, -1, 9, 16, r);
    specialPlusTerm(p, 0, 8, -1, 8, 16, r);
    specialPlusTerm(p, 0, 9, -1, 7, 16, r);
    specialPlusTerm(p, 0, 10, -1, 6, 16, r);
    specialPlusTerm(p, 0, 11, -1, 5, 16, r);
    specialPlusTerm(p, 0, 12, -1, 4, 16, r);
    specialPlusTerm(p, 0, 13, -1, 3, 16, r);
    specialPlusTerm(p, 0, 14, -1, 2, 16, r);
    specialPlusTerm(p, 0, 15, -1, 1, 16, r);
    specialPlusTerm(p, 0, 16, -1, 0, 16, r);
    specialPlusTerm(p, 0, 1, 1, 29, 15, r);
    specialPlusTerm(p, 0, 3, 1, 28, 15, r);
    specialPlusTerm(p, 0, 7, 1, 27, 15, r);
    specialPlusTerm(p, 0, 13, 1, 26, 15, r);
    specialPlusTerm(p, 0, 22, 1, 25, 15, r);
    specialPlusTerm(p, 0, 34, 1, 24, 15, r);
    specialPlusTerm(p, 0, 50, 1, 23, 15, r);
    specialPlusTerm(p, 0, 70, 1, 22, 15, r);
    specialPlusTerm(p, 0, 95, 1, 21, 15, r);
    specialPlusTerm(p, 0, 125, 1, 20, 15, r);
    specialPlusTerm(p, 0, 161, 1, 19, 15, r);
    specialPlusTerm(p, 0, 203, 1, 18, 15, r);
    specialPlusTerm(p, 0, 252, 1, 17, 15, r);
    specialPlusTerm(p, 0, 308, 1, 16, 15, r);
    specialPlusTerm(p, 0, 372, 1, 15, 15, r);
    specialPlusTerm(p, 0, 428, 1, 14, 15, r);
    specialPlusTerm(p, 0, 476, 1, 13, 15, r);
    specialPlusTerm(p, 0, 515, 1, 12, 15, r);
    specialPlusTerm(p, 0, 545, 1, 11, 15, r);
    specialPlusTerm(p, 0, 565, 1, 10, 15, r);
    specialPlusTerm(p, 0, 575, 1, 9, 15, r);
    specialPlusTerm(p, 0, 574, 1, 8, 15, r);
    specialPlusTerm(p, 0, 562, 1, 7, 15, r);
    specialPlusTerm(p, 0, 538, 1, 6, 15, r);
    specialPlusTerm(p, 0, 502, 1, 5, 15, r);
    specialPlusTerm(p, 0, 453, 1, 4, 15, r);
    specialPlusTerm(p, 0, 391, 1, 3, 15, r);
    specialPlusTerm(p, 0, 315, 1, 2, 15, r);
    specialPlusTerm(p, 0, 225, 1, 1, 15, r);
    specialPlusTerm(p, 0, 120, 1, 0, 15, r);
    specialPlusTerm(p, 0, 1, -1, 42, 14, r);
    specialPlusTerm(p, 0, 4, -1, 41, 14, r);
    specialPlusTerm(p, 0, 11, -1, 40, 14, r);
    specialPlusTerm(p, 0, 25, -1, 39, 14, r);
    specialPlusTerm(p, 0, 50, -1, 38, 14, r);
    specialPlusTerm(p, 0, 91, -1, 37, 14, r);
    specialPlusTerm(p, 0, 155, -1, 36, 14, r);
    specialPlusTerm(p, 0, 250, -1, 35, 14, r);
    specialPlusTerm(p, 0, 386, -1, 34, 14, r);
    specialPlusTerm(p, 0, 575, -1, 33, 14, r);
    specialPlusTerm(p, 0, 831, -1, 32, 14, r);
    specialPlusTerm(p, 0, 1170, -1, 31, 14, r);
    specialPlusTerm(p, 0, 1611, -1, 30, 14, r);
    specialPlusTerm(p, 0, 2175, -1, 29, 14, r);
    specialPlusTerm(p, 0, 2871, -1, 28, 14, r);
    specialPlusTerm(p, 0, 3710, -1, 27, 14, r);
    specialPlusTerm(p, 0, 4690, -1, 26, 14, r);
    specialPlusTerm(p, 0, 5810, -1, 25, 14, r);
    specialPlusTerm(p, 0, 7056, -1, 24, 14, r);
    specialPlusTerm(p, 0, 8414, -1, 23, 14, r);
    specialPlusTerm(p, 0, 9856, -1, 22, 14, r);
    specialPlusTerm(p, 0, 11354, -1, 21, 14, r);
    specialPlusTerm(p, 0, 12865, -1, 20, 14, r);
    specialPlusTerm(p, 0, 14345, -1, 19, 14, r);
    specialPlusTerm(p, 0, 15735, -1, 18, 14, r);
    specialPlusTerm(p, 0, 16974, -1, 17, 14, r);
    specialPlusTerm(p, 0, 17985, -1, 16, 14, r);
    specialPlusTerm(p, 0, 18689, -1, 15, 14, r);
    specialPlusTerm(p, 0, 18990, -1, 14, 14, r);
    specialPlusTerm(p, 0, 18909, -1, 13, 14, r);
    specialPlusTerm(p, 0, 18465, -1, 12, 14, r);
    specialPlusTerm(p, 0, 17689, -1, 11, 14, r);
    specialPlusTerm(p, 0, 16610, -1, 10, 14, r);
    specialPlusTerm(p, 0, 15270, -1, 9, 14, r);
    specialPlusTerm(p, 0, 13709, -1, 8, 14, r);
    specialPlusTerm(p, 0, 11980, -1, 7, 14, r);
    specialPlusTerm(p, 0, 10135, -1, 6, 14, r);
    specialPlusTerm(p, 0, 8239, -1, 5, 14, r);
    specialPlusTerm(p, 0, 6356, -1, 4, 14, r);
    specialPlusTerm(p, 0, 4564, -1, 3, 14, r);
    specialPlusTerm(p, 0, 2940, -1, 2, 14, r);
    specialPlusTerm(p, 0, 1575, -1, 1, 14, r);
    specialPlusTerm(p, 0, 560, -1, 0, 14, r);
    specialPlusTerm(p, 0, 1, 1, 54, 13, r);
    specialPlusTerm(p, 0, 5, 1, 53, 13, r);
    specialPlusTerm(p, 0, 16, 1, 52, 13, r);
    specialPlusTerm(p, 0, 41, 1, 51, 13, r);
    specialPlusTerm(p, 0, 92, 1, 50, 13, r);
    specialPlusTerm(p, 0, 187, 1, 49, 13, r);
    specialPlusTerm(p, 0, 353, 1, 48, 13, r);
    specialPlusTerm(p, 0, 628, 1, 47, 13, r);
    specialPlusTerm(p, 0, 1065, 1, 46, 13, r);
    specialPlusTerm(p, 0, 1735, 1, 45, 13, r);
    specialPlusTerm(p, 0, 2732, 1, 44, 13, r);
    specialPlusTerm(p, 0, 4177, 1, 43, 13, r);
    specialPlusTerm(p, 0, 6225, 1, 42, 13, r);
    specialPlusTerm(p, 0, 9056, 1, 41, 13, r);
    specialPlusTerm(p, 0, 12882, 1, 40, 13, r);
    specialPlusTerm(p, 0, 17939, 1, 39, 13, r);
    specialPlusTerm(p, 0, 24481, 1, 38, 13, r);
    specialPlusTerm(p, 0, 32771, 1, 37, 13, r);
    specialPlusTerm(p, 0, 43075, 1, 36, 13, r);
    specialPlusTerm(p, 0, 55639, 1, 35, 13, r);
    specialPlusTerm(p, 0, 70682, 1, 34, 13, r);
    specialPlusTerm(p, 0, 88372, 1, 33, 13, r);
    specialPlusTerm(p, 0, 108804, 1, 32, 13, r);
    specialPlusTerm(p, 0, 131974, 1, 31, 13, r);
    specialPlusTerm(p, 0, 157756, 1, 30, 13, r);
    specialPlusTerm(p, 0, 185860, 1, 29, 13, r);
    specialPlusTerm(p, 0, 215806, 1, 28, 13, r);
    specialPlusTerm(p, 0, 246985, 1, 27, 13, r);
    specialPlusTerm(p, 0, 278631, 1, 26, 13, r);
    specialPlusTerm(p, 0, 309893, 1, 25, 13, r);
    specialPlusTerm(p, 0, 339819, 1, 24, 13, r);
    specialPlusTerm(p, 0, 367427, 1, 23, 13, r);
    specialPlusTerm(p, 0, 391701, 1, 22, 13, r);
    specialPlusTerm(p, 0, 411675, 1, 21, 13, r);
    specialPlusTerm(p, 0, 426428, 1, 20, 13, r);
    specialPlusTerm(p, 0, 435181, 1, 19, 13, r);
    specialPlusTerm(p, 0, 437306, 1, 18, 13, r);
    specialPlusTerm(p, 0, 432423, 1, 17, 13, r);
    specialPlusTerm(p, 0, 420422, 1, 16, 13, r);
    specialPlusTerm(p, 0, 401575, 1, 15, 13, r);
    specialPlusTerm(p, 0, 376559, 1, 14, 13, r);
    specialPlusTerm(p, 0, 346582, 1, 13, 13, r);
    specialPlusTerm(p, 0, 312861, 1, 12, 13, r);
    specialPlusTerm(p, 0, 276645, 1, 11, 13, r);
    specialPlusTerm(p, 0, 239149, 1, 10, 13, r);
    specialPlusTerm(p, 0, 201578, 1, 9, 13, r);
    specialPlusTerm(p, 0, 165048, 1, 8, 13, r);
    specialPlusTerm(p, 0, 130611, 1, 7, 13, r);
    specialPlusTerm(p, 0, 99177, 1, 6, 13, r);
    specialPlusTerm(p, 0, 71526, 1, 5, 13, r);
    specialPlusTerm(p, 0, 48230, 1, 4, 13, r);
    specialPlusTerm(p, 0, 29666, 1, 3, 13, r);
    specialPlusTerm(p, 0, 15925, 1, 2, 13, r);
    specialPlusTerm(p, 0, 6825, 1, 1, 13, r);
    specialPlusTerm(p, 0, 1820, 1, 0, 13, r);
    specialPlusTerm(p, 0, 1, -1, 65, 12, r);
    specialPlusTerm(p, 0, 6, -1, 64, 12, r);
    specialPlusTerm(p, 0, 22, -1, 63, 12, r);
    specialPlusTerm(p, 0, 63, -1, 62, 12, r);
    specialPlusTerm(p, 0, 155, -1, 61, 12, r);
    specialPlusTerm(p, 0, 343, -1, 60, 12, r);
    specialPlusTerm(p, 0, 701, -1, 59, 12, r);
    specialPlusTerm(p, 0, 1345, -1, 58, 12, r);
    specialPlusTerm(p, 0, 2451, -1, 57, 12, r);
    specialPlusTerm(p, 0, 4278, -1, 56, 12, r);
    specialPlusTerm(p, 0, 7198, -1, 55, 12, r);
    specialPlusTerm(p, 0, 11733, -1, 54, 12, r);
    specialPlusTerm(p, 0, 18589, -1, 53, 12, r);
    specialPlusTerm(p, 0, 28699, -1, 52, 12, r);
    specialPlusTerm(p, 0, 43265, -1, 51, 12, r);
    specialPlusTerm(p, 0, 63799, -1, 50, 12, r);
    specialPlusTerm(p, 0, 92152, -1, 49, 12, r);
    specialPlusTerm(p, 0, 130543, -1, 48, 12, r);
    specialPlusTerm(p, 0, 181565, -1, 47, 12, r);
    specialPlusTerm(p, 0, 248179, -1, 46, 12, r);
    specialPlusTerm(p, 0, 333673, -1, 45, 12, r);
    specialPlusTerm(p, 0, 441596, -1, 44, 12, r);
    specialPlusTerm(p, 0, 575643, -1, 43, 12, r);
    specialPlusTerm(p, 0, 739501, -1, 42, 12, r);
    specialPlusTerm(p, 0, 936619, -1, 41, 12, r);
    specialPlusTerm(p, 1, 170014, -1, 40, 12, r);
    specialPlusTerm(p, 1, 441997, -1, 39, 12, r);
    specialPlusTerm(p, 1, 753919, -1, 38, 12, r);
    specialPlusTerm(p, 2, 105911, -1, 37, 12, r);
    specialPlusTerm(p, 2, 496652, -1, 36, 12, r);
    specialPlusTerm(p, 2, 923127, -1, 35, 12, r);
    specialPlusTerm(p, 3, 380499, -1, 34, 12, r);
    specialPlusTerm(p, 3, 861979, -1, 33, 12, r);
    specialPlusTerm(p, 4, 358819, -1, 32, 12, r);
    specialPlusTerm(p, 4, 860402, -1, 31, 12, r);
    specialPlusTerm(p, 5, 354477, -1, 30, 12, r);
    specialPlusTerm(p, 5, 827501, -1, 29, 12, r);
    specialPlusTerm(p, 6, 265239, -1, 28, 12, r);
    specialPlusTerm(p, 6, 653507, -1, 27, 12, r);
    specialPlusTerm(p, 6, 978743, -1, 26, 12, r);
    specialPlusTerm(p, 7, 228746, -1, 25, 12, r);
    specialPlusTerm(p, 7, 393190, -1, 24, 12, r);
    specialPlusTerm(p, 7, 464241, -1, 23, 12, r);
    specialPlusTerm(p, 7, 436963, -1, 22, 12, r);
    specialPlusTerm(p, 7, 309763, -1, 21, 12, r);
    specialPlusTerm(p, 7, 84549, -1, 20, 12, r);
    specialPlusTerm(p, 6, 766941, -1, 19, 12, r);
    specialPlusTerm(p, 6, 366119, -1, 18, 12, r);
    specialPlusTerm(p, 5, 894636, -1, 17, 12, r);
    specialPlusTerm(p, 5, 367870, -1, 16, 12, r);
    specialPlusTerm(p, 4, 803365, -1, 15, 12, r);
    specialPlusTerm(p, 4, 219710, -1, 14, 12, r);
    specialPlusTerm(p, 3, 635296, -1, 13, 12, r);
    specialPlusTerm(p, 3, 66522, -1, 12, 12, r);
    specialPlusTerm(p, 2, 527597, -1, 11, 12, r);
    specialPlusTerm(p, 2, 30222, -1, 10, 12, r);
    specialPlusTerm(p, 1, 583491, -1, 9, 12, r);
    specialPlusTerm(p, 1, 193660, -1, 8, 12, r);
    specialPlusTerm(p, 0, 864214, -1, 7, 12, r);
    specialPlusTerm(p, 0, 595790, -1, 6, 12, r);
    specialPlusTerm(p, 0, 386386, -1, 5, 12, r);
    specialPlusTerm(p, 0, 231504, -1, 4, 12, r);
    specialPlusTerm(p, 0, 124579, -1, 3, 12, r);
    specialPlusTerm(p, 0, 57330, -1, 2, 12, r);
    specialPlusTerm(p, 0, 20475, -1, 1, 12, r);
    specialPlusTerm(p, 0, 4368, -1, 0, 12, r);
    specialPlusTerm(p, 0, 1, 1, 75, 11, r);
    specialPlusTerm(p, 0, 7, 1, 74, 11, r);
    specialPlusTerm(p, 0, 29, 1, 73, 11, r);
    specialPlusTerm(p, 0, 92, 1, 72, 11, r);
    specialPlusTerm(p, 0, 247, 1, 71, 11, r);
    specialPlusTerm(p, 0, 590, 1, 70, 11, r);
    specialPlusTerm(p, 0, 1292, 1, 69, 11, r);
    specialPlusTerm(p, 0, 2643, 1, 68, 11, r);
    specialPlusTerm(p, 0, 5116, 1, 67, 11, r);
    specialPlusTerm(p, 0, 9457, 1, 66, 11, r);
    specialPlusTerm(p, 0, 16810, 1, 65, 11, r);
    specialPlusTerm(p, 0, 28874, 1, 64, 11, r);
    specialPlusTerm(p, 0, 48105, 1, 63, 11, r);
    specialPlusTerm(p, 0, 77963, 1, 62, 11, r);
    specialPlusTerm(p, 0, 123209, 1, 61, 11, r);
    specialPlusTerm(p, 0, 190245, 1, 60, 11, r);
    specialPlusTerm(p, 0, 287494, 1, 59, 11, r);
    specialPlusTerm(p, 0, 425804, 1, 58, 11, r);
    specialPlusTerm(p, 0, 618866, 1, 57, 11, r);
    specialPlusTerm(p, 0, 883609, 1, 56, 11, r);
    specialPlusTerm(p, 1, 240541, 1, 55, 11, r);
    specialPlusTerm(p, 1, 713978, 1, 54, 11, r);
    specialPlusTerm(p, 2, 332107, 1, 53, 11, r);
    specialPlusTerm(p, 3, 126867, 1, 52, 11, r);
    specialPlusTerm(p, 4, 133571, 1, 51, 11, r);
    specialPlusTerm(p, 5, 390227, 1, 50, 11, r);
    specialPlusTerm(p, 6, 936520, 1, 49, 11, r);
    specialPlusTerm(p, 8, 812464, 1, 48, 11, r);
    specialPlusTerm(p, 11, 56669, 1, 47, 11, r);
    specialPlusTerm(p, 13, 704280, 1, 46, 11, r);
    specialPlusTerm(p, 16, 784594, 1, 45, 11, r);
    specialPlusTerm(p, 20, 318470, 1, 44, 11, r);
    specialPlusTerm(p, 24, 315607, 1, 43, 11, r);
    specialPlusTerm(p, 28, 771888, 1, 42, 11, r);
    specialPlusTerm(p, 33, 666946, 1, 41, 11, r);
    specialPlusTerm(p, 38, 962309, 1, 40, 11, r);
    specialPlusTerm(p, 44, 600023, 1, 39, 11, r);
    specialPlusTerm(p, 50, 502149, 1, 38, 11, r);
    specialPlusTerm(p, 56, 571139, 1, 37, 11, r);
    specialPlusTerm(p, 62, 691244, 1, 36, 11, r);
    specialPlusTerm(p, 68, 730935, 1, 35, 11, r);
    specialPlusTerm(p, 74, 546533, 1, 34, 11, r);
    specialPlusTerm(p, 79, 986722, 1, 33, 11, r);
    specialPlusTerm(p, 84, 898120, 1, 32, 11, r);
    specialPlusTerm(p, 89, 131550, 1, 31, 11, r);
    specialPlusTerm(p, 92, 548805, 1, 30, 11, r);
    specialPlusTerm(p, 95, 29441, 1, 29, 11, r);
    specialPlusTerm(p, 96, 477351, 1, 28, 11, r);
    specialPlusTerm(p, 96, 826185, 1, 27, 11, r);
    specialPlusTerm(p, 96, 43174, 1, 26, 11, r);
    specialPlusTerm(p, 94, 131647, 1, 25, 11, r);
    specialPlusTerm(p, 91, 131660, 1, 24, 11, r);
    specialPlusTerm(p, 87, 119171, 1, 23, 11, r);
    specialPlusTerm(p, 82, 203392, 1, 22, 11, r);
    specialPlusTerm(p, 76, 522667, 1, 21, 11, r);
    specialPlusTerm(p, 70, 238707, 1, 20, 11, r);
    specialPlusTerm(p, 63, 529796, 1, 19, 11, r);
    specialPlusTerm(p, 56, 582713, 1, 18, 11, r);
    specialPlusTerm(p, 49, 584249, 1, 17, 11, r);
    specialPlusTerm(p, 42, 712395, 1, 16, 11, r);
    specialPlusTerm(p, 36, 128059, 1, 15, 11, r);
    specialPlusTerm(p, 29, 967652, 1, 14, 11, r);
    specialPlusTerm(p, 24, 337808, 1, 13, 11, r);
    specialPlusTerm(p, 19, 312634, 1, 12, 11, r);
    specialPlusTerm(p, 14, 935096, 1, 11, 11, r);
    specialPlusTerm(p, 11, 218988, 1, 10, 11, r);
    specialPlusTerm(p, 8, 151858, 1, 9, 11, r);
    specialPlusTerm(p, 5, 698407, 1, 8, 11, r);
    specialPlusTerm(p, 3, 804801, 1, 7, 11, r);
    specialPlusTerm(p, 2, 403115, 1, 6, 11, r);
    specialPlusTerm(p, 1, 416415, 1, 5, 11, r);
    specialPlusTerm(p, 0, 763763, 1, 4, 11, r);
    specialPlusTerm(p, 0, 365365, 1, 3, 11, r);
    specialPlusTerm(p, 0, 147147, 1, 2, 11, r);
    specialPlusTerm(p, 0, 45045, 1, 1, 11, r);
    specialPlusTerm(p, 0, 8008, 1, 0, 11, r);
    specialPlusTerm(p, 0, 1, -1, 84, 10, r);
    specialPlusTerm(p, 0, 8, -1, 83, 10, r);
    specialPlusTerm(p, 0, 37, -1, 82, 10, r);
    specialPlusTerm(p, 0, 129, -1, 81, 10, r);
    specialPlusTerm(p, 0, 376, -1, 80, 10, r);
    specialPlusTerm(p, 0, 966, -1, 79, 10, r);
    specialPlusTerm(p, 0, 2258, -1, 78, 10, r);
    specialPlusTerm(p, 0, 4902, -1, 77, 10, r);
    specialPlusTerm(p, 0, 10025, -1, 76, 10, r);
    specialPlusTerm(p, 0, 19511, -1, 75, 10, r);
    specialPlusTerm(p, 0, 36402, -1, 74, 10, r);
    specialPlusTerm(p, 0, 65457, -1, 73, 10, r);
    specialPlusTerm(p, 0, 113910, -1, 72, 10, r);
    specialPlusTerm(p, 0, 192472, -1, 71, 10, r);
    specialPlusTerm(p, 0, 316620, -1, 70, 10, r);
    specialPlusTerm(p, 0, 508215, -1, 69, 10, r);
    specialPlusTerm(p, 0, 797473, -1, 68, 10, r);
    specialPlusTerm(p, 1, 225306, -1, 67, 10, r);
    specialPlusTerm(p, 1, 846017, -1, 66, 10, r);
    specialPlusTerm(p, 2, 730299, -1, 65, 10, r);
    specialPlusTerm(p, 3, 968427, -1, 64, 10, r);
    specialPlusTerm(p, 5, 673536, -1, 63, 10, r);
    specialPlusTerm(p, 7, 984777, -1, 62, 10, r);
    specialPlusTerm(p, 11, 70116, -1, 61, 10, r);
    specialPlusTerm(p, 15, 128465, -1, 60, 10, r);
    specialPlusTerm(p, 20, 390819, -1, 59, 10, r);
    specialPlusTerm(p, 27, 120017, -1, 58, 10, r);
    specialPlusTerm(p, 35, 608761, -1, 57, 10, r);
    specialPlusTerm(p, 46, 175479, -1, 56, 10, r);
    specialPlusTerm(p, 59, 157726, -1, 55, 10, r);
    specialPlusTerm(p, 74, 902862, -1, 54, 10, r);
    specialPlusTerm(p, 93, 755945, -1, 53, 10, r);
    specialPlusTerm(p, 116, 44932, -1, 52, 10, r);
    specialPlusTerm(p, 142, 63382, -1, 51, 10, r);
    specialPlusTerm(p, 172, 51120, -1, 50, 10, r);
    specialPlusTerm(p, 206, 173563, -1, 49, 10, r);
    specialPlusTerm(p, 244, 500607, -1, 48, 10, r);
    specialPlusTerm(p, 286, 986083, -1, 47, 10, r);
    specialPlusTerm(p, 333, 449096, -1, 46, 10, r);
    specialPlusTerm(p, 383, 558571, -1, 45, 10, r);
    specialPlusTerm(p, 436, 822461, -1, 44, 10, r);
    specialPlusTerm(p, 492, 582939, -1, 43, 10, r);
    specialPlusTerm(p, 550, 18832, -1, 42, 10, r);
    specialPlusTerm(p, 608, 156141, -1, 41, 10, r);
    specialPlusTerm(p, 665, 887167, -1, 40, 10, r);
    specialPlusTerm(p, 721, 997807, -1, 39, 10, r);
    specialPlusTerm(p, 775, 202852, -1, 38, 10, r);
    specialPlusTerm(p, 824, 187996, -1, 37, 10, r);
    specialPlusTerm(p, 867, 657138, -1, 36, 10, r);
    specialPlusTerm(p, 904, 382945, -1, 35, 10, r);
    specialPlusTerm(p, 933, 258541, -1, 34, 10, r);
    specialPlusTerm(p, 953, 347368, -1, 33, 10, r);
    specialPlusTerm(p, 963, 928871, -1, 32, 10, r);
    specialPlusTerm(p, 964, 536834, -1, 31, 10, r);
    specialPlusTerm(p, 954, 987850, -1, 30, 10, r);
    specialPlusTerm(p, 935, 397536, -1, 29, 10, r);
    specialPlusTerm(p, 906, 183073, -1, 28, 10, r);
    specialPlusTerm(p, 868, 50862, -1, 27, 10, r);
    specialPlusTerm(p, 821, 970338, -1, 26, 10, r);
    specialPlusTerm(p, 769, 135827, -1, 25, 10, r);
    specialPlusTerm(p, 710, 918057, -1, 24, 10, r);
    specialPlusTerm(p, 648, 808233, -1, 23, 10, r);
    specialPlusTerm(p, 584, 357073, -1, 22, 10, r);
    specialPlusTerm(p, 519, 112032, -1, 21, 10, r);
    specialPlusTerm(p, 454, 555728, -1, 20, 10, r);
    specialPlusTerm(p, 392, 49009, -1, 19, 10, r);
    specialPlusTerm(p, 332, 781273, -1, 18, 10, r);
    specialPlusTerm(p, 277, 731388, -1, 17, 10, r);
    specialPlusTerm(p, 227, 641117, -1, 16, 10, r);
    specialPlusTerm(p, 183, 2853, -1, 15, 10, r);
    specialPlusTerm(p, 144, 62127, -1, 14, 10, r);
    specialPlusTerm(p, 110, 834647, -1, 13, 10, r);
    specialPlusTerm(p, 83, 135184, -1, 12, 10, r);
    specialPlusTerm(p, 60, 614741, -1, 11, 10, r);
    specialPlusTerm(p, 42, 799119, -1, 10, 10, r);
    specialPlusTerm(p, 29, 127956, -1, 9, 10, r);
    specialPlusTerm(p, 18, 992545, -1, 8, 10, r);
    specialPlusTerm(p, 11, 771474, -1, 7, 10, r);
    specialPlusTerm(p, 6, 862141, -1, 6, 10, r);
    specialPlusTerm(p, 3, 707704, -1, 5, 10, r);
    specialPlusTerm(p, 1, 817816, -1, 4, 10, r);
    specialPlusTerm(p, 0, 782782, -1, 3, 10, r);
    specialPlusTerm(p, 0, 280280, -1, 2, 10, r);
    specialPlusTerm(p, 0, 75075, -1, 1, 10, r);
    specialPlusTerm(p, 0, 11440, -1, 0, 10, r);
    specialPlusTerm(p, 0, 1, 1, 92, 9, r);
    specialPlusTerm(p, 0, 9, 1, 91, 9, r);
    specialPlusTerm(p, 0, 46, 1, 90, 9, r);
    specialPlusTerm(p, 0, 175, 1, 89, 9, r);
    specialPlusTerm(p, 0, 551, 1, 88, 9, r);
    specialPlusTerm(p, 0, 1517, 1, 87, 9, r);
    specialPlusTerm(p, 0, 3775, 1, 86, 9, r);
    specialPlusTerm(p, 0, 8677, 1, 85, 9, r);
    specialPlusTerm(p, 0, 18703, 1, 84, 9, r);
    specialPlusTerm(p, 0, 38212, 1, 83, 9, r);
    specialPlusTerm(p, 0, 74581, 1, 82, 9, r);
    specialPlusTerm(p, 0, 139877, 1, 81, 9, r);
    specialPlusTerm(p, 0, 253243, 1, 80, 9, r);
    specialPlusTerm(p, 0, 444211, 1, 79, 9, r);
    specialPlusTerm(p, 0, 757189, 1, 78, 9, r);
    specialPlusTerm(p, 1, 257386, 1, 77, 9, r);
    specialPlusTerm(p, 2, 38445, 1, 76, 9, r);
    specialPlusTerm(p, 3, 232029, 1, 75, 9, r);
    specialPlusTerm(p, 5, 19547, 1, 74, 9, r);
    specialPlusTerm(p, 7, 646140, 1, 73, 9, r);
    specialPlusTerm(p, 11, 436924, 1, 72, 9, r);
    specialPlusTerm(p, 16, 815312, 1, 71, 9, r);
    specialPlusTerm(p, 24, 323016, 1, 70, 9, r);
    specialPlusTerm(p, 34, 641066, 1, 69, 9, r);
    specialPlusTerm(p, 48, 610873, 1, 68, 9, r);
    specialPlusTerm(p, 67, 254061, 1, 67, 9, r);
    specialPlusTerm(p, 91, 789450, 1, 66, 9, r);
    specialPlusTerm(p, 123, 645277, 1, 65, 9, r);
    specialPlusTerm(p, 164, 464508, 1, 64, 9, r);
    specialPlusTerm(p, 216, 101017, 1, 63, 9, r);
    specialPlusTerm(p, 280, 604281, 1, 62, 9, r);
    specialPlusTerm(p, 360, 190441, 1, 61, 9, r);
    specialPlusTerm(p, 457, 197865, 1, 60, 9, r);
    specialPlusTerm(p, 574, 25922, 1, 59, 9, r);
    specialPlusTerm(p, 713, 56365, 1, 58, 9, r);
    specialPlusTerm(p, 876, 557703, 1, 57, 9, r);
    specialPlusTerm(p, 1066, 573992, 1, 56, 9, r);
    specialPlusTerm(p, 1284, 800887, 1, 55, 9, r);
    specialPlusTerm(p, 1532, 453056, 1, 54, 9, r);
    specialPlusTerm(p, 1810, 128450, 1, 53, 9, r);
    specialPlusTerm(p, 2117, 675954, 1, 52, 9, r);
    specialPlusTerm(p, 2454, 73820, 1, 51, 9, r);
    specialPlusTerm(p, 2817, 326941, 1, 50, 9, r);
    specialPlusTerm(p, 3204, 391243, 1, 49, 9, r);
    specialPlusTerm(p, 3611, 133043, 1, 48, 9, r);
    specialPlusTerm(p, 4032, 330356, 1, 47, 9, r);
    specialPlusTerm(p, 4461, 721846, 1, 46, 9, r);
    specialPlusTerm(p, 4892, 107024, 1, 45, 9, r);
    specialPlusTerm(p, 5315, 498836, 1, 44, 9, r);
    specialPlusTerm(p, 5723, 326840, 1, 43, 9, r);
    specialPlusTerm(p, 6106, 686025, 1, 42, 9, r);
    specialPlusTerm(p, 6456, 623000, 1, 41, 9, r);
    specialPlusTerm(p, 6764, 448368, 1, 40, 9, r);
    specialPlusTerm(p, 7022, 61523, 1, 39, 9, r);
    specialPlusTerm(p, 7222, 273135, 1, 38, 9, r);
    specialPlusTerm(p, 7359, 108966, 1, 37, 9, r);
    specialPlusTerm(p, 7428, 79054, 1, 36, 9, r);
    specialPlusTerm(p, 7426, 396980, 1, 35, 9, r);
    specialPlusTerm(p, 7353, 135921, 1, 34, 9, r);
    specialPlusTerm(p, 7209, 310506, 1, 33, 9, r);
    specialPlusTerm(p, 6997, 877688, 1, 32, 9, r);
    specialPlusTerm(p, 6723, 653256, 1, 31, 9, r);
    specialPlusTerm(p, 6393, 146079, 1, 30, 9, r);
    specialPlusTerm(p, 6014, 316858, 1, 29, 9, r);
    specialPlusTerm(p, 5596, 273275, 1, 28, 9, r);
    specialPlusTerm(p, 5148, 917160, 1, 27, 9, r);
    specialPlusTerm(p, 4682, 563401, 1, 26, 9, r);
    specialPlusTerm(p, 4207, 550601, 1, 25, 9, r);
    specialPlusTerm(p, 3733, 862769, 1, 24, 9, r);
    specialPlusTerm(p, 3270, 780702, 1, 23, 9, r);
    specialPlusTerm(p, 2826, 579105, 1, 22, 9, r);
    specialPlusTerm(p, 2408, 283186, 1, 21, 9, r);
    specialPlusTerm(p, 2021, 494926, 1, 20, 9, r);
    specialPlusTerm(p, 1670, 295297, 1, 19, 9, r);
    specialPlusTerm(p, 1357, 224099, 1, 18, 9, r);
    specialPlusTerm(p, 1083, 335088, 1, 17, 9, r);
    specialPlusTerm(p, 848, 318658, 1, 16, 9, r);
    specialPlusTerm(p, 650, 680536, 1, 15, 9, r);
    specialPlusTerm(p, 487, 961232, 1, 14, 9, r);
    specialPlusTerm(p, 356, 978589, 1, 13, 9, r);
    specialPlusTerm(p, 254, 74425, 1, 12, 9, r);
    specialPlusTerm(p, 175, 348338, 1, 11, 9, r);
    specialPlusTerm(p, 116, 865243, 1, 10, 9, r);
    specialPlusTerm(p, 74, 831757, 1, 9, 9, r);
    specialPlusTerm(p, 45, 737406, 1, 8, 9, r);
    specialPlusTerm(p, 26, 458575, 1, 7, 9, r);
    specialPlusTerm(p, 14, 324310, 1, 6, 9, r);
    specialPlusTerm(p, 7, 145853, 1, 5, 9, r);
    specialPlusTerm(p, 3, 212352, 1, 4, 9, r);
    specialPlusTerm(p, 1, 257828, 1, 3, 9, r);
    specialPlusTerm(p, 0, 405405, 1, 2, 9, r);
    specialPlusTerm(p, 0, 96525, 1, 1, 9, r);
    specialPlusTerm(p, 0, 12870, 1, 0, 9, r);
    specialPlusTerm(p, 0, 1, -1, 99, 8, r);
    specialPlusTerm(p, 0, 10, -1, 98, 8, r);
    specialPlusTerm(p, 0, 56, -1, 97, 8, r);
    specialPlusTerm(p, 0, 231, -1, 96, 8, r);
    specialPlusTerm(p, 0, 782, -1, 95, 8, r);
    specialPlusTerm(p, 0, 2299, -1, 94, 8, r);
    specialPlusTerm(p, 0, 6074, -1, 93, 8, r);
    specialPlusTerm(p, 0, 14751, -1, 92, 8, r);
    specialPlusTerm(p, 0, 33445, -1, 91, 8, r);
    specialPlusTerm(p, 0, 71586, -1, 90, 8, r);
    specialPlusTerm(p, 0, 145843, -1, 89, 8, r);
    specialPlusTerm(p, 0, 284605, -1, 88, 8, r);
    specialPlusTerm(p, 0, 534639, -1, 87, 8, r);
    specialPlusTerm(p, 0, 970707, -1, 86, 8, r);
    specialPlusTerm(p, 1, 709091, -1, 85, 8, r);
    specialPlusTerm(p, 2, 926134, -1, 84, 8, r);
    specialPlusTerm(p, 4, 883022, -1, 83, 8, r);
    specialPlusTerm(p, 7, 958118, -1, 82, 8, r);
    specialPlusTerm(p, 12, 688161, -1, 81, 8, r);
    specialPlusTerm(p, 19, 819543, -1, 80, 8, r);
    specialPlusTerm(p, 30, 370620, -1, 79, 8, r);
    specialPlusTerm(p, 45, 705588, -1, 78, 8, r);
    specialPlusTerm(p, 67, 619803, -1, 77, 8, r);
    specialPlusTerm(p, 98, 435565, -1, 76, 8, r);
    specialPlusTerm(p, 141, 106293, -1, 75, 8, r);
    specialPlusTerm(p, 199, 325748, -1, 74, 8, r);
    specialPlusTerm(p, 277, 637550, -1, 73, 8, r);
    specialPlusTerm(p, 381, 538692, -1, 72, 8, r);
    specialPlusTerm(p, 517, 569184, -1, 71, 8, r);
    specialPlusTerm(p, 693, 378528, -1, 70, 8, r);
    specialPlusTerm(p, 917, 758545, -1, 69, 8, r);
    specialPlusTerm(p, 1200, 631327, -1, 68, 8, r);
    specialPlusTerm(p, 1552, 980935, -1, 67, 8, r);
    specialPlusTerm(p, 1986, 718008, -1, 66, 8, r);
    specialPlusTerm(p, 2514, 467898, -1, 65, 8, r);
    specialPlusTerm(p, 3149, 275373, -1, 64, 8, r);
    specialPlusTerm(p, 3904, 222333, -1, 63, 8, r);
    specialPlusTerm(p, 4791, 959202, -1, 62, 8, r);
    specialPlusTerm(p, 5824, 155954, -1, 61, 8, r);
    specialPlusTerm(p, 7010, 884530, -1, 60, 8, r);
    specialPlusTerm(p, 8359, 950639, -1, 59, 8, r);
    specialPlusTerm(p, 9876, 199091, -1, 58, 8, r);
    specialPlusTerm(p, 11560, 822647, -1, 57, 8, r);
    specialPlusTerm(p, 13410, 709233, -1, 56, 8, r);
    specialPlusTerm(p, 15417, 866033, -1, 55, 8, r);
    specialPlusTerm(p, 17568, 960466, -1, 54, 8, r);
    specialPlusTerm(p, 19845, 17463, -1, 53, 8, r);
    specialPlusTerm(p, 22221, 309147, -1, 52, 8, r);
    specialPlusTerm(p, 24667, 467342, -1, 51, 8, r);
    specialPlusTerm(p, 27147, 841107, -1, 50, 8, r);
    specialPlusTerm(p, 29622, 110916, -1, 49, 8, r);
    specialPlusTerm(p, 32046, 158421, -1, 48, 8, r);
    specialPlusTerm(p, 34373, 176970, -1, 47, 8, r);
    specialPlusTerm(p, 36554, 993654, -1, 46, 8, r);
    specialPlusTerm(p, 38543, 559373, -1, 45, 8, r);
    specialPlusTerm(p, 40292, 550458, -1, 44, 8, r);
    specialPlusTerm(p, 41759, 14716, -1, 43, 8, r);
    specialPlusTerm(p, 42904, 987186, -1, 42, 8, r);
    specialPlusTerm(p, 43698, 997388, -1, 41, 8, r);
    specialPlusTerm(p, 44117, 391022, -1, 40, 8, r);
    specialPlusTerm(p, 44145, 394986, -1, 39, 8, r);
    specialPlusTerm(p, 43777, 865188, -1, 38, 8, r);
    specialPlusTerm(p, 43019, 670206, -1, 37, 8, r);
    specialPlusTerm(p, 41885, 681598, -1, 36, 8, r);
    specialPlusTerm(p, 40400, 361216, -1, 35, 8, r);
    specialPlusTerm(p, 38596, 956954, -1, 34, 8, r);
    specialPlusTerm(p, 36516, 339240, -1, 33, 8, r);
    specialPlusTerm(p, 34205, 530818, -1, 32, 8, r);
    specialPlusTerm(p, 31715, 999202, -1, 31, 8, r);
    specialPlusTerm(p, 29101, 795368, -1, 30, 8, r);
    specialPlusTerm(p, 26417, 630514, -1, 29, 8, r);
    specialPlusTerm(p, 23716, 985664, -1, 28, 8, r);
    specialPlusTerm(p, 21050, 345182, -1, 27, 8, r);
    specialPlusTerm(p, 18463, 636290, -1, 26, 8, r);
    specialPlusTerm(p, 15996, 940992, -1, 25, 8, r);
    specialPlusTerm(p, 13683, 529134, -1, 24, 8, r);
    specialPlusTerm(p, 11549, 242308, -1, 23, 8, r);
    specialPlusTerm(p, 9612, 238032, -1, 22, 8, r);
    specialPlusTerm(p, 7883, 84208, -1, 21, 8, r);
    specialPlusTerm(p, 6365, 175366, -1, 20, 8, r);
    specialPlusTerm(p, 5055, 425826, -1, 19, 8, r);
    specialPlusTerm(p, 3945, 181834, -1, 18, 8, r);
    specialPlusTerm(p, 3021, 285630, -1, 17, 8, r);
    specialPlusTerm(p, 2267, 219328, -1, 16, 8, r);
    specialPlusTerm(p, 1664, 257320, -1, 15, 8, r);
    specialPlusTerm(p, 1192, 561524, -1, 14, 8, r);
    specialPlusTerm(p, 832, 164102, -1, 13, 8, r);
    specialPlusTerm(p, 563, 796684, -1, 12, 8, r);
    specialPlusTerm(p, 369, 542514, -1, 11, 8, r);
    specialPlusTerm(p, 233, 304720, -1, 10, 8, r);
    specialPlusTerm(p, 141, 98386, -1, 9, 8, r);
    specialPlusTerm(p, 81, 180528, -1, 8, 8, r);
    specialPlusTerm(p, 44, 38137, -1, 7, 8, r);
    specialPlusTerm(p, 22, 259094, -1, 6, 8, r);
    specialPlusTerm(p, 10, 314447, -1, 5, 8, r);
    specialPlusTerm(p, 4, 281420, -1, 4, 8, r);
    specialPlusTerm(p, 1, 537107, -1, 3, 8, r);
    specialPlusTerm(p, 0, 450450, -1, 2, 8, r);
    specialPlusTerm(p, 0, 96525, -1, 1, 8, r);
    specialPlusTerm(p, 0, 11440, -1, 0, 8, r);
    specialPlusTerm(p, 0, 1, 1, 105, 7, r);
    specialPlusTerm(p, 0, 11, 1, 104, 7, r);
    specialPlusTerm(p, 0, 67, 1, 103, 7, r);
    specialPlusTerm(p, 0, 298, 1, 102, 7, r);
    specialPlusTerm(p, 0, 1080, 1, 101, 7, r);
    specialPlusTerm(p, 0, 3379, 1, 100, 7, r);
    specialPlusTerm(p, 0, 9453, 1, 99, 7, r);
    specialPlusTerm(p, 0, 24196, 1, 98, 7, r);
    specialPlusTerm(p, 0, 57569, 1, 97, 7, r);
    specialPlusTerm(p, 0, 128787, 1, 96, 7, r);
    specialPlusTerm(p, 0, 273231, 1, 95, 7, r);
    specialPlusTerm(p, 0, 553438, 1, 94, 7, r);
    specialPlusTerm(p, 1, 75997, 1, 93, 7, r);
    specialPlusTerm(p, 2, 16735, 1, 92, 7, r);
    specialPlusTerm(p, 3, 657192, 1, 91, 7, r);
    specialPlusTerm(p, 6, 436037, 1, 90, 7, r);
    specialPlusTerm(p, 11, 19717, 1, 89, 7, r);
    specialPlusTerm(p, 18, 397189, 1, 88, 7, r);
    specialPlusTerm(p, 30, 3973, 1, 87, 7, r);
    specialPlusTerm(p, 47, 880870, 1, 86, 7, r);
    specialPlusTerm(p, 74, 872383, 1, 85, 7, r);
    specialPlusTerm(p, 114, 869036, 1, 84, 7, r);
    specialPlusTerm(p, 173, 96283, 1, 83, 7, r);
    specialPlusTerm(p, 256, 450472, 1, 82, 7, r);
    specialPlusTerm(p, 373, 879259, 1, 81, 7, r);
    specialPlusTerm(p, 536, 799939, 1, 80, 7, r);
    specialPlusTerm(p, 759, 544391, 1, 79, 7, r);
    specialPlusTerm(p, 1059, 813880, 1, 78, 7, r);
    specialPlusTerm(p, 1459, 121011, 1, 77, 7, r);
    specialPlusTerm(p, 1983, 190069, 1, 76, 7, r);
    specialPlusTerm(p, 2662, 281192, 1, 75, 7, r);
    specialPlusTerm(p, 3531, 398853, 1, 74, 7, r);
    specialPlusTerm(p, 4630, 341477, 1, 73, 7, r);
    specialPlusTerm(p, 6003, 547268, 1, 72, 7, r);
    specialPlusTerm(p, 7699, 692101, 1, 71, 7, r);
    specialPlusTerm(p, 9770, 999253, 1, 70, 7, r);
    specialPlusTerm(p, 12272, 228155, 1, 69, 7, r);
    specialPlusTerm(p, 15259, 320516, 1, 68, 7, r);
    specialPlusTerm(p, 18787, 697047, 1, 67, 7, r);
    specialPlusTerm(p, 22910, 216326, 1, 66, 7, r);
    specialPlusTerm(p, 27674, 828551, 1, 65, 7, r);
    specialPlusTerm(p, 33121, 980096, 1, 64, 7, r);
    specialPlusTerm(p, 39281, 848637, 1, 63, 7, r);
    specialPlusTerm(p, 46171, 511745, 1, 62, 7, r);
    specialPlusTerm(p, 53792, 172793, 1, 61, 7, r);
    specialPlusTerm(p, 62126, 584715, 1, 60, 7, r);
    specialPlusTerm(p, 71136, 823074, 1, 59, 7, r);
    specialPlusTerm(p, 80762, 563409, 1, 58, 7, r);
    specialPlusTerm(p, 90920, 12755, 1, 57, 7, r);
    specialPlusTerm(p, 101501, 630586, 1, 56, 7, r);
    specialPlusTerm(p, 112376, 750037, 1, 55, 7, r);
    specialPlusTerm(p, 123393, 176185, 1, 54, 7, r);
    specialPlusTerm(p, 134379, 795971, 1, 53, 7, r);
    specialPlusTerm(p, 145150, 185558, 1, 52, 7, r);
    specialPlusTerm(p, 155507, 148257, 1, 51, 7, r);
    specialPlusTerm(p, 165248, 62169, 1, 50, 7, r);
    specialPlusTerm(p, 174170, 864688, 1, 49, 7, r);
    specialPlusTerm(p, 182080, 454274, 1, 48, 7, r);
    specialPlusTerm(p, 188795, 252141, 1, 47, 7, r);
    specialPlusTerm(p, 194153, 640367, 1, 46, 7, r);
    specialPlusTerm(p, 198019, 981033, 1, 45, 7, r);
    specialPlusTerm(p, 200289, 925212, 1, 44, 7, r);
    specialPlusTerm(p, 200894, 741748, 1, 43, 7, r);
    specialPlusTerm(p, 199804, 433264, 1, 42, 7, r);
    specialPlusTerm(p, 197029, 459520, 1, 41, 7, r);
    specialPlusTerm(p, 192620, 953188, 1, 40, 7, r);
    specialPlusTerm(p, 186669, 386562, 1, 39, 7, r);
    specialPlusTerm(p, 179301, 725204, 1, 38, 7, r);
    specialPlusTerm(p, 170677, 181240, 1, 37, 7, r);
    specialPlusTerm(p, 160981, 751274, 1, 36, 7, r);
    specialPlusTerm(p, 150421, 786460, 1, 35, 7, r);
    specialPlusTerm(p, 139216, 891710, 1, 34, 7, r);
    specialPlusTerm(p, 127592, 483930, 1, 33, 7, r);
    specialPlusTerm(p, 115772, 353582, 1, 32, 7, r);
    specialPlusTerm(p, 103971, 568098, 1, 31, 7, r);
    specialPlusTerm(p, 92390, 30732, 1, 30, 7, r);
    specialPlusTerm(p, 81206, 964790, 1, 29, 7, r);
    specialPlusTerm(p, 70576, 534794, 1, 28, 7, r);
    specialPlusTerm(p, 60624, 746714, 1, 27, 7, r);
    specialPlusTerm(p, 51447, 694586, 1, 26, 7, r);
    specialPlusTerm(p, 43111, 145244, 1, 25, 7, r);
    specialPlusTerm(p, 35651, 383472, 1, 24, 7, r);
    specialPlusTerm(p, 29077, 179220, 1, 23, 7, r);
    specialPlusTerm(p, 23372, 689230, 1, 22, 7, r);
    specialPlusTerm(p, 18501, 69995, 1, 21, 7, r);
    specialPlusTerm(p, 14408, 558395, 1, 20, 7, r);
    specialPlusTerm(p, 11028, 771237, 1, 19, 7, r);
    specialPlusTerm(p, 8286, 985223, 1, 18, 7, r);
    specialPlusTerm(p, 6104, 182964, 1, 17, 7, r);
    specialPlusTerm(p, 4400, 686554, 1, 16, 7, r);
    specialPlusTerm(p, 3099, 245270, 1, 15, 7, r);
    specialPlusTerm(p, 2127, 493720, 1, 14, 7, r);
    specialPlusTerm(p, 1419, 747120, 1, 13, 7, r);
    specialPlusTerm(p, 918, 147362, 1, 12, 7, r);
    specialPlusTerm(p, 573, 212794, 1, 11, 7, r);
    specialPlusTerm(p, 343, 872738, 1, 10, 7, r);
    specialPlusTerm(p, 197, 84030, 1, 9, 7, r);
    specialPlusTerm(p, 107, 132168, 1, 8, 7, r);
    specialPlusTerm(p, 54, 719808, 1, 7, 7, r);
    specialPlusTerm(p, 25, 940200, 1, 6, 7, r);
    specialPlusTerm(p, 11, 223212, 1, 5, 7, r);
    specialPlusTerm(p, 4, 327323, 1, 4, 7, r);
    specialPlusTerm(p, 1, 434433, 1, 3, 7, r);
    specialPlusTerm(p, 0, 385385, 1, 2, 7, r);
    specialPlusTerm(p, 0, 75075, 1, 1, 7, r);
    specialPlusTerm(p, 0, 8008, 1, 0, 7, r);
    specialPlusTerm(p, 0, 1, -1, 110, 6, r);
    specialPlusTerm(p, 0, 12, -1, 109, 6, r);
    specialPlusTerm(p, 0, 79, -1, 108, 6, r);
    specialPlusTerm(p, 0, 377, -1, 107, 6, r);
    specialPlusTerm(p, 0, 1457, -1, 106, 6, r);
    specialPlusTerm(p, 0, 4836, -1, 105, 6, r);
    specialPlusTerm(p, 0, 14282, -1, 104, 6, r);
    specialPlusTerm(p, 0, 38408, -1, 103, 6, r);
    specialPlusTerm(p, 0, 95585, -1, 102, 6, r);
    specialPlusTerm(p, 0, 222755, -1, 101, 6, r);
    specialPlusTerm(p, 0, 490512, -1, 100, 6, r);
    specialPlusTerm(p, 1, 27858, -1, 99, 6, r);
    specialPlusTerm(p, 2, 61348, -1, 98, 6, r);
    specialPlusTerm(p, 3, 974921, -1, 97, 6, r);
    specialPlusTerm(p, 7, 398548, -1, 96, 6, r);
    specialPlusTerm(p, 13, 335851, -1, 95, 6, r);
    specialPlusTerm(p, 23, 342939, -1, 94, 6, r);
    specialPlusTerm(p, 39, 772696, -1, 93, 6, r);
    specialPlusTerm(p, 66, 100395, -1, 92, 6, r);
    specialPlusTerm(p, 107, 347509, -1, 91, 6, r);
    specialPlusTerm(p, 170, 620609, -1, 90, 6, r);
    specialPlusTerm(p, 265, 780891, -1, 89, 6, r);
    specialPlusTerm(p, 406, 256761, -1, 88, 6, r);
    specialPlusTerm(p, 610, 6662, -1, 87, 6, r);
    specialPlusTerm(p, 900, 631638, -1, 86, 6, r);
    specialPlusTerm(p, 1308, 626820, -1, 85, 6, r);
    specialPlusTerm(p, 1872, 748088, -1, 84, 6, r);
    specialPlusTerm(p, 2641, 454830, -1, 83, 6, r);
    specialPlusTerm(p, 3674, 372459, -1, 82, 6, r);
    specialPlusTerm(p, 5043, 699859, -1, 81, 6, r);
    specialPlusTerm(p, 6835, 468267, -1, 80, 6, r);
    specialPlusTerm(p, 9150, 540598, -1, 79, 6, r);
    specialPlusTerm(p, 12105, 225506, -1, 78, 6, r);
    specialPlusTerm(p, 15831, 370284, -1, 77, 6, r);
    specialPlusTerm(p, 20475, 792894, -1, 76, 6, r);
    specialPlusTerm(p, 26198, 917672, -1, 75, 6, r);
    specialPlusTerm(p, 33172, 493074, -1, 74, 6, r);
    specialPlusTerm(p, 41576, 294268, -1, 73, 6, r);
    specialPlusTerm(p, 51593, 749027, -1, 72, 6, r);
    specialPlusTerm(p, 63406, 472134, -1, 71, 6, r);
    specialPlusTerm(p, 77187, 750422, -1, 70, 6, r);
    specialPlusTerm(p, 93095, 85649, -1, 69, 6, r);
    specialPlusTerm(p, 111261, 972846, -1, 68, 6, r);
    specialPlusTerm(p, 131789, 163843, -1, 67, 6, r);
    specialPlusTerm(p, 154735, 735031, -1, 66, 6, r);
    specialPlusTerm(p, 180110, 340164, -1, 65, 6, r);
    specialPlusTerm(p, 207863, 78032, -1, 64, 6, r);
    specialPlusTerm(p, 237878, 436148, -1, 63, 6, r);
    specialPlusTerm(p, 269969, 780853, -1, 62, 6, r);
    specialPlusTerm(p, 303875, 847919, -1, 61, 6, r);
    specialPlusTerm(p, 339259, 643405, -1, 60, 6, r);
    specialPlusTerm(p, 375710, 91709, -1, 59, 6, r);
    specialPlusTerm(p, 412746, 667621, -1, 58, 6, r);
    specialPlusTerm(p, 449827, 124838, -1, 57, 6, r);
    specialPlusTerm(p, 486358, 289884, -1, 56, 6, r);
    specialPlusTerm(p, 521709, 734617, -1, 55, 6, r);
    specialPlusTerm(p, 555229, 980825, -1, 54, 6, r);
    specialPlusTerm(p, 586264, 736452, -1, 53, 6, r);
    specialPlusTerm(p, 614176, 524327, -1, 52, 6, r);
    specialPlusTerm(p, 638364, 950461, -1, 51, 6, r);
    specialPlusTerm(p, 658286, 778382, -1, 50, 6, r);
    specialPlusTerm(p, 673474, 935703, -1, 49, 6, r);
    specialPlusTerm(p, 683555, 584035, -1, 48, 6, r);
    specialPlusTerm(p, 688262, 435939, -1, 47, 6, r);
    specialPlusTerm(p, 687447, 601958, -1, 46, 6, r);
    specialPlusTerm(p, 681088, 393321, -1, 45, 6, r);
    specialPlusTerm(p, 669289, 685002, -1, 44, 6, r);
    specialPlusTerm(p, 652281, 650056, -1, 43, 6, r);
    specialPlusTerm(p, 630412, 897910, -1, 42, 6, r);
    specialPlusTerm(p, 604139, 273828, -1, 41, 6, r);
    specialPlusTerm(p, 574008, 790372, -1, 40, 6, r);
    specialPlusTerm(p, 540643, 351434, -1, 39, 6, r);
    specialPlusTerm(p, 504718, 83850, -1, 38, 6, r);
    specialPlusTerm(p, 466939, 201686, -1, 37, 6, r);
    specialPlusTerm(p, 428021, 387818, -1, 36, 6, r);
    specialPlusTerm(p, 388665, 682772, -1, 35, 6, r);
    specialPlusTerm(p, 349538, 822179, -1, 34, 6, r);
    specialPlusTerm(p, 311254, 865006, -1, 33, 6, r);
    specialPlusTerm(p, 274359, 811613, -1, 32, 6, r);
    specialPlusTerm(p, 239319, 732987, -1, 31, 6, r);
    specialPlusTerm(p, 206512, 732148, -1, 30, 6, r);
    specialPlusTerm(p, 176224, 848533, -1, 29, 6, r);
    specialPlusTerm(p, 148649, 810169, -1, 28, 6, r);
    specialPlusTerm(p, 123892, 349306, -1, 27, 6, r);
    specialPlusTerm(p, 101974, 636229, -1, 26, 6, r);
    specialPlusTerm(p, 82845, 261439, -1, 25, 6, r);
    specialPlusTerm(p, 66390, 113996, -1, 24, 6, r);
    specialPlusTerm(p, 52444, 464455, -1, 23, 6, r);
    specialPlusTerm(p, 40805, 563557, -1, 22, 6, r);
    specialPlusTerm(p, 31245, 109466, -1, 21, 6, r);
    specialPlusTerm(p, 23521, 11382, -1, 20, 6, r);
    specialPlusTerm(p, 17387, 978784, -1, 19, 6, r);
    specialPlusTerm(p, 12606, 585145, -1, 18, 6, r);
    specialPlusTerm(p, 8950, 583389, -1, 17, 6, r);
    specialPlusTerm(p, 6212, 378502, -1, 16, 6, r);
    specialPlusTerm(p, 4206, 681655, -1, 15, 6, r);
    specialPlusTerm(p, 2772, 472065, -1, 14, 6, r);
    specialPlusTerm(p, 1773, 471986, -1, 13, 6, r);
    specialPlusTerm(p, 1097, 393517, -1, 12, 6, r);
    specialPlusTerm(p, 654, 242447, -1, 11, 6, r);
    specialPlusTerm(p, 373, 966384, -1, 10, 6, r);
    specialPlusTerm(p, 203, 716799, -1, 9, 6, r);
    specialPlusTerm(p, 104, 963287, -1, 8, 6, r);
    specialPlusTerm(p, 50, 659180, -1, 7, 6, r);
    specialPlusTerm(p, 22, 613305, -1, 6, 6, r);
    specialPlusTerm(p, 9, 176167, -1, 5, 6, r);
    specialPlusTerm(p, 3, 303300, -1, 4, 6, r);
    specialPlusTerm(p, 1, 17016, -1, 3, 6, r);
    specialPlusTerm(p, 0, 252252, -1, 2, 6, r);
    specialPlusTerm(p, 0, 45045, -1, 1, 6, r);
    specialPlusTerm(p, 0, 4368, -1, 0, 6, r);
    specialPlusTerm(p, 0, 1, 1, 114, 5, r);
    specialPlusTerm(p, 0, 13, 1, 113, 5, r);
    specialPlusTerm(p, 0, 92, 1, 112, 5, r);
    specialPlusTerm(p, 0, 469, 1, 111, 5, r);
    specialPlusTerm(p, 0, 1926, 1, 110, 5, r);
    specialPlusTerm(p, 0, 6756, 1, 109, 5, r);
    specialPlusTerm(p, 0, 20972, 1, 108, 5, r);
    specialPlusTerm(p, 0, 58978, 1, 107, 5, r);
    specialPlusTerm(p, 0, 152775, 1, 106, 5, r);
    specialPlusTerm(p, 0, 369050, 1, 105, 5, r);
    specialPlusTerm(p, 0, 839288, 1, 104, 5, r);
    specialPlusTerm(p, 1, 810449, 1, 103, 5, r);
    specialPlusTerm(p, 3, 726832, 1, 102, 5, r);
    specialPlusTerm(p, 7, 357527, 1, 101, 5, r);
    specialPlusTerm(p, 13, 988283, 1, 100, 5, r);
    specialPlusTerm(p, 25, 701547, 1, 99, 5, r);
    specialPlusTerm(p, 45, 773588, 1, 98, 5, r);
    specialPlusTerm(p, 79, 222619, 1, 97, 5, r);
    specialPlusTerm(p, 133, 546121, 1, 96, 5, r);
    specialPlusTerm(p, 219, 688458, 1, 95, 5, r);
    specialPlusTerm(p, 353, 280515, 1, 94, 5, r);
    specialPlusTerm(p, 556, 190572, 1, 93, 5, r);
    specialPlusTerm(p, 858, 418990, 1, 92, 5, r);
    specialPlusTerm(p, 1300, 357646, 1, 91, 5, r);
    specialPlusTerm(p, 1935, 417677, 1, 90, 5, r);
    specialPlusTerm(p, 2833, 5498, 1, 89, 5, r);
    specialPlusTerm(p, 4081, 797160, 1, 88, 5, r);
    specialPlusTerm(p, 5793, 225338, 1, 87, 5, r);
    specialPlusTerm(p, 8105, 52608, 1, 86, 5, r);
    specialPlusTerm(p, 11184, 860904, 1, 85, 5, r);
    specialPlusTerm(p, 15233, 242543, 1, 84, 5, r);
    specialPlusTerm(p, 20486, 436025, 1, 83, 5, r);
    specialPlusTerm(p, 27218, 113560, 1, 82, 5, r);
    specialPlusTerm(p, 35740, 941, 1, 81, 5, r);
    specialPlusTerm(p, 46400, 998184, 1, 80, 5, r);
    specialPlusTerm(p, 59584, 475380, 1, 79, 5, r);
    specialPlusTerm(p, 75703, 446044, 1, 78, 5, r);
    specialPlusTerm(p, 95193, 372588, 1, 77, 5, r);
    specialPlusTerm(p, 118502, 436837, 1, 76, 5, r);
    specialPlusTerm(p, 146079, 212557, 1, 75, 5, r);
    specialPlusTerm(p, 178357, 804753, 1, 74, 5, r);
    specialPlusTerm(p, 215740, 667987, 1, 73, 5, r);
    specialPlusTerm(p, 258579, 477117, 1, 72, 5, r);
    specialPlusTerm(p, 307154, 590636, 1, 71, 5, r);
    specialPlusTerm(p, 361653, 809467, 1, 70, 5, r);
    specialPlusTerm(p, 422151, 281668, 1, 69, 5, r);
    specialPlusTerm(p, 488587, 524562, 1, 68, 5, r);
    specialPlusTerm(p, 560751, 619043, 1, 67, 5, r);
    specialPlusTerm(p, 638266, 666013, 1, 66, 5, r);
    specialPlusTerm(p, 720579, 573693, 1, 65, 5, r);
    specialPlusTerm(p, 806956, 161270, 1, 64, 5, r);
    specialPlusTerm(p, 896482, 416756, 1, 63, 5, r);
    specialPlusTerm(p, 988072, 536828, 1, 62, 5, r);
    specialPlusTerm(p, 1080484, 109735, 1, 61, 5, r);
    specialPlusTerm(p, 1172340, 489345, 1, 60, 5, r);
    specialPlusTerm(p, 1262160, 63390, 1, 59, 5, r);
    specialPlusTerm(p, 1348391, 759522, 1, 58, 5, r);
    specialPlusTerm(p, 1429455, 778912, 1, 57, 5, r);
    specialPlusTerm(p, 1503788, 220123, 1, 56, 5, r);
    specialPlusTerm(p, 1569887, 977111, 1, 55, 5, r);
    specialPlusTerm(p, 1626364, 84113, 1, 54, 5, r);
    specialPlusTerm(p, 1671981, 553512, 1, 53, 5, r);
    specialPlusTerm(p, 1705703, 722632, 1, 52, 5, r);
    specialPlusTerm(p, 1726729, 198623, 1, 51, 5, r);
    specialPlusTerm(p, 1734521, 667786, 1, 50, 5, r);
    specialPlusTerm(p, 1728831, 111168, 1, 49, 5, r);
    specialPlusTerm(p, 1709705, 329909, 1, 48, 5, r);
    specialPlusTerm(p, 1677491, 113716, 1, 47, 5, r);
    specialPlusTerm(p, 1632824, 861010, 1, 46, 5, r);
    specialPlusTerm(p, 1576612, 953462, 1, 45, 5, r);
    specialPlusTerm(p, 1510002, 672602, 1, 44, 5, r);
    specialPlusTerm(p, 1434344, 893769, 1, 43, 5, r);
    specialPlusTerm(p, 1351150, 176624, 1, 42, 5, r);
    specialPlusTerm(p, 1262040, 169393, 1, 41, 5, r);
    specialPlusTerm(p, 1168696, 438727, 1, 40, 5, r);
    specialPlusTerm(p, 1072808, 918138, 1, 39, 5, r);
    specialPlusTerm(p, 976026, 132162, 1, 38, 5, r);
    specialPlusTerm(p, 879909, 204828, 1, 37, 5, r);
    specialPlusTerm(p, 785891, 410576, 1, 36, 5, r);
    specialPlusTerm(p, 695244, 690346, 1, 35, 5, r);
    specialPlusTerm(p, 609054, 157073, 1, 34, 5, r);
    specialPlusTerm(p, 528201, 178547, 1, 33, 5, r);
    specialPlusTerm(p, 453355, 178523, 1, 32, 5, r);
    specialPlusTerm(p, 384973, 865967, 1, 31, 5, r);
    specialPlusTerm(p, 323311, 212357, 1, 30, 5, r);
    specialPlusTerm(p, 268432, 169189, 1, 29, 5, r);
    specialPlusTerm(p, 220232, 868421, 1, 28, 5, r);
    specialPlusTerm(p, 178464, 887323, 1, 27, 5, r);
    specialPlusTerm(p, 142762, 89447, 1, 26, 5, r);
    specialPlusTerm(p, 112668, 572043, 1, 25, 5, r);
    specialPlusTerm(p, 87666, 348585, 1, 24, 5, r);
    specialPlusTerm(p, 67201, 559745, 1, 23, 5, r);
    specialPlusTerm(p, 50708, 221257, 1, 22, 5, r);
    specialPlusTerm(p, 37628, 764983, 1, 21, 5, r);
    specialPlusTerm(p, 27430, 891847, 1, 20, 5, r);
    specialPlusTerm(p, 19620, 514504, 1, 19, 5, r);
    specialPlusTerm(p, 13750, 807706, 1, 18, 5, r);
    specialPlusTerm(p, 9427, 591898, 1, 17, 5, r);
    specialPlusTerm(p, 6311, 440762, 1, 16, 5, r);
    specialPlusTerm(p, 4117, 20228, 1, 15, 5, r);
    specialPlusTerm(p, 2610, 233212, 1, 14, 5, r);
    specialPlusTerm(p, 1603, 763552, 1, 13, 5, r);
    specialPlusTerm(p, 951, 590329, 1, 12, 5, r);
    specialPlusTerm(p, 542, 988511, 1, 11, 5, r);
    specialPlusTerm(p, 296, 453834, 1, 10, 5, r);
    specialPlusTerm(p, 153, 899291, 1, 9, 5, r);
    specialPlusTerm(p, 75, 376496, 1, 8, 5, r);
    specialPlusTerm(p, 34, 484437, 1, 7, 5, r);
    specialPlusTerm(p, 14, 545453, 1, 6, 5, r);
    specialPlusTerm(p, 5, 557552, 1, 5, 5, r);
    specialPlusTerm(p, 1, 876238, 1, 4, 5, r);
    specialPlusTerm(p, 0, 539266, 1, 3, 5, r);
    specialPlusTerm(p, 0, 124215, 1, 2, 5, r);
    specialPlusTerm(p, 0, 20475, 1, 1, 5, r);
    specialPlusTerm(p, 0, 1820, 1, 0, 5, r);
    specialPlusTerm(p, 0, 1, -1, 117, 4, r);
    specialPlusTerm(p, 0, 14, -1, 116, 4, r);
    specialPlusTerm(p, 0, 106, -1, 115, 4, r);
    specialPlusTerm(p, 0, 575, -1, 114, 4, r);
    specialPlusTerm(p, 0, 2496, -1, 113, 4, r);
    specialPlusTerm(p, 0, 9192, -1, 112, 4, r);
    specialPlusTerm(p, 0, 29769, -1, 111, 4, r);
    specialPlusTerm(p, 0, 86862, -1, 110, 4, r);
    specialPlusTerm(p, 0, 232352, -1, 109, 4, r);
    specialPlusTerm(p, 0, 577237, -1, 108, 4, r);
    specialPlusTerm(p, 1, 345280, -1, 107, 4, r);
    specialPlusTerm(p, 2, 964694, -1, 106, 4, r);
    specialPlusTerm(p, 6, 218071, -1, 105, 4, r);
    specialPlusTerm(p, 12, 478024, -1, 104, 4, r);
    specialPlusTerm(p, 24, 64445, -1, 103, 4, r);
    specialPlusTerm(p, 44, 768554, -1, 102, 4, r);
    specialPlusTerm(p, 80, 598456, -1, 101, 4, r);
    specialPlusTerm(p, 140, 809905, -1, 100, 4, r);
    specialPlusTerm(p, 239, 293294, -1, 99, 4, r);
    specialPlusTerm(p, 396, 392192, -1, 98, 4, r);
    specialPlusTerm(p, 641, 228471, -1, 97, 4, r);
    specialPlusTerm(p, 1014, 602512, -1, 96, 4, r);
    specialPlusTerm(p, 1572, 522451, -1, 95, 4, r);
    specialPlusTerm(p, 2390, 392393, -1, 94, 4, r);
    specialPlusTerm(p, 3567, 854826, -1, 93, 4, r);
    specialPlusTerm(p, 5234, 236531, -1, 92, 4, r);
    specialPlusTerm(p, 7554, 490334, -1, 91, 4, r);
    specialPlusTerm(p, 10735, 458285, -1, 90, 4, r);
    specialPlusTerm(p, 15032, 207608, -1, 89, 4, r);
    specialPlusTerm(p, 20754, 112592, -1, 88, 4, r);
    specialPlusTerm(p, 28270, 278236, -1, 87, 4, r);
    specialPlusTerm(p, 38013, 830749, -1, 86, 4, r);
    specialPlusTerm(p, 50484, 542623, -1, 85, 4, r);
    specialPlusTerm(p, 66249, 223101, -1, 84, 4, r);
    specialPlusTerm(p, 85939, 295631, -1, 83, 4, r);
    specialPlusTerm(p, 110245, 8968, -1, 82, 4, r);
    specialPlusTerm(p, 139905, 793448, -1, 81, 4, r);
    specialPlusTerm(p, 175696, 382307, -1, 80, 4, r);
    specialPlusTerm(p, 218408, 471011, -1, 79, 4, r);
    specialPlusTerm(p, 268827, 883672, -1, 78, 4, r);
    specialPlusTerm(p, 327707, 449671, -1, 77, 4, r);
    specialPlusTerm(p, 395736, 57018, -1, 76, 4, r);
    specialPlusTerm(p, 473504, 629765, -1, 75, 4, r);
    specialPlusTerm(p, 561470, 59952, -1, 74, 4, r);
    specialPlusTerm(p, 659918, 392736, -1, 73, 4, r);
    specialPlusTerm(p, 768928, 797683, -1, 72, 4, r);
    specialPlusTerm(p, 888340, 40490, -1, 71, 4, r);
    specialPlusTerm(p, 1017721, 279436, -1, 70, 4, r);
    specialPlusTerm(p, 1156349, 33848, -1, 69, 4, r);
    specialPlusTerm(p, 1303192, 95883, -1, 68, 4, r);
    specialPlusTerm(p, 1456905, 975122, -1, 67, 4, r);
    specialPlusTerm(p, 1615838, 177110, -1, 66, 4, r);
    specialPlusTerm(p, 1778045, 228075, -1, 65, 4, r);
    specialPlusTerm(p, 1941321, 881690, -1, 64, 4, r);
    specialPlusTerm(p, 2103242, 399767, -1, 63, 4, r);
    specialPlusTerm(p, 2261213, 213104, -1, 62, 4, r);
    specialPlusTerm(p, 2412535, 672007, -1, 61, 4, r);
    specialPlusTerm(p, 2554477, 22053, -1, 60, 4, r);
    specialPlusTerm(p, 2684347, 224260, -1, 59, 4, r);
    specialPlusTerm(p, 2799578, 813532, -1, 58, 4, r);
    specialPlusTerm(p, 2897806, 685056, -1, 57, 4, r);
    specialPlusTerm(p, 2976944, 539523, -1, 56, 4, r);
    specialPlusTerm(p, 3035254, 721183, -1, 55, 4, r);
    specialPlusTerm(p, 3071408, 355156, -1, 54, 4, r);
    specialPlusTerm(p, 3084533, 29299, -1, 53, 4, r);
    specialPlusTerm(p, 3074245, 758101, -1, 52, 4, r);
    specialPlusTerm(p, 3040669, 588583, -1, 51, 4, r);
    specialPlusTerm(p, 2984432, 929416, -1, 50, 4, r);
    specialPlusTerm(p, 2906651, 466023, -1, 49, 4, r);
    specialPlusTerm(p, 2808893, 323361, -1, 48, 4, r);
    specialPlusTerm(p, 2693128, 909640, -1, 47, 4, r);
    specialPlusTerm(p, 2561667, 574760, -1, 46, 4, r);
    specialPlusTerm(p, 2417083, 807115, -1, 45, 4, r);
    specialPlusTerm(p, 2262136, 138608, -1, 44, 4, r);
    specialPlusTerm(p, 2099682, 206060, -1, 43, 4, r);
    specialPlusTerm(p, 1932593, 513922, -1, 42, 4, r);
    specialPlusTerm(p, 1763673, 355846, -1, 41, 4, r);
    specialPlusTerm(p, 1595581, 90061, -1, 40, 4, r);
    specialPlusTerm(p, 1430765, 545095, -1, 39, 4, r);
    specialPlusTerm(p, 1271409, 786637, -1, 38, 4, r);
    specialPlusTerm(p, 1119388, 838595, -1, 37, 4, r);
    specialPlusTerm(p, 976241, 261398, -1, 36, 4, r);
    specialPlusTerm(p, 843154, 789753, -1, 35, 4, r);
    specialPlusTerm(p, 720965, 560845, -1, 34, 4, r);
    specialPlusTerm(p, 610169, 859179, -1, 33, 4, r);
    specialPlusTerm(p, 510946, 796821, -1, 32, 4, r);
    specialPlusTerm(p, 423189, 960892, -1, 31, 4, r);
    specialPlusTerm(p, 346545, 808032, -1, 30, 4, r);
    specialPlusTerm(p, 280456, 472902, -1, 29, 4, r);
    specialPlusTerm(p, 224204, 680080, -1, 28, 4, r);
    specialPlusTerm(p, 176958, 593014, -1, 27, 4, r);
    specialPlusTerm(p, 137814, 680334, -1, 26, 4, r);
    specialPlusTerm(p, 105837, 4252, -1, 25, 4, r);
    specialPlusTerm(p, 80091, 710806, -1, 24, 4, r);
    specialPlusTerm(p, 59675, 899582, -1, 23, 4, r);
    specialPlusTerm(p, 43740, 445029, -1, 22, 4, r);
    specialPlusTerm(p, 31506, 709211, -1, 21, 4, r);
    specialPlusTerm(p, 22277, 407599, -1, 20, 4, r);
    specialPlusTerm(p, 15442, 151129, -1, 19, 4, r);
    specialPlusTerm(p, 10478, 380611, -1, 18, 4, r);
    specialPlusTerm(p, 6948, 530664, -1, 17, 4, r);
    specialPlusTerm(p, 4494, 312038, -1, 16, 4, r);
    specialPlusTerm(p, 2828, 990461, -1, 15, 4, r);
    specialPlusTerm(p, 1728, 477668, -1, 14, 4, r);
    specialPlusTerm(p, 1021, 949004, -1, 13, 4, r);
    specialPlusTerm(p, 582, 575810, -1, 12, 4, r);
    specialPlusTerm(p, 318, 823114, -1, 11, 4, r);
    specialPlusTerm(p, 166, 625848, -1, 10, 4, r);
    specialPlusTerm(p, 82, 629352, -1, 9, 4, r);
    specialPlusTerm(p, 38, 568972, -1, 8, 4, r);
    specialPlusTerm(p, 16, 773120, -1, 7, 4, r);
    specialPlusTerm(p, 6, 706050, -1, 6, 4, r);
    specialPlusTerm(p, 2, 420964, -1, 5, 4, r);
    specialPlusTerm(p, 0, 769496, -1, 4, 4, r);
    specialPlusTerm(p, 0, 207389, -1, 3, 4, r);
    specialPlusTerm(p, 0, 44590, -1, 2, 4, r);
    specialPlusTerm(p, 0, 6825, -1, 1, 4, r);
    specialPlusTerm(p, 0, 560, -1, 0, 4, r);
    specialPlusTerm(p, 0, 1, 1, 119, 3, r);
    specialPlusTerm(p, 0, 15, 1, 118, 3, r);
    specialPlusTerm(p, 0, 121, 1, 117, 3, r);
    specialPlusTerm(p, 0, 692, 1, 116, 3, r);
    specialPlusTerm(p, 0, 3136, 1, 115, 3, r);
    specialPlusTerm(p, 0, 11960, 1, 114, 3, r);
    specialPlusTerm(p, 0, 39853, 1, 113, 3, r);
    specialPlusTerm(p, 0, 119021, 1, 112, 3, r);
    specialPlusTerm(p, 0, 324469, 1, 111, 3, r);
    specialPlusTerm(p, 0, 818608, 1, 110, 3, r);
    specialPlusTerm(p, 1, 931746, 1, 109, 3, r);
    specialPlusTerm(p, 4, 299910, 1, 108, 3, r);
    specialPlusTerm(p, 9, 90121, 1, 107, 3, r);
    specialPlusTerm(p, 18, 353593, 1, 106, 3, r);
    specialPlusTerm(p, 35, 558983, 1, 105, 3, r);
    specialPlusTerm(p, 66, 370113, 1, 104, 3, r);
    specialPlusTerm(p, 119, 744513, 1, 103, 3, r);
    specialPlusTerm(p, 209, 439344, 1, 102, 3, r);
    specialPlusTerm(p, 356, 18067, 1, 101, 3, r);
    specialPlusTerm(p, 589, 452669, 1, 100, 3, r);
    specialPlusTerm(p, 952, 410212, 1, 99, 3, r);
    specialPlusTerm(p, 1504, 296795, 1, 98, 3, r);
    specialPlusTerm(p, 2326, 104760, 1, 97, 3, r);
    specialPlusTerm(p, 3526, 68612, 1, 96, 3, r);
    specialPlusTerm(p, 5246, 80850, 1, 95, 3, r);
    specialPlusTerm(p, 7668, 750896, 1, 94, 3, r);
    specialPlusTerm(p, 11024, 909967, 1, 93, 3, r);
    specialPlusTerm(p, 15601, 274876, 1, 92, 3, r);
    specialPlusTerm(p, 21747, 888690, 1, 91, 3, r);
    specialPlusTerm(p, 29884, 861708, 1, 90, 3, r);
    specialPlusTerm(p, 40507, 849459, 1, 89, 3, r);
    specialPlusTerm(p, 54191, 633482, 1, 88, 3, r);
    specialPlusTerm(p, 71591, 124221, 1, 87, 3, r);
    specialPlusTerm(p, 93439, 92109, 1, 86, 3, r);
    specialPlusTerm(p, 120539, 960778, 1, 85, 3, r);
    specialPlusTerm(p, 153759, 71799, 1, 84, 3, r);
    specialPlusTerm(p, 194006, 957680, 1, 83, 3, r);
    specialPlusTerm(p, 242218, 340335, 1, 82, 3, r);
    specialPlusTerm(p, 299325, 803637, 1, 81, 3, r);
    specialPlusTerm(p, 366228, 364763, 1, 80, 3, r);
    specialPlusTerm(p, 443755, 479465, 1, 79, 3, r);
    specialPlusTerm(p, 532627, 346793, 1, 78, 3, r);
    specialPlusTerm(p, 633412, 711280, 1, 77, 3, r);
    specialPlusTerm(p, 746485, 674616, 1, 76, 3, r);
    specialPlusTerm(p, 871983, 302274, 1, 75, 3, r);
    specialPlusTerm(p, 1009766, 21123, 1, 74, 3, r);
    specialPlusTerm(p, 1159382, 930867, 1, 73, 3, r);
    specialPlusTerm(p, 1320044, 177277, 1, 72, 3, r);
    specialPlusTerm(p, 1490602, 445312, 1, 71, 3, r);
    specialPlusTerm(p, 1669545, 418046, 1, 70, 3, r);
    specialPlusTerm(p, 1855000, 712658, 1, 69, 3, r);
    specialPlusTerm(p, 2044754, 355265, 1, 68, 3, r);
    specialPlusTerm(p, 2236283, 307760, 1, 67, 3, r);
    specialPlusTerm(p, 2426801, 935374, 1, 66, 3, r);
    specialPlusTerm(p, 2613321, 633418, 1, 65, 3, r);
    specialPlusTerm(p, 2792722, 150781, 1, 64, 3, r);
    specialPlusTerm(p, 2961832, 494671, 1, 63, 3, r);
    specialPlusTerm(p, 3117518, 715055, 1, 62, 3, r);
    specialPlusTerm(p, 3256775, 385842, 1, 61, 3, r);
    specialPlusTerm(p, 3376817, 256340, 1, 60, 3, r);
    specialPlusTerm(p, 3475167, 367466, 1, 59, 3, r);
    specialPlusTerm(p, 3549737, 930366, 1, 58, 3, r);
    specialPlusTerm(p, 3598900, 457938, 1, 57, 3, r);
    specialPlusTerm(p, 3621542, 18424, 1, 56, 3, r);
    specialPlusTerm(p, 3617105, 29495, 1, 55, 3, r);
    specialPlusTerm(p, 3585608, 705050, 1, 54, 3, r);
    specialPlusTerm(p, 3527651, 69853, 1, 53, 3, r);
    specialPlusTerm(p, 3444391, 326416, 1, 52, 3, r);
    specialPlusTerm(p, 3337513, 246817, 1, 51, 3, r);
    specialPlusTerm(p, 3209171, 120352, 1, 50, 3, r);
    specialPlusTerm(p, 3061920, 568442, 1, 49, 3, r);
    specialPlusTerm(p, 2898637, 197953, 1, 48, 3, r);
    specialPlusTerm(p, 2722426, 567254, 1, 47, 3, r);
    specialPlusTerm(p, 2536529, 259766, 1, 46, 3, r);
    specialPlusTerm(p, 2344224, 982535, 1, 45, 3, r);
    specialPlusTerm(p, 2148739, 529673, 1, 44, 3, r);
    specialPlusTerm(p, 1953158, 181630, 1, 43, 3, r);
    specialPlusTerm(p, 1760348, 671733, 1, 42, 3, r);
    specialPlusTerm(p, 1572896, 271477, 1, 41, 3, r);
    specialPlusTerm(p, 1393052, 863345, 1, 40, 3, r);
    specialPlusTerm(p, 1222701, 126910, 1, 39, 3, r);
    specialPlusTerm(p, 1063334, 204889, 1, 38, 3, r);
    specialPlusTerm(p, 916050, 483828, 1, 37, 3, r);
    specialPlusTerm(p, 781562, 458519, 1, 36, 3, r);
    specialPlusTerm(p, 660218, 83241, 1, 35, 3, r);
    specialPlusTerm(p, 552032, 571637, 1, 34, 3, r);
    specialPlusTerm(p, 456728, 306449, 1, 33, 3, r);
    specialPlusTerm(p, 373780, 366678, 1, 32, 3, r);
    specialPlusTerm(p, 302465, 169735, 1, 31, 3, r);
    specialPlusTerm(p, 241909, 847846, 1, 30, 3, r);
    specialPlusTerm(p, 191140, 212177, 1, 29, 3, r);
    specialPlusTerm(p, 149125, 480301, 1, 28, 3, r);
    specialPlusTerm(p, 114818, 324960, 1, 27, 3, r);
    specialPlusTerm(p, 87189, 215772, 1, 26, 3, r);
    specialPlusTerm(p, 65254, 442815, 1, 25, 3, r);
    specialPlusTerm(p, 48097, 606920, 1, 24, 3, r);
    specialPlusTerm(p, 34884, 715247, 1, 23, 3, r);
    specialPlusTerm(p, 24873, 316659, 1, 22, 3, r);
    specialPlusTerm(p, 17416, 339482, 1, 21, 3, r);
    specialPlusTerm(p, 11961, 450014, 1, 20, 3, r);
    specialPlusTerm(p, 8046, 834503, 1, 19, 3, r);
    specialPlusTerm(p, 5294, 325768, 1, 18, 3, r);
    specialPlusTerm(p, 3400, 757398, 1, 17, 3, r);
    specialPlusTerm(p, 2128, 345318, 1, 16, 3, r);
    specialPlusTerm(p, 1294, 781596, 1, 15, 3, r);
    specialPlusTerm(p, 763, 591975, 1, 14, 3, r);
    specialPlusTerm(p, 435, 169111, 1, 13, 3, r);
    specialPlusTerm(p, 238, 758463, 1, 12, 3, r);
    specialPlusTerm(p, 125, 551459, 1, 11, 3, r);
    specialPlusTerm(p, 62, 936621, 1, 10, 3, r);
    specialPlusTerm(p, 29, 876893, 1, 9, 3, r);
    specialPlusTerm(p, 13, 321355, 1, 8, 3, r);
    specialPlusTerm(p, 5, 520901, 1, 7, 3, r);
    specialPlusTerm(p, 2, 98067, 1, 6, 3, r);
    specialPlusTerm(p, 0, 717871, 1, 5, 3, r);
    specialPlusTerm(p, 0, 215565, 1, 4, 3, r);
    specialPlusTerm(p, 0, 54691, 1, 3, 3, r);
    specialPlusTerm(p, 0, 11025, 1, 2, 3, r);
    specialPlusTerm(p, 0, 1575, 1, 1, 3, r);
    specialPlusTerm(p, 0, 120, 1, 0, 3, r);
    specialPlusTerm(p, 0, 1, -1, 120, 2, r);
    specialPlusTerm(p, 0, 16, -1, 119, 2, r);
    specialPlusTerm(p, 0, 134, -1, 118, 2, r);
    specialPlusTerm(p, 0, 784, -1, 117, 2, r);
    specialPlusTerm(p, 0, 3602, -1, 116, 2, r);
    specialPlusTerm(p, 0, 13843, -1, 115, 2, r);
    specialPlusTerm(p, 0, 46286, -1, 114, 2, r);
    specialPlusTerm(p, 0, 138283, -1, 113, 2, r);
    specialPlusTerm(p, 0, 376259, -1, 112, 2, r);
    specialPlusTerm(p, 0, 945827, -1, 111, 2, r);
    specialPlusTerm(p, 2, 220933, -1, 110, 2, r);
    specialPlusTerm(p, 4, 914174, -1, 109, 2, r);
    specialPlusTerm(p, 10, 318553, -1, 108, 2, r);
    specialPlusTerm(p, 20, 680144, -1, 107, 2, r);
    specialPlusTerm(p, 39, 750869, -1, 106, 2, r);
    specialPlusTerm(p, 73, 579984, -1, 105, 2, r);
    specialPlusTerm(p, 131, 610793, -1, 104, 2, r);
    specialPlusTerm(p, 228, 154162, -1, 103, 2, r);
    specialPlusTerm(p, 384, 310996, -1, 102, 2, r);
    specialPlusTerm(p, 630, 410278, -1, 101, 2, r);
    specialPlusTerm(p, 1009, 15895, -1, 100, 2, r);
    specialPlusTerm(p, 1578, 532849, -1, 99, 2, r);
    specialPlusTerm(p, 2417, 410542, -1, 98, 2, r);
    specialPlusTerm(p, 3628, 897233, -1, 97, 2, r);
    specialPlusTerm(p, 5346, 245930, -1, 96, 2, r);
    specialPlusTerm(p, 7738, 209354, -1, 95, 2, r);
    specialPlusTerm(p, 11014, 592780, -1, 94, 2, r);
    specialPlusTerm(p, 15431, 562271, -1, 93, 2, r);
    specialPlusTerm(p, 21296, 336917, -1, 92, 2, r);
    specialPlusTerm(p, 28970, 832923, -1, 91, 2, r);
    specialPlusTerm(p, 38873, 781118, -1, 90, 2, r);
    specialPlusTerm(p, 51480, 814244, -1, 89, 2, r);
    specialPlusTerm(p, 67322, 22502, -1, 88, 2, r);
    specialPlusTerm(p, 86976, 510707, -1, 87, 2, r);
    specialPlusTerm(p, 111063, 562039, -1, 86, 2, r);
    specialPlusTerm(p, 140230, 123797, -1, 85, 2, r);
    specialPlusTerm(p, 175134, 479331, -1, 84, 2, r);
    specialPlusTerm(p, 216426, 154185, -1, 83, 2, r);
    specialPlusTerm(p, 264722, 317163, -1, 82, 2, r);
    specialPlusTerm(p, 320581, 169273, -1, 81, 2, r);
    specialPlusTerm(p, 384473, 53329, -1, 80, 2, r);
    specialPlusTerm(p, 456750, 250207, -1, 79, 2, r);
    specialPlusTerm(p, 537616, 638714, -1, 78, 2, r);
    specialPlusTerm(p, 627098, 568602, -1, 77, 2, r);
    specialPlusTerm(p, 725018, 414915, -1, 76, 2, r);
    specialPlusTerm(p, 830972, 332853, -1, 75, 2, r);
    specialPlusTerm(p, 944313, 704873, -1, 74, 2, r);
    specialPlusTerm(p, 1064143, 659019, -1, 73, 2, r);
    specialPlusTerm(p, 1189309, 837532, -1, 72, 2, r);
    specialPlusTerm(p, 1318414, 311129, -1, 71, 2, r);
    specialPlusTerm(p, 1449831, 176128, -1, 70, 2, r);
    specialPlusTerm(p, 1581733, 953526, -1, 69, 2, r);
    specialPlusTerm(p, 1712132, 450851, -1, 68, 2, r);
    specialPlusTerm(p, 1838918, 272730, -1, 67, 2, r);
    specialPlusTerm(p, 1959917, 700931, -1, 66, 2, r);
    specialPlusTerm(p, 2072950, 236513, -1, 65, 2, r);
    specialPlusTerm(p, 2175890, 732337, -1, 64, 2, r);
    specialPlusTerm(p, 2266732, 767744, -1, 63, 2, r);
    specialPlusTerm(p, 2343650, 748669, -1, 62, 2, r);
    specialPlusTerm(p, 2405058, 170088, -1, 61, 2, r);
    specialPlusTerm(p, 2449659, 560856, -1, 60, 2, r);
    specialPlusTerm(p, 2476493, 843437, -1, 59, 2, r);
    specialPlusTerm(p, 2484967, 174656, -1, 58, 2, r);
    specialPlusTerm(p, 2474873, 772785, -1, 57, 2, r);
    specialPlusTerm(p, 2446403, 758673, -1, 56, 2, r);
    specialPlusTerm(p, 2400137, 616515, -1, 55, 2, r);
    specialPlusTerm(p, 2337027, 481818, -1, 54, 2, r);
    specialPlusTerm(p, 2258366, 57059, -1, 53, 2, r);
    specialPlusTerm(p, 2165744, 506758, -1, 52, 2, r);
    specialPlusTerm(p, 2061001, 163040, -1, 51, 2, r);
    specialPlusTerm(p, 1946163, 254505, -1, 50, 2, r);
    specialPlusTerm(p, 1823384, 135692, -1, 49, 2, r);
    specialPlusTerm(p, 1694878, 629195, -1, 48, 2, r);
    specialPlusTerm(p, 1562859, 93097, -1, 47, 2, r);
    specialPlusTerm(p, 1429474, 696453, -1, 46, 2, r);
    specialPlusTerm(p, 1296756, 136371, -1, 45, 2, r);
    specialPlusTerm(p, 1166567, 679850, -1, 44, 2, r);
    specialPlusTerm(p, 1040567, 985354, -1, 43, 2, r);
    specialPlusTerm(p, 920180, 680239, -1, 42, 2, r);
    specialPlusTerm(p, 806575, 169521, -1, 41, 2, r);
    specialPlusTerm(p, 700657, 657865, -1, 40, 2, r);
    specialPlusTerm(p, 603071, 906907, -1, 39, 2, r);
    specialPlusTerm(p, 514208, 847321, -1, 38, 2, r);
    specialPlusTerm(p, 434223, 837763, -1, 37, 2, r);
    specialPlusTerm(p, 363060, 123557, -1, 36, 2, r);
    specialPlusTerm(p, 300476, 903127, -1, 35, 2, r);
    specialPlusTerm(p, 246080, 359923, -1, 34, 2, r);
    specialPlusTerm(p, 199356, 56374, -1, 33, 2, r);
    specialPlusTerm(p, 159701, 203700, -1, 32, 2, r);
    specialPlusTerm(p, 126455, 502830, -1, 31, 2, r);
    specialPlusTerm(p, 98929, 480219, -1, 30, 2, r);
    specialPlusTerm(p, 76429, 499829, -1, 29, 2, r);
    specialPlusTerm(p, 58278, 900847, -1, 28, 2, r);
    specialPlusTerm(p, 43834, 973084, -1, 27, 2, r);
    specialPlusTerm(p, 32501, 723962, -1, 26, 2, r);
    specialPlusTerm(p, 23738, 601098, -1, 25, 2, r);
    specialPlusTerm(p, 17065, 504721, -1, 24, 2, r);
    specialPlusTerm(p, 12064, 550062, -1, 23, 2, r);
    specialPlusTerm(p, 8379, 120417, -1, 22, 2, r);
    specialPlusTerm(p, 5710, 788839, -1, 21, 2, r);
    specialPlusTerm(p, 3814, 684934, -1, 20, 2, r);
    specialPlusTerm(p, 2493, 849460, -1, 19, 2, r);
    specialPlusTerm(p, 1593, 60962, -1, 18, 2, r);
    specialPlusTerm(p, 992, 543577, -1, 17, 2, r);
    specialPlusTerm(p, 601, 881264, -1, 16, 2, r);
    specialPlusTerm(p, 354, 378149, -1, 15, 2, r);
    specialPlusTerm(p, 202, 23344, -1, 14, 2, r);
    specialPlusTerm(p, 111, 145993, -1, 13, 2, r);
    specialPlusTerm(p, 58, 785390, -1, 12, 2, r);
    specialPlusTerm(p, 29, 753317, -1, 11, 2, r);
    specialPlusTerm(p, 14, 331491, -1, 10, 2, r);
    specialPlusTerm(p, 6, 525379, -1, 9, 2, r);
    specialPlusTerm(p, 2, 785083, -1, 8, 2, r);
    specialPlusTerm(p, 1, 102494, -1, 7, 2, r);
    specialPlusTerm(p, 0, 399235, -1, 6, 2, r);
    specialPlusTerm(p, 0, 129826, -1, 5, 2, r);
    specialPlusTerm(p, 0, 36944, -1, 4, 2, r);
    specialPlusTerm(p, 0, 8854, -1, 3, 2, r);
    specialPlusTerm(p, 0, 1680, -1, 2, 2, r);
    specialPlusTerm(p, 0, 225, -1, 1, 2, r);
    specialPlusTerm(p, 0, 16, -1, 0, 2, r);
    specialPlusTerm(p, 0, 1, 1, 120, 1, r);
    specialPlusTerm(p, 0, 15, 1, 119, 1, r);
    specialPlusTerm(p, 0, 119, 1, 118, 1, r);
    specialPlusTerm(p, 0, 664, 1, 117, 1, r);
    specialPlusTerm(p, 0, 2924, 1, 116, 1, r);
    specialPlusTerm(p, 0, 10813, 1, 115, 1, r);
    specialPlusTerm(p, 0, 34900, 1, 114, 1, r);
    specialPlusTerm(p, 0, 100913, 1, 113, 1, r);
    specialPlusTerm(p, 0, 266338, 1, 112, 1, r);
    specialPlusTerm(p, 0, 650658, 1, 111, 1, r);
    specialPlusTerm(p, 1, 487262, 1, 110, 1, r);
    specialPlusTerm(p, 3, 208036, 1, 109, 1, r);
    specialPlusTerm(p, 6, 574987, 1, 108, 1, r);
    specialPlusTerm(p, 12, 876702, 1, 107, 1, r);
    specialPlusTerm(p, 24, 210652, 1, 106, 1, r);
    specialPlusTerm(p, 43, 874857, 1, 105, 1, r);
    specialPlusTerm(p, 76, 893687, 1, 104, 1, r);
    specialPlusTerm(p, 130, 701986, 1, 103, 1, r);
    specialPlusTerm(p, 216, 8661, 1, 102, 1, r);
    specialPlusTerm(p, 347, 854815, 1, 101, 1, r);
    specialPlusTerm(p, 546, 871981, 1, 100, 1, r);
    specialPlusTerm(p, 840, 732790, 1, 99, 1, r);
    specialPlusTerm(p, 1265, 769513, 1, 98, 1, r);
    specialPlusTerm(p, 1868, 715733, 1, 97, 1, r);
    specialPlusTerm(p, 2708, 503701, 1, 96, 1, r);
    specialPlusTerm(p, 3858, 25899, 1, 95, 1, r);
    specialPlusTerm(p, 5405, 745562, 1, 94, 1, r);
    specialPlusTerm(p, 7457, 19331, 1, 93, 1, r);
    specialPlusTerm(p, 10134, 977992, 1, 92, 1, r);
    specialPlusTerm(p, 13580, 800674, 1, 91, 1, r);
    specialPlusTerm(p, 17953, 216130, 1, 90, 1, r);
    specialPlusTerm(p, 23427, 73737, 1, 89, 1, r);
    specialPlusTerm(p, 30190, 848078, 1, 88, 1, r);
    specialPlusTerm(p, 38442, 975195, 1, 87, 1, r);
    specialPlusTerm(p, 48386, 965771, 1, 86, 1, r);
    specialPlusTerm(p, 60225, 299589, 1, 85, 1, r);
    specialPlusTerm(p, 74152, 174574, 1, 84, 1, r);
    specialPlusTerm(p, 90345, 259476, 1, 83, 1, r);
    specialPlusTerm(p, 108956, 677797, 1, 82, 1, r);
    specialPlusTerm(p, 130103, 527156, 1, 81, 1, r);
    specialPlusTerm(p, 153858, 307693, 1, 80, 1, r);
    specialPlusTerm(p, 180239, 689955, 1, 79, 1, r);
    specialPlusTerm(p, 209204, 91832, 1, 78, 1, r);
    specialPlusTerm(p, 240638, 550997, 1, 77, 1, r);
    specialPlusTerm(p, 274355, 370450, 1, 76, 1, r);
    specialPlusTerm(p, 310088, 978041, 1, 75, 1, r);
    specialPlusTerm(p, 347495, 375766, 1, 74, 1, r);
    specialPlusTerm(p, 386154, 462530, 1, 73, 1, r);
    specialPlusTerm(p, 425575, 398166, 1, 72, 1, r);
    specialPlusTerm(p, 465205, 41801, 1, 71, 1, r);
    specialPlusTerm(p, 504439, 350762, 1, 70, 1, r);
    specialPlusTerm(p, 542637, 474944, 1, 69, 1, r);
    specialPlusTerm(p, 579138, 134521, 1, 68, 1, r);
    specialPlusTerm(p, 613277, 734909, 1, 67, 1, r);
    specialPlusTerm(p, 644409, 560477, 1, 66, 1, r);
    specialPlusTerm(p, 671923, 305201, 1, 65, 1, r);
    specialPlusTerm(p, 695264, 150336, 1, 64, 1, r);
    specialPlusTerm(p, 713950, 590364, 1, 63, 1, r);
    specialPlusTerm(p, 727590, 240796, 1, 62, 1, r);
    specialPlusTerm(p, 735892, 934219, 1, 61, 1, r);
    specialPlusTerm(p, 738680, 521142, 1, 60, 1, r);
    specialPlusTerm(p, 735892, 934219, 1, 59, 1, r);
    specialPlusTerm(p, 727590, 240796, 1, 58, 1, r);
    specialPlusTerm(p, 713950, 590364, 1, 57, 1, r);
    specialPlusTerm(p, 695264, 150336, 1, 56, 1, r);
    specialPlusTerm(p, 671923, 305201, 1, 55, 1, r);
    specialPlusTerm(p, 644409, 560477, 1, 54, 1, r);
    specialPlusTerm(p, 613277, 734909, 1, 53, 1, r);
    specialPlusTerm(p, 579138, 134521, 1, 52, 1, r);
    specialPlusTerm(p, 542637, 474944, 1, 51, 1, r);
    specialPlusTerm(p, 504439, 350762, 1, 50, 1, r);
    specialPlusTerm(p, 465205, 41801, 1, 49, 1, r);
    specialPlusTerm(p, 425575, 398166, 1, 48, 1, r);
    specialPlusTerm(p, 386154, 462530, 1, 47, 1, r);
    specialPlusTerm(p, 347495, 375766, 1, 46, 1, r);
    specialPlusTerm(p, 310088, 978041, 1, 45, 1, r);
    specialPlusTerm(p, 274355, 370450, 1, 44, 1, r);
    specialPlusTerm(p, 240638, 550997, 1, 43, 1, r);
    specialPlusTerm(p, 209204, 91832, 1, 42, 1, r);
    specialPlusTerm(p, 180239, 689955, 1, 41, 1, r);
    specialPlusTerm(p, 153858, 307693, 1, 40, 1, r);
    specialPlusTerm(p, 130103, 527156, 1, 39, 1, r);
    specialPlusTerm(p, 108956, 677797, 1, 38, 1, r);
    specialPlusTerm(p, 90345, 259476, 1, 37, 1, r);
    specialPlusTerm(p, 74152, 174574, 1, 36, 1, r);
    specialPlusTerm(p, 60225, 299589, 1, 35, 1, r);
    specialPlusTerm(p, 48386, 965771, 1, 34, 1, r);
    specialPlusTerm(p, 38442, 975195, 1, 33, 1, r);
    specialPlusTerm(p, 30190, 848078, 1, 32, 1, r);
    specialPlusTerm(p, 23427, 73737, 1, 31, 1, r);
    specialPlusTerm(p, 17953, 216130, 1, 30, 1, r);
    specialPlusTerm(p, 13580, 800674, 1, 29, 1, r);
    specialPlusTerm(p, 10134, 977992, 1, 28, 1, r);
    specialPlusTerm(p, 7457, 19331, 1, 27, 1, r);
    specialPlusTerm(p, 5405, 745562, 1, 26, 1, r);
    specialPlusTerm(p, 3858, 25899, 1, 25, 1, r);
    specialPlusTerm(p, 2708, 503701, 1, 24, 1, r);
    specialPlusTerm(p, 1868, 715733, 1, 23, 1, r);
    specialPlusTerm(p, 1265, 769513, 1, 22, 1, r);
    specialPlusTerm(p, 840, 732790, 1, 21, 1, r);
    specialPlusTerm(p, 546, 871981, 1, 20, 1, r);
    specialPlusTerm(p, 347, 854815, 1, 19, 1, r);
    specialPlusTerm(p, 216, 8661, 1, 18, 1, r);
    specialPlusTerm(p, 130, 701986, 1, 17, 1, r);
    specialPlusTerm(p, 76, 893687, 1, 16, 1, r);
    specialPlusTerm(p, 43, 874857, 1, 15, 1, r);
    specialPlusTerm(p, 24, 210652, 1, 14, 1, r);
    specialPlusTerm(p, 12, 876702, 1, 13, 1, r);
    specialPlusTerm(p, 6, 574987, 1, 12, 1, r);
    specialPlusTerm(p, 3, 208036, 1, 11, 1, r);
    specialPlusTerm(p, 1, 487262, 1, 10, 1, r);
    specialPlusTerm(p, 0, 650658, 1, 9, 1, r);
    specialPlusTerm(p, 0, 266338, 1, 8, 1, r);
    specialPlusTerm(p, 0, 100913, 1, 7, 1, r);
    specialPlusTerm(p, 0, 34900, 1, 6, 1, r);
    specialPlusTerm(p, 0, 10813, 1, 5, 1, r);
    specialPlusTerm(p, 0, 2924, 1, 4, 1, r);
    specialPlusTerm(p, 0, 664, 1, 3, 1, r);
    specialPlusTerm(p, 0, 119, 1, 2, 1, r);
    specialPlusTerm(p, 0, 15, 1, 1, 1, r);
    specialPlusTerm(p, 0, 1, 1, 0, 1, r);
  }
  void checkInverse(number n, const coeffs cf)
  {
    clog << "n = "; p_Write((poly)n, cf->extRing);
    number n1 = n_Invers(n, cf);
    clog << "==> n^(-1) = "; p_Write((poly)n1, cf->extRing);
    number n2 = n_Mult(n, n1, cf);
    clog << "check: n * n^(-1) = "; p_Write((poly)n2, cf->extRing);
    TS_ASSERT( n_IsOne(n2, cf) );
    n_Delete(&n1, cf); n_Delete(&n2, cf);
  }
  /* to be used over a rational function field, only!
     assumes that cf represents a rational function field;
     uses p inside the resulting number (and not a copy of it!) */
  number toFractionNumber(poly p, const coeffs cf)
  {
    number n = n_Init(1, cf);
    fraction f = (fraction)n;
    p_Delete(&(f->numerator), cf->extRing);
    f->numerator = p;
    return n;
  }
  void TestArithCf(const coeffs r)
  {
    clog << ("TEST: Simple Arithmetics: ");
    clog << endl;

    number two = n_Init(2, r);

    number t = n_Init(1, r);
    n_InpAdd(t, t, r);
    TS_ASSERT( n_Equal(two, t, r) );
    n_Delete(&t, r);

    if( getCoeffType(r) == n_Q )
    {
      number t = n_Init(1, r);
      n_InpAdd(t, t, r);
      TS_ASSERT( n_Equal(two, t, r) );
      n_Delete(&t, r);
    }

    const int N = 66666;

    number a = n_Init(N, r);

    clog<< "a: "; PrintSized(a, r);

    clog<< "two: "; PrintSized(two, r);

    number aa0 = n_Init(N*2, r);

    number aa = n_Add(a, a, r);

    clog<< "aa = a + a: "; PrintSized(aa, r);

    number aa2 = n_Mult(a, two, r);

    clog<< "aa2 = a * 2: "; PrintSized(aa2, r);

    number aa1 = n_Mult(two, a, r);

    clog<< "aa1 = 2 * a: "; PrintSized(aa1, r);

    n_Delete(&a, r);
    n_Delete(&two, r);

    a = n_Sub( aa, aa1, r );

    clog<< "a = aa - aa1: "; PrintSized(a, r);

    TS_ASSERT( n_IsZero(a, r) );

    n_Delete(&a, r);

    a = n_Sub( aa, aa2, r );

    clog<< "a = aa - aa2: "; PrintSized(a, r);

    TS_ASSERT( n_IsZero(a, r) );

    n_Delete(&a, r);

    a = n_Sub( aa1, aa2, r );

    clog<< "a = aa1 - aa2: "; PrintSized(a, r);

    TS_ASSERT( n_IsZero(a, r) );

    n_Delete(&a, r);

    TS_ASSERT( n_Equal(aa, aa1, r) );
    TS_ASSERT( n_Equal(aa, aa2, r) );
    TS_ASSERT( n_Equal(aa1, aa2, r) );

    TS_ASSERT( n_Equal(aa0, aa, r) );
    TS_ASSERT( n_Equal(aa0, aa1, r) );
    TS_ASSERT( n_Equal(aa0, aa2, r) );

    n_Delete(&aa, r);
    n_Delete(&aa1, r);
    n_Delete(&aa2, r);

    n_Delete(&aa0, r);

    clog << ( " >>> TEST DONE!" );
    clog << endl;
  }
  void TestSumCf(const coeffs r, const unsigned long N)
  {
    clog << ( _2S("TEST: sum[0..") + _2S(N) + "]: ");
    clog << endl;

    assume( N > 0 ); // just for now...

    const unsigned long ssss = (N * (N+1)) / 2;

    number sum1 = n_Init(ssss, r);
    clog<< "N*(N+1)/2 (int: " << ssss << "): "; PrintSized(sum1, r);

    number s, ss, i, res;

    s = n_Init(N  , r);
    i = n_Init(N+1, r);
    n_InpMult(s, i, r);
    n_Delete(&i, r);

    clog<< "N*(N+1): ("<< N*(N+1) << ")"; PrintSized(s, r);

    i = n_Init(2, r);
    clog<< "2: "; PrintSized(i, r);

    if( !n_IsZero( i, r) )
    {
  #ifdef HAVE_RINGS
      TS_ASSERT( n_DivBy(s, i, r) );
  #endif

      res = n_Div(s, i, r);

      clog<< "N*(N+1)/2: "; PrintSized(res, r);


      number d = n_Sub(res, sum1, r);
      TS_ASSERT( n_IsZeroDivisor(d, r) );
      n_Delete(&d, r);

      if( n_GetChar(r) == 0 )
      {
        TS_ASSERT( n_Equal(sum1, res, r) );
        TS_ASSERT( n_Equal(res, sum1, r) );
      }
    } else
      TS_ASSERT_EQUALS( n_GetChar(r), 2);


    n_Delete(&s, r);  n_Delete(&i, r);

    n_Delete(&sum1, r); n_Delete(&res, r);


    s = n_Init(0  , r);
    ss = n_Init(0 , r);
    for( int k = N; k >= 0; k-- )
    {
      i = n_Init(k, r);
      n_InpAdd(s, i, r); // s += i

      i = n_InpNeg(i, r);
      n_InpAdd(ss, i, r); // ss -= i

      n_Delete(&i, r);
    }
    clog<< "ss: "; PrintSized(ss, r);

    ss = n_InpNeg(ss, r); // ss = -ss

    clog<< "real sum    : "; PrintSized(s, r);
    clog<< "real sum(--): "; PrintSized(ss, r);

    TS_ASSERT( n_Equal(s, ss, r) );
    TS_ASSERT( n_Equal(ss, s, r) );

    n_Delete(&s, r);
    n_Delete(&ss, r);

    clog << ( " >>> TEST DONE!" );
    clog << endl;

  }
public:
  void test_Z13_t()
  {
    clog << "Creating  Z/13[t]: " << endl;

    char* n[] = {(char*)"t"};
    ring r = rDefault( 13, 1, n);
    TS_ASSERT_DIFFERS( r, NULLp );

    PrintRing(r);

    TS_ASSERT( rField_is_Domain(r) );
    TS_ASSERT( !rField_is_Q(r) );

    TS_ASSERT( rField_is_Zp(r) );
    TS_ASSERT( !rField_is_Zp(r, 11) );
    TS_ASSERT( rField_is_Zp(r, 13) );

    TS_ASSERT_EQUALS( rVar(r), 1);

    Test(r);

    rDelete(r);
  }

  void test_QQ_t()
  {
    clog << "Creating  Q[s]: " << endl;

    char* n[] = {(char*)"s"};
    ring r = rDefault( 0, 1, n);
    TS_ASSERT_DIFFERS( r, NULLp );

    PrintRing(r);

    TS_ASSERT( rField_is_Domain(r) );
    TS_ASSERT( rField_is_Q(r) );

    TS_ASSERT( !rField_is_Zp(r) );
    TS_ASSERT( !rField_is_Zp(r, 11) );

    TS_ASSERT_EQUALS( rVar(r), 1);

    Test(r);

    rDelete(r);
  }

  void test_Z11_x_y_z()
  {
     clog << "Creating  Z/11[x, y, z]: " << endl;

     char* n[] = {(char*)"x", (char*)"y", (char*)"z"};
     ring r = rDefault( 11, 3, n);
     TS_ASSERT_DIFFERS( r, NULLp );

     PrintRing(r);

     TS_ASSERT( rField_is_Domain(r) );
     TS_ASSERT( !rField_is_Q(r) );

     TS_ASSERT( rField_is_Zp(r) );
     TS_ASSERT( rField_is_Zp(r, 11) );
     TS_ASSERT( !rField_is_Zp(r, 13) );

     TS_ASSERT_EQUALS( rVar(r), 3);

     Test(r);

     rDelete(r);
  }
   void test_QQ_x_y_z()
   {
     clog << "Creating  QQ[x, y, z, u]: " << endl;

     char* n[] = {(char*)"x", (char*)"y", (char*)"z", (char*)"u"};
     ring r = rDefault( 0, 4, n);
     TS_ASSERT_DIFFERS( r, NULLp );

     PrintRing(r);

     TS_ASSERT( rField_is_Domain(r) );
     TS_ASSERT( rField_is_Q(r) );

     TS_ASSERT( !rField_is_Zp(r) );
     TS_ASSERT( !rField_is_Zp(r, 11) );

     TS_ASSERT_EQUALS( rVar(r), 4);

     Test(r);

     rDelete(r);
   }


   void test_Z13_t_GF()
   {
     clog << "Creating  GF[t]: " << endl;

     char* n[] = {(char*)"t"};

     GFInfo param;

     param.GFChar= 5;
     param.GFDegree= 2;
     param.GFPar_name= (const char*)"Q";

     const coeffs cf = nInitChar( n_GF, &param );

     if( cf == NULL )
       TS_FAIL("Could not get needed coeff. domain");

     TS_ASSERT_DIFFERS( cf, NULLp );

     ring r = rDefault( cf, 1, n);  // now cf belongs to r!
     TS_ASSERT_DIFFERS( r, NULLp );

     PrintRing(r);

     TS_ASSERT( rField_is_Domain(r) );
     TS_ASSERT( !rField_is_Q(r) );

     TS_ASSERT( !rField_is_Zp(r) );
     TS_ASSERT( !rField_is_Zp(r, 11) );
     TS_ASSERT( !rField_is_Zp(r, 13) );
     TS_ASSERT( rField_is_GF(r) );

     TS_ASSERT( rField_is_GF(r, 5) );
     TS_ASSERT( !rField_is_GF(r, 25) );

     TS_ASSERT_EQUALS( rVar(r), 1);

     Test(r);

     rDelete(r); // kills 'cf' as well!
   }

  void test_Q_Ext_a()
  {
    clog << "Start by creating Q[a]..." << endl;

    char* n[] = {(char*)"a"};
    ring r = rDefault( 0, 1, n);   // Q[a]
    TS_ASSERT_DIFFERS( r, NULLp );

    PrintRing(r);

    TS_ASSERT( rField_is_Domain(r) );
    TS_ASSERT( rField_is_Q(r) );

    TS_ASSERT( !rField_is_Zp(r) );
    TS_ASSERT( !rField_is_Zp(r, 11) );

    TS_ASSERT_EQUALS( rVar(r), 1);

    poly minPoly = p_ISet(1, r);                    // minPoly = 1
    p_SetExp(minPoly, 1, 2, r); p_Setm(minPoly, r); // minPoly = a^2
    minPoly = p_Add_q(minPoly, p_ISet(1, r), r);    // minPoly = a^2 + 1
    ideal minIdeal = idInit(1);                     // minIdeal = < 0 >
    minIdeal->m[0] = minPoly;                       // minIdeal = < a^2 + 1 >

    n_coeffType type = nRegister(n_algExt, naInitChar);
    TS_ASSERT(type == n_algExt);

    r->qideal = minIdeal;
    AlgExtInfo extParam;
    extParam.r = r;

    clog << "Next create the extension field Q[a]/<a2+1>..." << endl;

    const coeffs cf = nInitChar(type, &extParam);   // Q[a]/<a2+1>

    if( cf == NULL )
      TS_FAIL("Could not get needed coeff. domain");

    TS_ASSERT_DIFFERS( cf->cfCoeffWrite, NULLp );

    if( cf->cfCoeffWrite != NULL )
    {
      clog << "Coeff-domain: "  << endl;
      n_CoeffWrite(cf); PrintLn();
    }

    TS_ASSERT( nCoeff_is_algExt(cf) );
    TS_ASSERT( !nCoeff_is_transExt(cf) );

    // some tests for the coefficient field represented by cf:
    TestArithCf(cf);
    TestSumCf(cf, 10);
    TestSumCf(cf, 100);
    TestSumCf(cf, 101);
    TestSumCf(cf, 1001);
    TestSumCf(cf, 2000);

    clog << "Finally create the polynomial ring (Q[a]/<a2+1>)[x, y]..."
         << endl;

    char* m[] = {(char*)"x", (char*)"y"};
    ring s = rDefault(cf, 2, m);   // (Q[a]/<a2+1>)[x, y]
    TS_ASSERT_DIFFERS(s, NULLp);

    PrintRing(s);

    TS_ASSERT( rField_is_Domain(s) );
    TS_ASSERT( !rField_is_Q(s) );
    TS_ASSERT( !rField_is_Zp(s) );
    TS_ASSERT( !rField_is_Zp(s, 11) );
    TS_ASSERT( !rField_is_Zp(s, 13) );
    TS_ASSERT( !rField_is_GF(s) );
    TS_ASSERT( (s->cf->extRing!=NULL) );
    TS_ASSERT( !rField_is_GF(s, 25) );
    TS_ASSERT_EQUALS(rVar(s), 2);

    Test(s);

    clog << endl
         << "Now let's compute some inverses in Q[a]/<a^2+1>..."
         << endl;

    poly u;
    u = NULL; plusTerm(u, 1, 1, 1, cf->extRing);
    plusTerm(u, 1, 1, 0, cf->extRing);  // a + 1
    checkInverse((number)u, cf); p_Delete(&u, cf->extRing);
    u = NULL; plusTerm(u, 1, 1, 1, cf->extRing);
    plusTerm(u, -1, 1, 0, cf->extRing); // a - 1
    checkInverse((number)u, cf); p_Delete(&u, cf->extRing);
    u = NULL; plusTerm(u, 1, 1, 1, cf->extRing);
    plusTerm(u, 5, 1, 0, cf->extRing);  // a + 5
    checkInverse((number)u, cf); p_Delete(&u, cf->extRing);
    u = NULL; plusTerm(u, 1, 1, 1, cf->extRing);
    plusTerm(u, -5, 1, 0, cf->extRing); // a - 5
    checkInverse((number)u, cf); p_Delete(&u, cf->extRing);
    u = NULL; plusTerm(u, 17, 1, 1, cf->extRing);
    plusTerm(u, 5, 1, 0, cf->extRing); // 17a + 5
    checkInverse((number)u, cf); p_Delete(&u, cf->extRing);

    rDelete(s); // kills 'cf' and 'r' as well
  }
  void test_Q_Ext_b()
  {
    clog << "Start by creating Q[b]..." << endl;

    char* n[] = {(char*)"b"};
    ring r = rDefault( 0, 1, n);   // Q[b]
    TS_ASSERT_DIFFERS( r, NULLp );

    PrintRing(r);

    TS_ASSERT( rField_is_Domain(r) );
    TS_ASSERT( rField_is_Q(r) );

    TS_ASSERT( !rField_is_Zp(r) );
    TS_ASSERT( !rField_is_Zp(r, 11) );

    TS_ASSERT_EQUALS( rVar(r), 1);

    poly minPoly = p_ISet(1, r);                    // minPoly = 1
    p_SetExp(minPoly, 1, 7, r); p_Setm(minPoly, r); // minPoly = b^7
    minPoly = p_Add_q(minPoly, p_ISet(17, r), r);   // minPoly = b^7 + 17
    ideal minIdeal = idInit(1);                     // minIdeal = < 0 >
    minIdeal->m[0] = minPoly;                       // minIdeal = < b^7 + 17 >

    n_coeffType type = nRegister(n_algExt, naInitChar);
    TS_ASSERT(type == n_algExt);

    r->qideal = minIdeal;
    AlgExtInfo extParam;
    extParam.r = r;

    clog << "Next create the extension field Q[b]/<b^7+17>..." << endl;

    const coeffs cf = nInitChar(type, &extParam);   // Q[b]/<b^7+17>

    if( cf == NULL )
      TS_FAIL("Could not get needed coeff. domain");

    TS_ASSERT_DIFFERS( cf->cfCoeffWrite, NULLp );

    if( cf->cfCoeffWrite != NULL )
    {
      clog << "Coeff-domain: "  << endl;
      n_CoeffWrite(cf); PrintLn();
    }

    TS_ASSERT( nCoeff_is_algExt(cf) );
    TS_ASSERT( !nCoeff_is_transExt(cf) );

    // some tests for the coefficient field represented by cf:
    TestArithCf(cf);
    TestSumCf(cf, 10);
    TestSumCf(cf, 100);
    TestSumCf(cf, 101);
    TestSumCf(cf, 1001);
    TestSumCf(cf, 9000);

    clog << "Finally create the polynomial ring (Q[b]/<b^7+17>)[u, v, w]..."
         << endl;

    char* m[] = {(char*)"u", (char*)"v", (char*)"w"};
    ring s = rDefault(cf, 3, m);   // (Q[b]/<b^7+17>)[u, v, w]
    TS_ASSERT_DIFFERS(s, NULLp);

    PrintRing(s);

    TS_ASSERT( rField_is_Domain(s) );
    TS_ASSERT( !rField_is_Q(s) );
    TS_ASSERT( !rField_is_Zp(s) );
    TS_ASSERT( !rField_is_Zp(s, 11) );
    TS_ASSERT( !rField_is_Zp(s, 13) );
    TS_ASSERT( !rField_is_GF(s) );
    TS_ASSERT( (s->cf->extRing!=NULL) );
    TS_ASSERT( !rField_is_GF(s, 25) );
    TS_ASSERT_EQUALS(rVar(s), 3);

    Test(s);

    clog << endl
         << "Now let's compute some inverses in Q[b]/<b^7+17>..."
         << endl;

    poly u;
    u = NULL; plusTerm(u, 1, 1, 2, cf->extRing);
    plusTerm(u, 33, 1, 0, cf->extRing);     // b^2 + 33
    checkInverse((number)u, cf); p_Delete(&u, cf->extRing);
    u = NULL; plusTerm(u, 1, 1, 5, cf->extRing);
    plusTerm(u, -137, 1, 0, cf->extRing);   // b^5 - 137
    checkInverse((number)u, cf); p_Delete(&u, cf->extRing);

    clog << endl
         << "Now let's check a gcd computation in Q[b]..."
         << endl;

    poly v;
    v = NULL; plusTerm(v, 1, 1, 2, cf->extRing);
    plusTerm(v, 7, 1, 1, cf->extRing);
    plusTerm(v, 1, 1, 0, cf->extRing);            // b^2 + 7b + 1
    number w = n_Mult((number)v, (number)v, cf);  // (b^2 + 7b + 1)^2
    number y = n_Mult((number)v, (number)w, cf);  // (b^2 + 7b + 1)^3
    p_Delete(&v, cf->extRing);
    v = NULL; plusTerm(v, 2, 1, 2, cf->extRing);
    plusTerm(v, -61, 1, 1, cf->extRing);          // 2b^2 - 61b
    number z = n_Mult((number)w,
                      (number)v, cf);   // (b^2 + 7b + 1)^2 * (2b^2 - 61b)
    p_Delete(&v, cf->extRing);

    clog << "z = "; p_Write((poly)z, cf->extRing);
    clog << "y = "; p_Write((poly)y, cf->extRing);
    number theGcd = n_Gcd(z, y, cf);   // should yield w = (b^2 + 7b + 1)^2
    clog << "gcd(z, y) = "; p_Write((poly)theGcd, cf->extRing);

    v = (poly)n_Sub(theGcd, w, cf);
//    TS_ASSERT( v == NULL );
    p_Delete(&v, cf->extRing);

    clog << endl
         << "Now let's check an ext_gcd computation in Q[b]..."
         << endl;

    poly zFactor; poly yFactor;
    poly ppp = p_ExtGcd((poly)z, zFactor, (poly)y, yFactor, cf->extRing);
    v = (poly)n_Sub(theGcd, (number)ppp, cf);
//    TS_ASSERT( v == NULL );
    p_Delete(&v, cf->extRing);
    clog << "z = "; p_Write((poly)z, cf->extRing);
    clog << "zFactor = "; p_Write(zFactor, cf->extRing);
    clog << "y = "; p_Write((poly)y, cf->extRing);
    clog << "yFactor = "; p_Write((poly)yFactor, cf->extRing);
    number v1 = n_Mult(z, (number)zFactor, cf);
    number v2 = n_Mult(y, (number)yFactor, cf);
    number v3 = n_Add(v1, v2, cf);
    clog << "z * zFactor + y * yFactor = "; p_Write((poly)v3, cf->extRing);
    clog << "gcd(z, y) = "; p_Write(ppp, cf->extRing);
    number v4 = n_Sub(v3, w, cf);
    TS_ASSERT( v4 == NULL );

    p_Delete(&ppp, cf->extRing); p_Delete(&zFactor, cf->extRing);
    p_Delete(&yFactor, cf->extRing);
    n_Delete(&z, cf); n_Delete(&y, cf); n_Delete(&w, cf);
    n_Delete(&theGcd, cf); p_Delete(&v, cf->extRing); n_Delete(&v1, cf);
    n_Delete(&v2, cf); n_Delete(&v3, cf); n_Delete(&v4, cf);

    rDelete(s); // kills 'cf' and 'r' as well
  }
  void test_Z_17_Ext_a()
  {
    clog << "Start by creating Z_17[a]..." << endl;

    char* n[] = {(char*)"a"};
    ring r = rDefault( 17, 1, n);   // Z/17Z[a]
    TS_ASSERT_DIFFERS( r, NULLp );

    PrintRing(r);

    TS_ASSERT( rField_is_Domain(r) );
    TS_ASSERT( !rField_is_Q(r) );

    TS_ASSERT( rField_is_Zp(r) );
    TS_ASSERT( rField_is_Zp(r, 17) );

    TS_ASSERT_EQUALS( rVar(r), 1);

    poly minPoly = p_ISet(1, r);                    // minPoly = 1
    p_SetExp(minPoly, 1, 2, r); p_Setm(minPoly, r); // minPoly = a^2
    minPoly = p_Add_q(minPoly, p_ISet(3, r), r);    // minPoly = a^2 + 3
    ideal minIdeal = idInit(1);                     // minIdeal = < 0 >
    minIdeal->m[0] = minPoly;                       // minIdeal = < a^2 + 3 >

    n_coeffType type = nRegister(n_algExt, naInitChar);
    TS_ASSERT(type == n_algExt);

    r->qideal = minIdeal;
    AlgExtInfo extParam;
    extParam.r = r;

    clog << "Next create the extension field Z_17[a]/<a^2+3>..." << endl;

    const coeffs cf = nInitChar(type, &extParam);   // Z_17[a]/<a^2+3>

    if( cf == NULL )
      TS_FAIL("Could not get needed coeff. domain");

    TS_ASSERT_DIFFERS( cf->cfCoeffWrite, NULLp );

    if( cf->cfCoeffWrite != NULL )
    {
      clog << "Coeff-domain: "  << endl;
      n_CoeffWrite(cf); PrintLn();
    }

    TS_ASSERT( nCoeff_is_algExt(cf) );
    TS_ASSERT( !nCoeff_is_transExt(cf) );

    // some tests for the coefficient field represented by cf:
    TestArithCf(cf);
    TestSumCf(cf, 10);
    TestSumCf(cf, 100);
    TestSumCf(cf, 101);
    TestSumCf(cf, 1001);
    TestSumCf(cf, 9000);

    clog << "Finally create the polynomial ring (Z_17[a]/<a^2+3>)[u, v, w]..."
         << endl;

    char* m[] = {(char*)"u", (char*)"v", (char*)"w"};
    ring s = rDefault(cf, 3, m);   // (Z_17[a]/<a^2+3>)[u, v, w]
    TS_ASSERT_DIFFERS(s, NULLp);

    PrintRing(s);

    TS_ASSERT( rField_is_Domain(s) );
    TS_ASSERT( !rField_is_Q(s) );
    TS_ASSERT( !rField_is_Zp(s) );
    TS_ASSERT( !rField_is_Zp(s, 11) );
    TS_ASSERT( !rField_is_Zp(s, 17) );
    TS_ASSERT( !rField_is_GF(s) );
    TS_ASSERT( (s->cf->extRing!=NULL) );
    TS_ASSERT( !rField_is_GF(s, 25) );
    TS_ASSERT_EQUALS(rVar(s), 3);

    Test(s);

    rDelete(s); // kills 'cf' and 'r' as well
  }
  void test_Q_Ext_s_t()
  {
    clog << "Start by creating Q[s, t]..." << endl;

    char* n[] = {(char*)"s", (char*)"t"};
    ring r = rDefault( 0, 2, n);   // Q[s, t]
    TS_ASSERT_DIFFERS( r, NULLp );

    PrintRing(r);

    TS_ASSERT( rField_is_Domain(r) );
    TS_ASSERT( rField_is_Q(r) );

    TS_ASSERT( !rField_is_Zp(r) );
    TS_ASSERT( !rField_is_Zp(r, 17) );

    TS_ASSERT_EQUALS( rVar(r), 2);

    n_coeffType type = nRegister(n_transExt, ntInitChar);
    TS_ASSERT(type == n_transExt);

    TransExtInfo extParam;
    extParam.r = r;

    clog << "Next create the rational function field Q(s, t)..." << endl;

    const coeffs cf = nInitChar(type, &extParam);   // Q(s, t)

    if( cf == NULL )
      TS_FAIL("Could not get needed coeff. domain");

    TS_ASSERT_DIFFERS( cf->cfCoeffWrite, NULLp );

    if( cf->cfCoeffWrite != NULL )
    {
      clog << "Coeff-domain: "  << endl;
      n_CoeffWrite(cf); PrintLn();
    }

    TS_ASSERT( !nCoeff_is_algExt(cf) );
    TS_ASSERT( nCoeff_is_transExt(cf) );

    // some tests for the coefficient field represented by cf:
    TestArithCf(cf);
    TestSumCf(cf, 10);
    TestSumCf(cf, 100);
    TestSumCf(cf, 101);
    TestSumCf(cf, 1001);
    TestSumCf(cf, 9000);

    clog << "Finally create the polynomial ring Q(s, t)[x, y, z]..."
         << endl;

    char* m[] = {(char*)"x", (char*)"y", (char*)"z"};
    ring s = rDefault(cf, 3, m);   // Q(s, t)[x, y, z]
    TS_ASSERT_DIFFERS(s, NULLp);

    PrintRing(s);

    TS_ASSERT( rField_is_Domain(s) );
    TS_ASSERT( !rField_is_Q(s) );
    TS_ASSERT( !rField_is_Zp(s) );
    TS_ASSERT( !rField_is_Zp(s, 11) );
    TS_ASSERT( !rField_is_Zp(s, 17) );
    TS_ASSERT( !rField_is_GF(s) );
    TS_ASSERT( (s->cf->extRing!=NULL) );
    TS_ASSERT( !rField_is_GF(s, 25) );
    TS_ASSERT_EQUALS(rVar(s), 3);

    Test(s);

    /* some special tests: */
    poly v1 = NULL;
    plusTerm(v1, 1, 1, 1, cf->extRing);       // s
    plusTerm(v1, 1, 1, 0, cf->extRing);       // s + 1
    poly v2 = NULL;
    plusTerm(v2, 1, 1, 1, cf->extRing);       // s
    plusTerm(v2, 2, 2, 1, cf->extRing);       // s + 2t
    poly v = p_Mult_q(v1, v2, cf->extRing);   // (s + 1) * (s + 2t)
    number v_n = toFractionNumber(v, cf);
    PrintSized(v_n, cf);
    poly w1 = NULL;
    plusTerm(w1, 1, 1, 1, cf->extRing);       // s
    plusTerm(w1, 1, 1, 0, cf->extRing);       // s + 1
    poly w2 = NULL;
    plusTerm(w2, 3, 1, 1, cf->extRing);       // 3s
    plusTerm(w2, -7, 2, 1, cf->extRing);      // 3s - 7t
    poly w = p_Mult_q(w1, w2, cf->extRing);   // (s + 1) * (3s - 7t)
    number w_n = toFractionNumber(w, cf);
    PrintSized(w_n, cf);
    number vOverW_n = n_Div(v_n, w_n, cf);
    PrintSized(vOverW_n, cf);
    number wOverV_n = n_Invers(vOverW_n, cf);
    PrintSized(wOverV_n, cf);
    number prod = n_Mult(vOverW_n, wOverV_n, cf);
    PrintSized(prod, cf);
    number tmp; number nn = n_Copy(vOverW_n, cf);
    for (int i = 1; i <= 6; i++)
    {
      tmp = n_Div(nn, v_n, cf);
      n_Delete(&nn, cf);
      nn = tmp;
      clog << i << ". "; PrintSized(nn, cf);
    }

    n_Delete(&prod, cf); n_Delete(&nn, cf);
    n_Delete(&v_n, cf); n_Delete(&w_n, cf);
    n_Delete(&vOverW_n, cf); n_Delete(&wOverV_n, cf);

    rDelete(s); // kills 'cf' and 'r' as well
  }
  void test_Q_Ext_Performance()
  {
    clog << "Start by creating Q[q]..." << endl;

    char* n[] = {(char*)"q"};
    ring r = rDefault( 0, 1, n);   // Q[q]
    r->order[0] = ringorder_dp;
    TS_ASSERT_DIFFERS( r, NULLp );

    PrintRing(r);

    TS_ASSERT( rField_is_Domain(r) );
    TS_ASSERT( rField_is_Q(r) );

    TS_ASSERT( !rField_is_Zp(r) );
    TS_ASSERT( !rField_is_Zp(r, 17) );

    TS_ASSERT_EQUALS( rVar(r), 1);

    n_coeffType type = nRegister(n_transExt, ntInitChar);
    TS_ASSERT(type == n_transExt);

    TransExtInfo extParam;
    extParam.r = r;

    clog << "Next create the rational function field Q(q)..." << endl;

    const coeffs cf = nInitChar(type, &extParam);   // Q(q)

    if( cf == NULL )
      TS_FAIL("Could not get needed coeff. domain");

    TS_ASSERT_DIFFERS( cf->cfCoeffWrite, NULLp );

    if( cf->cfCoeffWrite != NULL )
    {
      clog << "Coeff-domain: "  << endl;
      n_CoeffWrite(cf); PrintLn();
    }

    TS_ASSERT( !nCoeff_is_algExt(cf) );
    TS_ASSERT( nCoeff_is_transExt(cf) );

    // some tests for the coefficient field represented by cf:
    TestArithCf(cf);
    TestSumCf(cf, 10);
    TestSumCf(cf, 100);
    TestSumCf(cf, 101);
    TestSumCf(cf, 1001);
    TestSumCf(cf, 9000);

    clog << "Finally create the polynomial ring Q(q)[t]..."
         << endl;

    char* m[] = {(char*)"t"};
    ring s = rDefault(cf, 1, m);   // Q(q)[t]
    s->order[0] = ringorder_dp;
    TS_ASSERT_DIFFERS(s, NULLp);

    PrintRing(s);

    TS_ASSERT( rField_is_Domain(s) );
    TS_ASSERT( !rField_is_Q(s) );
    TS_ASSERT( !rField_is_Zp(s) );
    TS_ASSERT( !rField_is_Zp(s, 11) );
    TS_ASSERT( !rField_is_Zp(s, 17) );
    TS_ASSERT( !rField_is_GF(s) );
    TS_ASSERT( (s->cf->extRing!=NULL) );
    TS_ASSERT( !rField_is_GF(s, 25) );
    TS_ASSERT_EQUALS(rVar(s), 1);

    Test(s);

    /* a special performance test: */
    poly entry = NULL;
    for (int ti = 0; ti <= 20; ti++)
    {
      for (int qi = 0; qi <= 100; qi++)
      {
        int c = rand() % 1000000+1; /* c must not be 0 */
        poly qterm = p_ISet(c, r); p_SetExp(qterm, 1, qi, r);
        p_Setm(qterm, r);
        number qtermAsN = toFractionNumber(qterm, cf);
        poly tterm = p_ISet(1, s); p_SetExp(tterm, 1, ti, s);
        p_Setm(tterm, r); p_SetCoeff(tterm, qtermAsN, s);
        entry = p_Add_q(entry, tterm, s);
      }
    }
    p_Write(entry, s);
    poly qfactor = p_ISet(1, r); p_SetExp(qfactor, 1, 66, r);
    p_Setm(qfactor, r);   /* q^66 */
    number qfactorAsN = toFractionNumber(qfactor, cf);
    number uuu = n_Invers(qfactorAsN, cf);   /* 1 / q^66 */
    poly factor = p_ISet(1, s); p_SetCoeff(factor, uuu, s);   /* 1 / q^66 */
    p_Write(factor, s);
    clog << "starting multiplication..." << endl;
    poly theProduct = p_Mult_q(entry, factor, s);
    p_Write(theProduct, s);
    clog << "...ending multiplication" << endl;
    n_Delete(&qfactorAsN, cf); p_Delete(&theProduct, s);

    /* a very special performance test: */
    specialPoly(entry, s);
    p_Write(entry, s);
    qfactor = p_ISet(1, r); p_SetExp(qfactor, 1, 66, r);
    p_Setm(qfactor, r);   /* q^66 */
    qfactorAsN = toFractionNumber(qfactor, cf);
    uuu = n_Invers(qfactorAsN, cf);   /* 1 / q^66 */
    factor = p_ISet(1, s); p_SetCoeff(factor, uuu, s);   /* 1 / q^66 */
    p_Write(factor, s);
    clog << "starting very special multiplication..." << endl;
    /* The following multiplication + output of the product is very slow
       in the svn/trunk SINGULAR version; see trac ticket #308.
       Here, in the Spielwiese, the result is instantaneous. */
    theProduct = p_Mult_q(entry, factor, s);
    p_Write(theProduct, s);
    clog << "...ending very special multiplication" << endl;
    n_Delete(&qfactorAsN, cf); p_Delete(&theProduct, s);

    rDelete(s); // kills 'cf' and 'r' as well
  }
  void test_Q_Ext_s_t_NestedFractions()
  {
    clog << "Start by creating Q[s, t]..." << endl;

    char* n[] = {(char*)"s", (char*)"t"};
    ring r = rDefault( 0, 2, n);   // Q[s, t]
    TS_ASSERT_DIFFERS( r, NULLp );

    PrintRing(r);

    TS_ASSERT( rField_is_Domain(r) );
    TS_ASSERT( rField_is_Q(r) );

    TS_ASSERT( !rField_is_Zp(r) );
    TS_ASSERT( !rField_is_Zp(r, 17) );

    TS_ASSERT_EQUALS( rVar(r), 2);

    n_coeffType type = nRegister(n_transExt, ntInitChar);
    TS_ASSERT(type == n_transExt);

    TransExtInfo extParam;
    extParam.r = r;

    clog << "Next create the rational function field Q(s, t)..." << endl;

    const coeffs cf = nInitChar(type, &extParam);   // Q(s, t)

    if( cf == NULL )
      TS_FAIL("Could not get needed coeff. domain");

    TS_ASSERT_DIFFERS( cf->cfCoeffWrite, NULLp );

    if( cf->cfCoeffWrite != NULL )
    {
      clog << "Coeff-domain: "  << endl;
      n_CoeffWrite(cf); PrintLn();
    }

    TS_ASSERT( !nCoeff_is_algExt(cf) );
    TS_ASSERT( nCoeff_is_transExt(cf) );

    clog << "Finally create the polynomial ring Q(s, t)[x, y, z]..."
         << endl;

    char* m[] = {(char*)"x", (char*)"y", (char*)"z"};
    ring s = rDefault(cf, 3, m);   // Q(s, t)[x, y, z]
    TS_ASSERT_DIFFERS(s, NULLp);

    PrintRing(s);

    TS_ASSERT( rField_is_Domain(s) );
    TS_ASSERT( !rField_is_Q(s) );
    TS_ASSERT( !rField_is_Zp(s) );
    TS_ASSERT( !rField_is_Zp(s, 11) );
    TS_ASSERT( !rField_is_Zp(s, 17) );
    TS_ASSERT( !rField_is_GF(s) );
    TS_ASSERT( (s->cf->extRing!=NULL) );
    TS_ASSERT( !rField_is_GF(s, 25) );
    TS_ASSERT_EQUALS(rVar(s), 3);

    /* test 1 for nested fractions, i.e. fractional coefficients: */
    poly v1 = NULL;
    plusTermOverQ(v1, 21, 2, 1, 1, cf->extRing);       // 21/2*s
    plusTermOverQ(v1, 14, 3, 1, 0, cf->extRing);       // 21/2*s + 14/3
    number v1_n = toFractionNumber(v1, cf);
    PrintSized(v1_n, cf);
    poly v2 = NULL;
    plusTermOverQ(v2, 7, 5, 1, 1, cf->extRing);       // 7/5*s
    plusTermOverQ(v2, -49, 6, 2, 1, cf->extRing);     // 7/5*s - 49/6*t
    number v2_n = toFractionNumber(v2, cf);
    PrintSized(v2_n, cf);
    number v3_n = n_Div(v1_n, v2_n, cf);   // (45*s + 20) / (6s - 35*t)
    PrintSized(v3_n, cf);
    n_Delete(&v1_n, cf); n_Delete(&v2_n, cf); n_Delete(&v3_n, cf);

    /* test 2 for nested fractions, i.e. fractional coefficients: */
    v1 = NULL;
    plusTermOverQ(v1, 1, 2, 1, 1, cf->extRing);       // 1/2*s
    plusTermOverQ(v1, 1, 1, 1, 0, cf->extRing);       // 1/2*s + 1
    v2 = NULL;
    plusTermOverQ(v2, 1, 1, 1, 1, cf->extRing);       // s
    plusTermOverQ(v2, 2, 3, 2, 1, cf->extRing);       // s + 2/3*t
    poly v3 = p_Mult_q(v1, v2, cf->extRing);   // (1/2*s + 1) * (s + 2/3*t)
    number v_n = toFractionNumber(v3, cf);
    PrintSized(v_n, cf);
    poly w1 = NULL;
    plusTermOverQ(w1, 1, 2, 1, 1, cf->extRing);       // 1/2*s
    plusTermOverQ(w1, 1, 1, 1, 0, cf->extRing);       // 1/2*s + 1
    poly w2 = NULL;
    plusTermOverQ(w2, -7, 5, 1, 0, cf->extRing);      // -7/5
    poly w3 = p_Mult_q(w1, w2, cf->extRing);   // (1/2*s + 1) * (-7/5)
    number w_n = toFractionNumber(w3, cf);
    PrintSized(w_n, cf);
    number z_n = n_Div(v_n, w_n, cf);          // -5/7*s - 10/21*t
    PrintSized(z_n, cf);
    n_Delete(&v_n, cf); n_Delete(&w_n, cf); n_Delete(&z_n, cf);

    rDelete(s); // kills 'cf' and 'r' as well
  }
};

