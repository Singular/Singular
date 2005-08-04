#include "Poly.h"

void testPoly(){
  Poly p(1,currRing);
  Poly p2(2,currRing);
  p.print();

  PrintS("\n");
  p2.print();

  PrintS("\n");
  (p+p2).print();
  p2.print();
  PrintS("\n");
  (p+=p).print();
  PrintS("\n");
  p2.print();
}
