/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/* $Id: ring.cc,v 1.43 1999-02-10 16:00:04 Singular Exp $ */

/*
* ABSTRACT - the interpreter related ring operations
*/

/* includes */
#include <math.h>
#include "mod2.h"
#include "mmemory.h"
#include "tok.h"
#include "polys.h"
#include "numbers.h"
#include "febase.h"
#include "ipid.h"
#include "ipshell.h"
#include "ipconv.h"
#include "intvec.h"
#include "longalg.h"
#include "ffields.h"
#include "spolys.h"
#include "subexpr.h"
#include "ideals.h"
#include "lists.h"
#include "ring.h"

/* global variables */
#ifdef RDEBUG
short rNumber=0;
#endif

/*0 implementation*/
int rBlocks(ring r)
{
  int i=0;
  while (r->order[i]!=0) i++;
  return i+1;
}

// internally changes the gloabl ring and resets the relevant
// global variables:
// complete == FALSE : only delete operations are enabled
// complete == TRUE  : full reset of all variables
#ifdef DRING
void rChangeCurrRing(ring r, BOOLEAN complete, idhdl h)
#else
void rChangeCurrRing(ring r, BOOLEAN complete)
#endif
{
  /*------------ set global ring vars --------------------------------*/
  currRing = r;
  currQuotient=NULL;

  if (r != NULL)
  {
    rTest(r);
    if (complete)
    {
      /*------------ set global ring vars --------------------------------*/
      currQuotient=r->qideal;
      /*------------ set redTail, except reset by nSetChar or pChangeRing */
      test |= Sy_bit(OPT_REDTAIL);
    }

    /*------------ global variables related to coefficients ------------*/
    nSetChar(r->ch, complete, r->parameter, r->P);

    /*------------ global variables related to polys -------------------*/
    pSetGlobals(r, complete);


    if (complete)
    {
    /*------------ set naMinimalPoly -----------------------------------*/
      if (r->minpoly!=NULL)
      {
        naMinimalPoly=((lnumber)r->minpoly)->z;
      }

#ifdef DRING
      pDRING=FALSE;
      pSDRING=FALSE;
      if ((h!=NULL) && (hasFlag(h,FLAG_DRING))) rDSet();
#endif // DRING

#ifdef SRING
      if ((currRing->partN<=currRing->N)
#ifdef DRING
          && ((h==NULL) || (!hasFlag(h,FLAG_DRING)))
#endif
          )
      {
        pAltVars=currRing->partN;
        pSRING=TRUE;
        pSDRING=TRUE;
      }
      else
      {
        pAltVars=currRing->N+1;
      }
#endif // SRING

    /*------------ set spolys ------------------------------------------*/
      spSet(r);
    }
  }
}

void rSetHdl(idhdl h, BOOLEAN complete)
{
  int i;
  ring rg = NULL;
  if (h!=NULL)
  {
    rg = IDRING(h);
    mmTestP((ADDRESS)h,sizeof(idrec));
    mmTestLP((ADDRESS)IDID(h));
    mmTestP(rg,sizeof(ip_sring));
#ifdef MDEBUG
    i=rBlocks(rg);
#endif
    mmTestP(rg->order,i*sizeof(int));
    mmTestP(rg->block0,i*sizeof(int));
    mmTestP(rg->block1,i*sizeof(int));
    mmTestP(rg->wvhdl,i*sizeof(short *));
  }
  else complete=FALSE;

  // clean up history
    if (((sLastPrinted.rtyp>BEGIN_RING) && (sLastPrinted.rtyp<END_RING))
        || ((sLastPrinted.rtyp==LIST_CMD)&&(lRingDependend((lists)sLastPrinted.data))))
    {
      sLastPrinted.CleanUp();
      memset(&sLastPrinted,0,sizeof(sleftv));
    }

   /*------------ change the global ring -----------------------*/
  #ifdef DRING
  rChangeCurrRing(rg,complete,h);
  #else
  rChangeCurrRing(rg,complete);
  #endif
  currRingHdl = h;

    /*------------ set pShortOut -----------------------*/
  if (complete /*&&(h!=NULL)*/)
  {
    #ifdef HAVE_TCL
    if (tclmode)
    {
      PrintTCLS('R',IDID(h));
      pShortOut=(int)FALSE;
    }
    else
    #endif
    {
      pShortOut=(int)TRUE;
      if ((rg->parameter!=NULL) && (rg->ch<2))
      {
        for (i=0;i<rg->P;i++)
        {
          if(strlen(rg->parameter[i])>1)
          {
            pShortOut=(int)FALSE;
            break;
          }
        }
      }
      if (pShortOut)
      {
        for (i=(rg->N-1);i>=0;i--)
        {
          if(strlen(rg->names[i])>1)
          {
            pShortOut=(int)FALSE;
            break;
          }
        }
      }
    }
  }

}

idhdl rDefault(char *s)
{
  idhdl tmp=NULL;

  if (s!=NULL) tmp = enterid(s, myynest, RING_CMD, &IDROOT);
  if (tmp==NULL) return NULL;

  if (ppNoether!=NULL) pDelete(&ppNoether);
  if ((sLastPrinted.rtyp>BEGIN_RING) && (sLastPrinted.rtyp<END_RING))
  {
    sLastPrinted.CleanUp();
    memset(&sLastPrinted,0,sizeof(sleftv));
  }

  currRing = IDRING(tmp);

  currRing->ch    = 32003;
  currRing->N     = 3;
  /*currRing->P     = 0; Alloc0 in idhdl::set, ipid.cc*/
#ifdef RDEBUG
  rNumber++;
  currRing->no    =rNumber;
#endif
#ifdef SRING
  currRing->partN = 4;
#endif
  /*names*/
  currRing->names = (char **) Alloc(3 * sizeof(char *));
  currRing->names[0]  = mstrdup("x");
  currRing->names[1]  = mstrdup("y");
  currRing->names[2]  = mstrdup("z");
  /*weights: entries for 3 blocks: NULL*/
  currRing->wvhdl = (short **)Alloc0(3 * sizeof(short *));
  /*order: dp,C,0*/
  currRing->order = (int *) Alloc(3 * sizeof(int *));
  currRing->block0 = (int *)Alloc(3 * sizeof(int *));
  currRing->block1 = (int *)Alloc(3 * sizeof(int *));
  /* ringorder dp for the first block: var 1..3 */
  currRing->order[0]  = ringorder_dp;
  currRing->block0[0] = 1;
  currRing->block1[0] = 3;
  /* ringorder C for the second block: no vars */
  currRing->order[1]  = ringorder_C;
  currRing->block0[1] = 0;
  currRing->block1[1] = 0;
  /* the last block: everything is 0 */
  currRing->order[2]  = 0;
  currRing->block0[2] = 0;
  currRing->block1[2] = 0;
  /*polynomial ring*/
  currRing->OrdSgn    = 1;

  /* complete ring intializations */
  rComplete(currRing);
  rSetHdl(tmp,TRUE);
  return currRingHdl;
}

/*2
 *check intvec, describing the ordering
 */
BOOLEAN rCheckIV(intvec *iv)
{
  if ((iv->length()!=2)&&(iv->length()!=3))
  {
    WerrorS("weights only for orderings wp,ws,Wp,Ws,a,M");
    return TRUE;
  }
  return FALSE;
}

static int rTypeOfMatrixOrder(intvec * order)
{
  int i=0,j,typ=1;
  int sz = (int)sqrt((double)(order->length()-2));

  while ((i<sz) && (typ==1))
  {
    j=0;
    while ((j<sz) && ((*order)[j*sz+i+2]==0)) j++;
    if (j>=sz)
    {
      typ = 0;
      WerrorS("Matrix order not complete");
    }
    else if ((*order)[j*sz+i+2]<0)
      typ = -1;
    else
      i++;
  }
  return typ;
}

/*2
 * define a new ring from the data:
 *s: name, chr: ch, parameter names (or NULL): pn,
 *varnames: rv, ordering: ord, typ: typ
 */
idhdl rInit(char *s, sleftv* pn, sleftv* rv, sleftv* ord,
            BOOLEAN isDRing)
{
  int ch;
  if (pn->Typ()==INT_CMD)
  {
    ch=(int)pn->Data();
  }
  else if (strcmp(pn->name,"real")==0)
  {
    ch=-1;
  }
  else
  {
    return NULL;
  }
  pn=pn->next;

  int l, last;
  int typ = 1;
  sleftv * sl;
  idhdl tmp;
  ip_sring tmpR;
  BOOLEAN ffChar=FALSE;
  /*every entry in the new ring is initialized to 0*/

  /* characteristic -----------------------------------------------*/
  /* input: 0 ch=0 : Q     parameter=NULL    ffChar=FALSE
   *         0    1 : Q(a,...)        *names         FALSE
   *         0   -1 : R               NULL           FALSE
   *         p    p : Fp              NULL           FALSE
   *         p   -p : Fp(a)           *names         FALSE
   *         q    q : GF(q=p^n)       *names         TRUE
   */
  if (ch!=-1)
  {
    if ((ch!=0) &&((ch<2) || (ch > 32003)))
    {
      ch=32003;
    }
    l=0;
    if (pn!=NULL)
    {
      while ((ch!=fftable[l]) && (fftable[l])) l++;
      if (fftable[l]==0)
      {
        ch = IsPrime(ch);
      }
      else
      {
        char *m[1]={(char *)sNoName};
        nfSetChar(ch,m);
        if(errorreported)
        {
          return NULL;
        }
        else
        {
          ffChar=TRUE;
        }
      }
    }
    else
    {
      ch = IsPrime(ch);
    }
  }
  memset(&tmpR,0,sizeof(tmpR));

  tmpR.ch = ch;

  /* parameter -------------------------------------------------------*/
  sleftv* hs;
  const char* h;

  if ((pn!=NULL)&& (ffChar||(ch==-1)))
  {
    if((ffChar && (pn->next!=NULL))
       || (ch==-1))
    {
      WarnS("too many parameters");
      if (ffChar) hs=pn->next;
      else hs=pn;
      hs->CleanUp();
      if (ffChar)
      {
        pn->next=NULL;
        Free((ADDRESS)hs,sizeof(sleftv));
      }
      else pn=NULL;
    }
  }
  /* a tempory pointer for typ conversion
   * and for deallocating sleftv*-lists:
   *  don't deallocate the first but all other entries*/

  if (pn!=NULL)
  {
    tmpR.P=pn->listLength();
    if((ffChar && (tmpR.P>1))
       || ((ch==-1) && (tmpR.P>0)))
    {
      tmpR.P=ffChar; /* GF(q): 1, R: 0 */
      WarnS("too many parameters");
      if (ffChar) hs=pn->next;
      else hs=pn;
      hs->CleanUp();
      Free((ADDRESS)hs,sizeof(sleftv));
      if (ffChar) pn->next=NULL;
      else pn=NULL;
    }
    tmpR.parameter=(char**)Alloc(tmpR.P*sizeof(char *));
    sl=pn;
    char** p=tmpR.parameter;
    while(sl!=NULL)
    {
      hs=NULL;
      h=sl->Name();
      if ((h==sNoName)&&(sl->Typ()==POLY_CMD))
      {
        hs=(leftv)Alloc(sizeof(sleftv));
        iiConvert(POLY_CMD,ANY_TYPE,-1,sl,hs);
        sl->next=hs->next;
        hs->next=NULL;
        h=hs->Name();
      }
      if (h==sNoName)
      {
        WerrorS("parameter expected");
        return NULL;
      }
      *p=mstrdup(h);
      p++;
      if (hs!=NULL)
      {
        hs->CleanUp();
        Free((ADDRESS)hs,sizeof(sleftv));
      }
      hs=sl;
      sl=sl->next;
      hs->next=NULL;
      hs->CleanUp();
      if (hs!=pn) Free((ADDRESS)hs,sizeof(sleftv));
    }
    if ((ch>1) && /*(pn!=NULL) &&*/ (!ffChar)) tmpR.ch=-tmpR.ch;
    if (ch==0) tmpR.ch=1;
  }

  /* names and number of variables-------------------------------------*/
  {
    int i, n;
    sl = rv;
#ifdef DRING
    char *tmpname=NULL;
#endif
    n=rv->listLength();
    tmpR.N = n;
#ifdef SDRING
    tmpR.partN=n+1-isDRing; // set to N+1 for SRING, N for DRING
      if (isDRing) n=2*n+1;
#endif
    tmpR.N = n;
    tmpR.names   = (char **)Alloc(n * sizeof(char *));
    for (sl=rv, i=0; i<n; i++)
    {
      hs=NULL;
#ifdef DRING
      if (sl==NULL)
      {
        if (i==tmpR.N-1)
          tmpname=mstrdup("");
        else
        {
          tmpname=(char*)AllocL(strlen(tmpR.names[i-tmpR.partN])+2);
          strcpy(tmpname,"d");
          strcat(tmpname,tmpR.names[i-tmpR.partN]);
        }
        h=tmpname;
      }
      else
#endif
        h=sl->Name();
      if ((h==sNoName)&&(sl->Typ()==POLY_CMD))
      {
        hs=(leftv)Alloc(sizeof(sleftv));
        iiConvert(POLY_CMD,ANY_TYPE,-1,sl,hs);
        sl->next=hs->next;
        hs->next=NULL;
        h=hs->Name();
      }
      if (h==sNoName)
      {
        WerrorS("expected name of ring variable");
        return NULL;
      }
      tmpR.names[i] = mstrdup(h);
      if (hs!=NULL)
      {
        hs->CleanUp();
        Free((ADDRESS)hs,sizeof(sleftv));
      }
      hs=sl;
#ifdef DRING
      if (sl!=NULL)
      {
#endif
        sl=sl->next;
        hs->next=NULL;
        hs->CleanUp();
        if (hs!=rv) Free((ADDRESS)hs,sizeof(sleftv));
#ifdef DRING
      }
      if (tmpname!=NULL)
      {
        FreeL((ADDRESS)tmpname);
        tmpname=NULL;
      }
#endif
    }

    /* ordering -------------------------------------------------------------*/
    sl = ord;
    /* the number of orderings*/
    n = 1; i=0;
    int o=0;
    while (sl!=NULL)
    {
      intvec *iv = (intvec *)(sl->data);
      if (((*iv)[1]==ringorder_c)||((*iv)[1]==ringorder_C)) i++;
      else if ((*iv)[1]!=ringorder_a) o++;
      n++;
      sl=sl->next;
    }
    if (o==0)
    {
      WerrorS("invalid combination of orderings");
      return NULL;
    }
    if (i==0) n++;
    else if (i!=1)
      WarnS("more than one ordering c/C -- ignored");

    /* allocating */
    tmpR.order=(int *)Alloc0(n*sizeof(int));
    tmpR.block0=(int *)Alloc0(n*sizeof(int));
    tmpR.block1=(int *)Alloc0(n*sizeof(int));
    tmpR.wvhdl=(short**)Alloc0(n*sizeof(short*));

    /* init orders */
    sl=ord;
    n=0;
    last=0;
    while (sl!=NULL)
    {
      intvec *iv;
      iv = (intvec *)(sl->data);

      /* the format of an ordering:
       *  iv[0]: factor
       *  iv[1]: ordering
       *  iv[2..end]: weights
       */
      tmpR.order[n] = (*iv)[1];
      switch ((*iv)[1])
      {
      case ringorder_ws:
      case ringorder_Ws:
        typ=-1;
      case ringorder_wp:
      case ringorder_Wp:
        tmpR.wvhdl[n]=(short*)AllocL((iv->length()-1)*sizeof(short));
        for (l=2;l<iv->length();l++)
          tmpR.wvhdl[n][l-2]=(short)(*iv)[l];
        tmpR.block0[n]=last+1;
        last+=iv->length()-2;
        tmpR.block1[n]=last;
        break;
      case ringorder_ls:
      case ringorder_ds:
      case ringorder_Ds:
        typ=-1;
      case ringorder_lp:
      case ringorder_dp:
      case ringorder_Dp:
        tmpR.block0[n]=last+1;
        //last+=(*iv)[0];
        if (iv->length()==3) last+=(*iv)[2];
        else last+=(*iv)[0];
        tmpR.block1[n]=last;
        if (rCheckIV(iv)) return NULL;
        break;
      case ringorder_c:
      case ringorder_C:
        if (rCheckIV(iv)) return NULL;
        break;
      case ringorder_a:
        tmpR.block0[n]=last+1;
        tmpR.block1[n]=last+iv->length()-2;
        tmpR.wvhdl[n]=(short*)AllocL((iv->length()-1)*sizeof(short));
        for (l=2;l<iv->length();l++)
        {
          tmpR.wvhdl[n][l-2]=(short)(*iv)[l];
          if ((*iv)[l]<0) typ=-1;
        }
        break;
      case ringorder_M:
        {
          int Mtyp=rTypeOfMatrixOrder(iv);
          if (Mtyp==0) return NULL;
          if (Mtyp==-1) typ=-1;
          tmpR.wvhdl[n]=(short*)AllocL((iv->length()-1)*sizeof(short));
          for (l=2;l<iv->length();l++)
            tmpR.wvhdl[n][l-2]=(short)(*iv)[l];
          tmpR.block0[n]=last+1;
          last+=(int)sqrt((double)(iv->length()-2));
          tmpR.block1[n]=last;
          break;
        }
#ifdef TEST
      default:
        Print("order ??? %d\n",(*iv)[1]);
        break;
#endif
      }
      sl=sl->next;
      n++;
    }
    ord->CleanUp();
    if (i==0)
    {
      /*there is no c/C-ordering, so append it at the end*/
      tmpR.order[n]=ringorder_C;
    }
    else n--;
    while ((tmpR.order[n]==ringorder_c)
           ||(tmpR.order[n]==ringorder_C))
      n--;
    if (tmpR.block1[n]!=tmpR.N)
    {
      if ((tmpR.order[n]==ringorder_dp) ||
          (tmpR.order[n]==ringorder_ds) ||
          (tmpR.order[n]==ringorder_Dp) ||
          (tmpR.order[n]==ringorder_Ds) ||
          (tmpR.order[n]==ringorder_lp) ||
          (tmpR.order[n]==ringorder_ls))
      {
        tmpR.block1[n]=tmpR.N;
        if (tmpR.block0[n]>tmpR.N/*tmpR.block1[n]*/)
        {
          tmpR.block1[n]=tmpR.block0[n];
          goto ord_mismatch;
        }
      }
      else
      {
      ord_mismatch:
        Werror("mismatch of number of vars (%d) and ordering (%d vars)",
               tmpR.N,tmpR.block1[n]);
        return NULL;
      }
    }
  }
  tmpR.OrdSgn = typ;
  // Complete the initialization
  rComplete(&tmpR);
  /* try to enter the ring into the name list*/
  if ((tmp = enterid(s, myynest, RING_CMD, &IDROOT))==NULL)
  {
    return NULL;
  }

  memcpy(IDRING(tmp),&tmpR,sizeof(tmpR));
  if (isDRing) setFlag(tmp,FLAG_DRING);
  rSetHdl(tmp,TRUE);

#ifdef RDEBUG
  rNumber++;
  currRing->no    =rNumber;
#endif

  return currRingHdl;
}

// set those fields of the ring, which can be computed from other fields:
// More particularly, sets r->VarOffset

void rComplete(ring r, int force)
{
  int VarCompIndex, VarLowIndex, VarHighIndex;
  
  r->VarOffset = (int*) Alloc((r->N + 1)*sizeof(int));
  pGetVarIndicies(r, r->VarOffset, VarCompIndex, 
                  VarLowIndex, VarHighIndex);
  r->VarCompIndex = VarCompIndex;
  r->VarLowIndex = VarLowIndex;
  r->VarHighIndex = VarHighIndex;
}

/*2
 * set a new ring from the data:
 s: name, chr: ch, varnames: rv, ordering: ord, typ: typ
 */
#ifdef DRING
void rDSet()
{
  pDRING=TRUE;
  pSDRING=TRUE;
  pdN=currRing->partN;
  pdK=pVariables-pdN*2-1;
}
#endif

int rIsRingVar(char *n)
{
  if ((currRing!=NULL) && (currRing->names!=NULL))
  {
    for (int i=0; i<currRing->N; i++)
    {
      if (currRing->names[i]==NULL) return -1;
      if (strcmp(n,currRing->names[i]) == 0) return (int)i;
    }
  }
  return -1;
}

char* RingVar(short i)
{
  return currRing->names[i];
}

void rWrite(ring r)
{
  if ((r==NULL)||(r->order==NULL))
    return; /*to avoid printing after errors....*/

  int nblocks=rBlocks(r);

  mmTestP(r,sizeof(ip_sring));
  mmTestP(r->order,nblocks*sizeof(int));
  mmTestP(r->block0,nblocks*sizeof(int));
  mmTestP(r->block1,nblocks*sizeof(int));
  mmTestP(r->wvhdl,nblocks*sizeof(short *));
  mmTestP(r->names,r->N*sizeof(char *));

  nblocks--;


  if ((r->parameter!=NULL)&&(r->ch>1))
    PrintS("//   # ground field : ");
  else
    PrintS("//   characteristic : ");
  if (r->ch==-1)     PrintS("0 (real)\n");  /* R */
  else if (r->ch<0)  Print ("%d\n",-r->ch); /* Fp(a) */
  else if (r->ch==1) PrintS("0\n");         /* Q(a)  */
  else               Print ("%d\n",r->ch);  /* Fq, Fp, Q */
  if (r->parameter!=NULL)
  {
    if (r->ch<2)
    {
      Print ("//   %d parameter    : ",rPar(r));
      char **sp=r->parameter;
      int nop=0;
      while (nop<r->P)
      {
        PrintS(*sp);
        PrintS(" ");
        sp++; nop++;
      }
      PrintS("\n//   minpoly        : ");
      if (r==currRing)
      {
        StringSetS(""); nWrite(r->minpoly); PrintS(StringAppendS("\n"));
      }
      else if (r->minpoly==NULL)
      {
        PrintS("0\n");
      }
      else
      {
        PrintS("...\n");
      }
    }
    else
    {
      Print("//   primitive element : %s\n", r->parameter[0]);
      if (r==currRing)
      {
        StringSetS("//   minpoly        : ");
        nfShowMipo();PrintS(StringAppend("\n"));
      }
    }
  }
  Print("//   number of vars : %d",r->N);

  //for (nblocks=0; r->order[nblocks]; nblocks++);
  nblocks=rBlocks(r)-1;

  for (int l=0, nlen=0 ; l<nblocks; l++)
  {
    int i;
    Print("\n//        block %3d : ",l+1);

    Print("ordering %c", (" acCMldDwWldDwWu")[r->order[l]]);
    if ((r->order[l]>=ringorder_lp)&&(r->order[l]!=ringorder_unspec))
    {
      if (r->order[l]>=ringorder_ls)
        PrintS("s");
      else
        PrintS("p");
    }

    if ((r->order[l] != ringorder_c) && (r->order[l] != ringorder_C))
    {
      PrintS("\n//                  : names    ");
      for (i = r->block0[l]-1; i<r->block1[l]; i++)
      {
        nlen = strlen(r->names[i]);
        Print("%s ",r->names[i]);
      }
    }

    if (r->wvhdl[l]!=NULL)
    {
      for (int j= 0;
           j<(r->block1[l]-r->block0[l]+1)*(r->block1[l]-r->block0[l]+1);
           j+=i)
      {
        PrintS("\n//                  : weights  ");
        for (i = 0; i<=r->block1[l]-r->block0[l]; i++)
        {
          Print("%*d " ,nlen,r->wvhdl[l][i+j],i+j);
        }
        if (r->order[l]!=ringorder_M) break;
      }
    }
  }
  if (r->qideal!=NULL)
  {
    PrintS("\n// quotient ring from ideal");
    if (r==currRing)
    {
      PrintLn();
      iiWriteMatrix((matrix)r->qideal,"_",1);
    }
    else PrintS(" ...");
  }
}

void rKill(ring r)
{
  rTest(r);
  if ((r->ref<=0)&&(r->order!=NULL))
  {
#ifdef RDEBUG
    if (traceit &TRACE_SHOW_RINGS) Print("kill ring %d\n",r->no);
#endif
    if (r==currRing)
    {
      if (r->qideal!=NULL)
      {
        idDelete(&r->qideal);
        r->qideal=NULL;
        currQuotient=NULL;
      }
      if (ppNoether!=NULL) pDelete(&ppNoether);
      if ((sLastPrinted.rtyp>BEGIN_RING) && (sLastPrinted.rtyp<END_RING))
      {
        sLastPrinted.CleanUp();
        memset(&sLastPrinted,0,sizeof(sleftv));
      }
      currRing=NULL;
      currRingHdl=NULL;
    }
    else if (r->qideal!=NULL)
    {
      ring savecurrRing = currRing;
      rChangeCurrRing((ring)r,FALSE);
      idDelete(&r->qideal);
      r->qideal=NULL;
      rChangeCurrRing(savecurrRing,FALSE);
    }
    int i=1;
    int j;
    int *pi=r->order;
#ifdef USE_IILOCALRING
    for (j=0;j<iiRETURNEXPR_len;j++)
    {
      if (iiLocalRing[j]==r)
      {
        if (j<myynest) Warn("killing the basering for level %d",j);
        iiLocalRing[j]=NULL;
      }
    }
#else /* USE_IILOCALRING */
    {
      namehdl nshdl = namespaceroot;

      for(nshdl=namespaceroot; nshdl->isroot != TRUE; nshdl = nshdl->next) {
        //Print("NSstack: %s:%d, nesting=%d\n", nshdl->name, nshdl->lev, nshdl->myynest);
        if (nshdl->currRing==r)
        {
          if (nshdl->myynest<myynest)
//            Warn("killing the basering for level %d/%d",nshdl->lev,nshdl->myynest);
          Warn("killing the basering for level %d",nshdl->myynest);
          nshdl->currRing=NULL;
        }
      }
      if (nshdl->currRing==r)
      {
        //Print("NSstack: %s:%d, nesting=%d\n", nshdl->name, nshdl->lev, nshdl->myynest);
        if (nshdl->myynest<myynest)
//          Warn("killing the basering for level %d/%d",nshdl->lev,nshdl->myynest);
          Warn("killing the basering for level %d",nshdl->myynest);
        nshdl->currRing=NULL;
      }
    }
#endif /* USE_IILOCALRING */
    if (pi!=NULL)
    {
      //while(*pi!=0) { pi++;i++; }
      i=rBlocks(r);
      Free((ADDRESS)r->order,i*sizeof(int));
      Free((ADDRESS)r->block0,i*sizeof(int));
      Free((ADDRESS)r->block1,i*sizeof(int));
      for (j=0; j<i; j++)
      {
        if (r->wvhdl[j]!=NULL)
          FreeL(r->wvhdl[j]);
      }
      Free((ADDRESS)r->wvhdl,i*sizeof(short *));
      if(r->names!=NULL)
      {
        for (i=0; i<r->N; i++)
        {
          FreeL((ADDRESS)r->names[i]);
        }
        Free((ADDRESS)r->names,r->N*sizeof(char *));
      }
      if (r->parameter!=NULL)
      {
        int len=0;
        char **s=r->parameter;
        while (len<r->P)
        {
          FreeL((ADDRESS)*s);
          s++;
          len++;
        }
        Free((ADDRESS)r->parameter,r->P*sizeof(char *));
      }
      Free((ADDRESS)r->VarOffset, (r->N +1)*sizeof(int));
    }
#ifdef TEST
    else
      PrintS("internal error: ring structure destroyed\n");
    memset(r,0,sizeof(ip_sring));
#endif
    Free((ADDRESS)r,sizeof(ip_sring));
    return;
  }
  r->ref--;
}

void rKill(idhdl h)
{
#ifndef HAVE_NAMESPACES1
  ring r = IDRING(h);
  if (r!=NULL) rKill(r);
  if (h==currRingHdl)
  {
#ifdef HAVE_NAMESPACES
    namehdl nsHdl = namespaceroot;
    while(nsHdl!=NULL) {
      currRingHdl=NSROOT(nsHdl);
#else /* HAVE_NAMESPACES */
      currRingHdl=IDROOT;
#endif /* HAVE_NAMESPACES */
      while (currRingHdl!=NULL)
      {
        if ((currRingHdl!=h)
            && (IDTYP(currRingHdl)==IDTYP(h))
            && (h->data.uring==currRingHdl->data.uring))
          break;
        currRingHdl=IDNEXT(currRingHdl);
      }
#ifdef HAVE_NAMESPACES
      if ((currRingHdl != NULL) && (currRingHdl!=h)
          && (IDTYP(currRingHdl)==IDTYP(h))
          && (h->data.uring==currRingHdl->data.uring))
        break;
      nsHdl = nsHdl->next;
    }
#endif /* HAVE_NAMESPACES */
  }
#else
    if(currRingHdl==NULL) {
      namehdl ns = namespaceroot;
      BOOLEAN found=FALSE;

      while(!ns->isroot) {
        currRingHdl=NSROOT(namespaceroot->next);
        while (currRingHdl!=NULL)
        {
          if ((currRingHdl!=h)
              && (IDTYP(currRingHdl)==IDTYP(h))
              && (h->data.uring==currRingHdl->data.uring))
          { found=TRUE; break; }

          currRingHdl=IDNEXT(currRingHdl);
        }
        if(found) break;
        ns=IDNEXT(ns);
      }
    }
    if(currRingHdl == NULL || IDRING(h) != IDRING(currRingHdl)) {
      currRingHdl = namespaceroot->currRingHdl;

/*      PrintS("Running rFind()\n");
      currRingHdl = rFindHdl(IDRING(h), NULL, NULL);
      if(currRingHdl == NULL)
      {
        PrintS("rFind()return 0\n");
      }
      else
      {
        PrintS("Huppi rfind return an currRingHDL\n");
        Print("%x, %x\n", IDRING(h),IDRING(currRingHdl) );
      }
*/
    }
    else
    {
      //PrintS("Huppi found an currRingHDL\n");
      //Print("%x, %x\n", IDRING(h),IDRING(currRingHdl) );

    }
#endif /* HAVE_NAMESPACES */
}

idhdl rFindHdl(ring r, idhdl n, idhdl w)
{
#ifdef HAVE_NAMESPACES
  idhdl h;
  namehdl ns = namespaceroot;

  while(!ns->isroot) {
    h = NSROOT(ns);
    if(w != NULL) h = w;
    while (h!=NULL)
    {
      if (((IDTYP(h)==RING_CMD)||(IDTYP(h)==QRING_CMD))
          && (h->data.uring==r)
          && (h!=n))
        return h;
      h=IDNEXT(h);
    }
    ns = ns->next;
  }
  h = NSROOT(ns);
  if(w != NULL) h = w;
  while (h!=NULL)
  {
    if (((IDTYP(h)==RING_CMD)||(IDTYP(h)==QRING_CMD))
        && (h->data.uring==r)
        && (h!=n))
      return h;
    h=IDNEXT(h);
  }
#if 0
  if(namespaceroot->isroot) h = IDROOT;
  else h = NSROOT(namespaceroot->next);
  if(w != NULL) h = w;
  while (h!=NULL)
  {
    if (((IDTYP(h)==RING_CMD)||(IDTYP(h)==QRING_CMD))
        && (h->data.uring==r)
        && (h!=n))
      return h;
    h=IDNEXT(h);
  }
#endif
#else
  idhdl h=IDROOT;
  if(w != NULL) h = w;
  while (h!=NULL)
  {
    if (((IDTYP(h)==RING_CMD)||(IDTYP(h)==QRING_CMD))
        && (h->data.uring==r)
        && (h!=n))
      return h;
    h=IDNEXT(h);
  }
#endif
  return NULL;
}

int rOrderName(char * ordername)
{
  int order=0;

  switch (*ordername)
  {
  case 'l':
    if (*(ordername+1)=='p') order = ringorder_lp;
    else if (*(ordername+1)=='s') order = ringorder_ls;
    break;
  case 'd':
    if (*(ordername+1)=='p') order = ringorder_dp;
    else if (*(ordername+1)=='s') order = ringorder_ds;
    break;
  case 'w':
    if (*(ordername+1)=='p') order = ringorder_wp;
    else if (*(ordername+1)=='s') order = ringorder_ws;
    break;
  case 'D':
    if (*(ordername+1)=='p') order = ringorder_Dp;
    else if (*(ordername+1)=='s') order = ringorder_Ds;
    break;
  case 'W':
    if (*(ordername+1)=='p') order = ringorder_Wp;
    else if (*(ordername+1)=='s') order = ringorder_Ws;
    break;
  case 'c': order = ringorder_c; break;
  case 'C': order = ringorder_C; break;
  case 'a': order = ringorder_a; break;
  case 'M': order = ringorder_M; break;
  default: break;
  }
  if (order==0) Werror("wrong ring order `%s`",ordername);
  FreeL((ADDRESS)ordername);
  return order;
}

char * rOrdStr(ring r)
{
  int nblocks,l,i;

  for (nblocks=0; r->order[nblocks]; nblocks++);
  nblocks--;

  StringSetS("");
  for (l=0; ; l++)
  {
    StringAppend("%c",(" acCMldDwWldDwW")[r->order[l]]);
    if (r->order[l]>=ringorder_lp)
    {
      if (r->order[l]>=ringorder_ls)
        StringAppendS("s");
      else
        StringAppendS("p");
    }
    if ((r->order[l] != ringorder_c) && (r->order[l] != ringorder_C))
    {
      if (r->wvhdl[l]!=NULL)
      {
        StringAppendS("(");
        for (int j= 0;
             j<(r->block1[l]-r->block0[l]+1)*(r->block1[l]-r->block0[l]+1);
             j+=i+1)
        {
          char c=',';
          for (i = 0; i<r->block1[l]-r->block0[l]; i++)
          {
            StringAppend("%d," ,r->wvhdl[l][i+j]);
          }
          if (r->order[l]!=ringorder_M)
          {
            StringAppend("%d)" ,r->wvhdl[l][i+j]);
            break;
          }
          if (j+i+1==(r->block1[l]-r->block0[l]+1)*(r->block1[l]-r->block0[l]+1))
            c=')';
          StringAppend("%d%c" ,r->wvhdl[l][i+j],c);
        }
      }
      else
        StringAppend("(%d)",r->block1[l]-r->block0[l]+1);
    }
    if (l==nblocks) return mstrdup(StringAppendS(""));
    StringAppendS(",");
  }
}

char * rVarStr(ring r)
{
  int i;
  int l=2;
  char *s;

  for (i=0; i<r->N; i++)
  {
    l+=strlen(r->names[i])+1;
  }
  s=(char *)AllocL(l);
  s[0]='\0';
  for (i=0; i<r->N-1; i++)
  {
    strcat(s,r->names[i]);
    strcat(s,",");
  }
  strcat(s,r->names[i]);
  return s;
}

char * rCharStr(ring r)
{
  char *s;
  int i;

  if (r->parameter==NULL)
  {
    i=r->ch;
    if(i==-1)
      s=mstrdup("real");                    /* R */
    else
    {
      s=(char *)AllocL(6);
      sprintf(s,"%d",i);                   /* Q, Z/p */
    }
    return s;
  }
  int l=0;
  for(i=0; i<r->P;i++)
  {
    l+=(strlen(r->parameter[i])+1);
  }
  s=(char *)AllocL(l+6);
  s[0]='\0';
  if (r->ch<0)       sprintf(s,"%d",-r->ch); /* Fp(a) */
  else if (r->ch==1) sprintf(s,"0");         /* Q(a)  */
  else
  {
    sprintf(s,"%d,%s",r->ch,r->parameter[0]); /* Fq  */
    return s;
  }
  char tt[2];
  tt[0]=',';
  tt[1]='\0';
  for(i=0; i<r->P;i++)
  {
    strcat(s,tt);
    strcat(s,r->parameter[i]);
  }
  return s;
}

char * rParStr(ring r)
{
  if (r->parameter==NULL) return mstrdup("");

  int i;
  int l=2;

  for (i=0; i<r->P; i++)
  {
    l+=strlen(r->parameter[i])+1;
  }
  char *s=(char *)AllocL(l);
  s[0]='\0';
  for (i=0; i<r->P-1; i++)
  {
    strcat(s,r->parameter[i]);
    strcat(s,",");
  }
  strcat(s,r->parameter[i]);
  return s;
}

char * rString(ring r)
{
  char *ch=rCharStr(r);
  char *var=rVarStr(r);
  char *ord=rOrdStr(r);
  char *res=(char *)AllocL(strlen(ch)+strlen(var)+strlen(ord)+9);
  sprintf(res,"(%s),(%s),(%s)",ch,var,ord);
  FreeL((ADDRESS)ch);
  FreeL((ADDRESS)var);
  FreeL((ADDRESS)ord);
  return res;
}

int rChar(ring r)
{
  if (r->ch==-1)
    return 0;
  if (r->parameter==NULL) /* Q, Fp */
    return r->ch;
  if (r->ch<0)           /* Fp(a)  */
    return -r->ch;
  if (r->ch==1)          /* Q(a)  */
    return 0;
  /*else*/               /* GF(p,n) */
  {
    if ((r->ch & 1)==0) return 2;
    int i=3;
    while ((r->ch % i)!=0) i+=2;
    return i;
  }
}

int    rIsExtension(ring r)
{
  if (r->parameter==NULL) /* Q, Fp */
    return FALSE;
  else
    return TRUE;
}

int    rIsExtension()
{
  return rIsExtension( currRing );
}

/*2
 *returns -1 for not compatible, (sum is undefined)
 *         0 for equal, (and sum)
 *         1 for compatible (and sum)
 */
int rSum(ring r1, ring r2, ring &sum)
{
  if (r1==r2)
  {
    sum=r1;
    r1->ref++;
    return 0;
  }
  ip_sring tmpR;
  memset(&tmpR,0,sizeof(tmpR));
  /* check coeff. field =====================================================*/
  if (r1->ch==r2->ch)
  {
    tmpR.ch=r1->ch;
    if ((r1->ch==0)||(r1->ch>=2)) /* Q, Z/p, GF(p,n) */
    {
      if (r1->parameter!=NULL)
      {
        if (strcmp(r1->parameter[0],r2->parameter[0])==0) /* 1 char */
        {
          tmpR.parameter=(char **)Alloc(sizeof(char *));
          tmpR.parameter[0]=mstrdup(r1->parameter[0]);
          tmpR.P=1;
        }
        else
        {
          WerrorS("GF(p,n)+GF(p,n)");
          return -1;
        }
      }
    }
    else if ((r1->ch==1)||(r1->ch<-1)) /* Q(a),Z/p(a) */
    {
      if (r1->minpoly!=NULL)
      {
        if (r2->minpoly!=NULL)
        {
          nSetChar(r1->ch,TRUE,r1->parameter,r1->P);
          if ((strcmp(r1->parameter[0],r2->parameter[0])==0) /* 1 char */
              && naEqual(r1->minpoly,r2->minpoly))
          {
            tmpR.parameter=(char **)Alloc(sizeof(char *));
            tmpR.parameter[0]=mstrdup(r1->parameter[0]);
            tmpR.minpoly=naCopy(r1->minpoly);
            tmpR.P=1;
            nSetChar(currRing->ch,TRUE,currRing->parameter,currRing->P);
          }
          else
          {
            nSetChar(currRing->ch,TRUE,currRing->parameter,currRing->P);
            WerrorS("different minpolys");
            return -1;
          }
        }
        else
        {
          if ((strcmp(r1->parameter[0],r2->parameter[0])==0) /* 1 char */
              && (r2->P==1))
          {
            tmpR.parameter=(char **)Alloc0(sizeof(char *));
            tmpR.parameter[0]=mstrdup(r1->parameter[0]);
            tmpR.P=1;
            nSetChar(r1->ch,TRUE,r1->parameter,r1->P);
            tmpR.minpoly=naCopy(r1->minpoly);
            nSetChar(currRing->ch,TRUE,currRing->parameter,currRing->P);
          }
          else
          {
            WerrorS("different parameters and minpoly!=0");
            return -1;
          }
        }
      }
      else /* r1->minpoly==NULL */
      {
        if (r2->minpoly!=NULL)
        {
          if ((strcmp(r1->parameter[0],r2->parameter[0])==0) /* 1 char */
              && (r1->P==1))
          {
            tmpR.parameter=(char **)Alloc(sizeof(char *));
            tmpR.parameter[0]=mstrdup(r1->parameter[0]);
            tmpR.P=1;
            nSetChar(r2->ch,TRUE,r2->parameter,r2->P);
            tmpR.minpoly=naCopy(r2->minpoly);
            nSetChar(currRing->ch,TRUE,currRing->parameter,currRing->P);
          }
          else
          {
            WerrorS("different parameters and minpoly!=0");
            return -1;
          }
        }
        else
        {
          int len=rPar(r1)+rPar(r2);
          tmpR.parameter=(char **)Alloc(len*sizeof(char *));
          int i;
          for (i=0;i<r1->P;i++)
          {
            tmpR.parameter[i]=mstrdup(r1->parameter[i]);
          }
          int j,l;
          for(j=0;j<r2->P;j++)
          {
            for(l=0;l<i;l++)
            {
              if(strcmp(tmpR.parameter[l],r2->parameter[j])==0)
                break;
            }
            if (l==i)
            {
              tmpR.parameter[i]=mstrdup(r2->parameter[j]);
              i++;
            }
          }
          if (i!=len)
          {
            ReAlloc(tmpR.parameter,len*sizeof(char *),i*sizeof(char *));
          }
        }
      }
    }
  }
  else /* r1->ch!=r2->ch */
  {
    if (r1->ch<-1) /* Z/p(a) */
    {
      if ((r2->ch==0) /* Q */
          || (r2->ch==-r1->ch)) /* Z/p */
      {
        tmpR.ch=r1->ch;
        tmpR.parameter=(char **)Alloc(rPar(r1)*sizeof(char *));
        tmpR.P=r1->P;
        memcpy(tmpR.parameter,r1->parameter,rPar(r1)*sizeof(char *));
        if (r1->minpoly!=NULL)
        {
          nSetChar(r1->ch,TRUE,r1->parameter,r1->P);
          tmpR.minpoly=naCopy(r1->minpoly);
          nSetChar(currRing->ch,TRUE,currRing->parameter,currRing->P);
        }
      }
      else  /* R, Q(a),Z/q,Z/p(a),GF(p,n) */
      {
        WerrorS("Z/p(a)+(R,Q(a),Z/q(a),GF(q,n))");
        return -1;
      }
    }
    else if (r1->ch==-1) /* R */
    {
      WerrorS("R+..");
      return -1;
    }
    else if (r1->ch==0) /* Q */
    {
      if ((r2->ch<-1)||(r2->ch==1)) /* Z/p(a),Q(a) */
      {
        tmpR.ch=r2->ch;
        tmpR.P=r2->P;
        tmpR.parameter=(char **)Alloc(rPar(r2)*sizeof(char *));
        memcpy(tmpR.parameter,r2->parameter,rPar(r2)*sizeof(char *));
        if (r2->minpoly!=NULL)
        {
          nSetChar(r1->ch,TRUE,r1->parameter,r1->P);
          tmpR.minpoly=naCopy(r2->minpoly);
          nSetChar(currRing->ch,TRUE,currRing->parameter,currRing->P);
        }
      }
      else if (r2->ch>1) /* Z/p,GF(p,n) */
      {
        tmpR.ch=r2->ch;
        if (r2->parameter!=NULL)
        {
          tmpR.parameter=(char **)Alloc(sizeof(char *));
          tmpR.P=1;
          tmpR.parameter[0]=mstrdup(r2->parameter[0]);
        }
      }
      else
      {
        WerrorS("Q+R");
        return -1; /* R */
      }
    }
    else if (r1->ch==1) /* Q(a) */
    {
      if (r2->ch==0) /* Q */
      {
        tmpR.ch=r1->ch;
        tmpR.P=rPar(r1);
        tmpR.parameter=(char **)Alloc0(rPar(r1)*sizeof(char *));
        int i;
        for(i=0;i<r1->P;i++)
        {
          tmpR.parameter[i]=mstrdup(r1->parameter[i]);
        }
        if (r1->minpoly!=NULL)
        {
          nSetChar(r1->ch,TRUE,r1->parameter,r1->P);
          tmpR.minpoly=naCopy(r1->minpoly);
          nSetChar(currRing->ch,TRUE,currRing->parameter,currRing->P);
        }
      }
      else  /* R, Z/p,GF(p,n) */
      {
        WerrorS("Q(a)+(R,Z/p,GF(p,n))");
        return -1;
      }
    }
    else /* r1->ch >=2 , Z/p */
    {
      if (r2->ch==0) /* Q */
      {
        tmpR.ch=r1->ch;
      }
      else if (r2->ch==-r1->ch) /* Z/p(a) */
      {
        tmpR.ch=r2->ch;
        tmpR.P=rPar(r2);
        tmpR.parameter=(char **)Alloc(rPar(r2)*sizeof(char *));
        int i;
        for(i=0;i<r2->P;i++)
        {
          tmpR.parameter[i]=mstrdup(r2->parameter[i]);
        }
        if (r2->minpoly!=NULL)
        {
          nSetChar(r2->ch,TRUE,r2->parameter,r2->P);
          tmpR.minpoly=naCopy(r2->minpoly);
          nSetChar(currRing->ch,TRUE,currRing->parameter,currRing->P);
        }
      }
      else
      {
        WerrorS("Z/p+(GF(q,n),Z/q(a),R,Q(a))");
        return -1; /* GF(p,n),Z/q(a),R,Q(a) */
      }
    }
  }
  /* variable names ========================================================*/
  int i,j,k;
  int l=r1->N+r2->N;
  char **names=(char **)Alloc0(l*sizeof(char*));
  k=0;

  // collect all varnames from r1, except those which are parameters
  // of r2, or those which are the empty string
  for (i=0;i<r1->N;i++)
  {
    BOOLEAN b=TRUE;

    if (*(r1->names[i]) == '\0')
      b = FALSE;
    else if ((r2->parameter!=NULL) && (strlen(r1->names[i])==1))
    {
      for(j=0;j<r2->P;j++)
      {
        if (strcmp(r1->names[i],r2->parameter[j])==0)
        {
          b=FALSE;
          break;
        }
      }
    }

    if (b)
    {
      //Print("name : %d: %s\n",k,r1->names[i]);
      names[k]=mstrdup(r1->names[i]);
      k++;
    }
    //else
    //  Print("no name (par1) %s\n",r1->names[i]);
  }
  // Add variables from r2, except those which are parameters of r1
  // those which are empty strings, and those which equal a var of r1
  for(i=0;i<r2->N;i++)
  {
    BOOLEAN b=TRUE;

    if (*(r2->names[i]) == '\0')
      b = FALSE;
    else if ((r1->parameter!=NULL) && (strlen(r2->names[i])==1))
    {
      for(j=0;j<r1->P;j++)
      {
        if (strcmp(r2->names[i],r1->parameter[j])==0)
        {
          b=FALSE;
          break;
        }
      }
    }

    if (b)
    {
      for(j=0;j<r1->N;j++)
      {
        if (strcmp(r1->names[j],r2->names[i])==0)
        {
          b=FALSE;
          break;
        }
      }
      if (b)
      {
        names[k]=mstrdup(r2->names[i]);
        //Print("name : %d : %s\n",k,r2->names[i]);
        k++;
      }
      //else
      //  Print("no name (var): %s\n",r2->names[i]);
    }
    //else
    //  Print("no name (par): %s\n",r2->names[i]);
  }
  // check whether we found any vars at all
  if (k == 0)
  {
    names[k]=mstrdup("");
    k=1;
  }
  tmpR.N=k;
  tmpR.names=names;
  /* ordering *======================================================== */
  tmpR.OrdSgn=1;
  if ((r1->order[0]==ringorder_unspec)
      && (r2->order[0]==ringorder_unspec))
  {
    tmpR.order=(int*)Alloc(3*sizeof(int));
    tmpR.block0=(int*)Alloc(3*sizeof(int));
    tmpR.block1=(int*)Alloc(3*sizeof(int));
    tmpR.wvhdl=(short**)Alloc0(3*sizeof(short*));
    tmpR.order[0]=ringorder_unspec;
    tmpR.order[1]=ringorder_C;
    tmpR.order[2]=0;
    tmpR.block0[0]=1;
    tmpR.block1[0]=tmpR.N;
  }
  else if (l==k) /* r3=r1+r2 */
  {
    int b;
    ring rb;
    if (r1->order[0]==ringorder_unspec)
    {
      /* extend order of r2 to r3 */
      b=rBlocks(r2);
      rb=r2;
      tmpR.OrdSgn=r2->OrdSgn;
    }
    else if (r2->order[0]==ringorder_unspec)
    {
      /* extend order of r1 to r3 */
      b=rBlocks(r1);
      rb=r1;
      tmpR.OrdSgn=r1->OrdSgn;
    }
    else
    {
      b=rBlocks(r1)+rBlocks(r2)-2; /* for only one order C, only one 0 */
      rb=NULL;
    }
    tmpR.order=(int*)Alloc0(b*sizeof(int));
    tmpR.block0=(int*)Alloc0(b*sizeof(int));
    tmpR.block1=(int*)Alloc0(b*sizeof(int));
    tmpR.wvhdl=(short**)Alloc0(b*sizeof(short*));
    /* weights not implemented yet ...*/
    if (rb!=NULL)
    {
      for (i=0;i<b;i++)
      {
        tmpR.order[i]=rb->order[i];
        tmpR.block0[i]=rb->block0[i];
        tmpR.block1[i]=rb->block1[i];
      }
      tmpR.block0[0]=1;
    }
    else /* ring sum for complete rings */
    {
      for (i=0;r1->order[i]!=0;i++)
      {
        tmpR.order[i]=r1->order[i];
        tmpR.block0[i]=r1->block0[i];
        tmpR.block1[i]=r1->block1[i];
      }
      j=i;
      i--;
      if ((r1->order[i]==ringorder_c)
          ||(r1->order[i]==ringorder_C))
      {
        j--;
        tmpR.order[b-2]=r1->order[i];
      }
      for (i=0;r2->order[i]!=0;i++,j++)
      {
        if ((r2->order[i]!=ringorder_c)
            &&(r2->order[i]!=ringorder_C))
        {
          tmpR.order[j]=r2->order[i];
          tmpR.block0[j]=r2->block0[i]+r1->N;
          tmpR.block1[j]=r2->block1[i]+r1->N;
        }
      }
      if((r1->OrdSgn==-1)||(r2->OrdSgn==-1))
        tmpR.OrdSgn=-1;
    }
  }
  else if ((k==r1->N) && (k==r2->N)) /* r1 and r2 are "quite" the same ring */
    /* copy r1, because we have the variables from r1 */
  {
    int b=rBlocks(r1);

    tmpR.order=(int*)Alloc0(b*sizeof(int));
    tmpR.block0=(int*)Alloc0(b*sizeof(int));
    tmpR.block1=(int*)Alloc0(b*sizeof(int));
    tmpR.wvhdl=(short**)Alloc0(b*sizeof(short*));
    /* weights not implemented yet ...*/
    for (i=0;i<b;i++)
    {
      tmpR.order[i]=r1->order[i];
      tmpR.block0[i]=r1->block0[i];
      tmpR.block1[i]=r1->block1[i];
    }
    tmpR.OrdSgn=r1->OrdSgn;
  }
  else
  {
    for(i=0;i<k;i++) FreeL((ADDRESS)tmpR.names[i]);
    Free((ADDRESS)names,tmpR.N*sizeof(char *));
    Werror("difficulties with variables: %d,%d -> %d",r1->N,r2->N,k);
    return -1;
  }
  sum=(ring)Alloc(sizeof(ip_sring));
  memcpy(sum,&tmpR,sizeof(ip_sring));
  rComplete(sum);
  return 1;
}

/*2
 * create a copy of the ring r, which must be equivalent to currRing
 * used for qring definition,..
 * (i.e.: normal rings: same nCopy as currRing;
 *        qring:        same nCopy, same idCopy as currRing)
 */
ring rCopy(ring r)
{
  int i,j;
  int *pi;
  ring res=(ring)Alloc(sizeof(ip_sring));

  memcpy4(res,r,sizeof(ip_sring));
  res->ref=0;
  if (r->parameter!=NULL)
  {
    res->minpoly=nCopy(r->minpoly);
    int l=rPar(r);
    res->parameter=(char **)Alloc(l*sizeof(char *));
    int i;
    for(i=0;i<r->P;i++)
    {
      res->parameter[i]=mstrdup(r->parameter[i]);
    }
  }
  res->names   = (char **)Alloc(r->N * sizeof(char *));
  i=1;
  pi=r->order;
  while ((*pi)!=0) { i++;pi++; }
  res->wvhdl   = (short **)Alloc(i * sizeof(short *));
  res->order   = (int *)   Alloc(i * sizeof(int));
  res->block0  = (int *)   Alloc(i * sizeof(int));
  res->block1  = (int *)   Alloc(i * sizeof(int));
  for (j=0; j<i; j++)
  {
    if (r->wvhdl[j]!=NULL)
    {
      res->wvhdl[j]=(short*)AllocL(mmSizeL((ADDRESS)r->wvhdl[j]));
      memcpy(res->wvhdl[j],r->wvhdl[j],mmSizeL((ADDRESS)r->wvhdl[j]));
    }
    else
      res->wvhdl[j]=NULL;
  }
  memcpy4(res->order,r->order,i * sizeof(int));
  memcpy4(res->block0,r->block0,i * sizeof(int));
  memcpy4(res->block1,r->block1,i * sizeof(int));
  for (i=0; i<res->N; i++)
  {
    res->names[i] = mstrdup(r->names[i]);
  }
  res->idroot = NULL;
  if (r->qideal!=NULL) res->qideal= idCopy(r->qideal);
  res->VarOffset = (int*) Alloc((r->N + 1)*sizeof(int));
  memcpy4(res->VarOffset, r->VarOffset, (r->N + 1)*sizeof(int));

#ifdef RDEBUG
  rNumber++;
  res->no=rNumber;
#endif

  return res;
}

rOrderType_t rGetOrderType(ring r)
{
  // check for simple ordering
  if (rHasSimpleOrder(r))
  {
    if ((r->order[1] == ringorder_c) || (r->order[1] == ringorder_C))
    {
      switch(r->order[0])
      {
          case ringorder_dp:
          case ringorder_wp:
          case ringorder_ds:
          case ringorder_ws:
          case ringorder_ls:
          case ringorder_unspec:
            if (r->order[1] == ringorder_C ||  r->order[0] == ringorder_unspec)
              return rOrderType_ExpComp;
            return rOrderType_Exp;

          default:
            assume(r->order[0] == ringorder_lp ||
                   r->order[0] == ringorder_Dp ||
                   r->order[0] == ringorder_Wp ||
                   r->order[0] == ringorder_Ds ||
                   r->order[0] == ringorder_Ws);

            if (r->order[1] == ringorder_c) return rOrderType_ExpComp;
            return rOrderType_Exp;
      }
    }
    else
    {
      assume((r->order[0]==ringorder_c)||(r->order[0]==ringorder_C));
      return rOrderType_CompExp;
    }
  }
  else
    return rOrderType_General;
}

BOOLEAN rHasSimpleOrder(ring r)
{
  return
    (r->order[0] == ringorder_unspec) ||
    ((r->order[2] == 0) &&
     (r->order[1] != ringorder_M &&
      r->order[0] != ringorder_M));
}

// returns TRUE, if simple lp or ls ordering
BOOLEAN rHasSimpleLexOrder(ring r)
{
  return rHasSimpleOrder(r) &&
    (r->order[0] == ringorder_ls ||
     r->order[0] == ringorder_lp ||
     r->order[1] == ringorder_ls ||
     r->order[1] == ringorder_lp);
}

BOOLEAN rIsPolyVar(int v)
{
  int  i=0;
  while(currRing->order[i]!=0)
  {
    if((currRing->block0[i]<=v)
    && (currRing->block1[i]>=v))
    {
      switch(currRing->order[i])
      {
        case ringorder_a:
          return (currRing->wvhdl[i][v-currRing->block0[i]]>0);
        case ringorder_M:
          return 2; /*don't know*/
        case ringorder_lp:
        case ringorder_dp:
        case ringorder_Dp:
        case ringorder_wp:
        case ringorder_Wp:
          return TRUE;
        case ringorder_ls:
        case ringorder_ds:
        case ringorder_Ds:
        case ringorder_ws:
        case ringorder_Ws:
          return FALSE;
        default:
          break;
      }
    }
    i++;
  }
  return 3; /* could not find var v*/
}

void rUnComplete(ring r)
{
  Free((ADDRESS)r->VarOffset,(r->N + 1)*sizeof(int));
  r->VarOffset=NULL;
}

#ifdef RDEBUG
// This should eventually become a full-fledge ring check, like pTest
BOOLEAN rDBTest(ring r, char* fn, int l)
{
  if (r == NULL)
  {
    Werror("Null ring in %s:%l\n", fn, l);
    return false;
  }

  if (r->N == 0) return true;
  
  if (r->VarOffset == NULL)
  {
    Werror("Null ring VarOffset -- no rComplete (?) in n %s:%d\n", fn, l);
    assume(0);
    return false;
  }
  
  int  
    VarCompIndex = r->VarCompIndex, 
    VarLowIndex  = r->VarLowIndex, 
    VarHighIndex = r->VarHighIndex,
    i;
  BOOLEAN ok = false;
  int* VarOffset = r->VarOffset;
  
  rComplete(r);
  
  if (   VarCompIndex != r->VarCompIndex ||
         VarLowIndex  != r->VarLowIndex ||
         VarHighIndex != r->VarHighIndex)
  {
    Werror("Wrong ring VarIndicies -- no rComplete (?) in n %s:%d\n", fn, l);
    assume(0);
    ok = FALSE;
  }
  
  for (i=0; i<=r->N; i++)
  {
    if (VarOffset[i] != r->VarOffset[i])
    {
      Werror("Wrong VarOffset value at %d in %s:%d\n", i, fn, l);
      assume(0);
      ok = FALSE;
    }
  }
  Free(r->VarOffset, (r->N + 1)*sizeof(int));
  r->VarOffset = VarOffset;
  return ok;
}
#endif
