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
#include <time.h>

#include <kernel/mod2.h>
#include <reporter/si_signals.h>
// #include "mod2.h"

#include <Singular/tok.h>
#include <Singular/ipid.h>
#include <Singular/ipshell.h>
#include <omalloc/omalloc.h>
#include <libpolys/polys/monomials/ring.h>
#include <libpolys/polys/matpol.h>
#include <libpolys/polys/simpleideals.h>
#include <libpolys/polys/monomials/p_polys.h>
#define TRANSEXT_PRIVATES 1 // allow access to transext internals
#include <libpolys/polys/ext_fields/transext.h>
#include <libpolys/coeffs/longrat.h>
#include <libpolys/misc/intvec.h>
#include <libpolys/coeffs/bigintmat.h>
#include <libpolys/misc/options.h>
#include <kernel/timer.h>
#include <Singular/rlimit.h>
#include <Singular/subexpr.h>
#include <Singular/links/silink.h>
#include <Singular/cntrlc.h>
#include <Singular/lists.h>
#include <Singular/blackbox.h>
#include <libpolys/reporter/s_buff.h>
#include <Singular/links/ssiLink.h>

#ifdef HAVE_SIMPLEIPC
#include <Singular/links/simpleipc.h>
#endif

#define SSI_VERSION 6
// 5->6: changed newstruct representation

// 64 bit version:
//#if SIZEOF_LONG == 8
#if 0
#define MAX_NUM_SIZE 60
#define POW_2_28 (1L<<60)
#define LONG long
#else
// 32 bit version:
#define MAX_NUM_SIZE 28
#define POW_2_28 (1L<<28)
#define LONG int
#endif

#define SSI_BASE 16
typedef struct
{
  s_buff f_read;
  FILE *f_write;
  ring r;
  pid_t pid; /* only valid for fork/tcp mode*/
  int fd_read,fd_write; /* only valid for fork/tcp mode*/
  char level;
  char send_quit_at_exit;
  char quit_sent;

} ssiInfo;

link_list ssiToBeClosed=NULL;
volatile BOOLEAN ssiToBeClosed_inactive=TRUE;

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
  fprintf(d->f_write,"%d ",i);
  //if (d->f_debug!=NULL) fprintf(d->f_debug,"int: %d ",i);
}

void ssiWriteString(ssiInfo *d,const char *s)
{
  fprintf(d->f_write,"%d %s ",(int)strlen(s),s);
  //if (d->f_debug!=NULL) fprintf(d->f_debug,"stringi: %d \"%s\" ",strlen(s),s);
}


void ssiWriteBigInt(const ssiInfo *d, const number n)
{
  // syntax is as follows:
  // case 2 Q:     3 4 <int>
  //        or     3 3 <mpz_t nominator>
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
}

void ssiWritePoly_R(const ssiInfo *d, int typ, poly p, const ring r);
void ssiWriteNumber_CF(const ssiInfo *d, const number n, const coeffs cf)
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
  if (getCoeffType(cf)==n_transExt)
  {
    fraction f=(fraction)n;
    ssiWritePoly_R(d,POLY_CMD,NUM(f),cf->extRing);
    ssiWritePoly_R(d,POLY_CMD,DEN(f),cf->extRing);
  }
  else if (getCoeffType(cf)==n_algExt)
  {
    ssiWritePoly_R(d,POLY_CMD,(poly)n,cf->extRing);
  }
  else if (cf->cfWriteFd!=NULL)
  {
    cf->cfWriteFd(n,d->f_write,cf);
  }
  else WerrorS("coeff field not implemented");
}

void ssiWriteNumber(const ssiInfo *d, const number n)
{
  ssiWriteNumber_CF(d,n,d->r->cf);
}

void ssiWriteRing(ssiInfo *d,const ring r)
{
  /* 5 <ch> <N> <l1> <v1> ...<lN> <vN> <number of orderings> <ord1> <block0_1> <block1_1> .... */
  /* ch=-1: transext, coeff ring follows */
  /* ch=-2: algext, coeff ring and minpoly follows */
  if (r==currRing) // see recursive calls for transExt/algExt
  {
    if (d->r!=NULL) rKill(d->r);
    d->r=r;
  }
  if (r!=NULL)
  {
    /*d->*/r->ref++;
    if (rField_is_Q(r) || rField_is_Zp(r))
      fprintf(d->f_write,"%d %d ",n_GetChar(r->cf),r->N);
    else if (rFieldType(r)==n_transExt)
      fprintf(d->f_write,"-1 %d ",r->N);
    else if (rFieldType(r)==n_algExt)
      fprintf(d->f_write,"-2 %d ",r->N);
    else /*dummy*/
      fprintf(d->f_write,"0 %d ",r->N);

    int i;
    for(i=0;i<r->N;i++)
    {
      fprintf(d->f_write,"%d %s ",(int)strlen(r->names[i]),r->names[i]);
    }
    /* number of orderings:*/
    i=0;
    // remember dummy ring: everything 0:
    if (r->order!=NULL) while (r->order[i]!=0) i++;
    fprintf(d->f_write,"%d ",i);
    /* each ordering block: */
    i=0;
    if (r->order!=NULL) while(r->order[i]!=0)
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
    if ((rFieldType(r)==n_transExt)
    || (rFieldType(r)==n_algExt))
    {
      ssiWriteRing(d,r->cf->extRing);
      if  (rFieldType(r)==n_algExt)
      {
        ssiWritePoly_R(d,POLY_CMD,r->cf->extRing->qideal->m[0],r->cf->extRing);
      }
    }
  }
  else /* dummy ring r==NULL*/
  {
    fprintf(d->f_write,"0 0 0 "/*,r->ch,r->N, blocks*/);
  }
}

void ssiWritePoly_R(const ssiInfo *d, int typ, poly p, const ring r)
{
  fprintf(d->f_write,"%d ",pLength(p));//number of terms
  int i;

  while(p!=NULL)
  {
    ssiWriteNumber_CF(d,pGetCoeff(p),r->cf);
    //nWrite(fich,pGetCoeff(p));
    fprintf(d->f_write,"%ld ",p_GetComp(p,r));//component

    for(int j=1;j<=rVar(r);j++)
    {
      fprintf(d->f_write,"%ld ",p_GetExp(p,j,r ));//x^j
    }
    pIter(p);
  }
}

void ssiWritePoly(const ssiInfo *d, int typ, poly p)
{
  ssiWritePoly_R(d,typ,p,d->r);
}

void ssiWriteIdeal(ssiInfo *d, int typ,ideal I)
{
   // syntax: 7 # of elements <poly 1> <poly2>.....
   // syntax: 8 <rows> <cols> <poly 1> <poly2>.....
   matrix M=(matrix)I;
   int mn;
   if (typ==MATRIX_CMD)
   {
     mn=MATROWS(M)*MATCOLS(M);
     fprintf(d->f_write,"%d %d ", MATROWS(M),MATCOLS(M));
   }
   else
   {
     mn=IDELEMS(I);
     fprintf(d->f_write,"%d ",IDELEMS(I));
   }

   int i;
   int tt;
   if (typ==MODUL_CMD) tt=VECTOR_CMD;
   else                tt=POLY_CMD;

   for(i=0;i<mn;i++)
   {
     ssiWritePoly(d,tt,I->m[i]);
   }
}

void ssiWriteCommand(si_link l, command D)
{
  ssiInfo *d=(ssiInfo*)l->data;
  // syntax: <num ops> <operation> <op1> <op2> ....
  fprintf(d->f_write,"%d %d ",D->argc,D->op);
  if (D->argc >0) ssiWrite(l, &(D->arg1));
  if (D->argc < 4)
  {
    if (D->argc >1) ssiWrite(l, &(D->arg2));
    if (D->argc >2) ssiWrite(l, &(D->arg3));
  }
}

void ssiWriteProc(ssiInfo *d,procinfov p)
{
  if (p->data.s.body==NULL)
    iiGetLibProcBuffer(p);
  if (p->data.s.body!=NULL)
    ssiWriteString(d,p->data.s.body);
  else
    ssiWriteString(d,"");
}

void ssiWriteList(si_link l,lists dd)
{
  ssiInfo *d=(ssiInfo*)l->data;
  int Ll=lSize(dd);
  fprintf(d->f_write,"%d ",Ll+1);
  int i;
  for(i=0;i<=Ll;i++)
  {
    ssiWrite(l,&(dd->m[i]));
  }
}
void ssiWriteIntvec(ssiInfo *d,intvec * v)
{
  fprintf(d->f_write,"%d ",v->length());
  int i;
  for(i=0;i<v->length();i++)
  {
    fprintf(d->f_write,"%d ",(*v)[i]);
  }
}
void ssiWriteIntmat(ssiInfo *d,intvec * v)
{
  fprintf(d->f_write,"%d %d ",v->rows(),v->cols());
  int i;
  for(i=0;i<v->length();i++)
  {
    fprintf(d->f_write,"%d ",(*v)[i]);
  }
}

void ssiWriteBigintmat(ssiInfo *d,bigintmat * v)
{
  fprintf(d->f_write,"%d %d ",v->rows(),v->cols());
  int i;
  for(i=0;i<v->length();i++)
  {
    ssiWriteBigInt(d,(*v)[i]);
  }
}

char *ssiReadString(ssiInfo *d)
{
  char *buf;
  int l;
  l=s_readint(d->f_read);
  buf=(char*)omAlloc0(l+1);
  int c =s_getc(d->f_read); /* skip ' '*/
  int ll=s_readbytes(buf,l,d->f_read);
  //if (ll!=l) printf("want %d, got %d bytes\n",l,ll);
  buf[l]='\0';
  return buf;
}

int ssiReadInt(s_buff fich)
{
  return s_readint(fich);
}

number ssiReadBigInt(ssiInfo *d)
{
  int sub_type=-1;
  sub_type=s_readint(d->f_read);
  switch(sub_type)
  {
   case 3:
     {// read int or mpz_t or mpz_t, mpz_t
       number n=nlRInit(0);
       s_readmpz(d->f_read,n->z);
       n->s=sub_type;
       return n;
     }
   case 4:
     {
       int dd;
       dd=s_readint(d->f_read);
       return INT_TO_SR(dd);
     }
   default:
       Werror("error in reading bigint: invalid subtype %d",sub_type);
       return NULL;
   }
}

static number ssiReadQNumber(ssiInfo *d)
{
  int sub_type=-1;
  sub_type=s_readint(d->f_read);
  switch(sub_type)
  {
     case 0:
     case 1:
       {// read mpz_t, mpz_t
         number n=nlRInit(0);
         mpz_init(n->n);
         s_readmpz(d->f_read,n->z);
         s_readmpz(d->f_read,n->n);
         n->s=sub_type;
         return n;
       }

     case 3:
       {// read mpz_t
         number n=nlRInit(0);
         s_readmpz(d->f_read,n->z);
         n->s=3; /*sub_type*/
         return n;
       }
     case 4:
       {
         LONG dd=s_readlong(d->f_read);
         //#if SIZEOF_LONG == 8
         return INT_TO_SR(dd);
         //#else
         //return nlInit(dd,NULL);
         //#endif
       }
     case 5:
     case 6:
       {// read raw mpz_t, mpz_t
         number n=nlRInit(0);
         mpz_init(n->n);
         s_readmpz_base (d->f_read,n->z, SSI_BASE);
         s_readmpz_base (d->f_read,n->n, SSI_BASE);
         n->s=sub_type-5;
         return n;
       }
     case 8:
       {// read raw mpz_t
         number n=nlRInit(0);
         s_readmpz_base (d->f_read,n->z, SSI_BASE);
         n->s=sub_type=3; /*subtype-5*/
         return n;
       }

     default: Werror("error in reading number: invalid subtype %d",sub_type);
              return NULL;
  }
  return NULL;
}

poly ssiReadPoly_R(ssiInfo *D, const ring r);
number ssiReadNumber_CF(ssiInfo *d, const coeffs cf)
{
  if (cf->cfReadFd!=NULL)
  {
     return cf->cfReadFd(d->f_read,cf);
  }
  else if (getCoeffType(cf) == n_transExt)
  {
    // poly poly
    fraction f=(fraction)n_Init(1,cf);
    p_Delete(&NUM(f),cf->extRing);
    NUM(f)=ssiReadPoly_R(d,cf->extRing);
    DEN(f)=ssiReadPoly_R(d,cf->extRing);
    return (number)f;
  }
  else if (getCoeffType(cf) == n_algExt)
  {
    // poly
    return (number)ssiReadPoly_R(d,cf->extRing);
  }
  else Werror("coeffs not implemented in ssiReadNumber");
  return NULL;
}

number ssiReadNumber(ssiInfo *d)
{
  return ssiReadNumber_CF(d,d->r->cf);
}

ring ssiReadRing(ssiInfo *d)
{
/* syntax is <ch> <N> <l1> <v1> ...<lN> <vN> <number of orderings> <ord1> <block0_1> <block1_1> .... */
  int ch, N,i,l;
  char **names;
  ch=s_readint(d->f_read);
  N=s_readint(d->f_read);
  if (N!=0)
  {
    names=(char**)omAlloc(N*sizeof(char*));
    for(i=0;i<N;i++)
    {
      names[i]=ssiReadString(d);
    }
  }
  // read the orderings:
  int num_ord; // number of orderings
  num_ord=s_readint(d->f_read);
  int *ord=(int *)omAlloc0((num_ord+1)*sizeof(int));
  int *block0=(int *)omAlloc0((num_ord+1)*sizeof(int));
  int *block1=(int *)omAlloc0((num_ord+1)*sizeof(int));
  int **wvhdl=(int**)omAlloc0((num_ord+1)*sizeof(int*));
  for(i=0;i<num_ord;i++)
  {
    ord[i]=s_readint(d->f_read);
    block0[i]=s_readint(d->f_read);
    block1[i]=s_readint(d->f_read);
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
          wvhdl[i][ii-block0[i]]=s_readint(d->f_read);
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
  if (N==0)
  {
    omFree(ord);
    omFree(block0);
    omFree(block1);
    omFree(wvhdl);
    return NULL;
  }
  else if (ch>=0) /* Q, Z/p */
    return rDefault(ch,N,names,num_ord,ord,block0,block1,wvhdl);
  else if (ch==-1) /* trans ext. */
  {
    TransExtInfo T;
    T.r=ssiReadRing(d);
    coeffs cf=nInitChar(n_transExt,&T);
    return rDefault(cf,N,names,num_ord,ord,block0,block1,wvhdl);
  }
  else if (ch==-2) /* alg ext. */
  {
    TransExtInfo T;
    T.r=ssiReadRing(d);
    T.r->qideal=idInit(1,1);
    T.r->qideal->m[0]=ssiReadPoly_R(d,T.r);
    coeffs cf=nInitChar(n_algExt,&T);
    return rDefault(cf,N,names,num_ord,ord,block0,block1,wvhdl);
  }
  else
  {
    Werror("ssi: read unknown coeffs type (%d)",ch);
    return NULL;
  }
}

poly ssiReadPoly_R(ssiInfo *D, const ring r)
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
    p=p_Init(r);
    pSetCoeff0(p,ssiReadNumber_CF(D,r->cf));
    int d;
    d=s_readint(D->f_read);
    p_SetComp(p,d,r);
    for(i=1;i<=rVar(r);i++)
    {
      d=s_readint(D->f_read);
      p_SetExp(p,i,d,r);
    }
    p_Setm(p,r);
    p_Test(p,r);
    if (ret==NULL) ret=p;
    else           pNext(prev)=p;
    prev=p;
 }
 return ret;
}

poly ssiReadPoly(ssiInfo *D)
{
// < # of terms> < term1> < .....
  return ssiReadPoly_R(D,D->r);
}

ideal ssiReadIdeal(ssiInfo *d)
{
  int n,i;
  ideal I;
  n=s_readint(d->f_read);
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
  m=s_readint(d->f_read);
  n=s_readint(d->f_read);
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
  argc=s_readint(d->f_read);
  op=s_readint(d->f_read);
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
  nr=s_readint(d->f_read);
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
  nr=s_readint(d->f_read);
  intvec *v=new intvec(nr);
  for(int i=0;i<nr;i++)
  {
    (*v)[i]=s_readint(d->f_read);
  }
  return v;
}
intvec* ssiReadIntmat(ssiInfo *d)
{
  int r,c;
  r=s_readint(d->f_read);
  c=s_readint(d->f_read);
  intvec *v=new intvec(r,c,0);
  for(int i=0;i<r*c;i++)
  {
    (*v)[i]=s_readint(d->f_read);
  }
  return v;
}
bigintmat* ssiReadBigintmat(ssiInfo *d)
{
  int r,c;
  r=s_readint(d->f_read);
  c=s_readint(d->f_read);
  bigintmat *v=new bigintmat(r,c,coeffs_BIGINT);
  for(int i=0;i<r*c;i++)
  {
    (*v)[i]=ssiReadBigInt(d);
  }
  return v;
}

void ssiReadBlackbox(leftv res, si_link l)
{
  ssiInfo *d=(ssiInfo*)l->data;
  int throwaway;
  throwaway=s_readint(d->f_read);
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
        pid_t pid = fork();
        if (pid == -1 && errno == EAGAIN)   // RLIMIT_NPROC too low?
        {
          raise_rlimit_nproc();
          pid = fork();
        }
        if (pid == -1)
        {
          WerrorS("could not fork");
        }
        if (pid==0) /*fork: child*/
        {
          /* block SIGINT */
          sigset_t sigint;
          sigemptyset(&sigint);
          sigaddset(&sigint, SIGINT);
          sigprocmask(SIG_BLOCK, &sigint, NULL);

          link_list hh=(link_list)ssiToBeClosed->next;
          /* we know: l is the first entry in ssiToBeClosed-list */
          while(hh!=NULL)
          {
            SI_LINK_SET_CLOSE_P(hh->l);
            ssiInfo *dd=(ssiInfo*)hh->l->data;
            s_close(dd->f_read);
            s_free(dd->f_read);
            fclose(dd->f_write);
            if (dd->r!=NULL) rKill(dd->r);
            omFreeSize((ADDRESS)dd,(sizeof *dd));
            hh->l->data=NULL;
            link_list nn=(link_list)hh->next;
            omFree(hh);
            hh=nn;
          }
          ssiToBeClosed->next=NULL;
#ifdef HAVE_SIMPLEIPC
          memset(sem_acquired, 0, SIPC_MAX_SEMAPHORES*sizeof(sem_acquired[0]));
#endif   // HAVE_SIMPLEIPC
          si_close(pc[1]); si_close(cp[0]);
          d->f_write=fdopen(cp[1],"w");
          d->f_read=s_open(pc[0]);
          d->fd_read=pc[0];
          d->fd_write=cp[1];
	  //d->r=currRing;
	  //if (d->r!=NULL) d->r->ref++;
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
          si_close(pc[0]); si_close(cp[1]);
          d->f_write=fdopen(pc[1],"w");
          d->f_read=s_open(cp[0]);
          d->fd_read=cp[0];
          d->fd_write=pc[1];
          SI_LINK_SET_RW_OPEN_P(l);
          d->send_quit_at_exit=1;
	  //d->r=currRing;
	  //if (d->r!=NULL) d->r->ref++;
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
        listen(sockfd,1);
        newsockfd = si_accept(sockfd, (struct sockaddr *) &cli_addr, (socklen_t *)&clilen);
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
        d->f_read = s_open(newsockfd);
        d->f_write = fdopen(newsockfd, "w");
        SI_LINK_SET_RW_OPEN_P(l);
        si_close(sockfd);
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
        listen(sockfd,1);
        char* cli_host = (char*)omAlloc(256);
        char* path = (char*)omAlloc(1024);
        int r = si_sscanf(l->name,"%255[^:]:%s",cli_host,path);
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
        newsockfd = si_accept(sockfd, (struct sockaddr *) &cli_addr, (socklen_t *)&clilen);
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
        d->f_read = s_open(newsockfd);
        d->f_write = fdopen(newsockfd, "w");
        si_close(sockfd);
        SI_LINK_SET_RW_OPEN_P(l);
        d->send_quit_at_exit=1;
        link_list newlink=(link_list)omAlloc(sizeof(link_struct));
        newlink->u=u;
        newlink->l=l;
        newlink->next=(void *)ssiToBeClosed;
        ssiToBeClosed=newlink;
        fprintf(d->f_write,"98 %d %d %u %u\n",SSI_VERSION,MAX_TOK,si_opt_1,si_opt_2);
      }
      // ----------------------------------------------------------------------
      else if(strcmp(mode,"connect")==0)
      {
        char* host = (char*)omAlloc(256);
        int sockfd, portno, n;
        struct sockaddr_in serv_addr;
        struct hostent *server;

        si_sscanf(l->name,"%255[^:]:%d",host,&portno);
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
          if (si_connect(sockfd,(sockaddr*)&serv_addr,sizeof(serv_addr)) < 0)
          { Werror("ERROR connecting(errno=%d)",errno); return TRUE; }
          //PrintS("connected\n");mflush();
          d->f_read=s_open(sockfd);
          d->fd_read=sockfd;
          d->f_write=fdopen(sockfd,"w");
          d->fd_write=sockfd;
          SI_LINK_SET_RW_OPEN_P(l);
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
          if (strcmp(l->mode,"r")==0)
          {
            fclose(outfile);
            d->f_read=s_open_by_name(filename);
          }
          else
          {
            d->f_write = outfile;
            fprintf(d->f_write,"98 %d %d %u %u\n",SSI_VERSION,MAX_TOK,si_opt_1,si_opt_2);
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
BOOLEAN ssiPrepClose(si_link l)
{
  if (l!=NULL)
  {
    ssiInfo *d = (ssiInfo *)l->data;
    if (d!=NULL)
    {
      if (d->send_quit_at_exit)
      {
        fputs("99\n",d->f_write);
        fflush(d->f_write);
      }
      d->quit_sent=1;
    }
  }
  return FALSE;
}

BOOLEAN ssiClose(si_link l)
{
  if (l!=NULL)
  {
    SI_LINK_SET_CLOSE_P(l);
    ssiInfo *d = (ssiInfo *)l->data;
    if (d!=NULL)
    {
      if ((d->send_quit_at_exit)
      && (d->quit_sent==0))
      {
        fputs("99\n",d->f_write);
        fflush(d->f_write);
      }
      if (d->r!=NULL) rKill(d->r);
      if ((d->pid!=0)
      && (si_waitpid(d->pid,NULL,WNOHANG)==0))
      {
        struct timespec t;
        t.tv_sec=0;
        t.tv_nsec=100000000; // <=100 ms
        struct timespec rem;
        int r;
        do
        {
          r = nanosleep(&t, &rem);
          t = rem;
        } while ((r < 0) && (errno == EINTR)
            && (si_waitpid(d->pid,NULL,WNOHANG) == 0));
        if ((r == 0) && (si_waitpid(d->pid,NULL,WNOHANG) == 0))
        {
          kill(d->pid,15);
          t.tv_sec=5; // <=5s
          t.tv_nsec=0;
          do
          {
            r = nanosleep(&t, &rem);
            t = rem;
          } while ((r < 0) && (errno == EINTR)
              && (si_waitpid(d->pid,NULL,WNOHANG) == 0));
          if ((r == 0) && (si_waitpid(d->pid,NULL,WNOHANG) == 0))
          {
            kill(d->pid,9); // just to be sure
            si_waitpid(d->pid,NULL,0);
          }
        }
      }
      if (d->f_read!=NULL) s_close(d->f_read);
      if (d->f_read!=NULL) s_free(d->f_read);
      if (d->f_write!=NULL) fclose(d->f_write);
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
          else while(hh->next!=NULL)
          {
            link_list hhh=(link_list)hh->next;
            if (hhh->l==l)
            {
              hh->next=hhh->next;
              omFreeSize(hhh,sizeof(link_struct));
              break;
            }
            else
              hh=(link_list)hh->next;
          }
        }
      }
      omFreeSize((ADDRESS)d,(sizeof *d));
    }
    l->data=NULL;
  }
  return FALSE;
}

//**************************************************************************/
leftv ssiRead1(si_link l)
{
  ssiInfo *d = (ssiInfo *)l->data;
  leftv res=(leftv)omAlloc0(sizeof(sleftv));
  int t=0;
  t=s_readint(d->f_read);
  //Print("got type %d\n",t);
  switch(t)
  {
    case 1:res->rtyp=INT_CMD;
           res->data=(char *)(long)ssiReadInt(d->f_read);
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
             res->rtyp=RING_CMD;
             res->data=(char*)d->r;
             // we are in the top-level, so set the basering to d->r:
             if (d->r!=NULL)
             {
               d->r->ref++;
               ssiSetCurrRing(d->r);
             }
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
    case 18: res->rtyp=INTMAT_CMD;
             res->data=ssiReadIntmat(d);
             break;
    case 19: res->rtyp=BIGINTMAT_CMD;
             res->data=ssiReadBigintmat(d);
             break;
    case 20: ssiReadBlackbox(res,l);
             break;
    // ------------
    case 98: // version
             {
                int n98_v,n98_m;
                BITSET n98_o1,n98_o2;
                n98_v=s_readint(d->f_read);
                n98_m=s_readint(d->f_read);
                n98_o1=s_readint(d->f_read);
                n98_o2=s_readint(d->f_read);
                if ((n98_v!=SSI_VERSION) ||(n98_m!=MAX_TOK))
                {
                  Print("incompatible versions of ssi: %d/%d vs %d/%d",
                                  SSI_VERSION,MAX_TOK,n98_v,n98_m);
                }
                #ifndef SING_NDEBUG
                if (TEST_OPT_DEBUG)
                  Print("// opening ssi-%d, MAX_TOK=%d\n",n98_v,n98_m);
                #endif
                si_opt_1=n98_o1;
                si_opt_2=n98_o2;
                return ssiRead1(l);
             }
    case 99: ssiClose(l); m2_end(0);
    case 0: if (s_iseof(d->f_read))
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
BOOLEAN ssiSetRing(si_link l, ring r, BOOLEAN send)
{
  if(SI_LINK_W_OPEN_P(l)==0)
     if (slOpen(l,SI_LINK_OPEN|SI_LINK_WRITE,NULL)) return TRUE;
  ssiInfo *d = (ssiInfo *)l->data;
  if (d->r!=r)
  {
    if (send)
    {
      fputs("15 ",d->f_write);
      ssiWriteRing(d,r);
    }
    d->r=r;
  }
  if (currRing!=r) rChangeCurrRing(r);
  return FALSE;
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
    if ((dd==NULL) && (data->name!=NULL) && (tt==0)) tt=DEF_CMD;
      // return pure undefined names as def

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
          case INTMAT_CMD:
                   fputs("18 ",d->f_write);
                   ssiWriteIntmat(d,(intvec *)dd);
                   break;
          case BIGINTMAT_CMD:
                   fputs("19 ",d->f_write);
                   ssiWriteBigintmat(d,(bigintmat *)dd);
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

BOOLEAN ssiGetDump(si_link l);
BOOLEAN ssiDump(si_link l);

si_link_extension slInitSsiExtension(si_link_extension s)
{
  s->Open=ssiOpen;
  s->Close=ssiClose;
  s->Kill=ssiClose;
  s->Read=ssiRead1;
  s->Read2=(slRead2Proc)NULL;
  s->Write=ssiWrite;
  s->Dump=ssiDump;
  s->GetDump=ssiGetDump;

  s->Status=slStatusSsi;
  s->SetRing=ssiSetRing;
  s->type="ssi";
  return s;
}

const char* slStatusSsi(si_link l, const char* request)
{
  ssiInfo *d=(ssiInfo*)l->data;
  if (d==NULL) return "not open";
  if (((strcmp(l->mode,"fork")==0)
  ||(strcmp(l->mode,"tcp")==0)
  ||(strcmp(l->mode,"connect")==0))
  && (strcmp(request, "read") == 0))
  {
    fd_set  mask, fdmask;
    struct timeval wt;
    if (s_isready(d->f_read)) return "ready";
    loop
    {
      /* Don't block. Return socket status immediately. */
      wt.tv_sec  = 0;
      wt.tv_usec = 0;

      FD_ZERO(&mask);
      FD_SET(d->fd_read, &mask);
      //Print("test fd %d\n",d->fd_read);
    /* check with select: chars waiting: no -> not ready */
      switch (si_select(d->fd_read+1, &mask, NULL, NULL, &wt))
      {
        case 0: /* not ready */ return "not ready";
        case -1: /*error*/      return "error";
        case 1: /*ready ? */    break;
      }
    /* yes: read 1 char*/
    /* if \n, check again with select else ungetc(c), ready*/
      int c=s_getc(d->f_read);
      //Print("try c=%d\n",c);
      if (c== -1) return "eof"; /* eof or error */
      else if (isdigit(c))
      { s_ungetc(c,d->f_read); return "ready"; }
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
    if (SI_LINK_R_OPEN_P(l) && (!s_iseof(d->f_read)) && (s_isready(d->f_read))) return "ready";
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
//           ssi-connect, ssi-fork, ssi-tcp, MPtcp-fork or MPtcp-launch.
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
  int d_fd;
  fd_set  mask, fdmask;
  FD_ZERO(&fdmask);
  FD_ZERO(&mask);
  int max_fd=0; /* 1 + max fd in fd_set */

  /* timeout */
  struct timeval wt;
  struct timeval *wt_ptr=&wt;
  int startingtime = getRTimer()/TIMER_RESOLUTION;  // in seconds
  if (timeout== -1)
  {
    wt_ptr=NULL;
  }
  else
  {
    wt.tv_sec  = timeout / 1000000;
    wt.tv_usec = timeout % 1000000;
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
        && (strcmp(l->mode,"launch")!=0) && (strcmp(l->mode,"connect")!=0)))
      {
        WerrorS("all links must be of type ssi:fork, ssi:tcp, ssi:connect");
        return -2;
      }
      if (strcmp(l->m->type,"ssi")==0)
      {
        d=(ssiInfo*)l->data;
        d_fd=d->fd_read;
        if (!s_isready(d->f_read))
        {
          FD_SET(d_fd, &fdmask);
          if (d_fd > max_fd) max_fd=d_fd;
        }
        else
          return i+1;
      }
      else
      {
        Werror("wrong link type >>%s<<",l->m->type);
        return -2;
      }
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
  s = si_select(max_fd, &mask, NULL, NULL, wt_ptr);
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
        if (strcmp(l->m->type,"ssi")==0)
        {
          d=(ssiInfo*)l->data;
          d_fd=d->fd_read;
          if(j==d_fd) break;
        }
        else
        {
          Werror("wrong link type >>%s<<",l->m->type);
          return -2;
        }
      }
    }
    // only ssi links:
    loop
    {
      /* yes: read 1 char*/
      /* if \n, check again with select else ungetc(c), ready*/
      /* setting: d: current ssiInfo, j current fd, i current entry in L*/
      int c=s_getc(d->f_read);
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
          wt.tv_usec = (timeout % 1000000);
        }
        goto do_select;
      }

      else if (isdigit(c))
      { s_ungetc(c,d->f_read); return i+1; }
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

static int ssiReserved_P=0;
static int ssiReserved_sockfd;
static  struct sockaddr_in ssiResverd_serv_addr;
static int  ssiReserved_Clients;
int ssiReservePort(int clients)
{
  if (ssiReserved_P!=0)
  {
    WerrorS("ERROR already a reverved port requested");
    return 0;
  }
  int portno;
  int n;
  ssiReserved_sockfd = socket(AF_INET, SOCK_STREAM, 0);
  if(ssiReserved_sockfd < 0)
  {
    WerrorS("ERROR opening socket");
    return 0;
  }
  memset((char *) &ssiResverd_serv_addr,0, sizeof(ssiResverd_serv_addr));
  portno = 1025;
  ssiResverd_serv_addr.sin_family = AF_INET;
  ssiResverd_serv_addr.sin_addr.s_addr = INADDR_ANY;
  do
  {
    portno++;
    ssiResverd_serv_addr.sin_port = htons(portno);
    if(portno > 50000)
    {
      WerrorS("ERROR on binding (no free port available?)");
      return 0;
    }
  }
  while(bind(ssiReserved_sockfd, (struct sockaddr *) &ssiResverd_serv_addr, sizeof(ssiResverd_serv_addr)) < 0);
  ssiReserved_P=portno;
  listen(ssiReserved_sockfd,clients);
  ssiReserved_Clients=clients;
  return portno;
}

extern si_link_extension si_link_root;
si_link ssiCommandLink()
{
  if (ssiReserved_P==0)
  {
    WerrorS("ERROR no reverved port requested");
    return NULL;
  }
  struct sockaddr_in cli_addr;
  int clilen = sizeof(cli_addr);
  int newsockfd = si_accept(ssiReserved_sockfd, (struct sockaddr *) &cli_addr, (socklen_t *)&clilen);
  if(newsockfd < 0)
  {
    Werror("ERROR on accept (errno=%d)",errno);
    return NULL;
  }
  si_link l=(si_link) omAlloc0Bin(sip_link_bin);
  si_link_extension s = si_link_root;
  si_link_extension prev = s;
  while (strcmp(s->type, "ssi") != 0)
  {
    if (s->next == NULL)
    {
      prev = s;
      s = NULL;
      break;
    }
    else
    {
      s = s->next;
    }
  }
  if (s != NULL)
    l->m = s;
  else
  {
    si_link_extension ns = (si_link_extension)omAlloc0Bin(s_si_link_extension_bin);
    prev->next=slInitSsiExtension(ns);
    l->m = prev->next;
  }
  l->name=omStrDup("");
  l->mode=omStrDup("tcp");
  l->ref=1;
  ssiInfo *d=(ssiInfo*)omAlloc0(sizeof(ssiInfo));
  l->data=d;
  d->fd_read = newsockfd;
  d->fd_write = newsockfd;
  d->f_read = s_open(newsockfd);
  d->f_write = fdopen(newsockfd, "w");
  SI_LINK_SET_RW_OPEN_P(l);
  ssiReserved_Clients--;
  if (ssiReserved_Clients<=0)
  {
    ssiReserved_P=0;
    si_close(ssiReserved_sockfd);
  }
  return l;
}
/*---------------------------------------------------------------------*/
/**
 * @brief additional default signal handler

  // some newer Linux version cannot have SIG_IGN for SIGCHLD,
  // so use this nice routine here:
  //  SuSe 9.x reports -1 always
  //  Redhat 9.x/FC x reports sometimes -1
  // see also: hpux_system
  // also needed by getrusage (timer etc.)

 @param[in] sig
**/
/*---------------------------------------------------------------------*/
void sig_chld_hdl(int sig)
{
  pid_t kidpid;
  int status;

  loop
  {
    kidpid = si_waitpid(-1, &status, WNOHANG);
    if (kidpid==-1)
    {
      /* continue on interruption (EINTR): */
      if (errno == EINTR) continue;
      /* break on anything else (EINVAL or ECHILD according to manpage): */
      break;
    }
    else if (kidpid==0) break; /* no more children to process, so break */

    //printf("Child %ld terminated\n", kidpid);
    link_list hh=ssiToBeClosed;
    while((hh!=NULL)&&(ssiToBeClosed_inactive))
    {
      if((hh->l!=NULL) && (hh->l->m->Open==ssiOpen))
      {
        ssiInfo *d = (ssiInfo *)hh->l->data;
        if(d->pid==kidpid)
        {
          if(ssiToBeClosed_inactive)
          {
            ssiToBeClosed_inactive=FALSE;
            slClose(hh->l);
            ssiToBeClosed_inactive=TRUE;
            break;
          }
          else break;
        }
        else hh=(link_list)hh->next;
      }
      else hh=(link_list)hh->next;
    }
  }
}

static BOOLEAN DumpSsiIdhdl(si_link l, idhdl h)
{
  int type_id = IDTYP(h);

  // C-proc not to be dumped, also LIB-proc not
  if (type_id == PROC_CMD)
  {
    if (IDPROC(h)->language == LANG_C) return FALSE;
    if (IDPROC(h)->libname != NULL) return FALSE;
  }
  // do not dump links
  if (type_id == LINK_CMD) return FALSE;

  // do not dump ssi internal rings: ssiRing*
  if ((type_id == RING_CMD) && (strncmp(IDID(h),"ssiRing",7)==0))
    return FALSE;

  command D=(command)omAlloc0(sizeof(*D));
  sleftv tmp;
  memset(&tmp,0,sizeof(tmp));
  tmp.rtyp=COMMAND;
  tmp.data=D;

  if (type_id == PACKAGE_CMD)
  {
    // do not dump Top
    if (strcmp(IDID(h), "Top") == 0) return FALSE;
    package p=(package)IDDATA(h);
    // dump Singular-packages as load("...");
    if (p->language==LANG_SINGULAR)
    {
      D->op=LOAD_CMD;
      D->argc=1;
      D->arg1.rtyp=STRING_CMD;
      D->arg1.data=p->libname;
      ssiWrite(l,&tmp);
      omFree(D);
      return FALSE;
    }
  }

  // handle qrings separately
  //if (type_id == QRING_CMD)
  //  return DumpSsiQringQring(l, h);

  // put type and name
  //Print("generic dump:%s,%s\n",IDID(h),Tok2Cmdname(IDTYP(h)));
  D->op='=';
  D->argc=2;
  D->arg1.rtyp=DEF_CMD;
  D->arg1.name=IDID(h);
  D->arg2.rtyp=IDTYP(h);
  D->arg2.data=IDDATA(h);
  ssiWrite(l,&tmp);
  omFree(D);
  return FALSE;
}
static BOOLEAN ssiDumpIter(si_link l, idhdl h)
{
  if (h == NULL) return FALSE;

  if (ssiDumpIter(l, IDNEXT(h))) return TRUE;

  // need to set the ring before writing it, otherwise we get in
  // trouble with minpoly
  if (IDTYP(h) == RING_CMD || IDTYP(h) == QRING_CMD)
    rSetHdl(h);

  if (DumpSsiIdhdl(l, h)) return TRUE;

  // do not dump ssi internal rings: ssiRing*
  // but dump objects of all other rings
  if ((IDTYP(h) == RING_CMD || IDTYP(h) == QRING_CMD)
  && (strncmp(IDID(h),"ssiRing",7)!=0))
    return ssiDumpIter(l, IDRING(h)->idroot);
  else
    return FALSE;
}
BOOLEAN ssiDump(si_link l)
{
  idhdl h = IDROOT, rh = currRingHdl;
  BOOLEAN status = ssiDumpIter(l, h);

  //if (! status ) status = DumpAsciiMaps(fd, h, NULL);

  if (currRingHdl != rh) rSetHdl(rh);
  //fprintf(fd, "option(set, intvec(%d, %d));\n", si_opt_1, si_opt_2);

  return status;
}
BOOLEAN ssiGetDump(si_link l)
{
  ssiInfo *d=(ssiInfo*)l->data;
  loop
  {
    if (!SI_LINK_OPEN_P(l)) break;
    if (s_iseof(d->f_read)) break;
    leftv h=ssiRead1(l); /*contains an exit.... */
    if (feErrors != NULL && *feErrors != '\0')
    {
      // handle errors:
      PrintS(feErrors); /* currently quite simple */
      return TRUE;
      *feErrors = '\0';
    }
    h->CleanUp();
    omFreeBin(h, sleftv_bin);
  }
  return FALSE;
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
// 18 intmat
// 19 bigintmat <r> <c> ...
//
// 20 blackbox <name> 1 <len> ...
//
// 98: verify version: <ssi-version> <MAX_TOK> <OPT1> <OPT2>
// 99: quit Singular
