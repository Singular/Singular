/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/* $Id: ring.cc,v 1.117 2000-09-18 09:19:31 obachman Exp $ */

/*
* ABSTRACT - the interpreter related ring operations
*/

/* includes */
#include <math.h>
#include "mod2.h"
#include "structs.h"
#include "omalloc.h"
#include "tok.h"
#include "ipid.h"
#include "polys.h"
#include "numbers.h"
#include "febase.h"
#include "ipshell.h"
#include "ipconv.h"
#include "intvec.h"
#include "longalg.h"
#include "ffields.h"
#include "subexpr.h"
#include "ideals.h"
#include "lists.h"
#include "ring.h"
#include "prCopy.h"
#include "p_Procs.h"

#define BITS_PER_LONG 8*SIZEOF_LONG

static const char * const ringorder_name[] =
{
  " ?", //ringorder_no = 0,
  "a", //ringorder_a,
  "c", //ringorder_c,
  "C", //ringorder_C,
  "M", //ringorder_M,
  "S", //ringorder_S,
  "s", //ringorder_s,
  "lp", //ringorder_lp,
  "dp", //ringorder_dp,
  "rp", //ringorder_rp,
  "Dp", //ringorder_Dp,
  "wp", //ringorder_wp,
  "Wp", //ringorder_Wp,
  "ls", //ringorder_ls,
  "ds", //ringorder_ds,
  "Ds", //ringorder_Ds,
  "ws", //ringorder_ws,
  "Ws", //ringorder_Ws,
  "L", //ringorder_L,
  " _" //ringorder_unspec
};

static inline const char * rSimpleOrdStr(int ord)
{
  return ringorder_name[ord];
}

// unconditionally deletes fields in r
static void rDelete(ring r);

/*0 implementation*/
//BOOLEAN rField_is_R(ring r=currRing)
//{
//  if (r->ch== -1)
//  {
//    if (r->ch_flags==(short)0) return TRUE;
//  }
//  return FALSE;
//}

// internally changes the gloabl ring and resets the relevant
// global variables:
// complete == FALSE : only delete operations are enabled
// complete == TRUE  : full reset of all variables
void rChangeCurrRing(ring r, BOOLEAN complete)
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
      /*------------ set redTail, except reset by nSetChar or pSetGlobals */
      test |= Sy_bit(OPT_REDTAIL);
    }

    /*------------ global variables related to coefficients ------------*/
    nSetChar(r, complete);

    /*------------ global variables related to polys -------------------*/
    pSetGlobals(r, complete);


    if (complete)
    {
    /*------------ set naMinimalPoly -----------------------------------*/
      if (r->minpoly!=NULL)
      {
        naMinimalPoly=((lnumber)r->minpoly)->z;
      }
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
    omCheckAddrSize((ADDRESS)h,sizeof(idrec));
    if (IDID(h))  // OB: ????
      omCheckAddr((ADDRESS)IDID(h));
    rTest(rg);
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
  rChangeCurrRing(rg,complete);
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
        for (i=0;i<rPar(rg);i++)
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
  if (((sLastPrinted.rtyp>BEGIN_RING) && (sLastPrinted.rtyp<END_RING)) ||
      ((sLastPrinted.rtyp==LIST_CMD)&&(lRingDependend((lists)sLastPrinted.data))))

  {
    sLastPrinted.CleanUp();
    memset(&sLastPrinted,0,sizeof(sleftv));
  }

  ring r = IDRING(tmp);

  r->ch    = 32003;
  r->N     = 3;
  /*r->P     = 0; Alloc0 in idhdl::set, ipid.cc*/
  /*names*/
  r->names = (char **) omAlloc(3 * sizeof(char_ptr));
  r->names[0]  = omStrDup("x");
  r->names[1]  = omStrDup("y");
  r->names[2]  = omStrDup("z");
  /*weights: entries for 3 blocks: NULL*/
  r->wvhdl = (int **)omAlloc0(3 * sizeof(int_ptr));
  /*order: dp,C,0*/
  r->order = (int *) omAlloc(3 * sizeof(int *));
  r->block0 = (int *)omAlloc0(3 * sizeof(int *));
  r->block1 = (int *)omAlloc0(3 * sizeof(int *));
  /* ringorder dp for the first block: var 1..3 */
  r->order[0]  = ringorder_dp;
  r->block0[0] = 1;
  r->block1[0] = 3;
  /* ringorder C for the second block: no vars */
  r->order[1]  = ringorder_C;
  /* the last block: everything is 0 */
  r->order[2]  = 0;
  /*polynomial ring*/
  r->OrdSgn    = 1;

  /* complete ring intializations */
  rComplete(r);
  rSetHdl(tmp,TRUE);
  return currRingHdl;
}

///////////////////////////////////////////////////////////////////////////
//
// rInit: define a new ring from sleftv's
//

/////////////////////////////
// Auxillary functions
//

// check intvec, describing the ordering
static BOOLEAN rCheckIV(intvec *iv)
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

// set R->order, R->block, R->wvhdl, r->OrdSgn from sleftv
static BOOLEAN rSleftvOrdering2Ordering(sleftv *ord, ring R)
{
  int last = 0, o=0, n = 1, i=0, typ = 1, j;
  sleftv *sl = ord;

  // determine nBlocks
  while (sl!=NULL)
  {
    intvec *iv = (intvec *)(sl->data);
    if (((*iv)[1]==ringorder_c)||((*iv)[1]==ringorder_C)) i++;
    else if ((*iv)[1]==ringorder_L)
    {
      R->bitmask=(*iv)[2];
      n--;
    }
    else if ((*iv)[1]!=ringorder_a) o++;
    n++;
    sl=sl->next;
  }
  // check whether at least one real ordering
  if (o==0)
  {
    WerrorS("invalid combination of orderings");
    return TRUE;
  }
  // if no c/C ordering is given, increment n
  if (i==0) n++;
  else if (i != 1)
  {
    // throw error if more than one is given
    WerrorS("more than one ordering c/C specified");
    return TRUE;
  }

  // initialize fields of R
  R->order=(int *)omAlloc0(n*sizeof(int));
  R->block0=(int *)omAlloc0(n*sizeof(int));
  R->block1=(int *)omAlloc0(n*sizeof(int));
  R->wvhdl=(int**)omAlloc0(n*sizeof(int_ptr));

  // init order, so that rBlocks works correctly
  for (j=0; j < n-1; j++)
    R->order[j] = (int) ringorder_unspec;
  // set last _C order, if no c/C order was given
  if (i == 0) R->order[n-2] = ringorder_C;

  /* init orders */
  sl=ord;
  n=-1;
  while (sl!=NULL)
  {
    intvec *iv;
    iv = (intvec *)(sl->data);
    if ((*iv)[1]!=ringorder_L)
    {
      n++;

      /* the format of an ordering:
       *  iv[0]: factor
       *  iv[1]: ordering
       *  iv[2..end]: weights
       */
      R->order[n] = (*iv)[1];
      switch ((*iv)[1])
      {
          case ringorder_ws:
          case ringorder_Ws:
            typ=-1;
          case ringorder_wp:
          case ringorder_Wp:
            R->wvhdl[n]=(int*)omAlloc((iv->length()-1)*sizeof(int));
            for (i=2; i<iv->length(); i++)
              R->wvhdl[n][i-2] = (*iv)[i];
            R->block0[n] = last+1;
            last += iv->length()-2;
            R->block1[n] = last;
            break;
          case ringorder_ls:
          case ringorder_ds:
          case ringorder_Ds:
            typ=-1;
          case ringorder_lp:
          case ringorder_dp:
          case ringorder_Dp:
          case ringorder_rp:
            R->block0[n] = last+1;
            if (iv->length() == 3) last+=(*iv)[2];
            else last += (*iv)[0];
            R->block1[n] = last;
            if (rCheckIV(iv)) return TRUE;
            break;
          case ringorder_S:
          case ringorder_c:
          case ringorder_C:
            if (rCheckIV(iv)) return TRUE;
            break;
          case ringorder_a:
            R->block0[n] = last+1;
            R->block1[n] = last + iv->length() - 2;
            R->wvhdl[n] = (int*)omAlloc((iv->length()-1)*sizeof(int));
            for (i=2; i<iv->length(); i++)
            {
              R->wvhdl[n][i-2]=(*iv)[i];
              if ((*iv)[i]<0) typ=-1;
            }
            break;
          case ringorder_M:
          {
            int Mtyp=rTypeOfMatrixOrder(iv);
            if (Mtyp==0) return TRUE;
            if (Mtyp==-1) typ = -1;

            R->wvhdl[n] =( int *)omAlloc((iv->length()-1)*sizeof(int));
            for (i=2; i<iv->length();i++)
              R->wvhdl[n][i-2]=(*iv)[i];

            R->block0[n] = last+1;
            last += (int)sqrt((double)(iv->length()-2));
            R->block1[n] = last;
            break;
          }

          case ringorder_no:
            R->order[n] = ringorder_unspec;
            return TRUE;

          default:
            Werror("Internal Error: Unknown ordering %d", (*iv)[1]);
            R->order[n] = ringorder_unspec;
            return TRUE;
      }
    }
    sl=sl->next;
  }

  // check for complete coverage
  if ((R->order[n]==ringorder_c) ||  (R->order[n]==ringorder_C)) n--;
  if (R->block1[n] != R->N)
  {
    if (((R->order[n]==ringorder_dp) ||
         (R->order[n]==ringorder_ds) ||
         (R->order[n]==ringorder_Dp) ||
         (R->order[n]==ringorder_Ds) ||
         (R->order[n]==ringorder_rp) ||
         (R->order[n]==ringorder_lp) ||
         (R->order[n]==ringorder_ls))
        &&
        R->block0[n] <= R->N)
    {
      R->block1[n] = R->N;
    }
    else
    {
      Werror("mismatch of number of vars (%d) and ordering (%d vars)",
             R->N,R->block1[n]);
      return TRUE;
    }
  }
  R->OrdSgn = typ;
  return FALSE;
}

// get array of strings from list of sleftv's
static BOOLEAN rSleftvList2StringArray(sleftv* sl, char** p)
{

  while(sl!=NULL)
  {
    if (sl->Name() == sNoName)
    {
      if (sl->Typ()==POLY_CMD)
      {
        sleftv s_sl;
        iiConvert(POLY_CMD,ANY_TYPE,-1,sl,&s_sl);
        if (s_sl.Name() != sNoName)
          *p = omStrDup(s_sl.Name());
        else
          *p = NULL;
        sl->next = s_sl.next;
        s_sl.next = NULL;
        s_sl.CleanUp();
        if (*p == NULL) return TRUE;
      }
      else
        return TRUE;
    }
    else
      *p = omStrDup(sl->Name());
    p++;
    sl=sl->next;
  }
  return FALSE;
}


////////////////////
//
// rInit itself:
//
// INPUT:  s: name, pn: ch & parameter (names), rv: variable (names)
//         ord: ordering
// RETURN: currRingHdl on success
//         NULL        on error
// NOTE:   * makes new ring to current ring, on success
//         * considers input sleftv's as read-only
//idhdl rInit(char *s, sleftv* pn, sleftv* rv, sleftv* ord)
ring rInit(sleftv* pn, sleftv* rv, sleftv* ord)
{
  int ch;
  int float_len=0;
  ring R = NULL;
  idhdl tmp = NULL;
  BOOLEAN ffChar=FALSE;
  int typ = 1;

  /* ch -------------------------------------------------------*/
  // get ch of ground field
  int numberOfAllocatedBlocks;

  if (pn->Typ()==INT_CMD)
  {
    ch=(int)pn->Data();
  }
  else if ((pn->name != NULL)
  && ((strcmp(pn->name,"real")==0) || (strcmp(pn->name,"complex")==0)))
  {
    BOOLEAN complex_flag=(strcmp(pn->name,"complex")==0);
    ch=-1;
    if ((pn->next!=NULL) && (pn->next->Typ()==INT_CMD))
    {
      float_len=(int)pn->next->Data();
      pn=pn->next;
    }
    if ((pn->next==NULL) && complex_flag)
    {
      pn->next=(leftv)omAlloc0Bin(sleftv_bin);
      pn->next->name=omStrDup("i");
    }
  }
  else
  {
    Werror("Wrong ground field specification");
    goto rInitError;
  }
  pn=pn->next;

  int l, last;
  sleftv * sl;
  /*every entry in the new ring is initialized to 0*/

  /* characteristic -----------------------------------------------*/
  /* input: 0 ch=0 : Q     parameter=NULL    ffChar=FALSE   ch_flags
   *         0    1 : Q(a,...)        *names         FALSE
   *         0   -1 : R               NULL           FALSE  0
   *         0   -1 : R               NULL           FALSE  prec. >6
   *         0   -1 : C               *names         FALSE  prec. 0..?
   *         p    p : Fp              NULL           FALSE
   *         p   -p : Fp(a)           *names         FALSE
   *         q    q : GF(q=p^n)       *names         TRUE
   */
  if (ch!=-1)
  {
    int l = 0;

    if (ch!=0 && (ch<2) || (ch > 32003))
    {
      Warn("%d is invalid characteristic of ground field. 32003 is used.", ch);
      ch=32003;
    }
    // load fftable, if necessary
    if (pn!=NULL)
    {
      while ((ch!=fftable[l]) && (fftable[l])) l++;
      if (fftable[l]==0) ch = IsPrime(ch);
      else
      {
        char *m[1]={(char *)sNoName};
        nfSetChar(ch,m);
        if (errorreported) goto rInitError;
        else ffChar=TRUE;
      }
    }
    else
      ch = IsPrime(ch);
  }
  // allocated ring and set ch
  R = (ring) omAlloc0Bin(sip_sring_bin);
  R->ch = ch;
  if (ch == -1)
  {
    R->ch_flags= min(float_len,32767);
  }

  /* parameter -------------------------------------------------------*/
  if (pn!=NULL)
  {
    R->P=pn->listLength();
    //if ((ffChar|| (ch == 1)) && (R->P > 1))
    if ((R->P > 1) && (ffChar || (ch == -1)))
    {
      WerrorS("too many parameters");
      goto rInitError;
    }
    R->parameter=(char**)omAlloc0(R->P*sizeof(char_ptr));
    if (rSleftvList2StringArray(pn, R->parameter))
    {
      WerrorS("parameter expected");
      goto rInitError;
    }
    if (ch>1 && !ffChar) R->ch=-ch;
    else if (ch==0) R->ch=1;
  }
  else if (ffChar)
  {
    WerrorS("need one parameter");
    goto rInitError;
  }
  /* post-processing of field description */
  // we have short reals, but no short complex
  if ((R->ch == - 1)
  && (R->parameter !=NULL)
  && (R->ch_flags < SHORT_REAL_LENGTH))
    R->ch_flags = SHORT_REAL_LENGTH;

  /* names and number of variables-------------------------------------*/
  R->N = rv->listLength();
  R->names   = (char **)omAlloc0(R->N * sizeof(char_ptr));
  if (rSleftvList2StringArray(rv, R->names))
  {
    WerrorS("name of ring variable expected");
    goto rInitError;
  }

  /* check names and parameters for conflicts ------------------------- */
  {
    int i,j;
    for(i=0;i<R->P; i++)
    {
      for(j=0;j<R->N;j++)
      {
        if (strcmp(R->parameter[i],R->names[j])==0)
        {
          Werror("parameter %d conflicts with variable %d",i+1,j+1);
          goto rInitError;
        }
      }
    }
  }
  /* ordering -------------------------------------------------------------*/
  if (rSleftvOrdering2Ordering(ord, R))
    goto rInitError;

  // Complete the initialization
  if (rComplete(R))
    goto rInitError;

  rTest(R);

  // try to enter the ring into the name list
  // need to clean up sleftv here, before this ring can be set to
  // new currRing or currRing can be killed beacuse new ring has
  // same name
  if (pn != NULL) pn->CleanUp();
  if (rv != NULL) rv->CleanUp();
  if (ord != NULL) ord->CleanUp();
  //if ((tmp = enterid(s, myynest, RING_CMD, &IDROOT))==NULL)
  //  goto rInitError;

  //memcpy(IDRING(tmp),R,sizeof(*R));
  // set current ring
  //omFreeBin(R,  ip_sring_bin);
  //return tmp;
  return R;

  // error case:
  rInitError:
  if  (R != NULL) rDelete(R);
  if (pn != NULL) pn->CleanUp();
  if (rv != NULL) rv->CleanUp();
  if (ord != NULL) ord->CleanUp();
  return NULL;
}

/*2
 * set a new ring from the data:
 s: name, chr: ch, varnames: rv, ordering: ord, typ: typ
 */

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

  omCheckAddrSize(r,sizeof(ip_sring));
  omCheckAddrSize(r->order,nblocks*sizeof(int));
  omCheckAddrSize(r->block0,nblocks*sizeof(int));
  omCheckAddrSize(r->block1,nblocks*sizeof(int));
  omCheckAddrSize(r->wvhdl,nblocks*sizeof(int_ptr));
  omCheckAddrSize(r->names,r->N*sizeof(char_ptr));

  nblocks--;


  if (rField_is_GF(r))
  {
    Print("//   # ground field : %d\n",rInternalChar(r));
    Print("//   primitive element : %s\n", r->parameter[0]);
    if (r==currRing)
    {
      StringSetS("//   minpoly        : ");
      nfShowMipo();PrintS(StringAppendS("\n"));
    }
  }
  else
  {
    PrintS("//   characteristic : ");
    if ( rField_is_R(r) )             PrintS("0 (real)\n");  /* R */
    else if ( rField_is_long_R(r) )
      Print("0 (real:%d digits)\n",r->ch_flags);  /* long R */
    else if ( rField_is_long_C(r) )
      Print("0 (complex:%d digits)\n",r->ch_flags);  /* long C */
    else
      Print ("%d\n",rChar(r)); /* Fp(a) */
    if (r->parameter!=NULL)
    {
      Print ("//   %d parameter    : ",rPar(r));
      char **sp=r->parameter;
      int nop=0;
      while (nop<rPar(r))
      {
        PrintS(*sp);
        PrintS(" ");
        sp++; nop++;
      }
      PrintS("\n//   minpoly        : ");
      if ( rField_is_long_C(r) )
      {
        // i^2+1:
        Print("(%s^2+1)\n",r->parameter[0]);
      }
      else if (r->minpoly==NULL)
      {
        PrintS("0\n");
      }
      else if (r==currRing)
      {
        StringSetS(""); nWrite(r->minpoly); PrintS(StringAppendS("\n"));
      }
      else
      {
        PrintS("...\n");
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

    Print("ordering %s", rSimpleOrdStr(r->order[l]));

    if ((r->order[l] >= ringorder_lp)
    ||(r->order[l] == ringorder_M)
    ||(r->order[l] == ringorder_a))
    {
      PrintS("\n//                  : names    ");
      for (i = r->block0[l]-1; i<r->block1[l]; i++)
      {
        nlen = strlen(r->names[i]);
        Print("%s ",r->names[i]);
      }
    }
#ifndef NDEBUG
    else if (r->order[l] == ringorder_s)
    {
      Print("  syzcomp at %d",r->typ[l].data.syz.limit);
    }
#endif

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

static void rDelete(ring r)
{
  int i, j;

  if (r == NULL) return;

  rUnComplete(r);
  // delete order stuff
  if (r->order != NULL)
  {
    i=rBlocks(r);
    assume(r->block0 != NULL && r->block1 != NULL && r->wvhdl != NULL);
    // delete order
    omFreeSize((ADDRESS)r->order,i*sizeof(int));
    omFreeSize((ADDRESS)r->block0,i*sizeof(int));
    omFreeSize((ADDRESS)r->block1,i*sizeof(int));
    // delete weights
    for (j=0; j<i; j++)
    {
      if (r->wvhdl[j]!=NULL)
        omFree(r->wvhdl[j]);
    }
    omFreeSize((ADDRESS)r->wvhdl,i*sizeof(short *));
  }
  else
  {
    assume(r->block0 == NULL && r->block1 == NULL && r->wvhdl == NULL);
  }

  // delete varnames
  if(r->names!=NULL)
  {
    for (i=0; i<r->N; i++)
    {
      if (r->names[i] != NULL) omFree((ADDRESS)r->names[i]);
    }
    omFreeSize((ADDRESS)r->names,r->N*sizeof(char_ptr));
  }

  // delete parameter
  if (r->parameter!=NULL)
  {
    char **s=r->parameter;
    j = 0;
    while (j < rPar(r))
    {
      if (*s != NULL) omFree((ADDRESS)*s);
      s++;
      j++;
    }
    omFreeSize((ADDRESS)r->parameter,rPar(r)*sizeof(char_ptr));
  }
  omFreeBin(r, ip_sring_bin);
}

void rKill(ring r)
{
  if ((r->ref<=0)&&(r->order!=NULL))
  {
#ifdef RDEBUG
    if (traceit &TRACE_SHOW_RINGS) Print("kill ring %x\n",r);
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
      if (((sLastPrinted.rtyp>BEGIN_RING) && (sLastPrinted.rtyp<END_RING)) ||
          ((sLastPrinted.rtyp==LIST_CMD)&&(lRingDependend((lists)sLastPrinted.data))))
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
      rChangeCurrRing(savecurrRing,TRUE);
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

    rDelete(r);
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
    if(currRingHdl==NULL)
    {
      namehdl ns = namespaceroot;
      BOOLEAN found=FALSE;

      while(!ns->isroot)
      {
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
    if(currRingHdl == NULL || IDRING(h) != IDRING(currRingHdl))
    {
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
  int order=ringorder_unspec;
  while (order!= 0)
  {
    if (strcmp(ordername,rSimpleOrdStr(order))==0)
      break;
    order--;
  }
  if (order==0) Werror("wrong ring order `%s`",ordername);
  omFree((ADDRESS)ordername);
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
    StringAppend((char *)rSimpleOrdStr(r->order[l]));
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
    if (l==nblocks) return omStrDup(StringAppendS(""));
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
  s=(char *)omAlloc(l);
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
      s=omStrDup("real");                    /* R */
    else
    {
      s=(char *)omAlloc(6);
      sprintf(s,"%d",i);                   /* Q, Z/p */
    }
    return s;
  }
  int l=0;
  for(i=0; i<rPar(r);i++)
  {
    l+=(strlen(r->parameter[i])+1);
  }
  s=(char *)omAlloc(l+6);
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
  for(i=0; i<rPar(r);i++)
  {
    strcat(s,tt);
    strcat(s,r->parameter[i]);
  }
  return s;
}

char * rParStr(ring r)
{
  if (r->parameter==NULL) return omStrDup("");

  int i;
  int l=2;

  for (i=0; i<rPar(r); i++)
  {
    l+=strlen(r->parameter[i])+1;
  }
  char *s=(char *)omAlloc(l);
  s[0]='\0';
  for (i=0; i<rPar(r)-1; i++)
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
  char *res=(char *)omAlloc(strlen(ch)+strlen(var)+strlen(ord)+9);
  sprintf(res,"(%s),(%s),(%s)",ch,var,ord);
  omFree((ADDRESS)ch);
  omFree((ADDRESS)var);
  omFree((ADDRESS)ord);
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
  if (rInternalChar(r1)==rInternalChar(r2))
  {
    tmpR.ch=rInternalChar(r1);
    if (rField_is_Q(r1)||rField_is_Zp(r1)||rField_is_GF(r1)) /*Q, Z/p, GF(p,n)*/
    {
      if (r1->parameter!=NULL)
      {
        if (strcmp(r1->parameter[0],r2->parameter[0])==0) /* 1 char */
        {
          tmpR.parameter=(char **)omAllocBin(char_ptr_bin);
          tmpR.parameter[0]=omStrDup(r1->parameter[0]);
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
          nSetChar(r1,TRUE);
          if ((strcmp(r1->parameter[0],r2->parameter[0])==0) /* 1 char */
              && naEqual(r1->minpoly,r2->minpoly))
          {
            tmpR.parameter=(char **)omAllocBin(char_ptr_bin);
            tmpR.parameter[0]=omStrDup(r1->parameter[0]);
            tmpR.minpoly=naCopy(r1->minpoly);
            tmpR.P=1;
            nSetChar(currRing,TRUE);
          }
          else
          {
            nSetChar(currRing,TRUE);
            WerrorS("different minpolys");
            return -1;
          }
        }
        else
        {
          if ((strcmp(r1->parameter[0],r2->parameter[0])==0) /* 1 char */
              && (rPar(r2)==1))
          {
            tmpR.parameter=(char **)omAlloc0Bin(char_ptr_bin);
            tmpR.parameter[0]=omStrDup(r1->parameter[0]);
            tmpR.P=1;
            nSetChar(r1,TRUE);
            tmpR.minpoly=naCopy(r1->minpoly);
            nSetChar(currRing,TRUE);
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
              && (rPar(r1)==1))
          {
            tmpR.parameter=(char **)omAllocBin(char_ptr_bin);
            tmpR.parameter[0]=omStrDup(r1->parameter[0]);
            tmpR.P=1;
            nSetChar(r2,TRUE);
            tmpR.minpoly=naCopy(r2->minpoly);
            nSetChar(currRing,TRUE);
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
          tmpR.parameter=(char **)omAlloc(len*sizeof(char_ptr));
          int i;
          for (i=0;i<rPar(r1);i++)
          {
            tmpR.parameter[i]=omStrDup(r1->parameter[i]);
          }
          int j,l;
          for(j=0;j<rPar(r2);j++)
          {
            for(l=0;l<i;l++)
            {
              if(strcmp(tmpR.parameter[l],r2->parameter[j])==0)
                break;
            }
            if (l==i)
            {
              tmpR.parameter[i]=omStrDup(r2->parameter[j]);
              i++;
            }
          }
          if (i!=len)
          {
            omReallocSize(tmpR.parameter,len*sizeof(char_ptr),i*sizeof(char_ptr));
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
        tmpR.ch=rInternalChar(r1);
        tmpR.parameter=(char **)omAlloc(rPar(r1)*sizeof(char_ptr));
        tmpR.P=rPar(r1);
        memcpy(tmpR.parameter,r1->parameter,rPar(r1)*sizeof(char_ptr));
        if (r1->minpoly!=NULL)
        {
          nSetChar(r1,TRUE);
          tmpR.minpoly=naCopy(r1->minpoly);
          nSetChar(currRing,TRUE);
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
        tmpR.ch=rInternalChar(r2);
        tmpR.P=rPar(r2);
        tmpR.parameter=(char **)omAlloc(rPar(r2)*sizeof(char_ptr));
        memcpy(tmpR.parameter,r2->parameter,rPar(r2)*sizeof(char_ptr));
        if (r2->minpoly!=NULL)
        {
          nSetChar(r1,TRUE);
          tmpR.minpoly=naCopy(r2->minpoly);
          nSetChar(currRing,TRUE);
        }
      }
      else if (r2->ch>1) /* Z/p,GF(p,n) */
      {
        tmpR.ch=r2->ch;
        if (r2->parameter!=NULL)
        {
          tmpR.parameter=(char **)omAllocBin(char_ptr_bin);
          tmpR.P=1;
          tmpR.parameter[0]=omStrDup(r2->parameter[0]);
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
        tmpR.ch=rInternalChar(r1);
        tmpR.P=rPar(r1);
        tmpR.parameter=(char **)omAlloc0(rPar(r1)*sizeof(char_ptr));
        int i;
        for(i=0;i<rPar(r1);i++)
        {
          tmpR.parameter[i]=omStrDup(r1->parameter[i]);
        }
        if (r1->minpoly!=NULL)
        {
          nSetChar(r1,TRUE);
          tmpR.minpoly=naCopy(r1->minpoly);
          nSetChar(currRing,TRUE);
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
        tmpR.ch=rInternalChar(r2);
        tmpR.P=rPar(r2);
        tmpR.parameter=(char **)omAlloc(rPar(r2)*sizeof(char_ptr));
        int i;
        for(i=0;i<rPar(r2);i++)
        {
          tmpR.parameter[i]=omStrDup(r2->parameter[i]);
        }
        if (r2->minpoly!=NULL)
        {
          nSetChar(r2,TRUE);
          tmpR.minpoly=naCopy(r2->minpoly);
          nSetChar(currRing,TRUE);
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
  char **names=(char **)omAlloc0(l*sizeof(char_ptr));
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
      for(j=0;j<rPar(r2);j++)
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
      names[k]=omStrDup(r1->names[i]);
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
      for(j=0;j<rPar(r1);j++)
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
        names[k]=omStrDup(r2->names[i]);
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
    names[k]=omStrDup("");
    k=1;
  }
  tmpR.N=k;
  tmpR.names=names;
  /* ordering *======================================================== */
  tmpR.OrdSgn=1;
  if ((r1->order[0]==ringorder_unspec)
      && (r2->order[0]==ringorder_unspec))
  {
    tmpR.order=(int*)omAlloc(3*sizeof(int));
    tmpR.block0=(int*)omAlloc(3*sizeof(int));
    tmpR.block1=(int*)omAlloc(3*sizeof(int));
    tmpR.wvhdl=(int**)omAlloc0(3*sizeof(int_ptr));
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
    tmpR.order=(int*)omAlloc0(b*sizeof(int));
    tmpR.block0=(int*)omAlloc0(b*sizeof(int));
    tmpR.block1=(int*)omAlloc0(b*sizeof(int));
    tmpR.wvhdl=(int**)omAlloc0(b*sizeof(int_ptr));
    /* weights not implemented yet ...*/
    if (rb!=NULL)
    {
      for (i=0;i<b;i++)
      {
        tmpR.order[i]=rb->order[i];
        tmpR.block0[i]=rb->block0[i];
        tmpR.block1[i]=rb->block1[i];
        if (rb->wvhdl[i]!=NULL)
          WarnS("rSum: weights not implemented");
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
        if (r1->wvhdl[i]!=NULL)
          tmpR.wvhdl[i] = (int*) omMemDup(r1->wvhdl[i]);
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
          if (r2->wvhdl[i]!=NULL)
          {
            tmpR.wvhdl[j] = (int*) omMemDup(r2->wvhdl[i]);
          }
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

    tmpR.order=(int*)omAlloc0(b*sizeof(int));
    tmpR.block0=(int*)omAlloc0(b*sizeof(int));
    tmpR.block1=(int*)omAlloc0(b*sizeof(int));
    tmpR.wvhdl=(int**)omAlloc0(b*sizeof(int_ptr));
    /* weights not implemented yet ...*/
    for (i=0;i<b;i++)
    {
      tmpR.order[i]=r1->order[i];
      tmpR.block0[i]=r1->block0[i];
      tmpR.block1[i]=r1->block1[i];
      if (r1->wvhdl[i]!=NULL)
      {
        tmpR.wvhdl[i] = (int*) omMemDup(r1->wvhdl[i]);
      }
    }
    tmpR.OrdSgn=r1->OrdSgn;
  }
  else
  {
    for(i=0;i<k;i++) omFree((ADDRESS)tmpR.names[i]);
    omFreeSize((ADDRESS)names,tmpR.N*sizeof(char_ptr));
    Werror("difficulties with variables: %d,%d -> %d",r1->N,r2->N,k);
    return -1;
  }
  sum=(ring)omAllocBin(ip_sring_bin);
  memcpy(sum,&tmpR,sizeof(ip_sring));
  rComplete(sum);
  return 1;
}

/*2
 * create a copy of the ring r, which must be equivalent to currRing
 * used for qring definition,..
 * (i.e.: normal rings: same nCopy as currRing;
 *        qring:        same nCopy, same idCopy as currRing)
 * DOES NOT CALL rComplete
 */
static ring rCopy0(ring r, BOOLEAN copy_qideal = TRUE,
                   BOOLEAN copy_ordering = TRUE)
{
  if (r == NULL) return NULL;
  int i,j;
  ring res=(ring)omAllocBin(ip_sring_bin);

  memcpy4(res,r,sizeof(ip_sring));
  res->VarOffset = NULL;
  res->ref=0;
  if (r->parameter!=NULL)
  {
    res->minpoly=nCopy(r->minpoly);
    int l=rPar(r);
    res->parameter=(char **)omAlloc(l*sizeof(char_ptr));
    int i;
    for(i=0;i<rPar(r);i++)
    {
      res->parameter[i]=omStrDup(r->parameter[i]);
    }
  }
  if (copy_ordering == TRUE)
  {
    i=rBlocks(r);
    res->wvhdl   = (int **)omAlloc(i * sizeof(int_ptr));
    res->order   = (int *) omAlloc(i * sizeof(int));
    res->block0  = (int *) omAlloc(i * sizeof(int));
    res->block1  = (int *) omAlloc(i * sizeof(int));
    for (j=0; j<i; j++)
    {
      if (r->wvhdl[j]!=NULL)
      {
        res->wvhdl[j] = (int*) omMemDup(r->wvhdl[j]);
      }
      else
        res->wvhdl[j]=NULL;
    }
    memcpy4(res->order,r->order,i * sizeof(int));
    memcpy4(res->block0,r->block0,i * sizeof(int));
    memcpy4(res->block1,r->block1,i * sizeof(int));
  }
  else
  {
    res->wvhdl = NULL;
    res->order = NULL;
    res->block0 = NULL;
    res->block1 = NULL;
  }

  res->names   = (char **)omAlloc(r->N * sizeof(char_ptr));
  for (i=0; i<res->N; i++)
  {
    res->names[i] = omStrDup(r->names[i]);
  }
  res->idroot = NULL;
  if (r->qideal!=NULL)
  {
    if (copy_qideal) res->qideal= idrCopyR_NoSort(r->qideal, r);
    else res->qideal = NULL;
  }
  return res;
}

/*2
 * create a copy of the ring r, which must be equivalent to currRing
 * used for qring definition,..
 * (i.e.: normal rings: same nCopy as currRing;
 *        qring:        same nCopy, same idCopy as currRing)
 */
ring rCopy(ring r)
{
  if (r == NULL) return NULL;
  ring res=rCopy0(r);
  rComplete(res, 1);
  return res;
}

// returns TRUE, if r1 equals r2 FALSE, otherwise Equality is
// determined componentwise, if qr == 1, then qrideal equality is
// tested, as well
BOOLEAN rEqual(ring r1, ring r2, BOOLEAN qr)
{
  int i, j;

  if (r1 == r2) return 1;

  if (r1 == NULL || r2 == NULL) return 0;

  if ((rInternalChar(r1) != rInternalChar(r2))
  || (r1->ch_flags != r2->ch_flags)
  || (r1->N != r2->N)
  || (r1->OrdSgn != r2->OrdSgn)
  || (rPar(r1) != rPar(r2)))
    return 0;

  for (i=0; i<r1->N; i++)
    if (strcmp(r1->names[i], r2->names[i])) return 0;

  i=0;
  while (r1->order[i] != 0)
  {
    if (r2->order[i] == 0) return 0;
    if ((r1->order[i] != r2->order[i]) ||
        (r1->block0[i] != r2->block0[i]) || (r2->block0[i] != r1->block0[i]))
      return 0;
    if (r1->wvhdl[i] != NULL)
    {
      if (r2->wvhdl[i] == NULL)
        return 0;
      for (j=0; j<r1->block1[i]-r1->block0[i]+1; j++)
        if (r2->wvhdl[i][j] != r1->wvhdl[i][j])
          return 0;
    }
    else if (r2->wvhdl[i] != NULL) return 0;
    i++;
  }

  for (i=0; i<rPar(r1);i++)
  {
      if (strcmp(r1->parameter[i], r2->parameter[i])!=0)
        return 0;
  }

  if (r1->minpoly != NULL)
  {
    if (r2->minpoly == NULL) return 0;
    if (currRing == r1 || currRing == r2)
    {
      if (! nEqual(r1->minpoly, r2->minpoly)) return 0;
    }
  }
  else if (r2->minpoly != NULL) return 0;

  if (qr)
  {
    if (r1->qideal != NULL)
    {
      ideal id1 = r1->qideal, id2 = r2->qideal;
      int i, n;
      poly *m1, *m2;

      if (id2 == NULL) return 0;
      if ((n = IDELEMS(id1)) != IDELEMS(id2)) return 0;

      if (currRing == r1 || currRing == r2)
      {
        m1 = id1->m;
        m2 = id2->m;
        for (i=0; i<n; i++)
          if (! pEqualPolys(m1[i],m2[i])) return 0;
      }
    }
    else if (r2->qideal != NULL) return 0;
  }

  return 1;
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
                   r->order[0] == ringorder_rp ||
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
        case ringorder_rp:
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

#ifdef RDEBUG
// This should eventually become a full-fledge ring check, like pTest
BOOLEAN rDBTest(ring r, char* fn, int l)
{
  int i,j;

  if (r == NULL)
  {
    dReportError("Null ring in %s:%d", fn, l);
    return FALSE;
  }

  if (r->N == 0) return TRUE;

//  omCheckAddrSize(r,sizeof(ip_sring));
#if OM_CHECK > 0
  i=rBlocks(r);
  omCheckAddrSize(r->order,i*sizeof(int));
  omCheckAddrSize(r->block0,i*sizeof(int));
  omCheckAddrSize(r->block1,i*sizeof(int));
  omCheckAddrSize(r->wvhdl,i*sizeof(int *));
  for (j=0;j<i; j++)
  {
    if (r->wvhdl[j] != NULL) omCheckAddr(r->wvhdl[j]);
  }
#endif
  if (r->VarOffset == NULL)
  {
    dReportError("Null ring VarOffset -- no rComplete (?) in n %s:%d", fn, l);
    return FALSE;
  }
  omCheckAddrSize(r->VarOffset,(r->N+1)*sizeof(int));

  if ((r->OrdSize==0)!=(r->typ==NULL))
  {
    dReportError("mismatch OrdSize and typ-pointer in %s:%d");
    return FALSE;
  }
  omcheckAddrSize(r->typ,r->OrdSize*sizeof(*(r->typ)));
  omCheckAddrSize(r->VarOffset,(r->N+1)*sizeof(*(r->VarOffset)));
  // test assumptions:
  for(i=0;i<=r->N;i++)
  {
    if(r->typ!=NULL)
    {
      for(j=0;j<r->OrdSize;j++)
      {
        if (r->typ[j].ord_typ==ro_cp)
        {
          if(((short)r->VarOffset[i]) == r->typ[j].data.cp.place)
            dReportError("ordrec %d conflicts with var %d",j,i);
        }
        else
          if ((r->typ[j].ord_typ!=ro_syzcomp)
          && (r->VarOffset[i] == r->typ[j].data.dp.place))
            dReportError("ordrec %d conflicts with var %d",j,i);
      }
    }
    int tmp;
      tmp=r->VarOffset[i] & 0xffffff;
      #if SIZEOF_LONG == 8
        if ((r->VarOffset[i] >> 24) >63)
      #else
        if ((r->VarOffset[i] >> 24) >31)
      #endif
          dReportError("bit_start out of range:%d",r->VarOffset[i] >> 24);
      if (i > 0 && ((tmp<0) ||(tmp>r->ExpESize-1)))
    {
      dReportError("varoffset out of range for var %d: %d",i,tmp);
    }
  }
  if(r->typ!=NULL)
  {
    for(j=0;j<r->OrdSize;j++)
    {
      if ((r->typ[j].ord_typ==ro_dp)
      || (r->typ[j].ord_typ==ro_wp))
      {
        if (r->typ[j].data.dp.start > r->typ[j].data.dp.end)
          dReportError("in ordrec %d: start(%d) > end(%d)",j,
            r->typ[j].data.dp.start, r->typ[j].data.dp.end);
        if ((r->typ[j].data.dp.start < 1)
        || (r->typ[j].data.dp.end > r->N))
          dReportError("in ordrec %d: start(%d)<1 or end(%d)>vars(%d)",j,
            r->typ[j].data.dp.start, r->typ[j].data.dp.end,r->N);
      }
    }
  }
  return TRUE;
}
#endif

static void rO_Align(int &place, int &bitplace)
{
  // increment place to the next aligned one
  // (count as Exponent_t,align as longs)
  if (bitplace!=BITS_PER_LONG)
  {
    place++;
    bitplace=BITS_PER_LONG;
  }
}

static void rO_TDegree(int &place, int &bitplace, int start, int end,
    long *o, sro_ord &ord_struct)
{
  // degree (aligned) of variables v_start..v_end, ordsgn 1
  rO_Align(place,bitplace);
  ord_struct.ord_typ=ro_dp;
  ord_struct.data.dp.start=start;
  ord_struct.data.dp.end=end;
  ord_struct.data.dp.place=place;
  o[place]=1;
  place++;
  rO_Align(place,bitplace);
}

static void rO_TDegree_neg(int &place, int &bitplace, int start, int end,
    long *o, sro_ord &ord_struct)
{
  // degree (aligned) of variables v_start..v_end, ordsgn -1
  rO_Align(place,bitplace);
  ord_struct.ord_typ=ro_dp;
  ord_struct.data.dp.start=start;
  ord_struct.data.dp.end=end;
  ord_struct.data.dp.place=place;
  o[place]=-1;
  place++;
  rO_Align(place,bitplace);
}

static void rO_WDegree(int &place, int &bitplace, int start, int end,
    long *o, sro_ord &ord_struct, int *weights)
{
  // weighted degree (aligned) of variables v_start..v_end, ordsgn 1
  rO_Align(place,bitplace);
  ord_struct.ord_typ=ro_wp;
  ord_struct.data.wp.start=start;
  ord_struct.data.wp.end=end;
  ord_struct.data.wp.place=place;
  ord_struct.data.wp.weights=weights;
  o[place]=1;
  place++;
  rO_Align(place,bitplace);
}

static void rO_WDegree_neg(int &place, int &bitplace, int start, int end,
    long *o, sro_ord &ord_struct, int *weights)
{
  // weighted degree (aligned) of variables v_start..v_end, ordsgn -1
  rO_Align(place,bitplace);
  ord_struct.ord_typ=ro_wp;
  ord_struct.data.wp.start=start;
  ord_struct.data.wp.end=end;
  ord_struct.data.wp.place=place;
  ord_struct.data.wp.weights=weights;
  o[place]=-1;
  place++;
  rO_Align(place,bitplace);
}

static void rO_LexVars(int &place, int &bitplace, int start, int end,
  int &prev_ord, long *o,int *v, int bits)
{
  // a block of variables v_start..v_end with lex order, ordsgn 1
  int k;
  int incr=1;
  if(prev_ord==-1) rO_Align(place,bitplace);

  if (start>end)
  {
    incr=-1;
  }
  for(k=start;;k+=incr)
  {
    bitplace-=bits;
    if (bitplace < 0) { bitplace=BITS_PER_LONG-bits; place++; }
    o[place]=1;
    v[k]= place | (bitplace << 24);
    if (k==end) break;
  }
  prev_ord=1;
}

static void rO_LexVars_neg(int &place, int &bitplace, int start, int end,
  int &prev_ord, long *o,int *v, int bits)
{
  // a block of variables v_start..v_end with lex order, ordsgn -1
  int k;
  int incr=1;
  if(prev_ord==1) rO_Align(place,bitplace);

  if (start>end)
  {
    incr=-1;
  }
  for(k=start;;k+=incr)
  {
    bitplace-=bits;
    if (bitplace < 0) { bitplace=BITS_PER_LONG-bits; place++; }
    o[place]=-1;
    v[k]=place | (bitplace << 24);
    if (k==end) break;
  }
  prev_ord=-1;
}

static void rO_Syzcomp(int &place, int &bitplace, int &prev_ord,
    long *o, sro_ord &ord_struct)
{
  // ordering is derived from component number
  rO_Align(place,bitplace);
  ord_struct.ord_typ=ro_syzcomp;
  ord_struct.data.syzcomp.place=place;
  ord_struct.data.syzcomp.Components=NULL;
  ord_struct.data.syzcomp.ShiftedComponents=NULL;
  o[place]=1;
  prev_ord=1;
  place++;
  rO_Align(place,bitplace);
}

static void rO_Syz(int &place, int &bitplace, int &prev_ord,
    long *o, sro_ord &ord_struct)
{
  // ordering is derived from component number
  // let's reserve one Exponent_t for it
  if ((prev_ord== 1) || (bitplace!=BITS_PER_LONG))
    rO_Align(place,bitplace);
  ord_struct.ord_typ=ro_syz;
  ord_struct.data.syz.place=place;
  ord_struct.data.syz.limit=0;
  ord_struct.data.syz.syz_index = NULL;
  ord_struct.data.syz.curr_index = 1;
  o[place]= -1;
  prev_ord=-1;
  place++;
}

unsigned long rGetExpSize(unsigned long bitmask, int & bits)
{
  if (bitmask == 0)
  {
    bits=16; bitmask=0xffff;
  }
  else if (bitmask <= 1)
  {
    bits=1; bitmask = 1;
  }
  else if (bitmask <= 3)
  {
    bits=2; bitmask = 3;
  }
  else if (bitmask <= 7)
  {
    bits=3; bitmask=7;
  }
  else if (bitmask <= 0xf)
  {
    bits=4; bitmask=0xf;
  }
  else if (bitmask <= 0x1f)
  {
    bits=5; bitmask=0x1f;
  }
  else if (bitmask <= 0x3f)
  {
    bits=6; bitmask=0x3f;
  }
#if SIZEOF_LONG == 8
  else if (bitmask <= 0x7f)
  {
    bits=7; bitmask=0x7f; /* 64 bit longs only */
  }
#endif
  else if (bitmask <= 0xff)
  {
    bits=8; bitmask=0xff;
  }
#if SIZEOF_LONG == 8
  else if (bitmask <= 0x1ff)
  {
    bits=9; bitmask=0x1ff; /* 64 bit longs only */
  }
#endif
  else if (bitmask <= 0x3ff)
  {
    bits=10; bitmask=0x3ff;
  }
#if SIZEOF_LONG == 8
  else if (bitmask <= 0xfff)
  {
    bits=12; bitmask=0xfff; /* 64 bit longs only */
  }
#endif
  else if (bitmask <= 0xffff)
  {
    bits=16; bitmask=0xffff;
  }
#if SIZEOF_LONG == 8
  else if (bitmask <= 0xfffff)
  {
    bits=20; bitmask=0xfffff; /* 64 bit longs only */
  }
  else if (bitmask <= 0xffffffff)
  {
    bits=32; bitmask=0xffffffff;
  }
  else
  {
    bits=64; bitmask=0xffffffffffffffff;
  }
#else
  else
  {
    bits=32; bitmask=0xffffffff;
  }
#endif
  return bitmask;
}

/*2
 * create a copy of the ring r, which must be equivalent to currRing
 * used for std computations
 * may share data structures with currRing
 * DOES CALL rComplete
 */
ring rModifyRing(ring r, BOOLEAN omit_degree,
                         BOOLEAN omit_comp,
                         unsigned long exp_limit)
{
  assume (r != NULL );
  assume (exp_limit > 1);

  BOOLEAN need_other_ring;
  int bits;

  exp_limit=rGetExpSize(exp_limit, bits);
  need_other_ring = (exp_limit==r->bitmask);

  int nblocks=rBlocks(r);
  int *order=(int*)omAlloc0((nblocks+1)*sizeof(int));
  int *block0=(int*)omAlloc0((nblocks+1)*sizeof(int));
  int *block1=(int*)omAlloc0((nblocks+1)*sizeof(int));
  int **wvhdl=(int**)omAlloc0((nblocks+1)*sizeof(int_ptr));

  int i=0;
  int j=0; /*  i index in r, j index in res */
  loop
  {
    switch(r->order[i])
    {
      case ringorder_C:
      case ringorder_c:
        if (!omit_comp)
        {
          order[j]=r->order[i];
        }
        else
        {
          j--;
          need_other_ring=TRUE;
          omit_comp=FALSE;
        }
        break;
      case ringorder_wp:
      case ringorder_dp:
      case ringorder_ws:
      case ringorder_ds:
        if(!omit_degree)
        {
          order[j]=r->order[i];
        }
        else
        {
          order[j]=ringorder_rp;
          need_other_ring=TRUE;
          omit_degree=FALSE;
        }
        break;
      case ringorder_Wp:
      case ringorder_Dp:
      case ringorder_Ws:
      case ringorder_Ds:
        if(!omit_degree)
        {
          order[j]=r->order[i];
        }
        else
        {
          order[j]=ringorder_lp;
          need_other_ring=TRUE;
          omit_degree=FALSE;
        }
        break;
      default:
        order[j]=r->order[i];
        break;
    }
    block0[i]=r->block0[j];
    block1[i]=r->block1[j];
    wvhdl[i]=r->wvhdl[j];
    i++;j++;
    // order[j]=ringorder_no; //  done by omAlloc0
    if (i==nblocks) break;
  }
  if(!need_other_ring)
  {
    omFreeSize(order,(nblocks+1)*sizeof(int));
    omFreeSize(block0,(nblocks+1)*sizeof(int));
    omFreeSize(block1,(nblocks+1)*sizeof(int));
    omFreeSize(wvhdl,(nblocks+1)*sizeof(int_ptr));
    return r;
  }
  ring res=(ring)omAlloc0Bin(ip_sring_bin);
  memcpy(res,r,sizeof(*r));
  res->names   = r->names;
  // res->idroot = NULL;
  res->qideal =  r->qideal; // ?? or NULL
  res->wvhdl=wvhdl;
  res->order=order;
  res->block0=block0;
  res->block1=block1;
  res->bitmask=exp_limit;
  rComplete(res);
  return res;
}

void rKillModifiedRing(ring r)
{
  rUnComplete(r);
  omFree(r->order);
  omFree(r->block0);
  omFree(r->block1);
  omFree(r->wvhdl);
  omFreeBin(r,ip_sring_bin);
}

BOOLEAN rComplete(ring r, int force)
{
  if (r->VarOffset!=NULL && force == 0) return FALSE;

  int n=rBlocks(r)-1;
  int i;
  int bits;
  r->bitmask=rGetExpSize(r->bitmask,bits);
  // will be used for ordsgn:
  long *tmp_ordsgn=(long *)omAlloc0(2*(n+r->N)*sizeof(long));
  // will be used for VarOffset:
  int *v=(int *)omAlloc((r->N+1)*sizeof(int));
  for(i=r->N; i>=0 ; i--)
  {
    v[i]=-1;
  }
  sro_ord *tmp_typ=(sro_ord *)omAlloc0(2*(n+r->N)*sizeof(sro_ord));
  int typ_i=0;
  int prev_ordsgn=0;
  r->pVarLowIndex=0;

  // fill in v, tmp_typ, tmp_ordsgn, determine pVarLowIndex, typ_i (== ordSize)
  int j=0;
  int j_bits=BITS_PER_LONG;
  BOOLEAN need_to_add_comp=FALSE;
  for(i=0;i<n;i++)
  {
    switch (r->order[i])
    {
      case ringorder_a:
        rO_WDegree(j,j_bits,r->block0[i],r->block1[i],tmp_ordsgn,tmp_typ[typ_i],
                   r->wvhdl[i]);
        r->pVarLowIndex=j;
        typ_i++;
        break;

      case ringorder_c:
        rO_Align(j, j_bits);
        rO_LexVars_neg(j, j_bits, 0,0, prev_ordsgn,tmp_ordsgn,v,BITS_PER_LONG);
        break;

      case ringorder_C:
        rO_Align(j, j_bits);
        rO_LexVars(j, j_bits, 0,0, prev_ordsgn,tmp_ordsgn,v,BITS_PER_LONG);
        break;

      case ringorder_M:
        {
          int k,l;
          k=r->block1[i]-r->block0[i]+1; // number of vars
          for(l=0;l<k;l++)
          {
            rO_WDegree(j,j_bits,r->block0[i],r->block1[i],tmp_ordsgn,
                       tmp_typ[typ_i],
                       r->wvhdl[i]+(r->block1[i]-r->block0[i]+1)*l);
            typ_i++;
          }
          r->pVarLowIndex=j;
          break;
        }

      case ringorder_lp:
        rO_LexVars(j, j_bits, r->block0[i],r->block1[i], prev_ordsgn,
                   tmp_ordsgn,v,bits);
        break;

      case ringorder_ls:
        rO_LexVars_neg(j, j_bits, r->block0[i],r->block1[i], prev_ordsgn,
                       tmp_ordsgn,v, bits);
        break;

      case ringorder_rp:
        rO_LexVars_neg(j, j_bits, r->block1[i],r->block0[i], prev_ordsgn,
                       tmp_ordsgn,v, bits);
        break;

      case ringorder_dp:
        if (r->block0[i]==r->block1[i])
        {
          rO_LexVars(j, j_bits, r->block0[i],r->block0[i], prev_ordsgn,
                     tmp_ordsgn,v, bits);
        }
        else
        {
          rO_TDegree(j,j_bits,r->block0[i],r->block1[i],tmp_ordsgn,
                     tmp_typ[typ_i]);
          r->pVarLowIndex=j;
          typ_i++;
          rO_LexVars_neg(j, j_bits, r->block1[i],r->block0[i]+1,
                         prev_ordsgn,tmp_ordsgn,v,bits);
          if ((j_bits >= bits)&&(j_bits != BITS_PER_LONG-bits))
          // there is enough space for the last var
            rO_LexVars_neg(j, j_bits, r->block0[i],r->block0[i],
                         prev_ordsgn,tmp_ordsgn,v,bits);
        }
        break;

      case ringorder_Dp:
        if (r->block0[i]==r->block1[i])
        {
          rO_LexVars(j, j_bits, r->block0[i],r->block0[i], prev_ordsgn,
                     tmp_ordsgn,v, bits);
        }
        else
        {
          rO_TDegree(j,j_bits,r->block0[i],r->block1[i],tmp_ordsgn,
                     tmp_typ[typ_i]);
          r->pVarLowIndex=j;
          typ_i++;
          rO_LexVars(j, j_bits, r->block0[i],r->block1[i]-1, prev_ordsgn,
                     tmp_ordsgn,v, bits);
          if ((j_bits >= bits)&&(j_bits != BITS_PER_LONG-bits))
          // there is enough space for the last var
            rO_LexVars(j, j_bits, r->block0[i],r->block0[i],
                         prev_ordsgn,tmp_ordsgn,v,bits);
        }
        break;

      case ringorder_ds:
        if (r->block0[i]==r->block1[i])
        {
          rO_LexVars_neg(j, j_bits,r->block0[i],r->block1[i],prev_ordsgn,
                         tmp_ordsgn,v,bits);
        }
        else
        {
          rO_TDegree_neg(j,j_bits,r->block0[i],r->block1[i],tmp_ordsgn,
                         tmp_typ[typ_i]);
          r->pVarLowIndex=j;
          typ_i++;
          rO_LexVars_neg(j, j_bits, r->block1[i],r->block0[i]+1,
                         prev_ordsgn,tmp_ordsgn,v,bits);
          if ((j_bits >= bits)&&(j_bits != BITS_PER_LONG-bits))
          // there is enough space for the last var
            rO_LexVars_neg(j, j_bits, r->block0[i],r->block0[i],
                         prev_ordsgn,tmp_ordsgn,v,bits);
        }
        break;

      case ringorder_Ds:
        if (r->block0[i]==r->block1[i])
        {
          rO_LexVars_neg(j, j_bits, r->block0[i],r->block0[i],prev_ordsgn,
                         tmp_ordsgn,v, bits);
        }
        else
        {
          rO_TDegree_neg(j,j_bits,r->block0[i],r->block1[i],tmp_ordsgn,
                         tmp_typ[typ_i]);
          r->pVarLowIndex=j;
          typ_i++;
          rO_LexVars(j, j_bits, r->block0[i],r->block1[i]-1, prev_ordsgn,
                     tmp_ordsgn,v, bits);
          if ((j_bits >= bits)&&(j_bits != BITS_PER_LONG-bits))
          // there is enough space for the last var
            rO_LexVars(j, j_bits, r->block0[i],r->block0[i],
                         prev_ordsgn,tmp_ordsgn,v,bits);
        }
        break;

      case ringorder_wp:
        rO_WDegree(j,j_bits,r->block0[i],r->block1[i],tmp_ordsgn,
                   tmp_typ[typ_i], r->wvhdl[i]);
        r->pVarLowIndex=j;
        typ_i++;
        if (r->block1[i]!=r->block0[i])
        {
          rO_LexVars_neg(j, j_bits,r->block1[i],r->block0[i]+1, prev_ordsgn,
                         tmp_ordsgn, v,bits);
          if ((j_bits >= bits)&&(j_bits != BITS_PER_LONG-bits))
          // there is enough space for the last var
            rO_LexVars_neg(j, j_bits, r->block0[i],r->block0[i],
                         prev_ordsgn,tmp_ordsgn,v,bits);
        }
        break;

      case ringorder_Wp:
        rO_WDegree(j,j_bits,r->block0[i],r->block1[i],tmp_ordsgn,
                   tmp_typ[typ_i], r->wvhdl[i]);
        r->pVarLowIndex=j;
        typ_i++;
        if (r->block1[i]!=r->block0[i])
        {
          rO_LexVars(j, j_bits,r->block0[i],r->block1[i]-1, prev_ordsgn,
                     tmp_ordsgn,v, bits);
          if ((j_bits >= bits)&&(j_bits != BITS_PER_LONG-bits))
          // there is enough space for the last var
            rO_LexVars(j, j_bits, r->block0[i],r->block0[i],
                         prev_ordsgn,tmp_ordsgn,v,bits);
        }
        break;

      case ringorder_ws:
        rO_WDegree_neg(j,j_bits,r->block0[i],r->block1[i],tmp_ordsgn,
                       tmp_typ[typ_i], r->wvhdl[i]);
        r->pVarLowIndex=j;
        typ_i++;
        if (r->block1[i]!=r->block0[i])
        {
          rO_LexVars_neg(j, j_bits,r->block1[i],r->block0[i]+1, prev_ordsgn,
                         tmp_ordsgn, v,bits);
          if ((j_bits >= bits)&&(j_bits != BITS_PER_LONG-bits))
          // there is enough space for the last var
            rO_LexVars_neg(j, j_bits, r->block0[i],r->block0[i],
                         prev_ordsgn,tmp_ordsgn,v,bits);
        }
        break;

      case ringorder_Ws:
        rO_WDegree_neg(j,j_bits,r->block0[i],r->block1[i],tmp_ordsgn,
                       tmp_typ[typ_i], r->wvhdl[i]);
        r->pVarLowIndex=j;
        typ_i++;
        if (r->block1[i]!=r->block0[i])
        {
          rO_LexVars(j, j_bits,r->block0[i],r->block1[i]-1, prev_ordsgn,
                     tmp_ordsgn,v, bits);
          if ((j_bits >= bits)&&(j_bits != BITS_PER_LONG-bits))
          // there is enough space for the last var
            rO_LexVars_neg(j, j_bits, r->block0[i],r->block0[i],
                         prev_ordsgn,tmp_ordsgn,v,bits);
        }
        break;

      case ringorder_S:
        rO_Syzcomp(j, j_bits,prev_ordsgn, tmp_ordsgn,tmp_typ[typ_i]);
        r->pVarLowIndex=j;
        need_to_add_comp=TRUE;
        typ_i++;
        break;

      case ringorder_s:
        rO_Syz(j, j_bits,prev_ordsgn, tmp_ordsgn,tmp_typ[typ_i]);
        r->pVarLowIndex=j;
        need_to_add_comp=TRUE;
        typ_i++;
        break;

      case ringorder_unspec:
      case ringorder_no:
        default:
          dReportError("undef. ringorder used\n");
          break;
    }
  }

  int j0=j; // save j
  int j_bits0=j_bits; // save jbits
  rO_Align(j,j_bits);
  r->pCompHighIndex=j-1;

  j_bits=j_bits0; j=j0;

  // fill in some empty slots with variables not already covered
  // v0 is special, is therefore normally already covered
  // now we do have rings without comp...
  if((need_to_add_comp) && (v[0]== -1))
  {
    if (prev_ordsgn==1)
    {
      rO_Align(j, j_bits);
      rO_LexVars(j, j_bits, 0,0, prev_ordsgn,tmp_ordsgn,v,BITS_PER_LONG);
    }
    else
    {
      rO_Align(j, j_bits);
      rO_LexVars_neg(j, j_bits, 0,0, prev_ordsgn,tmp_ordsgn,v,BITS_PER_LONG);
    }
  }
  // the variables
  for(i=1 ; i<r->N+1 ; i++)
  {
    if(v[i]==(-1))
    {
      if (prev_ordsgn==1)
      {
        rO_LexVars(j, j_bits, i,i, prev_ordsgn,tmp_ordsgn,v,bits);
      }
      else
      {
        rO_LexVars_neg(j,j_bits,i,i, prev_ordsgn,tmp_ordsgn,v,bits);
      }
    }
  }

  r->pVarHighIndex=j - (j_bits==0);
  rO_Align(j,j_bits);
  // ----------------------------
  // finished with constructing the monomial, computing sizes:

  r->ExpESize=j;
  r->ExpLSize=j;
  r->PolyBin = omGetSpecBin(POLYSIZE + (r->ExpLSize)*sizeof(long));
  assume(r->PolyBin != NULL);

  // ----------------------------
  // indices and ordsgn vector for comparison
  //
  // r->pCompHighIndex already set
  r->pCompLSize = r->pCompHighIndex + 1;
  r->ordsgn=(long *)omAlloc0(r->ExpLSize*sizeof(long));

  for(j=0;j<=r->pCompHighIndex;j++)
  {
    r->ordsgn[j] = tmp_ordsgn[j];
  }

  omFreeSize((ADDRESS)tmp_ordsgn,(2*(n+r->N)*sizeof(long)));

  // ----------------------------
  // description of orderings for setm:
  //
  r->OrdSize=typ_i;
  if (typ_i==0) r->typ=NULL;
  else
  {
    r->typ=(sro_ord*)omAlloc(typ_i*sizeof(sro_ord));
    memcpy(r->typ,tmp_typ,typ_i*sizeof(sro_ord));
  }
  omFreeSize((ADDRESS)tmp_typ,(2*(n+r->N)*sizeof(sro_ord)));

  // ----------------------------
  // indices for (first copy of ) variable entries in exp.e vector (VarOffset):
  r->VarOffset=v;

  // ----------------------------
  // other indicies
  r->pCompIndex=(r->VarOffset[0] & 0xffff); //r->VarOffset[0];
  i=0; // position
  j=0; // index in r->typ
  if (i==r->pCompIndex) i++;
  while ((j < r->OrdSize)
  && ((r->typ[j].ord_typ==ro_syzcomp) || (r->typ[j].ord_typ==ro_syz)))
  {
    i++; j++;
  }
  if (i==r->pCompIndex) i++;
  r->pOrdIndex=i;

  // ----------------------------
  // p_Procs
  r->p_Procs = (p_Procs_s*)omAlloc(sizeof(p_Procs_s));
  p_SetProcs(r, r->p_Procs);
  return FALSE;
}

void rUnComplete(ring r)
{
  if (r == NULL) return;
  if (r->VarOffset != NULL)
  {
    if (r->PolyBin != NULL)
      omUnGetSpecBin(&(r->PolyBin));

    omFreeSize((ADDRESS)r->VarOffset, (r->N +1)*sizeof(int));
    if (r->order != NULL)
    {
      if (r->order[0] == ringorder_s && r->typ[0].data.syz.limit > 0)
      {
        omFreeSize(r->typ[0].data.syz.syz_index,
             (r->typ[0].data.syz.limit +1)*sizeof(int));
      }
    }
    if (r->OrdSize!=0 && r->typ != NULL)
    {
      omFreeSize((ADDRESS)r->typ,r->OrdSize*sizeof(sro_ord));
    }
    if (r->ordsgn != NULL && r->pCompLSize != 0)
      omFreeSize((ADDRESS)r->ordsgn,r->ExpLSize*sizeof(long));
    if (r->p_Procs != NULL)
      omFreeSize(r->p_Procs, sizeof(p_Procs_s));
  }
}

#ifdef RDEBUG
void rDebugPrint(ring r)
{
  if (r==NULL)
  {
    PrintS("NULL ?\n");
    return;
  }
  char *TYP[]={"ro_dp","ro_wp","ro_cp","ro_syzcomp", "ro_syz", "ro_none"};
  int i,j;
  PrintS("varoffset:\n");
  for(j=0;j<=r->N;j++) Print("  v%d at e-pos %d, bit %d\n",
     j,r->VarOffset[j] & 0xffffff, r->VarOffset[j] >>24);
  Print("bitmask=0x%x\n",r->bitmask);
  PrintS("ordsgn:\n");
  for(j=0;j<r->pCompLSize;j++)
    Print("  ordsgn %d at pos %d\n",r->ordsgn[j],j);
  Print("OrdSgn:%d\n",r->OrdSgn);
  PrintS("ordrec:\n");
  for(j=0;j<r->OrdSize;j++)
  {
    Print("  typ %s",TYP[r->typ[j].ord_typ]);
    Print("  place %d",r->typ[j].data.dp.place);
    if (r->typ[j].ord_typ!=ro_syzcomp)
    {
      Print("  start %d",r->typ[j].data.dp.start);
      Print("  end %d",r->typ[j].data.dp.end);
      if (r->typ[j].ord_typ==ro_wp)
      {
        Print(" w:");
        int l;
        for(l=r->typ[j].data.wp.start;l<=r->typ[j].data.wp.end;l++)
          Print(" %d",r->typ[j].data.wp.weights[l-r->typ[j].data.wp.start]);
      }
    }
    PrintLn();
  }
  Print("pVarLowIndex:%d ",r->pVarLowIndex);
  Print("pVarHighIndex:%d\n",r->pVarHighIndex);
#ifdef LONG_MONOMS
  Print("pDivLow:%d ",r->pDivLow);
  Print("pDivHigh:%d\n",r->pDivHigh);
#endif
  Print("pCompHighIndex:%d\n",r->pCompHighIndex);
  Print("pOrdIndex:%d pCompIndex:%d\n", r->pOrdIndex, r->pCompIndex);
  Print("ExpESize:%d ",r->ExpESize);
  Print("ExpLSize:%d ",r->ExpLSize);
  Print("OrdSize:%d\n",r->OrdSize);
  PrintS("--------------------\n");
  for(j=0;j<r->ExpLSize;j++)
  {
    Print("L[%d]: ",j);
    if (j<=r->pCompHighIndex)
      Print("ordsgn %d ", r->ordsgn[j]);
    else
      PrintS("no comp ");
    i=1;
    for(;i<=r->N;i++)
    {
      if( (r->VarOffset[i] & 0xffffff) == j )
      {  Print("v%d at e[%d], bit %d; ", i,r->VarOffset[i] & 0xffffff,
                                         r->VarOffset[i] >>24 ); }
    }
    if( r->pCompIndex==j ) PrintS("v0; ");
    for(i=0;i<r->OrdSize;i++)
    {
      if (r->typ[i].data.dp.place == j)
      {
        Print("ordrec:%s (start:%d, end:%d) ",TYP[r->typ[i].ord_typ],
          r->typ[i].data.dp.start, r->typ[i].data.dp.end);
      }
    }

    if (j==r->pOrdIndex)
      PrintS("pOrdIndex\n");
    else
      PrintLn();
  }

  // p_Procs stuff
  p_Procs_s proc_names;
  char* field;
  char* length;
  char* ord;
  p_Debug_GetProcNames(r, &proc_names);
  p_Debug_GetSpecNames(r, field, length, ord);

  Print("p_Spec  : %s, %s, %s\n", field, length, ord);
  PrintS("p_Procs :\n");
  for (i=0; i<(int) (sizeof(p_Procs_s)/sizeof(void*)); i++)
  {
    Print(" %s,\n", ((char**) &proc_names)[i]);
  }
}

void pDebugPrint(poly p)
{
  int i,j;
  pWrite(p);
  j=10;
  while(p!=NULL)
  {
    Print("\nexp[0..%d]\n",currRing->ExpLSize-1);
    for(i=0;i<currRing->ExpLSize;i++)
      Print("%d ",p->exp[i]);
    PrintLn();
    Print("v0:%d ",pGetComp(p));
    for(i=1;i<=pVariables;i++) Print(" v%d:%d",i,pGetExp(p,i));
    PrintLn();
    pIter(p);
    j--;
    if (j==0) { PrintS("...\n"); break; }
  }
}
#endif // RDEBUG


/*2
* asssume that rComplete was called with r
* assume that the first block ist ringorder_S
* change the block to reflect the sequence given by appending v
*/

#ifdef PDEBUG
void rDBChangeSComps(int* currComponents,
                     long* currShiftedComponents,
                     int length,
                     ring r)
{
  r->typ[1].data.syzcomp.length = length;
  rNChangeSComps( currComponents, currShiftedComponents, r);
}
void rDBGetSComps(int** currComponents,
                 long** currShiftedComponents,
                 int *length,
                 ring r)
{
  *length = r->typ[1].data.syzcomp.length;
  rNGetSComps( currComponents, currShiftedComponents, r);
}
#endif

void rNChangeSComps(int* currComponents, long* currShiftedComponents, ring r)
{
  assume(r->order[1]==ringorder_S);

  r->typ[1].data.syzcomp.ShiftedComponents = currShiftedComponents;
  r->typ[1].data.syzcomp.Components = currComponents;
}

void rNGetSComps(int** currComponents, long** currShiftedComponents, ring r)
{
  assume(r->order[1]==ringorder_S);

  *currShiftedComponents = r->typ[1].data.syzcomp.ShiftedComponents;
  *currComponents =   r->typ[1].data.syzcomp.Components;
}

/////////////////////////////////////////////////////////////////////////////
//
// The following routines all take as input a ring r, and return R
// where R has a certain property. P might be equal r in which case r
// had already this property
//
// Without argument, these functions work on currRing and change it,
// if necessary

// for the time being, this is still here
static ring rAssure_SyzComp(ring r, BOOLEAN complete = TRUE);
ring rCurrRingAssure_SyzComp()
{
  ring r = rAssure_SyzComp(currRing);
  if (r != currRing)
  {
    ring old_ring = currRing;
    rChangeCurrRing(r, TRUE);
    if (old_ring->qideal != NULL)
    {
      r->qideal = idrCopyR_NoSort(old_ring->qideal, old_ring);
      assume(idRankFreeModule(r->qideal) == 0);
      currQuotient = r->qideal;
    }
  }
  return r;
}

static ring rAssure_SyzComp(ring r, BOOLEAN complete = TRUE)
{
  if (r->order[0] == ringorder_s) return r;
  ring res=rCopy0(r, FALSE, FALSE);
  int i=rBlocks(r);
  int j;

  res->order=(int *)omAlloc0((i+1)*sizeof(int));
  for(j=i;j>0;j--) res->order[j]=r->order[j-1];
  res->order[0]=ringorder_s;

  res->block0=(int *)omAlloc0((i+1)*sizeof(int));
  for(j=i;j>0;j--) res->block0[j]=r->block0[j-1];

  res->block1=(int *)omAlloc0((i+1)*sizeof(int));
  for(j=i;j>0;j--) res->block1[j]=r->block1[j-1];

  int ** wvhdl =(int **)omAlloc0((i+1)*sizeof(int**));
  for(j=i;j>0;j--)
  {
    if (r->wvhdl[j-1] != NULL)
    {
      wvhdl[j] = (int*) omMemDup(r->wvhdl[j-1]);
    }
  }
  res->wvhdl = wvhdl;

  if (complete) rComplete(res, 1);
  return res;
}

static ring rAssure_CompLastBlock(ring r, BOOLEAN complete = TRUE)
{
  int last_block = rBlocks(r) - 2;
  if (r->order[last_block] != ringorder_c &&
      r->order[last_block] != ringorder_C)
  {
    int c_pos = 0;
    int i;

    for (i=0; i< last_block; i++)
    {
      if (r->order[i] == ringorder_c || r->order[i] == ringorder_C)
      {
        c_pos = i;
        break;
      }
    }
    if (c_pos != -1)
    {
      ring new_r = rCopy0(r, FALSE, TRUE);
      for (i=c_pos+1; i<=last_block; i++)
      {
        new_r->order[i-1] = new_r->order[i];
        new_r->block0[i-1] = new_r->block0[i];
        new_r->block1[i-1] = new_r->block1[i];
        new_r->wvhdl[i-1] = new_r->wvhdl[i];
      }
      new_r->order[last_block] = r->order[c_pos];
      new_r->block0[last_block] = r->block0[c_pos];
      new_r->block1[last_block] = r->block1[c_pos];
      new_r->wvhdl[last_block] = r->wvhdl[c_pos];
      if (complete) rComplete(new_r, 1);
      return new_r;
    }
  }
  return r;
}

ring rCurrRingAssure_CompLastBlock()
{
  ring new_r = rAssure_CompLastBlock(currRing);
  if (currRing != new_r)
  {
    ring old_r = currRing;
    rChangeCurrRing(new_r, TRUE);
    if (old_r->qideal != NULL)
    {
      new_r->qideal = idrCopyR(old_r->qideal, old_r);
      currQuotient = new_r->qideal;
    }
  }
  return new_r;
}

ring rCurrRingAssure_SyzComp_CompLastBlock()
{
  ring new_r_1 = rAssure_CompLastBlock(currRing, FALSE);
  ring new_r = rAssure_SyzComp(new_r_1, FALSE);

  if (new_r != currRing)
  {
    ring old_r = currRing;
    if (new_r_1 != new_r && new_r_1 != old_r) rDelete(new_r_1);
    rComplete(new_r, 1);
    rChangeCurrRing(new_r, TRUE);
    if (old_r->qideal != NULL)
    {
      new_r->qideal = idrCopyR(old_r->qideal, old_r);
      currQuotient = new_r->qideal;
    }
    rTest(new_r);
    rTest(old_r);
  }
  return new_r;
}

// use this for global orderings consisting of two blocks
static ring rCurrRingAssure_Global(rRingOrder_t b1, rRingOrder_t b2)
{
  int r_blocks = rBlocks(currRing);
  int i;

  assume(b1 == ringorder_c || b1 == ringorder_C ||
         b2 == ringorder_c || b2 == ringorder_C ||
         b2 == ringorder_S);
  if ((r_blocks == 3) &&
      (currRing->order[0] == b1) &&
      (currRing->order[1] == b2) &&
      (currRing->order[2] == 0))
    return currRing;
  ring res = rCopy0(currRing, TRUE, FALSE);
  res->order = (int*)omAlloc0(3*sizeof(int));
  res->block0 = (int*)omAlloc0(3*sizeof(int));
  res->block1 = (int*)omAlloc0(3*sizeof(int));
  res->wvhdl = (int**)omAlloc0(3*sizeof(int*));
  res->order[0] = b1;
  res->order[1] = b2;
  if (b1 == ringorder_c || b1 == ringorder_C)
  {
    res->block0[1] = 1;
    res->block1[1] = currRing->N;
  }
  else
  {
    res->block0[0] = 1;
    res->block1[0] = currRing->N;
  }
  // HANNES: This sould be set in rComplete
  res->OrdSgn = 1;
  rComplete(res, 1);
  rChangeCurrRing(res, TRUE);
  return res;
}


ring rCurrRingAssure_dp_S()
{
  return rCurrRingAssure_Global(ringorder_dp, ringorder_S);
}

ring rCurrRingAssure_dp_C()
{
  return rCurrRingAssure_Global(ringorder_dp, ringorder_C);
}

ring rCurrRingAssure_C_dp()
{
  return rCurrRingAssure_Global(ringorder_C, ringorder_dp);
}


void rSetSyzComp(int k)
{
  if (TEST_OPT_PROT) Print("{%d}", k);
  if ((currRing->typ!=NULL) && (currRing->typ[0].ord_typ==ro_syz))
  {
    assume(k > currRing->typ[0].data.syz.limit);
    int i;
    if (currRing->typ[0].data.syz.limit == 0)
    {
      currRing->typ[0].data.syz.syz_index = (int*) omAlloc0((k+1)*sizeof(int));
      currRing->typ[0].data.syz.syz_index[0] = 0;
      currRing->typ[0].data.syz.curr_index = 1;
    }
    else
    {
      currRing->typ[0].data.syz.syz_index = (int*)
        omReallocSize(currRing->typ[0].data.syz.syz_index,
                (currRing->typ[0].data.syz.limit+1)*sizeof(int),
                (k+1)*sizeof(int));
    }
    for (i=currRing->typ[0].data.syz.limit + 1; i<= k; i++)
    {
      currRing->typ[0].data.syz.syz_index[i] =
        currRing->typ[0].data.syz.curr_index;
    }
    currRing->typ[0].data.syz.limit = k;
    currRing->typ[0].data.syz.curr_index++;
  }
  else if ((currRing->order[0]!=ringorder_c) && (k!=0))
  {
    dReportError("syzcomp in incompatible ring");
  }
#ifdef PDEBUG
  extern int pDBsyzComp;
  pDBsyzComp=k;
#endif
}

// return the max-comonent wchich has syzIndex i
int rGetMaxSyzComp(int i)
{
  if ((currRing->typ!=NULL) && (currRing->typ[0].ord_typ==ro_syz) &&
      currRing->typ[0].data.syz.limit > 0 && i > 0)
  {
    assume(i <= currRing->typ[0].data.syz.limit);
    int j;
    for (j=0; j<currRing->typ[0].data.syz.limit; j++)
    {
      if (currRing->typ[0].data.syz.syz_index[j] == i  &&
          currRing->typ[0].data.syz.syz_index[j+1] != i)
      {
        assume(currRing->typ[0].data.syz.syz_index[j+1] == i+1);
        return j;
      }
    }
    return currRing->typ[0].data.syz.limit;
  }
  else
  {
    return 0;
  }
}

BOOLEAN rRing_is_Homog(ring r)
{
  if (r == NULL) return FALSE;
  int i, j, nb = rBlocks(r);
  for (i=0; i<nb; i++)
  {
    if (r->wvhdl[i] != NULL)
    {
      int length = r->block1[i] - r->block0[i];
      int* wvhdl = r->wvhdl[i];
      if (r->order[i] == ringorder_M) length *= length;
      assume(omSizeOfAddr(wvhdl) >= length*sizeof(int));

      for (j=0; j< length; j++)
      {
        if (wvhdl[j] != 0 && wvhdl[j] != 1) return FALSE;
      }
    }
  }
  return TRUE;
}

BOOLEAN rRing_has_CompLastBlock(ring r)
{
  assume(r != NULL);
  int lb = rBlocks(r) - 2;
  return (r->order[lb] == ringorder_c || r->order[lb] == ringorder_C);
}

