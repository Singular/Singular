#include "mod2.h"
#include <iostream>
#include <fstream>
#include "Poly.h"
#include "ObjectFactory.h"
#include "Wrappers.h"

using namespace std;

void testWrappers ()
{
  cout << "calling test1";
  test1();
  cout << "\n\n";
  cout << "calling test2";
  test2();
}

void test1()
{
  cout << "\n   Test1: Creation, copying and destruction of instances of StringPoly";

  Poly p1 = ObjectFactory::newStringPoly("x + 1");
  Poly p2(p1);
  Poly p3(p2);
  Poly p4(p1);
  p1.print();
  p2.print();
  p3.print();
  p4.print();
}

void test2()
{
  cout << "\n   Test2: Creation, copying and destruction of instances of CanonicalPoly";

  const char* theVariables[2] = {"x", "y"};
  ring r = ObjectFactory::newRing("myRing", 7, 2, theVariables, "lp");
  Poly p1 = ObjectFactory::newCanonicalPoly(17, r);
  Poly p2(p1);
  Poly p3(p2);
  Poly p4(p1);
  p1.print();
  p2.print();
  p3.print();
  p4.print();
}

