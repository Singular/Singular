#include "mod2.h"

#ifdef HAVE_WRAPPERS

#include <iostream>
#include "febase.h"
#include "Wrappers.h"
#include "RingWrapper.h"
#include "PolyWrapper.h"

void test0 ();
void test1 ();
void test2 ();
void test3 ();
void test4 ();

PrettyPrinter prpr("", "", false, false, -1, "   ");

void wrapSINGULARPolys (const SingularPoly& sp1, const SingularPoly& sp2)
{
  prpr.setConsole(-1);
  PrintLn();
  RingWrapper r; /* wraps the current ring */
  PolyWrapper p1(sp1, r);
  PolyWrapper p2(sp2, r);
  PrintS("This is how the created instances of PolyWrapper look like:");
  PrintLn(); PrintS("p1 = "); p1.print();
  PrintLn(); PrintS("p2 = "); p2.print();
  PrintLn(); PrintS("And here comes their sum:");
  PolyWrapper q = p1 + p2;
  PrintLn(); PrintS("p1 + p2 = "); q.print();
  PrintLn(); PrintLn();
}

void testWrappers (bool detailedOutput)
{
  prpr.setConsole(detailedOutput ? 0 : -1);
  prpr+1;

  test0();
  test1();
  test2();
  test3();
  test4();

  prpr-1;
  PrintLn();
  PrintLn();
}

void test0 ()
{
  if (currRing != NULL)
  {
    PrintLn();
    PrintS("Test0: Creation of RingWrapper from current SINGULAR ring");
  
    RingWrapper r;
    PrintLn(); PrintS("r = "); r.print();
  }
}

void test1 ()
{
  PrintLn();
  PrintS("Test1: Creation and destruction of instances of RingWrapper");

  const char* theVariables1[2] = {"x", "y"};
  RingWrapper r1("myRing1", 7, 2, theVariables1, "dp");
  PrintLn(); PrintS("r1 = "); r1.print();
  
  const char* theVariables2[3] = {"u", "v", "w"};
  RingWrapper r2("myRing2", 0, 3, theVariables2, "Ds");
  PrintLn(); PrintS("r2 = "); r2.print();
}

void test2 ()
{
  PrintLn();
  PrintS("Test2: Creation, copying, assignment, and destruction of instances of PolyWrapper");
  
  const char* theVariables[2] = {"x", "y"};
  RingWrapper r("myRing", 0, 2, theVariables, "lp");
  PrintLn(); PrintS("r = "); r.print();
  
  PolyWrapper p1(4, r);
  PrintLn(); PrintS("p1 = "); p1.print();
  PolyWrapper p2(p1);
  PrintLn(); PrintS("p2 = "); p2.print();
  PolyWrapper p3(-37, r);
  PrintLn(); PrintS("p3 = "); p3.print();
  PolyWrapper p4(p3);
  PrintLn(); PrintS("p4 = "); p4.print();
  PolyWrapper p5 = p2;
  PrintLn(); PrintS("p5 = "); p5.print();
}

void test3 ()
{
  PrintLn();
  PrintS("Test3: Addition of compatible instances of PolyWrapper");
  
  const char* theVariables[2] = {"x", "y"};
  RingWrapper r("myRing", 0, 2, theVariables, "Ds");
  PrintLn(); PrintS("r = "); r.print();
  
  PolyWrapper p1(4, r);
  PrintLn(); PrintS("p1 = "); p1.print();
  PolyWrapper p2(7, r);
  PrintLn(); PrintS("p2 = "); p2.print();
  PolyWrapper p3 = p1 + p2;
  PrintLn(); PrintS("p3 = "); p3.print();
  p3 = p1;
  PrintLn(); PrintS("p3 = "); p3.print();
  p3 += p2;  /* this should change p3 but NOT p2! */
  PrintLn(); PrintS("p3 = "); p3.print();
  PrintLn(); PrintS("p2 = "); p2.print();
}

void test4 ()
{
  PrintLn();
  PrintS("Test4: Addition of incompatible instances of PolyWrapper");
  
  const char* theVariables1[2] = {"x", "y"};
  RingWrapper r1("myRing", 0, 2, theVariables1, "dp");
  PrintLn(); PrintS("r1 = "); r1.print();

  const char* theVariables2[2] = {"x", "z"};
  RingWrapper r2("myRing", 0, 2, theVariables2, "dp");
  PrintLn(); PrintS("r2 = "); r2.print();

  PolyWrapper p1(4, r1);
  PrintLn(); PrintS("p1 = "); p1.print();
  PolyWrapper p2(7, r2);
  PrintLn(); PrintS("p2 = "); p2.print();
  PolyWrapper p3 = p1 + p2; /* this will not work */
  /* The last instruction will abnormally stop the system call,
     due to an "assume(false)". Use detailed output to see more
     details. */
}

#endif
/* HAVE_WRAPPERS */