#include "mod2.h"
#include "Poly.h"

using namespace std;

Poly::~Poly()
{
  std::cout << "\n      Poly destructor for '";
  this->print();
  std::cout << "'";
  InternPoly* p_ip = this->getInternPoly();
  p_ip->decrement();
  if (p_ip->getCounter() == 0) delete p_ip;
}

Poly::Poly(const Poly& p)
{
  std::cout << "\n      creating a shallow copy of Poly '";
  p.print();
  std::cout << "'";
  InternPoly* p_ip = p.getInternPoly();
  p_ip->increment();
  m_pInternPoly = p_ip;
}
