#include "mod2.h"
#include <stdio.h>
#include <string>
#include <kernel/matpol.h>
#include <kernel/intvec.h>
#include <Singular/lists.h>
#include <Fan.h>
#include <Cone.h>

#ifdef HAVE_FANS

Cone* Cone::noFacet()
{
  Cone* c = new Cone();
  c->setDim(1); /* abuse of dim member */
  return c;
}

Cone* Cone::noAdjacency()
{
  Cone* c = new Cone();
  c->setDim(2); /* abuse of dim member */
  return c;
}

bool Cone::isNoFacet ()
{
  /* will return true iff this cone has been created using
     Cone::noFacet() */
  return ((m_facetNs == NULL) && (m_rays == NULL) &&
          (m_dim == 1));
}

bool Cone::isNoAdj()
{
  /* will return true iff this cone has been created using
     Cone::noAdjacency() */
  return ((m_facetNs == NULL) && (m_rays == NULL) &&
          (m_dim == 2));
}

int Cone::checkConeData(const Fan* pFan,
                        const intvec* rays,
                        const intvec* facetNs)
{
  int nMaxRays = 0;
  if (pFan->getMaxRays() != NULL) nMaxRays = pFan->getMaxRays()->cols();
  int nFacetNs = 0;
  if (pFan->getFacetNs() != NULL) nFacetNs = pFan->getFacetNs()->cols();
  int index;                        
  if (rays != NULL)
  {
    for (int i = 0; i < rays->length(); i++)
    {
      index = (int)(long)((*rays)[i]);
      if ((index < 1) || (index > nMaxRays)) return i + 1;
    }
  }
  if (facetNs != NULL)
  {
    for (int i = 0; i < facetNs->length(); i++)
    {
      index = (int)(long)((*facetNs)[i]);
      if ((index < 1) || (index > nFacetNs)) return -i - 1;
    }
  }
  return 0;
}

Cone::Cone(Fan* pFan,
           const intvec* rays,
           const intvec* facetNs)
{
  m_pFan = NULL;
  if (pFan != NULL) m_pFan = pFan;
  m_rays = NULL;
  if (rays != NULL) m_rays = ivCopy(rays);
  m_facetNs = NULL;
  if (facetNs != NULL) m_facetNs = ivCopy(facetNs);
  m_dim = -1;
}

Fan*    Cone::getFan()      const { return m_pFan;     }
intvec* Cone::getRays()     const { return m_rays;     }
intvec* Cone::getFacetNs()  const { return m_facetNs;  }
int     Cone::getDim()      const { return m_dim;      }

Cone::Cone(const Cone& c)
{ /* this is deep copy */
  m_pFan = c.m_pFan;
  m_rays = NULL;
  if (c.getRays() != NULL) m_rays = ivCopy(c.getRays());
  m_facetNs = NULL;
  if (c.getFacetNs() != NULL) m_facetNs = ivCopy(c.getFacetNs());
  m_dim = c.getDim();
}

Cone::Cone()
{
  m_pFan    = NULL;
  m_rays    = NULL;
  m_facetNs = NULL;
  m_dim = -1;
}

Cone::~Cone()
{
  m_pFan = NULL;
  if (m_rays    != NULL) delete m_rays;
  if (m_facetNs != NULL) delete m_facetNs;
}

void Cone::setDim(const int dim)
{
  m_dim = dim;
  if (m_dim < 0) m_dim = -1;
}

char* Cone::toString() const
{
  char h[50];
  std::string s;
  if (m_pFan == NULL)
    s = "//   cone (no properties set)";
  else
  {
    s = "//   cone:\n";
    int i = 0; if (m_rays != NULL) i = m_rays->length();
    sprintf(h, "//      %d rays\n", i); s += h;
    i = 0; if (m_facetNs != NULL) i = m_facetNs->length();
    sprintf(h, "//      %d facet normals\n", i); s += h;
    s += "//      dimension: " ;
    if (m_dim == -1) s += "?";
    else { sprintf(h, "%d", m_dim); s += h; }
  }
  return omStrDup(s.c_str());
}

void Cone::print() const
{
  char* s = this->toString();
  printf("%s", s);
  omFree(s);
}

#endif
/* HAVE_FANS */
