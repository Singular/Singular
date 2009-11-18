#ifndef POLY_H
#define POLY_H

#include "InternPoly.h"

class Poly
{
private:
  InternPoly* m_pInternPoly;
public:
  Poly(): m_pInternPoly(0) { assume(false); /* default constructor without arguments should never be called */ }
  Poly(const Poly& p); // shallow copy
  ~Poly();
  Poly(InternPoly* ip): m_pInternPoly(ip) { std::cout << "\n      Poly constructor with argument InternPoly*"; }
  void print() const { m_pInternPoly->print(); }
  InternPoly* getInternPoly() const { return m_pInternPoly; }
  Poly operator+  (const Poly& p) const;
  Poly operator+= (const Poly& p);
};

#endif
/* POLY_H */
