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

      TS_ASSERT( n_DivBy(ii, t, r->cf) );
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
  TestSum( r, 10 );
  TestSum( r, 100 );
  TestSum( r, 101 );
  TestSum( r, 1001 );
  TestSum( r, 9000 );
}

}

class PolysTestSuite : public CxxTest::TestSuite 
{
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

    TS_ASSERT_DIFFERS( cf->cfCoeffWrite, NULLp );
  
    if( cf->cfCoeffWrite != NULL )
    {
      clog << "Coeff-domain: "  << endl; 
      n_CoeffWrite(cf); PrintLn();
    }

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

    //Test(s);

    rDelete(s); // kills 'cf' and 'r' as well
  }
};

