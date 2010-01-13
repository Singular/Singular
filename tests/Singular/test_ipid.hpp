#ifndef _test_ipid_hpp_
#define _test_ipid_hpp_

#include <Singular/utils.h>
#include <Singular/mod2.h>
#include <kernel/structs.h>
#include <cppunit/extensions/HelperMacros.h>

namespace Singular {
  namespace tests {
    class IpIdTest : public CPPUNIT_NS::TestFixture {

      CPPUNIT_TEST_SUITE(Singular::tests::IpIdTest);
      CPPUNIT_TEST( test_iiS2I );
      CPPUNIT_TEST( test_enterid );
      CPPUNIT_TEST( test_killid );
      CPPUNIT_TEST( test_killhdl );
      CPPUNIT_TEST( test_ggetid );
      CPPUNIT_TEST_SUITE_END();

    public:
      IpIdTest();
      virtual ~IpIdTest() {}

      void setUp();
      void tearDown();

    protected:
      void test_iiS2I();
      void test_enterid();
      void test_killid();
      void test_killhdl();
      void test_ggetid();
      
    private:
      package _basePack;
    };
  }
}
#endif /* _test_ipid_hpp_ */
