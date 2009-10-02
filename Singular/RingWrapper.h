#ifndef RING_WRAPPER_H
#define RING_WRAPPER_H

#ifdef HAVE_WRAPPERS

#include "ring.h"
#include "Wrappers.h"

class RingWrapper
{
private:
  SingularRing m_singularRing;
public:
  RingWrapper (const char* ringName, const int characteristic,
               const int varNumber, const char** varNames, const char* ringOrder);
  RingWrapper ();
  ~RingWrapper ();
  void print () const;
  void printLn () const;
  char* toString () const;
  bool isCompatible (const RingWrapper& r) const;
  const SingularRing& getSingularRing () const;
};

#endif // HAVE_WRAPPERS

#endif
/* RING_WRAPPER_H */