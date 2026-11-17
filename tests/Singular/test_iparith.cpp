#include <iostream>
#include <sstream>

#include <stdlib.h>

#include "test_iparith.hpp"
#include <kernel/mod2.h>
#include <kernel/polys.h>
#include <polys/monomials/ring.h>
#include <Singular/grammar.h>
#include <Singular/ipid.h>
#include <Singular/ipshell.h>
#include <Singular/htable.h>

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
  CPPUNIT_ASSERT(pos==-1);

  return;
}


void IpArithTest::test_HTableCommandRegistration()
{
  int pos = iiArithFindCmd("htable");
  CPPUNIT_ASSERT(pos >= 0);
  CPPUNIT_ASSERT(std::string("htable") == std::string(iiArithGetCmd(pos)));
  CPPUNIT_ASSERT(std::string("htable") == std::string(Tok2Cmdname(HTABLE_CMD)));

  pos = iiArithFindCmd("getValue");
  CPPUNIT_ASSERT(pos >= 0);
  CPPUNIT_ASSERT(std::string("getValue") == std::string(iiArithGetCmd(pos)));
  CPPUNIT_ASSERT(std::string("getValue") == std::string(Tok2Cmdname(GETVALUE_CMD)));

  pos = iiArithFindCmd("parentOfValue");
  CPPUNIT_ASSERT(pos >= 0);
  CPPUNIT_ASSERT(std::string("parentOfValue") == std::string(iiArithGetCmd(pos)));
  CPPUNIT_ASSERT(std::string("parentOfValue") == std::string(Tok2Cmdname(PARENTOFVALUE_CMD)));

  pos = iiArithFindCmd("typeOfValue");
  CPPUNIT_ASSERT(pos >= 0);
  CPPUNIT_ASSERT(std::string("typeOfValue") == std::string(iiArithGetCmd(pos)));
  CPPUNIT_ASSERT(std::string("typeOfValue") == std::string(Tok2Cmdname(TYPEOFVALUE_CMD)));
}

void IpArithTest::test_HTableCore()
{
  stablerec *t = t_createTable(3);

  sleftv v;
  v.Init();
  v.rtyp = INT_CMD;
  v.data = (void*)42;

  CPPUNIT_ASSERT(!t_addTable(t, omStrDup("alpha"), &v));
  CPPUNIT_ASSERT_EQUAL(1, t_countTable(t));

  sleftv out;
  out.Init();
  CPPUNIT_ASSERT(!t_copyTableVal(&out, t, "alpha"));
  CPPUNIT_ASSERT_EQUAL(INT_CMD, out.Typ());
  CPPUNIT_ASSERT_EQUAL(42, (int)(long)out.Data());
  out.CleanUp();

  CPPUNIT_ASSERT(!t_addTable(t, omStrDup("alpha"), &v));
  CPPUNIT_ASSERT_EQUAL(1, t_countTable(t));

  stablerec *copy = t_cloneTable(t);
  CPPUNIT_ASSERT(copy != NULL);
  CPPUNIT_ASSERT_EQUAL(1, t_countTable(copy));
  t_destroyTable(copy);

  t_destroyTable(t);
  v.CleanUp();

  char **r1_vars = (char**)omAlloc(sizeof(char*));
  r1_vars[0] = omStrDup("x");
  ring r1 = rDefault(32003, 1, r1_vars);

  char **r2_vars = (char**)omAlloc(sizeof(char*));
  r2_vars[0] = omStrDup("y");
  ring r2 = rDefault(32003, 1, r2_vars);

  ring save = currRing;
  rChangeCurrRing(r1);

  stablerec *rt = t_createTable(3);
  sleftv pv;
  pv.Init();
  pv.rtyp = POLY_CMD;
  pv.data = (void*)p_ISet(1, r1);
  pSetExp((poly)pv.data, 1, 1);
  pSetm((poly)pv.data);

  CPPUNIT_ASSERT(!t_addTable(rt, omStrDup("x"), &pv));

  rChangeCurrRing(r2);
  out.Init();
  CPPUNIT_ASSERT(t_copyTableVal(&out, rt, "x"));
  CPPUNIT_ASSERT(errorreported);
  errorreported = 0;
  out.CleanUp(r2);

  rChangeCurrRing(r1);
  out.Init();
  CPPUNIT_ASSERT(!t_copyTableVal(&out, rt, "x"));
  CPPUNIT_ASSERT_EQUAL(POLY_CMD, out.Typ());
  out.CleanUp(r1);

  pv.CleanUp(r1);
  t_destroyTable(rt);
  rChangeCurrRing(save);
  rKill(r1);
  rKill(r2);
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
