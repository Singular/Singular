#include "mod2.h"
#include "structs.h"
#include "p_polys.h"
#include "fast_maps.h"
#include "febase.h"

// substitute p everywhere the monomial occours,
// return the number of substitutions
static int maPoly_Substitute(macoeff c, poly p, ring dest_r)
{
  // substitute the monomial: go through macoeff
  int len=pLength(p);
  int done=0;
  while (c!=NULL)
  {
    done++;
    poly t=pp_Mult_nn(p,c->n,dest_r);
    sBucket_Add_p(c->bucket, t, len);
    c=c->next;
  }
  return done;
}

static poly maPoly_EvalMon(poly src, ring src_r, poly* dest_id, ring dest_r)
{
  int i;
  int e;
  poly p=NULL;
  for(i=1;i<=src_r->N;i++)
  {
    e=p_GetExp(src,i,src_r);
    if ((e>0) && (dest_id[i-1]==NULL))
    {
      p_Delete(&p,dest_r);
      return NULL;
    }  
    if ((p==NULL) && (e>0))
    {
      p=p_Copy(dest_id[i-1],dest_r);
      e--;
    }
    while (e>0)
    {
      p=p_Mult_q(p,p_Copy(dest_id[i-1],dest_r),dest_r);
      e--;
    } 
  }
  if (p==NULL) p=p_ISet(1,dest_r);
  return p;
}

void maPoly_Eval(mapoly root, ring src_r, ideal dest_id, ring dest_r, int total_cost)
{
  // invert the list rooted at root:
  if ((root!=NULL) && (root->next!=NULL))
  {
    mapoly q=root->next;
    mapoly qn;
    root->next=NULL;
    do
    {
      qn=q->next;
      q->next=root;
      root=q;
      q=qn;
    }
    while (qn !=NULL);
  }

  total_cost /= 10;
  int next_print_cost = total_cost;
 
  // the evaluation -----------------------------------------
  mapoly p=root;
  int cost = 0;
 
  while (p!=NULL)
  {
    // look at each mapoly: compute its value in ->dest
    assume (p->dest==NULL);
    {
      if ((p->f1!=NULL)&&(p->f2!=NULL))
      {
#if 0
        printf("found prod:");
        p_wrp(p->src,src_r);printf("=");
        p_wrp(p->f1->src,src_r);printf(" * ");
        p_wrp(p->f2->src,src_r);printf("\n");
#endif
        poly f1=p->f1->dest;
        poly f2=p->f2->dest;
        if (p->f1->ref>0) f1=p_Copy(f1,dest_r);
        else
        {
          // we own p->f1->dest now (in f1)
          p->f1->dest=NULL;
        }
        if (p->f2->ref>0) f2=p_Copy(f2,dest_r);
        else
        {
          // we own p->f2->dest now (in f2)
          p->f2->dest=NULL;
        }
        maMonomial_Free(p->f1,src_r, dest_r);
        maMonomial_Free(p->f2,src_r, dest_r);
        p->dest=p_Mult_q(f1,f2,dest_r);
      } /* factors : 2 */
      else
      {
        //printf("compute "); p_wrp(p->src,src_r);printf("\n");
        assume((p->f1==NULL) && (p->f2==NULL));
        //if(p->f1!=NULL) { printf(" but f1="); p_wrp(p->f1->src,src_r);printf("\n"); }
        //if(p->f2!=NULL) { printf(" but f2="); p_wrp(p->f2->src,src_r);printf("\n"); }
        // no factorization provided, use the classical method:
        p->dest=maPoly_EvalMon(p->src,src_r,dest_id->m,dest_r);
        //p_wrp(p->dest, dest_r); printf(" is p->dest\n");
      }
    } /* p->dest==NULL */
    // substitute the monomial: go through macoeff
    p->ref -= maPoly_Substitute(p->coeff, p->dest, dest_r);
    //printf("subst done\n");
    if (total_cost)
    {
      assume(TEST_OPT_PROT);
      cost += pDeg(p->src, src_r);
      if (cost > next_print_cost)
      {
        Print("-");
        next_print_cost += total_cost;
      }
    }

    mapoly pp=p;
    p=p->next;
    //p_wrp(pp->src, src_r);
    if (pp->ref<=0)
    {
      //printf(" (%x) killed\n",pp);
      maMonomial_Destroy(pp, src_r, dest_r);
    }
    //else
    //  printf(" (%x) not killed, ref=%d\n",pp,pp->ref);
  }
}
