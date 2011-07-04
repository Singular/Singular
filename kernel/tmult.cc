/*******************************************************************
 *  Computer Algebra System SINGULAR
 *
 *  tmult.cc: p_Mult_nn with pthreads - experimental
 *
 *  Version: $Id$
 *******************************************************************/
#include <kernel/mod2.h>
#include <kernel/structs.h>
#include <kernel/numbers.h>
#include <kernel/polys.h>
#include <kernel/longrat.h>
#ifdef SI_THREADS
#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>

#define NUM_THREADS     8
#define THREAD_MIN_LENGTH 10*NUM_THREADS

struct p_Mult_nn_thread_data
{
   int  thread_id;
   poly p;
   number n;
   ring r;
};


void*  p_Mult_nn_doMult(void *threadarg)
{
  struct p_Mult_nn_thread_data *my_data;
  my_data = (struct p_Mult_nn_thread_data *) threadarg;
  //long taskid = my_data->thread_id;
  poly p = my_data->p;
  number n = my_data->n;
  ring r = my_data->r;
  while (1)
  {
    //if (p==NULL) return NULL;
    if (p==NULL) pthread_exit(NULL);
    nlInpMult(pGetCoeff(p), n,r);
    for (int i=0;i<NUM_THREADS;i++)
    {
      pIter(p);
      if (p==NULL) pthread_exit(NULL);
      //if (p==NULL) return NULL;
    }
  }
  return NULL;
}

static inline int pLengthOrMore(poly p, int m)
{
  int l;
  for(l=0;(p!=NULL) && (l<=m); l++) pIter(p);
  return l;
}

extern "C" poly p_Mult_nn__FieldQ_LengthGeneral_OrdGeneral(poly,const number,const ring);

poly p_Mult_nn_pthread(poly p, const number n, const ring r)
{
  if (p==NULL) return NULL;
  poly q=p;
  if ((nlSize(n)>2) && (pLengthOrMore(q,THREAD_MIN_LENGTH)>=THREAD_MIN_LENGTH))
  {
    pthread_t threads[NUM_THREADS];
    struct p_Mult_nn_thread_data thread_data_array[NUM_THREADS];
    pthread_attr_t attr;
    /* Initialize and set thread detached attribute */
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

    int rc;
    int t;
    for(t=0; t<NUM_THREADS; t++)
    {
      //printf("In main: creating thread %ld\n", t);
      thread_data_array[t].thread_id = t;
      thread_data_array[t].p = p;
      thread_data_array[t].n = n;
      thread_data_array[t].r = r;
      //p_Mult_nn_doMult(&(thread_data_array[t]));
      rc = pthread_create(&threads[t], &attr, p_Mult_nn_doMult, 
        (void *) &thread_data_array[t]);
      if (rc)
      {
        printf("ERROR; return code from pthread_create() is %d\n", rc);
        exit(-1);
      }
      pIter(p);
      if (p==NULL) break;
    }
    /* Free attribute and wait for the other threads */
    pthread_attr_destroy(&attr);
    for(t=NUM_THREADS-1; t>=0; t--)
    {
      void *status;

      rc = pthread_join(threads[t], &status);
      if (rc)
      {
        printf("ERROR; return code from pthread_join() is %d\n", rc);
        exit(-1);
      }
    }
 
    return q;
  }
  else
  {
    return p_Mult_nn__FieldQ_LengthGeneral_OrdGeneral(p,n,r);
  }
}
#endif
