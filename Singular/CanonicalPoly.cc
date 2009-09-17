#include "mod2.h"
#include "structs.h"
#include "polys.h"
#include "CanonicalPoly.h"
#include <string>

using namespace std;

CanonicalPoly::CanonicalPoly(const int i, const ring r)
{
  cout << "\n      CanonicalPoly constructor with argument '" << i << "'";
  m_Poly = p_ISet(i, r);
  m_r = r;
}

CanonicalPoly::~CanonicalPoly()
{
  std::cout << "\n      CanonicalPoly destructor for '";
  this->print();
  std::cout << "' THIS STILL MISSES THE DESTRUCTION OF THE WRAPPED SINGULAR POLY!";
}

void CanonicalPoly::print() const
{
  p_Write0(m_Poly, m_r, m_r);
}
