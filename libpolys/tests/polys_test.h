
#include "config.h"
#include <misc/auxiliary.h>
#include <omalloc/omalloc.h>

#include <reporter/reporter.h>

#include <coeffs/coeffs.h>
#include <coeffs/numbers.h>


#include <polys/monomials/ring.h>

#include "common.h"
using namespace std;



class PolysTestSuite : public CxxTest::TestSuite 
{
public:
   void test_1()
   {
     char* n = "x";
     ring r = rDefault( 11, 1, &n);
     TS_ASSERT_DIFFERS( r, NULLp );

     clog << "RING: Z/11 [x]: " << endl;
     rWrite(r);
#ifdef  RDEBUG
     rDebugPrint(r);
#endif
     
     rKill(r);
   }
};

