#include <cxxtest/TestSuite.h>
#include <cxxtest/GlobalFixture.h>

#include "config.h"
#include <misc/auxiliary.h>
#include <omalloc/omalloc.h>

#include <reporter/reporter.h>
#include <resources/feResource.h>

#include <coeffs/coeffs.h>
#include <coeffs/numbers.h>


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


#include "common.h"
using namespace std;


namespace
{

void TestSum(const coeffs r, const unsigned long N)
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
    TS_ASSERT( n_DivBy(s, i, r) );
    res = n_Div(s, i, r);
  
    clog<< "N*(N+1)/2: "; PrintSized(res, r);
    number d = n_Sub(res, sum1, r);
    
    TS_ASSERT( ndIsZeroDivisor(d, r) );
    
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


void TestArith(const coeffs r)
{
  clog << ("TEST: Simple Arithmetics: ");
  clog << endl;

  number a = n_Init(66666, r);
   
  clog<< "a: "; PrintSized(a, r);

  number two = n_Init(2, r);
  
  clog<< "two: "; PrintSized(two, r);

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
  

  n_Delete(&aa, r);
  n_Delete(&aa1, r);
  n_Delete(&aa2, r);


  clog << ( " >>> TEST DONE!" );
  clog << endl;

}



  

BOOLEAN Test(const n_coeffType type, void* p = NULLp)
{
  
  clog << endl;
  clog << ( "----------------------- Testing coeffs: [" + _2S(type) +  ", " + _2S(p) + "]: -----------------------");
  clog << endl;

  const coeffs r = nInitChar( type, p );

  if( r == NULLp )
  {
    clog << ( "Test: could not get this coeff. domain" );
    return FALSE;
  };

  TS_ASSERT_DIFFERS( r->cfCoeffWrite, NULLp );

  if( r->cfCoeffWrite != NULL )
  {
    clog << "Coeff-domain: "  << endl; 
    n_CoeffWrite(r); PrintLn();
  }


  if (getCoeffType(r) == n_GF) //some special test for GF
  {
    number z = nfPar (0, r); // also any integer instead of 0//?
    clog << "Generator: "; PrintSized(z, r);
    n_Delete(&z, r);    
  }

  clog << "Char: " << n_GetChar(r) << endl; 

  
  TS_ASSERT_DIFFERS( r, NULLp );
  nSetChar( r );
  TS_ASSERT_EQUALS( getCoeffType(r), type );

  TS_ASSERT_DIFFERS( r->cfInit, NULLp );
  TS_ASSERT_DIFFERS( r->cfWrite, NULLp );
  TS_ASSERT_DIFFERS( r->cfAdd, NULLp );
  TS_ASSERT_DIFFERS( r->cfDelete, NULLp );

  switch( type )
  {
    case n_Q:
    {
      TS_ASSERT_EQUALS( r->cfInit, nlInit );
      TS_ASSERT_EQUALS( r->cfWrite, nlWrite );
      TS_ASSERT_EQUALS( r->cfAdd, nlAdd );
      TS_ASSERT_EQUALS( r->cfDelete, nlDelete );
      
      TS_ASSERT(  nCoeff_is_Q( r ));
      TS_ASSERT(  nCoeff_is_Domain( r ));

      TS_ASSERT( !nCoeff_has_Units( r )); // ?
      TS_ASSERT( !nCoeff_has_simple_inverse( r ));// ?
      TS_ASSERT( !nCoeff_has_simple_Alloc( r )); // ?

      TS_ASSERT( !nCoeff_is_Ring_2toM( r ));
      TS_ASSERT( !nCoeff_is_Ring_ModN( r ));
      TS_ASSERT( !nCoeff_is_Ring_PtoM( r ));
      TS_ASSERT( !nCoeff_is_Ring_Z( r ));
      TS_ASSERT( !nCoeff_is_Ring( r ));
      TS_ASSERT( !nCoeff_is_Zp( r ));
      TS_ASSERT( !nCoeff_is_numeric( r ));
      TS_ASSERT( !nCoeff_is_R( r ));
      TS_ASSERT( !nCoeff_is_Q_a( r ));
      TS_ASSERT( !nCoeff_is_Zp_a( r ));
      TS_ASSERT( !nCoeff_is_GF( r ));
      TS_ASSERT( !nCoeff_is_long_R( r ));
      TS_ASSERT( !nCoeff_is_long_C( r ));
      TS_ASSERT( !nCoeff_is_CF( r ));
      TS_ASSERT( !nCoeff_is_Extension( r ));
      
      break;
    }
    case n_long_R:
    {
      TS_ASSERT_EQUALS( r->cfInit, ngfInit );
      TS_ASSERT_EQUALS( r->cfWrite, ngfWrite );
      TS_ASSERT_EQUALS( r->cfAdd, ngfAdd );
      TS_ASSERT_EQUALS( r->cfDelete, ngfDelete );
      break;
    }
    case n_long_C:
    {
      TS_ASSERT_EQUALS( r->cfInit, ngcInit );
      TS_ASSERT_EQUALS( r->cfWrite, ngcWrite );
      TS_ASSERT_EQUALS( r->cfAdd, ngcAdd );
      TS_ASSERT_EQUALS( r->cfDelete, ngcDelete );    
      break;
    }
    case n_R:
    {
      TS_ASSERT_EQUALS( r->cfInit, nrInit );
      TS_ASSERT_EQUALS( r->cfWrite, nrWrite );
      TS_ASSERT_EQUALS( r->cfAdd, nrAdd );
  //    TS_ASSERT_EQUALS( r->cfDelete, nrDelete ); // No?
      break;
    }
    case n_GF:
    {
      TS_ASSERT_EQUALS( r->cfInit, nfInit );
      TS_ASSERT_EQUALS( r->cfWrite, nfWrite );
      TS_ASSERT_EQUALS( r->cfAdd, nfAdd );
      //TS_ASSERT_EQUALS( r->cfDelete, nfDelete );
      break;
    }
#ifdef HAVE_RINGS
    case n_Z2m:
    {
      TS_ASSERT_EQUALS( r->cfInit, nr2mInit );
      TS_ASSERT_EQUALS( r->cfWrite, nr2mWrite );
      TS_ASSERT_EQUALS( r->cfAdd, nr2mAdd );
      TS_ASSERT_EQUALS( r->cfDelete, ndDelete );
      break;
    }
    case n_Zn:
    {
      TS_ASSERT_EQUALS( r->cfInit, nrnInit );
      TS_ASSERT_EQUALS( r->cfWrite, nrnWrite );
      TS_ASSERT_EQUALS( r->cfAdd, nrnAdd );
      TS_ASSERT_EQUALS( r->cfDelete, nrnDelete );
      break;
    }
#endif
    default:
    {
      // ...
    }
  }

  TestArith( r );
  TestSum( r, 10 );
  TestSum( r, 100 );
  TestSum( r, 101 );
  TestSum( r, 1001 );
  TestSum( r, 9000 );

  nKillChar( r );

  return TRUE;
}


}



class GlobalPrintingFixture : public CxxTest::GlobalFixture
{
  public:
    bool setUpWorld() {
      clog << endl << ( "<world>" ) << endl;
      feInitResources();
      return true;
    }
    bool tearDownWorld() { clog << endl <<( "</world>" )  << endl; return true; }
    bool setUp() { clog << endl <<( "<test>" )  << endl; return true; }
    bool tearDown() { clog << endl <<( "</test>" )  << endl; return true; }
};


//
// We can rely on this file being included exactly once
// and declare this global variable in the header file.
//
static GlobalPrintingFixture globalPrintingFixture;


class CoeffsTestSuite : public CxxTest::TestSuite 
{
 public:
//   void test_dummy() { float fnum = 2.00001f; TS_ASSERT_DELTA (fnum, 2.0f, 0.0001f);  }

   void test_Z2m4()
   {
#ifdef HAVE_RINGS
     n_coeffType type = nRegister( n_Z2m, nr2mInitChar); TS_ASSERT( type == n_Z2m ); 
     TS_ASSERT( Test(type, (void*) 4) );
#endif
   }

   void test_Zp101()
   {
     n_coeffType type = nRegister( n_Zp, npInitChar); TS_ASSERT( type == n_Zp );
     TS_ASSERT( Test(type, (void*) 101) );
   }

   void test_Z2m8()
   {
#ifdef HAVE_RINGS
     n_coeffType type = nRegister( n_Z2m, nr2mInitChar); TS_ASSERT( type == n_Z2m ); 
     TS_ASSERT( Test(type, (void*) 8) );
#endif
   }
   
   void simple(const n_coeffType _type, cfInitCharProc p)
   {
     n_coeffType type = nRegister( _type, p);
     TS_ASSERT( type == _type ); // ?
     TS_ASSERT( Test(type) );
   }
   
   void test_Q()
   {
     simple(n_Q, nlInitChar);
   }
   
   void test_R()
   {
     simple(n_R, nrInitChar);
   }


   void test_Z()
   {
#ifdef HAVE_RINGS
     simple(n_Z, nrzInitChar);  // No need in GMP?
#endif
   }

   
   void test_GF_toobig()
   {
     n_coeffType type = nRegister( n_GF, nfInitChar);
     TS_ASSERT( type == n_GF );

     GFInfo param;

     param.GFChar= 5;
     param.GFDegree= 12;
     param.GFPar_name= (const char*)"q";

     TS_ASSERT( !Test(type, (void*) &param) );

      // it should not be used by numbers... right? 
      // TODO: what is our policy wrt param-pointer-ownership?
   }

   void test_GF()
   {
     // TODO: what if it was already registered?
     // Q: no way to deRegister a type?
     n_coeffType type = nRegister( n_GF, nfInitChar);
     TS_ASSERT( type == n_GF );

     GFInfo param;

     param.GFChar= 5;
     param.GFDegree= 2;
     param.GFPar_name= (const char*)"Q";

     TS_ASSERT( Test(type, (void*) &param) );

      // it should not be used by numbers... right? 
      // TODO: what is our policy wrt param-pointer-ownership?
   }
   

   void test_Zn3()
   {
#ifdef HAVE_RINGS
     //  TODO(Somebody, This will result in memory corruption at Z_2^m later on (due to the succs. setGMPFloatDigits?)...!?); // ????
     n_coeffType type = nRegister( n_Zn, nrnInitChar); TS_ASSERT( type == n_Zn );

     TS_ASSERT( Test(type, (void*) 3) );
#endif
   }

   void test_Z2m2()
   {
#ifdef HAVE_RINGS
     n_coeffType type = nRegister( n_Z2m, nr2mInitChar); TS_ASSERT( type == n_Z2m );

     TS_ASSERT( Test(type, (void*) 2) );
#endif
   }

   TODO(Somebody, floating arithmetics via GMP rely on two global variables (see setGMPFloatDigits). Please fix it!);

   void test_LR()
   {
     setGMPFloatDigits( 10, 5 ); // Init global variables in mpr_complex.cc for gmp_float's... // Note that this seems also to be required for Z_2^m (and Zn?)!????
     simple(n_long_R, ngfInitChar);
   }
   
   void test_LC()
   {
     setGMPFloatDigits( 10, 5 ); // Init global variables in mpr_complex.cc for gmp_float's... // Note that this seems also to be required for Z_2^m (and Zn?)!????
     simple(n_long_C, ngcInitChar);
   }  

   
   // polynomial rings needed for extentions: n_Zp_a, n_Q_a!
   
};

