/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/* $Id: ring.cc,v 1.181 2002-02-04 17:12:59 Singular Exp $ */

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
  "aa", //ringorder_aa
  " _" //ringorder_unspec
};

static inline const char * rSimpleOrdStr(int ord)
{
  return ringorder_name[ord];
}

// unconditionally deletes fields in r
static void rDelete(ring r);
// set r->VarL_Size, r->VarL_Offset, r->VarL_LowIndex
static void rSetVarL(ring r);
// get r->divmask depending on bits per exponent
static unsigned long rGetDivMask(int bits);
// right-adjust r->VarOffset
static void rRightAdjustVarOffset(ring r);
static void rOptimizeLDeg(ring r);

/*0 implementation*/
//BOOLEAN rField_is_R(ring r=currRing)
//{
//  if (r->ch== -1)
//  {
//    if (r->float_len==(short)0) return TRUE;
//  }
//  return FALSE;
//}

// internally changes the gloabl ring and resets the relevant
// global variables:
// complete == FALSE : only delete operations are enabled
// complete == TRUE  : full reset of all variables
void rChangeCurrRing(ring r)
{
  /*------------ set global ring vars --------------------------------*/
  currRing = r;
  currQuotient=NULL;
  if (r != NULL)
  {
    rTest(r);
    /*------------ set global ring vars --------------------------------*/
    currQuotient=r->qideal;

    /*------------ global variables related to coefficients ------------*/
    nSetChar(r);

    /*------------ global variables related to polys -------------------*/
    pSetGlobals(r);

    /*------------ set naMinimalPoly -----------------------------------*/
    if (r->minpoly!=NULL)
    {
      naMinimalPoly=((lnumber)r->minpoly)->z;
    }
  }
}

void rSetHdl(idhdl h)
{
  int i;
  ring rg = NULL;
  if (h!=NULL)
  {
//   Print(" new ring:%s (l:%d)\n",IDID(h),IDLEV(h));
    rg = IDRING(h);
    omCheckAddrSize((ADDRESS)h,sizeof(idrec));
    if (IDID(h))  // OB: ????
      omCheckAddr((ADDRESS)IDID(h));
    rTest(rg);
  }

  // clean up history
  if (sLastPrinted.RingDependend())
  {
    sLastPrinted.CleanUp();
    memset(&sLastPrinted,0,sizeof(sleftv));
  }

   /*------------ change the global ring -----------------------*/
  rChangeCurrRing(rg);
  currRingHdl = h;
}

idhdl rDefault(char *s)
{
  idhdl tmp=NULL;

  if (s!=NULL) tmp = enterid(s, myynest, RING_CMD, &IDROOT);
  if (tmp==NULL) return NULL;

  if (ppNoether!=NULL) pDelete(&ppNoether);
  if (sLastPrinted.RingDependend())
  {
    sLastPrinted.CleanUp();
    memset(&sLastPrinted,0,sizeof(sleftv));
  }

  ring r = IDRING(tmp);

  r->ch    = 32003;
  r->N     = 3;
  /*r->P     = 0; Alloc0 in idhdl::set, ipid.cc*/
  /*names*/
  r->names = (char **) omAlloc0(3 * sizeof(char_ptr));
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
  rSetHdl(tmp);
  return currRingHdl;
}

ring rDefault(int ch, int N, char **n)
{
  ring r=(ring) omAlloc0Bin(sip_sring_bin);
  r->ch    = ch;
  r->N     = N;
  /*r->P     = 0; Alloc0 */
  /*names*/
  r->names = (char **) omAlloc0(N * sizeof(char_ptr));
  int i;
  for(i=0;i<N;i++)
  {
    r->names[i]  = omStrDup(n[i]);
  }
  /*weights: entries for 2 blocks: NULL*/
  r->wvhdl = (int **)omAlloc0(2 * sizeof(int_ptr));
  /*order: lp,0*/
  r->order = (int *) omAlloc(2* sizeof(int *));
  r->block0 = (int *)omAlloc0(2 * sizeof(int *));
  r->block1 = (int *)omAlloc0(2 * sizeof(int *));
  /* ringorder dp for the first block: var 1..N */
  r->order[0]  = ringorder_lp;
  r->block0[0] = 1;
  r->block1[0] = N;
  /* the last block: everything is 0 */
  r->order[1]  = 0;
  /*polynomial ring*/
  r->OrdSgn    = 1;

  /* complete ring intializations */
  rComplete(r);
  return r;
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
          case ringorder_aa:
          case ringorder_a:
            R->block0[n] = last+1;
            R->block1[n] = min(last+iv->length()-2 , R->N);
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
  int float_len2=0;
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
      float_len2=float_len;
      pn=pn->next;
      if ((pn->next!=NULL) && (pn->next->Typ()==INT_CMD))
      {
        float_len2=(int)pn->next->Data();
        pn=pn->next;
      }
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
  /* input: 0 ch=0 : Q     parameter=NULL    ffChar=FALSE   float_len
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
    R->float_len= min(float_len,32767);
    R->float_len2= min(float_len2,32767);
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
  && (R->float_len < SHORT_REAL_LENGTH))
  {
    R->float_len = SHORT_REAL_LENGTH;
    R->float_len2 = SHORT_REAL_LENGTH;
  }

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
  if (rComplete(R,1))
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

int r_IsRingVar(char *n, ring r)
{
  if ((r!=NULL) && (r->names!=NULL))
  {
    for (int i=0; i<r->N; i++)
    {
      if (r->names[i]==NULL) return -1;
      if (strcmp(n,r->names[i]) == 0) return (int)i;
    }
  }
  return -1;
}


void rWrite(ring r)
{
  if ((r==NULL)||(r->order==NULL))
    return; /*to avoid printing after errors....*/

  int nblocks=rBlocks(r);

  // omCheckAddrSize(r,sizeof(ip_sring));
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
      Print("0 (real:%d digits, additional %d digits)\n",
             r->float_len,r->float_len2);  /* long R */
    else if ( rField_is_long_C(r) )
      Print("0 (complex:%d digits, additional %d digits)\n",
             r->float_len, r->float_len2);  /* long C */
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
    ||(r->order[l] == ringorder_a)
    ||(r->order[l] == ringorder_aa))
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
      if (sLastPrinted.RingDependend())
      {
        sLastPrinted.CleanUp();
      }
      if ((myynest>0) && (iiRETURNEXPR[myynest].RingDependend()))
      {
        WerrorS("return value depends on local ring variable (export missing ?)");
        iiRETURNEXPR[myynest].CleanUp();
      }
      currRing=NULL;
      currRingHdl=NULL;
    }
    else if (r->qideal!=NULL)
    {
      id_Delete(&r->qideal, r);
      r->qideal = NULL;
    }
    nKillChar(r);
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
//#endif /* USE_IILOCALRING */
    {
      proclevel * nshdl = procstack;
      int lev=myynest-1;

      for(; nshdl != NULL; nshdl = nshdl->next)
      {
        if (nshdl->currRing==r)
        {
          Warn("killing the basering for level %d",lev);
          nshdl->currRing=NULL;
          nshdl->currRingHdl=NULL;
        }
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
#ifndef HAVE_NAMESPACES
  ring r = IDRING(h);
  int ref=0;
  if (r!=NULL)
  {
    ref=r->ref;
    rKill(r);
  }
  if (h==currRingHdl)
  {
    if (ref<=0) { currRing=NULL; currRingHdl=NULL;}
    else
    {
      currRingHdl=rFindHdl(r,NULL,NULL);
    }
  }
#endif /* NOT HAVE_NAMESPACES */

#ifdef HAVE_NAMESPACES
  ring r = IDRING(h);
  if (r!=NULL) rKill(r);
  if (h==currRingHdl)
  {
    namehdl nsHdl = namespaceroot;
    while(nsHdl!=NULL) {
      currRingHdl=NSROOT(nsHdl);
      while (currRingHdl!=NULL)
      {
        if ((currRingHdl!=h)
            && (IDTYP(currRingHdl)==IDTYP(h))
            && (h->data.uring==currRingHdl->data.uring))
          break;
        currRingHdl=IDNEXT(currRingHdl);
      }
      if ((currRingHdl != NULL) && (currRingHdl!=h)
          && (IDTYP(currRingHdl)==IDTYP(h))
          && (h->data.uring==currRingHdl->data.uring))
        break;
      nsHdl = nsHdl->next;
    }
  }
#endif /* HAVE_NAMESPACES */
}

idhdl rSimpleFindHdl(ring r, idhdl root)
{
  idhdl h=root;
  while (h!=NULL)
  {
    if (((IDTYP(h)==RING_CMD)||(IDTYP(h)==QRING_CMD))
        && (h->data.uring==r))
      return h;
    h=IDNEXT(h);
  }
  return NULL;
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
  idhdl h=rSimpleFindHdl(r,IDROOT);
  if (h!=NULL)  return h;
#ifdef HAVE_NS
  if (IDROOT!=basePack->idroot) h=rSimpleFindHdl(r,basePack->idroot);
  if (h!=NULL)  return h;
  proclevel *p=procstack;
  while(p!=NULL)
  {
    if ((p->currPack!=basePack)
    && (p->currPack!=currPack))
      h=rSimpleFindHdl(r,p->currPack->idroot);
    if (h!=NULL)  return h;
    p=p->next;
  }
  idhdl tmp=basePack->idroot;
  while (tmp!=NULL)
  {
    if (IDTYP(tmp)==PACKAGE_CMD)
      h=rSimpleFindHdl(r,IDPACKAGE(tmp)->idroot);
    if (h!=NULL)  return h;
    tmp=IDNEXT(tmp);
  }
#endif
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
  ring save=currRing;
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
          // HANNES: TODO: delete nSetChar
          rChangeCurrRing(r1);
          if ((strcmp(r1->parameter[0],r2->parameter[0])==0) /* 1 char */
              && n_Equal(r1->minpoly,r2->minpoly, r1))
          {
            tmpR.parameter=(char **)omAllocBin(char_ptr_bin);
            tmpR.parameter[0]=omStrDup(r1->parameter[0]);
            tmpR.minpoly=n_Copy(r1->minpoly, r1);
            tmpR.P=1;
            // HANNES: TODO: delete nSetChar
            rChangeCurrRing(save);
          }
          else
          {
            // HANNES: TODO: delete nSetChar
            rChangeCurrRing(save);
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
            tmpR.minpoly=n_Copy(r1->minpoly, r1);
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
            tmpR.minpoly=n_Copy(r2->minpoly, r2);
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
          tmpR.minpoly=n_Copy(r1->minpoly, r1);
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
          tmpR.minpoly=n_Copy(r2->minpoly, r2);
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
          tmpR.minpoly=n_Copy(r1->minpoly, r1);
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
          tmpR.minpoly=n_Copy(r2->minpoly, r2);
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
        //Print("name : %d : %s\n",k,r2->names[i]);
        names[k]=omStrDup(r2->names[i]);
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
      for (i=0;r2->order[i]!=0;i++)
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
          j++;
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
  if (r->algring!=NULL)
    r->algring->ref++;
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

  res->names   = (char **)omAlloc0(r->N * sizeof(char_ptr));
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
  || (r1->float_len != r2->float_len)
  || (r1->float_len2 != r2->float_len2)
  || (r1->N != r2->N)
  || (r1->OrdSgn != r2->OrdSgn)
  || (rPar(r1) != rPar(r2)))
    return 0;

  for (i=0; i<r1->N; i++)
  {
    if (r1->names[i] != NULL && r2->names[i] != NULL)
    {
      if (strcmp(r1->names[i], r2->names[i])) return 0;
    }
    else if ((r1->names[i] != NULL) ^ (r2->names[i] != NULL))
    {
      return 0;
    }
  }

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
  if (r->order[0] == ringorder_unspec) return TRUE;
  int blocks = rBlocks(r) - 1;
  assume(blocks >= 1);
  if (blocks == 1) return TRUE;
  if (blocks > 2)  return FALSE;
  if (r->order[0] != ringorder_c && r->order[0] != ringorder_C &&
      r->order[1] != ringorder_c && r->order[1] != ringorder_C)
    return FALSE;
  if (r->order[1] == ringorder_M || r->order[0] == ringorder_M)
    return FALSE;
  return TRUE;
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

BOOLEAN rOrder_is_DegOrdering(rRingOrder_t order)
{
  switch(order)
  {
      case ringorder_dp:
      case ringorder_Dp:
      case ringorder_ds:
      case ringorder_Ds:
      case ringorder_Ws:
      case ringorder_Wp:
      case ringorder_ws:
      case ringorder_wp:
        return TRUE;

      default:
        return FALSE;
  }
}

BOOLEAN rOrder_is_WeightedOrdering(rRingOrder_t order)
{
  switch(order)
  {
      case ringorder_Ws:
      case ringorder_Wp:
      case ringorder_ws:
      case ringorder_wp:
        return TRUE;

      default:
        return FALSE;
  }
}

BOOLEAN rHasSimpleOrderAA(ring r)
{
  int blocks = rBlocks(r) - 1;
  if (blocks > 3 || blocks < 2) return FALSE;
  if (blocks == 3)
  {
    return ((r->order[0] == ringorder_aa && r->order[1] != ringorder_M &&
             (r->order[2] == ringorder_c || r->order[2] == ringorder_C)) ||
            ((r->order[0] == ringorder_c || r->order[0] == ringorder_C) &&
             r->order[1] == ringorder_aa && r->order[2] != ringorder_M));
  }
  else
  {
    return (r->order[0] == ringorder_aa && r->order[1] != ringorder_M);
  }
}

// return TRUE if p_SetComp requires p_Setm
BOOLEAN rOrd_SetCompRequiresSetm(ring r)
{
  if (r->typ != NULL)
  {
    int pos;
    for (pos=0;pos<r->OrdSize;pos++)
    {
      sro_ord* o=&(r->typ[pos]);
      if (o->ord_typ == ro_syzcomp || o->ord_typ == ro_syz) return TRUE;
    }
  }
  return FALSE;
}

// return TRUE if p->exp[r->pOrdIndex] holds total degree of p */
BOOLEAN rOrd_is_Totaldegree_Ordering(ring r)
{
  // Hmm.... what about Syz orderings?
  return (r->N > 1 &&
          ((rHasSimpleOrder(r) &&
           (rOrder_is_DegOrdering((rRingOrder_t)r->order[0]) ||
            rOrder_is_DegOrdering(( rRingOrder_t)r->order[1]))) ||
           (rHasSimpleOrderAA(r) &&
            (rOrder_is_DegOrdering((rRingOrder_t)r->order[1]) ||
             rOrder_is_DegOrdering((rRingOrder_t)r->order[2])))));
}

// return TRUE if p->exp[r->pOrdIndex] holds a weighted degree of p */
BOOLEAN rOrd_is_WeightedDegree_Ordering(ring r =currRing)
{
  // Hmm.... what about Syz orderings?
  return (r->N > 1 &&
          rHasSimpleOrder(r) &&
          (rOrder_is_WeightedOrdering((rRingOrder_t)r->order[0]) ||
           rOrder_is_WeightedOrdering(( rRingOrder_t)r->order[1])));
}

BOOLEAN rIsPolyVar(int v, ring r)
{
  int  i=0;
  while(r->order[i]!=0)
  {
    if((r->block0[i]<=v)
    && (r->block1[i]>=v))
    {
      switch(r->order[i])
      {
        case ringorder_a:
          return (r->wvhdl[i][v-r->block0[i]]>0);
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
      if (i > 0 && ((tmp<0) ||(tmp>r->ExpL_Size-1)))
      {
        dReportError("varoffset out of range for var %d: %d",i,tmp);
      }
  }
  if(r->typ!=NULL)
  {
    for(j=0;j<r->OrdSize;j++)
    {
      if ((r->typ[j].ord_typ==ro_dp)
      || (r->typ[j].ord_typ==ro_wp)
      || (r->typ[j].ord_typ==ro_wp_neg))
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
  //assume(r->cf!=NULL);

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
  int i;
  for(i=start;i<=end;i++)
  {
    if(weights[i-start]<0)
    {
      ord_struct.ord_typ=ro_wp_neg;
      break;
    }
  }
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
  int i;
  for(i=start;i<=end;i++)
  {
    if(weights[i-start]<0)
    {
      ord_struct.ord_typ=ro_wp_neg;
      break;
    }
  }
}

static void rO_LexVars(int &place, int &bitplace, int start, int end,
  int &prev_ord, long *o,int *v, int bits, int opt_var)
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
  if (opt_var!= -1)
  {
    assume((opt_var == end+1) ||(opt_var == end-1));
    if((opt_var != end+1) &&(opt_var != end-1)) WarnS("hier-2");
    int save_bitplace=bitplace;
    bitplace-=bits;
    if (bitplace < 0)
    {
      bitplace=save_bitplace;
      return;
    }
    // there is enough space for the optional var
    v[opt_var]=place | (bitplace << 24);
  }
}

static void rO_LexVars_neg(int &place, int &bitplace, int start, int end,
  int &prev_ord, long *o,int *v, int bits, int opt_var)
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
//  #if 0
  if (opt_var!= -1)
  {
    assume((opt_var == end+1) ||(opt_var == end-1));
    if((opt_var != end+1) &&(opt_var != end-1)) WarnS("hier-1");
    int save_bitplace=bitplace;
    bitplace-=bits;
    if (bitplace < 0)
    {
      bitplace=save_bitplace;
      return;
    }
    // there is enough space for the optional var
    v[opt_var]=place | (bitplace << 24);
  }
//  #endif
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

static unsigned long rGetExpSize(unsigned long bitmask, int & bits)
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
* optimize rGetExpSize for a block of N variables, exp <=bitmask
*/
static unsigned long rGetExpSize(unsigned long bitmask, int & bits, int N)
{
  bitmask =rGetExpSize(bitmask, bits);
  int vars_per_long=BIT_SIZEOF_LONG/bits;
  int bits1;
  loop
  {
    if (bits == BIT_SIZEOF_LONG)
    {
      bits =  BIT_SIZEOF_LONG - 1;
      return LONG_MAX;
    }
    unsigned long bitmask1 =rGetExpSize(bitmask+1, bits1);
    int vars_per_long1=BIT_SIZEOF_LONG/bits1;
    if ((((N+vars_per_long-1)/vars_per_long) ==
         ((N+vars_per_long1-1)/vars_per_long1)))
    {
      vars_per_long=vars_per_long1;
      bits=bits1;
      bitmask=bitmask1;
    }
    else
    {
      return bitmask; /* and bits */
    }
  }
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
  BOOLEAN omitted_degree = FALSE;
  int bits;

  exp_limit=rGetExpSize(exp_limit, bits, r->N);
  need_other_ring = (exp_limit != r->bitmask);

  int nblocks=rBlocks(r);
  int *order=(int*)omAlloc0((nblocks+1)*sizeof(int));
  int *block0=(int*)omAlloc0((nblocks+1)*sizeof(int));
  int *block1=(int*)omAlloc0((nblocks+1)*sizeof(int));
  int **wvhdl=(int**)omAlloc0((nblocks+1)*sizeof(int_ptr));

  int i=0;
  int j=0; /*  i index in r, j index in res */
  loop
  {
    BOOLEAN copy_block_index=TRUE;
    int r_ord=r->order[i];
    if (r->block0[i]==r->block1[i])
    {
      switch(r_ord)
      {
        case ringorder_wp:
        case ringorder_dp:
        case ringorder_Wp:
        case ringorder_Dp:
          r_ord=ringorder_lp;
          break;
        case ringorder_Ws:
        case ringorder_Ds:
        case ringorder_ws:
        case ringorder_ds:
          r_ord=ringorder_ls;
          break;
        default:
          break;
      }
    }
    switch(r_ord)
    {
      case ringorder_C:
      case ringorder_c:
        if (!omit_comp)
        {
          order[j]=r_ord; /*r->order[i]*/;
        }
        else
        {
          j--;
          need_other_ring=TRUE;
          omit_comp=FALSE;
          copy_block_index=FALSE;
        }
        break;
      case ringorder_wp:
      case ringorder_dp:
      case ringorder_ws:
      case ringorder_ds:
        if(!omit_degree)
        {
          order[j]=r_ord; /*r->order[i]*/;
        }
        else
        {
          order[j]=ringorder_rp;
          need_other_ring=TRUE;
          omit_degree=FALSE;
          omitted_degree = TRUE;
        }
        break;
      case ringorder_Wp:
      case ringorder_Dp:
      case ringorder_Ws:
      case ringorder_Ds:
        if(!omit_degree)
        {
          order[j]=r_ord; /*r->order[i];*/
        }
        else
        {
          order[j]=ringorder_lp;
          need_other_ring=TRUE;
          omit_degree=FALSE;
          omitted_degree = TRUE;
        }
        break;
      default:
        order[j]=r_ord; /*r->order[i];*/
        break;
    }
    if (copy_block_index)
    {
      block0[j]=r->block0[i];
      block1[j]=r->block1[i];
      wvhdl[j]=r->wvhdl[i];
    }
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
  *res = *r;
  // res->qideal, res->idroot ???
  res->wvhdl=wvhdl;
  res->order=order;
  res->block0=block0;
  res->block1=block1;
  res->bitmask=exp_limit;
  int tmpref=r->cf->ref;
  rComplete(res, 1);
  r->cf->ref=tmpref;

  // adjust res->pFDeg: if it was changed globally, then
  // it must also be changed for new ring
  if (r->pFDegOrig != res->pFDegOrig &&
           rOrd_is_WeightedDegree_Ordering(r))
  {
    // still might need adjustment for weighted orderings
    // and omit_degree
    res->firstwv = r->firstwv;
    res->firstBlockEnds = r->firstBlockEnds;
    res->pFDeg = res->pFDegOrig = pWFirstTotalDegree;
  }
  if (omitted_degree)
    res->pLDeg = res->pLDegOrig = r->pLDegOrig;

  rOptimizeLDeg(res);

  // set syzcomp
  if (res->typ != NULL && res->typ[0].ord_typ == ro_syz)
  {
    res->typ[0] = r->typ[0];
    if (r->typ[0].data.syz.limit > 0)
    {
      res->typ[0].data.syz.syz_index
        = (int*) omAlloc((r->typ[0].data.syz.limit +1)*sizeof(int));
      memcpy(res->typ[0].data.syz.syz_index, r->typ[0].data.syz.syz_index,
             (r->typ[0].data.syz.limit +1)*sizeof(int));
    }
  }
  return res;
}

// construct Wp,C ring
ring rModifyRing_Wp(ring r, int* weights)
{
  ring res=(ring)omAlloc0Bin(ip_sring_bin);
  *res = *r;
  /*weights: entries for 3 blocks: NULL*/
  res->wvhdl = (int **)omAlloc0(3 * sizeof(int_ptr));
  /*order: dp,C,0*/
  res->order = (int *) omAlloc(3 * sizeof(int *));
  res->block0 = (int *)omAlloc0(3 * sizeof(int *));
  res->block1 = (int *)omAlloc0(3 * sizeof(int *));
  /* ringorder dp for the first block: var 1..3 */
  res->order[0]  = ringorder_Wp;
  res->block0[0] = 1;
  res->block1[0] = r->N;
  res->wvhdl[0] = weights;
  /* ringorder C for the second block: no vars */
  res->order[1]  = ringorder_C;
  /* the last block: everything is 0 */
  res->order[2]  = 0;
  /*polynomial ring*/
  res->OrdSgn    = 1;

  int tmpref=r->cf->ref;
  rComplete(res, 1);
  r->cf->ref=tmpref;
  return res;
}

// construct lp ring
ring rModifyRing_Simple(ring r, BOOLEAN ommit_degree, BOOLEAN ommit_comp, unsigned long exp_limit, BOOLEAN &simple)
{
  simple=TRUE;
  if (!rHasSimpleOrder(r))
  {
    WarnS("Hannes: you still need to implement this");
    // simple=FALSE; // sorting needed
  }
  return rModifyRing(r, ommit_degree, ommit_comp, exp_limit);
}

void rKillModifiedRing_Simple(ring r)
{
  rKillModifiedRing(r);
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

void rKillModified_Wp_Ring(ring r)
{
  rUnComplete(r);
  omFree(r->order);
  omFree(r->block0);
  omFree(r->block1);
  omFree(r->wvhdl[0]);
  omFree(r->wvhdl);
  omFreeBin(r,ip_sring_bin);
}

static void rSetOutParams(ring r)
{
  r->VectorOut = (r->order[0] == ringorder_c);
  r->ShortOut = TRUE;
#ifdef HAVE_TCL
  if (tcllmode)
  {
    r->ShortOut = FALSE;
  }
  else
#endif
  {
    int i;
    if ((r->parameter!=NULL) && (r->ch<2))
    {
      for (i=0;i<rPar(r);i++)
      {
        if(strlen(r->parameter[i])>1)
        {
          r->ShortOut=FALSE;
          break;
        }
      }
    }
    if (r->ShortOut)
    {
      // Hmm... sometimes (e.g., from maGetPreimage) new variables
      // are intorduced, but their names are never set
      // hence, we do the following awkward trick
      int N = omSizeWOfAddr(r->names);
      if (r->N < N) N = r->N;

      for (i=(N-1);i>=0;i--)
      {
        if(r->names[i] != NULL && strlen(r->names[i])>1)
        {
          r->ShortOut=FALSE;
          break;
        }
      }
    }
  }
  r->CanShortOut = r->ShortOut;
}

/*2
* sets pMixedOrder and pComponentOrder for orderings with more than one block
* block of variables (ip is the block number, o_r the number of the ordering)
* o is the position of the orderingering in r
*/
static void rHighSet(ring r, int o_r, int o)
{
  switch(o_r)
  {
    case ringorder_lp:
    case ringorder_dp:
    case ringorder_Dp:
    case ringorder_wp:
    case ringorder_Wp:
    case ringorder_rp:
    case ringorder_a:
    case ringorder_aa:
      if (r->OrdSgn==-1) r->MixedOrder=TRUE;
      break;
    case ringorder_ls:
    case ringorder_ds:
    case ringorder_Ds:
    case ringorder_s:
      break;
    case ringorder_ws:
    case ringorder_Ws:
      if (r->wvhdl[o]!=NULL)
      {
        int i;
        for(i=r->block1[o]-r->block0[o];i>=0;i--)
          if (r->wvhdl[o][i]<0) { r->MixedOrder=TRUE; break; }
      }
      break;
    case ringorder_c:
      r->ComponentOrder=1;
      break;
    case ringorder_C:
    case ringorder_S:
      r->ComponentOrder=-1;
      break;
    case ringorder_M:
      r->MixedOrder=TRUE;
      break;
    default:
      dReportError("wrong internal ordering:%d at %s, l:%d\n",o_r,__FILE__,__LINE__);
  }
}

static void rSetFirstWv(ring r, int i, int* order, int* block1, int** wvhdl)
{
  // cheat for ringorder_aa
  if (order[i] == ringorder_aa)
    i++;
  if(block1[i]!=r->N) r->LexOrder=TRUE;
  r->firstBlockEnds=block1[i];
  r->firstwv = wvhdl[i];
  if ((order[i]== ringorder_ws) || (order[i]==ringorder_Ws))
  {
    int j;
    for(j=block1[i]-r->block0[i];j>=0;j--)
      if (r->firstwv[j]<0) { r->MixedOrder=TRUE; break; }
  }
}

static void rOptimizeLDeg(ring r)
{
  if (r->pFDeg == pDeg)
  {
    if (r->pLDeg == pLDeg1)
      r->pLDeg = pLDeg1_Deg;
    if (r->pLDeg == pLDeg1c)
      r->pLDeg = pLDeg1c_Deg;
  }
  else if (r->pFDeg == pTotaldegree)
  {
    if (r->pLDeg == pLDeg1)
      r->pLDeg = pLDeg1_Totaldegree;
    if (r->pLDeg == pLDeg1c)
      r->pLDeg = pLDeg1c_Totaldegree;
  }
  else if (r->pFDeg == pWFirstTotalDegree)
  {
    if (r->pLDeg == pLDeg1)
      r->pLDeg = pLDeg1_WFirstTotalDegree;
    if (r->pLDeg == pLDeg1c)
      r->pLDeg = pLDeg1c_WFirstTotalDegree;
  }
}

// set pFDeg, pLDeg, MixOrder, ComponentOrder, etc
static void rSetDegStuff(ring r)
{
  int* order = r->order;
  int* block0 = r->block0;
  int* block1 = r->block1;
  int** wvhdl = r->wvhdl;

  if (order[0]==ringorder_S ||order[0]==ringorder_s)
  {
    order++;
    block0++;
    block1++;
    wvhdl++;
  }
  r->LexOrder = FALSE;
  r->MixedOrder = FALSE;
  r->ComponentOrder = 1;
  r->pFDeg = pTotaldegree;
  r->pLDeg = (r->OrdSgn == 1 ? pLDegb : pLDeg0);

  /*======== ordering type is (_,c) =========================*/
  if ((order[0]==ringorder_unspec) || (order[1] == 0)
      ||(
    ((order[1]==ringorder_c)||(order[1]==ringorder_C)
     ||(order[1]==ringorder_S)
     ||(order[1]==ringorder_s))
    && (order[0]!=ringorder_M)
    && (order[2]==0))
    )
  {
    if ((order[0]!=ringorder_unspec)
    && ((order[1]==ringorder_C)||(order[1]==ringorder_S)||
        (order[1]==ringorder_s)))
      r->ComponentOrder=-1;
    if (r->OrdSgn == -1) r->pLDeg = pLDeg0c;
    if ((order[0] == ringorder_lp) || (order[0] == ringorder_ls) || order[0] == ringorder_rp)
    {
      r->LexOrder=TRUE;
      r->pLDeg = pLDeg1c;
    }
    if (order[0] == ringorder_wp || order[0] == ringorder_Wp ||
        order[0] == ringorder_ws || order[0] == ringorder_Ws)
      r->pFDeg = pWFirstTotalDegree;
    r->firstBlockEnds=block1[0];
    r->firstwv = wvhdl[0];
  }
  /*======== ordering type is (c,_) =========================*/
  else if (((order[0]==ringorder_c)
            ||(order[0]==ringorder_C)
            ||(order[0]==ringorder_S)
            ||(order[0]==ringorder_s))
  && (order[1]!=ringorder_M)
  &&  (order[2]==0))
  {
    if ((order[0]==ringorder_C)||(order[0]==ringorder_S)||
        order[0]==ringorder_s)
      r->ComponentOrder=-1;
    if ((order[1] == ringorder_lp) || (order[1] == ringorder_ls) || order[1] == ringorder_rp)
    {
      r->LexOrder=TRUE;
      r->pLDeg = pLDeg1c;
    }
    r->firstBlockEnds=block1[1];
    r->firstwv = wvhdl[1];
    if (order[1] == ringorder_wp || order[1] == ringorder_Wp ||
        order[1] == ringorder_ws || order[1] == ringorder_Ws)
      r->pFDeg = pWFirstTotalDegree;
  }
  /*------- more than one block ----------------------*/
  else
  {
    if ((r->VectorOut)||(order[0]==ringorder_C)||(order[0]==ringorder_S)||(order[0]==ringorder_s))
    {
      rSetFirstWv(r, 1, order, block1, wvhdl);
    }
    else
      rSetFirstWv(r, 0, order, block1, wvhdl);

    /*the number of orderings:*/
    int i = 0;
    while (order[++i] != 0);
    do
    {
      i--;
      rHighSet(r, order[i],i);
    }
    while (i != 0);

    if ((order[0]!=ringorder_c)
        && (order[0]!=ringorder_C)
        && (order[0]!=ringorder_S)
        && (order[0]!=ringorder_s))
    {
      r->pLDeg = pLDeg1c;
    }
    else
    {
      r->pLDeg = pLDeg1;
    }
    r->pFDeg = pWTotaldegree; // may be improved: pTotaldegree for lp/dp/ls/.. blocks
  }
  if (rOrd_is_Totaldegree_Ordering(r) || rOrd_is_WeightedDegree_Ordering(r))
    r->pFDeg = pDeg;

  r->pFDegOrig = r->pFDeg;
  r->pLDegOrig = r->pLDeg;
  rOptimizeLDeg(r);
}

/*2
* set NegWeightL_Size, NegWeightL_Offset
*/
static void rSetNegWeight(ring r)
{
  int i,l;
  if (r->typ!=NULL)
  {
    l=0;
    for(i=0;i<r->OrdSize;i++)
    {
      if(r->typ[i].ord_typ==ro_wp_neg) l++;
    }
    if (l>0)
    {
      r->NegWeightL_Size=l;
      r->NegWeightL_Offset=(int *) omAlloc(l*sizeof(int));
      l=0;
      for(i=0;i<r->OrdSize;i++)
      {
        if(r->typ[i].ord_typ==ro_wp_neg)
        {
          r->NegWeightL_Offset[l]=r->typ[i].data.wp.place;
          l++;
        }
      }
      return;
    }
  }
  r->NegWeightL_Size = 0;
  r->NegWeightL_Offset = NULL;
}

static void rSetOption(ring r)
{
  // set redthrough
  if (!TEST_OPT_OLDSTD && r->OrdSgn == 1 && ! r->LexOrder)
    r->options |= Sy_bit(OPT_REDTHROUGH);
  else
    r->options &= ~Sy_bit(OPT_REDTHROUGH);

  // set intStrategy
  if (rField_is_Extension(r) || rField_is_Q(r))
    r->options |= Sy_bit(OPT_INTSTRATEGY);
  else
    r->options &= ~Sy_bit(OPT_INTSTRATEGY);

  // set redTail
  if (r->LexOrder || r->OrdSgn == -1 || rField_is_Extension(r))
    r->options &= ~Sy_bit(OPT_REDTAIL);
  else
    r->options |= Sy_bit(OPT_REDTAIL);
}

BOOLEAN rComplete(ring r, int force)
{
  if (r->VarOffset!=NULL && force == 0) return FALSE;
  nInitChar(r);
  rSetOutParams(r);
  int n=rBlocks(r)-1;
  int i;
  int bits;
  r->bitmask=rGetExpSize(r->bitmask,bits,r->N);
  r->BitsPerExp = bits;
  r->ExpPerLong = BIT_SIZEOF_LONG / bits;
  r->divmask=rGetDivMask(bits);

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

  // fill in v, tmp_typ, tmp_ordsgn, determine typ_i (== ordSize)
  int j=0;
  int j_bits=BITS_PER_LONG;
  BOOLEAN need_to_add_comp=FALSE;
  for(i=0;i<n;i++)
  {
    tmp_typ[typ_i].order_index=i;
    switch (r->order[i])
    {
      case ringorder_a:
      case ringorder_aa:
        rO_WDegree(j,j_bits,r->block0[i],r->block1[i],tmp_ordsgn,tmp_typ[typ_i],
                   r->wvhdl[i]);
        typ_i++;
        break;

      case ringorder_c:
        rO_Align(j, j_bits);
        rO_LexVars_neg(j, j_bits, 0,0, prev_ordsgn,tmp_ordsgn,v,BITS_PER_LONG, -1);
        break;

      case ringorder_C:
        rO_Align(j, j_bits);
        rO_LexVars(j, j_bits, 0,0, prev_ordsgn,tmp_ordsgn,v,BITS_PER_LONG, -1);
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
          break;
        }

      case ringorder_lp:
        rO_LexVars(j, j_bits, r->block0[i],r->block1[i], prev_ordsgn,
                   tmp_ordsgn,v,bits, -1);
        break;

      case ringorder_ls:
        rO_LexVars_neg(j, j_bits, r->block0[i],r->block1[i], prev_ordsgn,
                       tmp_ordsgn,v, bits, -1);
        break;

      case ringorder_rp:
        rO_LexVars_neg(j, j_bits, r->block1[i],r->block0[i], prev_ordsgn,
                       tmp_ordsgn,v, bits, -1);
        break;

      case ringorder_dp:
        if (r->block0[i]==r->block1[i])
        {
          rO_LexVars(j, j_bits, r->block0[i],r->block0[i], prev_ordsgn,
                     tmp_ordsgn,v, bits, -1);
        }
        else
        {
          rO_TDegree(j,j_bits,r->block0[i],r->block1[i],tmp_ordsgn,
                     tmp_typ[typ_i]);
          typ_i++;
          rO_LexVars_neg(j, j_bits, r->block1[i],r->block0[i]+1,
                         prev_ordsgn,tmp_ordsgn,v,bits, r->block0[i]);
        }
        break;

      case ringorder_Dp:
        if (r->block0[i]==r->block1[i])
        {
          rO_LexVars(j, j_bits, r->block0[i],r->block0[i], prev_ordsgn,
                     tmp_ordsgn,v, bits, -1);
        }
        else
        {
          rO_TDegree(j,j_bits,r->block0[i],r->block1[i],tmp_ordsgn,
                     tmp_typ[typ_i]);
          typ_i++;
          rO_LexVars(j, j_bits, r->block0[i],r->block1[i]-1, prev_ordsgn,
                     tmp_ordsgn,v, bits, r->block1[i]);
        }
        break;

      case ringorder_ds:
        if (r->block0[i]==r->block1[i])
        {
          rO_LexVars_neg(j, j_bits,r->block0[i],r->block1[i],prev_ordsgn,
                         tmp_ordsgn,v,bits, -1);
        }
        else
        {
          rO_TDegree_neg(j,j_bits,r->block0[i],r->block1[i],tmp_ordsgn,
                         tmp_typ[typ_i]);
          typ_i++;
          rO_LexVars_neg(j, j_bits, r->block1[i],r->block0[i]+1,
                         prev_ordsgn,tmp_ordsgn,v,bits, r->block0[i]);
        }
        break;

      case ringorder_Ds:
        if (r->block0[i]==r->block1[i])
        {
          rO_LexVars_neg(j, j_bits, r->block0[i],r->block0[i],prev_ordsgn,
                         tmp_ordsgn,v, bits, -1);
        }
        else
        {
          rO_TDegree_neg(j,j_bits,r->block0[i],r->block1[i],tmp_ordsgn,
                         tmp_typ[typ_i]);
          typ_i++;
          rO_LexVars(j, j_bits, r->block0[i],r->block1[i]-1, prev_ordsgn,
                     tmp_ordsgn,v, bits, r->block1[i]);
        }
        break;

      case ringorder_wp:
        rO_WDegree(j,j_bits,r->block0[i],r->block1[i],tmp_ordsgn,
                   tmp_typ[typ_i], r->wvhdl[i]);
        typ_i++;
        if (r->block1[i]!=r->block0[i])
        {
          rO_LexVars_neg(j, j_bits,r->block1[i],r->block0[i]+1, prev_ordsgn,
                         tmp_ordsgn, v,bits, r->block0[i]);
        }
        break;

      case ringorder_Wp:
        rO_WDegree(j,j_bits,r->block0[i],r->block1[i],tmp_ordsgn,
                   tmp_typ[typ_i], r->wvhdl[i]);
        typ_i++;
        if (r->block1[i]!=r->block0[i])
        {
          rO_LexVars(j, j_bits,r->block0[i],r->block1[i]-1, prev_ordsgn,
                     tmp_ordsgn,v, bits, r->block1[i]);
        }
        break;

      case ringorder_ws:
        rO_WDegree_neg(j,j_bits,r->block0[i],r->block1[i],tmp_ordsgn,
                       tmp_typ[typ_i], r->wvhdl[i]);
        typ_i++;
        if (r->block1[i]!=r->block0[i])
        {
          rO_LexVars_neg(j, j_bits,r->block1[i],r->block0[i]+1, prev_ordsgn,
                         tmp_ordsgn, v,bits, r->block0[i]);
        }
        break;

      case ringorder_Ws:
        rO_WDegree_neg(j,j_bits,r->block0[i],r->block1[i],tmp_ordsgn,
                       tmp_typ[typ_i], r->wvhdl[i]);
        typ_i++;
        if (r->block1[i]!=r->block0[i])
        {
          rO_LexVars(j, j_bits,r->block0[i],r->block1[i]-1, prev_ordsgn,
                     tmp_ordsgn,v, bits, r->block1[i]);
        }
        break;

      case ringorder_S:
        rO_Syzcomp(j, j_bits,prev_ordsgn, tmp_ordsgn,tmp_typ[typ_i]);
        need_to_add_comp=TRUE;
        typ_i++;
        break;

      case ringorder_s:
        rO_Syz(j, j_bits,prev_ordsgn, tmp_ordsgn,tmp_typ[typ_i]);
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
  r->CmpL_Size = j;

  j_bits=j_bits0; j=j0;

  // fill in some empty slots with variables not already covered
  // v0 is special, is therefore normally already covered
  // now we do have rings without comp...
  if((need_to_add_comp) && (v[0]== -1))
  {
    if (prev_ordsgn==1)
    {
      rO_Align(j, j_bits);
      rO_LexVars(j, j_bits, 0,0, prev_ordsgn,tmp_ordsgn,v,BITS_PER_LONG, -1);
    }
    else
    {
      rO_Align(j, j_bits);
      rO_LexVars_neg(j, j_bits, 0,0, prev_ordsgn,tmp_ordsgn,v,BITS_PER_LONG, -1);
    }
  }
  // the variables
  for(i=1 ; i<r->N+1 ; i++)
  {
    if(v[i]==(-1))
    {
      if (prev_ordsgn==1)
      {
        rO_LexVars(j, j_bits, i,i, prev_ordsgn,tmp_ordsgn,v,bits, -1);
      }
      else
      {
        rO_LexVars_neg(j,j_bits,i,i, prev_ordsgn,tmp_ordsgn,v,bits, -1);
      }
    }
  }

  rO_Align(j,j_bits);
  // ----------------------------
  // finished with constructing the monomial, computing sizes:

  r->ExpL_Size=j;
  r->PolyBin = omGetSpecBin(POLYSIZE + (r->ExpL_Size)*sizeof(long));
  assume(r->PolyBin != NULL);

  // ----------------------------
  // indices and ordsgn vector for comparison
  //
  // r->pCompHighIndex already set
  r->ordsgn=(long *)omAlloc0(r->ExpL_Size*sizeof(long));

  for(j=0;j<r->CmpL_Size;j++)
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
         && ((r->typ[j].ord_typ==ro_syzcomp) ||
             (r->typ[j].ord_typ==ro_syz) ||
             (r->order[r->typ[j].order_index] == ringorder_aa)))
  {
    i++; j++;
  }
  if (i==r->pCompIndex) i++;
  r->pOrdIndex=i;

  // ----------------------------
  rSetDegStuff(r);
  rSetOption(r);
  // ----------------------------
  // r->p_Setm
  r->p_Setm = p_GetSetmProc(r);

  // ----------------------------
  // set VarL_*
  rSetVarL(r);

  //  ----------------------------
  // right-adjust VarOffset
  rRightAdjustVarOffset(r);

  // ----------------------------
  // set NegWeightL*
  rSetNegWeight(r);

  // ----------------------------
  // p_Procs: call AFTER NegWeightL
  r->p_Procs = (p_Procs_s*)omAlloc(sizeof(p_Procs_s));
  p_ProcsSet(r, r->p_Procs);

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
    if (r->ordsgn != NULL && r->CmpL_Size != 0)
      omFreeSize((ADDRESS)r->ordsgn,r->ExpL_Size*sizeof(long));
    if (r->p_Procs != NULL)
      omFreeSize(r->p_Procs, sizeof(p_Procs_s));
    omfreeSize(r->VarL_Offset, r->VarL_Size*sizeof(int));
  }
  if (r->NegWeightL_Offset!=NULL)
  {
    omFreeSize(r->NegWeightL_Offset, r->NegWeightL_Size*sizeof(int));
    r->NegWeightL_Offset=NULL;
  }
}

// set r->VarL_Size, r->VarL_Offset, r->VarL_LowIndex
static void rSetVarL(ring r)
{
  int  min = INT_MAX, min_j = -1;
  int* VarL_Number = (int*) omAlloc0(r->ExpL_Size*sizeof(int));

  int i,j;

  // count how often a var long is occupied by an exponent
  for (i=1; i<=r->N; i++)
  {
    VarL_Number[r->VarOffset[i] & 0xffffff]++;
  }

  // determine how many and min
  for (i=0, j=0; i<r->ExpL_Size; i++)
  {
    if (VarL_Number[i] != 0)
    {
      if (min > VarL_Number[i])
      {
        min = VarL_Number[i];
        min_j = j;
      }
      j++;
    }
  }

  r->VarL_Size = j;
  r->VarL_Offset = (int*) omAlloc(r->VarL_Size*sizeof(int));
  r->VarL_LowIndex = 0;

  // set VarL_Offset
  for (i=0, j=0; i<r->ExpL_Size; i++)
  {
    if (VarL_Number[i] != 0)
    {
      r->VarL_Offset[j] = i;
      if (j > 0 && r->VarL_Offset[j-1] != r->VarL_Offset[j] - 1)
        r->VarL_LowIndex = -1;
      j++;
    }
  }
  if (r->VarL_LowIndex >= 0)
    r->VarL_LowIndex = r->VarL_Offset[0];

  r->MinExpPerLong = min;
  if (min_j != 0)
  {
    j = r->VarL_Offset[min_j];
    r->VarL_Offset[min_j] = r->VarL_Offset[0];
    r->VarL_Offset[0] = j;
  }
  omFree(VarL_Number);
}

static void rRightAdjustVarOffset(ring r)
{
  int* shifts = (int*) omAlloc(r->ExpL_Size*sizeof(int));
  int i;
  // initialize shifts
  for (i=0;i<r->ExpL_Size;i++)
    shifts[i] = BIT_SIZEOF_LONG;

  // find minimal bit in each long var
  for (i=1;i<=r->N;i++)
  {
    if (shifts[r->VarOffset[i] & 0xffffff] > r->VarOffset[i] >> 24)
      shifts[r->VarOffset[i] & 0xffffff] = r->VarOffset[i] >> 24;
  }
  // reset r->VarOffset
  for (i=1;i<=r->N;i++)
  {
    if (shifts[r->VarOffset[i] & 0xffffff] != 0)
      r->VarOffset[i]
        = (r->VarOffset[i] & 0xffffff) |
        (((r->VarOffset[i] >> 24) - shifts[r->VarOffset[i] & 0xffffff]) << 24);
  }
  omFree(shifts);
}

// get r->divmask depending on bits per exponent
static unsigned long rGetDivMask(int bits)
{
  unsigned long divmask = 1;
  int i = bits;

  while (i < BIT_SIZEOF_LONG)
  {
    divmask |= (((unsigned long) 1) << (unsigned long) i);
    i += bits;
  }
  return divmask;
}

#ifdef RDEBUG
void rDebugPrint(ring r)
{
  if (r==NULL)
  {
    PrintS("NULL ?\n");
    return;
  }
  char *TYP[]={"ro_dp","ro_wp","ro_wp_neg","ro_cp",
               "ro_syzcomp", "ro_syz", "ro_none"};
  int i,j;

  Print("ExpL_Size:%d ",r->ExpL_Size);
  Print("CmpL_Size:%d ",r->CmpL_Size);
  Print("VarL_Size:%d\n",r->VarL_Size);
  Print("bitmask=0x%x (expbound=%d) \n",r->bitmask, r->bitmask);
  Print("BitsPerExp=%d ExpPerLong=%d MinExpPerLong=%d at L[%d]\n", r->BitsPerExp, r->ExpPerLong, r->MinExpPerLong, r->VarL_Offset[0]);
  PrintS("varoffset:\n");
  if (r->VarOffset==NULL) PrintS(" NULL\n");
  else
    for(j=0;j<=r->N;j++)
      Print("  v%d at e-pos %d, bit %d\n",
            j,r->VarOffset[j] & 0xffffff, r->VarOffset[j] >>24);
  Print("divmask=%p\n", r->divmask);
  PrintS("ordsgn:\n");
  for(j=0;j<r->CmpL_Size;j++)
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
  Print("pOrdIndex:%d pCompIndex:%d\n", r->pOrdIndex, r->pCompIndex);
  Print("OrdSize:%d\n",r->OrdSize);
  PrintS("--------------------\n");
  for(j=0;j<r->ExpL_Size;j++)
  {
    Print("L[%d]: ",j);
    if (j< r->CmpL_Size)
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

void pDebugPrintR(poly p, ring r)
{
  int i,j;
  pWrite(p);
  j=2;
  while(p!=NULL)
  {
    Print("\nexp[0..%d]\n",r->ExpL_Size-1);
    for(i=0;i<r->ExpL_Size;i++)
      Print("%d ",p->exp[i]);
    PrintLn();
    Print("v0:%d ",p_GetComp(p, r));
    for(i=1;i<=r->N;i++) Print(" v%d:%d",i,p_GetExp(p,i, r));
    PrintLn();
    pIter(p);
    j--;
    if (j==0) { PrintS("...\n"); break; }
  }
}

void pDebugPrint(poly p)
{
  pDebugPrintR(p, currRing);
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
    rChangeCurrRing(r);
    if (old_ring->qideal != NULL)
    {
      r->qideal = idrCopyR_NoSort(old_ring->qideal, old_ring);
      assume(idRankFreeModule(r->qideal) == 0);
      currQuotient = r->qideal;
    }
  }
  return r;
}

static ring rAssure_SyzComp(ring r, BOOLEAN complete)
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
    rChangeCurrRing(new_r);
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
    rChangeCurrRing(new_r);
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
  rChangeCurrRing(res);
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

n_coeffType rFieldType(ring r)
{
  if (rField_is_Zp(r))     return n_Zp;
  if (rField_is_Q(r))      return n_Q;
  if (rField_is_R(r))      return n_R;
  if (rField_is_GF(r))     return n_GF;
  if (rField_is_long_R(r)) return n_long_R;
  if (rField_is_Zp_a(r))   return n_Zp_a;
  if (rField_is_Q_a(r))    return n_Q_a;
  if (rField_is_long_C(r)) return n_long_C;
  return n_unknown;
}

int * rGetWeightVec(ring r)
{
  assume(r!=NULL);
  assume(r->OrdSize>0);
  assume(r->typ[0].ord_typ==ro_wp);
  return (r->typ[0].data.wp.weights);
}

void rSetWeightVec(ring r, int *wv)
{
  assume(r!=NULL);
  assume(r->OrdSize>0);
  assume(r->typ[0].ord_typ==ro_wp);
  memcpy(r->typ[0].data.wp.weights,wv,r->N*sizeof(int));
}

lists rDecompose(ring r)
{
  // 0: char/ cf - ring
  // 1: list (var)
  // 3: list (ord)
  // 4: qideal
  lists L=(lists)omAlloc0Bin(slists_bin);
  L->Init(4);
  // ----------------------------------------
  #if 0 /* TODO */
  if (rIsExtension(r))
    rDecomposeCF(&(L->m[0]),r);
  else
  #endif
  {
    L->m[0].rtyp=INT_CMD;
    L->m[0].data=(void *)r->ch;
  }
  // ----------------------------------------
  lists LL=(lists)omAlloc0Bin(slists_bin);
  L->m[1].rtyp=LIST_CMD;
  L->m[1].data=(void *)LL;
  LL->Init(r->N);
  int i;
  for(i=0; i<r->N; i++)
  {
    LL->m[i].rtyp=STRING_CMD;
    LL->m[i].data=(void *)omStrDup(r->names[i]);
  }
  // ----------------------------------------
  LL=(lists)omAlloc0Bin(slists_bin);
  L->m[2].rtyp=LIST_CMD;
  L->m[2].data=(void *)LL;
  LL->Init((i=rBlocks(r)-1));
  lists LLL;
  for(; i>=0; i--)
  {
    intvec *iv;
    int j;
    LL->m[i].rtyp=LIST_CMD;
    LLL=(lists)omAlloc0Bin(slists_bin);
    LLL->Init(2);
    LLL->m[0].rtyp=STRING_CMD;
    LLL->m[0].data=(void *)omStrDup(rSimpleOrdStr(r->order[i]));
    LLL->m[1].rtyp=INTVEC_CMD;
    j=r->block1[i]-r->block0[i];
    iv=new intvec(j+1);
    LLL->m[1].data=(void *)iv;
    if (r->block1[i]-r->block0[i] >=0 )
    {
      if ((r->wvhdl!=NULL) && (r->wvhdl[i]!=NULL))
      {
        for(;j>=0; j--) (*iv)[j]=r->wvhdl[i][j];
      }
      else switch (r->order[i])
      {
        case ringorder_dp:
        case ringorder_Dp:
        case ringorder_ds:
        case ringorder_Ds:
        case ringorder_lp:
          for(;j>=0; j--) (*iv)[j]=1;
          break;
        default: /* do nothing */;
      }
    }
    LL->m[i].data=(void *)LLL;
  }
  // ----------------------------------------
  L->m[3].rtyp=IDEAL_CMD;
  if (r->qideal==NULL)
    L->m[3].data=(void *)idInit(1,1);
  else
    L->m[3].data=(void *)idCopy(r->qideal);
  return L;
}

ring rCompose(lists  L)
{
  if (L->nr!=3) return NULL;
  // 0: char/ cf - ring
  // 1: list (var)
  // 2: list (ord)
  // 3: qideal
  ring R=(ring) omAlloc0Bin(sip_sring_bin);
  if (L->m[0].Typ()==INT_CMD)
  {
    R->ch=(int)L->m[0].Data();
  }
  else if (L->m[0].Typ()==LIST_CMD)
  {
    R->algring=rCompose((lists)L->m[0].Data());
    if (R->algring==NULL)
    {
      WerrorS("could not create rational function coefficient field");
      goto rCompose_err;
    }
    R->ch=R->algring->ch;
    R->parameter=R->algring->names;
    R->P=R->algring->N;
  }
  else
  {
    WerrorS("coefficient field must be described by `int` or `list`");
    goto rCompose_err;
  }
  // ------------------------- VARS ---------------------------
  if (L->m[1].Typ()==LIST_CMD)
  {
    lists v=(lists)L->m[1].Data();
    R->N = v->nr+1;
    R->names   = (char **)omAlloc0(R->N * sizeof(char_ptr));
    int i;
    for(i=0;i<R->N;i++)
    {
      if (v->m[i].Typ()==STRING_CMD)
        R->names[i]=omStrDup((char *)v->m[i].Data());
      else if (v->m[i].Typ()==POLY_CMD)
      {
        poly p=(poly)v->m[i].Data();
        int nr=pIsPurePower(p);
        if (nr>0)
          R->names[i]=omStrDup(currRing->names[nr-1]);
        else
        {
          Werror("var name %d must be a string or a ring variable",i+1);
          goto rCompose_err;
        }
      }
      else
      {
        Werror("var name %d must be `string`",i+1);
        goto rCompose_err;
      }
    }
  }
  else
  {
    WerrorS("variable must be given as `list`");
    goto rCompose_err;
  }
  // ------------------------ ORDER ------------------------------
  if (L->m[2].Typ()==LIST_CMD)
  {
    lists v=(lists)L->m[2].Data();
    int n= v->nr+2;
    int j;
    // initialize fields of R
    R->order=(int *)omAlloc0(n*sizeof(int));
    R->block0=(int *)omAlloc0(n*sizeof(int));
    R->block1=(int *)omAlloc0(n*sizeof(int));
    R->wvhdl=(int**)omAlloc0(n*sizeof(int_ptr));
    // init order, so that rBlocks works correctly
    for (j=0; j < n-2; j++)
      R->order[j] = (int) ringorder_unspec;
    // orderings
    R->OrdSgn=1;
    for(j=0;j<n-1;j++)
    {
    // todo: a(..), M
      lists vv=(lists)v->m[j].Data(); // assume LIST
      R->order[j]=rOrderName(omStrDup((char*)vv->m[0].Data())); // assume STRING
      if (j==0) R->block0[0]=1;
      else      R->block0[j]=R->block1[j-1]+1;
      intvec *iv=ivCopy((intvec*)vv->m[1].Data()); //assume INTVEC
      R->block1[j]=R->block0[j]+iv->length()-1;
      int i;
      switch (R->order[j])
      {
         case ringorder_ws:
         case ringorder_Ws:
            R->OrdSgn=-1;
         case ringorder_wp:
         case ringorder_Wp:
           R->wvhdl[j] =( int *)omAlloc((iv->length())*sizeof(int));
           for (i=1; i<iv->length();i++) R->wvhdl[n][i-1]=(*iv)[i];
           break;
         case ringorder_ls:
         case ringorder_ds:
         case ringorder_Ds:
           R->OrdSgn=-1;
         case ringorder_lp:
         case ringorder_dp:
         case ringorder_Dp:
         case ringorder_rp:
           break;
         case ringorder_S:
           break;
         case ringorder_c:
         case ringorder_C:
           R->block1[j]=R->block0[j]-1;
           break;
         case ringorder_aa:
         case ringorder_a:
           R->wvhdl[j] =( int *)omAlloc((iv->length())*sizeof(int));
           for (i=1; i<iv->length();i++) R->wvhdl[n][i-1]=(*iv)[i];
         // todo
           break;
         case ringorder_M:
         // todo
           break;
      }
    }
    // sanity check
    j=n-2;
    if ((R->order[j]==ringorder_c)
    || (R->order[j]==ringorder_C)) j--;
    if (R->block1[j] != R->N)
    {
      if (((R->order[j]==ringorder_dp) ||
           (R->order[j]==ringorder_ds) ||
           (R->order[j]==ringorder_Dp) ||
           (R->order[j]==ringorder_Ds) ||
           (R->order[j]==ringorder_rp) ||
           (R->order[j]==ringorder_lp) ||
           (R->order[j]==ringorder_ls))
          &&
            R->block0[j] <= R->N)
      {
        R->block1[j] = R->N;
      }
      else
      {
        Werror("ordering incomplete: size (%d) should be %d",R->block1[j],R->N);
        goto rCompose_err;
      }
    }
  }
  else
  {
    WerrorS("ordering must be given as `list`");
    goto rCompose_err;
  }
  // ------------------------ Q-IDEAL ------------------------
  if (L->m[3].Typ()==IDEAL_CMD)
  {
    ideal q=(ideal)L->m[3].Data();
    if (q->m[0]!=NULL)
      R->qideal=idCopy(q);
  }
  else
  {
    WerrorS("q-ideal must be given as `ideal`");
    goto rCompose_err;
  }

  // todo
  rComplete(R);
  return R;

rCompose_err:
  if (R->N>0)
  {
    int i;
    if (R->names!=NULL)
    {
      i=R->N;
      while (i>=0) { if (R->names[i]!=NULL) omFree(R->names[i]); i--; }
      omFree(R->names);
    }
  }
  if (R->order!=NULL) omFree(R->order);
  if (R->block0!=NULL) omFree(R->block0);
  if (R->block1!=NULL) omFree(R->block1);
  if (R->wvhdl!=NULL) omFree(R->wvhdl);
  omFree(R);
  return NULL;
}
