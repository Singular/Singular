#include "mod2.h"

#include <omalloc/omalloc.h>
#include <misc/auxiliary.h>
#include <misc/options.h>

#include "polys.h"

ring  currRing = NULL;
ideal currQuotient = NULL;

/* -------------------------------------------------------- */
/*2
* change all global variables to fit the description of the new ring
*/

void p_SetGlobals(const ring r, BOOLEAN complete)
{
// // //  if (r->ppNoether!=NULL) p_Delete(&r->ppNoether,r); // ???

  if (complete)
  {
    test &= ~ TEST_RINGDEP_OPTS;
    test |= r->options;
  }
}

void rChangeCurrRing(ring r)
{
   currRing = r;
   currQuotient = r->qideal;
   //------------ global variables related to coefficients ------------
   nSetChar(r->cf);
   //------------ global variables related to polys -------------------
   p_SetGlobals(r);
}
/*

/// internally changes the gloabl ring and resets the relevant
/// global variables:
/// SHOULD BE DEPRECATED NOW...?
void rChangeCurrRing(ring r)
{
 // if (!rMinpolyIsNULL(currRing))
 // {
 //   omCheckAddr(currRing->cf->minpoly);
 // }
  //------------ set global ring vars --------------------------------
  //currRing = r;
  //currQuotient=NULL;
  if (r != NULL)
  {
    rTest(r);
    //------------ set global ring vars --------------------------------
    //currQuotient=r->qideal;

    //------------ global variables related to coefficients ------------
    nSetChar(r->cf);

    //------------ global variables related to polys -------------------
    p_SetGlobals(r);
    //------------ global variables related to factory -----------------
  }
}
*/
