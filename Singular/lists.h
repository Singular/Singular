#ifndef LISTS_H
#define LISTS_H
/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/*
* ABSTRACT: handling of the list type
*/
/* $Log: not supported by cvs2svn $
*/
#include "structs.h"
#include "mmemory.h"
#include "subexpr.h"
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
      { nr=l-1; m=(sleftv *)((l>0) ? Alloc0(l*sizeof(sleftv)): NULL); }
    int    nr; /* the number of elements in the list -1 */
               /* -1: empty list */
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
resolvente liFindRes(lists L, int * len, int *typ0);
#endif
