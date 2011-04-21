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


#include <polys/monomials/ring.h>

#include "common.h"
using namespace std;



class GlobalPrintingFixture : public CxxTest::GlobalFixture
{
  public:
    bool setUpWorld() {
      clog << ( "<world>" ) << endl;
      feInitResources();
      
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
    bool tearDownWorld() { clog <<( "</world>" )  << endl; return true; }
    bool setUp() { clog <<( "<test>" )  << endl; return true; }
    bool tearDown() { clog <<( "</test>" )  << endl; return true; }
};


//
// We can rely on this file being included exactly once
// and declare this global variable in the header file.
//
static GlobalPrintingFixture globalPrintingFixture;


namespace
{
  void PrintRing(const ring r)
  {
    rWrite(r); PrintLn();
  #ifdef  RDEBUG
    rDebugPrint(r); PrintLn();
  #endif
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

     rDelete(r); // kills 'cf' as well!
   }

};

