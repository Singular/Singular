/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/* $Id: khstd.cc,v 1.4 1997-04-02 15:07:14 Singular Exp $ */
/*
* ABSTRACT:utils for hilbert driven std
*/

#include "mod2.h"
#include "tok.h"
#include "febase.h"
#include "polys.h"
#include "intvec.h"
#include "kutil.h"
#include "stairc.h"
#include "kstd1.h"
#include "khstd.h"

/*2
* compare the given hilbert series with the current one,
* delete not needed pairs (if possible)
*/
void khCheck( ideal Q, intvec *w, intvec *hilb, int &eledeg, int &count,
              kStrategy strat)
  /* ideal S=strat->Shdl, poly p=strat->P.p */
/*
* compute the number eledeg of elements with a degree >= deg(p) going into std,
* p is already in S and for all further q going into S yields deg(q) >= deg(p),
* the real computation is only done if the degree has changed,
* then we have eledeg == 0 on this degree and we make:
*   - compute the Hilbert series newhilb from S
*     (hilb is the final Hilbert series)
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
  int deg;
  pFDegProc degp=pFDeg;
  if (pFDeg!=kModDeg) { degp=pTotaldegree;}

  eledeg--;
  if (eledeg == 0)
  {
    newhilb = hHstdSeries(strat->Shdl,w,Q);
    deg = degp(strat->P.p);
    loop // compare the series in degree deg, try to increase deg -----------
    {
      if (deg < newhilb->length()) // deg may be out of range
      {
        if (deg < hilb->length())
          eledeg = (*newhilb)[deg]-(*hilb)[deg];
        else
          eledeg = (*newhilb)[deg];
      }
      else
      {
        if (deg < hilb->length())
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
          return;
        }
      }
      if (eledeg != 0)
        break;
      deg++;
    } /* loop */
    delete newhilb;
    while ((strat->Ll>=0) && (degp(strat->L[strat->Ll].p) < deg)) // the essential step
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
