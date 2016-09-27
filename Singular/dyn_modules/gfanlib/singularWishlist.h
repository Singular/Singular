#ifndef SINGULARWISHLIST_H
#define SINGULARWISHLIST_H

#include <polys/monomials/p_polys.h>
#include <libpolys/polys/simpleideals.h>

static inline BOOLEAN _p_LeadmonomDivisibleByNoComp(poly a, poly b, const ring r)
{
  int i=r->VarL_Size - 1;
  unsigned long divmask = r->divmask;
  unsigned long la, lb;

  if (r->VarL_LowIndex >= 0)
  {
    i += r->VarL_LowIndex;
    do
    {
      la = a->exp[i];
      lb = b->exp[i];
      if ((la > lb) ||
          (((la & divmask) ^ (lb & divmask)) != ((lb - la) & divmask)))
      {
        pDivAssume(p_DebugLmDivisibleByNoComp(a, b, r) == FALSE);
        return FALSE;
      }
      i--;
    }
    while (i>=r->VarL_LowIndex);
  }
  else
  {
    do
    {
      la = a->exp[r->VarL_Offset[i]];
      lb = b->exp[r->VarL_Offset[i]];
      if ((la > lb) ||
          (((la & divmask) ^ (lb & divmask)) != ((lb - la) & divmask)))
      {
        pDivAssume(p_DebugLmDivisibleByNoComp(a, b, r) == FALSE);
        return FALSE;
      }
      i--;
    }
    while (i>=0);
  }
  pDivAssume(p_DebugLmDivisibleByNoComp(a, b, r) == TRUE);
  return TRUE;
}

/**
 * p_LmDivisibleBy checks also the divisibility of coefficients
 **/
static inline BOOLEAN p_LeadmonomDivisibleBy(poly a, poly b, const ring r)
{
  p_LmCheckPolyRing1(b, r);

  pIfThen1(a != NULL, p_LmCheckPolyRing1(b, r));
  if (p_GetComp(a, r) == 0 || p_GetComp(a,r) == p_GetComp(b,r))
    return _p_LeadmonomDivisibleByNoComp(a, b, r);
  return FALSE;
}

/**
 * id_ShallowDelete deletes the monomials of the polynomials stored inside of it
 **/
inline void idShallowDelete (ideal *h)
{
  if (*h != NULL)
  {
    int k;
    k=(*h)->nrows*(*h)->ncols;
    if (k>0)
      omFreeSize((ADDRESS)((*h)->m),sizeof(poly)*k);
    omFreeBin((ADDRESS)*h, sip_sideal_bin);
    *h=NULL;
  }
  return;
}

inline void deleteOrdering(ring r)
{
  if (r->order != NULL)
  {
    int i=rBlocks(r);
    assume(r->block0 != NULL && r->block1 != NULL && r->wvhdl != NULL);
    /* delete order */
    omFreeSize((ADDRESS)r->order,i*sizeof(int));
    omFreeSize((ADDRESS)r->block0,i*sizeof(int));
    omFreeSize((ADDRESS)r->block1,i*sizeof(int));
    /* delete weights */
    for (int j=0; j<i; j++)
      if (r->wvhdl[j]!=NULL)
        omFree(r->wvhdl[j]);
    omFreeSize((ADDRESS)r->wvhdl,i*sizeof(int *));
  }
  else
    assume(r->block0 == NULL && r->block1 == NULL && r->wvhdl == NULL);
  return;
}


void z_Write(number p, ring r);
void id_Write(const ideal I, const ring r);


#endif
