#ifndef LISTS_H
#define LISTS_H
/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/* $Id: lists.h,v 1.6 1998-10-21 10:25:38 krueger Exp $ */
/*
* ABSTRACT: handling of the list type
*/
#include "structs.h"
#include "mmemory.h"
#include "subexpr.h"
#include "ipid.h"
#include "tok.h"

class slists
{
  public:
    void Clean()
    {
      if (this!=NULL)
      {
        if (nr>=0)
        {
          int i;
          for(i=nr;i>=0;i--)
          {
            if (m[i].rtyp!=DEF_CMD) m[i].CleanUp();
          }
          Free((ADDRESS)m, (nr+1)*sizeof(sleftv));
          nr=-1;
        }
        Free((ADDRESS)this, sizeof(slists));
      }
    }
    inline void Init(int l=0)
      { nr=l-1; m=(sleftv *)((l>0) ? Alloc0(l*sizeof(sleftv)): NULL);
#ifdef HAVE_NAMESPACES_N
        src_packhdl = namespaceroot->get(namespaceroot->name, 0, TRUE);
#endif /* HAVE_NAMESPACES */
      }
    int    nr; /* the number of elements in the list -1 */
               /* -1: empty list */
#ifdef HAVE_NAMESPACES_N
    idhdl src_packhdl;
#endif /* HAVE_NAMESPACES */
    sleftv  *m;  /* field of sleftv */
};

lists lCopy(lists L);
lists lInsert0(lists ul, leftv v, int pos);
BOOLEAN lInsert(leftv res, leftv u, leftv v);
BOOLEAN lInsert3(leftv res, leftv u, leftv v, leftv w);
BOOLEAN lAppend(leftv res, leftv u, leftv v);
BOOLEAN lDelete(leftv res, leftv u, leftv v);
BOOLEAN lAdd(leftv res, leftv u, leftv v);
BOOLEAN lRingDependend(lists L);

lists liMakeResolv(resolvente r, int length, int reallen, int typ0, intvec ** weights);
resolvente liFindRes(lists L, int * len, int *typ0,intvec *** weights=NULL);
#endif
