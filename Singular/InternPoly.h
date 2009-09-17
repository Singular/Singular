#ifndef INTERN_POLY_H
#define INTERN_POLY_H

#include "ReferenceCounter.h"

class InternPoly : public ReferenceCounter
{
public:
  InternPoly() { std::cout << "\n      InternPoly default constructor"; increment(); }
  InternPoly(const InternPoly& p) { assume(false); /* copy constructor should never be called */ }
  virtual ~InternPoly() { std::cout << "\n      InternPoly destructor"; }
  virtual void print() const { std::cout << "not yet implemented"; }
};

#endif
/* INTERN_POLY_H */
