


#include <misc/auxiliary.h>

class SimplestTestSuite : public CxxTest::TestSuite 
{
public:
   void testMyTest()
   {
		float fnum = 2.00001f;		
		TS_ASSERT_DELTA (fnum, 2.0f, 0.0001f);
   }
   
   void testSIZEOFLONG()
   {
      TS_ASSERT( sizeof(long) == SIZEOF_LONG ); // correct config.h? 
   }
   
};

