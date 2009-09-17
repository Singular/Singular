#ifndef STRING_POLY_H
#define STRING_POLY_H

#include "InternPoly.h"
#include <string>

class StringPoly : public InternPoly   // just a test class for polynomials as strings
{
private:
  std::string m_s;
public:
  StringPoly(): m_s("") { assume(false); /* default constructor without arguments should never be called */ }
  StringPoly(const StringPoly& p) { assume(false); /* copy constructor should never be called */ }
  ~StringPoly() { std::cout << "\n      StringPoly destructor for '" << m_s << "'"; m_s = ""; }
  StringPoly(const std::string& polyString): m_s(polyString) { std::cout << "\n      StringPoly constructor with argument '" << polyString << "'"; }
  virtual void print() const { std::cout << m_s; }
};

#endif
/* STRING_POLY_H */
