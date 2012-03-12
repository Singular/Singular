/****************************************
 * Computer Algebra System SINGULAR     *
 ****************************************/
/***************************************************************
 * File:    ssiLink.h
 *  Purpose: declaration of sl_link routines for ssi
 ***************************************************************/
#include <stdio.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <signal.h>
#include <sys/types.h>          /* for portability */
#include <sys/select.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <ctype.h>   /*for isdigit*/
#include <netdb.h>
#include <sys/wait.h>



#include <kernel/mod2.h>
// #include "mod2.h"

#include <Singular/tok.h>
#include <Singular/ipid.h>
#include <omalloc/omalloc.h>
#include <kernel/ring.h>
#include <kernel/matpol.h>
#include <kernel/ideals.h>
#include <kernel/polys.h>
#include <kernel/longrat.h>
#include <kernel/ideals.h>
#include <kernel/intvec.h>
#include <kernel/options.h>
#include <kernel/timer.h>
#include <Singular/subexpr.h>
#include <Singular/silink.h>
#include <Singular/cntrlc.h>
#include <Singular/lists.h>
#include <Singular/blackbox.h>
#include <Singular/ssiLink.h>

#ifdef HAVE_MPSR
#include <Singular/mpsr.h>
#endif

#ifdef HAVE_SIMPLEIPC
#include <Singular/simpleipc.h>
#endif
//#if (_POSIX_C_SOURCE >= 200112L) || (_XOPEN_SOURCE >= 600)
//#define HAVE_PSELECT
//#endif

#define SSI_VERSION 3

typedef struct
{
  FILE *f_read;
  FILE *f_write;
  ring r;
  pid_t pid; /* only valid for fork/tcp mode*/
  int fd_read,fd_write; /* only valid for fork/tcp mode*/
  char level;
  char ungetc_buf; /* status sets to !=0, if ungetc was used, ssiRead* set to 0*/
  char send_quit_at_exit;

} ssiInfo;


link_list ssiToBeClosed=NULL;
sigset_t ssi_sigmask;
sigset_t ssi_oldmask;
#define SSI_BLOCK_CHLD sigprocmask(SIG_SETMASK, &ssi_sigmask, &ssi_oldmask)
#define SSI_UNBLOCK_CHLD sigprocmask(SIG_SETMASK, &ssi_oldmask, NULL)

// the helper functions:
void ssiSetCurrRing(const ring r)
{
  if (!rEqual(r,currRing,1))
  {
    char name[20];
    int nr=0;
    do
    { sprintf(name,"ssiRing%d",nr); nr++; }
    while(IDROOT->get(name, 0)!=NULL);
    idhdl h=enterid(omStrDup(name),0,RING_CMD,&IDROOT,FALSE);
    IDRING(h)=r;
    r->ref++;
    rSetHdl(h);
  }
}
// the implementation of the functions:
void ssiWriteInt(ssiInfo *d,const int i)
{
  SSI_BLOCK_CHLD;
  fprintf(d->f_write,"%d ",i);
  SSI_UNBLOCK_CHLD;
  //if (d->f_debug!=NULL) fprintf(d->f_debug,"int: %d ",i);
}

void ssiWriteString(ssiInfo *d,const char *s)
{
  SSI_BLOCK_CHLD;
  fprintf(d->f_write,"%d %s ",strlen(s),s);
  SSI_UNBLOCK_CHLD;
  //if (d->f_debug!=NULL) fprintf(d->f_debug,"stringi: %d \"%s\" ",strlen(s),s);
}


void ssiWriteBigInt(const ssiInfo *d, const number n)
{
  // syntax is as follows:
  // case 2 Q:     3 4 <int>
  //        or     3 3 <mpz_t nominator>
  SSI_BLOCK_CHLD;
  if(SR_HDL(n) & SR_INT)
  {
    fprintf(d->f_write,"4 %ld ",SR_TO_INT(n));
    //if (d->f_debug!=NULL) fprintf(d->f_debug,"bigint: short \"%ld\" ",SR_TO_INT(n));
  }
  else if (n->s==3)
  {
    fputs("3 ",d->f_write);
    mpz_out_str(d->f_write,10,n->z);
    fputc(' ',d->f_write);
    //gmp_fprintf(d->f_write,"3 %Zd ",n->z);
    //if (d->f_debug!=NULL) gmp_fprintf(d->f_debug,"bigint: gmp \"%Zd\" ",n->z);
  }
  else WerrorS("illiegal bigint");
  SSI_UNBLOCK_CHLD;
}

void ssiWriteNumber(const ssiInfo *d, const number n)
{
  // syntax is as follows:
  // case 1 Z/p:   3 <int>
  // case 2 Q:     3 4 <int>
  //        or     3 0 <mpz_t nominator> <mpz_t denominator>
  //        or     3 1  dto.
  //        or     3 3 <mpz_t nominator>
  //        or     3 5 <mpz_t raw nom.> <mpz_t raw denom.>
  //        or     3 6 <mpz_t raw nom.> <mpz_t raw denom.>
  //        or     3 7 <mpz_t raw nom.>
  SSI_BLOCK_CHLD;
  if(rField_is_Zp(d->r))
  {
    fprintf(d->f_write,"%d ",(int)(long)n);
    //if (d->f_debug!=NULL) fprintf(d->f_debug,"number: \"%ld\" ",(int)(long)n);
  }
  else if (rField_is_Q(d->r))
  {
    if(SR_HDL(n) & SR_INT)
    {
      fprintf(d->f_write,"4 %ld ",SR_TO_INT(n));
      //if (d->f_debug!=NULL) fprintf(d->f_debug,"number: short \"%ld\" ",SR_TO_INT(n));
    }
    else if (n->s<2)
    {
      //gmp_fprintf(d->f_write,"%d %Zd %Zd ",n->s,n->z,n->n);
      fprintf(d->f_write,"%d ",n->s+5);
      mpz_out_str (d->f_write,32, n->z);
      fputc(' ',d->f_write);
      mpz_out_str (d->f_write,32, n->n);
      fputc(' ',d->f_write);

      //if (d->f_debug!=NULL) gmp_fprintf(d->f_debug,"number: s=%d gmp/gmp \"%Zd %Zd\" ",n->s,n->z,n->n);
    }
    else /*n->s==3*/
    {
      //gmp_fprintf(d->f_write,"3 %Zd ",n->z);
      fputs("8 ",d->f_write);
      mpz_out_str (d->f_write,32, n->z);
      fputc(' ',d->f_write);

      //if (d->f_debug!=NULL) gmp_fprintf(d->f_debug,"number: gmp \"%Zd\" ",n->z);
    }
  }
  else WerrorS("coeff field not implemented");
  SSI_UNBLOCK_CHLD;
}

void ssiWriteRing(ssiInfo *d,const ring r)
{
  /* 5 <ch> <N> <l1> <v1> ...<lN> <vN> <number of orderings> <ord1> <block0_1> <block1_1> .... */
  if (d->r!=NULL) rKill(d->r);
  d->r=r;
  d->r->ref++;
  SSI_BLOCK_CHLD;
  fprintf(d->f_write,"%d %d ",r->ch,r->N);

  int i;
  for(i=0;i<r->N;i++)
  {
    fprintf(d->f_write,"%d %s ",strlen(r->names[i]),r->names[i]);
  }
  /* number of orderings:*/
  i=0;
  while (r->order[i]!=0) i++;
  fprintf(d->f_write,"%d ",i);
  /* each ordering block: */
  i=0;
  while(r->order[i]!=0)
  {
    fprintf(d->f_write,"%d %d %d ",r->order[i],r->block0[i], r->block1[i]);
    switch(r->order[i])
    {
      case ringorder_a:
      case ringorder_wp:
      case ringorder_Wp:
      case ringorder_ws:
      case ringorder_Ws:
      case ringorder_aa:
      {
        int ii;
        for(ii=r->block0[i];ii<=r->block1[i];ii++)
          fprintf(d->f_write,"%d ",r->wvhdl[i][ii-r->block0[i]]);
      }
      break;

      case ringorder_a64:
      case ringorder_M:
      case ringorder_L:
      case ringorder_IS:
        Werror("ring oder not implemented for ssi:%d",r->order[i]);
        break;

      default: break;
    }
    i++;
  }
  SSI_UNBLOCK_CHLD;
}

void ssiWritePoly(ssiInfo *d, int typ, poly p)
{
  SSI_BLOCK_CHLD;
  fprintf(d->f_write,"%d ",pLength(p));//number of terms
  SSI_UNBLOCK_CHLD;
  int i;

  while(p!=NULL)
  {
    ssiWriteNumber(d,pGetCoeff(p));
    //nWrite(fich,pGetCoeff(p));
    SSI_BLOCK_CHLD;
    fprintf(d->f_write,"%ld ",p_GetComp(p,d->r));//component

    for(int j=1;j<=rVar(d->r);j++)
    {
      fprintf(d->f_write,"%ld ",p_GetExp(p,j,d->r ));//x^j
    }
    pIter(p);
    SSI_UNBLOCK_CHLD;
  }
}

void ssiWriteIdeal(ssiInfo *d, int typ,ideal I)
{
   // syntax: 7 # of elements <poly 1> <poly2>.....
   // syntax: 8 <rows> <cols> <poly 1> <poly2>.....
   matrix M=(matrix)I;
   SSI_BLOCK_CHLD;
   if (typ==MATRIX_CMD)
        fprintf(d->f_write,"%d %d ", MATROWS(M),MATCOLS(M));
   else
     fprintf(d->f_write,"%d ",IDELEMS(I));
    SSI_UNBLOCK_CHLD;

   int i;
   int tt;
   if (typ==MODUL_CMD) tt=VECTOR_CMD;
   else                tt=POLY_CMD;

   for(i=0;i<IDELEMS(I);i++)
   {
     ssiWritePoly(d,tt,I->m[i]);
   }
}
void ssiWriteCommand(si_link l, command D)
{
  ssiInfo *d=(ssiInfo*)l->data;
  // syntax: <num ops> <operation> <op1> <op2> ....
  SSI_BLOCK_CHLD;
  fprintf(d->f_write,"%d %d ",D->argc,D->op);
  SSI_UNBLOCK_CHLD;
  if (D->argc >0) ssiWrite(l, &(D->arg1));
  if (D->argc < 4)
  {
    if (D->argc >1) ssiWrite(l, &(D->arg2));
    if (D->argc >2) ssiWrite(l, &(D->arg3));
  }
}

void ssiWriteProc(ssiInfo *d,procinfov p)
{
  ssiWriteString(d,p->data.s.body);
}

void ssiWriteList(si_link l,lists dd)
{
  ssiInfo *d=(ssiInfo*)l->data;
  SSI_BLOCK_CHLD;
  fprintf(d->f_write,"%d ",dd->nr+1);
  SSI_UNBLOCK_CHLD;
  int i;
  for(i=0;i<=dd->nr;i++)
  {
    ssiWrite(l,&(dd->m[i]));
  }
}
void ssiWriteIntvec(ssiInfo *d,intvec * v)
{
  SSI_BLOCK_CHLD;
  fprintf(d->f_write,"%d ",v->length());
  int i;
  for(i=0;i<v->length();i++)
  {
    fprintf(d->f_write,"%d ",(*v)[i]);
  }
  SSI_UNBLOCK_CHLD;
}

char *ssiReadString(ssiInfo *d)
{
  char *buf;
  int l;
  SSI_BLOCK_CHLD;
  fscanf(d->f_read,"%d ",&l);
  SSI_UNBLOCK_CHLD;
  buf=(char*)omAlloc(l+1);
  SSI_BLOCK_CHLD;
  fread(buf,1,l,d->f_read);
  SSI_UNBLOCK_CHLD;
  buf[l]='\0';
  return buf;
}

int ssiReadInt(FILE *fich)
{
  int d;
  SSI_BLOCK_CHLD;
  fscanf(fich,"%d",&d);
  SSI_UNBLOCK_CHLD;
  return d;
}

number ssiReadBigInt(ssiInfo *d)
{
  int sub_type=-1;
  SSI_BLOCK_CHLD;
  fscanf(d->f_read,"%d",&sub_type);
  SSI_UNBLOCK_CHLD;
  switch(sub_type)
  {
   case 3:
     {// read int or mpz_t or mpz_t, mpz_t
       number n=nlRInit(0);
       SSI_BLOCK_CHLD;
       mpz_inp_str(n->z,d->f_read,0);
       SSI_UNBLOCK_CHLD;
       n->s=sub_type;
       return n;
     }
   case 4:
     {
       int dd;
       SSI_BLOCK_CHLD;
       fscanf(d->f_read,"%d",&dd);
       SSI_UNBLOCK_CHLD;
       return INT_TO_SR(dd);
     }
   default:
       Werror("error in reading bigint: invalid subtype %d",sub_type);
       return NULL;
   }
}

number ssiReadNumber(ssiInfo *d)
{
  if (rField_is_Q(d->r))
  {
     int sub_type=-1;
     SSI_BLOCK_CHLD;
     fscanf(d->f_read,"%d",&sub_type);
     SSI_UNBLOCK_CHLD;
     switch(sub_type)
     {
     case 0:
     case 1:
       {// read mpz_t, mpz_t
         number n=nlRInit(0);
         mpz_init(n->n);
         SSI_BLOCK_CHLD;
         gmp_fscanf(d->f_read,"%Zd %Zd",n->z,n->n);
         SSI_UNBLOCK_CHLD;
         n->s=sub_type;
         return n;
       }

     case 3:
       {// read mpz_t
         number n=nlRInit(0);
         SSI_BLOCK_CHLD;
         gmp_fscanf(d->f_read,"%Zd",n->z);
         SSI_UNBLOCK_CHLD;
         n->s=3; /*sub_type*/
         return n;
       }
     case 4:
       {
         int dd;
         SSI_BLOCK_CHLD;
         fscanf(d->f_read,"%d",&dd);
         SSI_UNBLOCK_CHLD;
         return INT_TO_SR(dd);
       }
     case 5:
     case 6:
       {// read raw mpz_t, mpz_t
         number n=nlRInit(0);
         mpz_init(n->n);
         SSI_BLOCK_CHLD;
         mpz_inp_str (n->z, d->f_read, 32);
         mpz_inp_str (n->n, d->f_read, 32);
         SSI_UNBLOCK_CHLD;
         n->s=sub_type-5;
         return n;
       }
     case 8:
       {// read raw mpz_t
         number n=nlRInit(0);
         SSI_BLOCK_CHLD;
         mpz_inp_str (n->z, d->f_read, 32);
         SSI_UNBLOCK_CHLD;
         n->s=sub_type=3; /*subtype-5*/
         return n;
       }

     default: Werror("error in reading number: invalid subtype %d",sub_type);
              return NULL;
     }
  }
  else if (rField_is_Zp(d->r))
  {
    // read int
    int dd;
    SSI_BLOCK_CHLD;
    fscanf(d->f_read,"%d",&dd);
    SSI_UNBLOCK_CHLD;
    return (number)dd;
  }
  else Werror("coeffs not implemented");
  return NULL;
}

ring ssiReadRing(ssiInfo *d)
{
/* syntax is <ch> <N> <l1> <v1> ...<lN> <vN> <number of orderings> <ord1> <block0_1> <block1_1> .... */
  int ch, N,i,l;
  char **names;
  SSI_BLOCK_CHLD;
  fscanf(d->f_read,"%d %d ",&ch,&N);
  SSI_UNBLOCK_CHLD;
  names=(char**)omAlloc(N*sizeof(char*));
  for(i=0;i<N;i++)
  {
    names[i]=ssiReadString(d);
  }
  // read the orderings:
  int num_ord; // number of orderings
  SSI_BLOCK_CHLD;
  fscanf(d->f_read,"%d",&num_ord);
  SSI_UNBLOCK_CHLD;
  int *ord=(int *)omAlloc0((num_ord+1)*sizeof(int));
  int *block0=(int *)omAlloc0((num_ord+1)*sizeof(int));
  int *block1=(int *)omAlloc0((num_ord+1)*sizeof(int));
  SSI_BLOCK_CHLD;
  int **wvhdl=(int**)omAlloc0((num_ord+1)*sizeof(int*));
  for(i=0;i<num_ord;i++)
  {
    fscanf(d->f_read,"%d %d %d",&ord[i],&block0[i],&block1[i]);
    switch(ord[i])
    {
      case ringorder_a:
      case ringorder_wp:
      case ringorder_Wp:
      case ringorder_ws:
      case ringorder_Ws:
      case ringorder_aa:
      {
        wvhdl[i]=(int*)omAlloc((block1[i]-block0[i]+1)*sizeof(int));
        int ii;
        for(ii=block0[i];ii<=block1[i];ii++)
          fscanf(d->f_read,"%d",&(wvhdl[i][ii-block0[i]]));
      }
      break;

      case ringorder_a64:
      case ringorder_M:
      case ringorder_L:
      case ringorder_IS:
        Werror("ring oder not implemented for ssi:%d",ord[i]);
        break;

      default: break;
    }
  }
  SSI_UNBLOCK_CHLD;
  return rDefault(ch,N,names,num_ord,ord,block0,block1,wvhdl);
}

poly ssiReadPoly(ssiInfo *D)
{
// < # of terms> < term1> < .....
  int n,i,l;
  n=ssiReadInt(D->f_read);
  //Print("poly: terms:%d\n",n);
  poly p;
  int j;
  j=0;
  poly ret=NULL;
  poly prev=NULL;
  for(l=0;l<n;l++) // read n terms
  {
// coef,comp.exp1,..exp N
    p=p_Init(D->r);
    pGetCoeff(p)=ssiReadNumber(D);
    int d;
    SSI_BLOCK_CHLD;
    fscanf(D->f_read,"%d",&d);
    SSI_UNBLOCK_CHLD;
    p_SetComp(p,d,D->r);
    for(i=1;i<=rVar(D->r);i++)
    {
      SSI_BLOCK_CHLD;
      fscanf(D->f_read,"%d",&d);
      SSI_UNBLOCK_CHLD;
      p_SetExp(p,i,d,D->r);
    }
    p_Setm(p,D->r);
    p_Test(p,D->r);
    if (ret==NULL) ret=p;
    else           pNext(prev)=p;
    prev=p;
 }
 return ret;
}

ideal ssiReadIdeal(ssiInfo *d)
{
  int n,i;
  ideal I;
  SSI_BLOCK_CHLD;
  fscanf(d->f_read,"%d",&n);
  SSI_UNBLOCK_CHLD;
  I=idInit(n,1);
  for(i=0;i<IDELEMS(I);i++) // read n terms
  {
    I->m [i]=ssiReadPoly(d);
  }
  return I;
}

matrix ssiReadMatrix(ssiInfo *d)
{
  int n,m,i,j;
  SSI_BLOCK_CHLD;
  fscanf(d->f_read,"%d %d",&m,&n);
  SSI_UNBLOCK_CHLD;
  matrix M=mpNew(m,n);
  poly p;
  for(int i=1;i<=MATROWS(M);i++)
    for(int j=1;j<=MATCOLS(M);j++)
    {
      p=ssiReadPoly(d);
      MATELEM(M,i,j)=p;
    }
  return M;
}

command ssiReadCommand(si_link l)
{
  ssiInfo *d=(ssiInfo*)l->data;
  // syntax: <num ops> <operation> <op1> <op2> ....
  command D=(command)omAlloc0(sizeof(*D));
  int argc,op;
  SSI_BLOCK_CHLD;
  fscanf(d->f_read,"%d %d",&argc,&op);
  SSI_UNBLOCK_CHLD;
  D->argc=argc; D->op=op;
  leftv v;
  if (argc >0)
  {
    v=ssiRead1(l);
    memcpy(&(D->arg1),v,sizeof(*v));
    omFreeBin(v,sleftv_bin);
  }
  if (argc <4)
  {
    if (D->argc >1)
    {
      v=ssiRead1(l);
      memcpy(&(D->arg2),v,sizeof(*v));
      omFreeBin(v,sleftv_bin);
    }
    if (D->argc >2)
    {
      v=ssiRead1(l);
      memcpy(&(D->arg3),v,sizeof(*v));
      omFreeBin(v,sleftv_bin);
    }
  }
  else
  {
    leftv prev=&(D->arg1);
    argc--;
    while(argc >0)
    {
      v=ssiRead1(l);
      prev->next=v;
      prev=v;
      argc--;
    }
  }
  return D;
}

procinfov ssiReadProc(ssiInfo *d)
{
  char *s=ssiReadString(d);
  procinfov p=(procinfov)omAlloc0Bin(procinfo_bin);
  p->language=LANG_SINGULAR;
  p->libname=omStrDup("");
  p->procname=omStrDup("");
  p->data.s.body=s;
  return p;
}
lists ssiReadList(si_link l)
{
  ssiInfo *d=(ssiInfo*)l->data;
  int nr;
  SSI_BLOCK_CHLD;
  fscanf(d->f_read,"%d",&nr);
  SSI_UNBLOCK_CHLD;
  lists L=(lists)omAlloc(sizeof(*L));
  L->Init(nr);

  int i;
  leftv v;
  for(i=0;i<nr;i++)
  {
    v=ssiRead1(l);
    memcpy(&(L->m[i]),v,sizeof(*v));
    omFreeBin(v,sleftv_bin);
  }
  return L;
}
intvec* ssiReadIntvec(ssiInfo *d)
{
  int nr;
  SSI_BLOCK_CHLD;
  fscanf(d->f_read,"%d",&nr);
  SSI_UNBLOCK_CHLD;
  intvec *v=new intvec(nr);
  SSI_BLOCK_CHLD;
  for(int i=0;i<nr;i++)
  {
    fscanf(d->f_read,"%d",&((*v)[i]));
  }
  SSI_UNBLOCK_CHLD;
  return v;
}

void ssiReadBlackbox(leftv res, si_link l)
{
  ssiInfo *d=(ssiInfo*)l->data;
  int throwaway;
  SSI_BLOCK_CHLD;
  fscanf(d->f_read,"%d ",&throwaway);
  SSI_UNBLOCK_CHLD;
  char *name=ssiReadString(d);
  int tok;
  blackboxIsCmd(name,tok);
  if (tok>MAX_TOK)
  {
    blackbox *b=getBlackboxStuff(tok);
    res->rtyp=tok;
    b->blackbox_deserialize(&b,&(res->data),l);
  }
  else
  {
    Werror("blackbox %s not found",name);
  }
}

//**************************************************************************/

BOOLEAN ssiOpen(si_link l, short flag, leftv u)
{
  if (l!=NULL)
  {
    const char *mode;
    ssiInfo *d=(ssiInfo*)omAlloc0(sizeof(ssiInfo));
    sigprocmask(SIG_SETMASK, NULL, &ssi_sigmask);
    sigaddset(&ssi_sigmask, SIGCHLD);
    if (flag & SI_LINK_OPEN)
    {
      if (l->mode[0] != '\0' && (strcmp(l->mode, "r") == 0))
        flag = SI_LINK_READ;
      else flag = SI_LINK_WRITE;
    }

    if (flag == SI_LINK_READ) mode = "r";
    else if (strcmp(l->mode, "w") == 0) mode = "w";
    else if (strcmp(l->mode, "fork") == 0) mode = "fork";
    else if (strcmp(l->mode, "tcp") == 0) mode = "tcp";
    else if (strcmp(l->mode, "connect") == 0) mode = "connect";
    else mode = "a";


    SI_LINK_SET_OPEN_P(l, flag);
    l->data=d;
    omFree(l->mode);
    l->mode = omStrDup(mode);

    if (l->name[0] == '\0')
    {
      if (strcmp(mode,"fork")==0)
      {
        link_list n=(link_list)omAlloc(sizeof(link_struct));
        n->u=u;
        n->l=l;
        n->next=(void *)ssiToBeClosed;
        ssiToBeClosed=n;

        int pc[2];
        int cp[2];
        pipe(pc);
        pipe(cp);
        pid_t pid=fork();
        if (pid==0) /*fork: child*/
        {
          link_list hh=(link_list)ssiToBeClosed->next;
          /* we know: l is the first entry in ssiToBeClosed-list */
          while(hh!=NULL)
          {
            ssiInfo *dd=(ssiInfo*)hh->l->data;
            fclose(dd->f_read);
            fclose(dd->f_write);
            if (dd->r!=NULL) rKill(dd->r);
            omFreeSize((ADDRESS)dd,(sizeof *dd));
            hh->l->data=NULL;
            SI_LINK_SET_CLOSE_P(hh->l);
            link_list nn=(link_list)hh->next;
            omFree(hh);
            hh=nn;
          }
          ssiToBeClosed->next=NULL;
          close(pc[1]); close(cp[0]);
          d->f_read=fdopen(pc[0],"r");
          d->fd_read=pc[0];
          d->f_write=fdopen(cp[1],"w");
          d->fd_write=cp[1];
          l->data=d;
          omFree(l->mode);
          l->mode = omStrDup(mode);
          singular_in_batchmode=TRUE;
          SI_LINK_SET_RW_OPEN_P(l);
          //myynest=0;
          fe_fgets_stdin=fe_fgets_dummy;
          if ((u!=NULL)&&(u->rtyp==IDHDL))
          {
            idhdl h=(idhdl)u->data;
            h->lev=0;
          }
          loop
          {
            leftv h=ssiRead1(l); /*contains an exit.... */
            if (feErrors != NULL && *feErrors != '\0')
            {
              // handle errors:
              PrintS(feErrors); /* currently quite simple */
              *feErrors = '\0';
            }
            ssiWrite(l,h);
            h->CleanUp();
            omFreeBin(h, sleftv_bin);
          }
          /* never reached*/
        }
        else if (pid>0) /*fork: parent*/
        {
          d->pid=pid;
          close(pc[0]); close(cp[1]);
          d->f_read=fdopen(cp[0],"r");
          d->fd_read=cp[0];
          d->f_write=fdopen(pc[1],"w");
          d->fd_write=pc[1];
          SI_LINK_SET_RW_OPEN_P(l);
          d->send_quit_at_exit=1;
        }
        else
        {
          Werror("fork failed (%d)",errno);
          l->data=NULL;
          omFree(d);
          return TRUE;
        }
      }
      // ---------------------------------------------------------------------
      else if (strcmp(mode,"tcp")==0)
      {
        int sockfd, newsockfd, portno, clilen;
        struct sockaddr_in serv_addr, cli_addr;
        int n;
        sockfd = socket(AF_INET, SOCK_STREAM, 0);
        if(sockfd < 0)
        {
          WerrorS("ERROR opening socket");
          l->data=NULL;
          omFree(d);
          return TRUE;
        }
        memset((char *) &serv_addr,0, sizeof(serv_addr));
        portno = 1025;
        serv_addr.sin_family = AF_INET;
        serv_addr.sin_addr.s_addr = INADDR_ANY;
        do
        {
          portno++;
          serv_addr.sin_port = htons(portno);
          if(portno > 50000)
          {
            WerrorS("ERROR on binding (no free port available?)");
            l->data=NULL;
            omFree(d);
            return TRUE;
          }
        }
        while(bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0);
        Print("waiting on port %d\n", portno);mflush();
        listen(sockfd,5);
        newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, (socklen_t *)&clilen);
        if(newsockfd < 0)
        {
          WerrorS("ERROR on accept");
          l->data=NULL;
          omFree(d);
          return TRUE;
        }
        PrintS("client accepted\n");
        d->fd_read = newsockfd;
        d->fd_write = newsockfd;
        d->f_read = fdopen(newsockfd, "r");
        d->f_write = fdopen(newsockfd, "w");
        SI_LINK_SET_RW_OPEN_P(l);
        close(sockfd);
      }
      // no ssi-Link on stdin or stdout
      else
      {
        Werror("invalid mode >>%s<< for ssi",mode);
        l->data=NULL;
        omFree(d);
        return TRUE;
      }
    }
    // =========================================================================
    else /*l->name=NULL*/
    {
      // tcp mode
      if(strcmp(mode,"tcp")==0)
      {
        int sockfd, newsockfd, portno, clilen;
        struct sockaddr_in serv_addr, cli_addr;
        int n;
        sockfd = socket(AF_INET, SOCK_STREAM, 0);
        if(sockfd < 0)
        {
          WerrorS("ERROR opening socket");
          l->data=NULL;
          omFree(d);
          return TRUE;
        }
        memset((char *) &serv_addr,0, sizeof(serv_addr));
        portno = 1025;
        serv_addr.sin_family = AF_INET;
        serv_addr.sin_addr.s_addr = INADDR_ANY;
        do
        {
          portno++;
          serv_addr.sin_port = htons(portno);
          if(portno > 50000)
          {
            WerrorS("ERROR on binding (no free port available?)");
            l->data=NULL;
            return TRUE;
          }
        }
        while(bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0);
        //Print("waiting on port %d\n", portno);mflush();
        listen(sockfd,5);
        char* cli_host = (char*)omAlloc(256);
        char* path = (char*)omAlloc(1024);
        int r = sscanf(l->name,"%255[^:]:%s",cli_host,path);
        if(r == 0)
        {
          WerrorS("ERROR: no host specified");
          l->data=NULL;
          omFree(d);
          omFree(path);
          omFree(cli_host);
          return TRUE;
        }
        else if(r == 1)
        {
          WarnS("program not specified, using /usr/local/bin/Singular");
          strcpy(path,"/usr/local/bin/Singular");
        }
        char* ssh_command = (char*)omAlloc(256);
        char* ser_host = (char*)omAlloc(64);
        gethostname(ser_host,64);
        sprintf(ssh_command,"ssh %s %s -q --batch --link=ssi --MPhost=%s --MPport=%d &",cli_host,path,ser_host,portno);
        //Print("client on %s started:%s\n",cli_host,path);
        omFree(path);
        omFree(cli_host);
        if (TEST_OPT_PROT) { Print("running >>%s<<\n",ssh_command); }
        system(ssh_command);
        omFree(ssh_command);
        omFree(ser_host);
        clilen = sizeof(cli_addr);
        newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, (socklen_t *)&clilen);
        if(newsockfd < 0)
        {
          WerrorS("ERROR on accept");
          l->data=NULL;
          omFree(d);
          return TRUE;
        }
        //PrintS("client accepted\n");
        d->fd_read = newsockfd;
        d->fd_write = newsockfd;
        d->f_read = fdopen(newsockfd, "r");
        d->f_write = fdopen(newsockfd, "w");
        SI_LINK_SET_RW_OPEN_P(l);
        d->send_quit_at_exit=1;
        close(sockfd);
        fprintf(d->f_write,"98 %d %d %u %u\n",SSI_VERSION,MAX_TOK,test,verbose);
      }
      // ----------------------------------------------------------------------
      else if(strcmp(mode,"connect")==0)
      {
        char* host = (char*)omAlloc(256);
        int sockfd, portno, n;
        struct sockaddr_in serv_addr;
        struct hostent *server;

        sscanf(l->name,"%255[^:]:%d",host,&portno);
        //Print("connect to host %s, port %d\n",host,portno);mflush();
        if (portno!=0)
        {
          sockfd = socket(AF_INET, SOCK_STREAM, 0);
          if (sockfd < 0) { WerrorS("ERROR opening socket"); return TRUE; }
          server = gethostbyname(host);
          if (server == NULL) {  WerrorS("ERROR, no such host");  return TRUE; }
          memset((char *) &serv_addr, 0, sizeof(serv_addr));
          serv_addr.sin_family = AF_INET;
          memcpy((char *)&serv_addr.sin_addr.s_addr,
                (char *)server->h_addr,
                server->h_length);
          serv_addr.sin_port = htons(portno);
          if (connect(sockfd,(sockaddr*)&serv_addr,sizeof(serv_addr)) < 0)
          { Werror("ERROR connecting(errno=%d)",errno); return TRUE; }
          //PrintS("connected\n");mflush();
          d->f_read=fdopen(sockfd,"r");
          d->fd_read=sockfd;
          d->f_write=fdopen(sockfd,"w");
          d->fd_write=sockfd;
          SI_LINK_SET_RW_OPEN_P(l);
          d->send_quit_at_exit=1;
          omFree(host);
        }
        else
        {
          l->data=NULL;
          omFree(d);
          return TRUE;
        }
      }
      // ======================================================================
      else
      {
        // normal link to a file
        FILE *outfile;
        char *filename=l->name;

        if(filename[0]=='>')
        {
          if (filename[1]=='>')
          {
            filename+=2;
            mode = "a";
          }
          else
          {
            filename++;
            mode="w";
          }
        }
        outfile=myfopen(filename,mode);
        if (outfile!=NULL)
        {
          if (strcmp(l->mode,"r")==0) d->f_read = outfile;
          else
          {
            d->f_write = outfile;
            fprintf(d->f_write,"98 %d %d %u %u\n",SSI_VERSION,MAX_TOK,test,verbose);
          }
        }
        else
        {
          omFree(d);
          l->data=NULL;
          return TRUE;
        }
      }
    }
  }

  return FALSE;
}

//**************************************************************************/
BOOLEAN ssiClose(si_link l)
{
  if (l!=NULL)
  {
    ssiInfo *d = (ssiInfo *)l->data;
    if (d!=NULL)
    {
      if (d->r!=NULL) rKill(d->r);
      if ((strcmp(l->mode,"tcp")==0)
      || (strcmp(l->mode,"fork")==0))
      {
        link_list hh=ssiToBeClosed;
        if (hh!=NULL)
        {
          if (hh->l==l)
          {
             ssiToBeClosed=(link_list)hh->next;
             omFreeSize(hh,sizeof(link_struct));
          }
          else while(hh!=NULL)
          {
            link_list hhh=(link_list)hh->next;
            if (hhh->l==l)
            {
              hh->next=hhh->next;
              omFreeSize(hhh,sizeof(link_struct));
              break;
            }
            hh=(link_list)hh->next;
          }
        }
        if (d->send_quit_at_exit)
        {
          fputs("99\n",d->f_write);fflush(d->f_write);
        }
      }
      if (d->f_read!=NULL) fclose(d->f_read);
      if (d->f_write!=NULL) fclose(d->f_write);
      if (d->pid!=0)
      {
        kill(d->pid,15);
        waitpid(d->pid,NULL,WNOHANG);
        kill(d->pid,9); // just to be sure
        waitpid(d->pid,NULL,0);
      }
      omFreeSize((ADDRESS)d,(sizeof *d));
    }
    l->data=NULL;
    SI_LINK_SET_CLOSE_P(l);
  }
  return FALSE;
}

//**************************************************************************/
leftv ssiRead1(si_link l)
{
  ssiInfo *d = (ssiInfo *)l->data;
  d->ungetc_buf='\0';
  leftv res=(leftv)omAlloc0(sizeof(sleftv));
  int t=0;
  SSI_BLOCK_CHLD;
  fscanf(d->f_read,"%d",&t);
  SSI_UNBLOCK_CHLD;
  //Print("got type %d\n",t);
  switch(t)
  {
    case 1:res->rtyp=INT_CMD;
           res->data=(char *)ssiReadInt(d->f_read);
           break;
    case 2:res->rtyp=STRING_CMD;
           res->data=(char *)ssiReadString(d);
           break;
    case 3:res->rtyp=NUMBER_CMD;
           res->data=(char *)ssiReadNumber(d);
           break;
    case 4:res->rtyp=BIGINT_CMD;
           res->data=(char *)ssiReadBigInt(d);
           break;
    case 15:
    case 5:{
             d->r=ssiReadRing(d);
             d->r->ref++;
             res->rtyp=RING_CMD;
             res->data=(char*)d->r;
             // we are in the top-level, so set the basering to d->r:
             ssiSetCurrRing(d->r);
             if (t==15) return ssiRead1(l);
           }
           break;
    case 6:res->rtyp=POLY_CMD;
           if (d->r==NULL) goto no_ring;
           res->data=(char*)ssiReadPoly(d);
           break;
    case 7:res->rtyp=IDEAL_CMD;
           if (d->r==NULL) goto no_ring;
           res->data=(char*)ssiReadIdeal(d);
           break;
    case 8:res->rtyp=MATRIX_CMD;
           if (d->r==NULL) goto no_ring;
           res->data=(char*)ssiReadMatrix(d);
           break;
    case 9:res->rtyp=VECTOR_CMD;
           if (d->r==NULL) goto no_ring;
           res->data=(char*)ssiReadPoly(d);
           break;
    case 10:res->rtyp=MODUL_CMD;
           if (d->r==NULL) goto no_ring;
           res->data=(char*)ssiReadIdeal(d);
           break;
    case 11:
           {
             res->rtyp=COMMAND;
             res->data=ssiReadCommand(l);
             int nok=res->Eval();
             if (nok) WerrorS("error in eval");
             break;
           }
    case 12: /*DEF_CMD*/
           {
             res->rtyp=0;
             res->name=(char *)ssiReadString(d);
             int nok=res->Eval();
             if (nok) WerrorS("error in name lookup");
             break;
           }
    case 13: res->rtyp=PROC_CMD;
             res->data=ssiReadProc(d);
             break;
    case 14: res->rtyp=LIST_CMD;
             res->data=ssiReadList(l);
             break;
    case 16: res->rtyp=NONE; res->data=NULL;
             break;
    case 17: res->rtyp=INTVEC_CMD;
             res->data=ssiReadIntvec(d);
             break;
    case 20: ssiReadBlackbox(res,l);
             break;
    // ------------
    case 98: // version
             {
                int n98_v,n98_m;
                BITSET n98_o1,n98_o2;
                SSI_BLOCK_CHLD;
                fscanf(d->f_read,"%d %d %u %u\n",&n98_v,&n98_m,&n98_o1,&n98_o2);
                SSI_UNBLOCK_CHLD;
                if ((n98_v!=SSI_VERSION) ||(n98_m!=MAX_TOK))
                {
                  Print("incompatible versions of ssi: %d/%d vs %d/%d",
                                  SSI_VERSION,MAX_TOK,n98_v,n98_m);
                }
                #ifndef NDEBUG
                if (TEST_OPT_DEBUG)
                  Print("// opening ssi-%d, MAX_TOK=%d\n",n98_v,n98_m);
                #endif
                test=n98_o1;
                verbose=n98_o2;
                return ssiRead1(l);
             }
    case 99: ssiClose(l); exit(0);
    case 0: if (feof(d->f_read))
            {
              ssiClose(l);
              res->rtyp=DEF_CMD;
              break;
            }
    default: Werror("not implemented (t:%d)",t);
             omFreeSize(res,sizeof(sleftv));
             res=NULL;
             break;
  }
  return res;
no_ring: WerrorS("no ring");
  omFreeSize(res,sizeof(sleftv));
  return NULL;
}
//**************************************************************************/
BOOLEAN ssiWrite(si_link l, leftv data)
{
  if(SI_LINK_W_OPEN_P(l)==0)
     if (slOpen(l,SI_LINK_OPEN|SI_LINK_WRITE,NULL)) return TRUE;
  ssiInfo *d = (ssiInfo *)l->data;
  d->level++;
  //FILE *fich=d->f;
  while (data!=NULL)
  {
    int tt=data->Typ();
    void *dd=data->Data();

    switch(tt /*data->Typ()*/)
    {
          case NONE/* nothing*/:fputs("16 ",d->f_write);
                          break;
          case STRING_CMD: fputs("2 ",d->f_write);
                           ssiWriteString(d,(char *)dd);
                           break;
          case INT_CMD: fputs("1 ",d->f_write);
                        ssiWriteInt(d,(int)(long)dd);
                        break;
          case BIGINT_CMD:fputs("4 ",d->f_write);
                        ssiWriteBigInt(d,(number)dd);
                        break;
          case NUMBER_CMD:
                          if (d->r!=currRing)
                          {
                            fputs("15 ",d->f_write);
                            ssiWriteRing(d,currRing);
                            if (d->level<=1) fputc('\n',d->f_write);
                          }
                          fputs("3 ",d->f_write);
                          ssiWriteNumber(d,(number)dd);
                        break;
          case RING_CMD:fputs("5 ",d->f_write);
                        ssiWriteRing(d,(ring)dd);
                        break;
          case POLY_CMD:
          case VECTOR_CMD:
                        if (d->r!=currRing)
                        {
                          fputs("15 ",d->f_write);
                          ssiWriteRing(d,currRing);
                          if (d->level<=1) fputc('\n',d->f_write);
                        }
                        if(tt==POLY_CMD) fputs("6 ",d->f_write);
                        else             fputs("9 ",d->f_write);
                        ssiWritePoly(d,tt,(poly)dd);
                        break;
          case IDEAL_CMD:
          case MODUL_CMD:
          case MATRIX_CMD:
                        if (d->r!=currRing)
                        {
                          fputs("15 ",d->f_write);
                          ssiWriteRing(d,currRing);
                          if (d->level<=1) fputc('\n',d->f_write);
                        }
                        if(tt==IDEAL_CMD)       fputs("7 ",d->f_write);
                        else if(tt==MATRIX_CMD) fputs("8 ",d->f_write);
                        else                    fputs("10 ",d->f_write);
                        ssiWriteIdeal(d,tt,(ideal)dd);
                        break;
          case COMMAND:
                   fputs("11 ",d->f_write);
                   ssiWriteCommand(l,(command)dd);
                   break;
          case DEF_CMD: /* not evaluated stuff in quotes */
                   fputs("12 ",d->f_write);
                   ssiWriteString(d,data->Name());
                   break;
          case PROC_CMD:
                   fputs("13 ",d->f_write);
                   ssiWriteProc(d,(procinfov)dd);
                   break;
          case LIST_CMD:
                   fputs("14 ",d->f_write);
                   ssiWriteList(l,(lists)dd);
                   break;
          case INTVEC_CMD:
                   fputs("17 ",d->f_write);
                   ssiWriteIntvec(d,(intvec *)dd);
                   break;
          default:
            if (tt>MAX_TOK)
            {
              blackbox *b=getBlackboxStuff(tt);
              fputs("20 ",d->f_write);
              b->blackbox_serialize(b,dd,l);
            }
            else
            {
              Werror("not implemented (t:%d, rtyp:%d)",tt, data->rtyp);
              d->level=0;
              return TRUE;
            }
            break;
    }
    if (d->level<=1) { fputc('\n',d->f_write); fflush(d->f_write); }
    data=data->next;
  }
  d->level--;
  return FALSE;
}

si_link_extension slInitSsiExtension(si_link_extension s)
{
  s->Open=ssiOpen;
  s->Close=ssiClose;
  s->Kill=ssiClose;
  s->Read=ssiRead1;
  s->Read2=(slRead2Proc)NULL;
  s->Write=ssiWrite;

  s->Status=slStatusSsi;
  s->type="ssi";
  return s;
}

const char* slStatusSsi(si_link l, const char* request)
{
  ssiInfo *d=(ssiInfo*)l->data;
  if (d==NULL) return "not open";
  if (((strcmp(l->mode,"fork")==0)||(strcmp(l->mode,"tcp")==0))
  && (strcmp(request, "read") == 0))
  {
    fd_set  mask, fdmask;
    struct timeval wt;
    if (d->ungetc_buf) return "ready";
    loop
    {
      /* Don't block. Return socket status immediately. */
      wt.tv_sec  = 0;
      wt.tv_usec = 0;

      FD_ZERO(&mask);
      FD_SET(d->fd_read, &mask);
      //Print("test fd %d\n",d->fd_read);
    /* check with select: chars waiting: no -> not ready */
      switch (select(d->fd_read+1, &mask, NULL, NULL, &wt))
      {
        case 0: /* not ready */ return "not ready";
        case -1: /*error*/      return "error";
        case 1: /*ready ? */    break;
      }
    /* yes: read 1 char*/
    /* if \n, check again with select else ungetc(c), ready*/
      int c=fgetc(d->f_read);
      //Print("try c=%d\n",c);
      if (c== -1) return "eof"; /* eof or error */
      else if (isdigit(c))
      { ungetc(c,d->f_read); d->ungetc_buf='\1'; return "ready"; }
      else if (c>' ')
      {
        Werror("unknown char in ssiLink(%d)",c);
        return "error";
      }
      /* else: next char */
    }
  }
  else if (strcmp(request, "read") == 0)
  {
    if (SI_LINK_R_OPEN_P(l) && (!feof(d->f_read))) return "ready";
    else return "not ready";
  }
  else if (strcmp(request, "write") == 0)
  {
    if (SI_LINK_W_OPEN_P(l)) return "ready";
    else return "not ready";
  }
  else return "unknown status request";
}

int slStatusSsiL(lists L, int timeout)
{
// input: L: a list with links of type
//           ssi-fork, ssi-tcp, MPtcp-fork or MPtcp-launch.
//           Note: Not every entry in L must be set.
//        timeout: timeout for select in micro-seconds
//           or -1 for infinity
//           or 0 for polling
// returns: ERROR (via Werror): L has wrong elements or link not open
//           -2: select returns an error
//           -1: the read state of all links is eof
//           0:  timeout (or polling): none ready,
//           i>0: (at least) L[i] is ready
  si_link l;
  ssiInfo *d;
  #ifdef HAVE_MPSR
  MP_Link_pt dd;
  #endif
  int d_fd;
  fd_set  mask, fdmask;
  FD_ZERO(&fdmask);
  FD_ZERO(&mask);
  int max_fd=0; /* 1 + max fd in fd_set */

  /* timeout */
  #ifdef HAVE_PSELECT
  struct timespec wt;
  struct timespec *wt_ptr=&wt;
  #else
  struct timeval wt;
  struct timeval *wt_ptr=&wt;
  #endif
  int startingtime = getRTimer()/TIMER_RESOLUTION;  // in seconds
  if (timeout== -1)
  {
    wt_ptr=NULL;
  }
  else
  {
    wt.tv_sec  = timeout / 1000000;
  #ifdef HAVE_PSELECT
    wt.tv_nsec = 1000 * (timeout % 1000000);
  #else
    wt.tv_usec = timeout % 1000000;
  #endif
  }

  /* signal mask for pselect() */
  sigset_t sigmask;
  if(sigprocmask(SIG_SETMASK, NULL, &sigmask) < 0)
  {
    WerrorS("error in sigprocmask()");
    return -2;
  }
  if(sigaddset(&sigmask, SIGCHLD) < 0)
  {
    WerrorS("error in sigaddset()");
    return -2;
  }

  /* auxiliary variables */
  int i;
  int j;
  int k;
  int s;
  char fdmaskempty;

  /* check the links and fill in fdmask */
  /* check ssi links for ungetc_buf */
  for(i=L->nr; i>=0; i--)
  {
    if (L->m[i].Typ()!=DEF_CMD)
    {
      if (L->m[i].Typ()!=LINK_CMD)
      { WerrorS("all elements must be of type link"); return -2;}
      l=(si_link)L->m[i].Data();
      if(SI_LINK_OPEN_P(l)==0)
      { WerrorS("all links must be open"); return -2;}
      if (((strcmp(l->m->type,"ssi")!=0) && (strcmp(l->m->type,"MPtcp")!=0))
      || ((strcmp(l->mode,"fork")!=0) && (strcmp(l->mode,"tcp")!=0)
        && (strcmp(l->mode,"launch")!=0)))
      {
        WerrorS("all links must be of type ssi:fork, ssi:tcp, MPtcp:fork\n");
        WerrorS("or MPtcp:launch");
        return -2;
      }
    #ifdef HAVE_MPSR
      if (strcmp(l->m->type,"ssi")==0)
      {
        d=(ssiInfo*)l->data;
        d_fd=d->fd_read;
        if (d->ungetc_buf=='\0')
        {
          FD_SET(d_fd, &fdmask);
          if (d_fd > max_fd) max_fd=d_fd;
        }
        else
          return i+1;
      }
      else
      {
        dd=(MP_Link_pt)l->data;
        d_fd=((MP_TCP_t *)dd->transp.private1)->sock;
        FD_SET(d_fd, &fdmask);
        if (d_fd > max_fd) max_fd=d_fd;
      }
    #else
      d=(ssiInfo*)l->data;
      d_fd=d->fd_read;
      if (d->ungetc_buf=='\0')
      {
        FD_SET(d_fd, &fdmask);
        if (d_fd > max_fd) max_fd=d_fd;
      }
      else
        return i+1;
    #endif
    }
  }
  max_fd++;

do_select:
  /* copy fdmask to mask */
  FD_ZERO(&mask);
  for(k = 0; k < max_fd; k++)
  {
    if(FD_ISSET(k, &fdmask))
    {
      FD_SET(k, &mask);
    }
  }

  /* check with select: chars waiting: no -> not ready */
  #ifdef HAVE_SIMPLEIPC
  sipc_semaphore_release(0);
  #endif
  #ifdef HAVE_PSELECT
  s = pselect(max_fd, &mask, NULL, NULL, wt_ptr, &sigmask);
  #else
  SSI_BLOCK_CHLD;
  s = select(max_fd, &mask, NULL, NULL, wt_ptr);
  SSI_UNBLOCK_CHLD;
  #endif
  #ifdef HAVE_SIMPLEIPC
  sipc_semaphore_acquire(0);
  #endif

  if (s==-1)
  {
    WerrorS("error in select call");
    return -2; /*error*/
  }
  if (s==0)
  {
    return 0; /*poll: not ready */
  }
  else /* s>0, at least one ready  (the number of fd which are ready is s)*/
  {
    j=0;
    while (j<=max_fd) { if (FD_ISSET(j,&mask)) break; j++; }
    for(i=L->nr; i>=0; i--)
    {
      if (L->m[i].rtyp==LINK_CMD)
      {
        l=(si_link)L->m[i].Data();
        #ifdef HAVE_MPSR
        if (strcmp(l->m->type,"ssi")!=0)
        {
          // for MP links, return here:
          dd=(MP_Link_pt)l->data;
          d_fd=((MP_TCP_t *)dd->transp.private1)->sock;
          if(j==d_fd) return i+1;
        }
        else
        {
          d=(ssiInfo*)l->data;
          d_fd=d->fd_read;
          if(j==d_fd) break;
        }
        #else
        d=(ssiInfo*)l->data;
        d_fd=d->fd_read;
        if(j==d_fd) break;
        #endif
      }
    }
    // only ssi links:
    loop
    {
      /* yes: read 1 char*/
      /* if \n, check again with select else ungetc(c), ready*/
      /* setting: d: current ssiInfo, j current fd, i current entry in L*/
      int c=fgetc(d->f_read);
      //Print("try c=%d\n",c);
      if (c== -1) /* eof */
      {
        FD_CLR(j,&fdmask);
        fdmaskempty = 1;
        for(k = 0; k < max_fd; k++)
        {
          if(FD_ISSET(k, &fdmask))
          {
            fdmaskempty = 0;
            break;
          }
        }
        if(fdmaskempty)
        {
          return -1;
        }
        if(timeout != -1)
        {
          timeout = si_max(0,
             timeout - 1000000*(getRTimer()/TIMER_RESOLUTION - startingtime));
          wt.tv_sec  = timeout / 1000000;
          #ifdef HAVE_PSELECT
          wt.tv_nsec = 1000 * (timeout % 1000000);
          #else
          wt.tv_usec = (timeout % 1000000);
          #endif
        }
        goto do_select;
      }

      else if (isdigit(c))
      { ungetc(c,d->f_read); d->ungetc_buf='\1'; return i+1; }
      else if (c>' ')
      {
        Werror("unknown char in ssiLink(%d)",c);
        return -2;
      }
      /* else: next char */
    }
  }
}

int ssiBatch(const char *host, const char * port)
/* return 0 on success, >0 else*/
{
  si_link l=(si_link) omAlloc0Bin(sip_link_bin);
  char *buf=(char*)omAlloc(256);
  sprintf(buf,"ssi:connect %s:%s",host,port);
  slInit(l, buf);
  if (slOpen(l,SI_LINK_OPEN,NULL)) return 1;
  SI_LINK_SET_RW_OPEN_P(l);

  idhdl id = enterid(omStrDup("link_ll"), 0, LINK_CMD, &IDROOT, FALSE);
  IDLINK(id) = l;

  loop
  {
    leftv h=ssiRead1(l); /*contains an exit.... */
    if (feErrors != NULL && *feErrors != '\0')
    {
      // handle errors:
      PrintS(feErrors); /* currently quite simple */
      *feErrors = '\0';
    }
    ssiWrite(l,h);
    h->CleanUp();
    omFreeBin(h, sleftv_bin);
  }
  /* never reached*/
  exit(0);
}

// ----------------------------------------------------------------
// format
// 1 int %d
// 2 string <len> %s
// 3 number
// 4 bigint 4 %d or 3 <mpz_t>
// 5 ring
// 6 poly
// 7 ideal
// 8 matrix
// 9 vector
// 10 module
// 11 command
// 12 def <len> %s
// 13 proc <len> %s
// 14 list %d <elem1> ....
// 15 setring .......
// 16 nothing
// 17 intvec <len> ...
//
// 20 blackbox <name> ...
//
// 98: verify version: <ssi-version> <MAX_TOK> <OPT1> <OPT2>
// 99: quit Singular
