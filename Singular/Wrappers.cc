#include "mod2.h"

#ifdef HAVE_WRAPPERS

#include <iostream>
#include "RingWrapper.h"
#include "PolyWrapper.h"
#include "Wrappers.h"
#include "febase.h"

void test1 ();
void test2 ();
void test3 ();

PrettyPrinter prpr("", "", false, false, -1, "   ");

void testWrappers (bool detailedOutput)
{
  prpr.setConsole(detailedOutput ? 0 : -1);
  prpr+1;

  test1();
  test2();
  test3();

  prpr-1;
  PrintLn();
  PrintLn();
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
  PrintS("Test3: Addition of instances of PolyWrapper");
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
  p3 += p2;  // this should change p3 but NOT p2!
  PrintLn(); PrintS("p3 = "); p3.print();
  PrintLn(); PrintS("p2 = "); p2.print();
}

#endif // HAVE_WRAPPERS