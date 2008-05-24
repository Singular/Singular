/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/* $Id: f5gb.cc,v 1.4 2008-05-24 07:39:22 ederc Exp $ */
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
ideal F5main(ideal i, ring r)
{
  ideal iTmp, g;
  int j;      
  iTmp = idInit(IDELEMS(i),i->rank);
  
  for(j=0; j<IDELEMS(i); j++)
  {
    if(NULL != i->m[j])
    {
      iTmp->m[j] = i->m[j];
    }
  }
   
  iTmp = kInterRed(i,0);  
  idShow(i);
  idShow(iTmp);
  idShow(iTmp);
  return iTmp;
}
#endif
