#ifndef LISTS_H
#define LISTS_H
/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/*
* ABSTRACT: handling of the list type
*/
#include <omalloc/omalloc.h>
#include <kernel/structs.h>
#include <kernel/ideals.h>
#include <Singular/subexpr.h>
#include <Singular/tok.h>

#ifdef MDEBUG
#define INLINE_THIS
#else
#define INLINE_THIS inline
#endif

extern omBin slists_bin;
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
        //omFreeSize((ADDRESS)this, sizeof(slists));
        omFreeBin((ADDRESS)this,slists_bin);
      }
    }
  INLINE_THIS void Init(int l=0);
    int    nr; /* the number of elements in the list -1 */
               /* -1: empty list */
    sleftv  *m;  /* field of sleftv */
};

int lSize(lists L);
lists lCopy(lists L);
lists lInsert0(lists ul, leftv v, int pos);
BOOLEAN lInsert(leftv res, leftv u, leftv v);
BOOLEAN lInsert3(leftv res, leftv u, leftv v, leftv w);
BOOLEAN lAppend(leftv res, leftv u, leftv v);
BOOLEAN lDelete(leftv res, leftv u, leftv v);
BOOLEAN lAdd(leftv res, leftv u, leftv v);
BOOLEAN lRingDependend(lists L);
char* lString(lists l, BOOLEAN typed = FALSE, int dim = 1);


lists liMakeResolv(resolvente r, int length, int reallen, int typ0, intvec ** weights,int add_row_shift);
resolvente liFindRes(lists L, int * len, int *typ0,intvec *** weights=NULL);

#if ! defined(MDEBUG) || defined(LISTS_CC)
INLINE_THIS void slists::Init(int l)
      { nr=l-1; m=(sleftv *)((l>0) ? omAlloc0(l*sizeof(sleftv)): NULL);
      }
#endif

#undef INLINE_THIS

#endif
