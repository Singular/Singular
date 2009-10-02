#include "mod2.h"

#ifdef HAVE_WRAPPERS

#include "PolyWrapper.h"
#include "CanonicalPoly.h"
#include "Wrappers.h"
#include <iostream>
#include "febase.h"

PolyWrapper PolyWrapper::operator+ (const PolyWrapper& p) const
{
  +prpr > "PolyWrapper::operator+ called";
  prpr+1;
  +prpr > "first argument = " < this->toString();
  +prpr > "second argument = " < p.toString();
  prpr-1;
  PolyWrapper q(this->getInternPoly()->deepCopy());  // deep copy of given PolyWrapper
  q.getInternPoly()->add(p.getInternPoly());
  return q;
}

PolyWrapper& PolyWrapper::operator+= (const PolyWrapper& p)
{
  +prpr > "PolyWrapper::operator+= called";
  prpr+1;
  +prpr > "first argument = " < this->toString();
  +prpr > "second argument = " < p.toString();
  prpr-1;
  if (this->getInternPoly()->isShared())
  {
    m_pInternPoly->decrement();
    m_pInternPoly = m_pInternPoly->deepCopy();
  }
  this->getInternPoly()->add(p.getInternPoly());
  return *this;
}

const RingWrapper& PolyWrapper::getRing () const
{
  return m_pInternPoly->getRing();
}

PolyWrapper::PolyWrapper (): m_pInternPoly(0)
{
  assume(false); // the default constructor, i.e. the one
                 // without arguments should never be called
}

PolyWrapper::PolyWrapper (InternPoly* ip): m_pInternPoly(ip)
{
  +prpr > "PolyWrapper constructor with argument InternPoly*";
}

PolyWrapper::~PolyWrapper ()
{
  +prpr > "PolyWrapper destructor, object = " < this->toString();
  InternPoly* p_ip = this->getInternPoly();
  p_ip->decrement();
  if (p_ip->getCounter() == 0) delete p_ip;
}

PolyWrapper::PolyWrapper (const PolyWrapper& p)
{
  +prpr > "creating a shallow copy of PolyWrapper, argument = " < p.toString();
  m_pInternPoly = p.getInternPoly();
  m_pInternPoly->increment();
}

PolyWrapper& PolyWrapper::operator= (const PolyWrapper& p)
{
  +prpr > "assignment operator of PolyWrapper, argument = " < p.toString();
  prpr+1;
  +prpr > "working with PolyWrapper " < this->toString();
  prpr+1;
  m_pInternPoly->decrement();
  if (m_pInternPoly->getCounter() == 0) delete m_pInternPoly;
  m_pInternPoly = p.getInternPoly();
  prpr-1;
  +prpr > "working with PolyWrapper " < this->toString();
  prpr+1;
  m_pInternPoly->increment();
  prpr-2;
  +prpr > "assignment of PolyWrapper completed";
  return *this;
}

void PolyWrapper::print () const
{
  PrintS(this->toString());
}

void PolyWrapper::printLn () const
{
  PrintS(this->toString());
  PrintLn();
}

char* PolyWrapper::toString () const
{
  InternPoly* pip = this->getInternPoly();
  return pip->toString();
}

InternPoly* PolyWrapper::getInternPoly () const
{
  return m_pInternPoly;
}

PolyWrapper::PolyWrapper (const int i, const RingWrapper& r) {
  +prpr > "creating a new PolyWrapper (internal type: CanonicalPoly)";
  m_pInternPoly = new CanonicalPoly(i, r);
}

#endif // HAVE_WRAPPERS