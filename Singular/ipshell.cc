/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/* $Id: ipshell.cc,v 1.98 2005-04-18 12:22:09 Singular Exp $ */
/*
* ABSTRACT:
*/

//#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <math.h>

#include "mod2.h"
#include "tok.h"
#include "ipid.h"
#include "intvec.h"
#include "omalloc.h"
#include "febase.h"
#include "polys.h"
#include "ideals.h"
#include "matpol.h"
#include "kstd1.h"
#include "ring.h"
#include "subexpr.h"
#include "maps.h"
#include "syz.h"
#include "numbers.h"
#include "lists.h"
#include "attrib.h"
#include "ipconv.h"
#include "silink.h"
#include "stairc.h"
#include "weight.h"
#include "semic.h"
#include "splist.h"
#include "spectrum.h"
#include "gnumpfl.h"
#include "mpr_base.h"
#include "ffields.h"
#include "clapsing.h"
#include "hutil.h"
#include "ipshell.h"
#ifdef HAVE_FACTORY
#define SI_DONT_HAVE_GLOBAL_VARS
#include <factory.h>
#endif

// define this if you want to use the fast_map routine for mapping ideals
#define FAST_MAP

#ifdef FAST_MAP
#include "fast_maps.h"
#endif

leftv iiCurrArgs=NULL;
int  traceit = 0;
char *lastreserved=NULL;

int  myynest = -1;

static BOOLEAN iiNoKeepRing=TRUE;

/*0 implementation*/

char * Tok2Cmdname(int tok)
{
  int i = 0;
  if (tok < 0)
  {
    return cmds[0].name;
  }
  if (tok==ANY_TYPE) return "any_type";
  if (tok==NONE) return "nothing";
  //if (tok==IFBREAK) return "if_break";
  //if (tok==VECTOR_FROM_POLYS) return "vector_from_polys";
  //if (tok==ORDER_VECTOR) return "ordering";
  //if (tok==REF_VAR) return "ref";
  //if (tok==OBJECT) return "object";
  //if (tok==PRINT_EXPR) return "print_expr";
  if (tok==IDHDL) return "identifier";
  while (cmds[i].tokval!=0)
  {
    if ((cmds[i].tokval == tok)&&(cmds[i].alias==0))
    {
      return cmds[i].name;
    }
    i++;
  }
  return cmds[0].name;
}

char * iiTwoOps(int t)
{
  if (t<127)
  {
    static char ch[2];
    switch (t)
    {
      case '&':
        return "and";
      case '|':
        return "or";
      default:
        ch[0]=t;
        ch[1]='\0';
        return ch;
    }
  }
  switch (t)
  {
    case COLONCOLON:  return "::";
    case DOTDOT:      return "..";
    //case PLUSEQUAL:   return "+=";
    //case MINUSEQUAL:  return "-=";
    case MINUSMINUS:  return "--";
    case PLUSPLUS:    return "++";
    case EQUAL_EQUAL: return "==";
    case LE:          return "<=";
    case GE:          return ">=";
    case NOTEQUAL:    return "<>";
    default:          return Tok2Cmdname(t);
  }
}

static void list1(char* s, idhdl h,BOOLEAN c, BOOLEAN fullname)
{
  char buffer[22];
  int l;
  char buf2[128];

  if(fullname) sprintf(buf2, "%s::%s", "", IDID(h));
  else sprintf(buf2, "%s", IDID(h));

  Print("%s%-20.20s [%d]  ",s,buf2,IDLEV(h));
  if (h == currRingHdl) PrintS("*");
  PrintS(Tok2Cmdname((int)IDTYP(h)));

  ipListFlag(h);
  switch(IDTYP(h))
  {
    case INT_CMD:   Print(" %d",IDINT(h)); break;
    case INTVEC_CMD:Print(" (%d)",IDINTVEC(h)->length()); break;
    case INTMAT_CMD:Print(" %d x %d",IDINTVEC(h)->rows(),IDINTVEC(h)->cols());
                    break;
    case POLY_CMD:
    case VECTOR_CMD:if (c)
                    {
                      PrintS(" ");wrp(IDPOLY(h));
                      if(IDPOLY(h) != NULL)
                      {
                        Print(", %d monomial(s)",pLength(IDPOLY(h)));
                      }
                    }
                    break;
    case MODUL_CMD: Print(", rk %d", IDIDEAL(h)->rank);
    case IDEAL_CMD: Print(", %u generator(s)",
                    IDELEMS(IDIDEAL(h)),IDIDEAL(h)->rank); break;
    case MAP_CMD:
                    Print(" from %s",IDMAP(h)->preimage); break;
    case MATRIX_CMD:Print(" %u x %u"
                      ,MATROWS(IDMATRIX(h))
                      ,MATCOLS(IDMATRIX(h))
                    );
                    break;
    case PACKAGE_CMD:
                    PrintS(" (");
                    switch (IDPACKAGE(h)->language)
                    {
                        case LANG_SINGULAR: PrintS("S"); break;
                        case LANG_C:        PrintS("C"); break;
                        case LANG_TOP:      PrintS("T"); break;
                        case LANG_NONE:     PrintS("N"); break;
                        default:            PrintS("U");
                    }
                    if(IDPACKAGE(h)->libname!=NULL)
                      Print(",%s", IDPACKAGE(h)->libname);
                    PrintS(")");
                    break;
    case PROC_CMD: if(strlen(IDPROC(h)->libname)>0)
                     Print(" from %s",IDPROC(h)->libname);
                   if(IDPROC(h)->is_static)
                     PrintS(" (static)");
                   break;
    case STRING_CMD:
                   {
                     char *s;
                     l=strlen(IDSTRING(h));
                     memset(buffer,0,22);
                     strncpy(buffer,IDSTRING(h),min(l,20));
                     if ((s=strchr(buffer,'\n'))!=NULL)
                     {
                       *s='\0';
                     }
                     PrintS(" ");
                     PrintS(buffer);
                     if((s!=NULL) ||(l>20))
                     {
                       Print("..., %d char(s)",l);
                     }
                     break;
                   }
    case LIST_CMD: Print(", size: %d",IDLIST(h)->nr+1);
                   break;
    case QRING_CMD:
    case RING_CMD:
                   if ((IDRING(h)==currRing) && (currRingHdl!=h))
                     PrintS("(*)"); /* this is an alias to currRing */
#ifdef RDEBUG
                   if (traceit &TRACE_SHOW_RINGS)
                     Print(" <%x>",IDRING(h));
#endif
                   break;
    /*default:     break;*/
  }
  PrintLn();
}

void type_cmd(idhdl h)
{
  BOOLEAN oldShortOut = FALSE;

  if (currRing != NULL)
  {
    oldShortOut = currRing->ShortOut;
    currRing->ShortOut = 1;
  }
  list1("// ",h,FALSE,FALSE);
  if (IDTYP(h)!=INT_CMD)
  {
    sleftv expr;
    memset(&expr,0,sizeof(expr));
    expr.rtyp=IDHDL;
    expr.name=IDID(h);
    expr.data=(void *)h;
    expr.Print();
  }
  if (currRing != NULL)
    currRing->ShortOut = oldShortOut;
}

static void killlocals0(int v, idhdl * localhdl)
{
  idhdl h = *localhdl;
  while (h!=NULL)
  {
    int vv;
    //Print("consider %s, lev: %d:",IDID(h),IDLEV(h));
    if ((vv=IDLEV(h))>0)
    {
      if (vv < v)
      {
        if (iiNoKeepRing)
        {
          //PrintS(" break\n");
          return;
        }
        h = IDNEXT(h);
        //PrintLn();
      }
      else if (vv >= v)
      {
        idhdl nexth = IDNEXT(h);
        killhdl2(h,localhdl,currRing);
        h = nexth;
        //PrintS("kill\n");
      }
    }
    else
    {
      h = IDNEXT(h);
      //PrintLn();
    }
  }
}
#ifndef HAVE_NS
void killlocals(int v)
{
  killlocals0(v,&IDROOT);

  if ((iiRETURNEXPR_len > myynest)
  && ((iiRETURNEXPR[myynest].Typ()==RING_CMD)
    || (iiRETURNEXPR[myynest].Typ()==QRING_CMD)))
  {
    leftv h=&iiRETURNEXPR[myynest];
    killlocals0(v,&(((ring)h->data)->idroot));
  }

  idhdl sh=currRingHdl;
  ring sr=currRing;
  BOOLEAN changed=FALSE;
  idhdl h = IDROOT;

//  Print("killlocals in %s\n",IDID(currPackHdl));
  while (h!=NULL)
  {
    if (((IDTYP(h)==QRING_CMD) || (IDTYP(h) == RING_CMD))
    && (IDRING(h)->idroot!=NULL))
    {
      if (IDRING(h)!=currRing) {changed=TRUE;rSetHdl(h);}
      killlocals0(v,&(IDRING(h)->idroot));
    }
    else if (IDTYP(h) == PACKAGE_CMD)
    {
      killlocals0(v,&(IDPACKAGE(h)->idroot));
    }
    h = IDNEXT(h);
  }
  if (changed)
  {
    currRing=NULL;
    currRingHdl=NULL;
    if (sh!=NULL) rSetHdl(sh);
    else if (sr!=NULL)
    {
      sh=rFindHdl(sr,NULL,NULL);
      rSetHdl(sh);
    }
  }

  if (myynest<=1) iiNoKeepRing=TRUE;
  //Print("end killlocals  >= %d\n",v);
  //listall();
}
#endif
#ifdef HAVE_NS
void killlocals_rec(idhdl *root,int v, ring r)
{
  idhdl h=*root;
  while (h!=NULL)
  {
    if (IDLEV(h)>=v)
    {
//      Print("kill %s, lev %d for lev %d\n",IDID(h),IDLEV(h),v);
      idhdl n=IDNEXT(h);
      killhdl2(h,root,r);
      h=n;
    }
    else if (IDTYP(h)==PACKAGE_CMD)
    {
 //     Print("into pack %s, lev %d for lev %d\n",IDID(h),IDLEV(h),v);
      if (IDPACKAGE(h)!=basePack)
        killlocals_rec(&(IDRING(h)->idroot),v,r);
      h=IDNEXT(h);
    }
    else if ((IDTYP(h)==RING_CMD)
    ||(IDTYP(h)==QRING_CMD))
    {
      if (IDRING(h)->idroot!=NULL)
      {
  //    Print("into ring %s, lev %d for lev %d\n",IDID(h),IDLEV(h),v);
        killlocals_rec(&(IDRING(h)->idroot),v,IDRING(h));
      }
      h=IDNEXT(h);
    }
    else
    {
//      Print("skip %s lev %d for lev %d\n",IDID(h),IDLEV(h),v);
      h=IDNEXT(h);
    }
  }
}
BOOLEAN killlocals_list(int v, lists L)
{
  if (L==NULL) return FALSE;
  BOOLEAN changed=FALSE;
  int n=L->nr;
  for(;n>=0;n--)
  {
    leftv h=&(L->m[n]);
    void *d=h->data;
    if (((h->rtyp==RING_CMD) || (h->rtyp==QRING_CMD))
    && (((ring)d)->idroot!=NULL))
    {
      if (d!=currRing) {changed=TRUE;rChangeCurrRing((ring)d);}
      killlocals0(v,&(((ring)h->data)->idroot));
    }
    else if (h->rtyp==LIST_CMD)
      changed|=killlocals_list(v,(lists)d);
  }
  return changed;
}
void killlocals(int v)
{
  BOOLEAN changed=FALSE;
  idhdl sh=currRingHdl;
  ring cr=currRing;
  if (sh!=NULL) changed=((IDLEV(sh)<v) || (IDRING(sh)->ref>0));
  //if (changed) Print("currRing=%s(%x), lev=%d,ref=%d\n",IDID(sh),IDRING(sh),IDLEV(sh),IDRING(sh)->ref);

  killlocals_rec(&(basePack->idroot),v,currRing);

  if (iiRETURNEXPR_len > myynest)
  {
    int t=iiRETURNEXPR[myynest].Typ();
    if ((/*iiRETURNEXPR[myynest].Typ()*/ t==RING_CMD)
    || (/*iiRETURNEXPR[myynest].Typ()*/ t==QRING_CMD))
    {
      leftv h=&iiRETURNEXPR[myynest];
      if (((ring)h->data)->idroot!=NULL)
        killlocals0(v,&(((ring)h->data)->idroot));
    }
    else if (/*iiRETURNEXPR[myynest].Typ()*/ t==LIST_CMD)
    {
      leftv h=&iiRETURNEXPR[myynest];
      changed |=killlocals_list(v,(lists)h->data);
    }
  }
  if (changed)
  {
    currRing=cr;
    currRingHdl=rFindHdl(cr,NULL,NULL);
    if (currRingHdl==NULL) currRingHdl=sh;
  }

  if (myynest<=1) iiNoKeepRing=TRUE;
  //Print("end killlocals  >= %d\n",v);
  //listall();
}
#endif

void list_cmd(int typ, const char* what, char *prefix,BOOLEAN iterate, BOOLEAN fullname)
{
  idhdl h,start;
  BOOLEAN all = typ<0;
  BOOLEAN really_all=FALSE;
  BOOLEAN do_packages=FALSE;

  if ( typ == -1 ) do_packages=TRUE;
  if ( typ==0 )
  {
    if (strcmp(what,"all")==0)
    {
      really_all=TRUE;
#ifdef HAVE_NS
      h=basePack->idroot;
#else
      h=IDROOT;
#endif
    }
    else
    {
      h = ggetid(what);
      if (h!=NULL)
      {
        if (iterate) list1(prefix,h,TRUE,fullname);
        if ((IDTYP(h)==RING_CMD)
            || (IDTYP(h)==QRING_CMD)
#ifdef HAVE_NS
            //|| (IDTYP(h)==PACKE_CMD)
#endif
        )
        {
          h=IDRING(h)->idroot;
        }
        else if((IDTYP(h)==PACKAGE_CMD) || (IDTYP(h)==POINTER_CMD))
        {
          Print("list_cmd:package or pointer\n");
          all=TRUE;typ=PROC_CMD;fullname=TRUE;really_all=TRUE;
          h=IDPACKAGE(h)->idroot;
        }
        else
          return;
      }
      else
      {
        Werror("%s is undefined",what);
        return;
      }
    }
    all=TRUE;
  }
  else if (RingDependend(typ))
  {
    h = currRing->idroot;
  }
  else
    h = IDROOT;
  start=h;
  while (h!=NULL)
  {
    if ((all && (IDTYP(h)!=PROC_CMD) &&(IDTYP(h)!=PACKAGE_CMD))
    || (typ == IDTYP(h))
    || ((IDTYP(h)==QRING_CMD) && (typ==RING_CMD)))
    {
      list1(prefix,h,start==currRingHdl, fullname);
      if (((IDTYP(h)==RING_CMD)||(IDTYP(h)==QRING_CMD))
        && (really_all || (all && (h==currRingHdl)))
        && ((IDLEV(h)==0)||(IDLEV(h)==myynest)))
      {
        list_cmd(0,IDID(h),"//      ",FALSE);
      }
#ifdef HAVE_NS
      if (IDTYP(h)==PACKAGE_CMD && really_all)
      {
        list_cmd(0,IDID(h),"//      ",FALSE);
      }
#endif /* HAVE_NS */
    }
    h = IDNEXT(h);
  }
}

void test_cmd(int i)
{
  int ii=(char)i;

  if (i == (-32))
  {
    test = 0;
  }
  else
  {
    if (i<0)
    {
      ii= -i;
      if (Sy_bit(ii) & kOptions)
      {
        Warn("Gerhard, use the option command");
        test &= ~Sy_bit(ii);
      }
      else if (Sy_bit(ii) & validOpts)
        test &= ~Sy_bit(ii);
    }
    else if (i<32)
    {
      if (Sy_bit(ii) & kOptions)
      {
        Warn("Gerhard, use the option command");
        test |= Sy_bit(ii);
      }
      else if (Sy_bit(ii) & validOpts)
        test |= Sy_bit(ii);
    }
  }
}

int exprlist_length(leftv v)
{
  int rc = 0;
  while (v!=NULL)
  {
    switch (v->Typ())
    {
      case INT_CMD:
      case POLY_CMD:
      case VECTOR_CMD:
      case NUMBER_CMD:
        rc++;
        break;
      case INTVEC_CMD:
      case INTMAT_CMD:
        rc += ((intvec *)(v->Data()))->length();
        break;
      case MATRIX_CMD:
      case IDEAL_CMD:
      case MODUL_CMD:
        {
          matrix mm = (matrix)(v->Data());
          rc += mm->rows() * mm->cols();
        }
        break;
      case LIST_CMD:
        rc+=((lists)v->Data())->nr+1;
        break;
      default:
        rc++;
    }
    v = v->next;
  }
  return rc;
}

int IsPrime(int p)  /* brute force !!!! */
{
  int i,j;
  if      (p == 0)    return 0;
  else if (p == 1)    return 1/*1*/;
  else if (p == 2)    return p;
  else if (p < 0)     return (-IsPrime(-p));
  else if (!(p & 1)) return IsPrime(p-1);
#ifdef HAVE_FACTORY
  else if (p<=32749) // max. small prime in factory
  {
    int a=0;
    int e=cf_getNumSmallPrimes()-1;
    i=e/2;
    do
    {
      if (p==(j=cf_getSmallPrime(i))) return p;
      if (p<j) e=i-1;
      else     a=i+1;
      i=a+(e-a)/2;
    } while ( a<= e);
    if (p>j) return j;
    else     return cf_getSmallPrime(i-1);
  }
#endif
#ifdef HAVE_FACTORY
  int end_i=cf_getNumSmallPrimes()-1;
#else
  int end_i=p/2;
#endif  
  int end_p=(int)sqrt((double)p);
restart:
  for (i=0; i<end_i; i++)
  {
#ifdef HAVE_FACTORY
    j=cf_getSmallPrime(i);
#else
    if (i==0) j=2;
    else j=2*i-1;
#endif    
    if ((p%j) == 0)
    {
    #ifdef HAVE_FACTORY
      if (p<=32751) return IsPrime(p-2);
    #endif  
      p-=2;
      goto restart;
    }
    if (j > end_p) return p;
  }
  return p;
}

BOOLEAN iiWRITE(leftv res,leftv v)
{
  sleftv vf;
  if (iiConvert(v->Typ(),LINK_CMD,iiTestConvert(v->Typ(),LINK_CMD),v,&vf))
  {
    WerrorS("link expected");
    return TRUE;
  }
  si_link l=(si_link)vf.Data();
  if (vf.next == NULL)
  {
    WerrorS("write: need at least two arguments");
    return TRUE;
  }

  BOOLEAN b=slWrite(l,vf.next); /* iiConvert preserves next */
  if (b)
  {
    const char *s;
    if ((l!=NULL)&&(l->name!=NULL)) s=l->name;
    else                            s=sNoName;
    Werror("cannot write to %s",s);
  }
  vf.CleanUp();
  return b;
}

leftv iiMap(map theMap, char * what)
{
  idhdl w,r;
  leftv v;
  int i;
  nMapFunc nMap;

  r=IDROOT->get(theMap->preimage,myynest);
#ifdef HAVE_NS
  if ((currPack!=basePack)
  &&((r==NULL) || ((r->typ != RING_CMD) && (r->typ != QRING_CMD))))
    r=basePack->idroot->get(theMap->preimage,myynest);
  if ((r==NULL) && (currRingHdl!=NULL)
  && (strcmp(theMap->preimage,IDID(currRingHdl))==0))
  {
    r=currRingHdl;
  }
#endif /* HAVE_NS */
  if ((r!=NULL) && ((r->typ == RING_CMD) || (r->typ== QRING_CMD)))
  {
    //if ((nMap=nSetMap(rInternalChar(IDRING(r)),
    //             IDRING(r)->parameter,
    //             rPar(IDRING(r)),
    //             IDRING(r)->minpoly)))
    if ((nMap=nSetMap(IDRING(r)))==NULL)
    {
      if (rEqual(IDRING(r),currRing))
      {
        nMap=nCopy;
      }
      else
      {
        Werror("can not map from ground field of %s to current ground field",
          theMap->preimage);
        return NULL;
      }
    }
    if (IDELEMS(theMap)<IDRING(r)->N)
    {
      theMap->m=(polyset)omReallocSize((ADDRESS)theMap->m,
                                 IDELEMS(theMap)*sizeof(poly),
                                 (IDRING(r)->N)*sizeof(poly));
      for(i=IDELEMS(theMap);i<IDRING(r)->N;i++)
        theMap->m[i]=NULL;
      IDELEMS(theMap)=IDRING(r)->N;
    }
    if (what==NULL)
    {
      WerrorS("argument of a map must have a name");
    }
    else if ((w=IDRING(r)->idroot->get(what,myynest))!=NULL)
    {
      v=(leftv)omAlloc0Bin(sleftv_bin);
      sleftv tmpW;
      memset(&tmpW,0,sizeof(sleftv));
      tmpW.rtyp=IDTYP(w);
      tmpW.data=IDDATA(w);
      #ifdef FAST_MAP
      if ((tmpW.rtyp==IDEAL_CMD) && (nMap==nCopy)
      #ifdef HAVE_PLURAL
      && (!rIsPluralRing(currRing))
      #endif
      )
      {
        v->rtyp=IDEAL_CMD;
        v->data=fast_map(IDIDEAL(w), IDRING(r), (ideal)theMap, currRing);
      }
      else
      #endif
      if (maApplyFetch(MAP_CMD,theMap,v,&tmpW,IDRING(r),NULL,NULL,0,nMap))
      {
        Werror("cannot map %s(%d)",Tok2Cmdname(w->typ),w->typ);
        omFreeBin((ADDRESS)v, sleftv_bin);
        return NULL;
      }
      return v;
    }
    else
    {
      Werror("%s undefined in %s",what,theMap->preimage);
    }
  }
  else
  {
    Werror("cannot find preimage %s",theMap->preimage);
  }
  return NULL;
}

#ifdef OLD_RES
void  iiMakeResolv(resolvente r, int length, int rlen, char * name, int typ0,
                   intvec ** weights)
{
  lists L=liMakeResolv(r,length,rlen,typ0,weights);
  int i=0;
  idhdl h;
  char * s=(char *)omAlloc(strlen(name)+5);

  while (i<=L->nr)
  {
    sprintf(s,"%s(%d)",name,i+1);
    if (i==0)
      h=enterid(s,myynest,typ0,&(currRing->idroot), FALSE);
    else
      h=enterid(s,myynest,MODUL_CMD,&(currRing->idroot), FALSE);
    if (h!=NULL)
    {
      h->data.uideal=(ideal)L->m[i].data;
      h->attribute=L->m[i].attribute;
      if (BVERBOSE(V_DEF_RES))
        Print("//defining: %s as %d-th syzygy module\n",s,i+1);
    }
    else
    {
      idDelete((ideal *)&(L->m[i].data));
      Warn("cannot define %s",s);
    }
    //L->m[i].data=NULL;
    //L->m[i].rtyp=0;
    //L->m[i].attribute=NULL;
    i++;
  }
  omFreeSize((ADDRESS)L->m,(L->nr+1)*sizeof(sleftv));
  omFreeBin((ADDRESS)L, slists_bin);
  omFreeSize((ADDRESS)s,strlen(name)+5);
}
#endif

//resolvente iiFindRes(char * name, int * len, int *typ0)
//{
//  char *s=(char *)omAlloc(strlen(name)+5);
//  int i=-1;
//  resolvente r;
//  idhdl h;
//
//  do
//  {
//    i++;
//    sprintf(s,"%s(%d)",name,i+1);
//    h=currRing->idroot->get(s,myynest);
//  } while (h!=NULL);
//  *len=i-1;
//  if (*len<=0)
//  {
//    Werror("no objects %s(1),.. found",name);
//    omFreeSize((ADDRESS)s,strlen(name)+5);
//    return NULL;
//  }
//  r=(ideal *)omAlloc(/*(len+1)*/ i*sizeof(ideal));
//  memset(r,0,(*len)*sizeof(ideal));
//  i=-1;
//  *typ0=MODUL_CMD;
//  while (i<(*len))
//  {
//    i++;
//    sprintf(s,"%s(%d)",name,i+1);
//    h=currRing->idroot->get(s,myynest);
//    if (h->typ != MODUL_CMD)
//    {
//      if ((i!=0) || (h->typ!=IDEAL_CMD))
//      {
//        Werror("%s is not of type module",s);
//        omFreeSize((ADDRESS)r,(*len)*sizeof(ideal));
//        omFreeSize((ADDRESS)s,strlen(name)+5);
//        return NULL;
//      }
//      *typ0=IDEAL_CMD;
//    }
//    if ((i>0) && (idIs0(r[i-1])))
//    {
//      *len=i-1;
//      break;
//    }
//    r[i]=IDIDEAL(h);
//  }
//  omFreeSize((ADDRESS)s,strlen(name)+5);
//  return r;
//}

static resolvente iiCopyRes(resolvente r, int l)
{
  int i;
  resolvente res=(ideal *)omAlloc0((l+1)*sizeof(ideal));

  for (i=0; i<l; i++)
    res[i]=idCopy(r[i]);
  return res;
}

BOOLEAN jjMINRES(leftv res, leftv v)
{
  int len=0;
  int typ0;
  resolvente rr=liFindRes((lists)v->Data(),&len,&typ0);
  if (rr==NULL) return TRUE;
  resolvente r=iiCopyRes(rr,len);

  syMinimizeResolvente(r,len,0);
  omFreeSize((ADDRESS)rr,len*sizeof(ideal));
  len++;
  res->data=(char *)liMakeResolv(r,len,-1,typ0,NULL);
  return FALSE;
}

BOOLEAN jjBETTI(leftv res, leftv u)
{
  sleftv tmp;
  memset(&tmp,0,sizeof(tmp));
  tmp.rtyp=INT_CMD;
  tmp.data=(void *)1;  
  if ((u->Typ()==IDEAL_CMD)
  || (u->Typ()==MODUL_CMD))
    return jjBETTI2_ID(res,u,&tmp);
  else
    return jjBETTI2(res,u,&tmp);
}

BOOLEAN jjBETTI2_ID(leftv res, leftv u, leftv v)
{
  lists l=(lists) omAllocBin(slists_bin);
  l->Init(1);
  l->m[0].rtyp=u->Typ();
  l->m[0].data=u->Data();
  l->m[0].attribute=u->attribute;
  sleftv tmp2;
  memset(&tmp2,0,sizeof(tmp2));
  tmp2.rtyp=LIST_CMD;
  tmp2.data=(void *)l;
  BOOLEAN r=jjBETTI2(res,&tmp2,v);
  l->m[0].data=NULL;
  l->m[0].attribute=NULL;
  l->m[0].rtyp=DEF_CMD;
  l->Clean();
  return r;
}

BOOLEAN jjBETTI2(leftv res, leftv u, leftv v)
{
  resolvente r;
  int len;
  int reg,typ0;
  lists l=(lists)u->Data();

  intvec *weights=NULL;
  int add_row_shift=0;
  intvec *ww=(intvec *)atGet(&(l->m[0]),"isHomog",INTVEC_CMD); 
  if (ww!=NULL)
  {
     weights=ivCopy(ww);
     add_row_shift = ww->min_in();
     (*weights) -= add_row_shift;
  }
  //Print("attr:%x\n",weights);

  r=liFindRes(l,&len,&typ0);
  if (r==NULL) return TRUE;
  res->data=(char *)syBetti(r,len,&reg,weights,(int)v->Data());
  omFreeSize((ADDRESS)r,(len)*sizeof(ideal));
  if (add_row_shift!=0) 
    atSet(res,omStrDup("rowShift"),(void*)add_row_shift,INT_CMD);
  if (weights!=NULL) delete weights;
  return FALSE;
}

int iiRegularity(lists L)
{
  int len,reg,typ0;

  resolvente r=liFindRes(L,&len,&typ0);

  if (r==NULL)
    return -2;
  intvec * dummy=syBetti(r,len,&reg);
  omFreeSize((ADDRESS)r,len*sizeof(ideal));
  delete dummy;
  return reg+1;
}

BOOLEAN iiDebugMarker=TRUE;
#define BREAK_LINE_LENGTH 80
void iiDebug()
{
  Print("\n-- break point in %s --\n",VoiceName());
  if (iiDebugMarker) VoiceBackTrack();
  char * s;
  iiDebugMarker=FALSE;
  s = (char *)omAlloc(BREAK_LINE_LENGTH+4);
  loop
  {
    memset(s,0,80);
    fe_fgets_stdin("",s,BREAK_LINE_LENGTH);
    if (s[BREAK_LINE_LENGTH-1]!='\0')
    {
      Print("line too long, max is %d chars\n",BREAK_LINE_LENGTH);
    }
    else
      break;
  }
  if (*s=='\n')
  {
    iiDebugMarker=TRUE;
  }
#if MDEBUG
  else if(strncmp(s,"cont;",5)==0)
  {
    iiDebugMarker=TRUE;
  }
#endif /* MDEBUG */
  else
  {
    strcat( s, "\n;~\n");
    newBuffer(s,BT_execute);
  }
}

lists scIndIndset(ideal S, BOOLEAN all, ideal Q)
{
  int i;
  indset save;
  lists res=(lists)omAlloc0Bin(slists_bin);

  hexist = hInit(S, Q, &hNexist);
  if ((hNexist == 0) || (hisModule!=0))
  {
    res->Init(0);
    return res;
  }
  save = ISet = (indset)omAlloc0Bin(indlist_bin);
  hMu = 0;
  hwork = (scfmon)omAlloc(hNexist * sizeof(scmon));
  hvar = (varset)omAlloc((pVariables + 1) * sizeof(int));
  hpure = (scmon)omAlloc((1 + (pVariables * pVariables)) * sizeof(Exponent_t));
  hrad = hexist;
  hNrad = hNexist;
  radmem = hCreate(pVariables - 1);
  hCo = pVariables + 1;
  hNvar = pVariables;
  hRadical(hrad, &hNrad, hNvar);
  hSupp(hrad, hNrad, hvar, &hNvar);
  if (hNvar)
  {
    hCo = hNvar;
    memset(hpure, 0, (pVariables + 1) * sizeof(Exponent_t));
    hPure(hrad, 0, &hNrad, hvar, hNvar, hpure, &hNpure);
    hLexR(hrad, hNrad, hvar, hNvar);
    hDimSolve(hpure, hNpure, hrad, hNrad, hvar, hNvar);
  }
  if (hCo && (hCo < pVariables))
  {
    hIndMult(hpure, hNpure, hrad, hNrad, hvar, hNvar);
  }
  if (hMu!=0)
  {
    ISet = save;
    hMu2 = 0;
    if (all && (hCo+1 < pVariables))
    {
      JSet = (indset)omAlloc0Bin(indlist_bin);
      hIndAllMult(hpure, hNpure, hrad, hNrad, hvar, hNvar);
      i=hMu+hMu2;
      res->Init(i);
      if (hMu2 == 0)
      {
        omFreeBin((ADDRESS)JSet, indlist_bin);
      }
    }
    else
    {
      res->Init(hMu);
    }
    for (i=0;i<hMu;i++)
    {
      res->m[i].data = (void *)save->set;
      res->m[i].rtyp = INTVEC_CMD;
      ISet = save;
      save = save->nx;
      omFreeBin((ADDRESS)ISet, indlist_bin);
    }
    omFreeBin((ADDRESS)save, indlist_bin);
    if (hMu2 != 0)
    {
      save = JSet;
      for (i=hMu;i<hMu+hMu2;i++)
      {
        res->m[i].data = (void *)save->set;
        res->m[i].rtyp = INTVEC_CMD;
        JSet = save;
        save = save->nx;
        omFreeBin((ADDRESS)JSet, indlist_bin);
      }
      omFreeBin((ADDRESS)save, indlist_bin);
    }
  }
  else
  {
    res->Init(0);
    omFreeBin((ADDRESS)ISet,  indlist_bin);
  }
  hKill(radmem, pVariables - 1);
  omFreeSize((ADDRESS)hpure, (1 + (pVariables * pVariables)) * sizeof(Exponent_t));
  omFreeSize((ADDRESS)hvar, (pVariables + 1) * sizeof(int));
  omFreeSize((ADDRESS)hwork, hNexist * sizeof(scmon));
  hDelete(hexist, hNexist);
  return res;
}

int iiDeclCommand(leftv sy, leftv name, int lev,int t, idhdl* root,BOOLEAN isring, BOOLEAN init_b)
{
  BOOLEAN res=FALSE;
  char *id = name->name;

  memset(sy,0,sizeof(sleftv));
  if ((name->name==NULL)||(isdigit(name->name[0])))
  {
    WerrorS("object to declare is not a name");
    res=TRUE;
  }
  else
  {
    //if (name->rtyp!=0)
    //{
    //  Warn("`%s` is already in use",name->name);
    //}
    {
      sy->data = (char *)enterid(id,lev,t,root,init_b);
    }
    if (sy->data!=NULL)
    {
      sy->rtyp=IDHDL;
      currid=sy->name=IDID((idhdl)sy->data);
      // name->name=NULL; /* used in enterid */
      //sy->e = NULL;
      if (name->next!=NULL)
      {
        sy->next=(leftv)omAllocBin(sleftv_bin);
        res=iiDeclCommand(sy->next,name->next,lev,t,root, isring);
      }
    }
    else res=TRUE;
  }
  name->CleanUp();
  return res;
}

BOOLEAN iiParameter(leftv p)
{
  if (iiCurrArgs==NULL)
  {
    if (strcmp(p->name,"#")==0) return FALSE;
    Werror("not enough arguments for proc %s",VoiceName());
    p->CleanUp();
    return TRUE;
  }
  leftv h=iiCurrArgs;
  if (strcmp(p->name,"#")==0)
  {
    iiCurrArgs=NULL;
  }
  else
  {
    iiCurrArgs=h->next;
    h->next=NULL;
  }
  BOOLEAN res=iiAssign(p,h);
  omFreeBin((ADDRESS)h, sleftv_bin);
  return res;
}

static BOOLEAN iiInternalExport (leftv v, int toLev)
{
  idhdl h=(idhdl)v->data;
  //Print("iiInternalExport('%s',%d)%s\n", v->name, toLev,"");
  if (IDLEV(h)==0) Warn("`%s` is already global",IDID(h));
  else
  {
    h=IDROOT->get(v->name,toLev);
    idhdl *root=&IDROOT;
    if ((h==NULL)&&(currRing!=NULL))
    {
      h=currRing->idroot->get(v->name,toLev);
      root=&currRing->idroot;
    }
    BOOLEAN keepring=FALSE;
    if ((h!=NULL)&&(IDLEV(h)==toLev))
    {
      if (IDTYP(h)==v->Typ())
      {
        if (((IDTYP(h)==RING_CMD)||(IDTYP(h)==QRING_CMD))
        && (v->Data()==IDDATA(h)))
        {
          IDRING(h)->ref++;
          keepring=TRUE;
          IDLEV(h)=toLev;
          //WarnS("keepring");
          return FALSE;
        }
        if (BVERBOSE(V_REDEFINE))
        {
          Warn("redefining %s",IDID(h));
        }
#ifdef USE_IILOCALRING
        if (iiLocalRing[0]==IDRING(h) && (!keepring)) iiLocalRing[0]=NULL;
#else
        proclevel *p=procstack;
        while (p->next!=NULL) p=p->next;
        if ((p->cRing==IDRING(h)) && (!keepring))
        {
          p->cRing=NULL;
          p->cRingHdl=NULL;
        }
#endif
        killhdl2(h,root,currRing);
      }
      else
      {
        return TRUE;
      }
    }
    h=(idhdl)v->data;
    IDLEV(h)=toLev;
    if (keepring) IDRING(h)->ref--;
    iiNoKeepRing=FALSE;
    //Print("export %s\n",IDID(h));
  }
  return FALSE;
}

#ifdef HAVE_NS
BOOLEAN iiInternalExport (leftv v, int toLev, idhdl roothdl)
{
  idhdl h=(idhdl)v->data;
  if(h==NULL)
  {
    Warn("'%s': no such identifier\n", v->name);
    return FALSE;
  }
  package frompack=v->req_packhdl; 
  if (frompack==NULL) frompack=currPack;
  package rootpack = IDPACKAGE(roothdl);
//  Print("iiInternalExport('%s',%d,%s->%s) typ:%d\n", v->name, toLev, IDID(currPackHdl),IDID(roothdl),v->Typ());
  if ((RingDependend(IDTYP(h)))
  || ((IDTYP(h)==LIST_CMD)
     && (lRingDependend(IDLIST(h)))
     )
  )
  {
    //Print("// ==> Ringdependent set nesting to 0\n");
    return (iiInternalExport(v, toLev));
  }
  else
  {
    IDLEV(h)=toLev;
    v->req_packhdl=rootpack;
    if (h==frompack->idroot)
    {
      frompack->idroot=h->next;
    }
    else
    {
      idhdl hh=frompack->idroot;
      while ((hh!=NULL) && (hh->next!=h))
        hh=hh->next;
      if ((hh!=NULL) && (hh->next==h))
        hh->next=h->next;
      else
      {
        Werror("`%s` not found",v->Name());
        return TRUE;
      }
    }
    h->next=rootpack->idroot;
    rootpack->idroot=h;
  }
  return FALSE;
}
#endif /* HAVE_NS */

BOOLEAN iiExport (leftv v, int toLev)
{
#ifdef HAVE_NS
  checkall();
#endif
  BOOLEAN nok=FALSE;
  leftv r=v;
  while (v!=NULL)
  {
    if ((v->name==NULL)||(v->rtyp==0)||(v->e!=NULL))
    {
      WerrorS("cannot export");
      nok=TRUE;
    }
    else
    {
      if(iiInternalExport(v, toLev))
      {
        r->CleanUp();
        return TRUE;
      }
    }
    v=v->next;
  }
  r->CleanUp();
#ifdef HAVE_NS
  checkall();
#endif
  return nok;
}

/*assume root!=idroot*/
#ifdef HAVE_NS
BOOLEAN iiExport (leftv v, int toLev, idhdl root)
{
  checkall();
  //  Print("iiExport1: pack=%s\n",IDID(root));
  BOOLEAN nok=FALSE;
  leftv rv=v;
  while (v!=NULL)
  {
    if ((v->name==NULL)||(v->rtyp==0)||(v->e!=NULL)
    )
    {
      WerrorS("cannot export");
      nok=TRUE;
    }
    else
    {
      idhdl old=root->get(v->name,toLev);
      if (old!=NULL)
      {
        if (IDTYP(old)==v->Typ())
        {
          if (BVERBOSE(V_REDEFINE))
          {
            Warn("redefining %s",IDID(old));
          }
          killhdl2(old,&root,currRing);
        }
        else
        {
          rv->CleanUp();
          return TRUE;
        }
      }
      //Print("iiExport: pack=%s\n",IDID(root));
      if(iiInternalExport(v, toLev, root))
      {
        rv->CleanUp();
        return TRUE;
      }
    }
    v=v->next;
  }
  rv->CleanUp();
  checkall();
  return nok;
}
#endif

BOOLEAN iiCheckRing(int i)
{
  if (currRingHdl==NULL)
  {
    #ifdef SIQ
    if (siq<=0)
    {
    #endif
      if (RingDependend(i))
      {
        WerrorS("no ring active");
        return TRUE;
      }
    #ifdef SIQ
    }
    #endif
  }
  return FALSE;
}

poly    iiHighCorner(ideal I, int ak)
{
  int i;
  if(!idIsZeroDim(I)) return NULL; // not zero-dim.
  poly po=NULL;
  if (currRing->OrdSgn== -1)
  {
    scComputeHC(I,currQuotient,ak,po);
    if (po!=NULL)
    {
      pGetCoeff(po)=nInit(1);
      for (i=pVariables; i>0; i--)
      {
        if (pGetExp(po, i) > 0) pDecrExp(po,i);
      }
      pSetComp(po,ak);
      pSetm(po);
    }
  }
  else
    po=pOne();
  return po;
}

#ifdef HAVE_NS
void iiCheckPack(package &p)
{
  if (p==basePack) return;

  idhdl t=basePack->idroot;

  while ((t!=NULL) && (IDTYP(t)!=PACKAGE_CMD) && (IDPACKAGE(t)!=p)) t=t->next;

  if (t==NULL)
  {
    WarnS("package not found\n");
    p=basePack;
  }
  return;
}
#endif

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

idhdl rFindHdl(ring r, idhdl n, idhdl w)
{
  idhdl h=rSimpleFindHdl(r,IDROOT,n);
  if (h!=NULL)  return h;
#ifdef HAVE_NS
  if (IDROOT!=basePack->idroot) h=rSimpleFindHdl(r,basePack->idroot,n);
  if (h!=NULL)  return h;
  proclevel *p=procstack;
  while(p!=NULL)
  {
    if ((p->cPack!=basePack)
    && (p->cPack!=currPack))
      h=rSimpleFindHdl(r,p->cPack->idroot,n);
    if (h!=NULL)  return h;
    p=p->next;
  }
  idhdl tmp=basePack->idroot;
  while (tmp!=NULL)
  {
    if (IDTYP(tmp)==PACKAGE_CMD)
      h=rSimpleFindHdl(r,IDPACKAGE(tmp)->idroot,n);
    if (h!=NULL)  return h;
    tmp=IDNEXT(tmp);
  }
#endif
  return NULL;
}

void rDecomposeCF(leftv h,const ring r,const ring R)
{
  lists L=(lists)omAlloc0Bin(slists_bin);
  L->Init(4);
  h->rtyp=LIST_CMD;
  h->data=(void *)L;
  // 0: char/ cf - ring
  // 1: list (var)
  // 2: list (ord)
  // 3: qideal
  // ----------------------------------------
  // 0: char/ cf - ring
  L->m[0].rtyp=INT_CMD;
  L->m[0].data=(void *)r->ch;
  // ----------------------------------------
  // 1: list (var)
  lists LL=(lists)omAlloc0Bin(slists_bin);
  LL->Init(r->N);
  int i;
  for(i=0; i<r->N; i++)
  {
    LL->m[i].rtyp=STRING_CMD;
    LL->m[i].data=(void *)omStrDup(r->names[i]);
  }
  L->m[1].rtyp=LIST_CMD;
  L->m[1].data=(void *)LL;
  // ----------------------------------------
  // 2: list (ord)
  LL=(lists)omAlloc0Bin(slists_bin);
  i=rBlocks(r)-1;
  LL->Init(i);
  i--;
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
    if (r->block1[i]-r->block0[i] >=0 )
    {
      j=r->block1[i]-r->block0[i];
      iv=new intvec(j+1);
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
    else
    {
      iv=new intvec(1);
    }
    LLL->m[1].rtyp=INTVEC_CMD;
    LLL->m[1].data=(void *)iv;
    LL->m[i].data=(void *)LLL;
  }
  L->m[2].rtyp=LIST_CMD;
  L->m[2].data=(void *)LL;
  // ----------------------------------------
  // 3: qideal
  L->m[3].rtyp=IDEAL_CMD;
  if (R->minpoly==NULL)
    L->m[3].data=(void *)idInit(1,1);
  else
  {
    ideal I=idInit(1,1);
    L->m[3].data=(void *)I;
    I->m[0]=pOne();
    pSetCoeff(I->m[0],R->minpoly);
  }
  // ----------------------------------------
}
void rDecomposeC(leftv h,const ring R)
{
  lists L=(lists)omAlloc0Bin(slists_bin);
  L->Init(4);
  h->rtyp=LIST_CMD;
  h->data=(void *)L;
  // 0: char/ cf - ring
  // 1: list (var)
  // 2: list (ord)
  // 3: qideal
  // ----------------------------------------
  // 0: char/ cf - ring
  L->m[0].rtyp=INT_CMD;
  L->m[0].data=(void *)R->ch;
  // ----------------------------------------
  // 1: list (var)
  lists LL=(lists)omAlloc0Bin(slists_bin);
  LL->Init(1);
    LL->m[0].rtyp=STRING_CMD;
    LL->m[0].data=(void *)omStrDup(R->parameter[0]);
  L->m[1].rtyp=LIST_CMD;
  L->m[1].data=(void *)LL;
  // ----------------------------------------
  // 2: list (ord)
  LL=(lists)omAlloc0Bin(slists_bin);
  LL->Init(1);
  lists LLL;
  {
    intvec *iv;
    int j;
    LL->m[0].rtyp=LIST_CMD;
    LLL=(lists)omAlloc0Bin(slists_bin);
    LLL->Init(2);
    LLL->m[0].rtyp=STRING_CMD;
    LLL->m[0].data=(void *)omStrDup("lp");
    {
      iv=new intvec(1);
      for(;j>=0; j--) (*iv)[0]=1;
    }
    LLL->m[1].rtyp=INTVEC_CMD;
    LLL->m[1].data=(void *)iv;
    LL->m[0].data=(void *)LLL;
  }
  L->m[2].rtyp=LIST_CMD;
  L->m[2].data=(void *)LL;
  // ----------------------------------------
  // 3: qideal
  L->m[3].rtyp=IDEAL_CMD;
  {
    ideal I=idInit(1,1);
    L->m[3].data=(void *)I;
    //I->m[0]=pOne();
    //pSetCoeff(I->m[0],R->minpoly);
  }
  // ----------------------------------------
}

lists rDecompose(ring r)
{
  // 0: char/ cf - ring
  // 1: list (var)
  // 2: list (ord)
  // 3: qideal
  // possibly:
  // 4: C
  // 5: D
  lists L=(lists)omAlloc0Bin(slists_bin);
  if (rIsPluralRing(r))
    L->Init(6);
  else
    L->Init(4);
  // ----------------------------------------
  // 0: char/ cf - ring
  #if 1 /* TODO */
  if (rIsExtension(r))
  {
    if (rField_is_long_C(r))
      rDecomposeC(&(L->m[0]),r);
    else
      rDecomposeCF(&(L->m[0]),r->algring,r);
  }
  else
  #endif
  {
    L->m[0].rtyp=INT_CMD;
    L->m[0].data=(void *)r->ch;
  }
  // ----------------------------------------
  // 1: list (var)
  lists LL=(lists)omAlloc0Bin(slists_bin);
  LL->Init(r->N);
  int i;
  for(i=0; i<r->N; i++)
  {
    LL->m[i].rtyp=STRING_CMD;
    LL->m[i].data=(void *)omStrDup(r->names[i]);
  }
  L->m[1].rtyp=LIST_CMD;
  L->m[1].data=(void *)LL;
  // ----------------------------------------
  // 2: list (ord)
  LL=(lists)omAlloc0Bin(slists_bin);
  i=rBlocks(r)-1;
  LL->Init(i);
  i--;
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
    if (r->block1[i]-r->block0[i] >=0 )
    {
      j=r->block1[i]-r->block0[i];
      iv=new intvec(j+1);
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
    else
    {
      iv=new intvec(1);
    }
    LLL->m[1].rtyp=INTVEC_CMD;
    LLL->m[1].data=(void *)iv;
    LL->m[i].data=(void *)LLL;
  }
  L->m[2].rtyp=LIST_CMD;
  L->m[2].data=(void *)LL;
  // ----------------------------------------
  // 3: qideal
  L->m[3].rtyp=IDEAL_CMD;
  if (r->qideal==NULL)
    L->m[3].data=(void *)idInit(1,1);
  else
    L->m[3].data=(void *)idCopy(r->qideal);
  // ----------------------------------------
  #ifdef HAVE_PLURAL
  if (rIsPluralRing(r))
  {
    L->m[4].rtyp=MATRIX_CMD;
    L->m[4].data=(void *)mpCopy(r->nc->C);
    L->m[5].rtyp=MATRIX_CMD;
    L->m[5].data=(void *)mpCopy(r->nc->D);
  }
  #endif
  return L;
}

ring rCompose(lists  L)
{
  if ((L->nr!=3)
#ifdef HAVE_PLURAL
  &&(L->nr!=5)
#endif
  )
    return NULL;
  // 0: char/ cf - ring
  // 1: list (var)
  // 2: list (ord)
  // 3: qideal
  // possibly:
  // 4: C
  // 5: D
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
    if (R->algring->ch>0)
       R->ch= -R->algring->ch;
    else
       R->ch=1;
    R->parameter=R->algring->names;
    R->P=R->algring->N;
    if (R->algring->qideal!=NULL)
    {
      R->minpoly=pGetCoeff(R->algring->qideal->m[0]);
    }
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
      if (v->m[j].Typ()!=LIST_CMD)
      {
        WerrorS("ordering must be list of lists");
        goto rCompose_err;
      }
      lists vv=(lists)v->m[j].Data();
      if ((vv->nr!=1)
      || (vv->m[0].Typ()!=STRING_CMD)
      || ((vv->m[1].Typ()!=INTVEC_CMD) && (vv->m[1].Typ()!=INT_CMD)))
      {
        WerrorS("ordering name must be a (string,intvec)");
        goto rCompose_err;
      }
      R->order[j]=rOrderName(omStrDup((char*)vv->m[0].Data())); // assume STRING
      if (j==0) R->block0[0]=1;
      else      R->block0[j]=R->block1[j-1]+1;
      intvec *iv;
      if (vv->m[1].Typ()==INT_CMD)
        iv=new intvec((int)vv->m[1].Data(),(int)vv->m[1].Data());
      else
        iv=ivCopy((intvec*)vv->m[1].Data()); //assume INTVEC
      R->block1[j]=max(R->block0[j],R->block0[j]+iv->length()-1);
      int i;
      switch (R->order[j])
      {
         case ringorder_ws:
         case ringorder_Ws:
            R->OrdSgn=-1;
         case ringorder_wp:
         case ringorder_Wp:
           R->wvhdl[j] =( int *)omAlloc((iv->length())*sizeof(int));
           for (i=0; i<iv->length();i++) R->wvhdl[j][i]=(*iv)[i];
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

  // ---------------------------------------------------------------
  #ifdef HAVE_PLURAL
  if (L->nr==5)
  {
    if (nc_CallPlural((matrix)L->m[4].Data(),(matrix)L->m[5].Data(),
        NULL,NULL,R))
      goto rCompose_err;
  }
  #endif
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

// from matpol.cc

/*2
* compute the jacobi matrix of an ideal
*/
BOOLEAN mpJacobi(leftv res,leftv a)
{
  int     i,j;
  matrix result;
  ideal id=(ideal)a->Data();

  result =mpNew(IDELEMS(id),pVariables);
  for (i=1; i<=IDELEMS(id); i++)
  {
    for (j=1; j<=pVariables; j++)
    {
      MATELEM(result,i,j) = pDiff(id->m[i-1],j);
    }
  }
  res->data=(char *)result;
  return FALSE;
}

/*2
* returns the Koszul-matrix of degree d of a vectorspace with dimension n
* uses the first n entrees of id, if id <> NULL
*/
BOOLEAN mpKoszul(leftv res,leftv c/*ip*/, leftv b/*in*/, leftv id)
{
  int n=(int)b->Data();
  int d=(int)c->Data();
  int     k,l,sign,row,col;
  matrix  result;
  ideal temp;
  BOOLEAN bo;
  poly    p;

  if ((d>n) || (d<1) || (n<1))
  {
    res->data=(char *)mpNew(1,1);
    return FALSE;
  }
  int *choise = (int*)omAlloc(d*sizeof(int));
  if (id==NULL)
    temp=idMaxIdeal(1);
  else
    temp=(ideal)id->Data();

  k = binom(n,d);
  l = k*d;
  l /= n-d+1;
  result =mpNew(l,k);
  col = 1;
  idInitChoise(d,1,n,&bo,choise);
  while (!bo)
  {
    sign = 1;
    for (l=1;l<=d;l++)
    {
      if (choise[l-1]<=IDELEMS(temp))
      {
        p = pCopy(temp->m[choise[l-1]-1]);
        if (sign == -1) p = pNeg(p);
        sign *= -1;
        row = idGetNumberOfChoise(l-1,d,1,n,choise);
        MATELEM(result,row,col) = p;
      }
    }
    col++;
    idGetNextChoise(d,n,&bo,choise);
  }
  if (id==NULL) idDelete(&temp);

  res->data=(char *)result;
  return FALSE;
}

// from syz1.cc
/*2
* read out the Betti numbers from resolution
* (interpreter interface)
*/
BOOLEAN syBetti2(leftv res, leftv u, leftv w)
{
  syStrategy syzstr=(syStrategy)u->Data();
  BOOLEAN minim=(int)w->Data();
  int row_shift=0;
  int add_row_shift=0;
  intvec *weights=NULL;
  intvec *ww=(intvec *)atGet(u,"isHomog",INTVEC_CMD);
  if (ww!=NULL)
  {
     weights=ivCopy(ww);
     add_row_shift = ww->min_in();
     (*weights) -= add_row_shift;
  }
  res->data=(void *)syBettiOfComputation(syzstr,minim,&row_shift,weights);
  //row_shift += add_row_shift;
  //Print("row_shift=%d, add_row_shift=%d\n",row_shift,add_row_shift);
  atSet(res,omStrDup("rowShift"),(void*)add_row_shift,INT_CMD);
  return FALSE;
}
BOOLEAN syBetti1(leftv res, leftv u)
{
  sleftv tmp;
  memset(&tmp,0,sizeof(tmp));
  tmp.rtyp=INT_CMD;
  tmp.data=(void *)1;  
  return syBetti2(res,u,&tmp);
}

/*3
* converts a resolution into a list of modules
*/
lists syConvRes(syStrategy syzstr,BOOLEAN toDel)
{
  if ((syzstr->fullres==NULL) && (syzstr->minres==NULL))
  {
    if (syzstr->hilb_coeffs==NULL)
    {
      syzstr->fullres = syReorder(syzstr->res,syzstr->length,syzstr);
    }
    else
    {
      syzstr->minres = syReorder(syzstr->orderedRes,syzstr->length,syzstr);
      syKillEmptyEntres(syzstr->minres,syzstr->length);
    }
  }
  resolvente tr;
  int typ0=IDEAL_CMD;
  if (syzstr->minres!=NULL)
    tr = syzstr->minres;
  else
    tr = syzstr->fullres;
  resolvente trueres=NULL;
  intvec ** w=NULL;
  if (syzstr->length>0)
  {
    trueres=(resolvente)omAlloc0((syzstr->length)*sizeof(ideal));
    for (int i=(syzstr->length)-1;i>=0;i--)
    {
      if (tr[i]!=NULL)
      {
        trueres[i] = idCopy(tr[i]);
      }
    }
    if (idRankFreeModule(trueres[0]) > 0)
      typ0 = MODUL_CMD;
    if (syzstr->weights!=NULL)
    {
      w = (intvec**)omAlloc0((syzstr->length)*sizeof(intvec*));
      for (int i=(syzstr->length)-1;i>=0;i--)
      {
        if (syzstr->weights[i]!=NULL) w[i] = ivCopy(syzstr->weights[i]);
      }
    }
  }
  lists li = liMakeResolv(trueres,syzstr->length,syzstr->list_length,typ0,w);
  if (w != NULL) omFreeSize(w, (syzstr->length)*sizeof(intvec*));
  if (toDel) syKillComputation(syzstr);
  return li;
}

/*3
* converts a list of modules into a resolution
*/
syStrategy syConvList(lists li,BOOLEAN toDel)
{
  int typ0;
  syStrategy result=(syStrategy)omAlloc0(sizeof(ssyStrategy));

  resolvente fr = liFindRes(li,&(result->length),&typ0,&(result->weights));
  if (fr != NULL)
  {

    result->fullres = (resolvente)omAlloc0((result->length+1)*sizeof(ideal));
    for (int i=result->length-1;i>=0;i--)
    {
      if (fr[i]!=NULL)
        result->fullres[i] = idCopy(fr[i]);
    }
    result->list_length=result->length;
    omFreeSize((ADDRESS)fr,(result->length)*sizeof(ideal));
  }
  else
  {
    omFreeSize(result, sizeof(ssyStrategy));
    result = NULL;
  }
  if (toDel) li->Clean();
  return result;
}

/*3
* converts a list of modules into a minimal resolution
*/
syStrategy syForceMin(lists li)
{
  int typ0;
  syStrategy result=(syStrategy)omAlloc0(sizeof(ssyStrategy));

  resolvente fr = liFindRes(li,&(result->length),&typ0);
  result->minres = (resolvente)omAlloc0((result->length+1)*sizeof(ideal));
  for (int i=result->length-1;i>=0;i--)
  {
    if (fr[i]!=NULL)
      result->minres[i] = idCopy(fr[i]);
  }
  omFreeSize((ADDRESS)fr,(result->length)*sizeof(ideal));
  return result;
}
// from weight.cc
BOOLEAN kWeight(leftv res,leftv id)
{
  ideal F=(ideal)id->Data();
  intvec * iv = new intvec(pVariables);
  polyset s;
  int  sl, n, i;
  int  *x;

  res->data=(char *)iv;
  s = F->m;
  sl = IDELEMS(F) - 1;
  n = pVariables;
  wNsqr = (double)2.0 / (double)n;
  wFunctional = wFunctionalBuch;
  x = (int * )omAlloc(2 * (n + 1) * sizeof(int));
  wCall(s, sl, x);
  for (i = n; i!=0; i--)
    (*iv)[i-1] = x[i + n + 1];
  omFreeSize((ADDRESS)x, 2 * (n + 1) * sizeof(int));
  return FALSE;
}

BOOLEAN kQHWeight(leftv res,leftv v)
{
  res->data=(char *)idQHomWeight((ideal)v->Data());
  if (res->data==NULL)
    res->data=(char *)new intvec(pVariables);
  return FALSE;
}
/*==============================================================*/
// from clapsing.cc
#if 0
BOOLEAN jjIS_SQR_FREE(leftv res, leftv u)
{
  BOOLEAN b=singclap_factorize((poly)(u->Data()), &v, 0);
  res->data=(void *)b;
}
#endif

BOOLEAN jjRESULTANT(leftv res, leftv u, leftv v, leftv w)
{
  res->data=singclap_resultant((poly)u->Data(),(poly)v->Data(), (poly)w->Data());
  return errorreported;
}
BOOLEAN jjCHARSERIES(leftv res, leftv u)
{
  res->data=singclap_irrCharSeries((ideal)u->Data());
  return (res->data==NULL);
}

// from semic.cc
#ifdef HAVE_SPECTRUM

// ----------------------------------------------------------------------------
//  Initialize a  spectrum  deep from another  spectrum
// ----------------------------------------------------------------------------

void spectrum::copy_deep( const spectrum &spec )
{
    mu = spec.mu;
    pg = spec.pg;
    n  = spec.n;

    copy_new( n );

    for( int i=0; i<n; i++ )
    {
        s[i] = spec.s[i];
        w[i] = spec.w[i];
    }
}

// ----------------------------------------------------------------------------
//  Initialize a  spectrum  deep from a  singular  lists
// ----------------------------------------------------------------------------

void spectrum::copy_deep( lists l )
{
    mu = (int)(l->m[0].Data( ));
    pg = (int)(l->m[1].Data( ));
    n  = (int)(l->m[2].Data( ));

    copy_new( n );

    intvec  *num = (intvec*)l->m[3].Data( );
    intvec  *den = (intvec*)l->m[4].Data( );
    intvec  *mul = (intvec*)l->m[5].Data( );

    for( int i=0; i<n; i++ )
    {
        s[i] = (Rational)((*num)[i])/(Rational)((*den)[i]);
        w[i] = (*mul)[i];
    }
}

// ----------------------------------------------------------------------------
//  singular lists  constructor for  spectrum
// ----------------------------------------------------------------------------

spectrum::spectrum( lists l )
{
    copy_deep( l );
}

// ----------------------------------------------------------------------------
//  generate a Singular  lists  from a spectrum
// ----------------------------------------------------------------------------

lists   spectrum::thelist( void )
{
    lists   L  = (lists)omAllocBin( slists_bin);

    L->Init( 6 );

    intvec            *num  = new intvec( n );
    intvec            *den  = new intvec( n );
    intvec            *mult = new intvec( n );

    for( int i=0; i<n; i++ )
    {
        (*num) [i] = s[i].get_num_si( );
        (*den) [i] = s[i].get_den_si( );
        (*mult)[i] = w[i];
    }

    L->m[0].rtyp = INT_CMD;    //  milnor number
    L->m[1].rtyp = INT_CMD;    //  geometrical genus
    L->m[2].rtyp = INT_CMD;    //  # of spectrum numbers
    L->m[3].rtyp = INTVEC_CMD; //  numerators
    L->m[4].rtyp = INTVEC_CMD; //  denomiantors
    L->m[5].rtyp = INTVEC_CMD; //  multiplicities

    L->m[0].data = (void*)mu;
    L->m[1].data = (void*)pg;
    L->m[2].data = (void*)n;
    L->m[3].data = (void*)num;
    L->m[4].data = (void*)den;
    L->m[5].data = (void*)mult;

    return  L;
}
// from spectrum.cc
// ----------------------------------------------------------------------------
//  print out an error message for a spectrum list
// ----------------------------------------------------------------------------

void    list_error( semicState state )
{
    switch( state )
    {
        case semicListTooShort:
            WerrorS( "the list is too short" );
            break;
        case semicListTooLong:
            WerrorS( "the list is too long" );
            break;

        case semicListFirstElementWrongType:
            WerrorS( "first element of the list should be int" );
            break;
        case semicListSecondElementWrongType:
            WerrorS( "second element of the list should be int" );
            break;
        case semicListThirdElementWrongType:
            WerrorS( "third element of the list should be int" );
            break;
        case semicListFourthElementWrongType:
            WerrorS( "fourth element of the list should be intvec" );
            break;
        case semicListFifthElementWrongType:
            WerrorS( "fifth element of the list should be intvec" );
            break;
        case semicListSixthElementWrongType:
            WerrorS( "sixth element of the list should be intvec" );
            break;

        case semicListNNegative:
            WerrorS( "first element of the list should be positive" );
            break;
        case semicListWrongNumberOfNumerators:
            WerrorS( "wrong number of numerators" );
            break;
        case semicListWrongNumberOfDenominators:
            WerrorS( "wrong number of denominators" );
            break;
        case semicListWrongNumberOfMultiplicities:
            WerrorS( "wrong number of multiplicities" );
            break;

        case semicListMuNegative:
            WerrorS( "the Milnor number should be positive" );
            break;
        case semicListPgNegative:
            WerrorS( "the geometrical genus should be nonnegative" );
            break;
        case semicListNumNegative:
            WerrorS( "all numerators should be positive" );
            break;
        case semicListDenNegative:
            WerrorS( "all denominators should be positive" );
            break;
        case semicListMulNegative:
            WerrorS( "all multiplicities should be positive" );
            break;

        case semicListNotSymmetric:
            WerrorS( "it is not symmetric" );
            break;
        case semicListNotMonotonous:
            WerrorS( "it is not monotonous" );
            break;

        case semicListMilnorWrong:
            WerrorS( "the Milnor number is wrong" );
            break;
        case semicListPGWrong:
            WerrorS( "the geometrical genus is wrong" );
            break;

        default:
            WerrorS( "unspecific error" );
            break;
    }
}
// ----------------------------------------------------------------------------
//  this is the main spectrum computation function
// ----------------------------------------------------------------------------

spectrumState   spectrumCompute( poly h,lists *L,int fast )
{
  int i,j;

  #ifdef SPECTRUM_DEBUG
  #ifdef SPECTRUM_PRINT
  #ifdef SPECTRUM_IOSTREAM
    cout << "spectrumCompute\n";
    if( fast==0 ) cout << "    no optimization" << endl;
    if( fast==1 ) cout << "    weight optimization" << endl;
    if( fast==2 ) cout << "    symmetry optimization" << endl;
  #else
    fprintf( stdout,"spectrumCompute\n" );
    if( fast==0 ) fprintf( stdout,"    no optimization\n" );
    if( fast==1 ) fprintf( stdout,"    weight optimization\n" );
    if( fast==2 ) fprintf( stdout,"    symmetry optimization\n" );
  #endif
  #endif
  #endif

  // ----------------------
  //  check if  h  is zero
  // ----------------------

  if( h==(poly)NULL )
  {
    return  spectrumZero;
  }

  // ----------------------------------
  //  check if  h  has a constant term
  // ----------------------------------

  if( hasConstTerm( h ) )
  {
    return  spectrumBadPoly;
  }

  // --------------------------------
  //  check if  h  has a linear term
  // --------------------------------

  if( hasLinearTerm( h ) )
  {
    *L = (lists)omAllocBin( slists_bin);
    (*L)->Init( 1 );
    (*L)->m[0].rtyp = INT_CMD;    //  milnor number
    /* (*L)->m[0].data = (void*)0;a  -- done by Init */

    return  spectrumNoSingularity;
  }

  // ----------------------------------
  //  compute the jacobi ideal of  (h)
  // ----------------------------------

  ideal J = NULL;
  J = idInit( pVariables,1 );

  #ifdef SPECTRUM_DEBUG
  #ifdef SPECTRUM_PRINT
  #ifdef SPECTRUM_IOSTREAM
    cout << "\n   computing the Jacobi ideal...\n";
  #else
    fprintf( stdout,"\n   computing the Jacobi ideal...\n" );
  #endif
  #endif
  #endif

  for( i=0; i<pVariables; i++ )
  {
    J->m[i] = pDiff( h,i+1); //j );

    #ifdef SPECTRUM_DEBUG
    #ifdef SPECTRUM_PRINT
    #ifdef SPECTRUM_IOSTREAM
      cout << "        ";
    #else
      fprintf( stdout,"        " );
    #endif
      pWrite( J->m[i] );
    #endif
    #endif
  }

  // --------------------------------------------
  //  compute a standard basis  stdJ  of  jac(h)
  // --------------------------------------------

  #ifdef SPECTRUM_DEBUG
  #ifdef SPECTRUM_PRINT
  #ifdef SPECTRUM_IOSTREAM
    cout << endl;
    cout << "    computing a standard basis..." << endl;
  #else
    fprintf( stdout,"\n" );
    fprintf( stdout,"    computing a standard basis...\n" );
  #endif
  #endif
  #endif

  ideal stdJ = kStd(J,currQuotient,isNotHomog,NULL);
  idSkipZeroes( stdJ );

  #ifdef SPECTRUM_DEBUG
  #ifdef SPECTRUM_PRINT
    for( i=0; i<IDELEMS(stdJ); i++ )
    {
      #ifdef SPECTRUM_IOSTREAM
        cout << "        ";
      #else
        fprintf( stdout,"        " );
      #endif

      pWrite( stdJ->m[i] );
    }
  #endif
  #endif

  idDelete( &J );

  // ------------------------------------------
  //  check if the  h  has a singularity
  // ------------------------------------------

  if( hasOne( stdJ ) )
  {
    // -------------------------------
    //  h is smooth in the origin
    //  return only the Milnor number
    // -------------------------------

    *L = (lists)omAllocBin( slists_bin);
    (*L)->Init( 1 );
    (*L)->m[0].rtyp = INT_CMD;    //  milnor number
    /* (*L)->m[0].data = (void*)0;a  -- done by Init */

    return  spectrumNoSingularity;
  }

  // ------------------------------------------
  //  check if the singularity  h  is isolated
  // ------------------------------------------

  for( i=pVariables; i>0; i-- )
  {
    if( hasAxis( stdJ,i )==FALSE )
    {
      return  spectrumNotIsolated;
    }
  }

  // ------------------------------------------
  //  compute the highest corner  hc  of  stdJ
  // ------------------------------------------

  #ifdef SPECTRUM_DEBUG
  #ifdef SPECTRUM_PRINT
  #ifdef SPECTRUM_IOSTREAM
    cout << "\n    computing the highest corner...\n";
  #else
    fprintf( stdout,"\n    computing the highest corner...\n" );
  #endif
  #endif
  #endif

  poly hc = (poly)NULL;

  scComputeHC( stdJ,currQuotient, 0,hc );

  if( hc!=(poly)NULL )
  {
    pGetCoeff(hc) = nInit(1);

    for( i=pVariables; i>0; i-- )
    {
      if( pGetExp( hc,i )>0 ) pDecrExp( hc,i );
    }
    pSetm( hc );
  }
  else
  {
    return  spectrumNoHC;
  }

  #ifdef SPECTRUM_DEBUG
  #ifdef SPECTRUM_PRINT
  #ifdef SPECTRUM_IOSTREAM
    cout << "       ";
  #else
    fprintf( stdout,"       " );
  #endif
    pWrite( hc );
  #endif
  #endif

  // ----------------------------------------
  //  compute the Newton polygon  nph  of  h
  // ----------------------------------------

  #ifdef SPECTRUM_DEBUG
  #ifdef SPECTRUM_PRINT
  #ifdef SPECTRUM_IOSTREAM
    cout << "\n    computing the newton polygon...\n";
  #else
    fprintf( stdout,"\n    computing the newton polygon...\n" );
  #endif
  #endif
  #endif

  newtonPolygon nph( h );

  #ifdef SPECTRUM_DEBUG
  #ifdef SPECTRUM_PRINT
    cout << nph;
  #endif
  #endif

  // -----------------------------------------------
  //  compute the weight corner  wc  of  (stdj,nph)
  // -----------------------------------------------

  #ifdef SPECTRUM_DEBUG
  #ifdef SPECTRUM_PRINT
  #ifdef SPECTRUM_IOSTREAM
    cout << "\n    computing the weight corner...\n";
  #else
    fprintf( stdout,"\n    computing the weight corner...\n" );
  #endif
  #endif
  #endif

  poly    wc = ( fast==0 ? pCopy( hc ) :
               ( fast==1 ? computeWC( nph,(Rational)pVariables ) :
              /* fast==2 */computeWC( nph,((Rational)pVariables)/(Rational)2 ) ) );

  #ifdef SPECTRUM_DEBUG
  #ifdef SPECTRUM_PRINT
  #ifdef SPECTRUM_IOSTREAM
    cout << "        ";
  #else
    fprintf( stdout,"        " );
  #endif
    pWrite( wc );
  #endif
  #endif

  // -------------
  //  compute  NF
  // -------------

  #ifdef SPECTRUM_DEBUG
  #ifdef SPECTRUM_PRINT
  #ifdef SPECTRUM_IOSTREAM
    cout << "\n    computing NF...\n" << endl;
  #else
    fprintf( stdout,"\n    computing NF...\n" );
  #endif
  #endif
  #endif

  spectrumPolyList NF( &nph );

  computeNF( stdJ,hc,wc,&NF );

  #ifdef SPECTRUM_DEBUG
  #ifdef SPECTRUM_PRINT
    cout << NF;
  #ifdef SPECTRUM_IOSTREAM
    cout << endl;
  #else
    fprintf( stdout,"\n" );
  #endif
  #endif
  #endif

  // ----------------------------
  //  compute the spectrum of  h
  // ----------------------------

  return  NF.spectrum( L,fast );
}

// ----------------------------------------------------------------------------
//  this procedure is called from the interpreter
// ----------------------------------------------------------------------------
//  first  = polynomial
//  result = list of spectrum numbers
// ----------------------------------------------------------------------------

BOOLEAN spectrumProc( leftv result,leftv first )
{
  spectrumState state = spectrumOK;

  // -------------------
  //  check consistency
  // -------------------

  //  check for a local ring

  if( !ringIsLocal( ) )
  {
    WerrorS( "only works for local orderings" );
    state = spectrumWrongRing;
  }

  //  no quotient rings are allowed

  else if( currRing->qideal != NULL )
  {
    WerrorS( "does not work in quotient rings" );
    state = spectrumWrongRing;
  }
  else
  {
    lists   L    = (lists)NULL;
    int     flag = 1; // weight corner optimization is safe

    state = spectrumCompute( (poly)first->Data( ),&L,flag );

    if( state==spectrumOK )
    {
      result->rtyp = LIST_CMD;
      result->data = (char*)L;
    }
    else
    {
      spectrumPrintError(state);
    }
  }

  return  (state!=spectrumOK);
}

// ----------------------------------------------------------------------------
//  this procedure is called from the interpreter
// ----------------------------------------------------------------------------
//  first  = polynomial
//  result = list of spectrum numbers
// ----------------------------------------------------------------------------

BOOLEAN spectrumfProc( leftv result,leftv first )
{
  spectrumState state = spectrumOK;

  // -------------------
  //  check consistency
  // -------------------

  //  check for a local polynomial ring

  if( currRing->OrdSgn != -1 )
  // ?? HS: the test above is also true for k[x][[y]], k[[x]][y]
  // or should we use:
  //if( !ringIsLocal( ) )
  {
    WerrorS( "only works for local orderings" );
    state = spectrumWrongRing;
  }
  else if( currRing->qideal != NULL )
  {
    WerrorS( "does not work in quotient rings" );
    state = spectrumWrongRing;
  }
  else
  {
    lists   L    = (lists)NULL;
    int     flag = 2; // symmetric optimization

    state = spectrumCompute( (poly)first->Data( ),&L,flag );

    if( state==spectrumOK )
    {
      result->rtyp = LIST_CMD;
      result->data = (char*)L;
    }
    else
    {
      spectrumPrintError(state);
    }
  }

  return  (state!=spectrumOK);
}

// ----------------------------------------------------------------------------
//  check if a list is a spectrum
//  check for:
//      list has 6 elements
//      1st element is int (mu=Milnor number)
//      2nd element is int (pg=geometrical genus)
//      3rd element is int (n =number of different spectrum numbers)
//      4th element is intvec (num=numerators)
//      5th element is intvec (den=denomiantors)
//      6th element is intvec (mul=multiplicities)
//      exactly n numerators
//      exactly n denominators
//      exactly n multiplicities
//      mu>0
//      pg>=0
//      n>0
//      num>0
//      den>0
//      mul>0
//      symmetriy with respect to numberofvariables/2
//      monotony
//      mu = sum of all multiplicities
//      pg = sum of all multiplicities where num/den<=1
// ----------------------------------------------------------------------------

semicState  list_is_spectrum( lists l )
{
    // -------------------
    //  check list length
    // -------------------

    if( l->nr < 5 )
    {
        return  semicListTooShort;
    }
    else if( l->nr > 5 )
    {
        return  semicListTooLong;
    }

    // -------------
    //  check types
    // -------------

    if( l->m[0].rtyp != INT_CMD )
    {
        return  semicListFirstElementWrongType;
    }
    else if( l->m[1].rtyp != INT_CMD )
    {
        return  semicListSecondElementWrongType;
    }
    else if( l->m[2].rtyp != INT_CMD )
    {
        return  semicListThirdElementWrongType;
    }
    else if( l->m[3].rtyp != INTVEC_CMD )
    {
        return  semicListFourthElementWrongType;
    }
    else if( l->m[4].rtyp != INTVEC_CMD )
    {
        return  semicListFifthElementWrongType;
    }
    else if( l->m[5].rtyp != INTVEC_CMD )
    {
        return  semicListSixthElementWrongType;
    }

    // -------------------------
    //  check number of entries
    // -------------------------

    int     mu = (int)(l->m[0].Data( ));
    int     pg = (int)(l->m[1].Data( ));
    int     n  = (int)(l->m[2].Data( ));

    if( n <= 0 )
    {
        return  semicListNNegative;
    }

    intvec  *num = (intvec*)l->m[3].Data( );
    intvec  *den = (intvec*)l->m[4].Data( );
    intvec  *mul = (intvec*)l->m[5].Data( );

    if( n != num->length( ) )
    {
        return  semicListWrongNumberOfNumerators;
    }
    else if( n != den->length( ) )
    {
        return  semicListWrongNumberOfDenominators;
    }
    else if( n != mul->length( ) )
    {
        return  semicListWrongNumberOfMultiplicities;
    }

    // --------
    //  values
    // --------

    if( mu <= 0 )
    {
        return  semicListMuNegative;
    }
    if( pg < 0 )
    {
        return  semicListPgNegative;
    }

    int i;

    for( i=0; i<n; i++ )
    {
        if( (*num)[i] <= 0 )
        {
            return  semicListNumNegative;
        }
        if( (*den)[i] <= 0 )
        {
            return  semicListDenNegative;
        }
        if( (*mul)[i] <= 0 )
        {
            return  semicListMulNegative;
        }
    }

    // ----------------
    //  check symmetry
    // ----------------

    int     j;

    for( i=0, j=n-1; i<=j; i++,j-- )
    {
        if( (*num)[i] != pVariables*((*den)[i]) - (*num)[j] ||
            (*den)[i] != (*den)[j] ||
            (*mul)[i] != (*mul)[j] )
        {
            return  semicListNotSymmetric;
        }
    }

    // ----------------
    //  check monotony
    // ----------------

    for( i=0, j=1; i<n/2; i++,j++ )
    {
        if( (*num)[i]*(*den)[j] >= (*num)[j]*(*den)[i] )
        {
            return  semicListNotMonotonous;
        }
    }

    // ---------------------
    //  check Milnor number
    // ---------------------

    for( mu=0, i=0; i<n; i++ )
    {
        mu += (*mul)[i];
    }

    if( mu != (int)(l->m[0].Data( )) )
    {
        return  semicListMilnorWrong;
    }

    // -------------------------
    //  check geometrical genus
    // -------------------------

    for( pg=0, i=0; i<n; i++ )
    {
        if( (*num)[i]<=(*den)[i] )
        {
            pg += (*mul)[i];
        }
    }

    if( pg != (int)(l->m[1].Data( )) )
    {
        return  semicListPGWrong;
    }

    return  semicOK;
}

// ----------------------------------------------------------------------------
//  this procedure is called from the interpreter
// ----------------------------------------------------------------------------
//  first  = list of spectrum numbers
//  second = list of spectrum numbers
//  result = sum of the two lists
// ----------------------------------------------------------------------------

BOOLEAN spaddProc( leftv result,leftv first,leftv second )
{
    semicState  state;

    // -----------------
    //  check arguments
    // -----------------

    lists l1 = (lists)first->Data( );
    lists l2 = (lists)second->Data( );

    if( (state=list_is_spectrum( l1 )) != semicOK )
    {
        WerrorS( "first argument is not a spectrum:" );
        list_error( state );
    }
    else if( (state=list_is_spectrum( l2 )) != semicOK )
    {
        WerrorS( "second argument is not a spectrum:" );
        list_error( state );
    }
    else
    {
        spectrum s1( l1 );
        spectrum s2( l2 );
        spectrum sum( s1+s2 );

        result->rtyp = LIST_CMD;
        result->data = (char*)(sum.thelist( ));
    }

    return  (state!=semicOK);
}

// ----------------------------------------------------------------------------
//  this procedure is called from the interpreter
// ----------------------------------------------------------------------------
//  first  = list of spectrum numbers
//  second = integer
//  result = the multiple of the first list by the second factor
// ----------------------------------------------------------------------------

BOOLEAN spmulProc( leftv result,leftv first,leftv second )
{
    semicState  state;

    // -----------------
    //  check arguments
    // -----------------

    lists   l = (lists)first->Data( );
    int     k = (int)second->Data( );

    if( (state=list_is_spectrum( l ))!=semicOK )
    {
        WerrorS( "first argument is not a spectrum" );
        list_error( state );
    }
    else if( k < 0 )
    {
        WerrorS( "second argument should be positive" );
        state = semicMulNegative;
    }
    else
    {
        spectrum s( l );
        spectrum product( k*s );

        result->rtyp = LIST_CMD;
        result->data = (char*)product.thelist( );
    }

    return  (state!=semicOK);
}

// ----------------------------------------------------------------------------
//  this procedure is called from the interpreter
// ----------------------------------------------------------------------------
//  first  = list of spectrum numbers
//  second = list of spectrum numbers
//  result = semicontinuity index
// ----------------------------------------------------------------------------

BOOLEAN    semicProc3   ( leftv res,leftv u,leftv v,leftv w )
{
  semicState  state;
  BOOLEAN qh=(((int)w->Data())==1);

  // -----------------
  //  check arguments
  // -----------------

  lists l1 = (lists)u->Data( );
  lists l2 = (lists)v->Data( );

  if( (state=list_is_spectrum( l1 ))!=semicOK )
  {
    WerrorS( "first argument is not a spectrum" );
    list_error( state );
  }
  else if( (state=list_is_spectrum( l2 ))!=semicOK )
  {
    WerrorS( "second argument is not a spectrum" );
    list_error( state );
  }
  else
  {
    spectrum s1( l1 );
    spectrum s2( l2 );

    res->rtyp = INT_CMD;
    if (qh)
      res->data = (void*)(s1.mult_spectrumh( s2 ));
    else
      res->data = (void*)(s1.mult_spectrum( s2 ));
  }

  // -----------------
  //  check status
  // -----------------

  return  (state!=semicOK);
}
BOOLEAN    semicProc   ( leftv res,leftv u,leftv v )
{
  sleftv tmp;
  memset(&tmp,0,sizeof(tmp));
  tmp.rtyp=INT_CMD;
  /* tmp.data = (void *)0;  -- done by memset */

  return  semicProc3(res,u,v,&tmp);
}
// from splist.cc
// ----------------------------------------------------------------------------
//  Compute the spectrum of a  spectrumPolyList
// ----------------------------------------------------------------------------

spectrumState   spectrumPolyList::spectrum( lists *L,int fast )
{
    spectrumPolyNode  **node = &root;
    spectrumPolyNode  *search;

    poly              f,tmp;
    int               found,cmp;

    Rational smax( ( fast==0 ? 0 : pVariables ),
                   ( fast==2 ? 2 : 1 ) );

    Rational weight_prev( 0,1 );

    int     mu = 0;          // the milnor number
    int     pg = 0;          // the geometrical genus
    int     n  = 0;          // number of different spectral numbers
    int     z  = 0;          // number of spectral number equal to smax

    int     k = 0;

    while( (*node)!=(spectrumPolyNode*)NULL &&
           ( fast==0 || (*node)->weight<=smax ) )
    {
        // ---------------------------------------
        //  determine the first normal form which
        //  contains the monomial  node->mon
        // ---------------------------------------

        found  = FALSE;
        search = *node;

        while( search!=(spectrumPolyNode*)NULL && found==FALSE )
        {
            if( search->nf!=(poly)NULL )
            {
                f = search->nf;

                do
                {
                    // --------------------------------
                    //  look for  (*node)->mon  in   f
                    // --------------------------------

                    cmp = pCmp( (*node)->mon,f );

                    if( cmp<0 )
                    {
                        f = pNext( f );
                    }
                    else if( cmp==0 )
                    {
                        // -----------------------------
                        //  we have found a normal form
                        // -----------------------------

                        found = TRUE;

                        //  normalize coefficient

                        number inv = nInvers( pGetCoeff( f ) );
                        pMult_nn( search->nf,inv );
                        nDelete( &inv );

                        //  exchange  normal forms

                        tmp         = (*node)->nf;
                        (*node)->nf = search->nf;
                        search->nf  = tmp;
                    }
                }
                while( cmp<0 && f!=(poly)NULL );
            }
            search = search->next;
        }

        if( found==FALSE )
        {
            // ------------------------------------------------
            //  the weight of  node->mon  is a spectrum number
            // ------------------------------------------------

            mu++;

            if( (*node)->weight<=(Rational)1 )              pg++;
            if( (*node)->weight==smax )           z++;
            if( (*node)->weight>weight_prev )     n++;

            weight_prev = (*node)->weight;
            node = &((*node)->next);
        }
        else
        {
            // -----------------------------------------------
            //  determine all other normal form which contain
            //  the monomial  node->mon
            //  replace for  node->mon  its normal form
            // -----------------------------------------------

            while( search!=(spectrumPolyNode*)NULL )
            {
                    if( search->nf!=(poly)NULL )
                {
                    f = search->nf;

                    do
                    {
                        // --------------------------------
                        //  look for  (*node)->mon  in   f
                        // --------------------------------

                        cmp = pCmp( (*node)->mon,f );

                        if( cmp<0 )
                        {
                            f = pNext( f );
                        }
                        else if( cmp==0 )
                        {
                            search->nf = pSub( search->nf,
                                ppMult_nn( (*node)->nf,pGetCoeff( f ) ) );
                            pNorm( search->nf );
                        }
                    }
                    while( cmp<0 && f!=(poly)NULL );
                }
                search = search->next;
            }
            delete_node( node );
        }

    }

    // --------------------------------------------------------
    //  fast computation exploits the symmetry of the spectrum
    // --------------------------------------------------------

    if( fast==2 )
    {
        mu = 2*mu - z;
        n  = ( z > 0 ? 2*n - 1 : 2*n );
    }

    // --------------------------------------------------------
    //  compute the spectrum numbers with their multiplicities
    // --------------------------------------------------------

    intvec            *nom  = new intvec( n );
    intvec            *den  = new intvec( n );
    intvec            *mult = new intvec( n );

    int count         = 0;
    int multiplicity  = 1;

    for( search=root; search!=(spectrumPolyNode*)NULL &&
                     ( fast==0 || search->weight<=smax );
                     search=search->next )
    {
        if( search->next==(spectrumPolyNode*)NULL ||
            search->weight<search->next->weight )
        {
            (*nom) [count] = search->weight.get_num_si( );
            (*den) [count] = search->weight.get_den_si( );
            (*mult)[count] = multiplicity;

            multiplicity=1;
            count++;
        }
        else
        {
            multiplicity++;
        }
    }

    // --------------------------------------------------------
    //  fast computation exploits the symmetry of the spectrum
    // --------------------------------------------------------

    if( fast==2 )
    {
        int n1,n2;
        for( n1=0, n2=n-1; n1<n2; n1++, n2-- )
        {
            (*nom) [n2] = pVariables*(*den)[n1]-(*nom)[n1];
            (*den) [n2] = (*den)[n1];
            (*mult)[n2] = (*mult)[n1];
        }
    }

    // -----------------------------------
    //  test if the spectrum is symmetric
    // -----------------------------------

    if( fast==0 || fast==1 )
    {
        int symmetric=TRUE;

        for( int n1=0, n2=n-1 ; n1<n2 && symmetric==TRUE; n1++, n2-- )
        {
            if( (*mult)[n1]!=(*mult)[n2] ||
                (*den) [n1]!= (*den)[n2] ||
                (*nom)[n1]+(*nom)[n2]!=pVariables*(*den) [n1] )
            {
                symmetric = FALSE;
            }
        }

        if( symmetric==FALSE )
        {
            // ---------------------------------------------
            //  the spectrum is not symmetric => degenerate
            //  principal part
            // ---------------------------------------------

            *L = (lists)omAllocBin( slists_bin);
            (*L)->Init( 1 );
            (*L)->m[0].rtyp = INT_CMD;    //  milnor number
            (*L)->m[0].data = (void*)mu;

            return spectrumDegenerate;
        }
    }

    *L = (lists)omAllocBin( slists_bin);

    (*L)->Init( 6 );

    (*L)->m[0].rtyp = INT_CMD;    //  milnor number
    (*L)->m[1].rtyp = INT_CMD;    //  geometrical genus
    (*L)->m[2].rtyp = INT_CMD;    //  number of spectrum values
    (*L)->m[3].rtyp = INTVEC_CMD; //  nominators
    (*L)->m[4].rtyp = INTVEC_CMD; //  denomiantors
    (*L)->m[5].rtyp = INTVEC_CMD; //  multiplicities

    (*L)->m[0].data = (void*)mu;
    (*L)->m[1].data = (void*)pg;
    (*L)->m[2].data = (void*)n;
    (*L)->m[3].data = (void*)nom;
    (*L)->m[4].data = (void*)den;
    (*L)->m[5].data = (void*)mult;

    return  spectrumOK;
}

#endif

//from mpr_inout.cc
extern void nPrint(number n);

BOOLEAN loNewtonP( leftv res, leftv arg1 )
{
  res->data= (void*)loNewtonPolytope( (ideal)arg1->Data() );
  return FALSE;
}

BOOLEAN loSimplex( leftv res, leftv args )
{
  if ( !(rField_is_long_R()) )
  {
    WerrorS("Ground field not implemented!");
    return TRUE;
  }

  simplex * LP;
  matrix m;

  leftv v= args;
  if ( v->Typ() != MATRIX_CMD ) // 1: matrix
    return TRUE;
  else
    m= (matrix)(v->CopyD());

  LP = new simplex(MATROWS(m),MATCOLS(m));
  LP->mapFromMatrix(m);

  v= v->next;
  if ( v->Typ() != INT_CMD )    // 2: m = number of constraints
    return TRUE;
  else
    LP->m= (int)(v->Data());

  v= v->next;
  if ( v->Typ() != INT_CMD )    // 3: n = number of variables
    return TRUE;
  else
    LP->n= (int)(v->Data());

  v= v->next;
  if ( v->Typ() != INT_CMD )    // 4: m1 = number of <= constraints
    return TRUE;
  else
    LP->m1= (int)(v->Data());

  v= v->next;
  if ( v->Typ() != INT_CMD )    // 5: m2 = number of >= constraints
    return TRUE;
  else
    LP->m2= (int)(v->Data());

  v= v->next;
  if ( v->Typ() != INT_CMD )    // 6: m3 = number of == constraints
    return TRUE;
  else
    LP->m3= (int)(v->Data());

#ifdef mprDEBUG_PROT
  Print("m (constraints) %d\n",LP->m);
  Print("n (columns) %d\n",LP->n);
  Print("m1 (<=) %d\n",LP->m1);
  Print("m2 (>=) %d\n",LP->m2);
  Print("m3 (==) %d\n",LP->m3);
#endif

  LP->compute();

  lists lres= (lists)omAlloc( sizeof(slists) );
  lres->Init( 6 );

  lres->m[0].rtyp= MATRIX_CMD; // output matrix
  lres->m[0].data=(void*)LP->mapToMatrix(m);

  lres->m[1].rtyp= INT_CMD;   // found a solution?
  lres->m[1].data=(void*)LP->icase;

  lres->m[2].rtyp= INTVEC_CMD;
  lres->m[2].data=(void*)LP->posvToIV();

  lres->m[3].rtyp= INTVEC_CMD;
  lres->m[3].data=(void*)LP->zrovToIV();

  lres->m[4].rtyp= INT_CMD;
  lres->m[4].data=(void*)LP->m;

  lres->m[5].rtyp= INT_CMD;
  lres->m[5].data=(void*)LP->n;

  res->data= (void*)lres;

  return FALSE;
}

BOOLEAN nuMPResMat( leftv res, leftv arg1, leftv arg2 )
{
  ideal gls = (ideal)(arg1->Data());
  int imtype= (int)arg2->Data();

  uResultant::resMatType mtype= determineMType( imtype );

  // check input ideal ( = polynomial system )
  if ( mprIdealCheck( gls, arg1->Name(), mtype, true ) != mprOk )
  {
    return TRUE;
  }

  uResultant *resMat= new uResultant( gls, mtype, false );

  res->rtyp = MODUL_CMD;
  res->data= (void*)resMat->accessResMat()->getMatrix();

  delete resMat;

  return FALSE;
}

BOOLEAN nuLagSolve( leftv res, leftv arg1, leftv arg2, leftv arg3 )
{

  poly gls;
  gls= (poly)(arg1->Data());
  int howclean= (int)arg3->Data();

  if ( !(rField_is_R() ||
         rField_is_Q() ||
         rField_is_long_R() ||
         rField_is_long_C()) )
  {
    WerrorS("Ground field not implemented!");
    return TRUE;
  }

  if ( !(rField_is_R()||rField_is_long_R()||rField_is_long_C()) )
  {
    unsigned long int ii = (unsigned long int)arg2->Data();
    setGMPFloatDigits( ii, ii );
  }

  if ( gls == NULL || pIsConstant( gls ) )
  {
    WerrorS("Input polynomial is constant!");
    return TRUE;
  }

  int ldummy;
  int deg= pLDeg( gls, &ldummy, currRing );
  //  int deg= pDeg( gls );
  int len= pLength( gls );
  int i,vpos;
  poly piter;
  lists elist;
  lists rlist;

  elist= (lists)omAlloc( sizeof(slists) );
  elist->Init( 0 );

  if ( pVariables > 1 )
  {
    piter= gls;
    for ( i= 1; i <= pVariables; i++ )
      if ( pGetExp( piter, i ) )
      {
        vpos= i;
        break;
      }
    while ( piter )
    {
      for ( i= 1; i <= pVariables; i++ )
        if ( (vpos != i) && (pGetExp( piter, i ) != 0) )
        {
          WerrorS("The input polynomial must be univariate!");
          return TRUE;
        }
      pIter( piter );
    }
  }

  rootContainer * roots= new rootContainer();
  number * pcoeffs= (number *)omAlloc( (deg+1) * sizeof( number ) );
  piter= gls;
  for ( i= deg; i >= 0; i-- )
  {
    //if ( piter ) Print("deg %d, pDeg(piter) %d\n",i,pTotaldegree(piter));
    if ( piter && pTotaldegree(piter) == i )
    {
      pcoeffs[i]= nCopy( pGetCoeff( piter ) );
      //nPrint( pcoeffs[i] );PrintS("  ");
      pIter( piter );
    }
    else
    {
      pcoeffs[i]= nInit(0);
    }
  }

#ifdef mprDEBUG_PROT
  for (i=deg; i >= 0; i--)
  {
    nPrint( pcoeffs[i] );PrintS("  ");
  }
  PrintLn();
#endif

  roots->fillContainer( pcoeffs, NULL, 1, deg, rootContainer::onepoly, 1 );
  roots->solver( howclean );

  int elem= roots->getAnzRoots();
  char *out;
  char *dummy;
  int j;

  rlist= (lists)omAlloc( sizeof(slists) );
  rlist->Init( elem );

  if (rField_is_long_C())
  {
    for ( j= 0; j < elem; j++ )
    {
      rlist->m[j].rtyp=NUMBER_CMD;
      rlist->m[j].data=(void *)nCopy((number)(roots->getRoot(j)));
      //rlist->m[j].data=(void *)(number)(roots->getRoot(j));
    }
  }
  else
  {
    for ( j= 0; j < elem; j++ )
    {
      dummy = complexToStr( (*roots)[j], gmp_output_digits );
      rlist->m[j].rtyp=STRING_CMD;
      rlist->m[j].data=(void *)dummy;
    }
  }

  elist->Clean();
  //omFreeSize( (ADDRESS) elist, sizeof(slists) );

  // this is (via fillContainer) the same data as in root
  //for ( i= deg; i >= 0; i-- ) nDelete( &pcoeffs[i] );
  //omFreeSize( (ADDRESS) pcoeffs, (deg+1) * sizeof( number ) );

  delete roots;

  res->rtyp= LIST_CMD;
  res->data= (void*)rlist;

  return FALSE;
}

BOOLEAN nuVanderSys( leftv res, leftv arg1, leftv arg2, leftv arg3)
{
  int i;
  ideal p,w;
  p= (ideal)arg1->Data();
  w= (ideal)arg2->Data();

  // w[0] = f(p^0)
  // w[1] = f(p^1)
  // ...
  // p can be a vector of numbers (multivariate polynom)
  //   or one number (univariate polynom)
  // tdg = deg(f)

  int n= IDELEMS( p );
  int m= IDELEMS( w );
  int tdg= (int)arg3->Data();

  res->data= (void*)NULL;

  // check the input
  if ( tdg < 1 )
  {
    WerrorS("Last input parameter must be > 0!");
    return TRUE;
  }
  if ( n != pVariables )
  {
    Werror("Size of first input ideal must be equal to %d!",pVariables);
    return TRUE;
  }
  if ( m != (int)pow((double)tdg+1,(double)n) )
  {
    Werror("Size of second input ideal must be equal to %d!",
      (int)pow((double)tdg+1,(double)n));
    return TRUE;
  }
  if ( !(rField_is_Q() /* ||
         rField_is_R() || rField_is_long_R() ||
         rField_is_long_C()*/ ) )
         {
    WerrorS("Ground field not implemented!");
    return TRUE;
  }

  number tmp;
  number *pevpoint= (number *)omAlloc( n * sizeof( number ) );
  for ( i= 0; i < n; i++ )
  {
    pevpoint[i]=nInit(0);
    if (  (p->m)[i] )
    {
      tmp = pGetCoeff( (p->m)[i] );
      if ( nIsZero(tmp) || nIsOne(tmp) || nIsMOne(tmp) )
      {
        omFreeSize( (ADDRESS)pevpoint, n * sizeof( number ) );
        WerrorS("Elements of first input ideal must not be equal to -1, 0, 1!");
        return TRUE;
      }
    } else tmp= NULL;
    if ( !nIsZero(tmp) )
    {
      if ( !pIsConstant((p->m)[i]))
      {
        omFreeSize( (ADDRESS)pevpoint, n * sizeof( number ) );
        WerrorS("Elements of first input ideal must be numbers!");
        return TRUE;
      }
      pevpoint[i]= nCopy( tmp );
    }
  }

  number *wresults= (number *)omAlloc( m * sizeof( number ) );
  for ( i= 0; i < m; i++ )
  {
    wresults[i]= nInit(0);
    if ( (w->m)[i] && !nIsZero(pGetCoeff((w->m)[i])) )
    {
      if ( !pIsConstant((w->m)[i]))
      {
        omFreeSize( (ADDRESS)pevpoint, n * sizeof( number ) );
        omFreeSize( (ADDRESS)wresults, m * sizeof( number ) );
        WerrorS("Elements of second input ideal must be numbers!");
        return TRUE;
      }
      wresults[i]= nCopy(pGetCoeff((w->m)[i]));
    }
  }

  vandermonde vm( m, n, tdg, pevpoint, FALSE );
  number *ncpoly= vm.interpolateDense( wresults );
  // do not free ncpoly[]!!
  poly rpoly= vm.numvec2poly( ncpoly );

  omFreeSize( (ADDRESS)pevpoint, n * sizeof( number ) );
  omFreeSize( (ADDRESS)wresults, m * sizeof( number ) );

  res->data= (void*)rpoly;
  return FALSE;
}

BOOLEAN nuUResSolve( leftv res, leftv args )
{
  leftv v= args;

  ideal gls;
  int imtype;
  int howclean;

  // get ideal
  if ( v->Typ() != IDEAL_CMD )
    return TRUE;
  else gls= (ideal)(v->Data());
  v= v->next;

  // get resultant matrix type to use (0,1)
  if ( v->Typ() != INT_CMD )
    return TRUE;
  else imtype= (int)v->Data();
  v= v->next;

  // get and set precision in digits ( > 0 )
  if ( v->Typ() != INT_CMD )
    return TRUE;
  else if ( !(rField_is_R()||rField_is_long_R()||rField_is_long_C()) )
  {
    unsigned long int ii=(unsigned long int)v->Data();
    setGMPFloatDigits( ii, ii );
  }
  v= v->next;

  // get interpolation steps (0,1,2)
  if ( v->Typ() != INT_CMD )
    return TRUE;
  else howclean= (int)v->Data();

  uResultant::resMatType mtype= determineMType( imtype );
  int i,c,count;
  lists listofroots= NULL;
  lists emptylist;
  number smv= NULL;
  BOOLEAN interpolate_det= (mtype==uResultant::denseResMat)?TRUE:FALSE;

  //emptylist= (lists)omAlloc( sizeof(slists) );
  //emptylist->Init( 0 );

  //res->rtyp = LIST_CMD;
  //res->data= (void *)emptylist;

  // check input ideal ( = polynomial system )
  if ( mprIdealCheck( gls, args->Name(), mtype ) != mprOk )
  {
    return TRUE;
  }

  uResultant * ures;
  rootContainer ** iproots;
  rootContainer ** muiproots;
  rootArranger * arranger;

  // main task 1: setup of resultant matrix
  ures= new uResultant( gls, mtype );
  if ( ures->accessResMat()->initState() != resMatrixBase::ready )
  {
    WerrorS("Error occurred during matrix setup!");
    return TRUE;
  }

  // if dense resultant, check if minor nonsingular
  if ( mtype == uResultant::denseResMat )
  {
    smv= ures->accessResMat()->getSubDet();
#ifdef mprDEBUG_PROT
    PrintS("// Determinant of submatrix: ");nPrint(smv);PrintLn();
#endif
    if ( nIsZero(smv) )
    {
      WerrorS("Unsuitable input ideal: Minor of resultant matrix is singular!");
      return TRUE;
    }
  }

  // main task 2: Interpolate specialized resultant polynomials
  if ( interpolate_det )
    iproots= ures->interpolateDenseSP( false, smv );
  else
    iproots= ures->specializeInU( false, smv );

  // main task 3: Interpolate specialized resultant polynomials
  if ( interpolate_det )
    muiproots= ures->interpolateDenseSP( true, smv );
  else
    muiproots= ures->specializeInU( true, smv );

#ifdef mprDEBUG_PROT
  c= iproots[0]->getAnzElems();
  for (i=0; i < c; i++) pWrite(iproots[i]->getPoly());
  c= muiproots[0]->getAnzElems();
  for (i=0; i < c; i++) pWrite(muiproots[i]->getPoly());
#endif

  // main task 4: Compute roots of specialized polys and match them up
  arranger= new rootArranger( iproots, muiproots, howclean );
  arranger->solve_all();

  // get list of roots
  if ( arranger->success() )
  {
    arranger->arrange();
    listofroots= arranger->listOfRoots( gmp_output_digits );
  }
  else
  {
    WerrorS("Solver was unable to find any roots!");
    return TRUE;
  }

  // free everything
  count= iproots[0]->getAnzElems();
  for (i=0; i < count; i++) delete iproots[i];
  omFreeSize( (ADDRESS) iproots, count * sizeof(rootContainer*) );
  count= muiproots[0]->getAnzElems();
  for (i=0; i < count; i++) delete muiproots[i];
  omFreeSize( (ADDRESS) muiproots, count * sizeof(rootContainer*) );

  delete ures;
  delete arranger;
  nDelete( &smv );

  res->data= (void *)listofroots;

  //emptylist->Clean();
  //  omFreeSize( (ADDRESS) emptylist, sizeof(slists) );

  return FALSE;
}

// from mpr_numeric.cc
lists rootArranger::listOfRoots( const unsigned int oprec )
{
  int i,j,tr;
  int count= roots[0]->getAnzRoots(); // number of roots
  int elem= roots[0]->getAnzElems();  // number of koordinates per root

  lists listofroots= (lists)omAlloc( sizeof(slists) ); // must be done this way!

  if ( found_roots )
  {
    listofroots->Init( count );

    for (i=0; i < count; i++)
    {
      lists onepoint= (lists)omAlloc(sizeof(slists)); // must be done this way!
      onepoint->Init(elem);
      for ( j= 0; j < elem; j++ )
      {
        if ( !rField_is_long_C() )
        {
          onepoint->m[j].rtyp=STRING_CMD;
          onepoint->m[j].data=(void *)complexToStr((*roots[j])[i],oprec);
        }
        else
        {
          onepoint->m[j].rtyp=NUMBER_CMD;
          onepoint->m[j].data=(void *)nCopy((number)(roots[j]->getRoot(i)));
        }
        onepoint->m[j].next= NULL;
        onepoint->m[j].name= NULL;
      }
      listofroots->m[i].rtyp=LIST_CMD;
      listofroots->m[i].data=(void *)onepoint;
      listofroots->m[j].next= NULL;
      listofroots->m[j].name= NULL;
    }

  }
  else
  {
    listofroots->Init( 0 );
  }

  return listofroots;
}

#ifdef PDEBUG

#if (OM_TRACK > 2) && defined(OM_TRACK_CUSTOM)

void p_SetRingOfPoly(poly p, ring r)
{
  while (p != NULL)
  {
    p_SetRingOfLm(p, r);
    pIter(p);
  }
}

void p_SetRingOfIdeal(ideal id, ring r)
{
  if (id == NULL) return;

  int i, n = id->ncols*id->nrows;

  for (i=0; i<n; i++)
  {
    p_SetRingOfPoly(id->m[i], r);
  }
}

void p_SetRingOfList(lists L, ring r)
{
  int i;
  for (i=0; i<L->nr; i++)
  {
    p_SetRingOfLeftv(&(L->m[i]), r);
  }
}

void p_SetRingOfCommand(command cmd, ring r)
{
  if (cmd->op == PROC_CMD && cmd->argc == 2)
    p_SetRingOfLeftv(&(cmd->arg2), r);
  else if (cmd->argc > 0)
  {
    p_SetRingOfLeftv(&(cmd->arg1), r);
    if (cmd->argc > 1)
    {
      p_SetRingOfLeftv(&(cmd->arg2), r);
      if (cmd->argc > 2)
        p_SetRingOfLeftv(&(cmd->arg3), r);
    }
  }
}

void p_SetRingOfLeftv(leftv l, ring r)
{
  while (l != NULL)
  {
    switch(l->rtyp)
    {
        case POLY_CMD:
        case VECTOR_CMD:
          p_SetRingOfPoly((poly) l->data, r);
      break;

      case IDEAL_CMD:
      case MODUL_CMD:
      case MATRIX_CMD:
      case MAP_CMD:
        p_SetRingOfIdeal((ideal) l->data, r);
        break;

        case LIST_CMD:
          p_SetRingOfList((lists) l->data, r);
          break;

        case COMMAND:
          p_SetRingOfCommand((command)l->data, r);
        default:
          break;
    }
    l = l->next;
  }
}
#endif // (OM_TRACK > 2) && defined(OM_TRACK_CUSTOM)

#endif // PDEBUG

// from ring.cc
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

BOOLEAN rSleftvOrdering2Ordering(sleftv *ord, ring R)
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
            R->block1[n] = si_min(last+iv->length()-2 , R->N);
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

BOOLEAN rSleftvList2StringArray(sleftv* sl, char** p)
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

    if (ch!=0 && (ch<2)
    #ifndef NV_OPS
    || (ch > 32003)
    #endif
    )
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
    R->float_len= si_min(float_len,32767);
    R->float_len2= si_min(float_len2,32767);
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
  {
    int l=rv->listLength();
#if SIZEOF_SHORT == 2
#define MAX_SHORT 0x7fff
#endif
    if (l>MAX_SHORT)
    {
      Werror("too many ring variables(%d), max is %d",l,MAX_SHORT);
       goto rInitError;
    }
    R->N = l; /*rv->listLength();*/
  }
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
        if (nshdl->cRing==r)
        {
          Warn("killing the basering for level %d",lev);
          nshdl->cRing=NULL;
          nshdl->cRingHdl=NULL;
        }
      }
    }
#endif /* USE_IILOCALRING */

    /* nKillChar(r); will be called from inside of rDelete */
    rDelete(r);
    return;
  }
  r->ref--;
}

void rKill(idhdl h)
{
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
      currRingHdl=rFindHdl(r,currRingHdl,NULL);
    }
  }
}

idhdl rSimpleFindHdl(ring r, idhdl root, idhdl n=NULL)
{
  //idhdl next_best=NULL;
  idhdl h=root;
  while (h!=NULL)
  {
    if (((IDTYP(h)==RING_CMD)||(IDTYP(h)==QRING_CMD))
    && (h!=n)
    && (IDRING(h)==r)
    )
    {
   //   if (IDLEV(h)==myynest)
   //     return h;
   //   if ((IDLEV(h)==0) || (next_best==NULL))
   //     next_best=h;
   //   else if (IDLEV(next_best)<IDLEV(h))
   //     next_best=h;
      return h;
    }
    h=IDNEXT(h);
  }
  //return next_best;
  return NULL;
}

