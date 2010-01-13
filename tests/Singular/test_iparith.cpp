#include <iostream>
#include <sstream>

#include <stdlib.h>

#include "test_iparith.hpp"
#include <Singular/mod2.h>
#include <Singular/grammar.h>
#include <Singular/ipid.h>

using namespace Singular::tests;

CPPUNIT_TEST_SUITE_REGISTRATION( Singular::tests::IpArithTest );

extern int iiInitArithmetic();
extern int iiArithAddItem2list(void **list, long  *item_count, long sizeofitem,
                               void *newitem);
extern int iiArithFindCmd(const char *szName);
extern char *iiArithGetCmd( int nPos );
extern int iiArithRemoveCmd(const char *szName);
extern int iiArithAddCmd(const char *szName, short nAlias, short nTokval,
                         short nToktype, short nPos);

/* To be tested

static BOOLEAN jjBREAK0(leftv res, leftv v)
static BOOLEAN jjBREAK1(leftv res, leftv v)
static BOOLEAN jjCALL1ARG(leftv res, leftv v)
static BOOLEAN jjCALL2ARG(leftv res, leftv u)
static BOOLEAN jjCALL3ARG(leftv res, leftv u)
static BOOLEAN jjCOEF_M(leftv res, leftv v)
static BOOLEAN jjDIVISION4(leftv res, leftv v)
static BOOLEAN jjIDEAL_PL(leftv res, leftv v)
static BOOLEAN jjINTERSECT_PL(leftv res, leftv v)
static BOOLEAN jjINTVEC_PL(leftv res, leftv v)
static BOOLEAN jjJET4(leftv res, leftv u)
static BOOLEAN jjKLAMMER_PL(leftv res, leftv u)
static BOOLEAN jjLIST_PL(leftv res, leftv v)
static BOOLEAN jjNAMES0(leftv res, leftv v)
static BOOLEAN jjOPTION_PL(leftv res, leftv v)
static BOOLEAN jjREDUCE4(leftv res, leftv u)
static BOOLEAN jjREDUCE5(leftv res, leftv u)
static BOOLEAN jjRESERVED0(leftv res, leftv v)
static BOOLEAN jjSTRING_PL(leftv res, leftv v)
static BOOLEAN jjSTATUS_M(leftv res, leftv v)
static BOOLEAN jjSUBST_M(leftv res, leftv u)
static BOOLEAN jjSTD_HILB_WP(leftv res, leftv INPUT)

BOOLEAN iiExprArith2(lef
tv res, leftv a, int op, leftv b, BOOLEAN proccall)
BOOLEAN iiExprArith1(leftv res, leftv a, int op)
BOOLEAN iiExprArith3(leftv res, int op, leftv a, leftv b, leftv c)
BOOLEAN jjANY2LIST(leftv res, leftv v, int cnt)
BOOLEAN iiExprArithM(leftv res, leftv a, int op)
int IsCmd(const char *n, int & tok)
static int iiTabIndex(const jjValCmdTab dArithTab, const int len, const int op)
const char * Tok2Cmdname(int tok)
static int _gentable_sort_cmds( const void *a, const void *b )
int iiInitArithmetic()
int iiArithAddItem2list(void **list, long  *item_count, long sizeofitem, void *newitem)
int iiArithFindCmd(const char *szName)
char *iiArithGetCmd( int nPos )
int iiArithRemoveCmd(const char *szName)
int iiArithAddCmd(const char *szName, short nAlias, short nTokval,
                  short nToktype, short nPos )

GENTABLE
void ttGen1()
void ttGen2b()
void ttGen4()
const char * Tok2Cmdname(int tok)
short IsCmdToken(short tok)
 */
void IpArithTest::setUp() {
  iiInitArithmetic();
}

void IpArithTest::tearDown() {
}

void IpArithTest::test_iiArithFindCmd() 
{
  const char *name = "LIB";
  double fStart = Time();
  int pos = iiArithFindCmd(name);
  double fEnd = Time();

  printf("Time: %f\n", fEnd-fStart);
  CPPUNIT_ASSERT(std::string(name) == std::string(iiArithGetCmd(pos)));

  const char * name2 = "nix";
  pos = iiArithFindCmd(name2);
  CPPUNIT_ASSERT(std::string(name2) == std::string(iiArithGetCmd(pos)));
  
  return;
}

void IpArithTest::test_Timing1()
{
  double fStart;
  double fEnd;
  int nMax = 0;

  fStart = Time();
  while(iiArithGetCmd(nMax)!=NULL) nMax++;
  fEnd = Time();
  printf("Find sizeof sArithBase Time: %f %f %f\n", fEnd, fStart, fEnd-fStart);
  
  fStart = Time();
  for(int i=0; i<1000000; i++) {
    int pos = random() % nMax;
    CPPUNIT_ASSERT(iiArithGetCmd(pos)!=NULL);
    
  }
  fEnd = Time();
  printf("Find sizeof iiArithGetCmd 1000000 loop Time: %f\n", fEnd-fStart);

  fStart = Time();
  int rc = iiArithAddCmd("TestCMD2", 0, 999, 0, -1);
  fEnd = Time();
  printf("Add rc=%d - %f\n", rc, fEnd-fStart);
  CPPUNIT_ASSERT_EQUAL(0, rc);
  CPPUNIT_ASSERT(0 != iiArithAddCmd("TestCMD2", 0, 999, 0, -1));
  fStart = Time();
  iiArithRemoveCmd("TestCMD2");
  fEnd = Time();
  printf("Remove rc=%d - %f\n", rc, fEnd-fStart);

  fStart = Time();
  rc = iiArithAddCmd("ATestCMD2", 0, 998, 0, -1);
  fEnd = Time();
  printf("Add rc=%d - %f\n", rc, fEnd-fStart);
  CPPUNIT_ASSERT_EQUAL(0, rc);
  CPPUNIT_ASSERT(0 != iiArithAddCmd("ATestCMD2", 0, 998, 0, -1));
  

  fStart = Time();
  for(int i=0; i<10000; i++) {
    int pos = random() % nMax;
    CPPUNIT_ASSERT_EQUAL(0, iiArithAddCmd("TestCMD", 0, 999, 0, -1));
    iiArithRemoveCmd("TestCMD");
}
  fEnd = Time();
  printf("Find sizeof iiArithAdd/RemoveCmd 10000 loop Time: %f\n", fEnd-fStart);

  // end of performance test
  printf("Time: %f\n", fEnd-fStart);
  return;
}
