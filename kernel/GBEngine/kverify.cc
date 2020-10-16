#include "kernel/mod2.h"

#include "misc/mylimits.h"
#include "misc/options.h"
#include "kernel/ideals.h"
#include "kernel/polys.h"
#include "polys/monomials/ring.h"
#include "kernel/GBEngine/kutil.h"
#include <stdlib.h>
#include <string.h>

BOOLEAN kVerify(ideal F, ideal Q)
{
  kStrategy strat=new skStrategy;
  strat->ak = id_RankFreeModule(F,currRing);
  strat->kModW=kModW=NULL;
  strat->kHomW=kHomW=NULL;
  initBuchMoraCrit(strat); /*set Gebauer, honey, sugarCrit*/
  initBuchMoraPos(strat);
  initBba(strat);
  initBuchMora(F, Q,strat);
  /*initBuchMora:*/
    strat->tail = pInit();
    /*- set s -*/
    strat->sl = -1;
    /*- set L -*/
    strat->Lmax = ((IDELEMS(F)+setmaxLinc-1)/setmaxLinc)*setmaxLinc;
    strat->Ll = -1;
    strat->L = initL(strat->Lmax);
    /*- set B -*/
    strat->Bmax = setmaxL;
    strat->Bl = -1;
    strat->B = initL();
    /*- set T -*/
    strat->tl = -1;
    strat->tmax = setmaxT;
    strat->T = initT();
    strat->R = initR();
    strat->sevT = initsevT();
    /*- init local data struct.---------------------------------------- -*/
    strat->P.ecart=0;
    strat->P.length=0;
    strat->P.pLength=0;
    initS(F, Q,strat); /*sets also S, ecartS, fromQ */
    strat->fromT = FALSE;
    strat->noTailReduction = FALSE;
  /*----------------------------------------------------------------------*/
  /* build pairs */
  if (strat->fromQ!=NULL)
  {
    for(int i=1; i<=strat->sl;i++)
    {
      initenterpairs(strat->S[i],i-1,0,strat->fromQ[i],strat);
    }
  }
  else
  {
    for(int i=1; i<=strat->sl;i++)
    {
      initenterpairs(strat->S[i],i-1,0,FALSE,strat);
    }
  }
  printf("%d pairs created\n",strat->Ll+1);
  if (TEST_OPT_DEBUG) messageSets(strat);
  /*---------------------------------------------------------------------*/
  #if 1 /* seriell */
  BOOLEAN all_okay=TRUE;
  for(int i=strat->Ll;i>=0; i--)
  {
  /* spolys */
    int red_result=1;
    /* picks the last element from the lazyset L */
    strat->P = strat->L[i];
    if (pNext(strat->P.p) == strat->tail)
    {
      // deletes the short spoly
      pLmFree(strat->P.p);
      strat->P.p = NULL;
      poly m1 = NULL, m2 = NULL;
      kCheckSpolyCreation(&(strat->P), strat, m1, m2);
      ksCreateSpoly(&(strat->P), NULL, strat->use_buckets,
                    strat->tailRing, m1, m2, strat->R);
    }
    if ((strat->P.p == NULL) && (strat->P.t_p == NULL))
    {
      red_result = 0;
    }
    else
    {
      int sl=strat->sl;
      strat->P.GetP();
      poly p=redNF(strat->P.p,sl,TRUE,strat);
      if (p==NULL) red_result=0;
      else
      {
      printf("p: ");p_wrp(p,currRing, currRing); printf("\n");
      }
    }
    if (red_result!=0)
    {
      printf("fail: %d, result: %d\n",i,red_result);
      all_okay=FALSE;
    }
  }
  return all_okay;
  #endif
  /*---------------------------------------------------------------------*/
  #if 0
  using namespace vspace;
  vmem_init();
  // Create a semaphore
  VRef<Semaphore> sem = vnew<Semaphore>(0);
  // Create a queue of int
  VRef<Queue<int> > queue = vnew<Queue<int> >();
  VRef<Queue<int> > rqueue = vnew<Queue<int> >();
  pid_t pid[MAX_PROCESS];
  pid[0] = fork_process();
  if (pid[0] > 0) // parent
  {
    for(int i=strat->Ll;i>=0; i--)
    {
     queue->enqueue(i);
    }
    for(int i=MAX_PROCESS-1;i>=0;i--)
    {
      queue->enqueue(-1); // stop sign
    }
    pid[1]=fork_process();
    if (pid[1] > 0) // parent
    {
      pid[2]=fork_process();
      if (pid[2] > 0) // parent
      {
        pid[3]=fork_process();
        if(pid[3]>0)
        {
          // wait for all process to stop:
          // each process sends an 0 at end
          int res;
          int remaining_childs=4;
          while(remaining_childs>0)
          {
            res=rqueue->dequeue();
            if (res==0) // a child finished
            {
              waitpid(-1,NULL,0); // ? see sig_chld_hdl
              remaining_childs--;
            }
            else if (res==1) // not a GB - clean up and return 0
            {
              // clean queue:
              int dummy;
              do
              {
                dummy=queue->dequeue();
              } while (dummy>=0);
              while(remaining_childs>0)
              {
                waitpid(-1,NULL,0);
                remaining_childs--;
              }
              queue.free();
              rqueue.free();
              return FALSE;
            }
          }
        }
      }
    }
  }
/* ------------------------------------------------ */
/* child */
  BOOLEAN all_okay=TRUE;

  int ind;
  do
  {
    ind=queue->dequeue();
    if (ind==0)
    {
      rqueue->enqueue(0);
      exit(0);
    }
    /* spolys */
    int red_result=1;
    /* picks the element from the lazyset L */
    LObject P;
    P = strat->L[ind];
    if (pNext(P.p) == strat->tail)
    {
      // deletes the short spoly
      pLmFree(P.p);
      P.p = NULL;
      poly m1 = NULL, m2 = NULL;
      kCheckSpolyCreation(&P, strat, m1, m2);
      ksCreateSpoly(&P, NULL, strat->use_buckets,
                    strat->tailRing, m1, m2, strat->R);
    }
    if ((P.p == NULL) && (P.t_p == NULL))
    {
      red_result = 0;
    }
    else
    {
      int sl=strat->sl;
      P.GetP();
      poly p=redNF(P.p,sl,TRUE,strat);
      if (p==NULL) red_result=0;
      else
      {
      printf("p: ");p_wrp(p,currRing, currRing); printf("\n");
      }
    }
    if (red_result!=0)
    {
      printf("fail: %d, result: %d\n",i,red_result);
      rqueue->enqueue(1);
      exit(0);
    }
  }
  #endif
}
