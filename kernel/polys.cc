
#include <kernel/mod2.h>

#include <omalloc/omalloc.h>
#include <misc/auxiliary.h>
#include <misc/options.h>

#include "polys.h"

ring  currRing = NULL;

void rChangeCurrRing(ring r)
{
  #if 0
  if ((currRing!=NULL)&&(currRing!=r))
  {
    currRing->options=si_opt_1 & TEST_RINGDEP_OPTS;
  }
  #endif
  if( r != NULL )
  {
    rTest(r);    

    //------------ set global ring vars --------------------------------
    currRing = r;
    //------------ global variables related to coefficients ------------
    assume( r->cf!= NULL );
    nSetChar(r->cf);
    //------------ global variables related to polys
    p_SetGlobals(r);
    //------------ global variables related to factory -----------------
  }
  else
  {
    currRing = NULL;
  }
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
  if (r != NULL)
  {
    rTest(r);
    //------------ set global ring vars --------------------------------

    //------------ global variables related to coefficients ------------
    nSetChar(r->cf);

    //------------ global variables related to polys -------------------
    p_SetGlobals(r);
    //------------ global variables related to factory -----------------
  }
}
*/
