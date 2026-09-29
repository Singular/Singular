/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/*
* ABSTRACT:utils for hilbert driven kStd
*/

#include "kernel/mod2.h"

#include "misc/options.h"
#include "misc/intvec.h"

#include "polys/simpleideals.h"

#include "kernel/combinatorics/stairc.h"
#include "kernel/combinatorics/hilb.h"

#include "kernel/GBEngine/kutil.h"
#include "kernel/GBEngine/kstd1.h"
#include "kernel/GBEngine/khstd.h"

#include "kernel/polys.h"

#include <limits>

/*2
* compare the given hilbert series with the current one,
* delete not needed pairs (if possible)
*/
void khCheck( ideal Q, intvec *w, bigintmat *hilb, int &eledeg, int &count,
              kStrategy strat)
  /* ideal S=strat->Shdl, poly p=strat->P.p */
/*
* compute the number eledeg of elements with a degree >= deg(p) going into kStd,
* p is already in S and for all further q going into S yields deg(q) >= deg(p),
* the real computation is only done if the degree has changed,
* then we have eledeg == 0 on this degree and we make:
*   - compute the Hilbert series newhilb from S
*     (hilb is the final Hilbert series)
*   - in module case: check that all comp up to strat->ak are used
*   - compute the eledeg from newhilb-hilb for the first degree deg with
*     newhilb-hilb != 0
*     (Remark: consider the Hilbert series with coeff. up to infinity)
*   - clear the set L for degree < deg
* the number count is only for statistics (in the caller initialise count = 0),
* in order to get a first computation, initialise eledeg = 1 in the caller.
* The weights w are needed in the module case, otherwise NULL.
*/
{
  if (strat->kHilb!=NULL)
  {
    khCheck64(Q,w,strat->kHilb,strat->kHilbRing,eledeg,count,strat);
    return;
  }
  bigintmat *newhilb;
  long deg;
  int l,ln,mw;
  pFDegProc degp;

  eledeg--;
  if (eledeg == 0)
  {
    if (strat->ak>0)
    {
      char *used_comp=(char*)omAlloc0(strat->ak+1);
      int i;
      for(i=strat->sl;i>0;i--)
      {
        used_comp[pGetComp(strat->S[i])]='\1';
      }
      for(i=strat->ak;i>0;i--)
      {
        if(used_comp[i]=='\0')
        {
          omFree((ADDRESS)used_comp);
          return;
        }
      }
      omFree((ADDRESS)used_comp);
    }
    degp=currRing->pFDeg;
    // if weights for variables were given to std computations,
    // then pFDeg == degp == kHomModDeg (see kStd)
    if ((degp!=kModDeg) && (degp!=kHomModDeg)) degp=p_Totaldegree;
    // degp = pWDegree;
    l = hilb->cols();
    mw = n_Int(BIMATELEM(*hilb,1,l),coeffs_BIGINT);
    if (strat->kHomW64!=NULL)
      newhilb=hFirstSeries0b64(strat->Shdl,Q,strat->kHomW64,w,
                               currRing,coeffs_BIGINT);
    else
      newhilb=hFirstSeries0b(strat->Shdl,Q,strat->kHomW,w,
                             currRing,coeffs_BIGINT);
    if (newhilb==NULL) return;
    ln = newhilb->cols();
    deg = degp(strat->P.p,currRing);
    loop // compare the series in degree deg, try to increase deg -----------
    {
      if ((deg >= 0) && (deg < ln)) // deg may be out of range
      {
        if (deg < l)
        {
          number s=n_Sub(BIMATELEM(*newhilb,1,deg+1),
                         BIMATELEM(*hilb,1,deg+1),coeffs_BIGINT);
          eledeg = n_Int(s,coeffs_BIGINT);
        }
        else
          eledeg = n_Int(BIMATELEM(*newhilb,1,deg+1),coeffs_BIGINT);
      }
      else
      {
        if ((deg >= 0) && (deg < l))
          eledeg = -n_Int(BIMATELEM(*hilb,1,deg+1),coeffs_BIGINT);
        else // we have newhilb = hilb
        {
          while (strat->Ll>=0)
          {
            count++;
            if(TEST_OPT_PROT)
            {
              PrintS("h");
              mflush();
            }
            deleteInL(strat->L,&strat->Ll,strat->Ll,strat);
          }
          delete newhilb;
          return;
        }
      }
      if (eledeg > 0) // elements to delete
        break;
      else if (eledeg <0) // strange....see bug_43
        return;
      deg++;
    } /* loop */
    delete newhilb;
    while ((strat->Ll>=0) && (degp(strat->L[strat->Ll].p,currRing)-mw < deg)) // the essential step
    {
      count++;
      if(TEST_OPT_PROT)
      {
        PrintS("h");
        mflush();
      }
      deleteInL(strat->L,&strat->Ll,strat->Ll,strat);
    }
  }
}

#if 0
/*2
* compare the given hilbert series with the current one,
* delete not needed pairs (if possible)
*/
void khCheck( ideal Q, intvec *w, poly hilb, const ring Qt, int &eledeg, int &count,
              kStrategy strat)
  /* ideal S=strat->Shdl, poly p=strat->P.p */
/*
* compute the number eledeg of elements with a degree >= deg(p) going into kStd,
* p is already in S and for all further q going into S yields deg(q) >= deg(p),
* the real computation is only done if the degree has changed,
* then we have eledeg == 0 on this degree and we make:
*   - compute the Hilbert series newhilb from S
*     (hilb is the final Hilbert series)
*   - in module case: check that all comp up to strat->ak are used
*   - compute the eledeg from newhilb-hilb for the first degree deg with
*     newhilb-hilb != 0
*     (Remark: consider the Hilbert series with coeff. up to infinity)
*   - clear the set L for degree < deg
* the number count is only for statistics (in the caller initialise count = 0),
* in order to get a first computation, initialise eledeg = 1 in the caller.
* The weights w are needed in the module case, otherwise NULL.
*/
{
  poly newhilb;
  int deg;
  int l,ln;
  mpz_t mw;
  pFDegProc degp;

  eledeg--;
  if (eledeg == 0)
  {
    if (strat->ak>0)
    {
      char *used_comp=(char*)omAlloc0(strat->ak+1);
      int i;
      for(i=strat->sl;i>0;i--)
      {
        used_comp[pGetComp(strat->S[i])]='\1';
      }
      for(i=strat->ak;i>0;i--)
      {
        if(used_comp[i]=='\0')
        {
          omFree((ADDRESS)used_comp);
          return;
        }
      }
      omFree((ADDRESS)used_comp);
    }
    degp=currRing->pFDeg;
    // if weights for variables were given to std computations,
    // then pFDeg == degp == kHomModDeg (see kStd)
    if ((degp!=kModDeg) && (degp!=kHomModDeg)) degp=p_Totaldegree;
    // degp = pWDegree;
    l = p_FDeg(hilb,Qt);
    number lt=pGetcoeff(hilb);
    n_MPZ(mw,&lt,Qt->cf);
    newhilb =hFirstSeries0m(strat->Shdl,Q,w,strat->kHomW,currRing,Qt);
    ln = p_FDeg(newhilb);
    deg = degp(strat->P.p,currRing);
    loop // compare the series in degree deg, try to increase deg -----------
    {
      if (deg < ln) // deg may be out of range
      {
        if (deg < l)
          eledeg = (*newhilb)[deg]-(*hilb)[deg];
        else
          eledeg = (*newhilb)[deg];
      }
      else
      {
        if (deg < l)
          eledeg = -(*hilb)[deg];
        else // we have newhilb = hilb
        {
          while (strat->Ll>=0)
          {
            count++;
            if(TEST_OPT_PROT)
            {
              PrintS("h");
              mflush();
            }
            deleteInL(strat->L,&strat->Ll,strat->Ll,strat);
          }
          delete newhilb;
          return;
        }
      }
      if (eledeg > 0) // elements to delete
        break;
      else if (eledeg <0) // strange....see bug_43
        return;
      deg++;
    } /* loop */
    delete newhilb;
    while ((strat->Ll>=0) && (degp(strat->L[strat->Ll].p,currRing)-mw < deg)) // the essential step
    {
      count++;
      if(TEST_OPT_PROT)
      {
        PrintS("h");
        mflush();
      }
      deleteInL(strat->L,&strat->Ll,strat->Ll,strat);
    }
  }
}
#endif

/* Compare sparse Hilbert numerators.  Unlike bigintmat, this representation
 * is indexed by monomial exponents and therefore remains practical when the
 * first relevant degree is larger than INT_MAX. */
void khCheck64(ideal Q, intvec *w, poly hilb, const ring Qt,
               int &eledeg, int &count, kStrategy strat)
{
  eledeg--;
  if (eledeg!=0) return;

  if (strat->ak>0)
  {
    char *used_comp=(char*)omAlloc0(strat->ak+1);
    int i;
    for(i=strat->sl;i>0;i--) used_comp[pGetComp(strat->S[i])]='\1';
    for(i=strat->ak;i>0;i--)
    {
      if(used_comp[i]=='\0')
      {
        omFree((ADDRESS)used_comp);
        return;
      }
    }
    omFree((ADDRESS)used_comp);
  }

  pFDegProc degp=currRing->pFDeg;
  if ((degp!=kModDeg) && (degp!=kHomModDeg)) degp=p_Totaldegree;

  poly newhilb;
  if (id_IsModule(strat->Shdl,currRing))
  {
    if (strat->kHomW64!=NULL)
      newhilb=hFirstSeries0m64(strat->Shdl,Q,strat->kHomW64,w,currRing,Qt);
    else
      newhilb=hFirstSeries0m(strat->Shdl,Q,strat->kHomW,w,currRing,Qt);
  }
  else
  {
    if (strat->kHomW64!=NULL)
      newhilb=hFirstSeries0p64(strat->Shdl,Q,strat->kHomW64,currRing,Qt);
    else
      newhilb=hFirstSeries0p(strat->Shdl,Q,strat->kHomW,currRing,Qt);
  }
  if (errorreported)
  {
    p_Delete(&newhilb,Qt);
    return;
  }

  poly difference=p_Sub(p_Copy(newhilb,Qt),p_Copy(hilb,Qt),Qt);
  p_Delete(&newhilb,Qt);
  const int64 currentDegree=(int64)degp(strat->P.p,currRing);
  int64 degree=std::numeric_limits<int64>::max();
  long coefficient=0;
  for (poly p=difference; p!=NULL; pIter(p))
  {
    const int64 d=(int64)p_GetExp(p,1,Qt);
    if ((d>=currentDegree) && (d<degree))
    {
      degree=d;
      coefficient=n_Int(pGetCoeff(p),Qt->cf);
    }
  }

  if (degree==std::numeric_limits<int64>::max())
  {
    while (strat->Ll>=0)
    {
      count++;
      if(TEST_OPT_PROT) { PrintS("h"); mflush(); }
      deleteInL(strat->L,&strat->Ll,strat->Ll,strat);
    }
    p_Delete(&difference,Qt);
    return;
  }

  if (coefficient<0)
  {
    p_Delete(&difference,Qt);
    return;
  }
  if (coefficient>INT_MAX)
  {
    WerrorS("Hilbert coefficient does not fit into int");
    p_Delete(&difference,Qt);
    return;
  }
  eledeg=(int)coefficient;
  p_Delete(&difference,Qt);

  while (strat->Ll>=0)
  {
    const int64 pairDegree=(int64)degp(strat->L[strat->Ll].p,currRing);
    if (pairDegree>=degree) break;
    count++;
    if(TEST_OPT_PROT) { PrintS("h"); mflush(); }
    deleteInL(strat->L,&strat->Ll,strat->Ll,strat);
  }
}

void khCheckLocInhom(ideal Q, intvec *w, bigintmat *hilb, int &count,
             kStrategy strat)

/*
This will be used for the local orderings in the case of the inhomogeneous ideals.
Assume f1,...,fs are already in the standard basis. Test if hilb(LM(f1),...,LM(fs),1)
is equal to the inputted one.
If no, do nothing.
If Yes, we know that all polys that we need are already in the standard basis
so delete all the remaining pairs
*/
{
  ideal Lm;
  bigintmat *newhilb;

  Lm = id_Head(strat->Shdl,currRing);

  if (strat->kHomW64!=NULL)
    newhilb=hFirstSeries0b64(strat->Shdl,Q,strat->kHomW64,w,
                             currRing,coeffs_BIGINT);
  else
    newhilb=hFirstSeries0b(strat->Shdl,Q,strat->kHomW,w,
                           currRing,coeffs_BIGINT);
  if (newhilb==NULL)
  {
    id_Delete(&Lm,currRing);
    return;
  }

  if(newhilb->compare(hilb) == 0)
  {
    while (strat->Ll>=0)
    {
      count++;
      if(TEST_OPT_PROT)
      {
        PrintS("h");
        mflush();
      }
      deleteInL(strat->L,&strat->Ll,strat->Ll,strat);
    }
    delete newhilb;
    id_Delete(&Lm,currRing);
    return;
  }
  delete newhilb;
  id_Delete(&Lm,currRing);
}
