/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/* $Id: ipshell.cc,v 1.12 1997-07-04 14:14:51 Singular Exp $ */
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
#include "mmemory.h"
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
#include "ipshell.h"

leftv iiCurrArgs=NULL;
int  traceit = 0;
char *lastreserved=NULL;

#ifdef SIC
int  myynest = 0;
#else
int  myynest = -1;
#endif

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

static void list1(char* s, idhdl h,BOOLEAN c)
{
  char buffer[22];
  int l;

  Print("%s%-20.20s [%d]  ",s,IDID(h),IDLEV(h));
  if (h == currRingHdl) PrintS("*");
  PrintS(Tok2Cmdname((int)IDTYP(h)));

  ipListFlag(h);
  switch(IDTYP(h))
  {
    case INT_CMD:     Print(" %d",IDINT(h)); break;
    case INTVEC_CMD:  Print(" (%d)",IDINTVEC(h)->length()); break;
    case INTMAT_CMD:  Print(" %d x %d",IDINTVEC(h)->rows(),IDINTVEC(h)->cols());
                   break;
    case POLY_CMD:
    case VECTOR_CMD:  if (c)
                   {
                     PrintS(" ");wrp(IDPOLY(h));
                     if(IDPOLY(h) != NULL)
                     {
                       Print(", %d monomial(s)",pLength(IDPOLY(h)));
                     }
                   }
                   break;
    case MODUL_CMD:   Print(", rk %d", IDIDEAL(h)->rank);
    case IDEAL_CMD:   Print(", %u generator(s)",
                     IDELEMS(IDIDEAL(h)),IDIDEAL(h)->rank); break;
    case MAP_CMD:
                   Print(" from %s",IDMAP(h)->preimage); break;
    case MATRIX_CMD: Print(" %u x %u"
                        ,MATROWS(IDMATRIX(h))
                        ,MATCOLS(IDMATRIX(h))
                        );
                   break;
    case PROC_CMD:    break;
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
    case LIST_CMD:   Print(", size: %d",IDLIST(h)->nr+1);
                   break;
    case QRING_CMD:
    case RING_CMD:
#ifdef RDEBUG
                   if (traceit &TRACE_SHOW_RINGS) Print(" <%d>",IDRING(h)->no);
#endif
                   break;
    /*default:     break;*/
  }
  PrintLn();
}

void type_cmd(idhdl h)
{
  int saveShortOut=pShortOut;
  pShortOut=1;
  list1("// ",h,FALSE);
  if (IDTYP(h)!=INT_CMD)
  {
    sleftv expr;
    memset(&expr,0,sizeof(expr));
    expr.rtyp=IDHDL;
    expr.name=IDID(h);
    expr.data=(void *)h;
    expr.Print();
  }
  pShortOut=saveShortOut;
}

void killlocals0(int v, idhdl * localhdl)
{
  idhdl h = *localhdl;
  while (h!=NULL)
  {
    int vv;
    if ((vv=IDLEV(h))>0)
    {
      if (vv < v)
      {
        if (iiNoKeepRing) return;
        h = IDNEXT(h);
      }
      else if (vv >= v)
      {
        idhdl nexth = IDNEXT(h);
        //Print("kill %s, lev: %d\n",IDID(h),IDLEV(h));
        killhdl(h,localhdl);
        h = nexth;
      }
    }
    else
      h = IDNEXT(h);
  }
}

void killlocals(int v)
{
  killlocals0(v,&idroot);

  idhdl h = idroot;
  idhdl sh=currRingHdl;
  BOOLEAN changed=FALSE;

  while (h!=NULL)
  {
    if (((IDTYP(h)==QRING_CMD) || (IDTYP(h) == RING_CMD))
    && (IDRING(h)->idroot!=NULL))
    {
      if (h!=currRingHdl) {changed=TRUE;rSetHdl(h,FALSE);}
      killlocals0(v,&(IDRING(h)->idroot));
    }
    h = IDNEXT(h);
  }
  if (changed)
  {
    currRing=NULL;
    currRingHdl=NULL;
    rSetHdl(sh,TRUE);
  }
  iiNoKeepRing=TRUE;
}

void list_cmd(int typ, const char* what, char *prefix,BOOLEAN iterate)
{
  idhdl h,start;
  BOOLEAN all = typ==-1;
  BOOLEAN really_all=FALSE;

  if ( typ==0 )
  {
    if (strcmp(what,"all")==0)
    {
      really_all=TRUE;
      h=idroot;
    }
    else
    {
      h = ggetid(what);
      if (h!=NULL)
      {
        if (iterate) list1(prefix,h,TRUE);
        if ((IDTYP(h)==RING_CMD)
        || (IDTYP(h)==QRING_CMD)
        || (IDTYP(h)==PACKAGE_CMD))
        {
          h=IDRING(h)->idroot;
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
  else if ((typ>BEGIN_RING) && (typ<END_RING))
  {
    h = currRing->idroot;
  }
  else
    h = idroot;
  start=h;
  while (h!=NULL)
  {
    if ((all && (IDTYP(h)!=PROC_CMD)) || (typ == IDTYP(h))
    || ((IDTYP(h)==QRING_CMD) && (typ==RING_CMD)))
    {
      list1(prefix,h,start==currRingHdl);
      if ((((IDTYP(h)==RING_CMD)||(IDTYP(h)==QRING_CMD))
        && (really_all || (all && (h==currRingHdl)))
        && ((IDLEV(h)==0)||(IDLEV(h)==myynest)))
      ||(IDTYP(h)==PACKAGE_CMD))
      {
        list_cmd(0,IDID(h),"//      ",FALSE);
      }
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
        Warn("use the option command");
        test &= ~Sy_bit(ii);
      }
      else if (Sy_bit(ii) & validOpts)
        test &= ~Sy_bit(ii);
    }
    else if (i<32)
    {
      if (Sy_bit(ii) & kOptions)
      {
        Warn("use the option command");
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
  char *s=StringSet("");

  for (i=0; i<ii; i++)
  {
    for (j=0; j<jj; j++)
    {
      pString0(*pp++);
      s=StringAppend("%c\n",ch);
    }
  }
  s[strlen(s)-2]='\0';
  return s;
}

int IsPrime(int p)  /* brute force !!!! */
{
  int i,j;
  if      (p == 0) return 0;
  else if (p == 1) return 1/*1*/;
  else if (p == 2) return p;
  else if (p <  0) return (-IsPrime(-p));
  else if (!(p & 1)) return IsPrime(p-1);
  for (j=p/2+1,i=3; i<p; i+=2)
  {
    if ((p%i) == 0) return IsPrime(p-2);
    if (j < i) return p;
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

  r=idroot->get(theMap->preimage,myynest);
  if ((r!=NULL) && ((r->typ == RING_CMD) || (r->typ== QRING_CMD)))
  {
    if (!nSetMap(IDRING(r)->ch,IDRING(r)->parameter,IDRING(r)->P, IDRING(r)->minpoly))
    {
      Werror("map from characteristic %d to %d not implemented",
        IDRING(r)->ch,currRing->ch);
      return NULL;
    }
    if (IDELEMS(theMap)<IDRING(r)->N)
    {
      theMap->m=(polyset)ReAlloc((ADDRESS)theMap->m,IDELEMS(theMap)*sizeof(poly),
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
      v=(leftv)Alloc0(sizeof(*v));
      sleftv tmpW;
      memset(&tmpW,0,sizeof(sleftv));
      tmpW.rtyp=IDTYP(w);
      tmpW.data=IDDATA(w);
      if (maApplyFetch(MAP_CMD,theMap,v,&tmpW,NULL,IDRING(r)->N,NULL,0))
      {
        Werror("cannot map %s(%d)",Tok2Cmdname(w->typ),w->typ);
        Free((ADDRESS)v,sizeof(*v));
        return NULL;
      }
      return v;
    }
    else
      Werror("%s undefined in %s",what,theMap->preimage);
  }
  else
    Werror("cannot find preimage %s",theMap->preimage);
  return NULL;
}

#ifdef OLD_RES
void  iiMakeResolv(resolvente r, int length, int rlen, char * name, int typ0,
                   intvec ** weights)
{
  lists L=liMakeResolv(r,length,rlen,typ0,weights);
  int i=0;
  idhdl h;
  char * s=(char *)Alloc(strlen(name)+5);

  while (i<=L->nr)
  {
    sprintf(s,"%s(%d)",name,i+1);
    if (i==0)
      h=enterid(mstrdup(s),myynest,typ0,&(currRing->idroot), FALSE);
    else
      h=enterid(mstrdup(s),myynest,MODUL_CMD,&(currRing->idroot), FALSE);
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
  Free((ADDRESS)L->m,(L->nr+1)*sizeof(sleftv));
  Free((ADDRESS)L,sizeof(slists));
  Free((ADDRESS)s,strlen(name)+5);
}
#endif

//resolvente iiFindRes(char * name, int * len, int *typ0)
//{
//  char *s=(char *)Alloc(strlen(name)+5);
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
//    Free((ADDRESS)s,strlen(name)+5);
//    return NULL;
//  }
//  r=(ideal *)Alloc(/*(len+1)*/ i*sizeof(ideal));
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
//        Free((ADDRESS)r,(*len)*sizeof(ideal));
//        Free((ADDRESS)s,strlen(name)+5);
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
//  Free((ADDRESS)s,strlen(name)+5);
//  return r;
//}

resolvente iiCopyRes(resolvente r, int l)
{
  int i;
  resolvente res=(ideal *)Alloc0((l+1)*sizeof(ideal));

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
  Free((ADDRESS)rr,len*sizeof(ideal));
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
  Free((ADDRESS)r,(len)*sizeof(ideal));
  return FALSE;
}

int iiRegularity(lists L)
{
  resolvente r;
  int len,reg,typ0;
  intvec * dummy;

  r=liFindRes(L,&len,&typ0);
  if (r==NULL) return -1;
  dummy=syBetti(r,len,&reg);
  Free((ADDRESS)r,len*sizeof(ideal));
  delete dummy;
  return reg-1;
}

BOOLEAN iiDebugMarker=TRUE;
void iiDebug()
{
  Print("\n-- break point in %s --\n",VoiceName());
  int i=voice-1;
  if (iiDebugMarker)
  {
    for(;i>=0;i--)
    {
      Print("-- %d: called from %s --\n",i,VoiceName(i));
    }
  }
  char * s;
  iiDebugMarker=FALSE;
  s = (char *)AllocL(84);
  fe_fgets_stdin(s,80);
  if (*s=='\n')
  {
    iiDebugMarker=TRUE;
  }
  else
  {
    strcat( s, "\n;~\n");
    newBuffer(s,BT_execute);
  }
}

int iiDeclCommand(leftv sy, leftv name, int lev,int t, idhdl* root,BOOLEAN init_b)
{
  BOOLEAN res=FALSE;
  memset(sy,0,sizeof(sleftv));
  if ((name->name==NULL)||(isdigit(name->name[0])))
  {
    WerrorS("object to declare is not a name");
    res=TRUE;
  }
  else
  {
    #ifdef SIC
    if (root==NULL)
    {
      idhdl h=rDefault(mstrdup("? default_ring"));
      ring r=IDRING(h);
      char **n=r->names;
      int a=r->N;
      while ((*n!=NULL)&&(a>0)) { *n[0]='?'; n++;a--; }
      root=&(IDRING(h)->idroot);
    }
    //egPrint(DEFINE_VAR,t,"def %s type=%d,lev=%d\n",name->name,t,lev);
    //egPrint(name);
    #endif
    sy->data = (char *)enterid(name->name,lev,t,root,init_b);
    if (sy->data!=NULL)
    {
      sy->rtyp=IDHDL;
      currid=sy->name=IDID((idhdl)sy->data);
      name->name=NULL; /* used in enterid */
      //sy->e = NULL;
      if (name->next!=NULL)
      {
        sy->next=(leftv)Alloc(sizeof(sleftv));
        res=iiDeclCommand(sy->next,name->next,lev,t,root);
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
  Free((ADDRESS)h,sizeof(sleftv));
  return res;
}

BOOLEAN iiExport (leftv v, int toLev)
{
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
      idhdl h=(idhdl)v->data;
      if (IDLEV(h)==0) Warn("`%s` is already global",IDID(h));
      else
      {
        h=idroot->get(v->name,toLev);
        idhdl *root=&idroot;
        if ((h==NULL)&&(currRing!=NULL))
        {
          h=currRing->idroot->get(v->name,toLev);
          root=&currRing->idroot;
        }
        if ((h!=NULL)&&(IDLEV(h)==toLev))
        {
          if (IDTYP(h)==v->Typ())
          {
            Warn("redefining %s",IDID(h));
            if (iiLocalRing[0]==IDRING(h)) iiLocalRing[0]=NULL;
            killhdl(h,root);
          }
          else
          {
            r->CleanUp();
            return TRUE;
          }
        }
        h=(idhdl)v->data;
        IDLEV(h)=toLev;
        iiNoKeepRing=FALSE;
      }
    }
    v=v->next;
  }
  r->CleanUp();
  return nok;
}

/*assume root!=idroot*/
BOOLEAN iiExport (leftv v, int toLev, idhdl &root)
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
          Warn("redefining %s",IDID(old));
          killhdl(old,&root);
        }
        else
        {
          rv->CleanUp();
          return TRUE;
        }
      }
      idhdl h=(idhdl)v->data;
      if (h==idroot)
      {
        idroot=h->next;
      }
      else
      {
        idhdl hh=idroot;
        while ((hh->next!=h)&&(hh->next!=NULL))
          hh=hh->next;
        if (hh->next==h)
          hh->next=h->next;
        else
          break;
      }
      h->next=root;
      root=h;
      IDLEV(h)=toLev;
    }
    v=v->next;
  }
  rv->CleanUp();
  return nok;
}

BOOLEAN iiCheckRing(int i)
{
  #ifndef SIC
  if (currRingHdl==NULL)
  {
    #ifdef SIQ
    if (siq<=0)
    {
    #endif
      if ((i>BEGIN_RING) && (i<END_RING))
      {
        WerrorS("no ring active");
        return TRUE;
      }
    #ifdef SIQ
    }
    #endif
  }
  #endif
  return FALSE;
}
