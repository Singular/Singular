


#include "kernel/mod2.h"

#include "kernel/polys.h"

/* Returns TRUE if
     * LM(p) | LM(lcm)
     * LC(p) | LC(lcm) only if ring
     * Exists i, j:
         * LE(p, i)  != LE(lcm, i)
         * LE(p1, i) != LE(lcm, i)   ==> LCM(p1, p) != lcm
         * LE(p, j)  != LE(lcm, j)
         * LE(p2, j) != LE(lcm, j)   ==> LCM(p2, p) != lcm
*/
BOOLEAN pCompareChain (poly p,poly p1,poly p2,poly lcm, const ring R)
{
  int k, j;

  if (lcm==NULL) return FALSE;

  for (j=(R->N); j; j--)
    if ( p_GetExp(p,j, R) >  p_GetExp(lcm,j, R)) return FALSE;
  if ( pGetComp(p) !=  pGetComp(lcm)) return FALSE;
  for (j=(R->N); j; j--)
  {
    if (p_GetExp(p1,j, R)!=p_GetExp(lcm,j, R))
    {
      if (p_GetExp(p,j, R)!=p_GetExp(lcm,j, R))
      {
        for (k=(R->N); k>j; k--)
        {
          if ((p_GetExp(p,k, R)!=p_GetExp(lcm,k, R))
          && (p_GetExp(p2,k, R)!=p_GetExp(lcm,k, R)))
            return TRUE;
        }
        for (k=j-1; k; k--)
        {
          if ((p_GetExp(p,k, R)!=p_GetExp(lcm,k, R))
          && (p_GetExp(p2,k, R)!=p_GetExp(lcm,k, R)))
            return TRUE;
        }
        return FALSE;
      }
    }
    else if (p_GetExp(p2,j, R)!=p_GetExp(lcm,j, R))
    {
      if (p_GetExp(p,j, R)!=p_GetExp(lcm,j, R))
      {
        for (k=(R->N); k>j; k--)
        {
          if ((p_GetExp(p,k, R)!=p_GetExp(lcm,k, R))
          && (p_GetExp(p1,k, R)!=p_GetExp(lcm,k, R)))
            return TRUE;
        }
        for (k=j-1; k!=0 ; k--)
        {
          if ((p_GetExp(p,k, R)!=p_GetExp(lcm,k, R))
          && (p_GetExp(p1,k, R)!=p_GetExp(lcm,k, R)))
            return TRUE;
        }
        return FALSE;
      }
    }
  }
  return FALSE;
}

#ifdef HAVE_RATGRING
BOOLEAN pCompareChainPart (poly p,poly p1,poly p2,poly lcm, const ring R)
{
  int k, j;

  if (lcm==NULL) return FALSE;

  for (j=R->real_var_end; j>=R->real_var_start; j--)
    if ( p_GetExp(p,j, R) >  p_GetExp(lcm,j, R)) return FALSE;
  if ( pGetComp(p) !=  pGetComp(lcm)) return FALSE;
  for (j=R->real_var_end; j>=R->real_var_start; j--)
  {
    if (p_GetExp(p1,j, R)!=p_GetExp(lcm,j, R))
    {
      if (p_GetExp(p,j, R)!=p_GetExp(lcm,j, R))
      {
        for (k=(R->N); k>j; k--)
        for (k=R->real_var_end; k>j; k--)
        {
          if ((p_GetExp(p,k, R)!=p_GetExp(lcm,k, R))
          && (p_GetExp(p2,k, R)!=p_GetExp(lcm,k, R)))
            return TRUE;
        }
        for (k=j-1; k>=R->real_var_start; k--)
        {
          if ((p_GetExp(p,k, R)!=p_GetExp(lcm,k, R))
          && (p_GetExp(p2,k, R)!=p_GetExp(lcm,k, R)))
            return TRUE;
        }
        return FALSE;
      }
    }
    else if (p_GetExp(p2,j, R)!=p_GetExp(lcm,j, R))
    {
      if (p_GetExp(p,j, R)!=p_GetExp(lcm,j, R))
      {
        for (k=R->real_var_end; k>j; k--)
        {
          if ((p_GetExp(p,k, R)!=p_GetExp(lcm,k, R))
          && (p_GetExp(p1,k, R)!=p_GetExp(lcm,k, R)))
            return TRUE;
        }
        for (k=j-1; k>=R->real_var_start; k--)
        {
          if ((p_GetExp(p,k, R)!=p_GetExp(lcm,k, R))
          && (p_GetExp(p1,k, R)!=p_GetExp(lcm,k, R)))
            return TRUE;
        }
        return FALSE;
      }
    }
  }
  return FALSE;
}
#endif

