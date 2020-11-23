#include "misc/auxiliary.h"

#include "misc/intvec.h"
#include "misc/options.h"
#include "polys/monomials/p_polys.h"
#include "polys/matpol.h"
#include "polys/simpleideals.h"
#include "coeffs/longrat.h"
#include "Singular/feOpt.h"
#include "kernel/polys.h"

#define mpz_isNeg(A) ((A)->_mp_size<0)
number nlRInit (long i);

#include "kernel/oswrapper/vspace.h"
#include "kernel/ideals.h"
#include <sys/types.h>
#include <sys/wait.h>

// send a number via a string s
static char *send_number(char *s, number n)
{
  long *d=(long*)s;
  if (SR_HDL(n) & SR_INT)
  {
    *d=(long)n;
    s+=SIZEOF_LONG;
  }
  else
  {
    *d=n->s*2;/* n->s in 0..3: 0..6, use +8 for neg. numbers */
    s+=SIZEOF_LONG;
    if (mpz_isNeg(n->z)) { *d+=8; mpz_neg(n->z,n->z); }
    size_t l;
    d=(long*)s;
    s+=SIZEOF_LONG;
    mpz_export(s,&l,-1,sizeof(mp_limb_t),0,0,n->z);
    *d=l;
    s+=l*sizeof(mp_limb_t);
    if (n->s!=3)
    {
      d=(long*)s;
      s+=SIZEOF_LONG;
      mpz_export(s,&l,-1,sizeof(mp_limb_t),0,0,n->n);
      *d=l;
      s+=l*sizeof(mp_limb_t);
    }
  }
  return s;
}

static char * get_number(char *s, number *n)
{
  // format: last bit 1: imm. number (long)
  //         otherwise: 0,2: size(long) mpz, size(long) mpz
  //                    6: size(long) mpz
  //                    8,10: size(long) -mpz, size(long) mpz
  //                    14: size(long) -mpz
  long *d=(long*)s;
  s+=SIZEOF_LONG;
  if (((*d)&1)==1) // immidiate number
  {
    *n=(number)(*d); 
  }
  else
  {
    *n=nlRInit(0);
    BOOLEAN neg=(*d>=8);
    if (neg) *d-=8;
    (*n)->s=(*d)/2;
    d=(long*)s;
    s+=SIZEOF_LONG;
    size_t l=*d;
    mpz_realloc2((*n)->z,l*sizeof(mp_limb_t)*8);
    mpz_import((*n)->z,l,-1,sizeof(mp_limb_t),0,0,s);
    if (neg) mpz_neg((*n)->z,(*n)->z);
    s+=l*sizeof(mp_limb_t);
    if ((*n)->s!=3)
    {
      d=(long*)s;
      s+=SIZEOF_LONG;
      l=*d;
      mpz_init2((*n)->n,l*sizeof(mp_limb_t)*8);
      mpz_import((*n)->n,l,-1,sizeof(mp_limb_t),0,0,s);
      s+=l*sizeof(mp_limb_t);
    }
  }
  return s;
}

static long size_number(number n)
{
  long ll=SIZEOF_LONG;
  if (SR_HDL(n) & SR_INT)
  {
    return SIZEOF_LONG;
  }
  else
  {
    if (n->s==3) 
    {
      ll+=SIZEOF_LONG*2; /* n->s, mpz size */
      long l=mpz_size1(n->z);
      ll+=l*sizeof(mp_limb_t);
    }
    else
    {
      ll+=SIZEOF_LONG*3; /* n->s, mpz size(n->z) mpz size(n->n)*/
      size_t l=mpz_size1(n->z);
      ll+=l*sizeof(mp_limb_t);
      l=mpz_size1(n->n);
      ll+=l*sizeof(mp_limb_t);
    }
  }
  return ll;
}

static char* send_mon(char *s, poly m, const ring r)
{
  // format: number exp[0..r->ExpL_Size]
  s=send_number(s,p_GetCoeff(m,r));
  memcpy(s,m->exp,r->ExpL_Size*sizeof(long));
  s+=r->ExpL_Size*sizeof(long);
  return s;
}

static char* get_mon(char *s, poly *m, const ring r)
{
  (*m)=p_Init(r);
  s=get_number(s,&p_GetCoeff(*m,r));
  memcpy((*m)->exp,s,r->ExpL_Size*sizeof(long));
  s+=r->ExpL_Size*sizeof(long);
  return s;
}

static long size_mon(poly m, const ring r)
{
  long ll=size_number(p_GetCoeff(m,r));
  ll+=r->ExpL_Size*sizeof(long);
  return ll;
}

static char* send_poly(char *s, int ind, poly p, const ring r)
{
  // format: index(long) length(long) mon...
  //p_Write(p,r);PrintLn();
  long *d=(long*)s;
  *d=ind;
  s+=SIZEOF_LONG;
  long l=pLength(p);
  d=(long*)s;
  *d=l;
  s+=SIZEOF_LONG;
  while(p!=NULL)
  {
    s=send_mon(s,p,r);
    pIter(p);
  }
  return s;
}

static char* get_poly(char *s,int &ind, poly *p,const ring r)
{
  long *d=(long*)s;
  ind=*d;
  s+=SIZEOF_LONG;
  d=(long*)s;
  long l=*d;
  s+=SIZEOF_LONG;
  for(long i=0;i<l;i++)
  {
    poly m;
    s=get_mon(s,&m,r);
    pNext(m)=*p;
    *p=m;
  }
  *p=pReverse(*p);
  return s;
}

static long size_poly(poly p, const ring r)
{
  long l=SIZEOF_LONG*2;
  while(p!=NULL)
  {
    l+=size_mon(p,r);
    pIter(p);
  }
  return l;
}

ideal id_ChineseRemainder_0(ideal *xx, number *q, int rl, const ring r)
{
  int cnt=0;int rw=0; int cl=0;
  // find max. size of xx[.]:
  for(int j=rl-1;j>=0;j--)
  {
    int i=IDELEMS(xx[j])*xx[j]->nrows;
    if (i>cnt) cnt=i;
    if (xx[j]->nrows >rw) rw=xx[j]->nrows; // for lifting matrices
    if (xx[j]->ncols >cl) cl=xx[j]->ncols; // for lifting matrices
  }
  if (rw*cl !=cnt)
  {
    WerrorS("format mismatch in CRT");
    return NULL;
  }
  int cpus=(int)(long)feOptValue(FE_OPT_CPUS);
  if (2*cpus>=cnt) /* at least 2 polys for each process, 
                     or switch to seriell version */
    return id_ChineseRemainder(xx,q,rl,r);
  ideal result=idInit(cnt,xx[0]->rank);
  result->nrows=rw; // for lifting matrices
  result->ncols=cl; // for lifting matrices
  int parent_pid=getpid();
  using namespace vspace;
  vmem_init();
  // Create a queue of int
  VRef<Queue<int> > queue = vnew<Queue<int> >();
  for(int i=cnt-1;i>=0; i--)
  {
    queue->enqueue(i); // the tasks: construct poly p[i]
  }
  for(int i=cpus;i>=0;i--)
  {
    queue->enqueue(-1); // stop sign, one for each child
  }
  // Create a queue of polys
  VRef<Queue<VRef<VString> > > rqueue = vnew<Queue<VRef<VString> > >();
  for (int i=0;i<cpus;i++)
  {
    int pid = fork_process();
    if (pid==0) break; //child
  }
  if (parent_pid!=getpid()) // child ------------------------------------------
  {
    number *x=(number *)omAlloc(rl*sizeof(number));
    poly *p=(poly *)omAlloc(rl*sizeof(poly));
    CFArray inv_cache(rl);
    EXTERN_VAR int n_SwitchChinRem;
    n_SwitchChinRem=1;
    loop
    {
      int ind=queue->dequeue();
      if (ind== -1)
      {
        exit(0);
      }

      for(int j=rl-1;j>=0;j--)
      {
        if(ind>=IDELEMS(xx[j])*xx[j]->nrows) // out of range of this ideal
          p[j]=NULL;
        else
          p[j]=xx[j]->m[ind];
      }
      poly res=p_ChineseRemainder(p,x,q,rl,inv_cache,r);
      long l=size_poly(res,r);
      //printf("size: %ld kB\n",(l+1023)/1024);
      VRef<VString> msg = vstring(l+1);
      char *s=(char*)msg->str();
      send_poly(s,ind,res,r);
      rqueue->enqueue(msg);
      if (TEST_OPT_PROT) printf(".");
    }
  }
  else // parent ---------------------------------------------------
  {
    if (TEST_OPT_PROT) printf("%d childs created\n",cpus);
    VRef<VString> msg;
    while(cnt>0)
    {
      msg=rqueue->dequeue();
      char *s=(char*)msg->str();
      int ind;
      poly p=NULL;
      get_poly(s,ind,&p,r);
      //printf("got res[%d]\n",ind);
      result->m[ind]=p;
      msg.free();
      cnt--;
    }
    // removes queues
    queue.free();
    rqueue.free();
    vmem_deinit();
  }
  return result;
}

ideal id_Farey_0(ideal x, number N, const ring r)
{
  int cnt=IDELEMS(x)*x->nrows;
  int cpus=(int)(long)feOptValue(FE_OPT_CPUS);
  if (2*cpus>=cnt) /* at least 2 polys for each process, 
                     or switch to seriell version */
    return id_Farey(x,N,r);
  ideal result=idInit(cnt,x->rank);
  result->nrows=x->nrows; // for lifting matrices
  result->ncols=x->ncols; // for lifting matrices

  int parent_pid=getpid();
  using namespace vspace;
  vmem_init();
  // Create a queue of int
  VRef<Queue<int> > queue = vnew<Queue<int> >();
  for(int i=cnt-1;i>=0; i--)
  {
    queue->enqueue(i); // the tasks: construct poly p[i]
  }
  for(int i=cpus;i>=0;i--)
  {
    queue->enqueue(-1); // stop sign, one for each child
  }
  // Create a queue of polys
  VRef<Queue<VRef<VString> > > rqueue = vnew<Queue<VRef<VString> > >();
  for (int i=0;i<cpus;i++)
  {
    int pid = fork_process();
    if (pid==0) break; //child
  }
  if (parent_pid!=getpid()) // child ------------------------------------------
  {
    loop
    {
      int ind=queue->dequeue();
      if (ind== -1)
      {
        exit(0);
      }

      poly res=p_Farey(x->m[ind],N,r);
      long l=size_poly(res,r);
      VRef<VString> msg = vstring(l+1);
      char *s=(char*)msg->str();
      send_poly(s,ind,res,r);
      rqueue->enqueue(msg);
      if (TEST_OPT_PROT) printf(".");
    }
  }
  else // parent ---------------------------------------------------
  {
    if (TEST_OPT_PROT) printf("%d childs created\n",cpus);
    VRef<VString> msg;
    while(cnt>0)
    {
      msg=rqueue->dequeue();
      char *s=(char*)msg->str();
      int ind;
      poly p=NULL;
      get_poly(s,ind,&p,r);
      //printf("got res[%d]\n",ind);
      result->m[ind]=p;
      msg.free();
      cnt--;
    }
    // removes queues
    queue.free();
    rqueue.free();
    vmem_deinit();
  }
  return result;
}

void test_n(poly n)
{
  p_Write(n,currRing);
  char *buf=(char*)omAlloc0(2048*1000);
  int ll=size_poly(n,currRing);
  printf("size: %d\n",ll);
  char *s=send_poly(buf,12345,n,currRing);
  printf("send len: %d\n",(int)(s-buf));
  long *d=(long*)buf;
  for(int i=0;i<=ll/SIZEOF_LONG;i++) printf("%ld ",d[i]);
  printf("\n");
  n=NULL;
  s=get_poly(buf,ll,&n,currRing);
  printf("read len: %d\n",(int)(s-buf));
  Print(":index: %d\n",ll);
  p_Write(n,currRing);
  PrintLn();
  omFree(buf);
}
