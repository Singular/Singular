/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/

/*
* ABSTRACT: interpreter:
*           assignment of expressions and lists to objects or lists
*/

#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include <kernel/mod2.h>
#include <Singular/tok.h>
#include <kernel/options.h>
#include <Singular/ipid.h>
#include <kernel/idrec.h>
#include <kernel/intvec.h>
#include <omalloc/omalloc.h>
#include <kernel/febase.h>
#include <kernel/polys.h>
#include <kernel/ideals.h>
#include <kernel/matpol.h>
#include <kernel/kstd1.h>
#include <kernel/timer.h>
#include <kernel/ring.h>
#include <Singular/subexpr.h>
#include <Singular/lists.h>
#include <kernel/numbers.h>
#include <kernel/longalg.h>
#include <kernel/stairc.h>
#include <kernel/maps.h>
#include <kernel/syz.h>
//#include "weight.h"
#include <Singular/ipconv.h>
#include <Singular/attrib.h>
#include <Singular/silink.h>
#include <Singular/ipshell.h>
#include <kernel/sca.h>
#include <Singular/blackbox.h>

/*=================== proc =================*/
static BOOLEAN jjECHO(leftv res, leftv a)
{
  si_echo=(int)((long)(a->Data()));
  return FALSE;
}
static BOOLEAN jjPRINTLEVEL(leftv res, leftv a)
{
  printlevel=(int)((long)(a->Data()));
  return FALSE;
}
static BOOLEAN jjCOLMAX(leftv res, leftv a)
{
  colmax=(int)((long)(a->Data()));
  return FALSE;
}
static BOOLEAN jjTIMER(leftv res, leftv a)
{
  timerv=(int)((long)(a->Data()));
  initTimer();
  return FALSE;
}
#ifdef HAVE_GETTIMEOFDAY
static BOOLEAN jjRTIMER(leftv res, leftv a)
{
  rtimerv=(int)((long)(a->Data()));
  initRTimer();
  return FALSE;
}
#endif
static BOOLEAN jjMAXDEG(leftv res, leftv a)
{
  Kstd1_deg=(int)((long)(a->Data()));
  if (Kstd1_deg!=0)
    test |=Sy_bit(OPT_DEGBOUND);
  else
    test &=(~Sy_bit(OPT_DEGBOUND));
  return FALSE;
}
static BOOLEAN jjMAXMULT(leftv res, leftv a)
{
  Kstd1_mu=(int)((long)(a->Data()));
  if (Kstd1_mu!=0)
    test |=Sy_bit(OPT_MULTBOUND);
  else
    test &=(~Sy_bit(OPT_MULTBOUND));
  return FALSE;
}
static BOOLEAN jjTRACE(leftv res, leftv a)
{
  traceit=(int)((long)(a->Data()));
  return FALSE;
}
static BOOLEAN jjSHORTOUT(leftv res, leftv a)
{
  if (currRing != NULL)
  {
    BOOLEAN shortOut = (BOOLEAN)((long)a->Data());
#if HAVE_CAN_SHORT_OUT
    if (!shortOut)
      currRing->ShortOut = 0;
    else
    {
      if (currRing->CanShortOut)
        currRing->ShortOut = 1;
    }
#else
    currRing->ShortOut = shortOut;
#endif
  }
  return FALSE;
}
static void jjMINPOLY_red(idhdl h)
{
  switch(IDTYP(h))
  {
    case NUMBER_CMD:
    {
      number n=(number)IDDATA(h);
      number one = nInit(1);
      number nn=nMult(n,one);
      nDelete(&n);nDelete(&one);
      IDDATA(h)=(char*)nn;
      break;
    }
    case VECTOR_CMD:
    case POLY_CMD:
    {
      poly p=(poly)IDDATA(h);
      IDDATA(h)=(char*)pMinPolyNormalize(p);
      break;
    }
    case IDEAL_CMD:
    case MODUL_CMD:
    case MAP_CMD:
    case MATRIX_CMD:
    {
      int i;
      ideal I=(ideal)IDDATA(h);
      for(i=IDELEMS(I)-1;i>=0;i--) I->m[i]=pMinPolyNormalize(I->m[i]);
      break;
    }
    case LIST_CMD:
    {
      lists L=(lists)IDDATA(h);
      int i=L->nr;
      for(;i>=0;i--)
      {
        jjMINPOLY_red((idhdl)&(L->m[i]));
      }
    }
    default:
    //case RESOLUTION_CMD:
       Werror("type %d too complex...set minpoly before",IDTYP(h)); break;
  }
}
static BOOLEAN jjMINPOLY(leftv res, leftv a)
{
  number p=(number)a->CopyD(NUMBER_CMD);
  if (nIsZero(p))
  {
    currRing->minpoly=NULL;
    naMinimalPoly=NULL;
  }
  else
  {
    if ((rPar(currRing)!=1)
      || (rField_is_GF()))
    {
      WerrorS("no minpoly allowed");
      return TRUE;
    }
    if (currRing->minpoly!=NULL)
    {
      WerrorS("minpoly already set");
      return TRUE;
    }
    nNormalize(p);
    currRing->minpoly=p;
    naMinimalPoly=((lnumber)currRing->minpoly)->z;
    if (p_GetExp(((lnumber)currRing->minpoly)->z,1,currRing->algring)==0)
    {
      Werror("minpoly must not be constant");
      currRing->minpoly=NULL;
      naMinimalPoly=NULL;
      nDelete(&p);
    }
    /* redefine function pointers due to switch from
       transcendental to algebraic field extension */
    //redefineFunctionPointers();
    // and now, normalize all already defined objects in this ring
    idhdl h=currRing->idroot;
    while(h!=NULL)
    {
      jjMINPOLY_red(h);
      h=IDNEXT(h);
    }
  }
  return FALSE;
}
static BOOLEAN jjNOETHER(leftv res, leftv a)
{
  poly p=(poly)a->CopyD(POLY_CMD);
  pDelete(&ppNoether);
  ppNoether=p;
  return FALSE;
}
/*=================== proc =================*/
static void jiAssignAttr(leftv l,leftv r)
{
  // get the attribute of th right side
  // and set it to l
  leftv rv=r->LData();
  if (rv!=NULL)
  {
    if (rv->e==NULL)
    {
      if (rv->attribute!=NULL)
      {
        attr la;
        if (r->rtyp!=IDHDL)
        {
          la=rv->attribute;
          rv->attribute=NULL;
        }
        else
        {
          la=rv->attribute->Copy();
        }
        l->attribute=la;
      }
      l->flag=rv->flag;
    }
  }
  if (l->rtyp==IDHDL)
  {
    idhdl h=(idhdl)l->data;
    IDATTR(h)=l->attribute;
    IDFLAG(h)=l->flag;
  }
}
static BOOLEAN jiA_INT(leftv res, leftv a, Subexpr e)
{
  if (e==NULL)
  {
    res->data=(void *)a->Data();
    jiAssignAttr(res,a);
  }
  else
  {
    int i=e->start-1;
    if (i<0)
    {
      Werror("index[%d] must be positive",i+1);
      return TRUE;
    }
    intvec *iv=(intvec *)res->data;
    if (e->next==NULL)
    {
      if (i>=iv->length())
      {
        intvec *iv1=new intvec(i+1);
        (*iv1)[i]=(int)((long)(a->Data()));
        intvec *ivn=ivAdd(iv,iv1);
        delete iv;
        delete iv1;
        res->data=(void *)ivn;
      }
      else
        (*iv)[i]=(int)((long)(a->Data()));
    }
    else
    {
      int c=e->next->start;
      if ((i>=iv->rows())||(c<1)||(c>iv->cols()))
      {
        Werror("wrong range [%d,%d] in intmat (%d,%d)",i+1,c,iv->rows(),iv->cols());
        return TRUE;
      }
      else
        IMATELEM(*iv,i+1,c) = (int)((long)(a->Data()));
    }
  }
  return FALSE;
}
static BOOLEAN jiA_NUMBER(leftv res, leftv a, Subexpr e)
{
  number p=(number)a->CopyD(NUMBER_CMD);
  if (res->data!=NULL) nDelete((number *)&res->data);
  nNormalize(p);
  res->data=(void *)p;
  jiAssignAttr(res,a);
  return FALSE;
}
static BOOLEAN jiA_BIGINT(leftv res, leftv a, Subexpr e)
{
  number p=(number)a->CopyD(BIGINT_CMD);
  if (res->data!=NULL) nlDelete((number *)&res->data,NULL);
  res->data=(void *)p;
  jiAssignAttr(res,a);
  return FALSE;
}
static BOOLEAN jiA_LIST_RES(leftv res, leftv a,Subexpr e)
{
  syStrategy r=(syStrategy)a->CopyD(RESOLUTION_CMD);
  if (res->data!=NULL) ((lists)res->data)->Clean();
  int add_row_shift = 0;
  intvec *weights=(intvec*)atGet(a,"isHomog",INTVEC_CMD);
  if (weights!=NULL)  add_row_shift=weights->min_in();
  res->data=(void *)syConvRes(r,TRUE,add_row_shift);
  //jiAssignAttr(res,a);
  return FALSE;
}
static BOOLEAN jiA_LIST(leftv res, leftv a,Subexpr e)
{
  lists l=(lists)a->CopyD(LIST_CMD);
  if (res->data!=NULL) ((lists)res->data)->Clean();
  res->data=(void *)l;
  jiAssignAttr(res,a);
  return FALSE;
}
static BOOLEAN jiA_POLY(leftv res, leftv a,Subexpr e)
{
  poly p=(poly)a->CopyD(POLY_CMD);
  pNormalize(p);
  if (e==NULL)
  {
    if (res->data!=NULL) pDelete((poly*)&res->data);
    res->data=(void*)p;
    jiAssignAttr(res,a);
    if (TEST_V_QRING && (currQuotient!=NULL) && (!hasFlag(res,FLAG_QRING))) jjNormalizeQRingP(res);
  }
  else
  {
    int i,j;
    matrix m=(matrix)res->data;
    i=e->start;
    if (e->next==NULL)
    {
      j=i; i=1;
      // for all ideal like data types: check indices
      if (j>MATCOLS(m))
      {
        pEnlargeSet(&(m->m),MATCOLS(m),j-MATCOLS(m));
        MATCOLS(m)=j;
      }
      else if (j<=0)
      {
        Werror("index[%d] must be positive",j/*e->start*/);
        return TRUE;
      }
    }
    else
    {
      // for matrices: indices are correct (see ipExprArith3(..,'['..) )
      j=e->next->start;
    }
    pDelete(&MATELEM(m,i,j));
    MATELEM(m,i,j)=p;
    /* for module: update rank */
    if ((p!=NULL) && (pGetComp(p)!=0))
    {
      m->rank=si_max(m->rank,pMaxComp(p));
    }
    if (TEST_V_QRING) jjNormalizeQRingP(res);
  }
  return FALSE;
}
static BOOLEAN jiA_1x1INTMAT(leftv res, leftv a,Subexpr e)
{
  if ((res->rtyp!=INTMAT_CMD) /*|| (e!=NULL) - TRUE because of type int */)
  {
    // no error message: assignment simply fails
    return TRUE;
  }
  intvec* am=(intvec*)a->CopyD(INTMAT_CMD);
  if ((am->rows()!=1) || (am->cols()!=1))
  {
    WerrorS("must be 1x1 intmat");
    delete am;
    return TRUE;
  }
  intvec* m=(intvec *)res->data;
  // indices are correct (see ipExprArith3(..,'['..) )
  int i=e->start;
  int j=e->next->start;
  IMATELEM(*m,i,j)=IMATELEM(*am,1,1);
  delete am;
  return FALSE;
}
static BOOLEAN jiA_1x1MATRIX(leftv res, leftv a,Subexpr e)
{
  if ((res->rtyp!=MATRIX_CMD) /*|| (e!=NULL) - TRUE because of type poly */)
  {
    // no error message: assignment simply fails
    return TRUE;
  }
  matrix am=(matrix)a->CopyD(MATRIX_CMD);
  if ((MATROWS(am)!=1) || (MATCOLS(am)!=1))
  {
    WerrorS("must be 1x1 matrix");
    idDelete((ideal *)&am);
    return TRUE;
  }
  matrix m=(matrix)res->data;
  // indices are correct (see ipExprArith3(..,'['..) )
  int i=e->start;
  int j=e->next->start;
  pDelete(&MATELEM(m,i,j));
  pNormalize(MATELEM(am,1,1));
  MATELEM(m,i,j)=MATELEM(am,1,1);
  MATELEM(am,1,1)=NULL;
  idDelete((ideal *)&am);
  return FALSE;
}
static BOOLEAN jiA_STRING(leftv res, leftv a, Subexpr e)
{
  if (e==NULL)
  {
    void* tmp = res->data;
    res->data=(void *)a->CopyD(STRING_CMD);
    jiAssignAttr(res,a);
    omfree(tmp);
  }
  else
  {
    char *s=(char *)res->data;
    if ((e->start>0)&&(e->start<=(int)strlen(s)))
      s[e->start-1]=(char)(*((char *)a->Data()));
    else
    {
      Werror("string index %d out of range 1..%d",e->start,(int)strlen(s));
      return TRUE;
    }
  }
  return FALSE;
}
static BOOLEAN jiA_PROC(leftv res, leftv a, Subexpr e)
{
  extern procinfo *iiInitSingularProcinfo(procinfo *pi, const char *libname,
                                          const char *procname, int line,
                                          long pos, BOOLEAN pstatic=FALSE);
  extern void piCleanUp(procinfov pi);

  if(res->data!=NULL) piCleanUp((procinfo *)res->data);
  if(a->rtyp==STRING_CMD)
  {
    res->data = (void *)omAlloc0Bin(procinfo_bin);
    ((procinfo *)(res->data))->language=LANG_NONE;
    iiInitSingularProcinfo((procinfo *)res->data,"",res->name,0,0);
    ((procinfo *)res->data)->data.s.body=(char *)a->CopyD(STRING_CMD);
  }
  else
    res->data=(void *)a->CopyD();
  jiAssignAttr(res,a);
  return FALSE;
}
static BOOLEAN jiA_INTVEC(leftv res, leftv a, Subexpr e)
{
  //if ((res->data==NULL) || (res->Typ()==a->Typ()))
  {
    if (res->data!=NULL) delete ((intvec *)res->data);
    res->data=(void *)a->CopyD(INTVEC_CMD);
    jiAssignAttr(res,a);
    return FALSE;
  }
#if 0
  else
  {
    intvec *r=(intvec *)(res->data);
    intvec *s=(intvec *)(a->Data());
    int i=si_min(r->length(), s->length())-1;
    for(;i>=0;i--)
    {
      (*r)[i]=(*s)[i];
    }
    return FALSE; //(r->length()< s->length());
  }
#endif
}
static BOOLEAN jiA_IDEAL(leftv res, leftv a, Subexpr e)
{
  if (res->data!=NULL) idDelete((ideal*)&res->data);
  res->data=(void *)a->CopyD(MATRIX_CMD);
  if (a->rtyp==IDHDL) idNormalize((ideal)a->Data());
  else                idNormalize((ideal)res->data);
  jiAssignAttr(res,a);
  if (((res->rtyp==IDEAL_CMD)||(res->rtyp==MODUL_CMD))
  && (IDELEMS((ideal)(res->data))==1)
  && (currRing->qideal==NULL)
  && (!rIsPluralRing(currRing))
  )
  {
    setFlag(res,FLAG_STD);
  }
  if (TEST_V_QRING && (currQuotient!=NULL)&& (!hasFlag(res,FLAG_QRING))) jjNormalizeQRingId(res);
  return FALSE;
}
static BOOLEAN jiA_RESOLUTION(leftv res, leftv a, Subexpr e)
{
  if (res->data!=NULL) syKillComputation((syStrategy)res->data);
  res->data=(void *)a->CopyD(RESOLUTION_CMD);
  jiAssignAttr(res,a);
  return FALSE;
}
static BOOLEAN jiA_MODUL_P(leftv res, leftv a, Subexpr e)
{
  if (res->data!=NULL) idDelete((ideal*)&res->data);
  ideal I=idInit(1,1);
  I->m[0]=(poly)a->CopyD(POLY_CMD);
  if (I->m[0]!=NULL) pSetCompP(I->m[0],1);
  pNormalize(I->m[0]);
  res->data=(void *)I;
  if (TEST_V_QRING && (currQuotient!=NULL))
  {
    if (hasFlag(a,FLAG_QRING)) setFlag(res,FLAG_QRING);
    else                       jjNormalizeQRingId(res);
  }
  return FALSE;
}
static BOOLEAN jiA_IDEAL_M(leftv res, leftv a, Subexpr e)
{
  if (res->data!=NULL) idDelete((ideal*)&res->data);
  matrix m=(matrix)a->CopyD(MATRIX_CMD);
  IDELEMS((ideal)m)=MATROWS(m)*MATCOLS(m);
  ((ideal)m)->rank=1;
  MATROWS(m)=1;
  idNormalize((ideal)m);
  res->data=(void *)m;
  if (TEST_V_QRING && (currQuotient!=NULL)) jjNormalizeQRingId(res);
  return FALSE;
}
static BOOLEAN jiA_LINK(leftv res, leftv a, Subexpr e)
{
  si_link l=(si_link)res->data;

  if (l!=NULL) slCleanUp(l);

  if (a->Typ() == STRING_CMD)
  {
    if (l == NULL)
    {
      l = (si_link) omAlloc0Bin(sip_link_bin);
      res->data = (void *) l;
    }
    return slInit(l, (char *) a->Data());
  }
  else if (a->Typ() == LINK_CMD)
  {
    if (l != NULL) omFreeBin(l, sip_link_bin);
    res->data = slCopy((si_link)a->Data());
    return FALSE;
  }
  return TRUE;
}
// assign map -> map
static BOOLEAN jiA_MAP(leftv res, leftv a, Subexpr e)
{
  if (res->data!=NULL)
  {
    omFree((ADDRESS)((map)res->data)->preimage);
    ((map)res->data)->preimage=NULL;
    idDelete((ideal*)&res->data);
  }
  res->data=(void *)a->CopyD(MAP_CMD);
  jiAssignAttr(res,a);
  return FALSE;
}
// assign ideal -> map
static BOOLEAN jiA_MAP_ID(leftv res, leftv a, Subexpr e)
{
  map f=(map)res->data;
  char *rn=f->preimage; // save the old/already assigned preimage ring name
  f->preimage=NULL;
  idDelete((ideal *)&f);
  res->data=(void *)a->CopyD(IDEAL_CMD);
  f=(map)res->data;
  idNormalize((ideal)f);
  f->preimage = rn;
  return FALSE;
}
static BOOLEAN jiA_QRING(leftv res, leftv a,Subexpr e)
{
  // the follwing can only happen, if:
  //   - the left side is of type qring AND not an id
  if ((e!=NULL)||(res->rtyp!=IDHDL))
  {
    WerrorS("qring_id expected");
    return TRUE;
  }

  ring qr;
  //qr=(ring)res->Data();
  //if (qr!=NULL) omFreeBin((ADDRESS)qr, ip_sring_bin);
  assume(res->Data()==NULL);
  qr=rCopy(currRing);
                 // we have to fill it, but the copy also allocates space
  idhdl h=(idhdl)res->data; // we have res->rtyp==IDHDL
  IDRING(h)=qr;

  ideal id=(ideal)a->CopyD(IDEAL_CMD);

  if ((idElem(id)>1) || rIsSCA(currRing) || (currRing->qideal!=NULL))
    assumeStdFlag(a);

#ifdef HAVE_RINGS
  if (rField_is_Ring(currRing))
  {
    if (idPosConstant(id) != -1)
    {
      WerrorS("constant in q-ideal; please modify ground field/ring instead");
      return TRUE;
    }
  }
#endif

  if (currRing->qideal!=NULL) /* we are already in a qring! */
  {
    ideal tmp=idSimpleAdd(id,currRing->qideal);
    // both ideals should be GB, so dSimpleAdd is sufficient
    idDelete(&id);
    id=tmp;
    // delete the qr copy of quotient ideal!!!
    idDelete(&qr->qideal);
  }
  qr->qideal = id;

  // qr is a copy of currRing with the new qideal!
  #ifdef HAVE_PLURAL
  if(rIsPluralRing(currRing))
  {
    if (!hasFlag(a,FLAG_TWOSTD))
    {
      Warn("%s is no twosided standard basis",a->Name());
    }

    if( nc_SetupQuotient(qr, currRing) )
    {
//      WarnS("error in nc_SetupQuotient");
    }
  }
  #endif
  rSetHdl((idhdl)res->data);
  return FALSE;
}

static BOOLEAN jiA_RING(leftv res, leftv a, Subexpr e)
{
  BOOLEAN have_id=TRUE;
  if ((e!=NULL)||(res->rtyp!=IDHDL))
  {
    //WerrorS("id expected");
    //return TRUE;
    have_id=FALSE;
  }
  ring r=(ring)a->Data();
  if (have_id)
  {
    idhdl rl=(idhdl)res->data;
    if (IDRING(rl)!=NULL) rKill(rl);
    IDRING(rl)=r;
    if ((IDLEV((idhdl)a->data)!=myynest) && (r==currRing))
      currRingHdl=(idhdl)res->data;
  }
  else
  {
    if (e==NULL) res->data=(char *)r;
    else
    {
      WerrorS("id expected");
      return TRUE;
    }
  }
  r->ref++;
  jiAssignAttr(res,a);
  return FALSE;
}
static BOOLEAN jiA_PACKAGE(leftv res, leftv a, Subexpr e)
{
  res->data=(void *)a->CopyD(PACKAGE_CMD);
  jiAssignAttr(res,a);
  return FALSE;
}
static BOOLEAN jiA_DEF(leftv res, leftv a, Subexpr e)
{
  res->data=(void *)0;
  return FALSE;
}
/*=================== table =================*/
#define IPASSIGN
#define D(A) A
#include <Singular/table.h>
/*=================== operations ============================*/
/*2
* assign a = b
*/
static BOOLEAN jiAssign_1(leftv l, leftv r)
{
  int rt=r->Typ();
  if (rt==0)
  {
    if (!errorreported) Werror("`%s` is undefined",r->Fullname());
    return TRUE;
  }

  int lt=l->Typ();
  if((lt==0)/*&&(l->name!=NULL)*/)
  {
    if (!errorreported) Werror("left side `%s` is undefined",l->Fullname());
    return TRUE;
  }
  if(rt==NONE)
  {
    WarnS("right side is not a datum, assignment ignored");
    // if (!errorreported)
    //   WerrorS("right side is not a datum");
    //return TRUE;
    return FALSE;
  }

  int i=0;
  BOOLEAN nok=FALSE;

  if (lt==DEF_CMD)
  {
    if (l->rtyp==IDHDL)
    {
      IDTYP((idhdl)l->data)=rt;
    }
    else if (l->name!=NULL)
    {
      sleftv ll;
      iiDeclCommand(&ll,l,myynest,rt,&IDROOT);
      memcpy(l,&ll,sizeof(sleftv));
    }
    else
    {
      l->rtyp=rt;
    }
    lt=rt;
  }
  else
  {
    if ((l->data==r->data)&&(l->e==NULL)&&(r->e==NULL))
      return FALSE;
  }
  leftv ld=l;
  if ((l->rtyp==IDHDL)&&(lt!=QRING_CMD)&&(lt!=RING_CMD))
    ld=(leftv)l->data;
  if (lt>MAX_TOK)
  {
    blackbox *bb=getBlackboxStuff(lt);
#ifdef BLACKBOX_DEVEL
    Print("bb-assign: bb=%lx\n",bb);
#endif
    return (bb==NULL) || bb->blackbox_Assign(l,r);
  }
  while (((dAssign[i].res!=lt)
      || (dAssign[i].arg!=rt))
    && (dAssign[i].res!=0)) i++;
  if (dAssign[i].res!=0)
  {
    if (TEST_V_ALLWARN) Print("assign %s=%s\n",Tok2Cmdname(lt),Tok2Cmdname(rt));
    BOOLEAN b;
    b=dAssign[i].p(ld,r,l->e);
    if(l!=ld) /* i.e. l is IDHDL, l->data is ld */
    {
      l->flag=ld->flag;
      l->attribute=ld->attribute;
    }
    return b;
  }
  // implicite type conversion ----------------------------------------------
  if (dAssign[i].res==0)
  {
    int ri;
    leftv rn = (leftv)omAlloc0Bin(sleftv_bin);
    BOOLEAN failed=FALSE;
    i=0;
    while ((dAssign[i].res!=lt)
      && (dAssign[i].res!=0)) i++;
    while (dAssign[i].res==lt)
    {
      if ((ri=iiTestConvert(rt,dAssign[i].arg))!=0)
      {
        failed= iiConvert(rt,dAssign[i].arg,ri,r,rn);
        if(!failed)
        {
          failed= dAssign[i].p(ld,rn,l->e);
          if (TEST_V_ALLWARN)
            Print("assign %s=%s ok? %d\n",Tok2Cmdname(lt),Tok2Cmdname(rn->rtyp),!failed);
        }
        // everything done, clean up temp. variables
        rn->CleanUp();
        omFreeBin((ADDRESS)rn, sleftv_bin);
        if (failed)
        {
          // leave loop, goto error handling
          break;
        }
        else
        {
          if(l!=ld) /* i.e. l is IDHDL, l->data is ld */
          {
            l->flag=ld->flag;
            l->attribute=ld->attribute;
          }
          // everything ok, return
          return FALSE;
        }
     }
     i++;
    }
    // error handling ---------------------------------------------------
    if (!errorreported)
    {
      if ((l->rtyp==IDHDL) && (l->e==NULL))
        Werror("`%s`(%s) = `%s` is not supported",
          Tok2Cmdname(lt),l->Name(),Tok2Cmdname(rt));
      else
         Werror("`%s` = `%s` is not supported"
             ,Tok2Cmdname(lt),Tok2Cmdname(rt));
      if (BVERBOSE(V_SHOW_USE))
      {
        i=0;
        while ((dAssign[i].res!=lt)
          && (dAssign[i].res!=0)) i++;
        while (dAssign[i].res==lt)
        {
          Werror("expected `%s` = `%s`"
              ,Tok2Cmdname(lt),Tok2Cmdname(dAssign[i].arg));
          i++;
        }
      }
    }
  }
  return TRUE;
}
/*2
* assign sys_var = val
*/
static BOOLEAN iiAssign_sys(leftv l, leftv r)
{
  int rt=r->Typ();

  if (rt==0)
  {
    if (!errorreported) Werror("`%s` is undefined",r->Fullname());
    return TRUE;
  }
  int i=0;
  int lt=l->rtyp;
  while (((dAssign_sys[i].res!=lt)
      || (dAssign_sys[i].arg!=rt))
    && (dAssign_sys[i].res!=0)) i++;
  if (dAssign_sys[i].res!=0)
  {
    if (!dAssign_sys[i].p(l,r))
    {
      // everything ok, clean up
      return FALSE;
    }
  }
  // implicite type conversion ----------------------------------------------
  if (dAssign_sys[i].res==0)
  {
    int ri;
    leftv rn = (leftv)omAlloc0Bin(sleftv_bin);
    BOOLEAN failed=FALSE;
    i=0;
    while ((dAssign_sys[i].res!=lt)
      && (dAssign_sys[i].res!=0)) i++;
    while (dAssign_sys[i].res==lt)
    {
      if ((ri=iiTestConvert(rt,dAssign_sys[i].arg))!=0)
      {
        failed= ((iiConvert(rt,dAssign_sys[i].arg,ri,r,rn))
            || (dAssign_sys[i].p(l,rn)));
        // everything done, clean up temp. variables
        rn->CleanUp();
        omFreeBin((ADDRESS)rn, sleftv_bin);
        if (failed)
        {
          // leave loop, goto error handling
          break;
        }
        else
        {
          // everything ok, return
          return FALSE;
        }
     }
     i++;
    }
    // error handling ---------------------------------------------------
    if(!errorreported)
    {
      Werror("`%s` = `%s` is not supported"
             ,Tok2Cmdname(lt),Tok2Cmdname(rt));
      if (BVERBOSE(V_SHOW_USE))
      {
        i=0;
        while ((dAssign_sys[i].res!=lt)
          && (dAssign_sys[i].res!=0)) i++;
        while (dAssign_sys[i].res==lt)
        {
          Werror("expected `%s` = `%s`"
              ,Tok2Cmdname(lt),Tok2Cmdname(dAssign_sys[i].arg));
          i++;
        }
      }
    }
  }
  return TRUE;
}
static BOOLEAN jiA_INTVEC_L(leftv l,leftv r)
{
  /* right side is intvec, left side is list (of int)*/
  BOOLEAN nok;
  int i=0;
  leftv l1=l;
  leftv h;
  sleftv t;
  intvec *iv=(intvec *)r->Data();
  memset(&t,0,sizeof(sleftv));
  t.rtyp=INT_CMD;
  while ((i<iv->length())&&(l!=NULL))
  {
    t.data=(char *)(*iv)[i];
    h=l->next;
    l->next=NULL;
    nok=jiAssign_1(l,&t);
    if (nok) return TRUE;
    i++;
    l=h;
  }
  l1->CleanUp();
  r->CleanUp();
  return FALSE;
}
static BOOLEAN jiA_VECTOR_L(leftv l,leftv r)
{
  /* right side is vector, left side is list (of poly)*/
  BOOLEAN nok;
  leftv l1=l;
  ideal I=idVec2Ideal((poly)r->Data());
  leftv h;
  sleftv t;
  int i=0;
  while (l!=NULL)
  {
    memset(&t,0,sizeof(sleftv));
    t.rtyp=POLY_CMD;
    if (i>=IDELEMS(I))
    {
      t.data=NULL;
    }
    else
    {
      t.data=(char *)I->m[i];
      I->m[i]=NULL;
    }
    h=l->next;
    l->next=NULL;
    nok=jiAssign_1(l,&t);
    t.CleanUp();
    if (nok)
    {
      idDelete(&I);
      return TRUE;
    }
    i++;
    l=h;
  }
  idDelete(&I);
  l1->CleanUp();
  r->CleanUp();
  //if (TEST_V_QRING && (currQuotient!=NULL)) jjNormalizeQRingP(l);
  return FALSE;
}
static BOOLEAN jjA_L_LIST(leftv l, leftv r)
/* left side: list/def, has to be a "real" variable
*  right side: expression list
*/
{
  int sl = r->listLength();
  lists L=(lists)omAllocBin(slists_bin);
  lists oldL;
  leftv h=NULL,o_r=r;
  int i;
  int rt;

  L->Init(sl);
  for (i=0;i<sl;i++)
  {
    if (h!=NULL) { /* e.g. not in the first step:
                   * h is the pointer to the old sleftv,
                   * r is the pointer to the next sleftv
                   * (in this moment) */
                   h->next=r;
                 }
    h=r;
    r=r->next;
    h->next=NULL;
    rt=h->Typ();
    if ((rt==0)||(rt==NONE)||(rt==DEF_CMD))
    {
      L->Clean();
      Werror("`%s` is undefined",h->Fullname());
      //listall();
      goto err;
    }
    //if ((rt==RING_CMD)||(rt==QRING_CMD))
    //{
    //  L->m[i].rtyp=rt;
    //  L->m[i].data=h->Data();
    //  ((ring)L->m[i].data)->ref++;
    //}
    //else
      L->m[i].CleanUp();
      L->m[i].Copy(h);
      if(errorreported)
      {
        L->Clean();
        goto err;
      }
  }
  oldL=(lists)l->Data();
  if (oldL!=NULL) oldL->Clean();
  if (l->rtyp==IDHDL)
  {
    IDLIST((idhdl)l->data)=L;
    IDTYP((idhdl)l->data)=LIST_CMD; // was possibly DEF_CMD
    ipMoveId((idhdl)l->data);
  }
  else
  {
    l->LData()->data=L;
    if ((l->e!=NULL) && (l->rtyp==DEF_CMD))
      l->rtyp=LIST_CMD;
  }
err:
  o_r->CleanUp();
  return errorreported;
}
static BOOLEAN jjA_L_INTVEC(leftv l,leftv r,intvec *iv)
{
  /* left side is intvec/intmat, right side is list (of int,intvec,intmat)*/
  leftv hh=r;
  int i = 0;
  while (hh!=NULL)
  {
    if (i>=iv->length())
    {
      if (TEST_V_ALLWARN)
      {
        Warn("expression list length(%d) does not match intmat size(%d)",
              iv->length()+exprlist_length(hh),iv->length());
      }
      break;
    }
    if (hh->Typ() == INT_CMD)
    {
      (*iv)[i++] = (int)((long)(hh->Data()));
    }
    else if ((hh->Typ() == INTVEC_CMD)
            ||(hh->Typ() == INTMAT_CMD))
    {
      intvec *ivv = (intvec *)(hh->Data());
      int ll = 0,l = si_min(ivv->length(),iv->length());
      for (; l>0; l--)
      {
        (*iv)[i++] = (*ivv)[ll++];
      }
    }
    else
    {
      delete iv;
      return TRUE;
    }
    hh = hh->next;
  }
  if (IDINTVEC((idhdl)l->data)!=NULL) delete IDINTVEC((idhdl)l->data);
  IDINTVEC((idhdl)l->data)=iv;
  return FALSE;
}
static BOOLEAN jjA_L_STRING(leftv l,leftv r)
{
  /* left side is string, right side is list of string*/
  leftv hh=r;
  int sl = 1;
  char *s;
  char *t;
  int tl;
  /* find the length */
  while (hh!=NULL)
  {
    if (hh->Typ()!= STRING_CMD)
    {
      return TRUE;
    }
    sl += strlen((char *)hh->Data());
    hh = hh->next;
  }
  s = (char * )omAlloc(sl);
  sl=0;
  hh = r;
  while (hh!=NULL)
  {
    t=(char *)hh->Data();
    tl=strlen(t);
    memcpy(s+sl,t,tl);
    sl+=tl;
    hh = hh->next;
  }
  s[sl]='\0';
  omFree((ADDRESS)IDDATA((idhdl)(l->data)));
  IDDATA((idhdl)(l->data))=s;
  return FALSE;
}
static BOOLEAN jiA_MATRIX_L(leftv l,leftv r)
{
  /* right side is matrix, left side is list (of poly)*/
  BOOLEAN nok=FALSE;
  int i;
  matrix m=(matrix)r->CopyD(MATRIX_CMD);
  leftv h;
  leftv ol=l;
  leftv o_r=r;
  sleftv t;
  memset(&t,0,sizeof(sleftv));
  t.rtyp=POLY_CMD;
  int mxn=MATROWS(m)*MATCOLS(m);
  loop
  {
    i=0;
    while ((i<mxn /*MATROWS(m)*MATCOLS(m)*/)&&(l!=NULL))
    {
      t.data=(char *)m->m[i];
      m->m[i]=NULL;
      h=l->next;
      l->next=NULL;
      nok=jiAssign_1(l,&t);
      l->next=h;
      if (nok)
      {
        idDelete((ideal *)&m);
        goto ende;
      }
      i++;
      l=h;
    }
    idDelete((ideal *)&m);
    h=r;
    r=r->next;
    if (l==NULL)
    {
      if (r!=NULL)
      {
        Warn("list length mismatch in assign (l>r)");
        nok=TRUE;
      }
      break;
    }
    else if (r==NULL)
    {
      Warn("list length mismatch in assign (l<r)");
      nok=TRUE;
      break;
    }
    if ((r->Typ()==IDEAL_CMD)||(r->Typ()==MATRIX_CMD))
    {
      m=(matrix)r->CopyD(MATRIX_CMD);
      mxn=MATROWS(m)*MATCOLS(m);
    }
    else if (r->Typ()==POLY_CMD)
    {
      m=mpNew(1,1);
      MATELEM(m,1,1)=(poly)r->CopyD(POLY_CMD);
      pNormalize(MATELEM(m,1,1));
      mxn=1;
    }
    else
    {
      nok=TRUE;
      break;
    }
  }
ende:
  o_r->CleanUp();
  ol->CleanUp();
  return nok;
}
static BOOLEAN jiA_STRING_L(leftv l,leftv r)
{
  /*left side are strings, right side is a string*/
  /*e.g. s[2..3]="12" */
  /*the case s=t[1..4] is handled in iiAssign,
  * the case s[2..3]=t[3..4] is handled in iiAssgn_rec*/
  int ll=l->listLength();
  int rl=r->listLength();
  BOOLEAN nok=FALSE;
  sleftv t;
  leftv h,l1=l;
  int i=0;
  char *ss;
  char *s=(char *)r->Data();
  int sl=strlen(s);

  memset(&t,0,sizeof(sleftv));
  t.rtyp=STRING_CMD;
  while ((i<sl)&&(l!=NULL))
  {
    ss=(char *)omAlloc(2);
    ss[1]='\0';
    ss[0]=s[i];
    t.data=ss;
    h=l->next;
    l->next=NULL;
    nok=jiAssign_1(l,&t);
    if (nok)
    {
      break;
    }
    i++;
    l=h;
  }
  r->CleanUp();
  l1->CleanUp();
  return nok;
}
static BOOLEAN jiAssign_list(leftv l, leftv r)
{
  int i=l->e->start-1;
  if (i<0)
  {
    Werror("index[%d] must be positive",i+1);
    return TRUE;
  }
  if(l->attribute!=NULL)
  {
    atKillAll((idhdl)l);
    l->attribute=NULL;
  }
  l->flag=0;
  lists li;
  if (l->rtyp==IDHDL)
  {
    li=IDLIST((idhdl)l->data);
  }
  else
  {
    li=(lists)l->data;
  }
  if (i>li->nr)
  {
    li->m=(leftv)omreallocSize(li->m,(li->nr+1)*sizeof(sleftv),(i+1)*sizeof(sleftv));
    memset(&(li->m[li->nr+1]),0,(i-li->nr)*sizeof(sleftv));
    int j=li->nr+1;
    for(;j<=i;j++)
      li->m[j].rtyp=DEF_CMD;
    li->nr=i;
  }
  leftv ld=&(li->m[i]);
  ld->e=l->e->next;
  BOOLEAN b;
  if (/*(ld->rtyp!=LIST_CMD)
  &&*/(ld->e==NULL)
  &&(ld->Typ()!=r->Typ()))
  {
    sleftv tmp;
    memset(&tmp,0,sizeof(sleftv));
    tmp.rtyp=DEF_CMD;
    b=iiAssign(&tmp,r);
    ld->CleanUp();
    memcpy(ld,&tmp,sizeof(sleftv));
  }
  else
  {
    b=iiAssign(ld,r);
    if (l->e!=NULL) l->e->next=ld->e;
    ld->e=NULL;
  }
  return b;
}
static BOOLEAN jiAssign_rec(leftv l, leftv r)
{
  leftv l1=l;
  leftv r1=r;
  leftv lrest;
  leftv rrest;
  BOOLEAN b;
  do
  {
    lrest=l->next;
    rrest=r->next;
    l->next=NULL;
    r->next=NULL;
    b=iiAssign(l,r);
    l->next=lrest;
    r->next=rrest;
    l=lrest;
    r=rrest;
  } while  ((!b)&&(l!=NULL));
  l1->CleanUp();
  r1->CleanUp();
  return b;
}
BOOLEAN iiAssign(leftv l, leftv r)
{
  if (errorreported) return TRUE;
  int ll=l->listLength();
  int rl;
  int lt=l->Typ();
  int rt=NONE;
  BOOLEAN b;
  if (l->rtyp==ALIAS_CMD)
  {
    Werror("`%s` is read-only",l->Name());
  }

  if (l->rtyp==IDHDL)
  {
    atKillAll((idhdl)l->data);
    IDFLAG((idhdl)l->data)=0;
    l->attribute=NULL;
  }
  else if (l->attribute!=NULL)
    atKillAll((idhdl)l);
  l->flag=0;
  if (ll==1)
  {
    /* l[..] = ... */
    if(l->e!=NULL)
    {
      BOOLEAN like_lists=0;
      blackbox *bb=NULL;
      int bt;
      if (((bt=l->rtyp)>MAX_TOK)
      || ((l->rtyp==IDHDL) && ((bt=IDTYP((idhdl)l->data))>MAX_TOK)))
      {
        bb=getBlackboxStuff(bt);
        like_lists=BB_LIKE_LIST(bb);
      }
      else if (((l->rtyp==IDHDL) && (IDTYP((idhdl)l->data)==LIST_CMD))
        || (l->rtyp==LIST_CMD))
      {
        like_lists=2;
      }
      if(like_lists)
      {
        if (TEST_V_ALLWARN) PrintS("assign list[..]=...or similar\n");
        b=jiAssign_list(l,r);
        if((!b) && (like_lists==2))
        {
          //Print("jjA_L_LIST: - 2 \n");
          if((l->rtyp==IDHDL) && (l->data!=NULL))
          {
            ipMoveId((idhdl)l->data);
            l->attribute=IDATTR((idhdl)l->data);
            l->flag=IDFLAG((idhdl)l->data);
          }
        }
        r->CleanUp();
        Subexpr h;
        while (l->e!=NULL)
        {
          h=l->e->next;
          omFreeBin((ADDRESS)l->e, sSubexpr_bin);
          l->e=h;
        }
        if ((!b) && (like_lists==1))
        {
          // check blackbox/newtype type:
          if(bb->blackbox_Check(bb,l->Data())) return TRUE;
        }
        return b;
      }
    }
    // end of handling elems of list and similar
    rl=r->listLength();
    if (rl==1)
    {
      /* system variables = ... */
      if(((l->rtyp>=VECHO)&&(l->rtyp<=VPRINTLEVEL))
      ||((l->rtyp>=VALTVARS)&&(l->rtyp<=VMINPOLY)))
      {
        b=iiAssign_sys(l,r);
        r->CleanUp();
        //l->CleanUp();
        return b;
      }
      rt=r->Typ();
      /* a = ... */
      if ((lt!=MATRIX_CMD)
      &&(lt!=INTMAT_CMD)
      &&((lt==rt)||(lt!=LIST_CMD)))
      {
        b=jiAssign_1(l,r);
        if (l->rtyp==IDHDL)
        {
          if ((lt==DEF_CMD)||(lt==LIST_CMD))
          {
            ipMoveId((idhdl)l->data);
          }
          l->attribute=IDATTR((idhdl)l->data);
          l->flag=IDFLAG((idhdl)l->data);
          l->CleanUp();
        }
        r->CleanUp();
        return b;
      }
      if (((lt!=LIST_CMD)
        &&((rt==MATRIX_CMD)
          ||(rt==INTMAT_CMD)
          ||(rt==INTVEC_CMD)
          ||(rt==MODUL_CMD)))
      ||((lt==LIST_CMD)
        &&(rt==RESOLUTION_CMD))
      )
      {
        b=jiAssign_1(l,r);
        if((l->rtyp==IDHDL)&&(l->data!=NULL))
        {
          if ((lt==DEF_CMD) || (lt==LIST_CMD))
          {
            //Print("ipAssign - 3.0\n");
            ipMoveId((idhdl)l->data);
          }
          l->attribute=IDATTR((idhdl)l->data);
          l->flag=IDFLAG((idhdl)l->data);
        }
        r->CleanUp();
        Subexpr h;
        while (l->e!=NULL)
        {
          h=l->e->next;
          omFreeBin((ADDRESS)l->e, sSubexpr_bin);
          l->e=h;
        }
        return b;
      }
    }
    if (rt==NONE) rt=r->Typ();
  }
  else if (ll==(rl=r->listLength()))
  {
    b=jiAssign_rec(l,r);
    return b;
  }
  else
  {
    if (rt==NONE) rt=r->Typ();
    if (rt==INTVEC_CMD)
      return jiA_INTVEC_L(l,r);
    else if (rt==VECTOR_CMD)
      return jiA_VECTOR_L(l,r);
    else if ((rt==IDEAL_CMD)||(rt==MATRIX_CMD))
      return jiA_MATRIX_L(l,r);
    else if ((rt==STRING_CMD)&&(rl==1))
      return jiA_STRING_L(l,r);
    Werror("length of lists in assignment does not match (l:%d,r:%d)",
      ll,rl);
    return TRUE;
  }

  leftv hh=r;
  BOOLEAN nok=FALSE;
  BOOLEAN map_assign=FALSE;
  switch (lt)
  {
    case INTVEC_CMD:
      nok=jjA_L_INTVEC(l,r,new intvec(exprlist_length(r)));
      break;
    case INTMAT_CMD:
    {
      nok=jjA_L_INTVEC(l,r,new intvec(IDINTVEC((idhdl)l->data)));
      break;
    }
    case MAP_CMD:
    {
      // first element in the list sl (r) must be a ring
      if (((rt == RING_CMD)||(rt == QRING_CMD))&&(r->e==NULL))
      {
        omFree((ADDRESS)IDMAP((idhdl)l->data)->preimage);
        IDMAP((idhdl)l->data)->preimage = omStrDup (r->Fullname());
        /* advance the expressionlist to get the next element after the ring */
        hh = r->next;
        //r=hh;
      }
      else
      {
        WerrorS("expected ring-name");
        nok=TRUE;
        break;
      }
      if (hh==NULL) /* map-assign: map f=r; */
      {
        WerrorS("expected image ideal");
        nok=TRUE;
        break;
      }
      if ((hh->next==NULL)&&(hh->Typ()==IDEAL_CMD))
        return jiAssign_1(l,hh); /* map-assign: map f=r,i; */
      //no break, handle the rest like an ideal:
      map_assign=TRUE;
    }
    case MATRIX_CMD:
    case IDEAL_CMD:
    case MODUL_CMD:
    {
      sleftv t;
      matrix olm = (matrix)l->Data();
      int rk=olm->rank;
      char *pr=((map)olm)->preimage;
      BOOLEAN module_assign=(/*l->Typ()*/ lt==MODUL_CMD);
      matrix lm ;
      int  num;
      int j,k;
      int i=0;
      int mtyp=MATRIX_CMD; /*Type of left side object*/
      int etyp=POLY_CMD;   /*Type of elements of left side object*/

      if (lt /*l->Typ()*/==MATRIX_CMD)
      {
        num=olm->cols()*olm->rows();
        lm=mpNew(olm->rows(),olm->cols());
        int el;
        if ((TEST_V_ALLWARN) && (num!=(el=exprlist_length(hh))))
        {
          Warn("expression list length(%d) does not match matrix size(%d)",el,num);
        }
      }
      else /* IDEAL_CMD or MODUL_CMD */
      {
        num=exprlist_length(hh);
        lm=(matrix)idInit(num,1);
        rk=1;
        if (module_assign)
        {
          mtyp=MODUL_CMD;
          etyp=VECTOR_CMD;
        }
      }

      int ht;
      loop
      {
        if (hh==NULL)
          break;
        else
        {
          matrix rm;
          ht=hh->Typ();
          if ((j=iiTestConvert(ht,etyp))!=0)
          {
            nok=iiConvert(ht,etyp,j,hh,&t);
            hh->next=t.next;
            if (nok) break;
            lm->m[i]=(poly)t.CopyD(etyp);
            pNormalize(lm->m[i]);
            if (module_assign) rk=si_max(rk,(int)pMaxComp(lm->m[i]));
            i++;
          }
          else
          if ((j=iiTestConvert(ht,mtyp))!=0)
          {
            nok=iiConvert(ht,mtyp,j,hh,&t);
            hh->next=t.next;
            if (nok) break;
            rm = (matrix)t.CopyD(mtyp);
            if (module_assign)
            {
              j = si_min(num,rm->cols());
              rk=si_max(rk,(int)rm->rank);
            }
            else
              j = si_min(num-i,rm->rows() * rm->cols());
            for(k=0;k<j;k++,i++)
            {
              lm->m[i]=rm->m[k];
              pNormalize(lm->m[i]);
              rm->m[k]=NULL;
            }
            idDelete((ideal *)&rm);
          }
          else
          {
            nok=TRUE;
            break;
          }
          t.next=NULL;t.CleanUp();
          if (i==num) break;
          hh=hh->next;
        }
      }
      if (nok)
        idDelete((ideal *)&lm);
      else
      {
        idDelete((ideal *)&olm);
        if (module_assign)   lm->rank=rk;
        else if (map_assign) ((map)lm)->preimage=pr;
        l=l->LData();
        if (l->rtyp==IDHDL)
          IDMATRIX((idhdl)l->data)=lm;
        else
          l->data=(char *)lm;
      }
      break;
    }
    case STRING_CMD:
      nok=jjA_L_STRING(l,r);
      break;
    case DEF_CMD:
    case LIST_CMD:
      nok=jjA_L_LIST(l,r);
      break;
    case NONE:
    case 0:
      Werror("cannot assign to %s",l->Fullname());
      nok=TRUE;
      break;
    default:
      WerrorS("assign not impl.");
      nok=TRUE;
      break;
  } /* end switch: typ */
  if (nok && (!errorreported)) WerrorS("incompatible type in list assignment");
  r->CleanUp();
  return nok;
}
void jjNormalizeQRingId(leftv I)
{
  if ((currQuotient!=NULL) && (!hasFlag(I,FLAG_QRING)))
  {
    if (I->e==NULL)
    {
      ideal I0=(ideal)I->Data();
      switch (I->Typ())
      {
        case IDEAL_CMD:
        case MODUL_CMD:
        {
          ideal F=idInit(1,1);
          ideal II=kNF(F,currQuotient,I0);
          idDelete(&F);
          if (I->rtyp!=IDHDL)
          {
            idDelete((ideal*)&(I0));
            I->data=II;
          }
          else
          {
            idhdl h=(idhdl)I->data;
            idDelete((ideal*)&IDIDEAL(h));
            IDIDEAL(h)=II;
            setFlag(h,FLAG_QRING);
          }
          break;
        }
        default: break;
      }
      setFlag(I,FLAG_QRING);
    }
  }
}
void jjNormalizeQRingP(leftv I)
{
  if ((currQuotient!=NULL) && (!hasFlag(I,FLAG_QRING)))
  {
    poly p=(poly)I->Data();
    if ((I->e==NULL) && (p!=NULL))
    {
      ideal F=idInit(1,1);
      poly II=kNF(F,currQuotient,p);
      idDelete(&F);
      if ((I->rtyp==POLY_CMD)
      || (I->rtyp==VECTOR_CMD))
      {
        pDelete(&p);
        I->data=II;
      }
      else if (I->rtyp==IDHDL)
      {
        pDelete(&p);
        idhdl h=(idhdl)I->data;
        IDPOLY(h)=II;
        setFlag(h,FLAG_QRING);
      }
      else
      {
        pDelete(&II);
      }
    }
    setFlag(I,FLAG_QRING);
  }
}
BOOLEAN jjIMPORTFROM(leftv res, leftv u, leftv v)
{
  //Print("importfrom %s::%s ->.\n",v->Name(),u->Name() );
  assume(u->Typ()==PACKAGE_CMD);
  char *vn=(char *)v->Name();
  idhdl h=((package)(u->Data()))->idroot->get(vn /*v->Name()*/, myynest);
  if (h!=NULL)
  {
    //check for existence
    if (((package)(u->Data()))==basePack)
    {
      WarnS("source and destination packages are identical");
      return FALSE;
    }
    idhdl t=basePack->idroot->get(vn /*v->Name()*/, myynest);
    if (t!=NULL)
    {
      Warn("redefining `%s`",vn);
      killhdl(t);
    }
    sleftv tmp_expr;
    if (iiDeclCommand(&tmp_expr,v,myynest,DEF_CMD,&IDROOT)) return TRUE;
    sleftv h_expr;
    memset(&h_expr,0,sizeof(h_expr));
    h_expr.rtyp=IDHDL;
    h_expr.data=h;
    h_expr.name=vn;
    return iiAssign(&tmp_expr,&h_expr);
  }
  else
  {
    Werror("`%s` not found in `%s`",v->Name(), u->Name());
    return TRUE;
  }
  return FALSE;
}
