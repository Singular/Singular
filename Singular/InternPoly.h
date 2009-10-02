#ifndef INTERN_POLY_H
#define INTERN_POLY_H

#ifdef HAVE_WRAPPERS

#include "ReferenceCounter.h"
#include "RingWrapper.h"

class InternPoly : public ReferenceCounter
{
protected:
  const RingWrapper& m_ring;
public:
  InternPoly (const RingWrapper& r);
  InternPoly (const InternPoly& p);
  virtual ~InternPoly ();
  const RingWrapper& getRing () const;
  void add (const InternPoly* ip);  // changes given object
  virtual void addCompatible (const InternPoly* ip);  // changes given object
  virtual InternPoly* deepCopy () const;
  virtual int getPolyType () const;
  virtual char* toString () const;
};

#endif // HAVE_WRAPPERS

#endif
/* INTERN_POLY_H */
