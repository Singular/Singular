
#include <cppunit/TestFixture.h>


#include <tests/Singular/test_ipid.hpp>
#include <tests/Singular/test_iparith.hpp>

namespace Singular {
  namespace tests {
    
    class Suite : public CPPUNIT_NS::TestFixture {
    public:
      static CPPUNIT_NS::Test *suite() {
        CPPUNIT_NS::TestSuite *suiteOfTests = 
          new CPPUNIT_NS::TestSuite( "SingularTestSuite" );


      suiteOfTests->addTest( IpIdTest::suite() );
      suiteOfTests->addTest( IpArithTest::suite() );

      return suiteOfTests;
      }
    };
  }
}
