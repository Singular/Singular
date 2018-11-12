#include "Singular/libsingular.h"

#ifdef HAVE_SHIFTBBA
static BOOLEAN freegb(leftv res, leftv args)
{
  const short t1[]={2,RING_CMD,INT_CMD};
  if (iiCheckTypes(args,t1,1))
  {
    ring r=(ring)args->Data();
    int d=(int)(long)args->next->Data();
    if (d<2)
    {
      WerrorS("degree must be >=2");
      return TRUE;
    }
    if ((r->order[2]!=0)
    || (rHasLocalOrMixedOrdering(r)))
    {
      WerrorS("only for rings with a global ordering of one block");
      return TRUE;
    }
    ring R=rCopy0(r);
    int p;
    if((r->order[0]==ringorder_C)
    ||(r->order[0]==ringorder_c))
      p=1;
    else
      p=0;
    // create R->N
    R->N=r->N*d;
    R->isLPring=r->N;
    R->block1[p]=R->N; /* only dp,Dp,wp,Wp; will be discarded for lp*/
    // create R->order
    switch(r->order[p])
    {
      case ringorder_dp:
      case ringorder_Dp:
        break;
      case ringorder_wp:
      case ringorder_Wp:
      {
        omFree(R->wvhdl[p]);
        int *w=(int*)omAlloc(R->N*sizeof(int));
        for(int b=1;b<d;b++)
        {
          for(int i=r->N-1;i>=0;i--)
            w[b*r->N+i]=r->wvhdl[p][i];
        }
        R->wvhdl[p]=w;
        break;
      }
      case ringorder_lp:
      case ringorder_rp:
      {
        int ** wvhdl=(int**)omAlloc0((r->N+2)*sizeof(int*));
        rRingOrder_t* ord=(rRingOrder_t*)omAlloc0((r->N+2)*sizeof(rRingOrder_t));
        int* blk0=(int*)omAlloc0((r->N+2)*sizeof(int));
        int* blk1=(int*)omAlloc0((r->N+2)*sizeof(int));
        omFree(R->wvhdl);  R->wvhdl=wvhdl;
        omFree(R->order);  R->order=ord;
        omFree(R->block0); R->block0=blk0;
        omFree(R->block1); R->block1=blk1;
        for(int i=0;i<r->N;i++)
        {
          ord[i+p]=ringorder_a;
          blk0[i+p]=1;
          blk1[i+p]=R->N;
          wvhdl[i+p]=(int*)omAlloc0(R->N*sizeof(int));
          for(int j=0;j<d;j++)
          {
            assume(j*r->N+i<R->N);
	    if (r->order[p]==ringorder_lp)
              wvhdl[i+p][j*r->N+i]=1;
	    else
              wvhdl[i+p][(j+1)*r->N-i-1]=1;
          }
        }
        ord[r->N+p]=r->order[p]; /* lp or rp */
        blk0[r->N+p]=1;
        blk1[r->N+p]=R->N;
        // copy component order
        if (p==1) ord[0]=r->order[0];
        else /*p==0*/ ord[r->N+1]=r->order[1];
        break;
      }
      default: WerrorS("ordering not implemented for LP-rings");
        return TRUE;
    }
    // create R->names
    char **names=(char**)omAlloc(R->N*sizeof(char_ptr));
    for(int b=0;b<d;b++)
    {
      for(int i=r->N-1;i>=0;i--)
        names[b*r->N+i]=omStrDup(r->names[i]);
    }
    for(int i=r->N-1;i>=0;i--) omFree(R->names[i]);
    omFree(R->names);
    R->names=names;

    rComplete(R,TRUE);
    res->rtyp=RING_CMD;
    res->data=R;
    return FALSE;
  }
  return TRUE;
}
#endif

//------------------------------------------------------------------------
// initialisation of the module
extern "C" int SI_MOD_INIT(freegb)(SModulFunctions* p)
{
#ifdef HAVE_SHIFTBBA
  p->iiAddCproc("freegb.so","freegb",FALSE,freegb);
#endif
  return (MAX_TOK);
}
