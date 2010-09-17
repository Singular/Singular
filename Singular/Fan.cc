#include "mod2.h"
#include <stdio.h>
#include <string>
#include <kernel/matpol.h>
#include <kernel/intvec.h>
#include <Singular/lists.h>
#include <Cone.h>
#include <Fan.h>

#ifdef HAVE_FANS

Fan::Fan(const intvec* maxRays,
         const intvec* facetNs,
         const intvec* linSpace)
{
  m_maxRays = NULL;
  if (maxRays != NULL) m_maxRays = ivCopy(maxRays);
  m_facetNs = NULL;
  if (facetNs != NULL) m_facetNs = ivCopy(facetNs);
  m_linSpace = NULL;
  if (linSpace != NULL) m_linSpace = ivCopy(linSpace);
  else
  {
    /* trivial lineality space: we use a matrix with a single zero column */
    if (m_maxRays != NULL) m_linSpace = new intvec(m_maxRays->rows(), 1, 0);
    else                   m_linSpace = new intvec(m_facetNs->rows(), 1, 0);
  }
  m_maxCones = (lists)omAllocBin(slists_bin);
  m_maxCones->Init(0);
  m_dim = -1;
  m_complete = -1;
  m_simplicial = -1;
  m_pure = -1;
  m_adjacency = (lists)omAllocBin(slists_bin);
  m_adjacency->Init(0);
}

intvec* Fan::getMaxRays()    const { return m_maxRays;    }
intvec* Fan::getFacetNs()    const { return m_facetNs;    }
intvec* Fan::getLinSpace()   const { return m_linSpace;   }
lists   Fan::getMaxCones()   const { return m_maxCones;   }
int     Fan::getDim()        const { return m_dim;        }
int     Fan::getComplete()   const { return m_complete;   }
int     Fan::getSimplicial() const { return m_simplicial; }
int     Fan::getPure()       const { return m_pure;       }
lists   Fan::getRawAdj()     const { return m_adjacency;  }

void Fan::setDim (const int dim)
{
  m_dim = dim;
  if (m_dim < 0) m_dim = -1;
}

void Fan::setComplete (const int complete)
{
  m_complete = complete;
  if ((m_complete != 0) && (m_complete != 1)) m_complete = -1;
}

void Fan::setSimplicial (const int simplicial)
{
  m_simplicial = simplicial;
  if ((m_simplicial != 0) && (m_simplicial != 1)) m_simplicial = -1;
}

void Fan::setPure (const int pure)
{
  m_pure = pure;
  if ((m_pure != 0) && (m_pure != 1)) m_pure = -1;
}

int Fan::getAmbientDim() const
{
  if (m_linSpace == NULL) return -1;
  else return m_linSpace->rows();
}

Fan::Fan(const Fan& f)
{ /* this is deep copy */
  m_maxRays = NULL;
  if (f.getMaxRays() != NULL) m_maxRays = ivCopy(f.getMaxRays());
  m_facetNs = NULL;
  if (f.getFacetNs() != NULL) m_facetNs = ivCopy(f.getFacetNs());
  m_linSpace = ivCopy(f.getLinSpace());
  m_maxCones = lCopy(f.getMaxCones());
  m_dim = f.getDim();
  m_complete = f.getComplete();
  m_simplicial = f.getSimplicial();
  m_pure = f.getPure();
  m_adjacency = lCopy(f.getRawAdj());
}

Fan::Fan()
{
  m_maxRays  = NULL;
  m_facetNs  = NULL;
  m_linSpace = NULL;
  m_maxCones = (lists)omAllocBin(slists_bin);
  m_maxCones->Init(0);
  m_dim = -1;
  m_complete = -1;
  m_simplicial = -1;
  m_pure = -1;
  m_adjacency = (lists)omAllocBin(slists_bin);
  m_adjacency->Init(0);
}

Fan::~Fan()
{
  if (m_maxRays  != NULL) delete m_maxRays;
  if (m_facetNs  != NULL) delete m_facetNs;
  if (m_linSpace != NULL) delete m_linSpace;
  if (m_maxCones->nr + 1 > 0)
    omFreeSize((ADDRESS)m_maxCones->m,
               (m_maxCones->nr + 1)*sizeof(sleftv));
  omFreeBin((ADDRESS)m_maxCones, slists_bin);
  if (m_adjacency->nr + 1 > 0)
    omFreeSize((ADDRESS)m_adjacency->m,
               (m_adjacency->nr + 1)*sizeof(sleftv));
  omFreeBin((ADDRESS)m_adjacency, slists_bin);
}

char* Fan::toString() const
{
  char h[50];
  std::string s;
  if (m_linSpace == NULL)
    s = "//   fan (no properties set)";
  else
  {
    s = "//   fan:\n";
    int i = getAmbientDim();
    sprintf(h, "//      ambient dimension: %d\n", i); s += h;
    i = 0; if (m_maxRays != NULL) i = m_maxRays->cols();
    sprintf(h, "//      %d maximal rays\n", i); s += h;
    i = 0; if (m_facetNs != NULL) i = m_facetNs->cols();
    sprintf(h, "//      %d facet normals\n", i); s += h;
    i = m_maxCones->nr + 1;
    sprintf(h, "//      %d maximal cones\n", i); s += h;
    s += "//      dimension: " ;
    if (m_dim == -1) s += "?\n";
    else { sprintf(h, "%d\n", m_dim); s += h; }
    s += "//      complete: " ;
    if (m_complete == -1)     s += "?\n";
    else if (m_complete == 0) s += "no\n";
    else                      s += "yes\n";
    s += "//      simplicial: " ;
    if (m_simplicial == -1)     s += "?\n";
    else if (m_simplicial == 0) s += "no\n";
    else                        s += "yes\n";
    s += "//      pure: " ;
    if (m_pure == -1)     s += "?";
    else if (m_pure == 0) s += "no";
    else                  s += "yes";
  }
  return omStrDup(s.c_str()); /* any caller must clean up this string */
}

void Fan::print() const
{
  char* s = this->toString();
  printf("%s", s);
  omFree(s);
}

bool Fan::addMaxCone(const Cone* pCone)
{
  /* the given cone must be based on this fan, i.e. refer to
     the same sets of maximal rays and facet normals, etc. */
  if (pCone->getFan() != this)
    return false;

  /* append the adjacency matrix by one more row and column */
  insertCrosses(1);
  /* in order to use lInsert0 from lists.h/.cc, we pack the cone
     in a list with one element */
  lists L = (lists)omAllocBin(slists_bin);
  L->Init(1); L->m[0].rtyp = CONE_CMD; L->m[0].data = (void*)pCone;
  /* lInsert0 will create and insert A COPY of its 2nd argument */
  m_maxCones = lInsert0(m_maxCones, &(L->m[0]), m_maxCones->nr + 1);
  omFreeSize((ADDRESS)L->m, sizeof(sleftv));
  omFreeBin((ADDRESS)L, slists_bin);
  return true;
}

int Fan::addMaxCones(const lists L)
{
  /* the given cones must be based on this fan, i.e. refer to
     the same sets of maximal rays and facet normals, etc. */
  for (int i = 0; i < L->nr + 1; i++)
    if (((Cone*)(L->m[i].data))->getFan() != this) return i + 1;

  /* append the adjacency matrix by sufficiently many new rows and col.'s */
  insertCrosses(L->nr + 1);
  lists M = lCopy(m_maxCones);
  if (m_maxCones->nr + 1 > 0)
    omFreeSize((ADDRESS)m_maxCones->m,
               (m_maxCones->nr + 1)*sizeof(sleftv));
  omFreeBin((ADDRESS)m_maxCones, slists_bin);
  m_maxCones = (lists)omAllocBin(slists_bin);
  m_maxCones->Init(M->nr + L->nr + 2);
  for (int i = 0; i < M->nr + 1; i++)
  {
    m_maxCones->m[i].rtyp = CONE_CMD;
    m_maxCones->m[i].data = new Cone(*(Cone*)(M->m[i].data));
  }
  for (int i = 0; i < L->nr + 1; i++)
  {
    m_maxCones->m[M->nr + 1 + i].rtyp = CONE_CMD;
    m_maxCones->m[M->nr + 1 + i].data = new Cone(*(Cone*)(L->m[i].data));
  }
  if (M->nr + 1 > 0)
    omFreeSize((ADDRESS)M->m, (M->nr + 1)*sizeof(sleftv));
  omFreeBin((ADDRESS)M, slists_bin);
  return 0;
}

bool Fan::contains(const intvec* iv, const int i)
{
  for (int j = 0; j < iv->length(); j++)
  {
    if ((*iv)[j] == i) return true;
  }
  return false;
}

bool Fan::deleteMaxCone(const int index)
{
  /* we want to call deleteMaxCones, thus pack the index in an intvec */
  intvec* iv = new intvec[1]; (*iv)[0] = index;
  bool result = (deleteMaxCones(iv) == 0);
  delete iv;
  return result;
}

int Fan::deleteMaxCones(const intvec* indices)
{
  if (indices == NULL)
  {
    /* delete all maximal cones from this fan */
    if (m_maxCones->nr + 1 > 0)
      omFreeSize((ADDRESS)m_maxCones->m,
                 (m_maxCones->nr + 1)*sizeof(sleftv));
    omFreeBin((ADDRESS)m_maxCones, slists_bin);
    m_maxCones = (lists)omAllocBin(slists_bin);
    m_maxCones->Init(0);
    /* delete adjacency information */
    if (m_adjacency->nr + 1 > 0)
      omFreeSize((ADDRESS)m_adjacency->m,
                 (m_adjacency->nr + 1)*sizeof(sleftv));
    omFreeBin((ADDRESS)m_adjacency, slists_bin);
    m_adjacency = (lists)omAllocBin(slists_bin);
    m_adjacency->Init(0);
    return 0;
  }
  else
  {
    int index;
    for (int i = 0; i < indices->length(); i++)
    {
      index = (*indices)[i];
      if ((index < 1) || (index > m_maxCones->nr + 1))
        return i + 1;
    }
    /* clean up the adjacency matrix */
    deleteCrosses(indices);
    lists M = lCopy(m_maxCones);
    if (m_maxCones->nr + 1 > 0)
      omFreeSize((ADDRESS)m_maxCones->m,
                 (m_maxCones->nr + 1)*sizeof(sleftv));
    omFreeBin((ADDRESS)m_maxCones, slists_bin);
    m_maxCones = (lists)omAllocBin(slists_bin);
    m_maxCones->Init(M->nr + 1 - indices->length()); int j = 0;
    for (int i = 0; i < M->nr + 1; i++)
    {
      if (!contains(indices, i + 1))
      {
        m_maxCones->m[j].rtyp = CONE_CMD;
        m_maxCones->m[j].data = new Cone(*(Cone*)(M->m[i].data));
        j++;
      }
    }
    if (M->nr + 1 > 0)
      omFreeSize((ADDRESS)M->m, (M->nr + 1)*sizeof(sleftv));
    omFreeBin((ADDRESS)M, slists_bin);
    return 0;
  }
}

void Fan::insertCrosses(int s)
{
  int k = m_maxCones->nr + 1;
  lists L = (lists)omAllocBin(slists_bin);
  L->Init((k + s) * (k + s)); int j = 0;
  for (int i = 0; i < k * k; i++)
  {
    L->m[j  ].rtyp = CONE_CMD;
    L->m[j++].data = new Cone(*(Cone*)(m_adjacency->m[i].data));
    if ((i % k) == k - 1)
    {
      for (int q = 0; q < s; q++)
      {
        L->m[j  ].rtyp = CONE_CMD;
        L->m[j++].data = Cone::noAdjacency();
      }
    }
  }
  for (int q = k * (k + s); q < (k + s) * (k + s); q++)
  {
    L->m[j  ].rtyp = CONE_CMD;
    L->m[j++].data = Cone::noAdjacency();
  }
  if (m_adjacency->nr + 1 > 0)
    omFreeSize((ADDRESS)m_adjacency->m,
               (m_adjacency->nr + 1)*sizeof(sleftv));
  omFreeBin((ADDRESS)m_adjacency, slists_bin);
  m_adjacency = lCopy(L);
  if (L->nr + 1 > 0)
    omFreeSize((ADDRESS)L->m,
               (L->nr + 1)*sizeof(sleftv));
  omFreeBin((ADDRESS)L, slists_bin);
}

void Fan::deleteCrosses(const intvec* indices)
{
  int k = m_maxCones->nr + 1;
  int d = indices->length();
  lists L = (lists)omAllocBin(slists_bin);
  L->Init((k - d) * (k - d));
  int j = 0;
  for (int i = 0; i < k * k; i++)
  {
    while (contains(indices, (i / k) + 1) || contains(indices, (i % k) + 1))
      i++; /* skip this entry: it's either on a row or on a column
              (or on both) with forbidden index */
    if (i < k * k)
    {
      L->m[j  ].rtyp = CONE_CMD;
      L->m[j++].data = new Cone(*(Cone*)(m_adjacency->m[i].data));
    }  
  }
  if (m_adjacency->nr + 1 > 0)
    omFreeSize((ADDRESS)m_adjacency->m,
               (m_adjacency->nr + 1)*sizeof(sleftv));
  omFreeBin((ADDRESS)m_adjacency, slists_bin);
  m_adjacency = lCopy(L);
  if (L->nr + 1 > 0)
    omFreeSize((ADDRESS)L->m,
               (L->nr + 1)*sizeof(sleftv));
  omFreeBin((ADDRESS)L, slists_bin);
}

Cone* Fan::getAdjacencyFacet(int maxCone1, int maxCone2) const
{
  int k = getMaxCones()->nr + 1;
  Cone* c = (Cone*)m_adjacency->m[k * (maxCone1 - 1) + maxCone2 - 1].data;
  return new Cone(*c);
}

intvec* Fan::getAdjacency() const
{
  int k = getMaxCones()->nr + 1;
  intvec* m = new intvec(k, k, 2);
  Cone* facet;
  for (int r = 1; r <= k; r++)
    for (int c = 1; c <= k; c++)
    {
      facet = (Cone*)m_adjacency->m[k * (r - 1) + c - 1].data;
      if      (facet->isNoAdj())   IMATELEM(*m, r, c) =  0;
      else if (facet->isNoFacet()) IMATELEM(*m, r, c) = -1;
      else                         IMATELEM(*m, r, c) =  1;
    }
  return m;
}

void Fan::addAdjacency(const int i, const int j, const Cone* c)
{
  int k = getMaxCones()->nr + 1;
  Cone* d = (Cone*)m_adjacency->m[k * (i - 1) + j - 1].data;
  /* delete old cone */
  delete d;
  d = (Cone*)m_adjacency->m[k * (j - 1) + i - 1].data;
  /* delete old cone */
  delete d;
  if (c == NULL)
  { /* symmetric fill */
    m_adjacency->m[k * (i - 1) + j - 1].data = Cone::noFacet();
    m_adjacency->m[k * (j - 1) + i - 1].data = Cone::noFacet();
  }
  else
  { /* symmetric fill */
    m_adjacency->m[k * (i - 1) + j - 1].data = new Cone(*c);
    m_adjacency->m[k * (j - 1) + i - 1].data = new Cone(*c);
  }
}

void Fan::addAdjacencies(const intvec* iv, const intvec* jv, const lists L)
{
  int k = getMaxCones()->nr + 1;
  Cone* d;
  for (int q = 0; q < iv->length(); q++)
  {
    d = (Cone*)m_adjacency->m[k * ((*iv)[q] - 1) + (*jv)[q] - 1].data;
    /* delete old cone */
    delete d;
    d = (Cone*)m_adjacency->m[k * ((*jv)[q] - 1) + (*iv)[q] - 1].data;
    /* delete old cone */
    delete d;
    if (L->m[q].rtyp == INT_CMD)   /* then (int)(long)L->m[q].data == 0 */
    { /* symmetric fill */
      m_adjacency->m[k * ((*iv)[q] - 1) + (*jv)[q] - 1].data = Cone::noFacet();
      m_adjacency->m[k * ((*jv)[q] - 1) + (*iv)[q] - 1].data = Cone::noFacet();
    }
    else
    { /* symmetric fill */
      m_adjacency->m[k * ((*iv)[q] - 1) + (*jv)[q] - 1].data
        = new Cone(*(Cone*)L->m[q].data);
      m_adjacency->m[k * ((*jv)[q] - 1) + (*iv)[q] - 1].data
        = new Cone(*(Cone*)L->m[q].data);
    }
  }
}

#endif
/* HAVE_FANS */
