/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/* $Id: ipshell.cc,v 1.74 2002-02-05 16:53:45 Singular Exp $ */
/*
* ABSTRACT:
*/

//#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

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
#ifndef HAVE_NAMESPACES
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
#else
  idhdl h = NSROOT(namespaceroot->root);

  while (h!=NULL)
  {
    if (((IDTYP(h)==QRING_CMD) || (IDTYP(h) == RING_CMD))
    && (IDRING(h)->idroot!=NULL))
    {
      //Print("=====> Toplevel: ring %s, lev: %d:\n",IDID(h),IDLEV(h));
      if (h!=currRingHdl) {changed=TRUE;rSetHdl(h);}
      killlocals0(v,&(IDRING(h)->idroot));
    }
    if (IDTYP(h)==PACKAGE_CMD && (IDPACKAGE(h)->idroot!=NULL))
    {
      idhdl h0 = (IDPACKAGE(h))->idroot;

      //Print("=====> package: %s, lev: %d:\n",IDID(h),IDLEV(h));
      while (h0!=NULL)
      {
        if (((IDTYP(h0)==QRING_CMD) || (IDTYP(h0) == RING_CMD))
            && (IDRING(h0)->idroot!=NULL))
        {
          //Print("=====> '%s': ring %s, lev: %d:\n",IDID(h),IDID(h0),IDLEV(h0));
          if (h0!=currRingHdl) {changed=TRUE;rSetHdl(h0);}
          killlocals0(v,&(IDRING(h0)->idroot));
        }
        h0 = IDNEXT(h0);
      }
      killlocals0(v,&((IDPACKAGE(h))->idroot));
    }
    h = IDNEXT(h);
  }
#endif /* HAVE_NAMESPACES */
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
void killlocals(int v)
{
  BOOLEAN changed=FALSE;
  idhdl sh=currRingHdl;
  if (sh!=NULL) changed=((IDLEV(sh)<v) || (IDRING(sh)->ref>0));
  //if (changed) Print("currRing=%s(%x), lev=%d,ref=%d\n",IDID(sh),IDRING(sh),IDLEV(sh),IDRING(sh)->ref);

  killlocals_rec(&(basePack->idroot),v,currRing);

  if ((iiRETURNEXPR_len > myynest)
  && ((iiRETURNEXPR[myynest].Typ()==RING_CMD)
    || (iiRETURNEXPR[myynest].Typ()==QRING_CMD)))
  {
    leftv h=&iiRETURNEXPR[myynest];
    killlocals0(v,&(((ring)h->data)->idroot));
  }

  if (changed)
  {
    currRingHdl=rFindHdl(currRing,NULL,NULL);
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
#ifdef HAVE_NAMESPACES
      idhdl pack;
      if(strchr(what, ':')!= NULL)
        iiname2hdl(what, &pack, &h);
      else h = ggetid(what);
#else /* HAVE_NAMESPACES */
      h = ggetid(what);
#endif /* HAVE_NAMESPACES */
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
#ifdef HAVE_NAMESPACES
      if (IDTYP(h)==PACKAGE_CMD && really_all && strcmp(IDID(h), "Top"))
      {
        namespaceroot->push(IDPACKAGE(h), IDID(h));
        list_cmd(0,IDID(h),"//      ",FALSE);
        namespaceroot->pop();
      }
#endif /* HAVE_NAMESPACES */
#ifdef HAVE_NS
      if (IDTYP(h)==PACKAGE_CMD && really_all)
      {
        list_cmd(0,IDID(h),"//      ",FALSE);
      }
#endif /* HAVE_NS */
    }
    h = IDNEXT(h);
  }
#ifdef HAVE_NAMESPACES
  if(!namespaceroot->isroot && do_packages) {
    namespaceroot->push(namespaceroot->root->pack, "Top", myynest);
    list_cmd(PACKAGE_CMD,"Top","// ",FALSE, TRUE);
    namespaceroot->pop();
  }
#endif /* HAVE_NAMESPACES */
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

void iiWriteMatrix(matrix im, const char *n, int dim,int spaces)
{
  int i,ii = MATROWS(im)-1;
  int j,jj = MATCOLS(im)-1;
  poly *pp = im->m;

  for (i=0; i<=ii; i++)
  {
    for (j=0; j<=jj; j++)
    {
      if (spaces>0)
        Print("%-*.*s",spaces,spaces," ");
      if (dim == 2) Print("%s[%u,%u]=",n,i+1,j+1);
      else if (dim == 1) Print("%s[%u]=",n,j+1);
      else if (dim == 0) Print("%s=",n);
      if ((i<ii)||(j<jj)) pWrite(*pp++);
      else                pWrite0(*pp);
    }
  }
}

char * iiStringMatrix(matrix im, int dim,char ch)
{
  int i,ii = MATROWS(im);
  int j,jj = MATCOLS(im);
  poly *pp = im->m;
  char *s=StringSetS("");

  for (i=0; i<ii; i++)
  {
    for (j=0; j<jj; j++)
    {
      pString0(*pp++);
      s=StringAppend("%c",ch);
      if (dim > 1) s = StringAppendS("\n");
    }
  }
  s[strlen(s)- (dim > 1 ? 2 : 1)]='\0';
  return s;
}

int IsPrime(int p)  /* brute force !!!! */
{
  int i,j;
  if      (p == 0)    return 0;
  else if (p == 1)    return 1/*1*/;
  else if (p == 2)    return p;
  else if (p < 0)     return (-IsPrime(-p));
  else if (p > 32002) return 32003; // without this line, max prime is 32749
  else if (!(p & 1)) return IsPrime(p-1);
#ifdef HAVE_FACTORY
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
#else
  for (j=p/2+1,i=3; i<p; i+=2)
  {
    if ((p%i) == 0) return IsPrime(p-2);
    if (j < i) return p;
  }
  return p;
#endif
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

#ifdef HAVE_NAMESPACES
  idhdl pack;
  //r=namespaceroot->get(theMap->preimage,myynest);
  iiname2hdl(theMap->preimage,&pack,&r);
#else
  r=IDROOT->get(theMap->preimage,myynest);
#ifdef HAVE_NS
  if ((currPack!=basePack)
  &&((r==NULL) || ((r->typ != RING_CMD) && (r->typ != QRING_CMD))))
    r=basePack->idroot->get(theMap->preimage,myynest);
#endif /* HAVE_NS */
#endif /* HAVE_NAMESPACES */
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
      if ((tmpW.rtyp==IDEAL_CMD) && (nMap==nCopy))
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

BOOLEAN jjBETTI(leftv res, leftv v)
{
  resolvente r;
  int len;
  int reg,typ0;

  r=liFindRes((lists)v->Data(),&len,&typ0);
  if (r==NULL) return TRUE;
  res->data=(char *)syBetti(r,len,&reg);
  omFreeSize((ADDRESS)r,(len)*sizeof(ideal));
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
#ifdef HAVE_NAMESPACES
    if(name->req_packhdl != NULL && name->packhdl != NULL &&
       name->req_packhdl != name->packhdl)
      id = omStrDup(name->name);

    //if(name->req_packhdl != NULL /*&& !isring*/) {
    if(name->req_packhdl != NULL && !isring &&
       IDPACKAGE(name->req_packhdl)->idroot != (*root)) {
      //Print("iiDeclCommand: PUSH(%s)\n",IDID(name->req_packhdl));
      namespaceroot->push( IDPACKAGE(name->req_packhdl) ,
                           IDID(name->req_packhdl));
      sy->data = (char *)enterid(id,lev,t,
                                 &IDPACKAGE(name->req_packhdl)->idroot,init_b);
      namespaceroot->pop();
    }
    else
#endif /* HAVE_NAMESPACES */
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
    if ((h!=NULL)&&(IDLEV(h)==toLev))
    {
      if (IDTYP(h)==v->Typ())
      {
        if (BVERBOSE(V_REDEFINE))
        {
          Warn("redefining %s",IDID(h));
        }
#ifdef HAVE_NAMESPACES
        //if (namespaceroot->currRing==IDRING(h)) namespaceroot->currRing=NULL;
#endif /* HAVE_NAMESPACES */
#ifdef USE_IILOCALRING
            if (iiLocalRing[0]==IDRING(h)) iiLocalRing[0]=NULL;
#else
            proclevel *p=procstack;
            while (p->next!=NULL) p=p->next;
            if (p->currRing==IDRING(h))
            {
              p->currRing=NULL;
              p->currRingHdl=NULL;
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
    iiNoKeepRing=FALSE;
    //Print("export %s\n",IDID(h));
  }
  return FALSE;
}

#ifdef HAVE_NAMESPACES
BOOLEAN iiInternalExport (leftv v, int toLev, idhdl roothdl)
{
  idhdl h=(idhdl)v->data;
  if(h==NULL)
  {
    Warn("'%s': no such identifier\n", v->name);
    return FALSE;
  }
  package rootpack = IDPACKAGE(roothdl);
  //Print("iiInternalExport('%s',%d,%s) typ:%d\n", v->name, toLev, IDID(roothdl),v->Typ());
#if 0
  if( (IDTYP(h) == RING_CMD) || (IDTYP(h) == QRING_CMD))
  {
    sleftv tmp_expr;
    //Print("// ==> Ring set nesting to 0\n");
    //Print("// ++> make a copy of ring\n");
    if(iiInternalExport(v, toLev)) return TRUE;
    if(IDPACKAGE(roothdl) != NSPACK(namespaceroot)) {
      namespaceroot->push(rootpack, IDID(roothdl));
      //namespaceroot->push(NSPACK(namespaceroot->root), "Top");
      idhdl rl=enterid(v->name, toLev, IDTYP(h),
                       &(rootpack->idroot), FALSE);
      namespaceroot->pop();

      if( rl == NULL) return TRUE;
      ring r=(ring)v->Data();
      if(r != NULL) {
        if (&IDRING(rl)!=NULL) rKill(rl);
        r->ref++;
        IDRING(rl)=r;
      }
      else PrintS("! ! ! ! ! r is empty!!!!!!!!!!!!\n");
    }
  }
  else
#endif
  if (RingDependend(IDTYP(h)))
  {
    //Print("// ==> Ringdependent set nesting to 0\n");
    if(iiInternalExport(v, toLev)) return TRUE;
  }
  else
  {
    if (h==IDROOT)
    {
      IDROOT=h->next;
    }
    else
    {
      idhdl hh=IDROOT;
      while ((hh->next!=h)&&(hh->next!=NULL))
        hh=hh->next;
      if (hh->next==h)
        hh->next=h->next;
      else
      {
        PrintS("not found:\n");
        //list_cmd(0,"all","// ",TRUE);
        return TRUE;
      }
    }
    h->next=rootpack->idroot;
    rootpack->idroot=h;
  }
  IDLEV(h)=toLev;
  return FALSE;
}
#endif /* HAVE_NAMESAPCES */
#ifdef HAVE_NS
BOOLEAN iiInternalExport (leftv v, int toLev, idhdl roothdl)
{
  idhdl h=(idhdl)v->data;
  if(h==NULL)
  {
    Warn("'%s': no such identifier\n", v->name);
    return FALSE;
  }
  package rootpack = IDPACKAGE(roothdl);
  Print("iiInternalExport('%s',%d,%s) typ:%d\n", v->name, toLev, IDID(roothdl),v->Typ());
  if (RingDependend(IDTYP(h)))
  {
    //Print("// ==> Ringdependent set nesting to 0\n");
    return (iiInternalExport(v, toLev));
  }
  else
  {
    if (h==IDROOT)
    {
      IDROOT=h->next;
    }
    else
    {
      idhdl hh=IDROOT;
      while ((hh!=NULL) && (hh->next!=h))
        hh=hh->next;
      if ((hh!=NULL) && (hh->next==h))
        hh->next=h->next;
      else
      {
        return TRUE;
      }
    }
    h->next=rootpack->idroot;
    rootpack->idroot=h;
  }
  IDLEV(h)=toLev;
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
#ifdef HAVE_NAMESPACES
BOOLEAN iiExport (leftv v, int toLev, idhdl root)
{
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
      if(iiInternalExport(v, toLev, root))
      {
        rv->CleanUp();
        return TRUE;
      }
    }
    v=v->next;
  }
  rv->CleanUp();
  return nok;
}
#endif /* HAVE_NAMESPACES */
#ifdef HAVE_NS
BOOLEAN iiExport (leftv v, int toLev, idhdl root)
{
  checkall();
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
