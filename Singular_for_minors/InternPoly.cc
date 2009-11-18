#include "mod2.h"

#ifdef HAVE_WRAPPERS

#include <iostream>
#include "Wrappers.h"
#include "InternPoly.h"

InternPoly::InternPoly (const RingWrapper& r):m_ring(r)
{
  +prpr > "InternPoly default constructor";
  increment();
}

InternPoly::InternPoly (const InternPoly& ip):m_ring(ip.getRing())
{
  assume(false); /* shallow copy constructor should never be called */
}

InternPoly* InternPoly::deepCopy () const
{
  assume(false); /* deep copy constructor should never be called */           
}

InternPoly::~InternPoly ()
{
  +prpr > "InternPoly destructor";
}

char* InternPoly::toString () const
{
  assume(false); /* should be overridden by each derived class */
}

const RingWrapper& InternPoly::getRing () const
{
  return m_ring;
}

void InternPoly::add (const InternPoly* ip) {
  if (this->getRing().isCompatible(ip->getRing()))
    this->addCompatible(ip);
  else
  {
    +prpr > "addition of objects in incompatible rings encountered";
    assume(false);
  }
}

int InternPoly::getPolyType () const
{
  return UNSPECIFIED_POLY_TYPE;
}

void InternPoly::addCompatible (const InternPoly* ip)
{
  assume(false); /* should be overridden by each derived class */
}

#endif
/* HAVE_WRAPPERS */