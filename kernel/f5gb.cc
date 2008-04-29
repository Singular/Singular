/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/* $Id: f5gb.cc,v 1.2 2008-04-29 18:34:06 ederc Exp $ */
/*
* ABSTRACT: f5gb interface
*/
#include "mod2.h"
#include "kutil.h"
#include "structs.h"
#include "omalloc.h"
#include "polys.h"
#include "p_polys.h"
#include "ideals.h"
#include "febase.h"
#include "kstd1.h"
#include "khstd.h"
#include "kbuckets.h"
#include "weight.h"
#include "intvec.h"
#include "pInline1.h"

#include "f5gb.h"

#ifdef HAVE_F5
ideal computeF5(ideal i, ring r)
{
  ideal res;
        
  Print("Hallo\n");
  res = idInit(IDELEMS(i),i->rank);
  for(int j=0; j<IDELEMS(i); j++)
  {
    if(NULL != i->m[j])
    {
      res->m[j] = i->m[j];
    }
  }
  Print("Ideal i: %d \n", IDELEMS(i));mflush();
  Print("Ideal res: %d", IDELEMS(res));mflush();

  return NULL;
}
#endif
