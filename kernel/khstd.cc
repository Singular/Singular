/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/* $Id$ */
/*
* ABSTRACT:utils for hilbert driven kStd
*/

#include <kernel/mod2.h>
#include <kernel/febase.h>
#include <kernel/options.h>
#include <kernel/polys.h>
#include <kernel/intvec.h>
#include <kernel/kutil.h>
#include <kernel/stairc.h>
#include <kernel/kstd1.h>
#include <kernel/khstd.h>


/*2
* compare the given hilbert series with the current one,
* delete not needed pairs (if possible)
*/
void khCheck( ideal Q, intvec *w, intvec *hilb, int &eledeg, int &count,
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
  intvec *newhilb;
  int deg,l,ln,mw;
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
    degp=pFDeg;
    // if weights for variables were given to std computations,
    // then pFDeg == degp == kHomModDeg (see kStd)
    if ((degp!=kModDeg) && (degp!=kHomModDeg)) degp=p_Totaldegree;
    // degp = pWDegree;
    l = hilb->length()-1;
    mw = (*hilb)[l];
    newhilb = hHstdSeries(strat->Shdl,w,strat->kHomW,Q,strat->tailRing);
    ln = newhilb->length()-1;
    deg = degp(strat->P.p,currRing)-mw;
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
