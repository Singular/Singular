/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/*
* ABSTRACT:
*/
#ifdef HAVE_CONFIG_H
#include "singularconfig.h"
#endif /* HAVE_CONFIG_H */
#include <kernel/mod2.h>
#include <misc/auxiliary.h>


#include <misc/options.h>
#include <misc/mylimits.h>

#include <factory/factory.h>

#include <Singular/maps_ip.h>
#include <Singular/tok.h>
#include <misc/options.h>
#include <Singular/ipid.h>
#include <misc/intvec.h>
#include <omalloc/omalloc.h>
#include <kernel/febase.h>
#include <kernel/polys.h>
#include <coeffs/numbers.h>
#include <polys/prCopy.h>
#include <kernel/ideals.h>
#include <polys/matpol.h>
#include <kernel/GBEngine/kstd1.h>
#include <polys/monomials/ring.h>
#include <Singular/subexpr.h>
#include <polys/monomials/maps.h>
#include <kernel/GBEngine/syz.h>
#include <coeffs/numbers.h>
//#include <polys/ext_fields/longalg.h>
#include <Singular/lists.h>
#include <Singular/attrib.h>
#include <Singular/ipconv.h>
#include <Singular/links/silink.h>
#include <kernel/GBEngine/stairc.h>
#include <polys/weight.h>
#include <kernel/spectrum/semic.h>
#include <kernel/spectrum/splist.h>
#include <kernel/spectrum/spectrum.h>
////// #include <coeffs/gnumpfl.h>
//#include <kernel/mpr_base.h>
////// #include <coeffs/ffields.h>
#include <polys/clapsing.h>
#include <kernel/combinatorics/hutil.h>
#include <polys/monomials/ring.h>
#include <Singular/ipshell.h>
#include <polys/ext_fields/algext.h>
#include <coeffs/mpr_complex.h>
#include <coeffs/longrat.h>
#include <coeffs/rmodulon.h>

#include <kernel/numeric/mpr_base.h>
#include <kernel/numeric/mpr_numeric.h>

#include <math.h>
#include <ctype.h>

#include <polys/ext_fields/algext.h>
#include <polys/ext_fields/transext.h>

// define this if you want to use the fast_map routine for mapping ideals
#define FAST_MAP

#ifdef FAST_MAP
#include <kernel/maps/fast_maps.h>
#endif

leftv iiCurrArgs=NULL;
idhdl iiCurrProc=NULL;
const char *lastreserved=NULL;

static BOOLEAN iiNoKeepRing=TRUE;

/*0 implementation*/

const char * iiTwoOps(int t)
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

int iiOpsTwoChar(const char *s)
{
/* not handling: &&, ||, ** */
  if (s[1]=='\0') return s[0];
  else if (s[2]!='\0') return 0;
  switch(s[0])
  {
    case '.': if (s[1]=='.') return DOTDOT;
              else           return 0;
    case ':': if (s[1]==':') return COLONCOLON;
              else           return 0;
    case '-': if (s[1]=='-') return COLONCOLON;
              else           return 0;
    case '+': if (s[1]=='+') return PLUSPLUS;
              else           return 0;
    case '=': if (s[1]=='=') return EQUAL_EQUAL;
              else           return 0;
    case '<': if (s[1]=='=') return LE;
              else if (s[1]=='>') return NOTEQUAL;
              else           return 0;
    case '>': if (s[1]=='=') return GE;
              else           return 0;
    case '!': if (s[1]=='=') return NOTEQUAL;
              else           return 0;
  }
  return 0;
}

static void list1(const char* s, idhdl h,BOOLEAN c, BOOLEAN fullname)
{
  char buffer[22];
  int l;
  char buf2[128];

  if(fullname) sprintf(buf2, "%s::%s", "", IDID(h));
  else sprintf(buf2, "%s", IDID(h));

  Print("%s%-30.30s [%d]  ",s,buf2,IDLEV(h));
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
    case MODUL_CMD: Print(", rk %d", (int)(IDIDEAL(h)->rank));
    case IDEAL_CMD: Print(", %u generator(s)",
                    IDELEMS(IDIDEAL(h))); break;
    case MAP_CMD:
                    Print(" from %s",IDMAP(h)->preimage); break;
    case MATRIX_CMD:Print(" %u x %u"
                      ,MATROWS(IDMATRIX(h))
                      ,MATCOLS(IDMATRIX(h))
                    );
                    break;
    case PACKAGE_CMD:
                    paPrint(IDID(h),IDPACKAGE(h));
                    break;
    case PROC_CMD: if((IDPROC(h)->libname!=NULL)
                   && (strlen(IDPROC(h)->libname)>0))
                     Print(" from %s",IDPROC(h)->libname);
                   if(IDPROC(h)->is_static)
                     PrintS(" (static)");
                   break;
    case STRING_CMD:
                   {
                     char *s;
                     l=strlen(IDSTRING(h));
                     memset(buffer,0,22);
                     strncpy(buffer,IDSTRING(h),si_min(l,20));
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
                     Print(" <%lx>",(long)(IDRING(h)));
#endif
                   break;
    /*default:     break;*/
  }
  PrintLn();
}

void type_cmd(leftv v)
{
  BOOLEAN oldShortOut = FALSE;

  if (currRing != NULL)
  {
    oldShortOut = currRing->ShortOut;
    currRing->ShortOut = 1;
  }
  int t=v->Typ();
  Print("// %s %s ",v->Name(),Tok2Cmdname(t));
  switch (t)
  {
    case MAP_CMD:Print(" from %s\n",((map)(v->Data()))->preimage); break;
    case INTMAT_CMD: Print(" %d x %d\n",((intvec*)(v->Data()))->rows(),
                                      ((intvec*)(v->Data()))->cols()); break;
    case MATRIX_CMD:Print(" %u x %u\n" ,
       MATROWS((matrix)(v->Data())),
       MATCOLS((matrix)(v->Data())));break;
    case MODUL_CMD: Print(", rk %d\n", (int)(((ideal)(v->Data()))->rank));break;
    case LIST_CMD: Print(", size %d\n",((lists)(v->Data()))->nr+1); break;

    case PROC_CMD:
    case RING_CMD:
    case IDEAL_CMD:
    case QRING_CMD: PrintLn(); break;

    //case INT_CMD:
    //case STRING_CMD:
    //case INTVEC_CMD:
    //case POLY_CMD:
    //case VECTOR_CMD:
    //case PACKAGE_CMD:

    default:
      break;
  }
  v->Print();
  if (currRing != NULL)
    currRing->ShortOut = oldShortOut;
}

static void killlocals0(int v, idhdl * localhdl, const ring r)
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
      else //if (vv >= v)
      {
        idhdl nexth = IDNEXT(h);
        killhdl2(h,localhdl,r);
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
      if ((IDRING(h)!=NULL) && (IDRING(h)->idroot!=NULL))
      // we have to test IDRING(h)!=NULL: qring Q=groebner(...): killlocals
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
      killlocals0(v,&(((ring)h->data)->idroot),(ring)h->data);
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
    int t=iiRETURNEXPR.Typ();
    if ((/*iiRETURNEXPR.Typ()*/ t==RING_CMD)
    || (/*iiRETURNEXPR.Typ()*/ t==QRING_CMD))
    {
      leftv h=&iiRETURNEXPR;
      if (((ring)h->data)->idroot!=NULL)
        killlocals0(v,&(((ring)h->data)->idroot),(ring)h->data);
    }
    else if (/*iiRETURNEXPR.Typ()*/ t==LIST_CMD)
    {
      leftv h=&iiRETURNEXPR;
      changed |=killlocals_list(v,(lists)h->data);
    }
  }
  if (changed)
  {
    currRingHdl=rFindHdl(cr,NULL,NULL);
    if (currRingHdl==NULL)
      currRing=NULL;
    else
      rChangeCurrRing(cr);
  }

  if (myynest<=1) iiNoKeepRing=TRUE;
  //Print("end killlocals  >= %d\n",v);
  //listall();
}

void list_cmd(int typ, const char* what, const char *prefix,BOOLEAN iterate, BOOLEAN fullname)
{
  idhdl h,start;
  BOOLEAN all = typ<0;
  BOOLEAN really_all=FALSE;

  if ( typ==0 )
  {
    if (strcmp(what,"all")==0)
    {
      if (currPack!=basePack)
        list_cmd(-1,NULL,prefix,iterate,fullname); // list current package
      really_all=TRUE;
      h=basePack->idroot;
    }
    else
    {
      h = ggetid(what);
      if (h!=NULL)
      {
        if (iterate) list1(prefix,h,TRUE,fullname);
        if (IDTYP(h)==ALIAS_CMD) PrintS("A");
        if ((IDTYP(h)==RING_CMD)
            || (IDTYP(h)==QRING_CMD)
            //|| (IDTYP(h)==PACKE_CMD)
        )
        {
          h=IDRING(h)->idroot;
        }
        else if((IDTYP(h)==PACKAGE_CMD) || (IDTYP(h)==POINTER_CMD))
        {
          //Print("list_cmd:package or pointer\n");
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
    if ((all && (IDTYP(h)!=PROC_CMD) &&(IDTYP(h)!=PACKAGE_CMD) &&(IDTYP(h)!=CRING_CMD))
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
      if (IDTYP(h)==PACKAGE_CMD && really_all)
      {
        package save_p=currPack;
        currPack=IDPACKAGE(h);
        list_cmd(0,IDID(h),"//      ",FALSE);
        currPack=save_p;
      }
    }
    h = IDNEXT(h);
  }
}

void test_cmd(int i)
{
  int ii;

  if (i<0)
  {
    ii= -i;
    if (ii < 32)
    {
      si_opt_1 &= ~Sy_bit(ii);
    }
    else if (ii < 64)
    {
      si_opt_2 &= ~Sy_bit(ii-32);
    }
    else
      WerrorS("out of bounds\n");
  }
  else if (i<32)
  {
    ii=i;
    if (Sy_bit(ii) & kOptions)
    {
      Warn("Gerhard, use the option command");
      si_opt_1 |= Sy_bit(ii);
    }
    else if (Sy_bit(ii) & validOpts)
      si_opt_1 |= Sy_bit(ii);
  }
  else if (i<64)
  {
    ii=i-32;
    si_opt_2 |= Sy_bit(ii);
  }
  else
    WerrorS("out of bounds\n");
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

int iiIsPrime0(unsigned p)  /* brute force !!!! */
{
  unsigned i,j=0 /*only to avoid compiler warnings*/;
  if (p<=32749) // max. small prime in factory
  {
    int a=0;
    int e=cf_getNumSmallPrimes()-1;
    i=e/2;
    do
    {
      j=cf_getSmallPrime(i);
      if (p==j) return p;
      if (p<j) e=i-1;
      else     a=i+1;
      i=a+(e-a)/2;
    } while ( a<= e);
    if (p>j) return j;
    else     return cf_getSmallPrime(i-1);
  }
  unsigned end_i=cf_getNumSmallPrimes()-1;
  unsigned end_p=(unsigned)sqrt((double)p);
restart:
  for (i=0; i<end_i; i++)
  {
    j=cf_getSmallPrime(i);
    if ((p%j) == 0)
    {
      if (p<=32751) return iiIsPrime0(p-2);
      p-=2;
      goto restart;
    }
    if (j > end_p) return p;
  }
  if (i>=end_i)
  {
    while(j<=end_p)
    {
      j+=2;
      if ((p%j) == 0)
      {
        if (p<=32751) return iiIsPrime0(p-2);
        p-=2;
        goto restart;
      }
    }
  }
  return p;
}
int IsPrime(int p)  /* brute force !!!! */
{
  if      (p == 0)    return 0;
  else if (p == 1)    return 1/*1*/;
  else if ((p == 2)||(p==3))    return p;
  else if (p < 0)     return 2; //(iiIsPrime0((unsigned)(-p)));
  else if ((p & 1)==0) return iiIsPrime0((unsigned)(p-1));
  return iiIsPrime0((unsigned)(p));
}

BOOLEAN iiWRITE(leftv,leftv v)
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

leftv iiMap(map theMap, const char * what)
{
  idhdl w,r;
  leftv v;
  int i;
  nMapFunc nMap;

  r=IDROOT->get(theMap->preimage,myynest);
  if ((currPack!=basePack)
  &&((r==NULL) || ((r->typ != RING_CMD) && (r->typ != QRING_CMD))))
    r=basePack->idroot->get(theMap->preimage,myynest);
  if ((r==NULL) && (currRingHdl!=NULL)
  && (strcmp(theMap->preimage,IDID(currRingHdl))==0))
  {
    r=currRingHdl;
  }
  if ((r!=NULL) && ((r->typ == RING_CMD) || (r->typ== QRING_CMD)))
  {
    //if ((nMap=nSetMap(rInternalChar(IDRING(r)),
    //             IDRING(r)->parameter,
    //             rPar(IDRING(r)),
    //             IDRING(r)->minpoly)))
    if ((nMap=n_SetMap(IDRING(r)->cf, currRing->cf))==NULL)
    {
////////// WTF?
//      if (rEqual(IDRING(r),currRing))
//      {
//        nMap = n_SetMap(currRing->cf, currRing->cf);
//      }
//      else
//      {
        Werror("can not map from ground field of %s to current ground field",
          theMap->preimage);
        return NULL;
//      }
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
      char *save_r=NULL;
      v=(leftv)omAlloc0Bin(sleftv_bin);
      sleftv tmpW;
      memset(&tmpW,0,sizeof(sleftv));
      tmpW.rtyp=IDTYP(w);
      if (tmpW.rtyp==MAP_CMD)
      {
        tmpW.rtyp=IDEAL_CMD;
        save_r=IDMAP(w)->preimage;
        IDMAP(w)->preimage=0;
      }
      tmpW.data=IDDATA(w);
#if 0
      if (((tmpW.rtyp==IDEAL_CMD)||(tmpW.rtyp==MODUL_CMD)) && idIs0(IDIDEAL(w)))
      {
        v->rtyp=tmpW.rtyp;
        v->data=idInit(IDELEMS(IDIDEAL(w)),IDIDEAL(w)->rank);
      }
      else
#endif
      {
#ifdef FAST_MAP
        if ((tmpW.rtyp==IDEAL_CMD) && (nMap == ndCopyMap)
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
          if (save_r!=NULL) IDMAP(w)->preimage=save_r;
          return NULL;
        }
      }
      if (save_r!=NULL)
      {
        IDMAP(w)->preimage=save_r;
        IDMAP((idhdl)v)->preimage=omStrDup(save_r);
        v->rtyp=MAP_CMD;
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
  lists L=(lists)v->Data();
  intvec *weights=(intvec*)atGet(v,"isHomog",INTVEC_CMD);
  int add_row_shift = 0;
  if (weights==NULL)
    weights=(intvec*)atGet(&(L->m[0]),"isHomog",INTVEC_CMD);
  if (weights!=NULL)  add_row_shift=weights->min_in();
  resolvente rr=liFindRes(L,&len,&typ0);
  if (rr==NULL) return TRUE;
  resolvente r=iiCopyRes(rr,len);

  syMinimizeResolvente(r,len,0);
  omFreeSize((ADDRESS)rr,len*sizeof(ideal));
  len++;
  res->data=(char *)liMakeResolv(r,len,-1,typ0,NULL,add_row_shift);
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
  attr *a=u->Attribute();
  if (a!=NULL)
  l->m[0].attribute=*a;
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
  res->data=(char *)syBetti(r,len,&reg,weights,(int)(long)v->Data());
  omFreeSize((ADDRESS)r,(len)*sizeof(ideal));
  atSet(res,omStrDup("rowShift"),(void*)(long)add_row_shift,INT_CMD);
  if (weights!=NULL) delete weights;
  return FALSE;
}

int iiRegularity(lists L)
{
  int len,reg,typ0;

  resolvente r=liFindRes(L,&len,&typ0);

  if (r==NULL)
    return -2;
  intvec *weights=NULL;
  int add_row_shift=0;
  intvec *ww=(intvec *)atGet(&(L->m[0]),"isHomog",INTVEC_CMD);
  if (ww!=NULL)
  {
     weights=ivCopy(ww);
     add_row_shift = ww->min_in();
     (*weights) -= add_row_shift;
  }
  //Print("attr:%x\n",weights);

  intvec *dummy=syBetti(r,len,&reg,weights);
  if (weights!=NULL) delete weights;
  delete dummy;
  omFreeSize((ADDRESS)r,len*sizeof(ideal));
  return reg+1+add_row_shift;
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
  if (hNexist == 0)
  {
    intvec *iv=new intvec(rVar(currRing));
    for(i=0; i<rVar(currRing); i++) (*iv)[i]=1;
    res->Init(1);
    res->m[0].rtyp=INTVEC_CMD;
    res->m[0].data=(intvec*)iv;
    return res;
  }
  else if (hisModule!=0)
  {
    res->Init(0);
    return res;
  }
  save = ISet = (indset)omAlloc0Bin(indlist_bin);
  hMu = 0;
  hwork = (scfmon)omAlloc(hNexist * sizeof(scmon));
  hvar = (varset)omAlloc((rVar(currRing) + 1) * sizeof(int));
  hpure = (scmon)omAlloc((1 + (rVar(currRing) * rVar(currRing))) * sizeof(long));
  hrad = hexist;
  hNrad = hNexist;
  radmem = hCreate(rVar(currRing) - 1);
  hCo = rVar(currRing) + 1;
  hNvar = rVar(currRing);
  hRadical(hrad, &hNrad, hNvar);
  hSupp(hrad, hNrad, hvar, &hNvar);
  if (hNvar)
  {
    hCo = hNvar;
    memset(hpure, 0, (rVar(currRing) + 1) * sizeof(long));
    hPure(hrad, 0, &hNrad, hvar, hNvar, hpure, &hNpure);
    hLexR(hrad, hNrad, hvar, hNvar);
    hDimSolve(hpure, hNpure, hrad, hNrad, hvar, hNvar);
  }
  if (hCo && (hCo < rVar(currRing)))
  {
    hIndMult(hpure, hNpure, hrad, hNrad, hvar, hNvar);
  }
  if (hMu!=0)
  {
    ISet = save;
    hMu2 = 0;
    if (all && (hCo+1 < rVar(currRing)))
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
  hKill(radmem, rVar(currRing) - 1);
  omFreeSize((ADDRESS)hpure, (1 + (rVar(currRing) * rVar(currRing))) * sizeof(long));
  omFreeSize((ADDRESS)hvar, (rVar(currRing) + 1) * sizeof(int));
  omFreeSize((ADDRESS)hwork, hNexist * sizeof(scmon));
  hDelete(hexist, hNexist);
  return res;
}

int iiDeclCommand(leftv sy, leftv name, int lev,int t, idhdl* root,BOOLEAN isring, BOOLEAN init_b)
{
  BOOLEAN res=FALSE;
  const char *id = name->name;

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

BOOLEAN iiDefaultParameter(leftv p)
{
  attr at=NULL;
  if (iiCurrProc!=NULL)
     at=iiCurrProc->attribute->get("default_arg");
  if (at==NULL)
    return FALSE;
  sleftv tmp;
  memset(&tmp,0,sizeof(sleftv));
  tmp.rtyp=at->atyp;
  tmp.data=at->CopyA();
  return iiAssign(p,&tmp);
}
BOOLEAN iiParameter(leftv p)
{
  if (iiCurrArgs==NULL)
  {
    if (strcmp(p->name,"#")==0)
      return iiDefaultParameter(p);
    Werror("not enough arguments for proc %s",VoiceName());
    p->CleanUp();
    return TRUE;
  }
  leftv h=iiCurrArgs;
  leftv rest=h->next; /*iiCurrArgs is not NULLi here*/
  BOOLEAN is_default_list=FALSE;
  if (strcmp(p->name,"#")==0)
  {
    is_default_list=TRUE;
    rest=NULL;
  }
  else
  {
    h->next=NULL;
  }
  BOOLEAN res=iiAssign(p,h);
  if (is_default_list)
  {
    iiCurrArgs=NULL;
  }
  else
  {
    iiCurrArgs=rest;
  }
  h->CleanUp();
  omFreeBin((ADDRESS)h, sleftv_bin);
  return res;
}
BOOLEAN iiAlias(leftv p)
{
  if (iiCurrArgs==NULL)
  {
    Werror("not enough arguments for proc %s",VoiceName());
    p->CleanUp();
    return TRUE;
  }
  leftv h=iiCurrArgs;
  iiCurrArgs=h->next;
  h->next=NULL;
  if (h->rtyp!=IDHDL)
  {
    BOOLEAN res=iiAssign(p,h);
    h->CleanUp();
    omFreeBin((ADDRESS)h, sleftv_bin);
    return res;
  }
  if (h->Typ()!=p->Typ())
  {
    WerrorS("type mismatch");
    return TRUE;
  }
  idhdl pp=(idhdl)p->data;
  switch(pp->typ)
  {
      case CRING_CMD:
        nKillChar((coeffs)pp);
	break;
      case INT_CMD:
        break;
      case INTVEC_CMD:
      case INTMAT_CMD:
         delete IDINTVEC(pp);
         break;
      case NUMBER_CMD:
         nDelete(&IDNUMBER(pp));
         break;
      case BIGINT_CMD:
         n_Delete(&IDNUMBER(pp),currRing->cf);
         break;
      case MAP_CMD:
         {
           map im = IDMAP(pp);
           omFree((ADDRESS)im->preimage);
         }
         // continue as ideal:
      case IDEAL_CMD:
      case MODUL_CMD:
      case MATRIX_CMD:
          idDelete(&IDIDEAL(pp));
         break;
      case PROC_CMD:
      case RESOLUTION_CMD:
      case STRING_CMD:
         omFree((ADDRESS)IDSTRING(pp));
         break;
      case LIST_CMD:
         IDLIST(pp)->Clean();
         break;
      case LINK_CMD:
         omFreeBin(IDLINK(pp),sip_link_bin);
         break;
       // case ring: cannot happen
       default:
         Werror("unknown type %d",p->Typ());
         return TRUE;
  }
  pp->typ=ALIAS_CMD;
  IDDATA(pp)=(char*)h->data;
  h->CleanUp();
  omFreeBin((ADDRESS)h, sleftv_bin);
  return FALSE;
}

static BOOLEAN iiInternalExport (leftv v, int toLev)
{
  idhdl h=(idhdl)v->data;
  //Print("iiInternalExport('%s',%d)%s\n", v->name, toLev,"");
  if (IDLEV(h)==0)
  {
    if (BVERBOSE(V_REDEFINE)) Warn("`%s` is already global",IDID(h));
  }
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

BOOLEAN iiInternalExport (leftv v, int toLev, package rootpack)
{
  idhdl h=(idhdl)v->data;
  if(h==NULL)
  {
    Warn("'%s': no such identifier\n", v->name);
    return FALSE;
  }
  package frompack=v->req_packhdl;
  if (frompack==NULL) frompack=currPack;
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

BOOLEAN iiExport (leftv v, int toLev)
{
  BOOLEAN nok=FALSE;
  leftv r=v;
  while (v!=NULL)
  {
    if ((v->name==NULL)||(v->rtyp==0)||(v->e!=NULL))
    {
      Werror("cannot export:%s of internal type %d",v->name,v->rtyp);
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
  return nok;
}

/*assume root!=idroot*/
BOOLEAN iiExport (leftv v, int toLev, package pack)
{
  BOOLEAN nok=FALSE;
  leftv rv=v;
  while (v!=NULL)
  {
    if ((v->name==NULL)||(v->rtyp==0)||(v->e!=NULL)
    )
    {
      Werror("cannot export:%s of internal type %d",v->name,v->rtyp);
      nok=TRUE;
    }
    else
    {
      idhdl old=pack->idroot->get( v->name,toLev);
      if (old!=NULL)
      {
        if ((pack==currPack) && (old==(idhdl)v->data))
        {
          if (BVERBOSE(V_REDEFINE)) Warn("`%s` is already global",IDID(old));
          break;
        }
        else if (IDTYP(old)==v->Typ())
        {
          if (BVERBOSE(V_REDEFINE))
          {
            Warn("redefining %s",IDID(old));
          }
          v->name=omStrDup(v->name);
          killhdl2(old,&(pack->idroot),currRing);
        }
        else
        {
          rv->CleanUp();
          return TRUE;
        }
      }
      //Print("iiExport: pack=%s\n",IDID(root));
      if(iiInternalExport(v, toLev, pack))
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

BOOLEAN iiCheckRing(int i)
{
  if (currRing==NULL)
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
  if (rHasLocalOrMixedOrdering_currRing())
  {
    scComputeHC(I,currQuotient,ak,po);
    if (po!=NULL)
    {
      pGetCoeff(po)=nInit(1);
      for (i=rVar(currRing); i>0; i--)
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

idhdl rDefault(const char *s)
{
  idhdl tmp=NULL;

  if (s!=NULL) tmp = enterid(s, myynest, RING_CMD, &IDROOT);
  if (tmp==NULL) return NULL;

// if ((currRing->ppNoether)!=NULL) pDelete(&(currRing->ppNoether));
  if (sLastPrinted.RingDependend())
  {
    sLastPrinted.CleanUp();
    memset(&sLastPrinted,0,sizeof(sleftv));
  }

  ring r = IDRING(tmp);

  r->cf = nInitChar(n_Zp, (void*)32003); //   r->cf->ch = 32003;
  r->N      = 3;
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

idhdl rFindHdl(ring r, idhdl n, idhdl)
{
  idhdl h=rSimpleFindHdl(r,IDROOT,n);
  if (h!=NULL)  return h;
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
  L->m[0].data=(void *)(long)r->cf->ch;
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
      if(r->order[i]==ringorder_M) j=(j+1)*(j+1)-1;
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
  if (nCoeff_is_transExt(R->cf))
    L->m[3].data=(void *)idInit(1,1);
  else
  {
    ideal q=idInit(IDELEMS(r->qideal));
    q->m[0]=p_Init(R);
    pSetCoeff0(q->m[0],(number)(r->qideal->m[0]));
    L->m[3].data=(void *)q;
//    I->m[0] = pNSet(R->minpoly);
  }
  // ----------------------------------------
}
void rDecomposeC(leftv h,const coeffs C)
/* field is R or C */
{
  lists L=(lists)omAlloc0Bin(slists_bin);
  if (nCoeff_is_long_C(C)) L->Init(3);
  else                     L->Init(2);
  h->rtyp=LIST_CMD;
  h->data=(void *)L;
  // 0: char/ cf - ring
  // 1: list (var)
  // 2: list (ord)
  // ----------------------------------------
  // 0: char/ cf - ring
  L->m[0].rtyp=INT_CMD;
  L->m[0].data=(void *)0;
  // ----------------------------------------
  // 1:
  lists LL=(lists)omAlloc0Bin(slists_bin);
  LL->Init(2);
    LL->m[0].rtyp=INT_CMD;
    LL->m[0].data=(void *)(long)si_max(C->float_len,SHORT_REAL_LENGTH/2);
    LL->m[1].rtyp=INT_CMD;
    LL->m[1].data=(void *)(long)si_max(C->float_len2,SHORT_REAL_LENGTH);
  L->m[1].rtyp=LIST_CMD;
  L->m[1].data=(void *)LL;
  // ----------------------------------------
  // 2: list (par)
  if (nCoeff_is_long_C(C))
  {
    L->m[2].rtyp=STRING_CMD;
    L->m[2].data=(void *)omStrDup(*n_ParameterNames(C));
  }
  // ----------------------------------------
}

#ifdef HAVE_RINGS
void rDecomposeRing(leftv h,const coeffs C)
/* field is R or C */
{
  lists L=(lists)omAlloc0Bin(slists_bin);
  if (nCoeff_is_Ring(C)) L->Init(1);
  else                   L->Init(2);
  h->rtyp=LIST_CMD;
  h->data=(void *)L;
  // 0: char/ cf - ring
  // 1: list (module)
  // ----------------------------------------
  // 0: char/ cf - ring
  L->m[0].rtyp=STRING_CMD;
  L->m[0].data=(void *)omStrDup("integer");
  // ----------------------------------------
  // 1: modulo
  if (nCoeff_is_Ring_Z(C)) return;
  lists LL=(lists)omAlloc0Bin(slists_bin);
  LL->Init(2);
  LL->m[0].rtyp=BIGINT_CMD;
  LL->m[0].data=nlMapGMP((number) C->modBase, C, coeffs_BIGINT);
  LL->m[1].rtyp=INT_CMD;
  LL->m[1].data=(void *) C->modExponent;
  L->m[1].rtyp=LIST_CMD;
  L->m[1].data=(void *)LL;
}
#endif


BOOLEAN rDecompose_CF(leftv res,const coeffs C)
{
  assume( C != NULL );

  // sanity check: require currRing==r for rings with polynomial data
  if ( nCoeff_is_algExt(C) && (C != currRing->cf))
  {
    WerrorS("ring with polynomial data must be the base ring or compatible");
    return TRUE;
  }
  if (nCoeff_is_numeric(C))
  {
    rDecomposeC(res,C);
  }
#ifdef HAVE_RINGS
  else if (nCoeff_is_Ring(C))
  {
    rDecomposeRing(res,C);
  }
#endif
  else if ( C->extRing!=NULL )// nCoeff_is_algExt(r->cf))
  {
    rDecomposeCF(res, C->extRing, currRing);
  }
  else if(nCoeff_is_GF(C))
  {
    lists Lc=(lists)omAlloc0Bin(slists_bin);
    Lc->Init(4);
    // char:
    Lc->m[0].rtyp=INT_CMD;
    Lc->m[0].data=(void*)(long)C->m_nfCharQ;
    // var:
    lists Lv=(lists)omAlloc0Bin(slists_bin);
    Lv->Init(1);
    Lv->m[0].rtyp=STRING_CMD;
    Lv->m[0].data=(void *)omStrDup(*n_ParameterNames(C));
    Lc->m[1].rtyp=LIST_CMD;
    Lc->m[1].data=(void*)Lv;
    // ord:
    lists Lo=(lists)omAlloc0Bin(slists_bin);
    Lo->Init(1);
    lists Loo=(lists)omAlloc0Bin(slists_bin);
    Loo->Init(2);
    Loo->m[0].rtyp=STRING_CMD;
    Loo->m[0].data=(void *)omStrDup(rSimpleOrdStr(ringorder_lp));

    intvec *iv=new intvec(1); (*iv)[0]=1;
    Loo->m[1].rtyp=INTVEC_CMD;
    Loo->m[1].data=(void *)iv;

    Lo->m[0].rtyp=LIST_CMD;
    Lo->m[0].data=(void*)Loo;

    Lc->m[2].rtyp=LIST_CMD;
    Lc->m[2].data=(void*)Lo;
    // q-ideal:
    Lc->m[3].rtyp=IDEAL_CMD;
    Lc->m[3].data=(void *)idInit(1,1);
    // ----------------------
    res->rtyp=LIST_CMD;
    res->data=(void*)Lc;
  }
  else
  {
    res->rtyp=INT_CMD;
    res->data=(void *)(long)C->ch;
  }
  // ----------------------------------------
  return FALSE;
}
lists rDecompose(const ring r)
{
  assume( r != NULL );
  const coeffs C = r->cf;
  assume( C != NULL );

  // sanity check: require currRing==r for rings with polynomial data
  if ( (r!=currRing) && (
           (nCoeff_is_algExt(C) && (C != currRing->cf))
        || (r->qideal != NULL)
#ifdef HAVE_PLURAL
        || (rIsPluralRing(r))
#endif
                        )
     )
  {
    WerrorS("ring with polynomial data must be the base ring or compatible");
    return NULL;
  }
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
  L->m[0].rtyp=CRING_CMD;
  L->m[0].data=(char*)r->cf; r->cf->ref++;
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

    if(r->order[i] == ringorder_IS) //  || r->order[i] == ringorder_s || r->order[i] == ringorder_S)
    {
      assume( r->block0[i] == r->block1[i] );
      const int s = r->block0[i];
      assume( -2 < s && s < 2);

      iv=new intvec(1);
      (*iv)[0] = s;
    }
    else if (r->block1[i]-r->block0[i] >=0 )
    {
      int bl=j=r->block1[i]-r->block0[i];
      if (r->order[i]==ringorder_M)
      {
        j=(j+1)*(j+1)-1;
        bl=j+1;
      }
      else if (r->order[i]==ringorder_am)
      {
        j+=r->wvhdl[i][bl+1];
      }
      iv=new intvec(j+1);
      if ((r->wvhdl!=NULL) && (r->wvhdl[i]!=NULL))
      {
        for(;j>=0; j--) (*iv)[j]=r->wvhdl[i][j+(j>bl)];
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
#ifdef HAVE_PLURAL // NC! in rDecompose
  if (rIsPluralRing(r))
  {
    L->m[4].rtyp=MATRIX_CMD;
    L->m[4].data=(void *)mp_Copy(r->GetNC()->C, r, r);
    L->m[5].rtyp=MATRIX_CMD;
    L->m[5].data=(void *)mp_Copy(r->GetNC()->D, r, r);
  }
#endif
  return L;
}

void rComposeC(lists L, ring R)
/* field is R or C */
{
  // ----------------------------------------
  // 0: char/ cf - ring
  if ((L->m[0].rtyp!=INT_CMD) || (L->m[0].data!=(char *)0))
  {
    Werror("invald coeff. field description, expecting 0");
    return;
  }
//  R->cf->ch=0;
  // ----------------------------------------
  // 1:
  if (L->m[1].rtyp!=LIST_CMD)
    Werror("invald coeff. field description, expecting precision list");
  lists LL=(lists)L->m[1].data;
  int r1=(int)(long)LL->m[0].data;
  int r2=(int)(long)LL->m[1].data;
  if (L->nr==2) // complex
    R->cf = nInitChar(n_long_C, NULL);
  else if ((r1<=SHORT_REAL_LENGTH)
  && (r2=SHORT_REAL_LENGTH))
    R->cf = nInitChar(n_R, NULL);
  else
    R->cf = nInitChar(n_long_R, NULL);

  if ((r1<=SHORT_REAL_LENGTH)   // should go into nInitChar
  && (r2=SHORT_REAL_LENGTH))
  {
    R->cf->float_len=SHORT_REAL_LENGTH/2;
    R->cf->float_len2=SHORT_REAL_LENGTH;
  }
  else
  {
    R->cf->float_len=si_min(r1,32767);
    R->cf->float_len2=si_min(r2,32767);
  }
  // ----------------------------------------
  // 2: list (par)
  if (L->nr==2)
  {
    //R->cf->extRing->N=1;
    if (L->m[2].rtyp!=STRING_CMD)
    {
      Werror("invald coeff. field description, expecting parameter name");
      return;
    }
    //(rParameter(R))=(char**)omAlloc0(rPar(R)*sizeof(char_ptr));
    rParameter(R)[0]=omStrDup((char *)L->m[2].data);
  }
  // ----------------------------------------
}

#ifdef HAVE_RINGS
void rComposeRing(lists L, ring R)
/* field is R or C */
{
  // ----------------------------------------
  // 0: string: integer
  // no further entries --> Z
  int_number modBase = NULL;
  unsigned int modExponent = 1;

  modBase = (int_number) omAlloc(sizeof(mpz_t));
  if (L->nr == 0)
  {
    mpz_init_set_ui(modBase,0);
    modExponent = 1;
  }
  // ----------------------------------------
  // 1:
  else
  {
    if (L->m[1].rtyp!=LIST_CMD) Werror("invald data, expecting list of numbers");
    lists LL=(lists)L->m[1].data;
    if ((LL->nr >= 0) && LL->m[0].rtyp == BIGINT_CMD)
    {
      number tmp= (number) LL->m[0].data;
      n_MPZ (modBase, tmp, coeffs_BIGINT);
    }
    else if (LL->nr >= 0 && LL->m[0].rtyp == INT_CMD)
    {
      mpz_init_set_ui(modBase,(unsigned long) LL->m[0].data);
    }
    else
    {
      mpz_init_set_ui(modBase,0);
    }
    if (LL->nr >= 1)
    {
      modExponent = (unsigned long) LL->m[1].data;
    }
    else
    {
      modExponent = 1;
    }
  }
  // ----------------------------------------
  if ((mpz_cmp_ui(modBase, 1) == 0) && (mpz_cmp_ui(modBase, 0) < 0))
  {
    Werror("Wrong ground ring specification (module is 1)");
    return;
  }
  if (modExponent < 1)
  {
    Werror("Wrong ground ring specification (exponent smaller than 1");
    return;
  }
  // module is 0 ---> integers
  if (mpz_cmp_ui(modBase, 0) == 0)
  {
    R->cf=nInitChar(n_Z,NULL);
  }
  // we have an exponent
  else if (modExponent > 1)
  {
    //R->cf->ch = R->cf->modExponent;
    if ((mpz_cmp_ui(modBase, 2) == 0) && (modExponent <= 8*sizeof(NATNUMBER)))
    {
      /* this branch should be active for modExponent = 2..32 resp. 2..64,
           depending on the size of a long on the respective platform */
      R->cf=nInitChar(n_Z2m,(void*)(long)modExponent);       // Use Z/2^ch
      omFreeSize (modBase, sizeof(mpz_t));
    }
    else
    {
      //ringtype 3
      ZnmInfo info;
      info.base= modBase;
      info.exp= modExponent;
      R->cf=nInitChar(n_Znm,(void*) &info);
    }
  }
  // just a module m > 1
  else
  {
    //ringtype = 2;
    //const int ch = mpz_get_ui(modBase);
    ZnmInfo info;
    info.base= modBase;
    info.exp= modExponent;
    R->cf=nInitChar(n_Zn,(void*) &info);
  }
}
#endif

static void rRenameVars(ring R)
{
  int i,j;
  for(i=0;i<R->N-1;i++)
  {
    for(j=i+1;j<R->N;j++)
    {
      if (strcmp(R->names[i],R->names[j])==0)
      {
        Warn("name conflict var(%d) and var(%d): `%s`, rename to `@(%d)`",i+1,j+1,R->names[i],j+1);
        omFree(R->names[j]);
        R->names[j]=(char *)omAlloc(10);
        sprintf(R->names[j],"@(%d)",j+1);
      }
    }
  }
  for(i=0;i<rPar(R); i++)
  {
    for(j=0;j<R->N;j++)
    {
      if (strcmp(rParameter(R)[i],R->names[j])==0)
      {
        Warn("name conflict par(%d) and var(%d): `%s`, renaming the VARIABLE to `@@(%d)`",i+1,j+1,R->names[j],i+1);
//        omFree(rParameter(R)[i]);
//        rParameter(R)[i]=(char *)omAlloc(10);
//        sprintf(rParameter(R)[i],"@@(%d)",i+1);
        omFree(R->names[j]);
        R->names[j]=(char *)omAlloc(10);
        sprintf(R->names[j],"@@(%d)",i+1);
      }
    }
  }
}

ring rCompose(const lists  L, const BOOLEAN check_comp)
{
  if ((L->nr!=3)
#ifdef HAVE_PLURAL
  &&(L->nr!=5)
#endif
  )
    return NULL;
  int is_gf_char=0;
  // 0: char/ cf - ring
  // 1: list (var)
  // 2: list (ord)
  // 3: qideal
  // possibly:
  // 4: C
  // 5: D

  ring R = (ring) omAlloc0Bin(sip_sring_bin);


  // ------------------------------------------------------------------
  // 0: char:
  if (L->m[0].Typ()==INT_CMD)
  {
    int ch = (int)(long)L->m[0].Data();
    assume( ch >= 0 );

    if (ch == 0) // Q?
      R->cf = nInitChar(n_Q, NULL);
    else
    {
      int l = IsPrime(ch); // Zp?
      if( l != ch )
      {
        Warn("%d is invalid characteristic of ground field. %d is used.", ch, l);
        ch = l;
      }
      R->cf = nInitChar(n_Zp, (void*)(long)ch);
    }
  }
  else if (L->m[0].Typ()==LIST_CMD) // something complicated...
  {
    lists LL=(lists)L->m[0].Data();

#ifdef HAVE_RINGS
    if (LL->m[0].Typ() == STRING_CMD) // 1st comes a string?
    {
      rComposeRing(LL, R); // Ring!?
    }
    else
#endif
    if (LL->nr < 3)
      rComposeC(LL,R); // R, long_R, long_C
    else
    {
      if (LL->m[0].Typ()==INT_CMD)
      {
        int ch = (int)(long)LL->m[0].Data();
        while ((ch!=fftable[is_gf_char]) && (fftable[is_gf_char])) is_gf_char++;
        if (fftable[is_gf_char]==0) is_gf_char=-1;

        if(is_gf_char!= -1)
        {
          GFInfo param;

          param.GFChar = ch;
          param.GFDegree = 1;
          param.GFPar_name = (const char*)(((lists)(LL->m[1].Data()))->m[0].Data());

          // nfInitChar should be able to handle the case when ch is in fftables!
          R->cf = nInitChar(n_GF, (void*)&param);
        }
      }

      if( R->cf == NULL )
      {
        ring extRing = rCompose((lists)L->m[0].Data(),FALSE);

        if (extRing==NULL)
        {
          WerrorS("could not create the specified coefficient field");
          goto rCompose_err;
        }

        if( extRing->qideal != NULL ) // Algebraic extension
        {
          AlgExtInfo extParam;

          extParam.r = extRing;

          R->cf = nInitChar(n_algExt, (void*)&extParam);
        }
        else // Transcendental extension
        {
          TransExtInfo extParam;
          extParam.r = extRing;
          assume( extRing->qideal == NULL );

          R->cf = nInitChar(n_transExt, &extParam);
        }
      }
    }
  }
  else if (L->m[0].Typ()==CRING_CMD)
  {
    R->cf=(coeffs)L->m[0].Data();
    R->cf->ref++;
  }
  else
  {
    WerrorS("coefficient field must be described by `int` or `list`");
    goto rCompose_err;
  }

  if( R->cf == NULL )
  {
    WerrorS("could not create coefficient field described by the input!");
    goto rCompose_err;
  }

  // ------------------------- VARS ---------------------------
  if (L->m[1].Typ()==LIST_CMD)
  {
    lists v=(lists)L->m[1].Data();
    R->N = v->nr+1;
    if (R->N<=0)
    {
      WerrorS("no ring variables");
      goto rCompose_err;
    }
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
    for (j=0; j < n-1; j++)
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
      else
      {
         int jj=j-1;
         while((jj>=0)
         && ((R->order[jj]== ringorder_a)
            || (R->order[jj]== ringorder_aa)
            || (R->order[jj]== ringorder_am)
            || (R->order[jj]== ringorder_c)
            || (R->order[jj]== ringorder_C)
            || (R->order[jj]== ringorder_s)
            || (R->order[jj]== ringorder_S)
         ))
         {
           //Print("jj=%, skip %s\n",rSimpleOrdStr(R->order[jj]));
           jj--;
         }
         if (jj<0) R->block0[j]=1;
         else       R->block0[j]=R->block1[jj]+1;
      }
      intvec *iv;
      if (vv->m[1].Typ()==INT_CMD)
        iv=new intvec((int)(long)vv->m[1].Data(),(int)(long)vv->m[1].Data());
      else
        iv=ivCopy((intvec*)vv->m[1].Data()); //assume INTVEC
      int iv_len=iv->length();
      R->block1[j]=si_max(R->block0[j],R->block0[j]+iv_len-1);
      if (R->block1[j]>R->N)
      {
        R->block1[j]=R->N;
        iv_len=R->block1[j]-R->block0[j]+1;
      }
      //Print("block %d from %d to %d\n",j,R->block0[j], R->block1[j]);
      int i;
      switch (R->order[j])
      {
         case ringorder_ws:
         case ringorder_Ws:
            R->OrdSgn=-1;
         case ringorder_aa:
         case ringorder_a:
         case ringorder_wp:
         case ringorder_Wp:
           R->wvhdl[j] =( int *)omAlloc(iv_len*sizeof(int));
           for (i=0; i<iv_len;i++)
           {
             R->wvhdl[j][i]=(*iv)[i];
           }
           break;
         case ringorder_am:
           R->wvhdl[j] =( int *)omAlloc((iv->length()+1)*sizeof(int));
           for (i=0; i<iv_len;i++)
           {
             R->wvhdl[j][i]=(*iv)[i];
           }
           R->wvhdl[j][i]=iv->length() - iv_len;
           //printf("ivlen:%d,iv->len:%d,mod:%d\n",iv_len,iv->length(),R->wvhdl[j][i]);
           for (; i<iv->length(); i++)
           {
              R->wvhdl[j][i+1]=(*iv)[i];
           }
           break;
         case ringorder_M:
           R->wvhdl[j] =( int *)omAlloc((iv->length())*sizeof(int));
           for (i=0; i<iv->length();i++) R->wvhdl[j][i]=(*iv)[i];
           R->block1[j]=si_max(R->block0[j],R->block0[j]+(int)sqrt((double)(iv->length()-1)));
           if (R->block1[j]>R->N)
           {
             WerrorS("ordering matrix too big");
             goto rCompose_err;
           }
           break;
         case ringorder_ls:
         case ringorder_ds:
         case ringorder_Ds:
         case ringorder_rs:
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
           R->block1[j]=R->block0[j]=0;
           break;

         case ringorder_s:
           break;

         case ringorder_IS:
         {
           R->block1[j] = R->block0[j] = 0;
           if( iv->length() > 0 )
           {
             const int s = (*iv)[0];
             assume( -2 < s && s < 2 );
             R->block1[j] = R->block0[j] = s;
           }
           break;
         }
         case 0:
         case ringorder_unspec:
           break;
      }
      delete iv;
    }
    // sanity check
    j=n-2;
    if ((R->order[j]==ringorder_c)
    || (R->order[j]==ringorder_C)
    || (R->order[j]==ringorder_unspec)) j--;
    if (R->block1[j] != R->N)
    {
      if (((R->order[j]==ringorder_dp) ||
           (R->order[j]==ringorder_ds) ||
           (R->order[j]==ringorder_Dp) ||
           (R->order[j]==ringorder_Ds) ||
           (R->order[j]==ringorder_rp) ||
           (R->order[j]==ringorder_rs) ||
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
    if (R->block0[j]>R->N)
    {
      Werror("not enough variables (%d) for ordering block %d, scanned so far:",R->N,j+1);
      for(int ii=0;ii<=j;ii++)
        Werror("ord[%d]: %s from v%d to v%d",ii+1,rSimpleOrdStr(R->order[ii]),R->block0[ii],R->block1[ii]);
      goto rCompose_err;
    }
    if (check_comp)
    {
      BOOLEAN comp_order=FALSE;
      int jj;
      for(jj=0;jj<n;jj++)
      {
        if ((R->order[jj]==ringorder_c) ||
            (R->order[jj]==ringorder_C)) { comp_order=TRUE; break; }
      }
      if (!comp_order)
      {
        R->order=(int*)omRealloc0Size(R->order,n*sizeof(int),(n+1)*sizeof(int));
        R->block0=(int*)omRealloc0Size(R->block0,n*sizeof(int),(n+1)*sizeof(int));
        R->block1=(int*)omRealloc0Size(R->block1,n*sizeof(int),(n+1)*sizeof(int));
        R->wvhdl=(int**)omRealloc0Size(R->wvhdl,n*sizeof(int_ptr),(n+1)*sizeof(int_ptr));
        R->order[n-1]=ringorder_C;
        R->block0[n-1]=0;
        R->block1[n-1]=0;
        R->wvhdl[n-1]=NULL;
        n++;
      }
    }
  }
  else
  {
    WerrorS("ordering must be given as `list`");
    goto rCompose_err;
  }

  // ------------------------ ??????? --------------------

  rRenameVars(R);
  rComplete(R);

/*#ifdef HAVE_RINGS
// currently, coefficients which are ring elements require a global ordering:
  if (rField_is_Ring(R) && (R->OrdSgn==-1))
  {
    WerrorS("global ordering required for these coefficients");
    goto rCompose_err;
  }
#endif*/


  // ------------------------ Q-IDEAL ------------------------

  if (L->m[3].Typ()==IDEAL_CMD)
  {
    ideal q=(ideal)L->m[3].Data();
    if (q->m[0]!=NULL)
    {
      if (R->cf != currRing->cf) //->cf->ch!=currRing->cf->ch)
      {
      #if 0
            WerrorS("coefficient fields must be equal if q-ideal !=0");
            goto rCompose_err;
      #else
        ring orig_ring=currRing;
        rChangeCurrRing(R);
        int *perm=NULL;
        int *par_perm=NULL;
        int par_perm_size=0;
        nMapFunc nMap;

        if ((nMap=nSetMap(orig_ring->cf))==NULL)
        {
          if (rEqual(orig_ring,currRing))
          {
            nMap=n_SetMap(currRing->cf, currRing->cf);
          }
          else
          // Allow imap/fetch to be make an exception only for:
          if ( (rField_is_Q_a(orig_ring) &&  // Q(a..) -> Q(a..) || Q || Zp || Zp(a)
            (rField_is_Q(currRing) || rField_is_Q_a(currRing) ||
             (rField_is_Zp(currRing) || rField_is_Zp_a(currRing))))
           ||
           (rField_is_Zp_a(orig_ring) &&  // Zp(a..) -> Zp(a..) || Zp
            (rField_is_Zp(currRing, rInternalChar(orig_ring)) ||
             rField_is_Zp_a(currRing, rInternalChar(orig_ring)))) )
          {
            par_perm_size=rPar(orig_ring);

//            if ((orig_ring->minpoly != NULL) || (orig_ring->qideal != NULL))
//              naSetChar(rInternalChar(orig_ring),orig_ring);
//            else ntSetChar(rInternalChar(orig_ring),orig_ring);

            nSetChar(currRing->cf);
          }
          else
          {
            WerrorS("coefficient fields must be equal if q-ideal !=0");
            goto rCompose_err;
          }
        }
        perm=(int *)omAlloc0((orig_ring->N+1)*sizeof(int));
        if (par_perm_size!=0)
          par_perm=(int *)omAlloc0(par_perm_size*sizeof(int));
        int i;
        #if 0
        // use imap:
        maFindPerm(orig_ring->names,orig_ring->N,orig_ring->parameter,orig_ring->P,
          currRing->names,currRing->N,currRing->parameter, currRing->P,
          perm,par_perm, currRing->ch);
        #else
        // use fetch
        if ((rPar(orig_ring)>0) && (rPar(currRing)==0))
        {
          for(i=si_min(rPar(orig_ring),rVar(currRing))-1;i>=0;i--) par_perm[i]=i+1;
        }
        else if (par_perm_size!=0)
          for(i=si_min(rPar(orig_ring),rPar(currRing))-1;i>=0;i--) par_perm[i]=-(i+1);
        for(i=si_min(orig_ring->N,rVar(currRing));i>0;i--) perm[i]=i;
        #endif
        ideal dest_id=idInit(IDELEMS(q),1);
        for(i=IDELEMS(q)-1; i>=0; i--)
        {
          dest_id->m[i]=p_PermPoly(q->m[i],perm,orig_ring, currRing,nMap,
                                  par_perm,par_perm_size);
          //  PrintS("map:");pWrite(dest_id->m[i]);PrintLn();
          pTest(dest_id->m[i]);
        }
        R->qideal=dest_id;
        if (perm!=NULL)
          omFreeSize((ADDRESS)perm,(orig_ring->N+1)*sizeof(int));
        if (par_perm!=NULL)
          omFreeSize((ADDRESS)par_perm,par_perm_size*sizeof(int));
        rChangeCurrRing(orig_ring);
      #endif
      }
      else
        R->qideal=idrCopyR(q,currRing,R);
    }
  }
  else
  {
    WerrorS("q-ideal must be given as `ideal`");
    goto rCompose_err;
  }


  // ---------------------------------------------------------------
  #ifdef HAVE_PLURAL
  if (L->nr==5)
  {
    if (nc_CallPlural((matrix)L->m[4].Data(),
                      (matrix)L->m[5].Data(),
                      NULL,NULL,
                      R,
                      true, // !!!
                      true, false,
                      currRing, FALSE)) goto rCompose_err;
    // takes care about non-comm. quotient! i.e. calls "nc_SetupQuotient" due to last true
  }
  #endif
  return R;

rCompose_err:
  if (R->N>0)
  {
    int i;
    if (R->names!=NULL)
    {
      i=R->N-1;
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

  result =mpNew(IDELEMS(id),rVar(currRing));
  for (i=1; i<=IDELEMS(id); i++)
  {
    for (j=1; j<=rVar(currRing); j++)
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
  int n=(int)(long)b->Data();
  int d=(int)(long)c->Data();
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

  BOOLEAN minim=(int)(long)w->Data();
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
  atSet(res,omStrDup("rowShift"),(void*)(long)add_row_shift,INT_CMD);

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
lists syConvRes(syStrategy syzstr,BOOLEAN toDel,int add_row_shift)
{
  resolvente fullres = syzstr->fullres;
  resolvente minres = syzstr->minres;

  const int length = syzstr->length;

  if ((fullres==NULL) && (minres==NULL))
  {
    if (syzstr->hilb_coeffs==NULL)
    { // La Scala
      fullres = syReorder(syzstr->res, length, syzstr);
    }
    else
    { // HRES
      minres = syReorder(syzstr->orderedRes, length, syzstr);
      syKillEmptyEntres(minres, length);
    }
  }

  resolvente tr;
  int typ0=IDEAL_CMD;

  if (minres!=NULL)
    tr = minres;
  else
    tr = fullres;

  resolvente trueres=NULL; intvec ** w=NULL;

  if (length>0)
  {
    trueres = (resolvente)omAlloc0((length)*sizeof(ideal));
    for (int i=(length)-1;i>=0;i--)
    {
      if (tr[i]!=NULL)
      {
        trueres[i] = idCopy(tr[i]);
      }
    }
    if ( id_RankFreeModule(trueres[0], currRing) > 0)
      typ0 = MODUL_CMD;
    if (syzstr->weights!=NULL)
    {
      w = (intvec**)omAlloc0(length*sizeof(intvec*));
      for (int i=length-1;i>=0;i--)
      {
        if (syzstr->weights[i]!=NULL) w[i] = ivCopy(syzstr->weights[i]);
      }
    }
  }

  lists li = liMakeResolv(trueres, length, syzstr->list_length,typ0,
                          w, add_row_shift);

  if (w != NULL) omFreeSize(w, length*sizeof(intvec*));

  if (toDel)
    syKillComputation(syzstr);
  else
  {
    if( fullres != NULL && syzstr->fullres == NULL )
      syzstr->fullres = fullres;

    if( minres != NULL && syzstr->minres == NULL )
      syzstr->minres = minres;
  }

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
  intvec * iv = new intvec(rVar(currRing));
  polyset s;
  int  sl, n, i;
  int  *x;

  res->data=(char *)iv;
  s = F->m;
  sl = IDELEMS(F) - 1;
  n = rVar(currRing);
  double wNsqr = (double)2.0 / (double)n;
  wFunctional = wFunctionalBuch;
  x = (int * )omAlloc(2 * (n + 1) * sizeof(int));
  wCall(s, sl, x, wNsqr, currRing);
  for (i = n; i!=0; i--)
    (*iv)[i-1] = x[i + n + 1];
  omFreeSize((ADDRESS)x, 2 * (n + 1) * sizeof(int));
  return FALSE;
}

BOOLEAN kQHWeight(leftv res,leftv v)
{
  res->data=(char *)id_QHomWeight((ideal)v->Data(), currRing);
  if (res->data==NULL)
    res->data=(char *)new intvec(rVar(currRing));
  return FALSE;
}
/*==============================================================*/
// from clapsing.cc
#if 0
BOOLEAN jjIS_SQR_FREE(leftv res, leftv u)
{
  BOOLEAN b=singclap_factorize((poly)(u->CopyD()), &v, 0);
  res->data=(void *)b;
}
#endif

BOOLEAN jjRESULTANT(leftv res, leftv u, leftv v, leftv w)
{
  res->data=singclap_resultant((poly)u->CopyD(),(poly)v->CopyD(),
                  (poly)w->CopyD(), currRing);
  return errorreported;
}

BOOLEAN jjCHARSERIES(leftv res, leftv u)
{
  res->data=singclap_irrCharSeries((ideal)u->Data(), currRing);
  return (res->data==NULL);
}

// from semic.cc
#ifdef HAVE_SPECTRUM

// ----------------------------------------------------------------------------
//  Initialize a  spectrum  deep from a  singular  lists
// ----------------------------------------------------------------------------

void copy_deep( spectrum& spec, lists l )
{
    spec.mu = (int)(long)(l->m[0].Data( ));
    spec.pg = (int)(long)(l->m[1].Data( ));
    spec.n  = (int)(long)(l->m[2].Data( ));

    spec.copy_new( spec.n );

    intvec  *num = (intvec*)l->m[3].Data( );
    intvec  *den = (intvec*)l->m[4].Data( );
    intvec  *mul = (intvec*)l->m[5].Data( );

    for( int i=0; i<spec.n; i++ )
    {
        spec.s[i] = (Rational)((*num)[i])/(Rational)((*den)[i]);
        spec.w[i] = (*mul)[i];
    }
}

// ----------------------------------------------------------------------------
//  singular lists  constructor for  spectrum
// ----------------------------------------------------------------------------

spectrum /*former spectrum::spectrum ( lists l )*/
spectrumFromList( lists l )
{
    spectrum result;
    copy_deep( result, l );
    return result;
}

// ----------------------------------------------------------------------------
//  generate a Singular  lists  from a spectrum
// ----------------------------------------------------------------------------

/* former spectrum::thelist ( void )*/
lists   getList( spectrum& spec )
{
    lists   L  = (lists)omAllocBin( slists_bin);

    L->Init( 6 );

    intvec            *num  = new intvec( spec.n );
    intvec            *den  = new intvec( spec.n );
    intvec            *mult = new intvec( spec.n );

    for( int i=0; i<spec.n; i++ )
    {
        (*num) [i] = spec.s[i].get_num_si( );
        (*den) [i] = spec.s[i].get_den_si( );
        (*mult)[i] = spec.w[i];
    }

    L->m[0].rtyp = INT_CMD;    //  milnor number
    L->m[1].rtyp = INT_CMD;    //  geometrical genus
    L->m[2].rtyp = INT_CMD;    //  # of spectrum numbers
    L->m[3].rtyp = INTVEC_CMD; //  numerators
    L->m[4].rtyp = INTVEC_CMD; //  denomiantors
    L->m[5].rtyp = INTVEC_CMD; //  multiplicities

    L->m[0].data = (void*)(long)spec.mu;
    L->m[1].data = (void*)(long)spec.pg;
    L->m[2].data = (void*)(long)spec.n;
    L->m[3].data = (void*)num;
    L->m[4].data = (void*)den;
    L->m[5].data = (void*)mult;

    return  L;
}
// from spectrum.cc
// ----------------------------------------------------------------------------
//  print out an error message for a spectrum list
// ----------------------------------------------------------------------------

typedef enum
{
    semicOK,
    semicMulNegative,

    semicListTooShort,
    semicListTooLong,

    semicListFirstElementWrongType,
    semicListSecondElementWrongType,
    semicListThirdElementWrongType,
    semicListFourthElementWrongType,
    semicListFifthElementWrongType,
    semicListSixthElementWrongType,

    semicListNNegative,
    semicListWrongNumberOfNumerators,
    semicListWrongNumberOfDenominators,
    semicListWrongNumberOfMultiplicities,

    semicListMuNegative,
    semicListPgNegative,
    semicListNumNegative,
    semicListDenNegative,
    semicListMulNegative,

    semicListNotSymmetric,
    semicListNotMonotonous,

    semicListMilnorWrong,
    semicListPGWrong

} semicState;

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

enum    spectrumState
{
    spectrumOK,
    spectrumZero,
    spectrumBadPoly,
    spectrumNoSingularity,
    spectrumNotIsolated,
    spectrumDegenerate,
    spectrumWrongRing,
    spectrumNoHC,
    spectrumUnspecErr
};

// from splist.cc
// ----------------------------------------------------------------------------
//  Compute the spectrum of a  spectrumPolyList
// ----------------------------------------------------------------------------

/* former spectrumPolyList::spectrum ( lists*, int) */
spectrumState   spectrumStateFromList( spectrumPolyList& speclist, lists *L,int fast )
{
  spectrumPolyNode  **node = &speclist.root;
  spectrumPolyNode  *search;

  poly              f,tmp;
  int               found,cmp;

  Rational smax( ( fast==0 ? 0 : rVar(currRing) ),
                 ( fast==2 ? 2 : 1 ) );

  Rational weight_prev( 0,1 );

  int     mu = 0;          // the milnor number
  int     pg = 0;          // the geometrical genus
  int     n  = 0;          // number of different spectral numbers
  int     z  = 0;          // number of spectral number equal to smax

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
      speclist.delete_node( node );
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

  for( search=speclist.root; search!=(spectrumPolyNode*)NULL &&
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
      (*nom) [n2] = rVar(currRing)*(*den)[n1]-(*nom)[n1];
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
          (*nom)[n1]+(*nom)[n2]!=rVar(currRing)*(*den) [n1] )
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
      (*L)->m[0].data = (void*)(long)mu;

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

  (*L)->m[0].data = (void*)(long)mu;
  (*L)->m[1].data = (void*)(long)pg;
  (*L)->m[2].data = (void*)(long)n;
  (*L)->m[3].data = (void*)nom;
  (*L)->m[4].data = (void*)den;
  (*L)->m[5].data = (void*)mult;

  return  spectrumOK;
}

spectrumState   spectrumCompute( poly h,lists *L,int fast )
{
  int i;

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

  if( hasConstTerm( h, currRing ) )
  {
    return  spectrumBadPoly;
  }

  // --------------------------------
  //  check if  h  has a linear term
  // --------------------------------

  if( hasLinearTerm( h, currRing ) )
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
  J = idInit( rVar(currRing),1 );

  #ifdef SPECTRUM_DEBUG
  #ifdef SPECTRUM_PRINT
  #ifdef SPECTRUM_IOSTREAM
    cout << "\n   computing the Jacobi ideal...\n";
  #else
    fprintf( stdout,"\n   computing the Jacobi ideal...\n" );
  #endif
  #endif
  #endif

  for( i=0; i<rVar(currRing); i++ )
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

  if( hasOne( stdJ, currRing ) )
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

  for( i=rVar(currRing); i>0; i-- )
  {
    if( hasAxis( stdJ,i, currRing )==FALSE )
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

    for( i=rVar(currRing); i>0; i-- )
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

  newtonPolygon nph( h, currRing );

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
               ( fast==1 ? computeWC( nph,(Rational)rVar(currRing), currRing ) :
              /* fast==2 */computeWC( nph,
                      ((Rational)rVar(currRing))/(Rational)2, currRing ) ) );

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

  computeNF( stdJ,hc,wc,&NF, currRing );

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
//  spectrumState spectrumStateFromList( spectrumPolyList& speclist, lists *L, int fast );

  return spectrumStateFromList(NF, L, fast );
}

// ----------------------------------------------------------------------------
//  this procedure is called from the interpreter
// ----------------------------------------------------------------------------
//  first  = polynomial
//  result = list of spectrum numbers
// ----------------------------------------------------------------------------

void spectrumPrintError(spectrumState state)
{
  switch( state )
  {
    case spectrumZero:
      WerrorS( "polynomial is zero" );
      break;
    case spectrumBadPoly:
      WerrorS( "polynomial has constant term" );
      break;
    case spectrumNoSingularity:
      WerrorS( "not a singularity" );
      break;
    case spectrumNotIsolated:
      WerrorS( "the singularity is not isolated" );
      break;
    case spectrumNoHC:
      WerrorS( "highest corner cannot be computed" );
      break;
    case spectrumDegenerate:
      WerrorS( "principal part is degenerate" );
      break;
    case spectrumOK:
      break;

    default:
      WerrorS( "unknown error occurred" );
      break;
  }
}

BOOLEAN spectrumProc( leftv result,leftv first )
{
  spectrumState state = spectrumOK;

  // -------------------
  //  check consistency
  // -------------------

  //  check for a local ring

  if( !ringIsLocal(currRing ) )
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

    int     mu = (int)(long)(l->m[0].Data( ));
    int     pg = (int)(long)(l->m[1].Data( ));
    int     n  = (int)(long)(l->m[2].Data( ));

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
        if( (*num)[i] != rVar(currRing)*((*den)[i]) - (*num)[j] ||
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

    if( mu != (int)(long)(l->m[0].Data( )) )
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

    if( pg != (int)(long)(l->m[1].Data( )) )
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
        spectrum s1= spectrumFromList ( l1 );
        spectrum s2= spectrumFromList ( l2 );
        spectrum sum( s1+s2 );

        result->rtyp = LIST_CMD;
        result->data = (char*)(getList(sum));
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
    int     k = (int)(long)second->Data( );

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
        spectrum s= spectrumFromList( l );
        spectrum product( k*s );

        result->rtyp = LIST_CMD;
        result->data = (char*)getList(product);
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
  BOOLEAN qh=(((int)(long)w->Data())==1);

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
    spectrum s1= spectrumFromList( l1 );
    spectrum s2= spectrumFromList( l2 );

    res->rtyp = INT_CMD;
    if (qh)
      res->data = (void*)(long)(s1.mult_spectrumh( s2 ));
    else
      res->data = (void*)(long)(s1.mult_spectrum( s2 ));
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
  if ( !(rField_is_long_R(currRing)) )
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
    LP->m= (int)(long)(v->Data());

  v= v->next;
  if ( v->Typ() != INT_CMD )    // 3: n = number of variables
    return TRUE;
  else
    LP->n= (int)(long)(v->Data());

  v= v->next;
  if ( v->Typ() != INT_CMD )    // 4: m1 = number of <= constraints
    return TRUE;
  else
    LP->m1= (int)(long)(v->Data());

  v= v->next;
  if ( v->Typ() != INT_CMD )    // 5: m2 = number of >= constraints
    return TRUE;
  else
    LP->m2= (int)(long)(v->Data());

  v= v->next;
  if ( v->Typ() != INT_CMD )    // 6: m3 = number of == constraints
    return TRUE;
  else
    LP->m3= (int)(long)(v->Data());

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
  lres->m[1].data=(void*)(long)LP->icase;

  lres->m[2].rtyp= INTVEC_CMD;
  lres->m[2].data=(void*)LP->posvToIV();

  lres->m[3].rtyp= INTVEC_CMD;
  lres->m[3].data=(void*)LP->zrovToIV();

  lres->m[4].rtyp= INT_CMD;
  lres->m[4].data=(void*)(long)LP->m;

  lres->m[5].rtyp= INT_CMD;
  lres->m[5].data=(void*)(long)LP->n;

  res->data= (void*)lres;

  return FALSE;
}

BOOLEAN nuMPResMat( leftv res, leftv arg1, leftv arg2 )
{
  ideal gls = (ideal)(arg1->Data());
  int imtype= (int)(long)arg2->Data();

  uResultant::resMatType mtype= determineMType( imtype );

  // check input ideal ( = polynomial system )
  if ( mprIdealCheck( gls, arg1->Name(), mtype, true ) != mprOk )
  {
    return TRUE;
  }

  uResultant *resMat= new uResultant( gls, mtype, false );
  if (resMat!=NULL)
  {
    res->rtyp = MODUL_CMD;
    res->data= (void*)resMat->accessResMat()->getMatrix();
    if (!errorreported) delete resMat;
  }
  return errorreported;
}

BOOLEAN nuLagSolve( leftv res, leftv arg1, leftv arg2, leftv arg3 )
{

  poly gls;
  gls= (poly)(arg1->Data());
  int howclean= (int)(long)arg3->Data();

  if ( !(rField_is_R(currRing) ||
         rField_is_Q(currRing) ||
         rField_is_long_R(currRing) ||
         rField_is_long_C(currRing)) )
  {
    WerrorS("Ground field not implemented!");
    return TRUE;
  }

  if ( !(rField_is_R(currRing) || rField_is_long_R(currRing) || \
                          rField_is_long_C(currRing)) )
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
  int deg= currRing->pLDeg( gls, &ldummy, currRing );
  //  int deg= pDeg( gls );
  //  int len= pLength( gls );
  int i,vpos=0;
  poly piter;
  lists elist;
  lists rlist;

  elist= (lists)omAlloc( sizeof(slists) );
  elist->Init( 0 );

  if ( rVar(currRing) > 1 )
  {
    piter= gls;
    for ( i= 1; i <= rVar(currRing); i++ )
      if ( pGetExp( piter, i ) )
      {
        vpos= i;
        break;
      }
    while ( piter )
    {
      for ( i= 1; i <= rVar(currRing); i++ )
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
  char *dummy;
  int j;

  rlist= (lists)omAlloc( sizeof(slists) );
  rlist->Init( elem );

  if (rField_is_long_C(currRing))
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
      dummy = complexToStr( (*roots)[j], gmp_output_digits, currRing->cf );
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
  int tdg= (int)(long)arg3->Data();

  res->data= (void*)NULL;

  // check the input
  if ( tdg < 1 )
  {
    WerrorS("Last input parameter must be > 0!");
    return TRUE;
  }
  if ( n != rVar(currRing) )
  {
    Werror("Size of first input ideal must be equal to %d!",rVar(currRing));
    return TRUE;
  }
  if ( m != (int)pow((double)tdg+1,(double)n) )
  {
    Werror("Size of second input ideal must be equal to %d!",
      (int)pow((double)tdg+1,(double)n));
    return TRUE;
  }
  if ( !(rField_is_Q(currRing) /* ||
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
  else imtype= (int)(long)v->Data();
  v= v->next;

  if (imtype==0)
  {
    ideal test_id=idInit(1,1);
    int j;
    for(j=IDELEMS(gls)-1;j>=0;j--)
    {
      if (gls->m[j]!=NULL)
      {
        test_id->m[0]=gls->m[j];
        intvec *dummy_w=id_QHomWeight(test_id, currRing);
        if (dummy_w!=NULL)
        {
          WerrorS("Newton polytope not of expected dimension");
          delete dummy_w;
          return TRUE;
        }
      }
    }
  }

  // get and set precision in digits ( > 0 )
  if ( v->Typ() != INT_CMD )
    return TRUE;
  else if ( !(rField_is_R(currRing) || rField_is_long_R(currRing) || \
                          rField_is_long_C(currRing)) )
  {
    unsigned long int ii=(unsigned long int)v->Data();
    setGMPFloatDigits( ii, ii );
  }
  v= v->next;

  // get interpolation steps (0,1,2)
  if ( v->Typ() != INT_CMD )
    return TRUE;
  else howclean= (int)(long)v->Data();

  uResultant::resMatType mtype= determineMType( imtype );
  int i,count;
  lists listofroots= NULL;
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
  int c= iproots[0]->getAnzElems();
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
    listofroots= listOfRoots(arranger, gmp_output_digits );
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
lists listOfRoots( rootArranger* self, const unsigned int oprec )
{
  int i,j;
  int count= self->roots[0]->getAnzRoots(); // number of roots
  int elem= self->roots[0]->getAnzElems();  // number of koordinates per root

  lists listofroots= (lists)omAlloc( sizeof(slists) ); // must be done this way!

  if ( self->found_roots )
  {
    listofroots->Init( count );

    for (i=0; i < count; i++)
    {
      lists onepoint= (lists)omAlloc(sizeof(slists)); // must be done this way!
      onepoint->Init(elem);
      for ( j= 0; j < elem; j++ )
      {
        if ( !rField_is_long_C(currRing) )
        {
          onepoint->m[j].rtyp=STRING_CMD;
          onepoint->m[j].data=(void *)complexToStr((*self->roots[j])[i],oprec, currRing->cf);
        }
        else
        {
          onepoint->m[j].rtyp=NUMBER_CMD;
          onepoint->m[j].data=(void *)n_Copy((number)(self->roots[j]->getRoot(i)), currRing->cf);
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

// from ring.cc
void rSetHdl(idhdl h)
{
  ring rg = NULL;
  if (h!=NULL)
  {
//   Print(" new ring:%s (l:%d)\n",IDID(h),IDLEV(h));
    rg = IDRING(h);
    if (rg==NULL) return; //id <>NULL, ring==NULL
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

  // test for valid "currRing":
  if ((rg!=NULL) && (rg->idroot==NULL))
  {
    ring old=rg;
    rg=rAssure_HasComp(rg);
    if (old!=rg)
    {
      rKill(old);
      IDRING(h)=rg;
    }
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
    if (((*iv)[1]==ringorder_c)||((*iv)[1]==ringorder_C))
      i++;
    else if ((*iv)[1]==ringorder_L)
    {
      R->bitmask=(*iv)[2];
      n--;
    }
    else if (((*iv)[1]!=ringorder_a)
    && ((*iv)[1]!=ringorder_a64)
    && ((*iv)[1]!=ringorder_am))
      o++;
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

  int *weights=(int*)omAlloc0((R->N+1)*sizeof(int));

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
      typ=1;
      switch ((*iv)[1])
      {
          case ringorder_ws:
          case ringorder_Ws:
            typ=-1;
          case ringorder_wp:
          case ringorder_Wp:
            R->wvhdl[n]=(int*)omAlloc((iv->length()-1)*sizeof(int));
            R->block0[n] = last+1;
            for (i=2; i<iv->length(); i++)
            {
              R->wvhdl[n][i-2] = (*iv)[i];
              last++;
              if (weights[last]==0) weights[last]=(*iv)[i]*typ;
            }
            R->block1[n] = last;
            break;
          case ringorder_ls:
          case ringorder_ds:
          case ringorder_Ds:
          case ringorder_rs:
            typ=-1;
          case ringorder_lp:
          case ringorder_dp:
          case ringorder_Dp:
          case ringorder_rp:
            R->block0[n] = last+1;
            if (iv->length() == 3) last+=(*iv)[2];
            else last += (*iv)[0];
            R->block1[n] = last;
            //if ((R->block0[n]>R->block1[n])
            //|| (R->block1[n]>rVar(R)))
            //{
            //  R->block1[n]=rVar(R);
            //  //WerrorS("ordering larger than number of variables");
            //  break;
            //}
            if (rCheckIV(iv)) return TRUE;
            for(i=si_min(rVar(R),R->block1[n]);i>=R->block0[n];i--)
            {
              if (weights[i]==0) weights[i]=typ;
            }
            break;

          case ringorder_s: // no 'rank' params!
          {

            if(iv->length() > 3)
              return TRUE;

            if(iv->length() == 3)
            {
              const int s = (*iv)[2];
              R->block0[n] = s;
              R->block1[n] = s;
            }
            break;
          }
          case ringorder_IS:
          {
            if(iv->length() != 3) return TRUE;

            const int s = (*iv)[2];

            if( 1 < s || s < -1 ) return TRUE;

            R->block0[n] = s;
            R->block1[n] = s;
            break;
          }
          case ringorder_S:
          case ringorder_c:
          case ringorder_C:
          {
            if (rCheckIV(iv)) return TRUE;
            break;
          }
          case ringorder_aa:
          case ringorder_a:
          {
            R->block0[n] = last+1;
            R->block1[n] = si_min(last+iv->length()-2 , rVar(R));
            R->wvhdl[n] = (int*)omAlloc((iv->length()-1)*sizeof(int));
            for (i=2; i<iv->length(); i++)
            {
              R->wvhdl[n][i-2]=(*iv)[i];
              last++;
              if (weights[last]==0) weights[last]=(*iv)[i]*typ;
            }
            last=R->block0[n]-1;
            break;
          }
          case ringorder_am:
          {
            R->block0[n] = last+1;
            R->block1[n] = si_min(last+iv->length()-2 , rVar(R));
            R->wvhdl[n] = (int*)omAlloc(iv->length()*sizeof(int));
            if (R->block1[n]- R->block0[n]+2>=iv->length())
               WarnS("missing module weights");
            for (i=2; i<=(R->block1[n]-R->block0[n]+2); i++)
            {
              R->wvhdl[n][i-2]=(*iv)[i];
              last++;
              if (weights[last]==0) weights[last]=(*iv)[i]*typ;
            }
            R->wvhdl[n][i-2]=iv->length() -3 -(R->block1[n]- R->block0[n]);
            for (; i<iv->length(); i++)
            {
              R->wvhdl[n][i-1]=(*iv)[i];
            }
            last=R->block0[n]-1;
            break;
          }
          case ringorder_a64:
          {
            R->block0[n] = last+1;
            R->block1[n] = si_min(last+iv->length()-2 , rVar(R));
            R->wvhdl[n] = (int*)omAlloc((iv->length()-1)*sizeof(int64));
            int64 *w=(int64 *)R->wvhdl[n];
            for (i=2; i<iv->length(); i++)
            {
              w[i-2]=(*iv)[i];
              last++;
              if (weights[last]==0) weights[last]=(*iv)[i]*typ;
            }
            last=R->block0[n]-1;
            break;
          }
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
            for(i=si_min(rVar(R),R->block1[n]);i>=R->block0[n];i--)
            {
              if (weights[i]==0) weights[i]=typ;
            }
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
  while ( n >= 0 && (
          (R->order[n]==ringorder_c)
      ||  (R->order[n]==ringorder_C)
      ||  (R->order[n]==ringorder_s)
      ||  (R->order[n]==ringorder_S)
      ||  (R->order[n]==ringorder_IS)
                    )) n--;

  assume( n >= 0 );

  if (R->block1[n] != R->N)
  {
    if (((R->order[n]==ringorder_dp) ||
         (R->order[n]==ringorder_ds) ||
         (R->order[n]==ringorder_Dp) ||
         (R->order[n]==ringorder_Ds) ||
         (R->order[n]==ringorder_rp) ||
         (R->order[n]==ringorder_rs) ||
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
  // find OrdSgn:
  R->OrdSgn = 1;
  for(i=1;i<=R->N;i++)
  { if (weights[i]<0) { R->OrdSgn=-1;break; }}
  omFree(weights);
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

const short MAX_SHORT = 32767; // (1 << (sizeof(short)*8)) - 1;

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
#ifdef HAVE_RINGS
  //unsigned int ringtype = 0;
  int_number modBase = NULL;
  unsigned int modExponent = 1;
#endif
  int float_len=0;
  int float_len2=0;
  ring R = NULL;
  //BOOLEAN ffChar=FALSE;

  /* ch -------------------------------------------------------*/
  // get ch of ground field

  // allocated ring
  R = (ring) omAlloc0Bin(sip_sring_bin);

  coeffs cf = NULL;

  assume( pn != NULL );
  const int P = pn->listLength();

  if (pn->Typ()==CRING_CMD)
  {
    cf=(coeffs)pn->CopyD();
    if(P>1) /*parameter*/
    {
      pn = pn->next;
      const int pars = pn->listLength();
      assume( pars > 0 );
      char ** names = (char**)omAlloc0(pars * sizeof(char_ptr));

      if (rSleftvList2StringArray(pn, names))
      {
        WerrorS("parameter expected");
        goto rInitError;
      }

      TransExtInfo extParam;

      extParam.r = rDefault( cf, pars, names); // Q/Zp [ p_1, ... p_pars ]
      for(int i=pars-1; i>=0;i--)
      {
        omFree(names[i]);
      }
      omFree(names);

      cf = nInitChar(n_transExt, &extParam);
    }
    assume( cf != NULL );
  }
  else if (pn->Typ()==INT_CMD)
  {
    int ch = (int)(long)pn->Data();

    /* parameter? -------------------------------------------------------*/
    pn = pn->next;

    if (pn == NULL) // no params!?
    {
      if (ch!=0)
      {
        int ch2=IsPrime(ch);
        if ((ch<2)||(ch!=ch2))
        {
          Warn("%d is invalid as characteristic of the ground field. 32003 is used.", ch);
          ch=32003;
        }
        cf = nInitChar(n_Zp, (void*)(long)ch);
      }
      else
        cf = nInitChar(n_Q, (void*)(long)ch);
    }
    else
    {
      const int pars = pn->listLength();

      assume( pars > 0 );

      // predefined finite field: (p^k, a)
      if ((ch!=0) && (ch!=IsPrime(ch)) && (pars == 1))
      {
        GFInfo param;

        param.GFChar = ch;
        param.GFDegree = 1;
        param.GFPar_name = pn->name;

        cf = nInitChar(n_GF, &param);
      }
      else // (0/p, a, b, ..., z)
      {
        if ((ch!=0) && (ch!=IsPrime(ch)))
        {
          WerrorS("too many parameters");
          goto rInitError;
        }

        char ** names = (char**)omAlloc0(pars * sizeof(char_ptr));

        if (rSleftvList2StringArray(pn, names))
        {
          WerrorS("parameter expected");
          goto rInitError;
        }

        TransExtInfo extParam;

        extParam.r = rDefault( ch, pars, names); // Q/Zp [ p_1, ... p_pars ]
        for(int i=pars-1; i>=0;i--)
        {
          omFree(names[i]);
        }
        omFree(names);

        cf = nInitChar(n_transExt, &extParam);
      }
    }

//    if (cf==NULL) goto rInitError;
    assume( cf != NULL );
  }
  else if ((pn->name != NULL)
  && ((strcmp(pn->name,"real")==0) || (strcmp(pn->name,"complex")==0)))
  {
    BOOLEAN complex_flag=(strcmp(pn->name,"complex")==0);
    if ((pn->next!=NULL) && (pn->next->Typ()==INT_CMD))
    {
      float_len=(int)(long)pn->next->Data();
      float_len2=float_len;
      pn=pn->next;
      if ((pn->next!=NULL) && (pn->next->Typ()==INT_CMD))
      {
        float_len2=(int)(long)pn->next->Data();
        pn=pn->next;
      }
    }

    if (!complex_flag)
      complex_flag= pn->next != NULL;
    if( !complex_flag && (float_len2 <= (short)SHORT_REAL_LENGTH))
       cf=nInitChar(n_R, NULL);
    else // longR or longC?
    {
       LongComplexInfo param;

       param.float_len = si_min (float_len, 32767);
       param.float_len2 = si_min (float_len2, 32767);

       // set the parameter name
       if (complex_flag)
       {
         if (param.float_len < SHORT_REAL_LENGTH)
         {
           param.float_len= SHORT_REAL_LENGTH;
           param.float_len2= SHORT_REAL_LENGTH;
         }
         if (pn->next == NULL)
           param.par_name=(const char*)"i"; //default to i
         else
           param.par_name = (const char*)pn->next->name;
       }

       cf = nInitChar(complex_flag ? n_long_C: n_long_R, (void*)&param);
    }
    assume( cf != NULL );
  }
#ifdef HAVE_RINGS
  else if ((pn->name != NULL) && (strcmp(pn->name, "integer") == 0))
  {
    modBase = (int_number) omAlloc(sizeof(mpz_t));
    mpz_init_set_si(modBase, 0);
    if (pn->next!=NULL)
    {
      if (pn->next->Typ()==INT_CMD)
      {
        mpz_set_ui(modBase, (int)(long) pn->next->Data());
        pn=pn->next;
        if ((pn->next!=NULL) && (pn->next->Typ()==INT_CMD))
        {
          modExponent = (long) pn->next->Data();
          pn=pn->next;
        }
        while ((pn->next!=NULL) && (pn->next->Typ()==INT_CMD))
        {
          mpz_mul_ui(modBase, modBase, (int)(long) pn->next->Data());
          pn=pn->next;
        }
      }
      else if (pn->next->Typ()==BIGINT_CMD)
      {
        number p=(number)pn->next->CopyD();
        nlGMP(p,(number)modBase,coeffs_BIGINT);
        nlDelete(&p,coeffs_BIGINT);
      }
    }
    else
      cf=nInitChar(n_Z,NULL);

    if ((mpz_cmp_ui(modBase, 1) == 0) && (mpz_cmp_ui(modBase, 0) < 0))
    {
      Werror("Wrong ground ring specification (module is 1)");
      goto rInitError;
    }
    if (modExponent < 1)
    {
      Werror("Wrong ground ring specification (exponent smaller than 1");
      goto rInitError;
    }
    // module is 0 ---> integers ringtype = 4;
    // we have an exponent
    if (modExponent > 1 && cf == NULL)
    {
      if ((mpz_cmp_ui(modBase, 2) == 0) && (modExponent <= 8*sizeof(NATNUMBER)))
      {
        /* this branch should be active for modExponent = 2..32 resp. 2..64,
           depending on the size of a long on the respective platform */
        //ringtype = 1;       // Use Z/2^ch
        cf=nInitChar(n_Z2m,(void*)(long)modExponent);
        omFreeSize (modBase, sizeof (mpz_t));
      }
      else
      {
        if (mpz_cmp_ui(modBase,0)==0)
        {
          WerrorS("modulus must not be 0 or parameter not allowed");
          goto rInitError;
        }
        //ringtype = 3;
        ZnmInfo info;
        info.base= modBase;
        info.exp= modExponent;
        cf=nInitChar(n_Znm,(void*) &info); //exponent is missing
      }
    }
    // just a module m > 1
    else if (cf == NULL)
    {
      if (mpz_cmp_ui(modBase,0)==0)
      {
        WerrorS("modulus must not be 0 or parameter not allowed");
        goto rInitError;
      }
      //ringtype = 2;
      ZnmInfo info;
      info.base= modBase;
      info.exp= modExponent;
      cf=nInitChar(n_Zn,(void*) &info);
    }
    assume( cf != NULL );
  }
#endif
  // ring NEW = OLD, (), (); where OLD is a polynomial ring...
  else if ((pn->Typ()==RING_CMD) && (P == 1))
  {
    TransExtInfo extParam;
    extParam.r = (ring)pn->Data();
    cf = nInitChar(n_transExt, &extParam);
  }
  else if ((pn->Typ()==QRING_CMD) && (P == 1)) // same for qrings - which should be fields!?
  {
    AlgExtInfo extParam;
    extParam.r = (ring)pn->Data();

    cf = nInitChar(n_algExt, &extParam);   // Q[a]/<minideal>
  }
  else
  {
    Werror("Wrong or unknown ground field specification");
#ifndef SING_NDEBUG
    sleftv* p = pn;
    while (p != NULL)
    {
      Print( "pn[%p]: type: %d [%s]: %p, name: %s", (void*)p, p->Typ(), Tok2Cmdname(p->Typ()), p->Data(), (p->name == NULL? "NULL" : p->name) );
      PrintLn();
      p = p->next;
    }
#endif
    goto rInitError;
  }
//  pn=pn->next;

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
  if (cf==NULL)
  {
    Werror("Invalid ground field specification");
    goto rInitError;
//    const int ch=32003;
//    cf=nInitChar(n_Zp, (void*)(long)ch);
  }

  assume( R != NULL );

  R->cf = cf;

  /* names and number of variables-------------------------------------*/
  {
    int l=rv->listLength();

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
  rRenameVars(R); // conflicting variables will be renamed
  /* ordering -------------------------------------------------------------*/
  if (rSleftvOrdering2Ordering(ord, R))
    goto rInitError;

  // Complete the initialization
  if (rComplete(R,1))
    goto rInitError;

/*#ifdef HAVE_RINGS
// currently, coefficients which are ring elements require a global ordering:
  if (rField_is_Ring(R) && (R->OrdSgn==-1))
  {
    WerrorS("global ordering required for these coefficients");
    goto rInitError;
  }
#endif*/

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
  if  ((R != NULL)&&(R->cf!=NULL)) rDelete(R);
  if (pn != NULL) pn->CleanUp();
  if (rv != NULL) rv->CleanUp();
  if (ord != NULL) ord->CleanUp();
  return NULL;
}

ring rSubring(ring org_ring, sleftv* rv)
{
  ring R = rCopy0(org_ring);
  int *perm=(int *)omAlloc0((org_ring->N+1)*sizeof(int));
  int n = rBlocks(org_ring), i=0, j;

  /* names and number of variables-------------------------------------*/
  {
    int l=rv->listLength();
    if (l>MAX_SHORT)
    {
      Werror("too many ring variables(%d), max is %d",l,MAX_SHORT);
       goto rInitError;
    }
    R->N = l; /*rv->listLength();*/
  }
  omFree(R->names);
  R->names   = (char **)omAlloc0(R->N * sizeof(char_ptr));
  if (rSleftvList2StringArray(rv, R->names))
  {
    WerrorS("name of ring variable expected");
    goto rInitError;
  }

  /* check names for subring in org_ring ------------------------- */
  {
    i=0;

    for(j=0;j<R->N;j++)
    {
      for(;i<org_ring->N;i++)
      {
        if (strcmp(org_ring->names[i],R->names[j])==0)
        {
          perm[i+1]=j+1;
          break;
        }
      }
      if (i>org_ring->N)
      {
        Werror("variable %d (%s) not in basering",j+1,R->names[j]);
        break;
      }
    }
  }
  //Print("perm=");
  //for(i=1;i<org_ring->N;i++) Print("v%d -> v%d\n",i,perm[i]);
  /* ordering -------------------------------------------------------------*/

  for(i=0;i<n;i++)
  {
    int min_var=-1;
    int max_var=-1;
    for(j=R->block0[i];j<=R->block1[i];j++)
    {
      if (perm[j]>0)
      {
        if (min_var==-1) min_var=perm[j];
        max_var=perm[j];
      }
    }
    if (min_var!=-1)
    {
      //Print("block %d: old %d..%d, now:%d..%d\n",
      //      i,R->block0[i],R->block1[i],min_var,max_var);
      R->block0[i]=min_var;
      R->block1[i]=max_var;
      if (R->wvhdl[i]!=NULL)
      {
        omFree(R->wvhdl[i]);
        R->wvhdl[i]=(int*)omAlloc0((max_var-min_var+1)*sizeof(int));
        for(j=org_ring->block0[i];j<=org_ring->block1[i];j++)
        {
          if (perm[j]>0)
          {
            R->wvhdl[i][perm[j]-R->block0[i]]=
                org_ring->wvhdl[i][j-org_ring->block0[i]];
            //Print("w%d=%d (orig_w%d)\n",perm[j],R->wvhdl[i][perm[j]-R->block0[i]],j);
          }
        }
      }
    }
    else
    {
      if(R->block0[i]>0)
      {
        //Print("skip block %d\n",i);
        R->order[i]=ringorder_unspec;
        if (R->wvhdl[i] !=NULL) omFree(R->wvhdl[i]);
        R->wvhdl[i]=NULL;
      }
      //else Print("keep block %d\n",i);
    }
  }
  i=n-1;
  while(i>0)
  {
    // removed unneded blocks
    if(R->order[i-1]==ringorder_unspec)
    {
      for(j=i;j<=n;j++)
      {
        R->order[j-1]=R->order[j];
        R->block0[j-1]=R->block0[j];
        R->block1[j-1]=R->block1[j];
        if (R->wvhdl[j-1] !=NULL) omFree(R->wvhdl[j-1]);
        R->wvhdl[j-1]=R->wvhdl[j];
      }
      R->order[n]=ringorder_unspec;
      n--;
    }
    i--;
  }
  n=rBlocks(org_ring)-1;
  while (R->order[n]==0)  n--;
  while (R->order[n]==ringorder_unspec)  n--;
  if ((R->order[n]==ringorder_c) ||  (R->order[n]==ringorder_C)) n--;
  if (R->block1[n] != R->N)
  {
    if (((R->order[n]==ringorder_dp) ||
         (R->order[n]==ringorder_ds) ||
         (R->order[n]==ringorder_Dp) ||
         (R->order[n]==ringorder_Ds) ||
         (R->order[n]==ringorder_rp) ||
         (R->order[n]==ringorder_rs) ||
         (R->order[n]==ringorder_lp) ||
         (R->order[n]==ringorder_ls))
        &&
        R->block0[n] <= R->N)
    {
      R->block1[n] = R->N;
    }
    else
    {
      Werror("mismatch of number of vars (%d) and ordering (%d vars) in block %d",
             R->N,R->block1[n],n);
      return NULL;
    }
  }
  omFree(perm);
  // find OrdSgn:
  R->OrdSgn = org_ring->OrdSgn; // IMPROVE!
  //for(i=1;i<=R->N;i++)
  //{ if (weights[i]<0) { R->OrdSgn=-1;break; }}
  //omFree(weights);
  // Complete the initialization
  if (rComplete(R,1))
    goto rInitError;

  rTest(R);

  if (rv != NULL) rv->CleanUp();

  return R;

  // error case:
  rInitError:
  if  (R != NULL) rDelete(R);
  if (rv != NULL) rv->CleanUp();
  return NULL;
}

void rKill(ring r)
{
  if ((r->ref<=0)&&(r->order!=NULL))
  {
#ifdef RDEBUG
    if (traceit &TRACE_SHOW_RINGS) Print("kill ring %lx\n",(long)r);
#endif
    if (r->qideal!=NULL)
    {
      id_Delete(&r->qideal, r);
      r->qideal = NULL;
    }
    int j;
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
// any variables depending on r ?
    while (r->idroot!=NULL)
    {
      killhdl2(r->idroot,&(r->idroot),r);
    }
    if (r==currRing)
    {
      // all dependend stuff is done, clean global vars:
      if (r->qideal!=NULL)
      {
        currQuotient=NULL;
      }
      if ((currRing->ppNoether)!=NULL) pDelete(&(currRing->ppNoether));
      if (sLastPrinted.RingDependend())
      {
        sLastPrinted.CleanUp();
      }
      //if ((myynest>0) && (iiRETURNEXPR.RingDependend()))
      //{
      //  WerrorS("return value depends on local ring variable (export missing ?)");
      //  iiRETURNEXPR.CleanUp();
      //}
      currRing=NULL;
      currRingHdl=NULL;
    }

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

idhdl rSimpleFindHdl(ring r, idhdl root, idhdl n)
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

extern BOOLEAN jjPROC(leftv res, leftv u, leftv v);
ideal kGroebner(ideal F, ideal Q)
{
  //test|=Sy_bit(OPT_PROT);
  idhdl save_ringhdl=currRingHdl;
  ideal resid;
  idhdl new_ring=NULL;
  if ((currRingHdl==NULL) || (IDRING(currRingHdl)!=currRing))
  {
    currRingHdl=enterid(omStrDup(" GROEBNERring"),0,RING_CMD,&IDROOT,FALSE);
    new_ring=currRingHdl;
    IDRING(currRingHdl)=currRing;
  }
  sleftv v; memset(&v,0,sizeof(v)); v.rtyp=IDEAL_CMD; v.data=(char *) F;
  idhdl h=ggetid("groebner");
  sleftv u; memset(&u,0,sizeof(u)); u.rtyp=IDHDL; u.data=(char *) h;
            u.name=IDID(h);

  sleftv res; memset(&res,0,sizeof(res));
  if(jjPROC(&res,&u,&v))
  {
    resid=kStd(F,Q,testHomog,NULL);
  }
  else
  {
    //printf("typ:%d\n",res.rtyp);
    resid=(ideal)(res.data);
  }
  // cleanup GROEBNERring, save_ringhdl, u,v,(res )
  if (new_ring!=NULL)
  {
    idhdl h=IDROOT;
    if (h==new_ring) IDROOT=h->next;
    else
    {
      while ((h!=NULL) &&(h->next!=new_ring)) h=h->next;
      if (h!=NULL) h->next=h->next->next;
    }
    if (h!=NULL) omFreeSize(h,sizeof(*h));
  }
  currRingHdl=save_ringhdl;
  u.CleanUp();
  v.CleanUp();
  return resid;
}

static void jjINT_S_TO_ID(int n,int *e, leftv res)
{
  if (n==0) n=1;
  ideal l=idInit(n,1);
  int i;
  poly p;
  for(i=rVar(currRing);i>0;i--)
  {
    if (e[i]>0)
    {
      n--;
      p=pOne();
      pSetExp(p,i,1);
      pSetm(p);
      l->m[n]=p;
      if (n==0) break;
    }
  }
  res->data=(char*)l;
  setFlag(res,FLAG_STD);
  omFreeSize((ADDRESS)e,(rVar(currRing)+1)*sizeof(int));
}
BOOLEAN jjVARIABLES_P(leftv res, leftv u)
{
  int *e=(int *)omAlloc0((rVar(currRing)+1)*sizeof(int));
  int n=pGetVariables((poly)u->Data(),e);
  jjINT_S_TO_ID(n,e,res);
  return FALSE;
}

BOOLEAN jjVARIABLES_ID(leftv res, leftv u)
{
  int *e=(int *)omAlloc0((rVar(currRing)+1)*sizeof(int));
  ideal I=(ideal)u->Data();
  int i;
  int n=0;
  for(i=I->nrows*I->ncols-1;i>=0;i--)
  {
    int n0=pGetVariables(I->m[i],e);
    if (n0>n) n=n0;
  }
  jjINT_S_TO_ID(n,e,res);
  return FALSE;
}

void paPrint(const char *n,package p)
{
  Print(" %s (",n);
  switch (p->language)
  {
    case LANG_SINGULAR: PrintS("S"); break;
    case LANG_C:        PrintS("C"); break;
    case LANG_TOP:      PrintS("T"); break;
    case LANG_NONE:     PrintS("N"); break;
    default:            PrintS("U");
  }
  if(p->libname!=NULL)
  Print(",%s", p->libname);
  PrintS(")");
}

BOOLEAN iiApplyINTVEC(leftv res, leftv a, int op, leftv proc)
{
  intvec *aa=(intvec*)a->Data();
  intvec *r=ivCopy(aa);
  sleftv tmp_out;
  sleftv tmp_in;
  BOOLEAN bo=FALSE;
  for(int i=0;i<aa->length(); i++)
  {
    memset(&tmp_in,0,sizeof(tmp_in));
    tmp_in.rtyp=INT_CMD;
    tmp_in.data=(void*)(long)(*aa)[i];
    if (proc==NULL)
      bo=iiExprArith1(&tmp_out,&tmp_in,op);
    else
      bo=jjPROC(&tmp_out,proc,&tmp_in);
    if (bo || (tmp_out.rtyp!=INT_CMD))
    {
      if (r!=NULL) delete r;
      Werror("apply fails at index %d",i+1);
      return TRUE;
    }
    (*r)[i]=(int)(long)tmp_out.data;
  }
  res->data=(void*)r;
  return FALSE;
}
BOOLEAN iiApplyBIGINTMAT(leftv res, leftv a, int op, leftv proc)
{
  WerrorS("not implemented");
  return TRUE;
}
BOOLEAN iiApplyIDEAL(leftv res, leftv a, int op, leftv proc)
{
  WerrorS("not implemented");
  return TRUE;
}
BOOLEAN iiApplyLIST(leftv res, leftv a, int op, leftv proc)
{
  lists aa=(lists)a->Data();
  lists r=(lists)omAlloc0Bin(slists_bin); r->Init(aa->nr+1);
  sleftv tmp_out;
  sleftv tmp_in;
  BOOLEAN bo=FALSE;
  for(int i=0;i<=aa->nr; i++)
  {
    memset(&tmp_in,0,sizeof(tmp_in));
    tmp_in.Copy(&(aa->m[i]));
    if (proc==NULL)
      bo=iiExprArith1(&tmp_out,&tmp_in,op);
    else
      bo=jjPROC(&tmp_out,proc,&tmp_in);
    tmp_in.CleanUp();
    if (bo)
    {
      if (r!=NULL) r->Clean();
      Werror("apply fails at index %d",i+1);
      return TRUE;
    }
    memcpy(&(r->m[i]),&tmp_out,sizeof(sleftv));
  }
  res->data=(void*)r;
  return FALSE;
}
BOOLEAN iiApply(leftv res, leftv a, int op, leftv proc)
{
  memset(res,0,sizeof(sleftv));
  res->rtyp=a->Typ();
  switch (res->rtyp /*a->Typ()*/)
  {
    case INTVEC_CMD:
    case INTMAT_CMD:
        return iiApplyINTVEC(res,a,op,proc);
    case BIGINTMAT_CMD:
        return iiApplyBIGINTMAT(res,a,op,proc);
    case IDEAL_CMD:
    case MODUL_CMD:
    case MATRIX_CMD:
        return iiApplyIDEAL(res,a,op,proc);
    case LIST_CMD:
        return iiApplyLIST(res,a,op,proc);
  }
  WerrorS("first argument to `apply` must allow an index");
  return TRUE;
}

BOOLEAN iiTestAssume(leftv a, leftv b)
{
  // assume a: level
  if ((a->Typ()==INT_CMD)&&((long)a->Data()>=0))
  {
    char       assume_yylinebuf[80];
    strncpy(assume_yylinebuf,my_yylinebuf,79);
    int lev=(long)a->Data();
    int startlev=0;
    idhdl h=ggetid("assumeLevel");
    if ((h!=NULL)&&(IDTYP(h)==INT_CMD)) startlev=(long)IDINT(h);
    if(lev <=startlev)
    {
      BOOLEAN bo=b->Eval();
      if (bo) { WerrorS("syntax error in ASSUME");return TRUE;}
      if (b->Typ()!=INT_CMD) { WerrorS("ASUMME(<level>,<int expr>)");return TRUE; }
      if (b->Data()==NULL) { Werror("ASSUME failed:%s",assume_yylinebuf);return TRUE;}
    }
  }
  else
     b->CleanUp();
  a->CleanUp();
  return FALSE;
}
