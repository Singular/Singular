/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/*
* ABSTRACT: polynomial powers reduced modulo a standard basis
*/

#include "kernel/mod2.h"

#include "kernel/GBEngine/qring_power.h"
#include "kernel/GBEngine/kstd1.h"
#include "kernel/ideals.h"
#include "kernel/polys.h"

static long pPowerModMaxTotaldegree(poly p, const ring r)
{
  long degree=0;
  while (p!=NULL)
  {
    degree=si_max(degree,p_Totaldegree(p,r));
    pIter(p);
  }
  return degree;
}

static BOOLEAN pPowerModOverflow(poly left, poly right, const ring r)
{
  const long left_degree=pPowerModMaxTotaldegree(left,r);
  const long right_degree=pPowerModMaxTotaldegree(right,r);
  const long max_degree=si_max((long)rVar(r),(long)r->bitmask/2);
  BOOLEAN overflow=(left_degree>max_degree-right_degree);

  if (!overflow)
  {
    poly left_max=p_GetMaxExpP(left,r);
    poly right_max=p_GetMaxExpP(right,r);
    overflow=!p_LmExpVectorAddIsOk(left_max,right_max,r);
    p_LmFree(left_max,r);
    p_LmFree(right_max,r);
  }

  if (!overflow) return FALSE;

  Werror("OVERFLOW in quotient power(d=%ld, d=%ld, max=%ld)",
         left_degree,right_degree,max_degree);
  return TRUE;
}

static poly pPowerModMultiply(poly left, poly right, ideal G, const ring r)
{
  if ((left==NULL) || (right==NULL))
    return p_Mult_q(left,right,r);

  if (pPowerModOverflow(left,right,r))
  {
    p_Delete(&left,r);
    p_Delete(&right,r);
    return NULL;
  }

  poly product=p_Mult_q(left,right,r);
  poly result=kNF(G,NULL,product);
  p_Delete(&product,r);
  return result;
}

static poly pPowerModCurrent(poly base, int exp, ideal G, const ring r)
{
  if (exp==0)
  {
    p_Delete(&base,r);
    poly one=p_One(r);
    poly result=kNF(G,NULL,one);
    p_Delete(&one,r);
    return result;
  }
  if (base==NULL) return NULL;

  poly reduced=kNF(G,NULL,base);
  p_Delete(&base,r);
  base=reduced;

  if ((base==NULL) || (exp==1)) return base;

  poly result=NULL;
  while (exp>0)
  {
    if ((exp & 1)!=0)
    {
      poly factor;
      if (exp==1)
      {
        factor=base;
        base=NULL;
      }
      else
        factor=p_Copy(base,r);

      if (result==NULL)
        result=factor;
      else
      {
        result=pPowerModMultiply(result,factor,G,r);
        if (result==NULL)
        {
          p_Delete(&base,r);
          return NULL;
        }
      }
    }

    exp>>=1;
    if (exp!=0)
    {
      base=pPowerModMultiply(base,p_Copy(base,r),G,r);
      if (base==NULL)
      {
        p_Delete(&result,r);
        return NULL;
      }
    }
  }
  p_Delete(&base,r);
  return result;
}

poly p_PowerMod(poly base, int exp, ideal G, const ring r)
{
  if ((G==NULL) || idIs0(G) || (exp<0))
    return p_Power(base,exp,r);
  if (rIsNCRing(r))
  {
    WerrorS("quotient power is not implemented for non-commutative rings");
    p_Delete(&base,r);
    return NULL;
  }

  const ring save_ring=currRing;
  if (r!=currRing) rChangeCurrRing(r);
  poly result=pPowerModCurrent(base,exp,G,r);
  if (r!=save_ring) rChangeCurrRing(save_ring);
  return result;
}
