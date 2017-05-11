#include "polys/monomials/p_polys.h"
#include "libpolys/polys/simpleideals.h"

#ifndef SING_NDEBUG
void z_Write(number p, ring r)
{
  poly g = p_One(r);
  p_SetCoeff(g,p,r);
  p_Write(g,r);
  p_SetCoeff0(g,n_Init(1,r->cf),r);
  p_Delete(&g,r);
  return;
}

void id_Write(const ideal I, const ring r)
{
  for (int i=0; i<IDELEMS(I); i++)
    p_Write(I->m[i],r);
}
#endif
