#ifndef LISTS_H
#define LISTS_H
/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/* $Id: lists.h,v 1.17 2002-01-10 12:33:21 Singular Exp $ */
/*
* ABSTRACT: handling of the list type
*/
#include "structs.h"
#include "omalloc.h"
#include "subexpr.h"
#include "tok.h"

#ifdef MDEBUG
#define INLINE_THIS
#else
#define INLINE_THIS inline
#endif

class slists
{
  public:
    void Clean(ring r=currRing)
    {
      if (this!=NULL)
      {
        if (nr>=0)
        {
          int i;
          for(i=nr;i>=0;i--)
          {
            if (m[i].rtyp!=DEF_CMD) m[i].CleanUp(r);
          }
          omFreeSize((ADDRESS)m, (nr+1)*sizeof(sleftv));
          nr=-1;
        }
        omFreeSize((ADDRESS)this, sizeof(slists));
      }
    }
  INLINE_THIS void Init(int l=0);
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
char* lString(lists l, BOOLEAN typed = FALSE, int dim = 1);


lists liMakeResolv(resolvente r, int length, int reallen, int typ0, intvec ** weights);
resolvente liFindRes(lists L, int * len, int *typ0,intvec *** weights=NULL);

#if ! defined(MDEBUG) || defined(LISTS_CC)
INLINE_THIS void slists::Init(int l)
      { nr=l-1; m=(sleftv *)((l>0) ? omAlloc0(l*sizeof(sleftv)): NULL);
#ifdef HAVE_NAMESPACES_N
        src_packhdl = namespaceroot->get(namespaceroot->name, 0, TRUE);
#endif /* HAVE_NAMESPACES */
      }
#endif

extern omBin slists_bin;
#undef INLINE_THIS

#endif
