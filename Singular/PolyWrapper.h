#ifndef POLY_WRAPPER_H
#define POLY_WRAPPER_H

#ifdef HAVE_WRAPPERS

#include "InternPoly.h"
#include "RingWrapper.h"
#include "Wrappers.h"

class PolyWrapper
{
private:
  InternPoly* m_pInternPoly;
  PolyWrapper (InternPoly* ip);
  InternPoly* getInternPoly () const;
public:
  PolyWrapper ();
  PolyWrapper (const PolyWrapper& p);                // shallow copy
  PolyWrapper (const int i, const RingWrapper& r);   // integer i as a poly
  ~PolyWrapper ();
  void print () const;
  void printLn () const;
  char* toString () const;
  PolyWrapper& operator= (const PolyWrapper& p);
  PolyWrapper operator+ (const PolyWrapper& p) const;
  PolyWrapper& operator+= (const PolyWrapper& p);
  const RingWrapper& getRing () const;
};

#endif // HAVE_WRAPPERS

#endif
/* POLY_WRAPPER_H */
