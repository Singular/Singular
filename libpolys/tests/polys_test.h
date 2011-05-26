#include "config.h"

#include "common.h"
using namespace std;

// the following headers are private...
#include <coeffs/longrat.h>
#include <coeffs/gnumpfl.h>
#include <coeffs/gnumpc.h>
#include <coeffs/shortfl.h>
#include <coeffs/ffields.h>
#include <coeffs/modulop.h>
#include <coeffs/rmodulon.h>
#include <coeffs/rmodulo2m.h>
#include <coeffs/rintegers.h>

#include <polys/ext_fields/algext.h>
#include <polys/monomials/ring.h>
#include <polys/monomials/p_polys.h>

#include <polys/simpleideals.h>

#ifdef HAVE_FACTORY
#include <polys/clapsing.h>
#include <factory/factory.h>
#endif

class MyGlobalPrintingFixture : public GlobalPrintingFixture
{
  public:
    virtual bool setUpWorld()
    {
    
      GlobalPrintingFixture::setUpWorld();
      

      TS_ASSERT_EQUALS( nRegister( n_Zp, npInitChar), n_Zp );
      TS_ASSERT_EQUALS( nRegister( n_GF, nfInitChar), n_GF );
      TS_ASSERT_EQUALS( nRegister( n_R, nrInitChar), n_R );
      TS_ASSERT_EQUALS( nRegister( n_Q, nlInitChar), n_Q );
      TS_ASSERT_EQUALS( nRegister( n_R, nrInitChar), n_R );
      
#ifdef HAVE_RINGS
      TS_ASSERT_EQUALS( nRegister( n_Z, nrzInitChar), n_Z ); // these are UNusable at the moment!
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

    const char* s = StringAppendS("");

    std::stringstream ss;  ss << s;

    StringSetS("");  
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

  TODO(somebody, fix the delete method!);

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
    poly t = p_ISet(c, r); p_SetExp(t, i, exp, r); p_Setm(t, r);
    p = p_Add_q(p, t, r);
  }
  void checkInverse(number n, const coeffs cf)
  {
    clog << "n = "; n_Write(n, cf);
    number n1 = n_Invers(n, cf);
    clog << "==> n^(-1) = "; n_Write(n1, cf);
    number n2 = n_Mult(n, n1, cf);
    clog << "(check: n * n^(-1) = "; n_Write(n2, cf);
    TS_ASSERT( n_IsOne(n2, cf) );
    n_Delete(&n1, cf); n_Delete(&n2, cf);
  }
  void TestArithCf(const coeffs r)
  {
    clog << ("TEST: Simple Arithmetics: ");
    clog << endl;

    number two = n_Init(2, r);

    number t = n_Init(1, r);  
    ndInpAdd(t, t, r);  
    TS_ASSERT( n_Equal(two, t, r) );
    n_Delete(&t, r);
  
    if( getCoeffType(r) == n_Q )
    {
      number t = n_Init(1, r);  
      nlInpAdd(t, t, r);
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
    ndInpMult(s, i, r);
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
      ndInpAdd(s, i, r); // s += i

      i = n_Neg(i, r);
      ndInpAdd(ss, i, r); // ss -= i
      
      n_Delete(&i, r);    
    }
    clog<< "ss: "; PrintSized(ss, r);  

    ss = n_Neg(ss, r); // ss = -ss
   
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

    char* n[] = {"t"};
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

    char* n[] = {"s"};
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
     
     char* n[] = {"x", "y", "z"};
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

     char* n[] = {"x", "y", "z", "u"};
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

     char* n[] = {"t"};

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

    char* n[] = {"a"};
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

    AlgExtInfo extParam;
    extParam.r = r;
    extParam.i = minIdeal;
    
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
    
    char* m[] = {"x", "y"};
    ring s = rDefault(cf, 2, m);   // (Q[a]/<a2+1>)[x, y]
    TS_ASSERT_DIFFERS(s, NULLp);

    PrintRing(s);

    TS_ASSERT( rField_is_Domain(s) );
    TS_ASSERT( !rField_is_Q(s) );
    TS_ASSERT( !rField_is_Zp(s) );
    TS_ASSERT( !rField_is_Zp(s, 11) );
    TS_ASSERT( !rField_is_Zp(s, 13) );
    TS_ASSERT( !rField_is_GF(s) );
    TS_ASSERT( rField_is_Extension(s) );
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

    char* n[] = {"b"};
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

    AlgExtInfo extParam;
    extParam.r = r;
    extParam.i = minIdeal;
    
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
    
    char* m[] = {"u", "v", "w"};
    ring s = rDefault(cf, 3, m);   // (Q[b]/<b^7+17>)[u, v, w]
    TS_ASSERT_DIFFERS(s, NULLp);

    PrintRing(s);

    TS_ASSERT( rField_is_Domain(s) );
    TS_ASSERT( !rField_is_Q(s) );
    TS_ASSERT( !rField_is_Zp(s) );
    TS_ASSERT( !rField_is_Zp(s, 11) );
    TS_ASSERT( !rField_is_Zp(s, 13) );
    TS_ASSERT( !rField_is_GF(s) );
    TS_ASSERT( rField_is_Extension(s) );
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
    TS_ASSERT( v == NULL );
    p_Delete(&v, cf->extRing);
    
    clog << endl
         << "Now let's check an ext_gcd computation in Q[b]..."
         << endl;
         
    poly zFactor; poly yFactor;
    poly ppp = p_ExtGcd((poly)z, zFactor, (poly)y, yFactor, cf->extRing);
    v = (poly)n_Sub(theGcd, (number)ppp, cf);
    TS_ASSERT( v == NULL );
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

    char* n[] = {"a"};
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

    AlgExtInfo extParam;
    extParam.r = r;
    extParam.i = minIdeal;
    
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
    
    char* m[] = {"u", "v", "w"};
    ring s = rDefault(cf, 3, m);   // (Z_17[a]/<a^2+3>)[u, v, w]
    TS_ASSERT_DIFFERS(s, NULLp);

    PrintRing(s);

    TS_ASSERT( rField_is_Domain(s) );
    TS_ASSERT( !rField_is_Q(s) );
    TS_ASSERT( !rField_is_Zp(s) );
    TS_ASSERT( !rField_is_Zp(s, 11) );
    TS_ASSERT( !rField_is_Zp(s, 17) );
    TS_ASSERT( !rField_is_GF(s) );
    TS_ASSERT( rField_is_Extension(s) );
    TS_ASSERT( !rField_is_GF(s, 25) );
    TS_ASSERT_EQUALS(rVar(s), 3);

    Test(s);
    /*
#ifdef HAVE_FACTORY
    poly f = p_ISet(3, s);
    p_SetExp(f, 1, 3, s);
    p_SetExp(f, 2, 1, s);
    p_SetExp(f, 3, 5, s);
    p_Setm(f, r);   // 3*u^3*v*w^5
    plusTerm(f, -2, 1, 6, s); // -2*u^6 + 3*u^3*v*w^5
    p_Write(f, s);
    poly g = p_ISet(7, s);
    p_SetExp(g, 1, 5, s);
    p_SetExp(g, 2, 6, s);
    p_SetExp(g, 3, 2, s);
    p_Setm(g, r);   // 7*u^5*v^6*w^2
    plusTerm(g, 8, 1, 4, s); // 7*u^5*v^6*w^2 + 8*u^4
    p_Write(g, s);
    poly h = singclap_gcd(f, g, s);   // at least u^3, destroys f and g
    p_Write(h, s);
    p_Test(h, s);
    p_Delete(&h, s);
#endif
    */
    rDelete(s); // kills 'cf' and 'r' as well
  }
};

