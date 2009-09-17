#ifndef CANONICAL_POLY_H
#define CANONICAL_POLY_H

#include "ring.h"
#include "InternPoly.h"
#include <string>

class CanonicalPoly : public InternPoly   // class for wrapping canonical SINGULAR polynomials
{
private:
  poly m_Poly;
  ring m_r;
public:
  CanonicalPoly() { assume(false); /* default constructor without arguments should never be called */ }
  CanonicalPoly(const CanonicalPoly& p) { assume(false); /* copy constructor should never be called */ }
  ~CanonicalPoly();
  CanonicalPoly(const int i, const ring r);
  virtual void print() const;
};

#endif
/* CANONICAL_POLY_H */
