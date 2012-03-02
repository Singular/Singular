/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/*
* ABSTRACT - the interpreter related ring operations
*/

/* includes */
#include <math.h>
#include <kernel/mod2.h>

#include <kernel/options.h>
#include <omalloc/omalloc.h>
#include <kernel/polys.h>
#include <kernel/numbers.h>
#include <kernel/febase.h>
#include <kernel/intvec.h>
#include <kernel/longtrans.h>
#include <kernel/ffields.h>
#include <kernel/ideals.h>
#include <kernel/ring.h>
#include <kernel/prCopy.h>
#include <Singular/ipshell.h>
#include <kernel/p_Procs.h>
#ifdef HAVE_PLURAL
#include <kernel/gring.h>
#include <kernel/sca.h>
#endif
#include <kernel/maps.h>
#include <kernel/matpol.h>
#ifdef HAVE_FACTORY
#define SI_DONT_HAVE_GLOBAL_VARS
#  include <factory/factory.h>
#endif

#define BITS_PER_LONG 8*SIZEOF_LONG

omBin sip_sring_bin = omGetSpecBin(sizeof(sip_sring));

static const char * const ringorder_name[] =
{
  " ?", ///< ringorder_no = 0,
  "a", ///< ringorder_a,
  "A", ///< ringorder_a64,
  "c", ///< ringorder_c,
  "C", ///< ringorder_C,
  "M", ///< ringorder_M,
  "S", ///< ringorder_S,
  "s", ///< ringorder_s,
  "lp", ///< ringorder_lp,
  "dp", ///< ringorder_dp,
  "rp", ///< ringorder_rp,
  "Dp", ///< ringorder_Dp,
  "wp", ///< ringorder_wp,
  "Wp", ///< ringorder_Wp,
  "ls", ///< ringorder_ls,
  "ds", ///< ringorder_ds,
  "Ds", ///< ringorder_Ds,
  "ws", ///< ringorder_ws,
  "Ws", ///< ringorder_Ws,
  "am",  ///< ringorder_am,
  "L", ///< ringorder_L,
  "aa", ///< ringorder_aa
  "rs", ///< ringorder_rs,
  "IS", ///<  ringorder_IS
  " _" ///< ringorder_unspec
};

const char * rSimpleOrdStr(int ord)
{
  return ringorder_name[ord];
}

/// unconditionally deletes fields in r
void rDelete(ring r);
/// set r->VarL_Size, r->VarL_Offset, r->VarL_LowIndex
static void rSetVarL(ring r);
/// get r->divmask depending on bits per exponent
static unsigned long rGetDivMask(int bits);
/// right-adjust r->VarOffset
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

/// internally changes the gloabl ring and resets the relevant
/// global variables:
void rChangeCurrRing(ring r)
{
 // if ((currRing!=NULL) && (currRing->minpoly!=NULL))
 // {
 //   omCheckAddr(currRing->minpoly);
 // }
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
    /*------------ global variables related to factory -------------------*/
#ifdef HAVE_FACTORY
    //int c=ABS(nGetChar());
    //if (c==1) c=0;
    //setCharacteristic( c );
#endif
  }
}

ring rDefault(int ch, int N, char **n,int ord_size, int *ord, int *block0, int *block1, int** wvhdl)
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
  /*weights: entries for ord_size  blocks: NULL*/
  if (wvhdl==NULL)
    r->wvhdl = (int **)omAlloc0((ord_size+1) * sizeof(int_ptr));
  else
    r->wvhdl=wvhdl;
  r->order = ord;
  r->block0 = block0;
  r->block1 = block1;
  /*polynomial ring*/
  r->OrdSgn    = 1;

  /* complete ring intializations */
  rComplete(r);
  return r;
}

ring rDefault(int ch, int N, char **n)
{
  /*order: lp,0*/
  int *order = (int *) omAlloc(2* sizeof(int));
  int *block0 = (int *)omAlloc0(2 * sizeof(int));
  int *block1 = (int *)omAlloc0(2 * sizeof(int));
  /* ringorder dp for the first block: var 1..N */
  order[0]  = ringorder_lp;
  block0[0] = 1;
  block1[0] = N;
  /* the last block: everything is 0 */
  order[1]  = 0;

  return rDefault(ch,N,n,2,order,block0,block1);
}

///////////////////////////////////////////////////////////////////////////
//
// rInit: define a new ring from sleftv's
//
//-> ipshell.cc

/////////////////////////////
// Auxillary functions
//

// check intvec, describing the ordering
BOOLEAN rCheckIV(intvec *iv)
{
  if ((iv->length()!=2)&&(iv->length()!=3))
  {
    WerrorS("weights only for orderings wp,ws,Wp,Ws,a,M");
    return TRUE;
  }
  return FALSE;
}

int rTypeOfMatrixOrder(intvec * order)
{
  int i=0,j,typ=1;
  int sz = (int)sqrt((double)(order->length()-2));
  if ((sz*sz)!=(order->length()-2))
  {
    WerrorS("Matrix order is not a square matrix");
    typ=0;
  }
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
BOOLEAN rSleftvOrdering2Ordering(sleftv *ord, ring R);

// get array of strings from list of sleftv's
BOOLEAN rSleftvList2StringArray(sleftv* sl, char** p);


/*2
 * set a new ring from the data:
 s: name, chr: ch, varnames: rv, ordering: ord, typ: typ
 */

int r_IsRingVar(const char *n, ring r)
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
#ifdef HAVE_RINGS
  else if (rField_is_Ring(r))
  {
    PrintS("//   coeff. ring is : ");
    if (rField_is_Ring_Z(r)) PrintS("Integers\n");
    long l = (long)mpz_sizeinbase(r->ringflaga, 10) + 2;
    char* s = (char*) omAlloc(l);
    mpz_get_str(s,10,r->ringflaga);
    if (rField_is_Ring_ModN(r)) Print("Z/%s\n", s);
    if (rField_is_Ring_2toM(r)) Print("Z/2^%lu\n", r->ringflagb);
    if (rField_is_Ring_PtoM(r)) Print("Z/%s^%lu\n", s, r->ringflagb);
    omFreeSize((ADDRESS)s, l);
  }
#endif
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
      if (r->minideal!=NULL)
      {
        if (r==currRing) iiWriteMatrix((matrix)r->minideal,"//   minpolys",1,0);
        else PrintS("//   minpolys=...");
        PrintLn();
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


    if (r->order[l] == ringorder_s)
    {
      assume( l == 0 );
#ifndef NDEBUG
      Print("  syzcomp at %d",r->typ[l].data.syz.limit);
#endif
      continue;
    }
    else if (r->order[l] == ringorder_IS)
    {
      assume( r->block0[l] == r->block1[l] );
      const int s = r->block0[l];
      assume( (-2 < s) && (s < 2) );
      Print("(%d)", s); // 0 => prefix! +/-1 => suffix!
      continue;
    }
    else if (
    (  (r->order[l] >= ringorder_lp)
    ||(r->order[l] == ringorder_M)
    ||(r->order[l] == ringorder_a)
    ||(r->order[l] == ringorder_am)
    ||(r->order[l] == ringorder_a64)
    ||(r->order[l] == ringorder_aa) ) && (r->order[l] < ringorder_IS) )
    {
      PrintS("\n//                  : names   ");
      for (i = r->block0[l]-1; i<r->block1[l]; i++)
      {
        nlen = strlen(r->names[i]);
        Print(" %s",r->names[i]);
      }
    }

    if (r->wvhdl[l]!=NULL)
    {
      for (int j= 0;
           j<(r->block1[l]-r->block0[l]+1)*(r->block1[l]-r->block0[l]+1);
           j+=i)
      {
        PrintS("\n//                  : weights ");
        for (i = 0; i<=r->block1[l]-r->block0[l]; i++)
        {
          if (r->order[l] == ringorder_a64)
          {
            int64 *w=(int64 *)r->wvhdl[l];
            #if SIZEOF_LONG == 4
            Print("%*lld " ,nlen,w[i+j]);
            #else
            Print(" %*ld"  ,nlen,w[i+j]);
            #endif
          }
          else
            Print(" %*d" ,nlen,r->wvhdl[l][i+j]);
        }
        if (r->order[l]!=ringorder_M) break;
      }
      if (r->order[l]==ringorder_am) /*j==0*/
      {
        int m=r->wvhdl[l][i];
        Print("\n//                  : %d module weights ",m);
        m+=i;i++;
        for(;i<=m;i++) Print(" %*d" ,nlen,r->wvhdl[l][i]);
      }
    }
  }
#ifdef HAVE_PLURAL
  if(rIsPluralRing(r))
  {
    PrintS("\n//   noncommutative relations:");
    if (r==currRing)
    {
      poly pl=NULL;
      int nl;
      int i,j;
      for (i = 1; i<r->N; i++)
      {
        for (j = i+1; j<=r->N; j++)
        {
          nl = nIsOne(p_GetCoeff(MATELEM(r->GetNC()->C,i,j),r->GetNC()->basering));
          if ( (MATELEM(r->GetNC()->D,i,j)!=NULL) || (!nl) )
          {
            Print("\n//    %s%s=",r->names[j-1],r->names[i-1]);
            pl = MATELEM(r->GetNC()->MT[UPMATELEM(i,j,r->N)],1,1);
            p_Write0(pl, r, r);
          }
        }
      }
    }
    else PrintS(" ...");
#if 0  /*Singularg should not differ from Singular except in error case*/
    Print("\n//   noncommutative type:%d", (int)ncRingType(r));
    Print("\n//      is skew constant:%d",r->GetNC()->IsSkewConstant);
    if( rIsSCA(r) )
    {
      Print("\n//   alternating variables: [%d, %d]", scaFirstAltVar(r), scaLastAltVar(r));
      const ideal Q = SCAQuotient(r); // resides within r!
      PrintS("\n//   quotient of sca by ideal");

      if (Q!=NULL)
      {
        if (r==currRing)
        {
          PrintLn();
          iiWriteMatrix((matrix)Q,"scaQ",1);
        }
        else PrintS(" ...");
      }
      else
        PrintS(" (NULL)");
    }
#endif
  }
#endif
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

void rDelete(ring r)
{
  int i, j;

  if (r == NULL) return;

#ifdef HAVE_PLURAL
  if (rIsPluralRing(r))
    nc_rKill(r);
#endif

  nKillChar(r);
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
    omFreeSize((ADDRESS)r->wvhdl,i*sizeof(int *));
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
#ifdef HAVE_RINGS
  if (r->ringflaga != NULL)
  {
    mpz_clear(r->ringflaga);
    omFree((ADDRESS)r->ringflaga);
  }
  if (r->nrnModul != NULL)
  {
    mpz_clear(r->nrnModul);
    omFree((ADDRESS)r->nrnModul);
  }
#endif
  omFreeBin(r, sip_sring_bin);
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
  if ((r==NULL)||(r->order==NULL)) return omStrDup("");
  int nblocks,l,i;

  for (nblocks=0; r->order[nblocks]; nblocks++);
  nblocks--;

  StringSetS("");
  for (l=0; ; l++)
  {
    StringAppendS((char *)rSimpleOrdStr(r->order[l]));
    if (
           (r->order[l] != ringorder_c)
        && (r->order[l] != ringorder_C)
        && (r->order[l] != ringorder_s)
        && (r->order[l] != ringorder_S)
        && (r->order[l] != ringorder_IS)
       )
    {
      if (r->wvhdl[l]!=NULL)
      {
        StringAppendS("(");
        for (int j= 0;
             j<(r->block1[l]-r->block0[l]+1)*(r->block1[l]-r->block0[l]+1);
             j+=i+1)
        {
          char c=',';
          if(r->order[l]==ringorder_a64)
          {
            int64 * w=(int64 *)r->wvhdl[l];
            for (i = 0; i<r->block1[l]-r->block0[l]; i++)
            {
              StringAppend("%lld," ,w[i]);
            }
            StringAppend("%lld)" ,w[i]);
            break;
          }
          else
          {
            for (i = 0; i<r->block1[l]-r->block0[l]; i++)
            {
              StringAppend("%d," ,r->wvhdl[l][i+j]);
            }
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
    else if (r->order[l] == ringorder_IS)
    {
      assume( r->block0[l] == r->block1[l] );
      const int s = r->block0[l];
      assume( (-2 < s) && (s < 2) );

      StringAppend("(%d)", s);
    }

    if (l==nblocks) return omStrDup(StringAppendS(""));
    StringAppendS(",");
  }
}

char * rVarStr(ring r)
{
  if ((r==NULL)||(r->names==NULL)) return omStrDup("");
  int i;
  int l=2;
  char *s;

  for (i=0; i<r->N; i++)
  {
    l+=strlen(r->names[i])+1;
  }
  s=(char *)omAlloc((long)l);
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

#ifdef HAVE_RINGS
  if (rField_is_Ring_Z(r))
  {
    s=omStrDup("integer");                   /* Z */
    return s;
  }
  if(rField_is_Ring_2toM(r))
  {
    char* s = (char*) omAlloc(7+10+2);
    sprintf(s,"integer,%lu",r->ringflagb);
    return s;
  }
  if(rField_is_Ring_ModN(r))
  {
    long l = (long)mpz_sizeinbase(r->ringflaga, 10) + 2+7;
    char* s = (char*) omAlloc(l);
    gmp_sprintf(s,"integer,%Zd",r->ringflaga);
    return s;
  }
  if(rField_is_Ring_PtoM(r))
  {
    long l = (long)mpz_sizeinbase(r->ringflaga, 10) + 2+7+10;
    char* s = (char*) omAlloc(l);
    gmp_sprintf(s,"integer,%Zd^%lu",r->ringflaga,r->ringflagb);
    return s;
  }
#endif
  if (r->parameter==NULL)
  {
    i=r->ch;
    if(i==-1)
      s=omStrDup("real");                    /* R */
    else
    {
      s=(char *)omAlloc(MAX_INT_LEN+1);
      sprintf(s,"%d",i);                   /* Q, Z/p */
    }
    return s;
  }
  if (rField_is_long_C(r))
  {
    s=(char *)omAlloc(21+strlen(r->parameter[0]));
    sprintf(s,"complex,%d,%s",r->float_len,r->parameter[0]);   /* C */
    return s;
  }
  int l=0;
  for(i=0; i<rPar(r);i++)
  {
    l+=(strlen(r->parameter[i])+1);
  }
  s=(char *)omAlloc((long)(l+MAX_INT_LEN+1));
  s[0]='\0';
  if (r->ch<0)       sprintf(s,"%d",-r->ch); /* Fp(a) */
  else if (r->ch==1) sprintf(s,"0");         /* Q(a)  */
  else
  {
    sprintf(s,"%d,%s",r->ch,r->parameter[0]); /* GF(q)  */
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
  if ((r==NULL)||(r->parameter==NULL)) return omStrDup("");

  int i;
  int l=2;

  for (i=0; i<rPar(r); i++)
  {
    l+=strlen(r->parameter[i])+1;
  }
  char *s=(char *)omAlloc((long)l);
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

int  rIsExtension(const ring r)
{
  return (r->parameter!=NULL); /* R, Q, Fp: FALSE */
}

static int binaryPower (const int a, const int b)
{
  /* computes a^b according to the binary representation of b,
     i.e., a^7 = a^4 * a^2 * a^1. This saves some multiplications. */
  int result = 1;
  int factor = a;
  int bb = b;
  while (bb != 0)
  {
    if (bb % 2 != 0) result = result * factor;
    bb = bb / 2;
    factor = factor * factor;
  }
  return result;
}

int rChar(ring r)
{
#ifdef HAVE_RINGS
  if (rField_is_Ring_2toM(r))
    return binaryPower(2, (int)(unsigned long)r->ringflagb);
  if (rField_is_Ring_ModN(r))
    return (int)mpz_get_ui(r->ringflaga);
  if (rField_is_Ring_PtoM(r))
    return binaryPower((int)mpz_get_ui(r->ringflaga),
                       (int)(unsigned long)r->ringflagb);
#endif
  if (rField_is_numeric(r))
    return 0;
  if (!rIsExtension(r)) /* Q, Fp */
    return r->ch;
  if (rField_is_Zp_a(r))  /* Fp(a)  */
    return -r->ch;
  if (rField_is_Q_a(r))   /* Q(a)  */
    return 0;
  /*else*/               /* GF(p,n) */
  {
    if ((r->ch & 1)==0) return 2;
    int i=3;
    while ((r->ch % i)!=0) i+=2;
    return i;
  }
}

/*2
 *returns -1 for not compatible, (sum is undefined)
 *         1 for compatible (and sum)
 */
/* vartest: test for variable/paramter names
* dp_dp: for comm. rings: use block order dp + dp/ds/wp
*/
int rSumInternal(ring r1, ring r2, ring &sum, BOOLEAN vartest, BOOLEAN dp_dp)
{
  ring save=currRing;
  ip_sring tmpR;
  memset(&tmpR,0,sizeof(tmpR));
  /* check coeff. field =====================================================*/
  if ((rFieldType(r1)==rFieldType(r2))
  && (rInternalChar(r1)==rInternalChar(r2)))
  {
    tmpR.ch=rInternalChar(r1);
    if (rField_is_Q(r1)||rField_is_Zp(r1)||rField_is_GF(r1)) /*Q, Z/p, GF(p,n)*/
    {
      if (r1->parameter!=NULL)
      {
        if (!vartest || (strcmp(r1->parameter[0],r2->parameter[0])==0)) /* 1 par */
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
    else if (rField_is_Extension(r1)) /* Q(a),Z/p(a) */
    {
      if (r1->minpoly!=NULL)
      {
        if (r2->minpoly!=NULL)
        {
          // HANNES: TODO: delete nSetChar
          rChangeCurrRing(r1);
          if ((!vartest || (strcmp(r1->parameter[0],r2->parameter[0])==0)) /* 1 par */
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
          if ((!vartest || (strcmp(r1->parameter[0],r2->parameter[0])==0)) /* 1 par */
              && (rPar(r2)==1))
          {
            tmpR.parameter=(char **)omAllocBin(char_ptr_bin);
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
          if ((!vartest || (strcmp(r1->parameter[0],r2->parameter[0])==0)) /* 1 par */
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
          tmpR.parameter=(char **)omAlloc0(len*sizeof(char_ptr));
          int i;
          for (i=0;i<rPar(r1);i++)
          {
            tmpR.parameter[i]=omStrDup(r1->parameter[i]);
          }
          int j,l;
          for(j=0;j<rPar(r2);j++)
          {
            if (vartest)
            {
              for(l=0;l<i;l++)
              {
                if(strcmp(tmpR.parameter[l],r2->parameter[j])==0)
                  break;
              }
            }
            else
              l=i;
            if (l==i)
            {
              tmpR.parameter[i]=omStrDup(r2->parameter[j]);
              i++;
            }
          }
          if (i!=len)
          {
            tmpR.parameter=(char**)omReallocSize(tmpR.parameter,
                                                 len*sizeof(char_ptr),
                                                 i*sizeof(char_ptr));
          }
          tmpR.P=i;
        }
      }
    }
    #ifdef HAVE_RINGS
    else if (rField_is_Ring(r1)||rField_is_Ring(r2))
    {
      if (r1->ringtype != r2->ringtype)
      {
        Werror("rSumInternal not yet implemented for %s",
               "different coefficient rings");
        return -1;
      }
      else
      {
        tmpR.ch        = rInternalChar(r1);
        tmpR.ringtype  = r1->ringtype;
        if (r1->ringflaga != NULL)
        {
          omBin tmpBin = omGetSpecBin(sizeof(mpz_t));
          tmpR.ringflaga = (int_number)omAllocBin(tmpBin);
          mpz_init_set(tmpR.ringflaga, (int_number)r1->ringflaga);
        }
        tmpR.ringflagb = r1->ringflagb;
        tmpR.nr2mModul = r1->nr2mModul;
        if (r1->nrnModul != NULL)
        {
          omBin tmpBin = omGetSpecBin(sizeof(mpz_t));
          tmpR.nrnModul = (int_number)omAllocBin(tmpBin);
          mpz_init_set(tmpR.nrnModul, (int_number)r1->nrnModul);
        }
      }
    }
    #endif
  }
  else /* r1->ch!=r2->ch */
  {
    if (r1->ch<-1) /* Z/p(a) */
    {
      if ((r2->ch==0) /* Q */
          || (r2->ch==-r1->ch)) /* Z/p */
      {
        tmpR.ch=rInternalChar(r1);
        tmpR.P=rPar(r1);
        tmpR.parameter=(char **)omAlloc(rPar(r1)*sizeof(char_ptr));
        int i;
        for (i=0;i<rPar(r1);i++)
        {
          tmpR.parameter[i]=omStrDup(r1->parameter[i]);
        }
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
        int i;
        for (i=0;i<rPar(r2);i++)
        {
          tmpR.parameter[i]=omStrDup(r2->parameter[i]);
        }
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
        tmpR.parameter=(char **)omAlloc(rPar(r1)*sizeof(char_ptr));
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
      if (vartest)
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
      if (vartest)
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
    }

    if (b)
    {
      if (vartest)
      {
        for(j=0;j<r1->N;j++)
        {
          if (strcmp(r1->names[j],r2->names[i])==0)
          {
            b=FALSE;
            break;
          }
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
  if (dp_dp
#ifdef HAVE_PLURAL
      && !rIsPluralRing(r1) && !rIsPluralRing(r2)
#endif
     )
  {
    tmpR.order=(int*)omAlloc(4*sizeof(int));
    tmpR.block0=(int*)omAlloc0(4*sizeof(int));
    tmpR.block1=(int*)omAlloc0(4*sizeof(int));
    tmpR.wvhdl=(int**)omAlloc0(4*sizeof(int_ptr));
    tmpR.order[0]=ringorder_dp;
    tmpR.block0[0]=1;
    tmpR.block1[0]=rVar(r1);
    if (r2->OrdSgn==1)
    {
      if ((r2->block0[0]==1)
      && (r2->block1[0]==rVar(r2))
      && ((r2->order[0]==ringorder_wp)
        || (r2->order[0]==ringorder_Wp)
        || (r2->order[0]==ringorder_Dp))
     )
     {
       tmpR.order[1]=r2->order[0];
       if (r2->wvhdl[0]!=NULL)
         tmpR.wvhdl[1]=(int *)omMemDup(r2->wvhdl[0]);
     }
     else
        tmpR.order[1]=ringorder_dp;
    }
    else
    {
      tmpR.order[1]=ringorder_ds;
      tmpR.OrdSgn=-1;
    }
    tmpR.block0[1]=rVar(r1)+1;
    tmpR.block1[1]=rVar(r1)+rVar(r2);
    tmpR.order[2]=ringorder_C;
    tmpR.order[3]=0;
  }
  else
  {
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
            tmpR.block0[j]=r2->block0[i]+rVar(r1);
            tmpR.block1[j]=r2->block1[i]+rVar(r1);
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
    else if ((k==rVar(r1)) && (k==rVar(r2))) /* r1 and r2 are "quite"
                                                the same ring */
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
      Werror("difficulties with variables: %d,%d -> %d",rVar(r1),rVar(r2),k);
      return -1;
    }
  }
  sum=(ring)omAllocBin(sip_sring_bin);
  memcpy(sum,&tmpR,sizeof(ip_sring));
  rComplete(sum);

//#ifdef RDEBUG
//  rDebugPrint(sum);
//#endif

#ifdef HAVE_PLURAL
  if(1)
  {
    ring old_ring = currRing;

    BOOLEAN R1_is_nc = rIsPluralRing(r1);
    BOOLEAN R2_is_nc = rIsPluralRing(r2);

    if ( (R1_is_nc) || (R2_is_nc))
    {
      rChangeCurrRing(r1); /* since rCopy works well only in currRing */
      ring R1 = rCopy(r1);
      if ( !R1_is_nc ) nc_rCreateNCcomm(R1);

#if 0
#ifdef RDEBUG
      rWrite(R1);
      rDebugPrint(R1);
#endif
#endif
      rChangeCurrRing(r2);
      ring R2 = rCopy(r2);
      if ( !R2_is_nc ) nc_rCreateNCcomm(R2);

#if 0
#ifdef RDEBUG
      rWrite(R2);
      rDebugPrint(R2);
#endif
#endif

      rChangeCurrRing(sum); // ?

      // Projections from R_i into Sum:
      /* multiplication matrices business: */
      /* find permutations of vars and pars */
      int *perm1 = (int *)omAlloc0((rVar(R1)+1)*sizeof(int));
      int *par_perm1 = NULL;
      if (rPar(R1)!=0) par_perm1=(int *)omAlloc0((rPar(R1)+1)*sizeof(int));

      int *perm2 = (int *)omAlloc0((rVar(R2)+1)*sizeof(int));
      int *par_perm2 = NULL;
      if (rPar(R2)!=0) par_perm2=(int *)omAlloc0((rPar(R2)+1)*sizeof(int));

      maFindPerm(R1->names,  rVar(R1),  R1->parameter,  rPar(R1),
                 sum->names, rVar(sum), sum->parameter, rPar(sum),
                 perm1, par_perm1, sum->ch);

      maFindPerm(R2->names,  rVar(R2),  R2->parameter,  rPar(R2),
                 sum->names, rVar(sum), sum->parameter, rPar(sum),
                 perm2, par_perm2, sum->ch);


      matrix C1 = R1->GetNC()->C, C2 = R2->GetNC()->C;
      matrix D1 = R1->GetNC()->D, D2 = R2->GetNC()->D;

      // !!!! BUG? C1 and C2 might live in different baserings!!!
      // it cannot be both the currRing! :)
      // the currRing is sum!

      int l = rVar(R1) + rVar(R2);

      matrix C  = mpNew(l,l);
      matrix D  = mpNew(l,l);

      for (i = 1; i <= rVar(R1); i++)
        for (j= rVar(R1)+1; j <= l; j++)
          MATELEM(C,i,j) = p_One( sum); // in 'sum'

      idTest((ideal)C);

      nMapFunc nMap1 = nSetMap(R1); /* can change something global: not usable
                                       after the next nSetMap call :( */
      // Create blocked C and D matrices:
      for (i=1; i<= rVar(R1); i++)
        for (j=i+1; j<=rVar(R1); j++)
        {
          assume(MATELEM(C1,i,j) != NULL);
          MATELEM(C,i,j) = pPermPoly(MATELEM(C1,i,j), perm1, R1, nMap1, par_perm1, rPar(R1)); // need ADD + CMP ops.

          if (MATELEM(D1,i,j) != NULL)
            MATELEM(D,i,j) = pPermPoly(MATELEM(D1,i,j),perm1,R1,nMap1,par_perm1,rPar(R1));
        }

      idTest((ideal)C);
      idTest((ideal)D);


      nMapFunc nMap2 = nSetMap(R2); /* can change something global: not usable
                                       after the next nSetMap call :( */
      for (i=1; i<= rVar(R2); i++)
        for (j=i+1; j<=rVar(R2); j++)
        {
          assume(MATELEM(C2,i,j) != NULL);
          MATELEM(C,rVar(R1)+i,rVar(R1)+j) = pPermPoly(MATELEM(C2,i,j),perm2,R2,nMap2,par_perm2,rPar(R2));

          if (MATELEM(D2,i,j) != NULL)
            MATELEM(D,rVar(R1)+i,rVar(R1)+j) = pPermPoly(MATELEM(D2,i,j),perm2,R2,nMap2,par_perm2,rPar(R2));
        }

      idTest((ideal)C);
      idTest((ideal)D);

      // Now sum is non-commutative with blocked structure constants!
      if (nc_CallPlural(C, D, NULL, NULL, sum, false, false, true, sum))
        WarnS("Error initializing non-commutative multiplication!");

      /* delete R1, R2*/

#if 0
#ifdef RDEBUG
      rWrite(sum);
      rDebugPrint(sum);

      Print("\nRefs: R1: %d, R2: %d\n", R1->GetNC()->ref, R2->GetNC()->ref);

#endif
#endif


      rDelete(R1);
      rDelete(R2);

      /* delete perm arrays */
      if (perm1!=NULL) omFree((ADDRESS)perm1);
      if (perm2!=NULL) omFree((ADDRESS)perm2);
      if (par_perm1!=NULL) omFree((ADDRESS)par_perm1);
      if (par_perm2!=NULL) omFree((ADDRESS)par_perm2);

      rChangeCurrRing(old_ring);
    }

  }
#endif

  ideal Q=NULL;
  ideal Q1=NULL, Q2=NULL;
  ring old_ring2 = currRing;
  if (r1->qideal!=NULL)
  {
    rChangeCurrRing(sum);
//     if (r2->qideal!=NULL)
//     {
//       WerrorS("todo: qring+qring");
//       return -1;
//     }
//     else
//     {}
    /* these were defined in the Plural Part above... */
    int *perm1 = (int *)omAlloc0((rVar(r1)+1)*sizeof(int));
    int *par_perm1 = NULL;
    if (rPar(r1)!=0) par_perm1=(int *)omAlloc0((rPar(r1)+1)*sizeof(int));
    maFindPerm(r1->names,  rVar(r1),  r1->parameter,  rPar(r1),
               sum->names, rVar(sum), sum->parameter, rPar(sum),
               perm1, par_perm1, sum->ch);
    nMapFunc nMap1 = nSetMap(r1);
    Q1 = idInit(IDELEMS(r1->qideal),1);
    for (int for_i=0;for_i<IDELEMS(r1->qideal);for_i++)
      Q1->m[for_i] = pPermPoly(r1->qideal->m[for_i],perm1,r1,nMap1,par_perm1,rPar(r1));
    omFree((ADDRESS)perm1);
  }

  if (r2->qideal!=NULL)
  {
    if (currRing!=sum)
      rChangeCurrRing(sum);
    int *perm2 = (int *)omAlloc0((rVar(r2)+1)*sizeof(int));
    int *par_perm2 = NULL;
    if (rPar(r2)!=0) par_perm2=(int *)omAlloc0((rPar(r2)+1)*sizeof(int));
    maFindPerm(r2->names,  rVar(r2),  r2->parameter,  rPar(r2),
               sum->names, rVar(sum), sum->parameter, rPar(sum),
               perm2, par_perm2, sum->ch);
    nMapFunc nMap2 = nSetMap(r2);
    Q2 = idInit(IDELEMS(r2->qideal),1);
    for (int for_i=0;for_i<IDELEMS(r2->qideal);for_i++)
      Q2->m[for_i] = pPermPoly(r2->qideal->m[for_i],perm2,r2,nMap2,par_perm2,rPar(r2));
    omFree((ADDRESS)perm2);
  }
  if ( (Q1!=NULL) || ( Q2!=NULL))
  {
    Q = idSimpleAdd(Q1,Q2);
    rChangeCurrRing(old_ring2);
  }
  sum->qideal = Q;

#ifdef HAVE_PLURAL
  if( rIsPluralRing(sum) )
    nc_SetupQuotient( sum );
#endif
  return 1;
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
  return rSumInternal(r1,r2,sum,TRUE,FALSE);
}

/*2
 * create a copy of the ring r, which must be equivalent to currRing
 * used for qring definition,..
 * (i.e.: normal rings: same nCopy as currRing;
 *        qring:        same nCopy, same idCopy as currRing)
 * DOES NOT CALL rComplete
 */
ring rCopy0(const ring r, BOOLEAN copy_qideal, BOOLEAN copy_ordering)
{
  if (r == NULL) return NULL;
  int i,j;
  ring res=(ring)omAllocBin(sip_sring_bin);
  memset(res,0,sizeof(ip_sring));
  //memcpy(res,r,sizeof(ip_sring));
  //memset: res->idroot=NULL; /* local objects */
  //ideal      minideal;
  res->options=r->options; /* ring dependent options */

  //memset: res->ordsgn=NULL;
  //memset: res->typ=NULL;
  //memset: res->VarOffset=NULL;
  //memset: res->firstwv=NULL;

  //struct omBin   PolyBin; /* Bin from where monoms are allocated */
  //memset: res->PolyBin=NULL; // rComplete
  res->ch=r->ch;     /* characteristic */
#ifdef HAVE_RINGS
  res->ringtype=r->ringtype;  /* cring = 0 => coefficient field, cring = 1 => coeffs from Z/2^m */
  if (r->ringflaga!=NULL)
  {
    res->ringflaga = (int_number) omAlloc(sizeof(mpz_t));
    mpz_init_set(res->ringflaga,r->ringflaga);
  }
  res->ringflagb=r->ringflagb;
  if (r->nrnModul!=NULL)
  {
    res->nrnModul = (int_number) omAlloc(sizeof(mpz_t));
    mpz_init_set(res->nrnModul,r->nrnModul);
  }
#endif
  //memset: res->ref=0; /* reference counter to the ring */

  res->float_len=r->float_len; /* additional char-flags */
  res->float_len2=r->float_len2; /* additional char-flags */

  res->N=r->N;      /* number of vars */
  res->P=r->P;      /* number of pars */
  res->OrdSgn=r->OrdSgn; /* 1 for polynomial rings, -1 otherwise */

  res->firstBlockEnds=r->firstBlockEnds;
#ifdef HAVE_PLURAL
  res->real_var_start=r->real_var_start;
  res->real_var_end=r->real_var_end;
#endif

#ifdef HAVE_SHIFTBBA
  res->isLPring=r->isLPring; /* 0 for non-letterplace rings, otherwise the number of LP blocks, at least 1, known also as lV */
#endif

  res->VectorOut=r->VectorOut;
  res->ShortOut=r->ShortOut;
  res->CanShortOut=r->CanShortOut;
  res->LexOrder=r->LexOrder; // TRUE if the monomial ordering has polynomial and power series blocks
  res->MixedOrder=r->MixedOrder; // ?? 1 for lex ordering (except ls), -1 otherwise
  res->ComponentOrder=r->ComponentOrder;

  //memset: res->ExpL_Size=0;
  //memset: res->CmpL_Size=0;
  //memset: res->VarL_Size=0;
  //memset: res->pCompIndex=0;
  //memset: res->pOrdIndex=0;
  //memset: res->OrdSize=0;
  //memset: res->VarL_LowIndex=0;
  //memset: res->MinExpPerLong=0;
  //memset: res->NegWeightL_Size=0;
  //memset: res->NegWeightL_Offset=NULL;
  //memset: res->VarL_Offset=NULL;

  // the following are set by rComplete unless predefined
  // therefore, we copy these values: maybe they are non-standard
  /* mask for getting single exponents */
  res->bitmask=r->bitmask;
  res->divmask=r->divmask;
  res->BitsPerExp = r->BitsPerExp;
  res->ExpPerLong =  r->ExpPerLong;

  //memset: res->p_Procs=NULL;
  //memset: res->pFDeg=NULL;
  //memset: res->pLDeg=NULL;
  //memset: res->pFDegOrig=NULL;
  //memset: res->pLDegOrig=NULL;
  //memset: res->p_Setm=NULL;
  //memset: res->cf=NULL;
  res->options=r->options;
  #ifdef HAVE_RINGS
  res->ringtype=r->ringtype;
  #endif
  //
  if (r->algring!=NULL)
    r->algring->ref++;
  res->algring=r->algring;
  //memset: res->minideal=NULL;
  if (r->parameter!=NULL)
  {
    if (r->minpoly!=NULL) res->minpoly=nCopy(r->minpoly);
    int l=rPar(r);
    res->parameter=(char **)omAlloc(l*sizeof(char_ptr));
    int i;
    for(i=0;i<rPar(r);i++)
    {
      res->parameter[i]=omStrDup(r->parameter[i]);
    }
    if (r->minideal!=NULL)
    {
      res->minideal=id_Copy(r->minideal,r->algring);
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
    memcpy(res->order,r->order,i * sizeof(int));
    memcpy(res->block0,r->block0,i * sizeof(int));
    memcpy(res->block1,r->block1,i * sizeof(int));
  }
  //memset: else
  //memset: {
  //memset:   res->wvhdl = NULL;
  //memset:   res->order = NULL;
  //memset:   res->block0 = NULL;
  //memset:   res->block1 = NULL;
  //memset: }

  res->names   = (char **)omAlloc0(rVar(r) * sizeof(char_ptr));
  for (i=0; i<rVar(res); i++)
  {
    res->names[i] = omStrDup(r->names[i]);
  }
  if (r->qideal!=NULL)
  {
    if (copy_qideal)
    {
      #ifndef NDEBUG
      if (!copy_ordering)
        WerrorS("internal error: rCopy0(Q,TRUE,FALSE)");
      else
      #endif
      {
      #ifndef NDEBUG
        WarnS("internal bad stuff: rCopy0(Q,TRUE,TRUE)");
      #endif
        rComplete(res);
        res->qideal= idrCopyR_NoSort(r->qideal, r, res);
        rUnComplete(res);
      }
    }
    //memset: else res->qideal = NULL;
  }
  //memset: else res->qideal = NULL;
  //memset: res->GetNC() = NULL; // copy is purely commutative!!!
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
  ring res=rCopy0(r,FALSE,TRUE);
  rComplete(res, 1); // res is purely commutative so far
  if (r->qideal!=NULL) res->qideal=idrCopyR_NoSort(r->qideal, r, res);

#ifdef HAVE_PLURAL
  if (rIsPluralRing(r))
    if( nc_rCopy(res, r, true) );
#endif

  return res;
}

// returns TRUE, if r1 equals r2 FALSE, otherwise Equality is
// determined componentwise, if qr == 1, then qrideal equality is
// tested, as well
BOOLEAN rEqual(ring r1, ring r2, BOOLEAN qr)
{
  int i, j;

  if (r1 == r2) return TRUE;

  if (r1 == NULL || r2 == NULL) return FALSE;

  if ((rInternalChar(r1) != rInternalChar(r2))
  || (r1->float_len != r2->float_len)
  || (r1->float_len2 != r2->float_len2)
  || (rVar(r1) != rVar(r2))
  || (r1->OrdSgn != r2->OrdSgn)
  || (rPar(r1) != rPar(r2)))
    return FALSE;

  for (i=0; i<rVar(r1); i++)
  {
    if (r1->names[i] != NULL && r2->names[i] != NULL)
    {
      if (strcmp(r1->names[i], r2->names[i])) return FALSE;
    }
    else if ((r1->names[i] != NULL) ^ (r2->names[i] != NULL))
    {
      return FALSE;
    }
  }

  i=0;
  while (r1->order[i] != 0)
  {
    if (r2->order[i] == 0) return FALSE;
    if ((r1->order[i] != r2->order[i])
    || (r1->block0[i] != r2->block0[i])
    || (r1->block1[i] != r2->block1[i]))
      return FALSE;
    if (r1->wvhdl[i] != NULL)
    {
      if (r2->wvhdl[i] == NULL)
        return FALSE;
      for (j=0; j<r1->block1[i]-r1->block0[i]+1; j++)
        if (r2->wvhdl[i][j] != r1->wvhdl[i][j])
          return FALSE;
    }
    else if (r2->wvhdl[i] != NULL) return FALSE;
    i++;
  }
  if (r2->order[i] != 0) return FALSE;

  for (i=0; i<rPar(r1);i++)
  {
      if (strcmp(r1->parameter[i], r2->parameter[i])!=0)
        return FALSE;
  }

  if (r1->minpoly != NULL)
  {
    if (r2->minpoly == NULL) return FALSE;
    if (currRing == r1 || currRing == r2)
    {
      if (! nEqual(r1->minpoly, r2->minpoly)) return FALSE;
    }
  }
  else if (r2->minpoly != NULL) return FALSE;

  if (qr)
  {
    if (r1->qideal != NULL)
    {
      ideal id1 = r1->qideal, id2 = r2->qideal;
      int i, n;
      poly *m1, *m2;

      if (id2 == NULL) return FALSE;
      if ((n = IDELEMS(id1)) != IDELEMS(id2)) return FALSE;

      if (currRing == r1 || currRing == r2)
      {
        m1 = id1->m;
        m2 = id2->m;
        for (i=0; i<n; i++)
          if (! pEqualPolys(m1[i],m2[i])) return FALSE;
      }
    }
    else if (r2->qideal != NULL) return FALSE;
  }

  return TRUE;
}

// returns TRUE, if r1 and r2 represents the monomials in the same way
// FALSE, otherwise
// this is an analogue to rEqual but not so strict
BOOLEAN rSamePolyRep(ring r1, ring r2)
{
  int i, j;

  if (r1 == r2) return TRUE;

  if (r1 == NULL || r2 == NULL) return FALSE;

  if ((rInternalChar(r1) != rInternalChar(r2))
  || (r1->float_len != r2->float_len)
  || (r1->float_len2 != r2->float_len2)
  || (rVar(r1) != rVar(r2))
  || (r1->OrdSgn != r2->OrdSgn)
  || (rPar(r1) != rPar(r2)))
    return FALSE;

  if (rVar(r1)!=rVar(r2)) return FALSE;
  if (rPar(r1)!=rPar(r2)) return FALSE;

  i=0;
  while (r1->order[i] != 0)
  {
    if (r2->order[i] == 0) return FALSE;
    if ((r1->order[i] != r2->order[i])
    || (r1->block0[i] != r2->block0[i])
    || (r1->block1[i] != r2->block1[i]))
      return FALSE;
    if (r1->wvhdl[i] != NULL)
    {
      if (r2->wvhdl[i] == NULL)
        return FALSE;
      for (j=0; j<r1->block1[i]-r1->block0[i]+1; j++)
        if (r2->wvhdl[i][j] != r1->wvhdl[i][j])
          return FALSE;
    }
    else if (r2->wvhdl[i] != NULL) return FALSE;
    i++;
  }
  if (r2->order[i] != 0) return FALSE;

  // we do not check minpoly
  // we do not check qideal

  return TRUE;
}

rOrderType_t rGetOrderType(ring r)
{
  // check for simple ordering
  if (rHasSimpleOrder(r))
  {
    if ((r->order[1] == ringorder_c)
    || (r->order[1] == ringorder_C))
    {
      switch(r->order[0])
      {
          case ringorder_dp:
          case ringorder_wp:
          case ringorder_ds:
          case ringorder_ws:
          case ringorder_ls:
          case ringorder_unspec:
            if (r->order[1] == ringorder_C
            ||  r->order[0] == ringorder_unspec)
              return rOrderType_ExpComp;
            return rOrderType_Exp;

          default:
            assume(r->order[0] == ringorder_lp ||
                   r->order[0] == ringorder_rs ||
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

BOOLEAN rHasSimpleOrder(const ring r)
{
  if (r->order[0] == ringorder_unspec) return TRUE;
  int blocks = rBlocks(r) - 1;
  assume(blocks >= 1);
  if (blocks == 1) return TRUE;

  int s = 0;
  while( (s < blocks) && (r->order[s] == ringorder_IS) && (r->order[blocks-1] == ringorder_IS) )
  {
    s++;
    blocks--;
  }

  if ((blocks - s) > 2)  return FALSE;

  assume( blocks == s + 2 );

  if (
     (r->order[s] != ringorder_c)
  && (r->order[s] != ringorder_C)
  && (r->order[s+1] != ringorder_c)
  && (r->order[s+1] != ringorder_C)
     )
    return FALSE;
  if ((r->order[s+1] == ringorder_M)
  || (r->order[s] == ringorder_M))
    return FALSE;
  return TRUE;
}

// returns TRUE, if simple lp or ls ordering
BOOLEAN rHasSimpleLexOrder(const ring r)
{
  return rHasSimpleOrder(r) &&
    (r->order[0] == ringorder_ls ||
     r->order[0] == ringorder_lp ||
     r->order[1] == ringorder_ls ||
     r->order[1] == ringorder_lp);
}

BOOLEAN rOrder_is_DegOrdering(const rRingOrder_t order)
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
  if (r->order[0] == ringorder_unspec) return TRUE;
  int blocks = rBlocks(r) - 1;
  assume(blocks >= 1);
  if (blocks == 1) return TRUE;

  int s = 0;
  while( (s < blocks) && (r->order[s] == ringorder_IS) && (r->order[blocks-1] == ringorder_IS) )
  {
    s++;
    blocks--;
  }

  if ((blocks - s) > 3)  return FALSE;

//  if ((blocks > 3) || (blocks < 2)) return FALSE;
  if ((blocks - s) == 3)
  {
    return (((r->order[s] == ringorder_aa) && (r->order[s+1] != ringorder_M) &&
             ((r->order[s+2] == ringorder_c) || (r->order[s+2] == ringorder_C))) ||
            (((r->order[s] == ringorder_c) || (r->order[s] == ringorder_C)) &&
             (r->order[s+1] == ringorder_aa) && (r->order[s+2] != ringorder_M)));
  }
  else
  {
    return ((r->order[s] == ringorder_aa) && (r->order[s+1] != ringorder_M));
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
      if ((o->ord_typ == ro_syzcomp) || (o->ord_typ == ro_syz) || (o->ord_typ == ro_is) || (o->ord_typ == ro_isTemp)) return TRUE;
    }
  }
  return FALSE;
}

// return TRUE if p->exp[r->pOrdIndex] holds total degree of p */
BOOLEAN rOrd_is_Totaldegree_Ordering(ring r)
{
  // Hmm.... what about Syz orderings?
  return (rVar(r) > 1 &&
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
  return ((rVar(r) > 1) &&
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
        case ringorder_a64: /* assume: all weight are non-negative!*/
        case ringorder_lp:
        case ringorder_rs:
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
BOOLEAN rDBTest(ring r, const char* fn, const int l)
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
  if (r->wvhdl!=NULL)
  {
    omCheckAddrSize(r->wvhdl,i*sizeof(int *));
    for (j=0;j<i; j++)
    {
      if (r->wvhdl[j] != NULL) omCheckAddr(r->wvhdl[j]);
    }
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
  for(i=0;i<=r->N;i++) // for all variables (i = 0..N)
  {
    if(r->typ!=NULL)
    {
      for(j=0;j<r->OrdSize;j++) // for all ordering blocks (j =0..OrdSize-1)
      {
        if(r->typ[j].ord_typ == ro_isTemp)
        {
          const int p = r->typ[j].data.isTemp.suffixpos;

          if(p <= j)
            dReportError("ordrec prefix %d is unmatched",j);

          assume( p < r->OrdSize );

          if(r->typ[p].ord_typ != ro_is)
            dReportError("ordrec prefix %d is unmatched (suffix: %d is wrong!!!)",j, p);

          // Skip all intermediate blocks for undone variables:
          if(r->typ[j].data.isTemp.pVarOffset[i] != -1) // Check i^th variable
          {
            j = p - 1; // SKIP ALL INTERNAL BLOCKS...???
            continue; // To make for check OrdSize bound...
          }
        }
        else if (r->typ[j].ord_typ == ro_is)
        {
          // Skip all intermediate blocks for undone variables:
          if(r->typ[j].data.is.pVarOffset[i] != -1)
          {
            // ???
          }

        }
        else
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
  if (r->minpoly!=NULL)
  {
    omCheckAddr(r->minpoly);
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
  while((start<end) && (weights[0]==0)) { start++; weights++; }
  while((start<end) && (weights[end-start]==0)) { end--; }
  int i;
  int pure_tdeg=1;
  for(i=start;i<=end;i++)
  {
    if(weights[i-start]!=1)
    {
      pure_tdeg=0;
      break;
    }
  }
  if (pure_tdeg)
  {
    rO_TDegree(place,bitplace,start,end,o,ord_struct);
    return;
  }
  rO_Align(place,bitplace);
  ord_struct.ord_typ=ro_wp;
  ord_struct.data.wp.start=start;
  ord_struct.data.wp.end=end;
  ord_struct.data.wp.place=place;
  ord_struct.data.wp.weights=weights;
  o[place]=1;
  place++;
  rO_Align(place,bitplace);
  for(i=start;i<=end;i++)
  {
    if(weights[i-start]<0)
    {
      ord_struct.ord_typ=ro_wp_neg;
      break;
    }
  }
}

static void rO_WMDegree(int &place, int &bitplace, int start, int end,
    long *o, sro_ord &ord_struct, int *weights)
{
  // weighted degree (aligned) of variables v_start..v_end, ordsgn 1
  while((start<end) && (weights[0]==0)) { start++; weights++; }
  while((start<end) && (weights[end-start]==0)) { end--; }
  rO_Align(place,bitplace);
  ord_struct.ord_typ=ro_am;
  ord_struct.data.am.start=start;
  ord_struct.data.am.end=end;
  ord_struct.data.am.place=place;
  ord_struct.data.am.len_gen=weights[end-start+1];
  ord_struct.data.am.weights=weights;
  o[place]=1;
  place++;
  rO_Align(place,bitplace);
}

static void rO_WDegree64(int &place, int &bitplace, int start, int end,
    long *o, sro_ord &ord_struct, int64 *weights)
{
  // weighted degree (aligned) of variables v_start..v_end, ordsgn 1,
  // reserved 2 places
  rO_Align(place,bitplace);
  ord_struct.ord_typ=ro_wp64;
  ord_struct.data.wp64.start=start;
  ord_struct.data.wp64.end=end;
  ord_struct.data.wp64.place=place;
  ord_struct.data.wp64.weights64=weights;
  o[place]=1;
  place++;
  o[place]=1;
  place++;
  rO_Align(place,bitplace);
}

static void rO_WDegree_neg(int &place, int &bitplace, int start, int end,
    long *o, sro_ord &ord_struct, int *weights)
{
  // weighted degree (aligned) of variables v_start..v_end, ordsgn -1
  while((start<end) && (weights[0]==0)) { start++; weights++; }
  while((start<end) && (weights[end-start]==0)) { end--; }
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

#ifndef NDEBUG
# define MYTEST 0
#else /* ifndef NDEBUG */
# define MYTEST 0
#endif /* ifndef NDEBUG */

static void rO_ISPrefix(int &place, int &bitplace, int &prev_ord,
    long *o, int N, int *v, sro_ord &ord_struct)
{
  if ((prev_ord== 1) || (bitplace!=BITS_PER_LONG))
    rO_Align(place,bitplace);
  // since we add something afterwards - it's better to start with anew!?

  ord_struct.ord_typ = ro_isTemp;
  ord_struct.data.isTemp.start = place;
  ord_struct.data.isTemp.pVarOffset = (int *)omMemDup(v);
  ord_struct.data.isTemp.suffixpos = -1;

  // We will act as rO_Syz on our own!!!
  // Here we allocate an exponent as a level placeholder
  o[place]= -1;
  prev_ord=-1;
  place++;

#if MYTEST
  Print("rO_ISPrefix: place = %d, v: {", ord_struct.data.isTemp.start);

  for( int i = 0; i <= N; i++ )
    Print("v[%d]: %09x", i, ord_struct.data.isTemp.pVarOffset[i]);

  PrintS("}!\n");
#endif
}
static void rO_ISSuffix(int &place, int &bitplace, int &prev_ord, long *o,
  int N, int *v, sro_ord *tmp_typ, int &typ_i, int sgn)
{
#if MYTEST
  Print("rO_ISSuffix: place = %d\n", place);
#endif

  // Let's find previous prefix:
  int typ_j = typ_i - 1;
  while(typ_j >= 0)
  {
    if( tmp_typ[typ_j].ord_typ == ro_isTemp)
      break;
    typ_j --;
  }

  assume( typ_j >= 0 );

  if( typ_j < 0 ) // Found NO prefix!!! :(
    return;

  assume( tmp_typ[typ_j].ord_typ == ro_isTemp );

  // Get saved state:
  const int start = tmp_typ[typ_j].data.isTemp.start;
  int *pVarOffset = tmp_typ[typ_j].data.isTemp.pVarOffset;

/*
  // shift up all blocks
  while(typ_j < (typ_i-1))
  {
    tmp_typ[typ_j] = tmp_typ[typ_j+1];
    typ_j++;
  }
  typ_j = typ_i - 1; // No increment for typ_i
*/
  tmp_typ[typ_j].data.isTemp.suffixpos = typ_i;

  // Let's keep that dummy for now...
  typ_j = typ_i; // the typ to change!
  typ_i++; // Just for now...


#if MYTEST
  PrintS("Changes in v: { ");
#endif

  for( int i = 0; i <= N; i++ ) // Note [0] == component !!! No Skip?
  {
    // Was i-th variable allocated inbetween?
    if( v[i] != pVarOffset[i] )
    {
      pVarOffset[i] = v[i]; // Save for later...
      v[i] = -1; // Undo!
      assume( pVarOffset[i] != -1 );
#if MYTEST
      Print("v[%d]: %010x; ", i, pVarOffset[i]);
#endif
    }
    else
      pVarOffset[i] = -1; // No change here...
  }

  if( pVarOffset[0] != -1 )
    pVarOffset[0] &= 0x0fff;

#if MYTEST
  PrintS(" }!\n");
#endif
  sro_ord &ord_struct = tmp_typ[typ_j];


  ord_struct.ord_typ = ro_is;
  ord_struct.data.is.start = start;
  ord_struct.data.is.end   = place;
  ord_struct.data.is.pVarOffset = pVarOffset;


  // What about component???
//   if( v[0] != -1 ) // There is a component already...???
//     if( o[ v[0] & 0x0fff ] == sgn )
//     {
//       pVarOffset[0] = -1; // NEVER USED Afterwards...
//       return;
//     }


  // Moreover: we need to allocate the module component (v[0]) here!
  if( v[0] == -1) // It's possible that there was module component v0 at the begining (before prefix)!
  {
    // Start with a whole long exponent
    if( bitplace != BITS_PER_LONG )
      rO_Align(place, bitplace);

    assume( bitplace == BITS_PER_LONG );
    bitplace -= BITS_PER_LONG;
    assume(bitplace == 0);
    v[0] = place | (bitplace << 24); // Never mind whether pVarOffset[0] > 0!!!
    o[place] = sgn; // Singnum for component ordering
    prev_ord = sgn;
  }
}


static unsigned long rGetExpSize(unsigned long bitmask, int & bits)
{
  if (bitmask == 0)
  {
    bits=16; bitmask=0xffff;
  }
  else if (bitmask <= 1L)
  {
    bits=1; bitmask = 1L;
  }
  else if (bitmask <= 3L)
  {
    bits=2; bitmask = 3L;
  }
  else if (bitmask <= 7L)
  {
    bits=3; bitmask=7L;
  }
  else if (bitmask <= 0xfL)
  {
    bits=4; bitmask=0xfL;
  }
  else if (bitmask <= 0x1fL)
  {
    bits=5; bitmask=0x1fL;
  }
  else if (bitmask <= 0x3fL)
  {
    bits=6; bitmask=0x3fL;
  }
#if SIZEOF_LONG == 8
  else if (bitmask <= 0x7fL)
  {
    bits=7; bitmask=0x7fL; /* 64 bit longs only */
  }
#endif
  else if (bitmask <= 0xffL)
  {
    bits=8; bitmask=0xffL;
  }
#if SIZEOF_LONG == 8
  else if (bitmask <= 0x1ffL)
  {
    bits=9; bitmask=0x1ffL; /* 64 bit longs only */
  }
#endif
  else if (bitmask <= 0x3ffL)
  {
    bits=10; bitmask=0x3ffL;
  }
#if SIZEOF_LONG == 8
  else if (bitmask <= 0xfffL)
  {
    bits=12; bitmask=0xfff; /* 64 bit longs only */
  }
#endif
  else if (bitmask <= 0xffffL)
  {
    bits=16; bitmask=0xffffL;
  }
#if SIZEOF_LONG == 8
  else if (bitmask <= 0xfffffL)
  {
    bits=20; bitmask=0xfffffL; /* 64 bit longs only */
  }
  else if (bitmask <= 0xffffffffL)
  {
    bits=32; bitmask=0xffffffffL;
  }
  else if (bitmask <= 0x7fffffffffffffffL)
  {
    bits=63; bitmask=0x7fffffffffffffffL; /* for overflow tests*/
  }
  else
  {
    bits=63; bitmask=0x7fffffffffffffffL; /* for overflow tests*/
  }
#else
  else if (bitmask <= 0x7fffffff)
  {
    bits=31; bitmask=0x7fffffff; /* for overflow tests*/
  }
  else
  {
    bits=31; bitmask=0x7fffffffL; /* for overflow tests*/
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
    if (bits == BIT_SIZEOF_LONG-1)
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


bool rSetISReference(const ideal F, const int i, const int p, const intvec * componentWeights, const ring r);


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

  int iNeedInducedOrderingSetup = 0; ///< How many induced ordering block do we have?
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

  for( int r_ord=r->order[i]; (r_ord != 0) && (i < nblocks); j++, r_ord=r->order[++i])
  {
    BOOLEAN copy_block_index=TRUE;

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
      case ringorder_S:
      {
#ifndef NDEBUG
        Warn("Error: unhandled ordering in rModifyRing: ringorder_S = [%d]", r_ord);
#endif
        order[j]=r_ord; /*r->order[i];*/
        break;
      }
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
          order[j]=ringorder_rs;
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
      case ringorder_IS:
      {
        if (omit_comp)
        {
#ifndef NDEBUG
          Warn("Error: WRONG USAGE of rModifyRing: cannot omit component due to the ordering block [%d]: %d (ringorder_IS)", i, r_ord);
#endif
          omit_comp = FALSE;
        }
        order[j]=r_ord; /*r->order[i];*/
        iNeedInducedOrderingSetup++;
        break;
      }
      case ringorder_s:
      {
        assume((i == 0) && (j == 0));
        if (omit_comp)
        {
#ifndef NDEBUG
          Warn("WRONG USAGE? of rModifyRing: omitting component due to the ordering block [%d]: %d (ringorder_s)", i, r_ord);
#endif
          omit_comp = FALSE;
        }
        order[j]=r_ord; /*r->order[i];*/
        break;
      }
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

    // order[j]=ringorder_no; //  done by omAlloc0
  }
  if(!need_other_ring)
  {
    omFreeSize(order,(nblocks+1)*sizeof(int));
    omFreeSize(block0,(nblocks+1)*sizeof(int));
    omFreeSize(block1,(nblocks+1)*sizeof(int));
    omFreeSize(wvhdl,(nblocks+1)*sizeof(int_ptr));
    return r;
  }
  ring res=(ring)omAlloc0Bin(sip_sring_bin);
  *res = *r;

#ifdef HAVE_PLURAL
  res->GetNC() = NULL;
#endif

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
  if (res->typ != NULL)
  {
    if( res->typ[0].ord_typ == ro_syz) // "s" Always on [0] place!
    {
      res->typ[0] = r->typ[0]; // Copy struct!? + setup the same limit!

      if (r->typ[0].data.syz.limit > 0)
      {
        res->typ[0].data.syz.syz_index
          = (int*) omAlloc((r->typ[0].data.syz.limit +1)*sizeof(int));
        memcpy(res->typ[0].data.syz.syz_index, r->typ[0].data.syz.syz_index,
              (r->typ[0].data.syz.limit +1)*sizeof(int));
      }
    }

    if( iNeedInducedOrderingSetup > 0 )
    {
      for(j = 0, i = 0; (i < nblocks) && (iNeedInducedOrderingSetup > 0); i++)
        if( res->typ[i].ord_typ == ro_is ) // Search for suffixes!
        {
          ideal F = idrHeadR(r->typ[i].data.is.F, r, res); // Copy F from r into res!
          assume(
            rSetISReference(
              F,  // WILL BE COPIED!
              r->typ[i].data.is.limit,
              j++,
              r->typ[i].data.is.componentWeights, // WILL BE COPIED
              res)
            );
          id_Delete(&F, res);
          iNeedInducedOrderingSetup--;
        }
    } // Process all induced Ordering blocks! ...
  }
  // the special case: homog (omit_degree) and 1 block rs: that is global:
  // it comes from dp
  res->OrdSgn=r->OrdSgn;


#ifdef HAVE_PLURAL
  if (rIsPluralRing(r))
  {
    if ( nc_rComplete(r, res, false) ) // no qideal!
    {
#ifndef NDEBUG
      WarnS("error in nc_rComplete");
#endif
      // cleanup?

//      rDelete(res);
//      return r;

      // just go on..
    }

    if( rIsSCA(r) )
    {
      if( !sca_Force(res, scaFirstAltVar(r), scaLastAltVar(r)) )
      WarnS("error in sca_Force!");
    }
  }
#endif

  return res;
}

// construct Wp,C ring
ring rModifyRing_Wp(ring r, int* weights)
{
  ring res=(ring)omAlloc0Bin(sip_sring_bin);
  *res = *r;
#ifdef HAVE_PLURAL
  res->GetNC() = NULL;
#endif

  /*weights: entries for 3 blocks: NULL*/
  res->wvhdl = (int **)omAlloc0(3 * sizeof(int_ptr));
  /*order: Wp,C,0*/
  res->order = (int *) omAlloc(3 * sizeof(int *));
  res->block0 = (int *)omAlloc0(3 * sizeof(int *));
  res->block1 = (int *)omAlloc0(3 * sizeof(int *));
  /* ringorder Wp for the first block: var 1..r->N */
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
#ifdef HAVE_PLURAL
  if (rIsPluralRing(r))
  {
    if ( nc_rComplete(r, res, false) ) // no qideal!
    {
#ifndef NDEBUG
      WarnS("error in nc_rComplete");
#endif
      // cleanup?

//      rDelete(res);
//      return r;

      // just go on..
    }
  }
#endif
  return res;
}

// construct lp, C ring with r->N variables, r->names vars....
ring rModifyRing_Simple(ring r, BOOLEAN ommit_degree, BOOLEAN ommit_comp, unsigned long exp_limit, BOOLEAN &simple)
{
  simple=TRUE;
  if (!rHasSimpleOrder(r))
  {
    simple=FALSE; // sorting needed
    assume (r != NULL );
    assume (exp_limit > 1);
    int bits;

    exp_limit=rGetExpSize(exp_limit, bits, r->N);

    int nblocks=1+(ommit_comp!=0);
    int *order=(int*)omAlloc0((nblocks+1)*sizeof(int));
    int *block0=(int*)omAlloc0((nblocks+1)*sizeof(int));
    int *block1=(int*)omAlloc0((nblocks+1)*sizeof(int));
    int **wvhdl=(int**)omAlloc0((nblocks+1)*sizeof(int_ptr));

    order[0]=ringorder_lp;
    block0[0]=1;
    block1[0]=r->N;
    if (!ommit_comp)
    {
      order[1]=ringorder_C;
    }
    ring res=(ring)omAlloc0Bin(sip_sring_bin);
    *res = *r;
#ifdef HAVE_PLURAL
    res->GetNC() = NULL;
#endif
    // res->qideal, res->idroot ???
    res->wvhdl=wvhdl;
    res->order=order;
    res->block0=block0;
    res->block1=block1;
    res->bitmask=exp_limit;
    int tmpref=r->cf->ref;
    rComplete(res, 1);
    r->cf->ref=tmpref;

#ifdef HAVE_PLURAL
    if (rIsPluralRing(r))
    {
      if ( nc_rComplete(r, res, false) ) // no qideal!
      {
#ifndef NDEBUG
        WarnS("error in nc_rComplete");
#endif
        // cleanup?

//      rDelete(res);
//      return r;

      // just go on..
      }
    }
#endif

    rOptimizeLDeg(res);

    return res;
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
  omFreeBin(r,sip_sring_bin);
}

void rKillModified_Wp_Ring(ring r)
{
  rUnComplete(r);
  omFree(r->order);
  omFree(r->block0);
  omFree(r->block1);
  omFree(r->wvhdl[0]);
  omFree(r->wvhdl);
  omFreeBin(r,sip_sring_bin);
}

static void rSetOutParams(ring r)
{
  r->VectorOut = (r->order[0] == ringorder_c);
  r->ShortOut = TRUE;
  {
    int i;
    if (r->parameter!=NULL)
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
      // are introduced, but their names are never set
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
* sets r->MixedOrder and r->ComponentOrder for orderings with more than one block
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
    case ringorder_am:
    case ringorder_a64:
      if (r->OrdSgn==-1) r->MixedOrder=TRUE;
      break;
    case ringorder_ls:
    case ringorder_rs:
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
      r->LexOrder=TRUE;
      break;
    case ringorder_IS:
    { // TODO: What is r->ComponentOrder???
      r->MixedOrder=TRUE;
/*
      if( r->block0[o] != 0 ) // Suffix has the comonent
        r->ComponentOrder = r->block0[o];
      else // Prefix has level...
        r->ComponentOrder=-1;
*/
      break;
    }

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
  if ((order[i]== ringorder_ws)
  || (order[i]==ringorder_Ws)
  || (order[i]== ringorder_wp)
  || (order[i]==ringorder_Wp)
  || (order[i]== ringorder_a)
   /*|| (order[i]==ringorder_A)*/)
  {
    int j;
    for(j=block1[i]-r->block0[i];j>=0;j--)
    {
      if (r->firstwv[j]<0) r->MixedOrder=TRUE;
      if (r->firstwv[j]==0) r->LexOrder=TRUE;
    }
  }
  else if (order[i]==ringorder_a64)
  {
    int j;
    int64 *w=rGetWeightVec(r);
    for(j=block1[i]-r->block0[i];j>=0;j--)
    {
      if (w[j]==0) r->LexOrder=TRUE;
    }
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
  else if (r->pFDeg == p_Totaldegree)
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
  r->pLDegOrig=r->pLDeg;
}

// set pFDeg, pLDeg, MixOrder, ComponentOrder, etc
static void rSetDegStuff(ring r)
{
  int* order = r->order;
  int* block0 = r->block0;
  int* block1 = r->block1;
  int** wvhdl = r->wvhdl;

  if (order[0]==ringorder_S ||order[0]==ringorder_s || order[0]==ringorder_IS)
  {
    order++;
    block0++;
    block1++;
    wvhdl++;
  }
  r->LexOrder = FALSE;
  r->MixedOrder = FALSE;
  r->ComponentOrder = 1;
  r->pFDeg = p_Totaldegree;
  r->pLDeg = (r->OrdSgn == 1 ? pLDegb : pLDeg0);

  /*======== ordering type is (am,_) ==================*/
  if ((order[0]==ringorder_am)
  )
  {
    r->MixedOrder = FALSE;
    for(int ii=block0[0];ii<=block1[0];ii++)
      if (wvhdl[0][ii-1]<0) { r->MixedOrder=TRUE;break;}
    r->LexOrder=FALSE;
    for(int ii=block0[0];ii<=block1[0];ii++)
      if (wvhdl[0][ii-1]==0) { r->LexOrder=TRUE;break;}
    if ((block0[0]==1)&&(block1[0]==r->N))
    {
      r->pFDeg = pDeg;
      r->pLDeg = pLDeg1c_Deg;
    }
    else
    {
      r->pFDeg = pWTotaldegree;
      r->LexOrder=TRUE;
      r->pLDeg = pLDeg1c_WFirstTotalDegree;
    }
    r->firstwv = wvhdl[0];
  }
  /*======== ordering type is (_,c) =========================*/
  else if ((order[0]==ringorder_unspec) || (order[1] == 0)
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
    if ((order[0] == ringorder_lp)
    || (order[0] == ringorder_ls)
    || (order[0] == ringorder_rp)
    || (order[0] == ringorder_rs))
    {
      r->LexOrder=TRUE;
      r->pLDeg = pLDeg1c;
      r->pFDeg = p_Totaldegree;
    }
    if ((order[0] == ringorder_a)
    || (order[0] == ringorder_wp)
    || (order[0] == ringorder_Wp)
    || (order[0] == ringorder_ws)
    || (order[0] == ringorder_Ws))
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
    if ((order[1] == ringorder_lp)
    || (order[1] == ringorder_ls)
    || (order[1] == ringorder_rp)
    || order[1] == ringorder_rs)
    {
      r->LexOrder=TRUE;
      r->pLDeg = pLDeg1c;
      r->pFDeg = p_Totaldegree;
    }
    r->firstBlockEnds=block1[1];
    if (wvhdl!=NULL) r->firstwv = wvhdl[1];
    if ((order[1] == ringorder_a)
    || (order[1] == ringorder_wp)
    || (order[1] == ringorder_Wp)
    || (order[1] == ringorder_ws)
    || (order[1] == ringorder_Ws))
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
    r->pFDeg = pWTotaldegree; // may be improved: p_Totaldegree for lp/dp/ls/.. blocks
  }

  if (rOrd_is_Totaldegree_Ordering(r) || rOrd_is_WeightedDegree_Ordering(r))
    r->pFDeg = pDeg;

  r->pFDegOrig = r->pFDeg;
  rOptimizeLDeg(r); // also sets r->pLDegOrig
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
      if((r->typ[i].ord_typ==ro_wp_neg)
      ||(r->typ[i].ord_typ==ro_am))
        l++;
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
        else if(r->typ[i].ord_typ==ro_am)
        {
          r->NegWeightL_Offset[l]=r->typ[i].data.am.place;
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
#ifdef HAVE_RINGS
  if (rField_is_Extension(r) || rField_is_Q(r) || rField_is_Ring(r))
#else
  if (rField_is_Extension(r) || rField_is_Q(r))
#endif
    r->options |= Sy_bit(OPT_INTSTRATEGY);
  else
    r->options &= ~Sy_bit(OPT_INTSTRATEGY);

  // set redTail
  if (r->LexOrder || r->OrdSgn == -1 || rField_is_Extension(r))
    r->options &= ~Sy_bit(OPT_REDTAIL);
  else
    r->options |= Sy_bit(OPT_REDTAIL);
}

static void rCheckOrdSgn(ring r,int i/*current block*/);

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
  long *tmp_ordsgn=(long *)omAlloc0(3*(n+r->N)*sizeof(long));
  // will be used for VarOffset:
  int *v=(int *)omAlloc((r->N+1)*sizeof(int));
  for(i=r->N; i>=0 ; i--)
  {
    v[i]=-1;
  }
  sro_ord *tmp_typ=(sro_ord *)omAlloc0(3*(n+r->N)*sizeof(sro_ord));
  int typ_i=0;
  int prev_ordsgn=0;

  // fill in v, tmp_typ, tmp_ordsgn, determine typ_i (== ordSize)
  int j=0;
  int j_bits=BITS_PER_LONG;

  BOOLEAN need_to_add_comp=FALSE; // Only for ringorder_s and ringorder_S!

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

      case ringorder_am:
        rO_WMDegree(j,j_bits,r->block0[i],r->block1[i],tmp_ordsgn,tmp_typ[typ_i],
                   r->wvhdl[i]);
        typ_i++;
        break;

      case ringorder_a64:
        rO_WDegree64(j,j_bits,r->block0[i],r->block1[i],tmp_ordsgn,
                     tmp_typ[typ_i], (int64 *)(r->wvhdl[i]));
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
        rCheckOrdSgn(r,i);
        break;

      case ringorder_rs:
        rO_LexVars_neg(j, j_bits, r->block1[i],r->block0[i], prev_ordsgn,
                       tmp_ordsgn,v, bits, -1);
        rCheckOrdSgn(r,i);
        break;

      case ringorder_rp:
        rO_LexVars(j, j_bits, r->block1[i],r->block0[i], prev_ordsgn,
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
        rCheckOrdSgn(r,i);
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
        rCheckOrdSgn(r,i);
        break;

      case ringorder_wp:
        rO_WDegree(j,j_bits,r->block0[i],r->block1[i],tmp_ordsgn,
                   tmp_typ[typ_i], r->wvhdl[i]);
        typ_i++;
        { // check for weights <=0
          int jj;
          BOOLEAN have_bad_weights=FALSE;
          for(jj=r->block1[i]-r->block0[i];jj>=0; jj--)
          {
            if (r->wvhdl[i][jj]<=0) have_bad_weights=TRUE;
          }
          if (have_bad_weights)
          {
             rO_TDegree(j,j_bits,r->block0[i],r->block1[i],tmp_ordsgn,
                                     tmp_typ[typ_i]);
             typ_i++;
             rCheckOrdSgn(r,i);
          }
        }
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
        { // check for weights <=0
          int jj;
          BOOLEAN have_bad_weights=FALSE;
          for(jj=r->block1[i]-r->block0[i];jj>=0; jj--)
          {
            if (r->wvhdl[i][jj]<=0) have_bad_weights=TRUE;
          }
          if (have_bad_weights)
          {
             rO_TDegree(j,j_bits,r->block0[i],r->block1[i],tmp_ordsgn,
                                     tmp_typ[typ_i]);
             typ_i++;
             rCheckOrdSgn(r,i);
          }
        }
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
        rCheckOrdSgn(r,i);
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
        rCheckOrdSgn(r,i);
        break;

      case ringorder_S:
        assume(typ_i == 1); // For LaScala3 only: on the 2nd place ([1])!
        // TODO: for K[x]: it is 0...?!
        rO_Syzcomp(j, j_bits,prev_ordsgn, tmp_ordsgn,tmp_typ[typ_i]);
        need_to_add_comp=TRUE;
        typ_i++;
        break;

      case ringorder_s:
        assume(typ_i == 0 && j == 0);
        rO_Syz(j, j_bits, prev_ordsgn, tmp_ordsgn, tmp_typ[typ_i]); // set syz-limit?
        need_to_add_comp=TRUE;
        typ_i++;
        break;

      case ringorder_IS:
      {

        assume( r->block0[i] == r->block1[i] );
        const int s = r->block0[i];
        assume( -2 < s && s < 2);

        if(s == 0) // Prefix IS
          rO_ISPrefix(j, j_bits, prev_ordsgn, tmp_ordsgn, r->N, v, tmp_typ[typ_i++]); // What about prev_ordsgn?
        else // s = +1 or -1 // Note: typ_i might be incrimented here inside!
        {
          rO_ISSuffix(j, j_bits, prev_ordsgn, tmp_ordsgn, r->N, v, tmp_typ, typ_i, s); // Suffix.
          need_to_add_comp=FALSE;
        }

        break;
      }
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
  for(i=1 ; i<=r->N ; i++)
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

  omFreeSize((ADDRESS)tmp_ordsgn,(3*(n+r->N)*sizeof(long)));

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
  omFreeSize((ADDRESS)tmp_typ,(3*(n+r->N)*sizeof(sro_ord)));

  // ----------------------------
  // indices for (first copy of ) variable entries in exp.e vector (VarOffset):
  r->VarOffset=v;

  // ----------------------------
  // other indicies
  r->pCompIndex=(r->VarOffset[0] & 0xffff); //r->VarOffset[0];
  i=0; // position
  j=0; // index in r->typ
  if (i==r->pCompIndex) i++; // IS???
  while ((j < r->OrdSize)
         && ((r->typ[j].ord_typ==ro_syzcomp) ||
             (r->typ[j].ord_typ==ro_syz) || (r->typ[j].ord_typ==ro_isTemp) || (r->typ[j].ord_typ==ro_is) ||
             (r->order[r->typ[j].order_index] == ringorder_aa)))
  {
    i++; j++;
  }
  // No use of j anymore!!!????

  if (i==r->pCompIndex) i++;
  r->pOrdIndex=i; // How came it is "i" here???!!!! exp[r->pOrdIndex] is order of a poly... This may be wrong!!! IS

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

static void rCheckOrdSgn(ring r,int i/*current block*/)
{ // set r->OrdSgn
  if ( r->OrdSgn==1)
  {
    int oo=-1;
    int jj;
    for(jj=i-1;jj>=0;jj--)
    {
      if(((r->order[jj]==ringorder_a)
        ||(r->order[jj]==ringorder_aa)
        ||(r->order[jj]==ringorder_a64))
      &&(r->block0[jj]<=r->block0[i])
      &&(r->block1[jj]>=r->block1[i]))
      { oo=1; break;}
    }
    r->OrdSgn=oo;
  }
}


void rUnComplete(ring r)
{
  if (r == NULL) return;
  if (r->VarOffset != NULL)
  {
    if (r->OrdSize!=0 && r->typ != NULL)
    {
      for(int i = 0; i < r->OrdSize; i++)
        if( r->typ[i].ord_typ == ro_is) // Search for suffixes! (prefix have the same VarOffset)
        {
          id_Delete(&r->typ[i].data.is.F, r);
          r->typ[i].data.is.F = NULL; // ?

          if( r->typ[i].data.is.componentWeights != NULL )
          {
            delete r->typ[i].data.is.componentWeights;
            r->typ[i].data.is.componentWeights = NULL; // ?
          }

          if( r->typ[i].data.is.pVarOffset != NULL )
          {
            omFreeSize((ADDRESS)r->typ[i].data.is.pVarOffset, (r->N +1)*sizeof(int));
            r->typ[i].data.is.pVarOffset = NULL; // ?
          }
        }
        else if (r->typ[i].ord_typ == ro_syz)
        {
          if(r->typ[i].data.syz.limit > 0)
            omFreeSize(r->typ[i].data.syz.syz_index, ((r->typ[i].data.syz.limit) +1)*sizeof(int));
          r->typ[i].data.syz.syz_index = NULL;
        }
        else if (r->typ[i].ord_typ == ro_syzcomp)
        {
          assume( r->typ[i].data.syzcomp.ShiftedComponents == NULL );
          assume( r->typ[i].data.syzcomp.Components        == NULL );
//          WarnS( "rUnComplete : ord_typ == ro_syzcomp was unhandled!!! Possibly memory leak!!!"  );
#ifndef NDEBUG
//          assume(0);
#endif
        }

      omFreeSize((ADDRESS)r->typ,r->OrdSize*sizeof(sro_ord)); r->typ = NULL;
    }

    if (r->order != NULL)
    {
      // delete r->order!!!???
    }

    if (r->PolyBin != NULL)
      omUnGetSpecBin(&(r->PolyBin));

    omFreeSize((ADDRESS)r->VarOffset, (r->N +1)*sizeof(int));

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

  r->VarL_Size = j; // number of long with exp. entries in
                    //  in p->exp
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

  // find minimal bit shift in each long exp entry
  for (i=1;i<=r->N;i++)
  {
    if (shifts[r->VarOffset[i] & 0xffffff] > r->VarOffset[i] >> 24)
      shifts[r->VarOffset[i] & 0xffffff] = r->VarOffset[i] >> 24;
  }
  // reset r->VarOffset: set the minimal shift to 0
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
  // corresponds to ro_typ from ring.h:
  const char *TYP[]={"ro_dp","ro_wp","ro_am", "ro_wp64","ro_wp_neg","ro_cp",
                     "ro_syzcomp", "ro_syz", "ro_isTemp", "ro_is", "ro_none"};
  int i,j;

  Print("ExpL_Size:%d ",r->ExpL_Size);
  Print("CmpL_Size:%d ",r->CmpL_Size);
  Print("VarL_Size:%d\n",r->VarL_Size);
  Print("bitmask=0x%lx (expbound=%ld) \n",r->bitmask, r->bitmask);
  Print("BitsPerExp=%d ExpPerLong=%d MinExpPerLong=%d at L[%d]\n", r->BitsPerExp, r->ExpPerLong, r->MinExpPerLong, r->VarL_Offset[0]);
  PrintS("varoffset:\n");
  if (r->VarOffset==NULL) PrintS(" NULL\n");
  else
    for(j=0;j<=r->N;j++)
      Print("  v%d at e-pos %d, bit %d\n",
            j,r->VarOffset[j] & 0xffffff, r->VarOffset[j] >>24);
  Print("divmask=%lx\n", r->divmask);
  PrintS("ordsgn:\n");
  for(j=0;j<r->CmpL_Size;j++)
    Print("  ordsgn %ld at pos %d\n",r->ordsgn[j],j);
  Print("OrdSgn:%d\n",r->OrdSgn);
  PrintS("ordrec:\n");
  for(j=0;j<r->OrdSize;j++)
  {
    Print("  typ %s", TYP[r->typ[j].ord_typ]);


    if (r->typ[j].ord_typ==ro_syz)
    {
      const short place = r->typ[j].data.syz.place;
      const int limit = r->typ[j].data.syz.limit;
      const int curr_index = r->typ[j].data.syz.curr_index;
      const int* syz_index = r->typ[j].data.syz.syz_index;

      Print("  limit %d (place: %d, curr_index: %d), syz_index: ", limit, place, curr_index);

      if( syz_index == NULL )
        PrintS("(NULL)");
      else
      {
        Print("{");
        for( i=0; i <= limit; i++ )
          Print("%d ", syz_index[i]);
        Print("}");
      }

    }
    else if (r->typ[j].ord_typ==ro_isTemp)
    {
      Print("  start (level) %d, suffixpos: %d, VO: ",r->typ[j].data.isTemp.start, r->typ[j].data.isTemp.suffixpos);

#ifndef NDEBUG
      for( int k = 0; k <= r->N; k++)
        if (r->typ[j].data.isTemp.pVarOffset[k] != -1)
          Print("[%2d]: %09x; ", k, r->typ[j].data.isTemp.pVarOffset[k]);
#endif
    }
    else if (r->typ[j].ord_typ==ro_is)
    {
      Print("  start %d, end: %d: ",r->typ[j].data.is.start, r->typ[j].data.is.end);

//      for( int k = 0; k <= r->N; k++) if (r->typ[j].data.is.pVarOffset[k] != -1) Print("[%2d]: %04x; ", k, r->typ[j].data.is.pVarOffset[k]);

      Print("  limit %d\n",r->typ[j].data.is.limit);
      #ifndef NDEBUG
      PrintS("  F: ");idShow(r->typ[j].data.is.F, r, r, 1);
      #endif

      PrintS("weights: ");

      if( r->typ[j].data.is.componentWeights == NULL )
        PrintS("NULL == [0,...0]\n");
      else
      {
        (r->typ[j].data.is.componentWeights)->show(); PrintLn();
      }
    }
    else if  (r->typ[j].ord_typ==ro_am)
    {
      Print("  place %d",r->typ[j].data.am.place);
      Print("  start %d",r->typ[j].data.am.start);
      Print("  end %d",r->typ[j].data.am.end);
      Print("  len_gen %d",r->typ[j].data.am.len_gen);
      PrintS(" w:");
      int l=0;
      for(l=r->typ[j].data.am.start;l<=r->typ[j].data.am.end;l++)
            Print(" %d",r->typ[j].data.am.weights[l-r->typ[j].data.am.start]);
      l=r->typ[j].data.am.end+1;
      int ll=r->typ[j].data.am.weights[l-r->typ[j].data.am.start];
      PrintS(" m:");
      for(int lll=l+1;lll<l+ll+1;lll++)
            Print(" %d",r->typ[j].data.am.weights[lll-r->typ[j].data.am.start]);
    }
    else
    {
      Print("  place %d",r->typ[j].data.dp.place);

      if (r->typ[j].ord_typ!=ro_syzcomp  && r->typ[j].ord_typ!=ro_syz)
      {
        Print("  start %d",r->typ[j].data.dp.start);
        Print("  end %d",r->typ[j].data.dp.end);
        if ((r->typ[j].ord_typ==ro_wp)
        || (r->typ[j].ord_typ==ro_wp_neg))
        {
          PrintS(" w:");
          for(int l=r->typ[j].data.wp.start;l<=r->typ[j].data.wp.end;l++)
            Print(" %d",r->typ[j].data.wp.weights[l-r->typ[j].data.wp.start]);
        }
        else if (r->typ[j].ord_typ==ro_wp64)
        {
          PrintS(" w64:");
          int l;
          for(l=r->typ[j].data.wp64.start;l<=r->typ[j].data.wp64.end;l++)
            Print(" %ld",(long)(((int64*)r->typ[j].data.wp64.weights64)+l-r->typ[j].data.wp64.start));
          }
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
      Print("ordsgn %ld ", r->ordsgn[j]);
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
  Print("LexOrder:%d, MixedOrder:%d\n",r->LexOrder, r->MixedOrder);

  // p_Procs stuff
  p_Procs_s proc_names;
  const char* field;
  const char* length;
  const char* ord;
  p_Debug_GetProcNames(r, &proc_names); // changes p_Procs!!!
  p_Debug_GetSpecNames(r, field, length, ord);

  Print("p_Spec  : %s, %s, %s\n", field, length, ord);
  PrintS("p_Procs :\n");
  for (i=0; i<(int) (sizeof(p_Procs_s)/sizeof(void*)); i++)
  {
    Print(" %s,\n", ((char**) &proc_names)[i]);
  }

  {
#define pFDeg_CASE(A) if(r->pFDeg == A) PrintS( "" #A "" )
    Print("pFDeg   : ");

    pFDeg_CASE(p_Totaldegree); else
      pFDeg_CASE(pWFirstTotalDegree); else
      pFDeg_CASE(pWTotaldegree); else
      pFDeg_CASE(pDeg); else
      Print("(%p)", r->pFDeg); // default case

    PrintLn();
#undef pFDeg_CASE
  }
  Print("pSetm:");
  void p_Setm_Dummy(poly p, const ring r);
  void p_Setm_TotalDegree(poly p, const ring r);
  void p_Setm_WFirstTotalDegree(poly p, const ring r);
  void p_Setm_General(poly p, const ring r);
  if (r->p_Setm==p_Setm_General) PrintS("p_Setm_General\n");
  else if (r->p_Setm==p_Setm_Dummy) PrintS("p_Setm_Dummy\n");
  else if (r->p_Setm==p_Setm_TotalDegree) PrintS("p_Setm_Totaldegree\n");
  else if (r->p_Setm==p_Setm_WFirstTotalDegree) PrintS("p_Setm_WFirstTotalDegree\n");
  else Print("%x\n",r->p_Setm);

}

void p_DebugPrint(poly p, const ring r)
{
  int i,j;
  p_Write(p,r);
  j=2;
  while(p!=NULL)
  {
    Print("\nexp[0..%d]\n",r->ExpL_Size-1);
    for(i=0;i<r->ExpL_Size;i++)
      Print("%ld ",p->exp[i]);
    PrintLn();
    Print("v0:%ld ",p_GetComp(p, r));
    for(i=1;i<=r->N;i++) Print(" v%d:%ld",i,p_GetExp(p,i, r));
    PrintLn();
    pIter(p);
    j--;
    if (j==0) { PrintS("...\n"); break; }
  }
}

void pDebugPrint(poly p)
{
  p_DebugPrint(p, currRing);
}
#endif // RDEBUG

/// debug-print monomial poly/vector p, assuming that it lives in the ring R
static inline void m_DebugPrint(const poly p, const ring R)
{
  Print("\nexp[0..%d]\n", R->ExpL_Size - 1);
  for(int i = 0; i < R->ExpL_Size; i++)
    Print("%09lx ", p->exp[i]);
  PrintLn();
  Print("v0:%9ld ", p_GetComp(p, R));
  for(int i = 1; i <= R->N; i++) Print(" v%d:%5ld",i, p_GetExp(p, i, R));
  PrintLn();
}


#ifndef NDEBUG
/// debug-print at most nTerms (2 by default) terms from poly/vector p,
/// assuming that lt(p) lives in lmRing and tail(p) lives in tailRing.
void p_DebugPrint(const poly p, const ring lmRing, const ring tailRing, const int nTerms)
{
  assume( nTerms >= 0 );
  if( p != NULL )
  {
    assume( p != NULL );

    p_Write(p, lmRing, tailRing);

    if( (p != NULL) && (nTerms > 0) )
    {
      assume( p != NULL );
      assume( nTerms > 0 );

      // debug pring leading term
      m_DebugPrint(p, lmRing);

      poly q = pNext(p); // q = tail(p)

      // debug pring tail (at most nTerms-1 terms from it)
      for(int j = nTerms - 1; (q !=NULL) && (j > 0); pIter(q), --j)
        m_DebugPrint(q, tailRing);

      if (q != NULL)
        PrintS("...\n");
    }
  }
  else
    PrintS("0\n");
}
#endif


//    F = system("ISUpdateComponents", F, V, MIN );
//    // replace gen(i) -> gen(MIN + V[i-MIN]) for all i > MIN in all terms from F!
void pISUpdateComponents(ideal F, const intvec *const V, const int MIN, const ring r = currRing)
{
  assume( V != NULL );
  assume( MIN >= 0 );

  if( F == NULL )
    return;

  for( int j = (F->ncols*F->nrows) - 1; j >= 0; j-- )
  {
#ifdef PDEBUG
    Print("F[%d]:", j);
    p_DebugPrint(F->m[j], r, r, 0);
#endif

    for( poly p = F->m[j]; p != NULL; pIter(p) )
    {
      int c = p_GetComp(p, r);

      if( c > MIN )
      {
#ifdef PDEBUG
        Print("gen[%d] -> gen(%d)\n", c, MIN + (*V)[ c - MIN - 1 ]);
#endif

        p_SetComp( p, MIN + (*V)[ c - MIN - 1 ], r );
      }
    }
#ifdef PDEBUG
    Print("new F[%d]:", j);
    p_Test(F->m[j], r);
    p_DebugPrint(F->m[j], r, r, 0);
#endif
  }

}




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
  assume(r->typ[1].ord_typ == ro_syzcomp);

  r->typ[1].data.syzcomp.length = length;
  rNChangeSComps( currComponents, currShiftedComponents, r);
}
void rDBGetSComps(int** currComponents,
                 long** currShiftedComponents,
                 int *length,
                 ring r)
{
  assume(r->typ[1].ord_typ == ro_syzcomp);

  *length = r->typ[1].data.syzcomp.length;
  rNGetSComps( currComponents, currShiftedComponents, r);
}
#endif

void rNChangeSComps(int* currComponents, long* currShiftedComponents, ring r)
{
  assume(r->typ[1].ord_typ == ro_syzcomp);

  r->typ[1].data.syzcomp.ShiftedComponents = currShiftedComponents;
  r->typ[1].data.syzcomp.Components = currComponents;
}

void rNGetSComps(int** currComponents, long** currShiftedComponents, ring r)
{
  assume(r->typ[1].ord_typ == ro_syzcomp);

  *currShiftedComponents = r->typ[1].data.syzcomp.ShiftedComponents;
  *currComponents =   r->typ[1].data.syzcomp.Components;
}

/////////////////////////////////////////////////////////////////////////////
//
// The following routines all take as input a ring r, and return R
// where R has a certain property. R might be equal r in which case r
// had already this property
//
// Without argument, these functions work on currRing and change it,
// if necessary

// for the time being, this is still here
static ring rAssure_SyzComp(const ring r, BOOLEAN complete = TRUE);

ring rCurrRingAssure_SyzComp()
{
#if MYTEST
  PrintS("rCurrRingAssure_SyzComp(), currRing:  \n");
  rWrite(currRing);
#ifdef RDEBUG
  rDebugPrint(currRing);
#endif
  PrintLn();
#endif

  ring r = rAssure_SyzComp(currRing, TRUE);

  if( r != currRing )
  {
    rChangeCurrRing(r);
    assume(currRing == r);

#if MYTEST
  PrintS("\nrCurrRingAssure_SyzComp(): new currRing: \n");
  rWrite(currRing);
#ifdef RDEBUG
  rDebugPrint(currRing);
#endif
  PrintLn();
#endif
  }

  return r;
}

static ring rAssure_SyzComp(const ring r, BOOLEAN complete)
{
  if ( (r->order[0] == ringorder_s) ) return r;

  if ( (r->order[0] == ringorder_IS) )
  {
#ifndef NDEBUG
    WarnS("rAssure_SyzComp: input ring has an IS-ordering!");
#endif
//    return r;
  }
  ring res=rCopy0(r, FALSE, FALSE);
  int i=rBlocks(r);
  int j;

  res->order=(int *)omAlloc((i+1)*sizeof(int));
  res->block0=(int *)omAlloc0((i+1)*sizeof(int));
  res->block1=(int *)omAlloc0((i+1)*sizeof(int));
  int ** wvhdl =(int **)omAlloc0((i+1)*sizeof(int**));
  for(j=i;j>0;j--)
  {
    res->order[j]=r->order[j-1];
    res->block0[j]=r->block0[j-1];
    res->block1[j]=r->block1[j-1];
    if (r->wvhdl[j-1] != NULL)
    {
      wvhdl[j] = (int*) omMemDup(r->wvhdl[j-1]);
    }
  }
  res->order[0]=ringorder_s;

  res->wvhdl = wvhdl;

  if (complete)
  {
    rComplete(res, 1);

#ifdef HAVE_PLURAL
    if (rIsPluralRing(r))
    {
      if ( nc_rComplete(r, res, false) ) // no qideal!
      {
#ifndef NDEBUG
        WarnS("error in nc_rComplete");      // cleanup?//      rDelete(res);//      return r;      // just go on..
#endif
      }
    }
    assume(rIsPluralRing(r) == rIsPluralRing(res));
#endif


#ifdef HAVE_PLURAL
    ring old_ring = r;

#if MYTEST
    PrintS("rCurrRingAssure_SyzComp(): temp r': ");
    rWrite(r);
#ifdef RDEBUG
    rDebugPrint(r);
#endif
    PrintLn();
#endif
#endif


    if (r->qideal!=NULL)
    {
      res->qideal= idrCopyR_NoSort(r->qideal, r, res);

      assume(idRankFreeModule(res->qideal, res) == 0);

#ifdef HAVE_PLURAL
      if( rIsPluralRing(res) )
        if( nc_SetupQuotient(res, r, true) )
        {
//          WarnS("error in nc_SetupQuotient"); // cleanup?      rDelete(res);       return r;  // just go on...?
        }

#endif
      assume(idRankFreeModule(res->qideal, res) == 0);
    }

#ifdef HAVE_PLURAL
    assume((res->qideal==NULL) == (old_ring->qideal==NULL));
    assume(rIsPluralRing(res) == rIsPluralRing(old_ring));
    assume(rIsSCA(res) == rIsSCA(old_ring));
    assume(ncRingType(res) == ncRingType(old_ring));
#endif

#if MYTEST
    PrintS("rCurrRingAssure_SyzComp(): res: ");
    rWrite(r);
#ifdef RDEBUG
    rDebugPrint(r);
#endif
    PrintLn();
#endif

  }

  return res;
}

ring rAssure_TDeg(ring r, int start_var, int end_var, int &pos)
{
  int i;
  if (r->typ!=NULL)
  {
    for(i=r->OrdSize-1;i>=0;i--)
    {
      if ((r->typ[i].ord_typ==ro_dp)
      && (r->typ[i].data.dp.start==start_var)
      && (r->typ[i].data.dp.end==end_var))
      {
        pos=r->typ[i].data.dp.place;
        //printf("no change, pos=%d\n",pos);
        return r;
      }
    }
  }

#ifdef HAVE_PLURAL
  nc_struct* save=r->GetNC();
  r->GetNC()=NULL;
#endif
  ring res=rCopy(r);

  i=rBlocks(r);
  int j;

  res->ExpL_Size=r->ExpL_Size+1; // one word more in each monom
  res->PolyBin=omGetSpecBin(POLYSIZE + (res->ExpL_Size)*sizeof(long));
  omFree((ADDRESS)res->ordsgn);
  res->ordsgn=(long *)omAlloc0(res->ExpL_Size*sizeof(long));
  for(j=0;j<r->CmpL_Size;j++)
  {
    res->ordsgn[j] = r->ordsgn[j];
  }
  res->OrdSize=r->OrdSize+1;   // one block more for pSetm
  if (r->typ!=NULL)
    omFree((ADDRESS)res->typ);
  res->typ=(sro_ord*)omAlloc0(res->OrdSize*sizeof(sro_ord));
  if (r->typ!=NULL)
    memcpy(res->typ,r->typ,r->OrdSize*sizeof(sro_ord));
  // the additional block for pSetm: total degree at the last word
  // but not included in the compare part
  res->typ[res->OrdSize-1].ord_typ=ro_dp;
  res->typ[res->OrdSize-1].data.dp.start=start_var;
  res->typ[res->OrdSize-1].data.dp.end=end_var;
  res->typ[res->OrdSize-1].data.dp.place=res->ExpL_Size-1;
  pos=res->ExpL_Size-1;
  //if ((start_var==1) && (end_var==res->N)) res->pOrdIndex=pos;
  extern void p_Setm_General(poly p, ring r);
  res->p_Setm=p_Setm_General;
  // ----------------------------
  omFree((ADDRESS)res->p_Procs);
  res->p_Procs = (p_Procs_s*)omAlloc(sizeof(p_Procs_s));

  p_ProcsSet(res, res->p_Procs);
  if (res->qideal!=NULL) id_Delete(&res->qideal,res);
#ifdef HAVE_PLURAL
  r->GetNC()=save;
  if (rIsPluralRing(r))
  {
    if ( nc_rComplete(r, res, false) ) // no qideal!
    {
#ifndef NDEBUG
      WarnS("error in nc_rComplete");
#endif
      // just go on..
    }
  }
#endif
  if (r->qideal!=NULL)
  {
     res->qideal=idrCopyR_NoSort(r->qideal,r, res);
#ifdef HAVE_PLURAL
     if (rIsPluralRing(res))
     {
       nc_SetupQuotient(res, currRing);
     }
     assume((res->qideal==NULL) == (r->qideal==NULL));
#endif
  }

#ifdef HAVE_PLURAL
  assume(rIsPluralRing(res) == rIsPluralRing(r));
  assume(rIsSCA(res) == rIsSCA(r));
  assume(ncRingType(res) == ncRingType(r));
#endif

  return res;
}

ring rAssure_HasComp(ring r)
{
  int last_block;
  int i=0;
  do
  {
     if (r->order[i] == ringorder_c ||
        r->order[i] == ringorder_C) return r;
     if (r->order[i] == 0)
        break;
     i++;
  } while (1);
  //WarnS("re-creating ring with comps");
  last_block=i-1;

  ring new_r = rCopy0(r, FALSE, FALSE);
  i+=2;
  new_r->wvhdl=(int **)omAlloc0(i * sizeof(int_ptr));
  new_r->order   = (int *) omAlloc0(i * sizeof(int));
  new_r->block0   = (int *) omAlloc0(i * sizeof(int));
  new_r->block1   = (int *) omAlloc0(i * sizeof(int));
  memcpy(new_r->order,r->order,(i-1) * sizeof(int));
  memcpy(new_r->block0,r->block0,(i-1) * sizeof(int));
  memcpy(new_r->block1,r->block1,(i-1) * sizeof(int));
  for (int j=0; j<=last_block; j++)
  {
    if (r->wvhdl[j]!=NULL)
    {
      new_r->wvhdl[j] = (int*) omMemDup(r->wvhdl[j]);
    }
  }
  last_block++;
  new_r->order[last_block]=ringorder_C;
  //new_r->block0[last_block]=0;
  //new_r->block1[last_block]=0;
  //new_r->wvhdl[last_block]=NULL;

  rComplete(new_r, 1);

#ifdef HAVE_PLURAL
  if (rIsPluralRing(r))
  {
    if ( nc_rComplete(r, new_r, false) ) // no qideal!
    {
#ifndef NDEBUG
      WarnS("error in nc_rComplete");      // cleanup?//      rDelete(res);//      return r;      // just go on..
#endif
    }
  }
  assume(rIsPluralRing(r) == rIsPluralRing(new_r));
#endif

  return new_r;
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
      if (complete)
      {
        rComplete(new_r, 1);

#ifdef HAVE_PLURAL
        if (rIsPluralRing(r))
        {
          if ( nc_rComplete(r, new_r, false) ) // no qideal!
          {
#ifndef NDEBUG
            WarnS("error in nc_rComplete");   // cleanup?//      rDelete(res);//      return r;      // just go on..
#endif
          }
        }
        assume(rIsPluralRing(r) == rIsPluralRing(new_r));
#endif
      }
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
#ifdef HAVE_PLURAL
      if( rIsPluralRing(new_r) )
        if( nc_SetupQuotient(new_r, old_r, true) )
        {
#ifndef NDEBUG
          WarnS("error in nc_SetupQuotient"); // cleanup?      rDelete(res);       return r;  // just go on...?
#endif
        }
#endif
    }

#ifdef HAVE_PLURAL
    assume((new_r->qideal==NULL) == (old_r->qideal==NULL));
    assume(rIsPluralRing(new_r) == rIsPluralRing(old_r));
    assume(rIsSCA(new_r) == rIsSCA(old_r));
    assume(ncRingType(new_r) == ncRingType(old_r));
#endif

    rTest(new_r);
    rTest(old_r);
  }
  return new_r;
}

// Moves _c or _C ordering to the last place AND adds _s on the 1st place
ring rCurrRingAssure_SyzComp_CompLastBlock()
{
  ring new_r_1 = rAssure_CompLastBlock(currRing, FALSE); // due to this FALSE - no completion!
  ring new_r = rAssure_SyzComp(new_r_1, FALSE); // new_r_1 is used only here!!!

  if (new_r != currRing)
  {
    ring old_r = currRing;
    if (new_r_1 != new_r && new_r_1 != old_r) rDelete(new_r_1);
    rComplete(new_r, 1);
#ifdef HAVE_PLURAL
    if (rIsPluralRing(old_r))
    {
      if ( nc_rComplete(old_r, new_r, false) ) // no qideal!
      {
#ifndef NDEBUG
        WarnS("error in nc_rComplete"); // cleanup?      rDelete(res);       return r;  // just go on...?
#endif
        }
    }
    assume(rIsPluralRing(new_r) == rIsPluralRing(old_r));
#endif
    rChangeCurrRing(new_r);
    if (old_r->qideal != NULL)
    {
      new_r->qideal = idrCopyR(old_r->qideal, old_r);
      currQuotient = new_r->qideal;

#ifdef HAVE_PLURAL
      if( rIsPluralRing(old_r) )
        if( nc_SetupQuotient(new_r, old_r, true) )
        {
#ifndef NDEBUG
          WarnS("error in nc_SetupQuotient"); // cleanup?      rDelete(res);       return r;  // just go on...?
#endif
        }
#endif
    }

#ifdef HAVE_PLURAL
    assume((new_r->qideal==NULL) == (old_r->qideal==NULL));
    assume(rIsPluralRing(new_r) == rIsPluralRing(old_r));
    assume(rIsSCA(new_r) == rIsSCA(old_r));
    assume(ncRingType(new_r) == ncRingType(old_r));
#endif

    rTest(new_r);
    rTest(old_r);
  }
  return new_r;
}

// use this for global orderings consisting of two blocks
static ring rCurrRingAssure_Global(rRingOrder_t b1, rRingOrder_t b2)
{
  int r_blocks = rBlocks(currRing);

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
#ifdef HAVE_PLURAL
  if (rIsPluralRing(currRing))
  {
    if ( nc_rComplete(currRing, res, false) ) // no qideal!
    {
#ifndef NDEBUG
      WarnS("error in nc_rComplete");
#endif
    }
  }
#endif
  rChangeCurrRing(res);
  return res;
}

ring rAssure_InducedSchreyerOrdering(const ring r, BOOLEAN complete = TRUE, int sgn = 1)
{ // TODO: ???? Add leading Syz-comp ordering here...????

#if MYTEST
    Print("rAssure_InducedSchreyerOrdering(r, complete = %d, sgn = %d): r: \n", complete, sgn);
    rWrite(r);
#ifdef RDEBUG
    rDebugPrint(r);
#endif
    PrintLn();
#endif
  assume((sgn == 1) || (sgn == -1));

  ring res=rCopy0(r, FALSE, FALSE); // No qideal & ordering copy.

  int n = rBlocks(r); // Including trailing zero!

  // Create 2 more blocks for prefix/suffix:
  res->order=(int *)omAlloc0((n+2)*sizeof(int)); // 0  ..  n+1
  res->block0=(int *)omAlloc0((n+2)*sizeof(int));
  res->block1=(int *)omAlloc0((n+2)*sizeof(int));
  int ** wvhdl =(int **)omAlloc0((n+2)*sizeof(int**));

  // Encapsulate all existing blocks between induced Schreyer ordering markers: prefix and suffix!
  // Note that prefix and suffix have the same ringorder marker and only differ in block[] parameters!

  // new 1st block
  int j = 0;
  res->order[j] = ringorder_IS; // Prefix
  res->block0[j] = res->block1[j] = 0;
  // wvhdl[j] = NULL;
  j++;

  for(int i = 0; (i <= n) && (r->order[i] != 0); i++, j++) // i = [0 .. n-1] <- non-zero old blocks
  {
    res->order [j] = r->order [i];
    res->block0[j] = r->block0[i];
    res->block1[j] = r->block1[i];

    if (r->wvhdl[i] != NULL)
    {
      wvhdl[j] = (int*) omMemDup(r->wvhdl[i]);
    } // else wvhdl[j] = NULL;
  }

  // new last block
  res->order [j] = ringorder_IS; // Suffix
  res->block0[j] = res->block1[j] = sgn; // Sign of v[o]: 1 for C, -1 for c
  // wvhdl[j] = NULL;
  j++;

  // res->order [j] = 0; // The End!
  res->wvhdl = wvhdl;

  // j == the last zero block now!
  assume(j == (n+1));
  assume(res->order[0]==ringorder_IS);
  assume(res->order[j-1]==ringorder_IS);
  assume(res->order[j]==0);


  if (complete)
  {
    rComplete(res, 1);

#if MYTEST
    PrintS("rAssure_InducedSchreyerOrdering(): temp res: ");
    rWrite(res);
#ifdef RDEBUG
    rDebugPrint(res);
#endif
    PrintLn();
#endif

#ifdef HAVE_PLURAL
    if (rIsPluralRing(r))
    {
      if ( nc_rComplete(r, res, false) ) // no qideal!
      {
#ifndef NDEBUG
        WarnS("error in nc_rComplete");      // cleanup?//      rDelete(res);//      return r;      // just go on..
#endif
      }
    }
    assume(rIsPluralRing(r) == rIsPluralRing(res));
#endif


#ifdef HAVE_PLURAL
    ring old_ring = r;

#if MYTEST
    PrintS("rAssure_InducedSchreyerOrdering(): temp nc res: ");
    rWrite(res);
#ifdef RDEBUG
    rDebugPrint(res);
#endif
    PrintLn();
#endif
#endif

    if (r->qideal!=NULL)
    {
      res->qideal= idrCopyR_NoSort(r->qideal, r, res);

      assume(idRankFreeModule(res->qideal, res) == 0);

#ifdef HAVE_PLURAL
      if( rIsPluralRing(res) )
        if( nc_SetupQuotient(res, r, true) )
        {
//          WarnS("error in nc_SetupQuotient"); // cleanup?      rDelete(res);       return r;  // just go on...?
        }

#endif
      assume(idRankFreeModule(res->qideal, res) == 0);
    }

#ifdef HAVE_PLURAL
    assume((res->qideal==NULL) == (old_ring->qideal==NULL));
    assume(rIsPluralRing(res) == rIsPluralRing(old_ring));
    assume(rIsSCA(res) == rIsSCA(old_ring));
    assume(ncRingType(res) == ncRingType(old_ring));
#endif
  }

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



/// Finds p^th IS ordering, and returns its position in r->typ[]
/// returns -1 if something went wrong!
int rGetISPos(const int p = 0, const ring r = currRing)
{
  // Put the reference set F into the ring -ordering -recor
#if MYTEST
  Print("rIsIS(p: %d)\nF:", p);
  PrintLn();
#endif

  if (r->typ==NULL)
  {
    dReportError("'rIsIS:' Error: wrong ring! (typ == NULL)");
    return -1;
  }

  int j = p; // Which IS record to use...
  for( int pos = 0; pos < r->OrdSize; pos++ )
    if( r->typ[pos].ord_typ == ro_is)
      if( j-- == 0 )
      {
        return pos;
      }

  return -1;
}






/// Changes r by setting induced ordering parameters: limit and reference leading terms
/// F belong to r, we will DO a copy! (same to componentWeights)
/// We will use it AS IS!
/// returns true is everything was allright!
bool rSetISReference(const ideal F, const int i = 0, const int p = 0, const intvec * componentWeights = NULL, const ring r = currRing)
{
  // Put the reference set F into the ring -ordering -recor
#if MYTEST
  Print("rSetISReference(F, i: %d, p: %d, w)\nF:", i, p);
  idShow(F, r, r, 1); // currRing!
  PrintLn();
  PrintS("w: ");
  if(componentWeights == NULL)
    PrintS("NULL\n");
  else
    componentWeights->show();
#endif

  // TEST THAT THERE ARE DEGs!!!
  // assume( componentWeights == NULL  ); // ???
  if( componentWeights != NULL )
  {
//    assure that the ring r has degrees!!!
//    Add weights to degrees of F[i]
  }

  if (r->typ==NULL)
  {
    dReportError("Error: WRONG USE of rSetISReference: wrong ring! (typ == NULL)");
    return false;
  }


  int pos = rGetISPos(p, r);

  if( pos == -1 )
  {
    dReportError("Error: WRONG USE of rSetISReference: specified ordering block was not found!!!" );
    return false;
  }

#if MYTEST
  if( i != r->typ[pos].data.is.limit )
    Print("Changing record on pos: %d\nOld limit: %d --->> New Limit: %d\n", pos, r->typ[pos].data.is.limit, i);
#endif

  const ideal FF = id_Copy(F, r); // idrHeadR(F, r, r);


  if( r->typ[pos].data.is.F != NULL)
  {
#if MYTEST
    PrintS("Deleting old reference set F... \n");        // idShow(r->typ[pos].data.is.F, r);         PrintLn();
#endif
    id_Delete(&r->typ[pos].data.is.F, r);
    r->typ[pos].data.is.F = NULL;
  }

  assume(r->typ[pos].data.is.F == NULL);

  r->typ[pos].data.is.F = FF; // F is owened by ring now! TODO: delete at the end!

  if(r->typ[pos].data.is.componentWeights != NULL)
  {
#if MYTEST
    PrintS("Deleting old componentWeights: "); r->typ[pos].data.is.componentWeights->show(); PrintLn();
#endif
    delete r->typ[pos].data.is.componentWeights;
    r->typ[pos].data.is.componentWeights = NULL;
  }


  assume(r->typ[pos].data.is.componentWeights == NULL);

  if( componentWeights != NULL )
    componentWeights = ivCopy(componentWeights); // componentWeights is owened by ring now! TODO: delete at the end!

  r->typ[pos].data.is.componentWeights = componentWeights;

  r->typ[pos].data.is.limit = i; // First induced component

#if MYTEST
  PrintS("New reference set FF : \n");        idShow(FF, r, r, 1);         PrintLn();
#endif

  return true;
}


void rSetSyzComp(int k)
{
  if(k < 0)
  {
    dReportError("rSetSyzComp with negative limit!");
    return;
  }

  assume( k >= 0 );
  if (TEST_OPT_PROT) Print("{%d}", k);
  if ((currRing->typ!=NULL) && (currRing->typ[0].ord_typ==ro_syz))
  {
    if( k == currRing->typ[0].data.syz.limit )
      return; // nothing to do

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
    if(k < currRing->typ[0].data.syz.limit) // ?
    {
#ifndef NDEBUG
      Warn("rSetSyzComp called with smaller limit (%d) as before (%d)", k, currRing->typ[0].data.syz.limit);
#endif
      currRing->typ[0].data.syz.curr_index = 1 + currRing->typ[0].data.syz.syz_index[k];
    }


    currRing->typ[0].data.syz.limit = k;
    currRing->typ[0].data.syz.curr_index++;
  }
  else if(
            (currRing->typ!=NULL) &&
            (currRing->typ[0].ord_typ==ro_isTemp)
           )
  {
//      (currRing->typ[currRing->typ[0].data.isTemp.suffixpos].data.is.limit == k)
#ifndef NDEBUG
    Warn("rSetSyzComp(%d) in an IS ring! Be careful!", k);
#endif
  }
  else
  if ((currRing->order[0]!=ringorder_c) && (k!=0)) // ???
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
  #ifdef HAVE_RINGS
   if (rField_is_Ring_Z(r)) return n_Z;
   if (rField_is_Ring_ModN(r)) return n_Zm;
   if (rField_is_Ring_PtoM(r)) return n_Zpn;
   if (rField_is_Ring_2toM(r)) return  n_Z2n;
  #endif

  return n_unknown;
}

int64 * rGetWeightVec(ring r)
{
  assume(r!=NULL);
  assume(r->OrdSize>0);
  int i=0;
  while((r->typ[i].ord_typ!=ro_wp64) && (r->typ[i].ord_typ>0)) i++;
  assume(r->typ[i].ord_typ==ro_wp64);
  return (int64*)(r->typ[i].data.wp64.weights64);
}

void rSetWeightVec(ring r, int64 *wv)
{
  assume(r!=NULL);
  assume(r->OrdSize>0);
  assume(r->typ[0].ord_typ==ro_wp64);
  memcpy(r->typ[0].data.wp64.weights64,wv,r->N*sizeof(int64));
}

#include <ctype.h>

static int rRealloc1(ring r, int size, int pos)
{
  r->order=(int*)omReallocSize(r->order, size*sizeof(int), (size+1)*sizeof(int));
  r->block0=(int*)omReallocSize(r->block0, size*sizeof(int), (size+1)*sizeof(int));
  r->block1=(int*)omReallocSize(r->block1, size*sizeof(int), (size+1)*sizeof(int));
  r->wvhdl=(int_ptr*)omReallocSize(r->wvhdl,size*sizeof(int_ptr), (size+1)*sizeof(int_ptr));
  for(int k=size; k>pos; k--) r->wvhdl[k]=r->wvhdl[k-1];
  r->order[size]=0;
  size++;
  return size;
}
//static int rReallocM1(ring r, ring src, int size, int pos)
//{
//  r->order=(int*)omReallocSize(r->order, size*sizeof(int), (size-1)*sizeof(int));
//  r->block0=(int*)omReallocSize(r->block0, size*sizeof(int), (size-1)*sizeof(int));
//  r->block1=(int*)omReallocSize(r->block1, size*sizeof(int), (size-1)*sizeof(int));
//  r->wvhdl=(int_ptr*)omReallocSize(r->wvhdl,size*sizeof(int_ptr), (size-1)*sizeof(int_ptr));
//  for(int k=pos+1; k<size; k++) r->wvhdl[k]=r->wvhdl[k+1];
//  size--;
//  return size;
//}
static void rOppWeight(int *w, int l)
{
  int i2=(l+1)/2;
  for(int j=0; j<=i2; j++)
  {
    int t=w[j];
    w[j]=w[l-j];
    w[l-j]=t;
  }
}

#define rOppVar(R,I) (rVar(R)+1-I)

ring rOpposite(ring src)
  /* creates an opposite algebra of R */
  /* that is R^opp, where f (*^opp) g = g*f  */
  /* treats the case of qring */
{
  if (src == NULL) return(NULL);

#ifdef RDEBUG
  rTest(src);
#endif

  ring save = currRing;
  rChangeCurrRing(src);

#ifdef RDEBUG
  rTest(src);
//  rWrite(src);
//  rDebugPrint(src);
#endif


//  ring r = rCopy0(src,TRUE); /* TRUE for copy the qideal: Why??? */
  ring r = rCopy0(src,FALSE); /* qideal will be deleted later on!!! */

  /*  rChangeCurrRing(r); */
  // change vars v1..vN -> vN..v1
  int i;
  int i2 = (rVar(r)-1)/2;
  for(i=i2; i>=0; i--)
  {
    // index: 0..N-1
    //Print("ex var names: %d <-> %d\n",i,rOppVar(r,i));
    // exchange names
    char *p;
    p = r->names[rVar(r)-1-i];
    r->names[rVar(r)-1-i] = r->names[i];
    r->names[i] = p;
  }
//  i2=(rVar(r)+1)/2;
//  for(int i=i2; i>0; i--)
//  {
//    // index: 1..N
//    //Print("ex var places: %d <-> %d\n",i,rVar(r)+1-i);
//    // exchange VarOffset
//    int t;
//    t=r->VarOffset[i];
//    r->VarOffset[i]=r->VarOffset[rOppVar(r,i)];
//    r->VarOffset[rOppVar(r,i)]=t;
//  }
  // change names:
  for (i=rVar(r)-1; i>=0; i--)
  {
    char *p=r->names[i];
    if(isupper(*p)) *p = tolower(*p);
    else            *p = toupper(*p);
  }
  // change ordering: listing
  // change ordering: compare
//  for(i=0; i<r->OrdSize; i++)
//  {
//    int t,tt;
//    switch(r->typ[i].ord_typ)
//    {
//      case ro_dp:
//      //
//        t=r->typ[i].data.dp.start;
//        r->typ[i].data.dp.start=rOppVar(r,r->typ[i].data.dp.end);
//        r->typ[i].data.dp.end=rOppVar(r,t);
//        break;
//      case ro_wp:
//      case ro_wp_neg:
//      {
//        t=r->typ[i].data.wp.start;
//        r->typ[i].data.wp.start=rOppVar(r,r->typ[i].data.wp.end);
//        r->typ[i].data.wp.end=rOppVar(r,t);
//        // invert r->typ[i].data.wp.weights
//        rOppWeight(r->typ[i].data.wp.weights,
//                   r->typ[i].data.wp.end-r->typ[i].data.wp.start);
//        break;
//      }
//      //case ro_wp64:
//      case ro_syzcomp:
//      case ro_syz:
//         WerrorS("not implemented in rOpposite");
//         // should not happen
//         break;
//
//      case ro_cp:
//        t=r->typ[i].data.cp.start;
//        r->typ[i].data.cp.start=rOppVar(r,r->typ[i].data.cp.end);
//        r->typ[i].data.cp.end=rOppVar(r,t);
//        break;
//      case ro_none:
//      default:
//       Werror("unknown type in rOpposite(%d)",r->typ[i].ord_typ);
//       break;
//    }
//  }
  // Change order/block structures (needed for rPrint, rAdd etc.)
  int j=0;
  int l=rBlocks(src);
  for(i=0; src->order[i]!=0; i++)
  {
    switch (src->order[i])
    {
      case ringorder_c: /* c-> c */
      case ringorder_C: /* C-> C */
      case ringorder_no /*=0*/: /* end-of-block */
        r->order[j]=src->order[i];
        j++; break;
      case ringorder_lp: /* lp -> rp */
        r->order[j]=ringorder_rp;
        r->block0[j]=rOppVar(r, src->block1[i]);
        r->block1[j]=rOppVar(r, src->block0[i]);
        break;
      case ringorder_rp: /* rp -> lp */
        r->order[j]=ringorder_lp;
        r->block0[j]=rOppVar(r, src->block1[i]);
        r->block1[j]=rOppVar(r, src->block0[i]);
        break;
      case ringorder_dp: /* dp -> a(1..1),ls */
      {
        l=rRealloc1(r,l,j);
        r->order[j]=ringorder_a;
        r->block0[j]=rOppVar(r, src->block1[i]);
        r->block1[j]=rOppVar(r, src->block0[i]);
        r->wvhdl[j]=(int*)omAlloc((r->block1[j]-r->block0[j]+1)*sizeof(int));
        for(int k=r->block0[j]; k<=r->block1[j]; k++)
          r->wvhdl[j][k-r->block0[j]]=1;
        j++;
        r->order[j]=ringorder_ls;
        r->block0[j]=rOppVar(r, src->block1[i]);
        r->block1[j]=rOppVar(r, src->block0[i]);
        j++;
        break;
      }
      case ringorder_Dp: /* Dp -> a(1..1),rp */
      {
        l=rRealloc1(r,l,j);
        r->order[j]=ringorder_a;
        r->block0[j]=rOppVar(r, src->block1[i]);
        r->block1[j]=rOppVar(r, src->block0[i]);
        r->wvhdl[j]=(int*)omAlloc((r->block1[j]-r->block0[j]+1)*sizeof(int));
        for(int k=r->block0[j]; k<=r->block1[j]; k++)
          r->wvhdl[j][k-r->block0[j]]=1;
        j++;
        r->order[j]=ringorder_rp;
        r->block0[j]=rOppVar(r, src->block1[i]);
        r->block1[j]=rOppVar(r, src->block0[i]);
        j++;
        break;
      }
      case ringorder_wp: /* wp -> a(...),ls */
      {
        l=rRealloc1(r,l,j);
        r->order[j]=ringorder_a;
        r->block0[j]=rOppVar(r, src->block1[i]);
        r->block1[j]=rOppVar(r, src->block0[i]);
        r->wvhdl[j]=r->wvhdl[j+1]; r->wvhdl[j+1]=NULL;
        rOppWeight(r->wvhdl[j], r->block1[j]-r->block0[j]);
        j++;
        r->order[j]=ringorder_ls;
        r->block0[j]=rOppVar(r, src->block1[i]);
        r->block1[j]=rOppVar(r, src->block0[i]);
        j++;
        break;
      }
      case ringorder_Wp: /* Wp -> a(...),rp */
      {
        l=rRealloc1(r,l,j);
        r->order[j]=ringorder_a;
        r->block0[j]=rOppVar(r, src->block1[i]);
        r->block1[j]=rOppVar(r, src->block0[i]);
        r->wvhdl[j]=r->wvhdl[j+1]; r->wvhdl[j+1]=NULL;
        rOppWeight(r->wvhdl[j], r->block1[j]-r->block0[j]);
        j++;
        r->order[j]=ringorder_rp;
        r->block0[j]=rOppVar(r, src->block1[i]);
        r->block1[j]=rOppVar(r, src->block0[i]);
        j++;
        break;
      }
      case ringorder_M: /* M -> M */
      {
        r->order[j]=ringorder_M;
        r->block0[j]=rOppVar(r, src->block1[i]);
        r->block1[j]=rOppVar(r, src->block0[i]);
        int n=r->block1[j]-r->block0[j];
        /* M is a (n+1)x(n+1) matrix */
        for (int nn=0; nn<=n; nn++)
        {
          rOppWeight(&(r->wvhdl[j][nn*(n+1)]), n /*r->block1[j]-r->block0[j]*/);
        }
        j++;
        break;
      }
      case ringorder_a: /*  a(...),ls -> wp/dp */
      {
        r->block0[j]=rOppVar(r, src->block1[i]);
        r->block1[j]=rOppVar(r, src->block0[i]);
        rOppWeight(r->wvhdl[j], r->block1[j]-r->block0[j]);
        if (src->order[i+1]==ringorder_ls)
        {
          r->order[j]=ringorder_wp;
          i++;
          //l=rReallocM1(r,src,l,j);
        }
        else
        {
          r->order[j]=ringorder_a;
        }
        j++;
        break;
      }
      // not yet done:
      case ringorder_ls:
      case ringorder_rs:
      case ringorder_ds:
      case ringorder_Ds:
      case ringorder_ws:
      case ringorder_Ws:
      // should not occur:
      case ringorder_S:
      case ringorder_IS:
      case ringorder_s:
      case ringorder_aa:
      case ringorder_L:
      case ringorder_unspec:
        Werror("order %s not (yet) supported", rSimpleOrdStr(src->order[i]));
        break;
    }
  }
  rComplete(r);


#ifdef RDEBUG
  rTest(r);
#endif

  rChangeCurrRing(r);

#ifdef RDEBUG
  rTest(r);
//  rWrite(r);
//  rDebugPrint(r);
#endif


#ifdef HAVE_PLURAL
  // now, we initialize a non-comm structure on r
  if (rIsPluralRing(src))
  {
    assume( currRing == r);

    int *perm       = (int *)omAlloc0((rVar(r)+1)*sizeof(int));
    int *par_perm   = NULL;
    nMapFunc nMap   = nSetMap(src);
    int ni,nj;
    for(i=1; i<=r->N; i++)
    {
      perm[i] = rOppVar(r,i);
    }

    matrix C = mpNew(rVar(r),rVar(r));
    matrix D = mpNew(rVar(r),rVar(r));

    for (i=1; i< rVar(r); i++)
    {
      for (j=i+1; j<=rVar(r); j++)
      {
        ni = r->N +1 - i;
        nj = r->N +1 - j; /* i<j ==>   nj < ni */

        assume(MATELEM(src->GetNC()->C,i,j) != NULL);
        MATELEM(C,nj,ni) = pPermPoly(MATELEM(src->GetNC()->C,i,j),perm,src,nMap,par_perm,src->P);

        if(MATELEM(src->GetNC()->D,i,j) != NULL)
          MATELEM(D,nj,ni) = pPermPoly(MATELEM(src->GetNC()->D,i,j),perm,src,nMap,par_perm,src->P);
      }
    }

    idTest((ideal)C);
    idTest((ideal)D);

    if (nc_CallPlural(C, D, NULL, NULL, r, false, false, true, r)) // no qring setup!
      WarnS("Error initializing non-commutative multiplication!");

#ifdef RDEBUG
    rTest(r);
//    rWrite(r);
//    rDebugPrint(r);
#endif

    assume( r->GetNC()->IsSkewConstant == src->GetNC()->IsSkewConstant);

    omFreeSize((ADDRESS)perm,(rVar(r)+1)*sizeof(int));
  }
#endif /* HAVE_PLURAL */

  /* now oppose the qideal for qrings */
  if (src->qideal != NULL)
  {
    id_Delete(&(r->qideal), r);

#ifdef HAVE_PLURAL
    r->qideal = idOppose(src, src->qideal); // into the currRing: r
#else
    r->qideal = id_Copy(src->qideal, currRing); // ?
#endif

#ifdef HAVE_PLURAL
    if( rIsPluralRing(r) )
    {
      nc_SetupQuotient(r);
#ifdef RDEBUG
      rTest(r);
//      rWrite(r);
//      rDebugPrint(r);
#endif
    }
#endif
  }
#ifdef HAVE_PLURAL
  if( rIsPluralRing(r) )
    assume( ncRingType(r) == ncRingType(src) );
#endif
  rTest(r);

  rChangeCurrRing(save);
  return r;
}

ring rEnvelope(ring R)
  /* creates an enveloping algebra of R */
  /* that is R^e = R \tensor_K R^opp */
{
  ring Ropp = rOpposite(R);
  ring Renv = NULL;
  int stat = rSum(R, Ropp, Renv); /* takes care of qideals */
  if ( stat <=0 )
    WarnS("Error in rEnvelope at rSum");
  rTest(Renv);
  return Renv;
}

#ifdef HAVE_PLURAL
BOOLEAN nc_rComplete(const ring src, ring dest, bool bSetupQuotient)
/* returns TRUE is there were errors */
/* dest is actualy equals src with the different ordering */
/* we map src->nc correctly to dest->src */
/* to be executed after rComplete, before rChangeCurrRing */
{
// NOTE: Originally used only by idElimination to transfer NC structure to dest
// ring created by dirty hack (without nc_CallPlural)
  rTest(src);

  assume(!rIsPluralRing(dest)); // destination must be a newly constructed commutative ring

  if (!rIsPluralRing(src))
  {
    return FALSE;
  }

  const int N = dest->N;

  assume(src->N == N);

  ring save = currRing;

  if (dest != save)
    rChangeCurrRing(dest);

  const ring srcBase = src;

  assume( nSetMap(srcBase) == nSetMap(currRing) ); // currRing is important here!

  matrix C = mpNew(N,N); // ring independent
  matrix D = mpNew(N,N);

  matrix C0 = src->GetNC()->C;
  matrix D0 = src->GetNC()->D;

  // map C and D into dest
  for (int i = 1; i < N; i++)
  {
    for (int j = i + 1; j <= N; j++)
    {
      const number n = n_Copy(p_GetCoeff(MATELEM(C0,i,j), srcBase), srcBase); // src, mapping for coeffs into currRing = dest!
      const poly   p = p_NSet(n, dest);
      MATELEM(C,i,j) = p;
      if (MATELEM(D0,i,j) != NULL)
        MATELEM(D,i,j) = prCopyR(MATELEM(D0,i,j), srcBase, dest); // ?
    }
  }
  /* One must test C and D _only_ in r->GetNC()->basering!!! not in r!!! */

  idTest((ideal)C); // in dest!
  idTest((ideal)D);

  if (nc_CallPlural(C, D, NULL, NULL, dest, bSetupQuotient, false, true, dest)) // also takes care about quotient ideal
  {
    //WarnS("Error transferring non-commutative structure");
    // error message should be in the interpreter interface

    mpDelete(&C, dest);
    mpDelete(&D, dest);

    if (currRing != save)
       rChangeCurrRing(save);

    return TRUE;
  }

//  mpDelete(&C, dest); // used by nc_CallPlural!
//  mpDelete(&D, dest);

  if (dest != save)
    rChangeCurrRing(save);

  assume(rIsPluralRing(dest));
  return FALSE;
}
#endif

void rModify_a_to_A(ring r)
// to be called BEFORE rComplete:
// changes every Block with a(...) to A(...)
{
   int i=0;
   int j;
   while(r->order[i]!=0)
   {
      if (r->order[i]==ringorder_a)
      {
        r->order[i]=ringorder_a64;
        int *w=r->wvhdl[i];
        int64 *w64=(int64 *)omAlloc((r->block1[i]-r->block0[i]+1)*sizeof(int64));
        for(j=r->block1[i]-r->block0[i];j>=0;j--)
                w64[j]=(int64)w[j];
        r->wvhdl[i]=(int*)w64;
        omFreeSize(w,(r->block1[i]-r->block0[i]+1)*sizeof(int));
      }
      i++;
   }
}
