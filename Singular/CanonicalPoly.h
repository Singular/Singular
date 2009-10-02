#ifndef CANONICAL_POLY_H
#define CANONICAL_POLY_H

#ifdef HAVE_WRAPPERS

#include "ring.h"
#include "InternPoly.h"
#include <string>
#include "Wrappers.h"

class CanonicalPoly : public InternPoly   // class for wrapping canonical SINGULAR polynomials
{
private:
  SingularPoly m_poly;
  const SingularPoly& getSingularPoly () const;
public:
  ~CanonicalPoly ();
  CanonicalPoly (const int i, const RingWrapper& r);
  CanonicalPoly (const SingularPoly&, const RingWrapper& r);
  void addCompatible (const InternPoly* ip);  // changes m_poly, i.e. given object
  CanonicalPoly* deepCopy () const;
  int getPolyType () const;
  char* toString () const;
};

#endif // HAVE_WRAPPERS

#endif
/* CANONICAL_POLY_H */
