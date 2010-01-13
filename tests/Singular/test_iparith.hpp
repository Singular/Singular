#ifndef _test_iparith_hpp_
#define _test_iparith_hpp_

#include <cppunit/extensions/HelperMacros.h>
#include "utils.hpp"

namespace Singular {
  namespace tests {
    class IpArithTest : public CPPUNIT_NS::TestFixture {

      CPPUNIT_TEST_SUITE(Singular::tests::IpArithTest);
      CPPUNIT_TEST( test_iiArithFindCmd );
      CPPUNIT_TEST( test_Timing1 );
      CPPUNIT_TEST_SUITE_END();

    public:
      IpArithTest() {}
      virtual ~IpArithTest() {}

      void setUp();
      void tearDown();

    protected:
      void test_iiArithFindCmd();
      void test_Timing1();
    };
  }
}
#endif /* _test_iparith_hpp_ */
