#include "mod2.h"

#ifdef HAVE_WRAPPERS

#include <iostream>
#include "structs.h"
#include "polys.h"
#include "Wrappers.h"
#include "CanonicalPoly.h"

CanonicalPoly::CanonicalPoly (const SingularPoly& sp, const RingWrapper& r):InternPoly(r)
{
  +prpr > "CanonicalPoly constructor with poly argument = " <
        p_String(sp, r.getSingularRing(), r.getSingularRing());
  m_poly = sp;
}

CanonicalPoly::CanonicalPoly (const int i, const RingWrapper& r):InternPoly(r)
{
  +prpr > "CanonicalPoly constructor with int argument = " < i;
  m_poly = p_ISet(i, r.getSingularRing());
}

CanonicalPoly::~CanonicalPoly ()
{
  +prpr > "CanonicalPoly destructor, object = " < this->toString();
}

char* CanonicalPoly::toString () const
{
  SingularRing r = this->getRing().getSingularRing();
  return p_String(this->getSingularPoly(), r, r);
}

void CanonicalPoly::addCompatible (const InternPoly* ip)
{
  if (ip->getPolyType() == CANONICAL_POLY_TYPE)
  {
    const CanonicalPoly* pcp = static_cast<const CanonicalPoly*>(ip);
    +prpr > "value of CanonicalPoly is being changed";
    prpr+1;
    +prpr > "old value = " < this->toString();
    +prpr > "value to be added = " < pcp->toString();
    m_poly = p_Add_q(m_poly, pcp->deepCopy()->getSingularPoly(), this->getRing().getSingularRing());
    +prpr > "new value = " < this->toString();
    prpr-1;
    +prpr > "value of CanonicalPoly has been changed";
  }
  else
    assume(false);
}

int CanonicalPoly::getPolyType () const
{
  return CANONICAL_POLY_TYPE;
}

const SingularPoly& CanonicalPoly::getSingularPoly () const
{
  return m_poly;
}

CanonicalPoly* CanonicalPoly::deepCopy () const
{
  +prpr > "creating a deep copy of CanonicalPoly, argument = " < this->toString();
  SingularPoly sp = p_Copy(this->getSingularPoly(), this->getRing().getSingularRing()); /* SINGULAR poly is deeply copied. */
  CanonicalPoly* pcp = new CanonicalPoly(sp, this->getRing()); /* ring is not deeply copied! */
  return pcp;
}

#endif
/* HAVE_WRAPPERS */