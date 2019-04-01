#include <iostream>
#include <sstream>

#include "test_ipid.hpp"
#include <kernel/mod2.h>
#include <Singular/ipid.h>

#include "utils.hpp"

using namespace Singular::tests;

CPPUNIT_TEST_SUITE_REGISTRATION( Singular::tests::IpIdTest );

/* To be tested:
idhdl   enterid(const char * s, int lev, idtyp t, idhdl* root, BOOLEAN init)
void    killid(const char * id, idhdl * ih)
void    killhdl(idhdl h, package proot)
void    killhdl2(idhdl h, idhdl * ih, ring r)
idhdl   ggetid(const char *n, BOOLEAN local, idhdl *packhdl)
idhdl   ggetid(const char *n)
void    ipListFlag(idhdl h)
lists   ipNameList(idhdl root)
static int ipSwapId(idhdl tomove, idhdl &root1, idhdl &root2)
void    ipMoveId(idhdl tomove)
const char * piProcinfo(procinfov pi, const char *request)
BOOLEAN piKill(procinfov pi)
void    paCleanUp(package pack)
char    *idhdl2id(idhdl pck, idhdl h)
void    iiname2hdl(const char *name, idhdl *pck, idhdl *h)
idhdl   packFindHdl(package r)

idhdl idrec::get(const char * s, int lev)
idhdl idrec::set(const char * s, int lev, idtyp t, BOOLEAN init)
char * idrec::String()

void proclevel::push(char *n)
void proclevel::pop()
 */

IpIdTest::IpIdTest() {
  _basePack=(package)omAlloc0(sizeof(*basePack));
  basePack=_basePack;
  currPack=_basePack;
  idhdl h=enterid("Top", 0, PACKAGE_CMD, &IDROOT, TRUE);
  //enterid("Top", 0, PACKAGE_CMD, &IDROOT, TRUE);
  //  IDPACKAGE(h)=basePack;
  currPackHdl=h;
  //  basePackHdl=h;
}

void IpIdTest::setUp() {
}

void IpIdTest::tearDown() {
}

void IpIdTest::test_enterid() {
  double fStart;
  double fEnd;

  idhdl start;
  idhdl rh;
  std::cout<<"enterid\n";
  printf("Base: %p\n", basePack);

  std::cout<<"enterid\n";
  std::cout<<"enterid\n";
  fStart = Time();
  int nMax = 10000;
  for(int i=0; i<nMax; i++) {
    char s[64];
    snprintf(s, sizeof(s), "dummy%06d", i);
    rh = enterid(s, 0, INT_CMD, &IDROOT, TRUE);
    CPPUNIT_ASSERT_EQUAL(rh, ggetid(s));
  }
  fEnd   = Time();
  printf("Find sizeof enterid Time: for %d : %f\n", nMax, fEnd-fStart);

  fStart = Time();
  for(int i=0; i<nMax; i++) {
    char s[64];
    snprintf(s, sizeof(s), "ZDummy%06d", i);
    rh = enterid(s, 0, INT_CMD, &IDROOT, TRUE);
    CPPUNIT_ASSERT_EQUAL(rh, ggetid(s));
    //snprintf(s, sizeof(s), "dummy%06d", i);
    killid(s, &IDROOT);
    CPPUNIT_ASSERT_EQUAL((idhdl)0, ggetid(s));
  }
  fEnd   = Time();
  printf("Find sizeof enterid and killid Time: for %d : %f\n", nMax, fEnd-fStart);

#if 0
  idhdl h=IDROOT;//_basePack->idroot;
  start=h;
  while (h!=NULL)
  {
    std::cout<<"Name: "<<h->id<<std::endl;

    h = IDNEXT(h);
  }
#endif
}

void IpIdTest::test_killid() {
}

void IpIdTest::test_killhdl() {
}

void IpIdTest::test_ggetid() {
}
