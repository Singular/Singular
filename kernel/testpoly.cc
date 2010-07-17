#include <kernel/Poly.h>

void testPoly(){
  Poly p(1,currRing);
  Poly p2(2,currRing);
  p.print();
  p2.print();
  (p+p2).print();
  p2.print();
  (p+=p).print();
  p2.print();
  std::vector<int> exp(3);
  exp[0]=1;
  Poly p3(exp,currRing);
  exp[1]=2;
  Poly p4(exp,currRing);
  Poly p5=p;
  Poly p6=(p+p2+p3+p4+p5);
  PolyInputIterator<Poly> it=p6.begin();
  p6.print();
  while(it!=p6.end()){

    it->print();

    it++;
  }

  p4*=p4;
  p4.print();
  p2=p*p4;
  p2.print();
  p.print();
  p4.print();
  
}
