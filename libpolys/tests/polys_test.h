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
  void checkInverse(number n, const coeffs cf)
  {
    clog << "n = "; n_Write(n, cf);
    number n1 = n_Invers(n, cf);
    clog << "==> n^(-1) = "; n_Write(n1, cf);
    number n2 = n_Mult(n, n1, cf);
    clog << "(check: n * n^(-1) = "; n_Write(n2, cf);
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
     StringSetS("ressources in use (as reported by feStringAppendResources(0):\n");
     feStringAppendResources(0);
     PrintS(StringAppendS("\n"));

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

    n_coeffType type = nRegister(n_Ext, naInitChar); 
    TS_ASSERT(type == n_Ext);

    ExtInfo extParam;
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
    
    // some tests for the coefficient field represented by cf:
    TestArithCf(cf);
    TestSumCf(cf, 10);
    TestSumCf(cf, 100);
    TestSumCf(cf, 101);
    TestSumCf(cf, 1001);
    TestSumCf(cf, 9000);

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
         << "Now let's compute some inverses in Q[a]/<a2+1>..."
         << endl;
         
    number a = n_Par(1, cf);
    number n1 = n_Init(1, cf);
    number n5 = n_Init(5, cf);
    number n17 = n_Init(17, cf);
    number u; number v;
    
    u = n_Add(a, n1, cf); checkInverse(u, cf); n_Delete(&u, cf); // a + 1
    u = n_Sub(a, n1, cf); checkInverse(u, cf); n_Delete(&u, cf); // a - 1
    u = n_Add(a, n5, cf); checkInverse(u, cf); n_Delete(&u, cf); // a + 5
    u = n_Sub(a, n5, cf); checkInverse(u, cf); n_Delete(&u, cf); // a - 5
    v = n_Mult(a, n17, cf);
    u = n_Add(v, n5, cf); n_Delete(&v, cf);                      // 17a + 5
       checkInverse(u, cf); n_Delete(&u, cf);
    
    n_Delete(&a, cf); n_Delete(&n1, cf); n_Delete(&n5, cf);
    n_Delete(&n17, cf);

    rDelete(s); // kills 'cf' and 'r' as well
  }
};

