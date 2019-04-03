/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/

/*
* ABSTRACT: identfier handling
*/





#include "kernel/mod2.h"

#include "misc/options.h"
#include "misc/intvec.h"

#include "coeffs/numbers.h"
#include "coeffs/bigintmat.h"

#include "polys/matpol.h"
#include "polys/monomials/ring.h"

#include "kernel/polys.h"
#include "kernel/ideals.h"
#include "kernel/GBEngine/syz.h"

#include "Singular/tok.h"
#include "Singular/ipshell.h"
#include "Singular/fevoices.h"
#include "Singular/lists.h"
#include "Singular/attrib.h"
#include "Singular/links/silink.h"
#include "Singular/ipid.h"
#include "Singular/blackbox.h"
#include "Singular/number2.h"

#ifdef SINGULAR_4_2
#include "Singular/number2.h"
#endif
#ifdef HAVE_DYNAMIC_LOADING
#include "polys/mod_raw.h"
#endif /* HAVE_DYNAMIC_LOADING */

VAR omBin sip_command_bin = omGetSpecBin(sizeof(sip_command));
VAR omBin sip_package_bin = omGetSpecBin(sizeof(sip_package));
//omBin ip_package_bin = omGetSpecBin(sizeof(ip_package));
VAR omBin idrec_bin = omGetSpecBin(sizeof(idrec));

VAR coeffs coeffs_BIGINT;

VAR proclevel *procstack=NULL;
//idhdl idroot = NULL;

VAR idhdl currPackHdl = NULL;
VAR idhdl basePackHdl = NULL;
VAR package currPack = NULL;
VAR package basePack = NULL;
VAR idhdl currRingHdl = NULL;

void paCleanUp(package pack);

/*0 implementation*/

static inline long iiS2I(const char *s)
{
  long l;
  strncpy((char*)&l,s,SIZEOF_LONG);
  return l;
}

idhdl idrec::get(const char * s, int level)
{
  assume(s!=NULL);
  assume((level>=0) && (level<=1000)); //not really, but if it isnt in that bounds..
  idhdl h = this;
  idhdl found=NULL;
  int l;
  const char *id_;
  unsigned long i=iiS2I(s);
  int less4=(i < (1L<<((SIZEOF_LONG-1)*8)));
  while (h!=NULL)
  {
    omCheckAddr((ADDRESS)IDID(h));
    l=IDLEV(h);
    if ((l==0)||(l==level))
    {
      if (i==h->id_i)
      {
        id_=IDID(h);
        if (less4 || (0 == strcmp(s+SIZEOF_LONG,id_+SIZEOF_LONG)))
        {
          if(l==level) return h;
          found=h;
        }
      }
    }
    h = IDNEXT(h);
  }
  return found;
}

idhdl idrec::get_level(const char * s, int level)
{
  assume(s!=NULL);
  assume((level>=0) && (level<=1000)); //not really, but if it isnt in that bounds..
  idhdl h = this;
  int l;
  const char *id_;
  unsigned long i=iiS2I(s);
  int less4=(i < (1L<<((SIZEOF_LONG-1)*8)));
  while (h!=NULL)
  {
    omCheckAddr((ADDRESS)IDID(h));
    l=IDLEV(h);
    if ((l==level)&&(i==h->id_i))
    {
      id_=IDID(h);
      if (less4 || (0 == strcmp(s+SIZEOF_LONG,id_+SIZEOF_LONG)))
      {
        return h;
      }
    }
    h = IDNEXT(h);
  }
  return NULL;
}

//idrec::~idrec()
//{
//  if (id!=NULL)
//  {
//    omFree((ADDRESS)id);
//    id=NULL;
//  }
//  /* much more !! */
//}

void *idrecDataInit(int t)
{
  switch (t)
  {
    //the type with init routines:
#ifdef SINGULAR_4_2
    case CNUMBER_CMD:
      return (void*)n2Init(0,NULL);
    case CPOLY_CMD:
      return (void*)p2Init(0,NULL);
    case CMATRIX_CMD:
#endif
    case BIGINTMAT_CMD:
      return (void *)new bigintmat();
    case BUCKET_CMD:
      if (currRing!=NULL)
      return (void*)sBucketCreate(currRing);
      else
      {
        WerrorS("need basering for polyBucket");
        return NULL;
      }
    case INTVEC_CMD:
    case INTMAT_CMD:
      return (void *)new intvec();
    case NUMBER_CMD:
    {
      if (currRing!=NULL) return (void *) nInit(0);
      else                return NULL;
    }
    case BIGINT_CMD:
      return (void *) n_Init(0, coeffs_BIGINT);
    case IDEAL_CMD:
    case MODUL_CMD:
    case MATRIX_CMD:
    case SMATRIX_CMD:
      return (void*) idInit(1,1);
    case MAP_CMD:
    {
      map m = (map)idInit(1,1);
      m->preimage = omStrDup(IDID(currRingHdl));
      return (void *)m;
    }
    case STRING_CMD:
      return (void *)omAlloc0(1);
    case LIST_CMD:
    {
      lists l=(lists)omAllocBin(slists_bin);
      l->Init();
      return (void*)l;
    }
    //the types with the standard init: set the struct to zero
    case LINK_CMD:
      return (void*) omAlloc0Bin(sip_link_bin);
    case RING_CMD:
      return NULL;
    case PACKAGE_CMD:
    {
      package pa=(package)omAlloc0Bin(sip_package_bin);
      pa->language=LANG_NONE;
      pa->loaded = FALSE;
      return (void*)pa;
    }
    case PROC_CMD:
    {
      procinfov pi=(procinfov)omAlloc0Bin(procinfo_bin);
      pi->ref=1;
      pi->language=LANG_NONE;
      return (void*)pi;
    }
    case RESOLUTION_CMD:
      return  (void *)omAlloc0(sizeof(ssyStrategy));
    //other types: without init (int,script,poly,def,package)
    case CRING_CMD:
    case INT_CMD:
    case DEF_CMD:
    case POLY_CMD:
    case VECTOR_CMD:
    case QRING_CMD:
       return (void*)0L;
    default:
      {
        if (t>MAX_TOK)
        {
#ifdef BLACKBOX_DEVEL
          Print("bb-type %d\n",t);
#endif
          blackbox *bb=getBlackboxStuff(t);
          if (bb!=NULL)
             return (void *)bb->blackbox_Init(bb);
        }
        else
          Werror("unknown type in idrecDataInit:%d",t);
        break;
      }
  }
  return (void *)0L;
}
idhdl idrec::set(const char * s, int level, int t, BOOLEAN init)
{
  //printf("define %s, %x, level: %d, typ: %d\n", s,s,level,t);
  idhdl h = (idrec *)omAlloc0Bin(idrec_bin);
  IDID(h)   = s;
  IDTYP(h)  = t;
  IDLEV(h)  = level;
  IDNEXT(h) = this;
  BOOLEAN at_start=(this==IDROOT);
  h->id_i=iiS2I(s);
  if (t==BUCKET_CMD) WarnS("defining polyBucket");
  if (init)
  {
    if ((t==IDEAL_CMD)||(t==MODUL_CMD))
      IDFLAG(h) = Sy_bit(FLAG_STD);
    IDSTRING(h)=(char *)idrecDataInit(t);
    // additional settings:--------------------------------------
#if 0
    // this leads to a memory leak
    if (t == QRING_CMD)
    {
      // IDRING(h)=rCopy(currRing);
      /* QRING_CMD is ring dep => currRing !=NULL */
    }
#endif
  }
  // --------------------------------------------------------
  if (at_start)
    IDNEXT(h) = IDROOT;
  return  h;
}

char * idrec::String(BOOLEAN typed)
{
  sleftv tmp;
  memset(&tmp,0,sizeof(sleftv));
  tmp.rtyp=IDTYP(this);
  tmp.data=IDDATA(this);
  tmp.name=IDID(this);
  return tmp.String(NULL, typed);
}

idhdl enterid(const char * s, int lev, int t, idhdl* root, BOOLEAN init, BOOLEAN search)
{
  if (s==NULL) return NULL;
  if (root==NULL) return NULL;
  idhdl h;
  s=omStrDup(s);
  // idhdl *save_root=root;
  if (t==PACKAGE_CMD)
  {
    if (root!=&(basePack->idroot))
    {
      root=&(basePack->idroot);
    }
  }
  // is it already defined in root ?
  if ((h=(*root)->get_level(s,lev))!=NULL)
  {
    if ((IDTYP(h) == t)||(t==DEF_CMD))
    {
      if (IDTYP(h)==PACKAGE_CMD)
      {
        if (strcmp(s,"Top")==0)
        {
          goto errlabel;
        }
        else return h;
      }
      else
      {
        if (BVERBOSE(V_REDEFINE))
        {
          const char *f=VoiceName();
          if (strcmp(f,"STDIN")==0)
            Warn("redefining %s (%s)",s,my_yylinebuf);
          else
            Warn("redefining %s (%s) %s:%d",s,my_yylinebuf,f, yylineno);
        }
        if (s==IDID(h)) IDID(h)=NULL;
        killhdl2(h,root,currRing);
      }
    }
    else
      goto errlabel;
  }
  // is it already defined in currRing->idroot ?
  else if (search && (currRing!=NULL)&&((*root) != currRing->idroot))
  {
    if ((h=currRing->idroot->get_level(s,lev))!=NULL)
    {
      if ((IDTYP(h) == t)||(t==DEF_CMD))
      {
        if (BVERBOSE(V_REDEFINE))
        {
          const char *f=VoiceName();
          if (strcmp(f,"STDIN")==0)
            Warn("redefining %s (%s)",s,my_yylinebuf);
          else
            Warn("redefining %s (%s) %s:%d",s,my_yylinebuf,f, yylineno);
        }
        if (s==IDID(h)) IDID(h)=NULL;
        killhdl2(h,&currRing->idroot,currRing);
      }
      else
        goto errlabel;
    }
  }
  // is it already defined in idroot ?
  else if (search && (*root != IDROOT))
  {
    if ((h=IDROOT->get_level(s,lev))!=NULL)
    {
      if ((IDTYP(h) == t)||(t==DEF_CMD))
      {
        if (BVERBOSE(V_REDEFINE))
        {
          const char *f=VoiceName();
          if (strcmp(f,"STDIN")==0)
            Warn("redefining %s (%s)",s,my_yylinebuf);
          else
            Warn("redefining %s (%s) %s:%d",s,my_yylinebuf,f, yylineno);
        }
        if (s==IDID(h)) IDID(h)=NULL;
        killhdl2(h,&IDROOT,NULL);
      }
      else
        goto errlabel;
    }
  }
  *root = (*root)->set(s, lev, t, init);
#ifndef SING_NDEBUG
  checkall();
#endif
  return *root;

  errlabel:
    //Werror("identifier `%s` in use(lev h=%d,typ=%d,t=%d, curr=%d)",s,IDLEV(h),IDTYP(h),t,lev);
    Werror("identifier `%s` in use",s);
    //listall();
    omFree((ADDRESS)s);
    return NULL;
}
void killid(const char * id, idhdl * ih)
{
  if (id!=NULL)
  {
    idhdl h = (*ih)->get(id,myynest);

    // id not found in global list, is it defined in current ring ?
    if (h==NULL)
    {
      if ((currRing!=NULL) && (*ih != (currRing->idroot)))
      {
        h = currRing->idroot->get(id,myynest);
        if (h!=NULL)
        {
          killhdl2(h,&(currRing->idroot),currRing);
          return;
        }
      }
      Werror("`%s` is not defined",id);
      return;
    }
    killhdl2(h,ih,currRing);
  }
  else
    WerrorS("kill what ?");
}

void killhdl(idhdl h, package proot)
{
  int t=IDTYP(h);
  if (((BEGIN_RING<t) && (t<END_RING))
  || ((t==LIST_CMD) && (lRingDependend((lists)IDDATA(h)))))
    killhdl2(h,&currRing->idroot,currRing);
  else
  {
    if(t==PACKAGE_CMD)
    {
      killhdl2(h,&(basePack->idroot),NULL);
    }
    else
    {
      idhdl s=proot->idroot;
      while ((s!=h) && (s!=NULL)) s=s->next;
      if (s!=NULL)
        killhdl2(h,&(proot->idroot),NULL);
      else if (basePack!=proot)
      {
        idhdl s=basePack->idroot;
        while ((s!=h) && (s!=NULL)) s=s->next;
        if (s!=NULL)
          killhdl2(h,&(basePack->idroot),currRing);
        else
          killhdl2(h,&(currRing->idroot),currRing);
       }
    }
  }
}

void killhdl2(idhdl h, idhdl * ih, ring r)
{
  //printf("kill %s, id %x, typ %d lev: %d\n",IDID(h),(int)IDID(h),IDTYP(h),IDLEV(h));
  idhdl hh;

  if (TEST_V_ALLWARN
  && (IDLEV(h)!=myynest)
  &&(IDLEV(h)==0))
  {
    if (((*ih)==basePack->idroot)
    || ((currRing!=NULL)&&((*ih)==currRing->idroot)))
      Warn("kill global `%s` at line >>%s<<\n",IDID(h),my_yylinebuf);
  }
  if (h->attribute!=NULL)
  {
    if ((IDTYP(h)==RING_CMD)&&(IDRING(h)!=r))
       h->attribute->killAll(IDRING(h));
    else
       h->attribute->killAll(r);
    h->attribute=NULL;
  }
  if (IDTYP(h) == PACKAGE_CMD)
  {
    if (((IDPACKAGE(h)->language==LANG_C)&&(IDPACKAGE(h)->idroot!=NULL))
    || (strcmp(IDID(h),"Top")==0))
    {
      Warn("cannot kill `%s`",IDID(h));
      return;
    }
    // any objects defined for this package ?
    if ((IDPACKAGE(h)->ref<=0)  &&  (IDPACKAGE(h)->idroot!=NULL))
    {
      if (currPack==IDPACKAGE(h))
      {
        currPack=basePack;
        currPackHdl=NULL;
      }
      idhdl * hd = &IDRING(h)->idroot;
      idhdl  hdh = IDNEXT(*hd);
      idhdl  temp;
      while (hdh!=NULL)
      {
        temp = IDNEXT(hdh);
        killhdl2(hdh,&(IDPACKAGE(h)->idroot),NULL);
        hdh = temp;
      }
      killhdl2(*hd,hd,NULL);
      if (IDPACKAGE(h)->libname!=NULL) omFree((ADDRESS)(IDPACKAGE(h)->libname));
    }
    paKill(IDPACKAGE(h));
    if (currPackHdl==h) currPackHdl=packFindHdl(currPack);
    iiCheckPack(currPack);
  }
  else if (IDTYP(h)==RING_CMD)
    rKill(h);
  else if (IDDATA(h)!=NULL)
    s_internalDelete(IDTYP(h),IDDATA(h),r);
  //  general  -------------------------------------------------------------
  // now dechain it and delete idrec
  if (IDID(h)!=NULL) // OB: ?????
    omFree((ADDRESS)IDID(h));
  IDID(h)=NULL;
  IDDATA(h)=NULL;
  if (h == (*ih))
  {
    // h is at the beginning of the list
    *ih = IDNEXT(h) /* ==*ih */;
  }
  else if (ih!=NULL)
  {
    // h is somethere in the list:
    hh = *ih;
    loop
    {
      if (hh==NULL)
      {
        PrintS(">>?<< not found for kill\n");
        return;
      }
      idhdl hhh = IDNEXT(hh);
      if (hhh == h)
      {
        IDNEXT(hh) = IDNEXT(hhh);
        break;
      }
      hh = hhh;
    }
  }
  omFreeBin((ADDRESS)h, idrec_bin);
}

#if 0
idhdl ggetid(const char *n, BOOLEAN /*local*/, idhdl *packhdl)
{
  idhdl h = IDROOT->get(n,myynest);
  idhdl h2=NULL;
  *packhdl = NULL;
  if ((currRing!=NULL) && ((h==NULL)||(IDLEV(h)!=myynest)))
  {
    h2 = currRing->idroot->get(n,myynest);
  }
  if (h2==NULL) return h;
  return h2;
}
#endif

#if 0
// debug version
idhdl ggetid(const char *n)
{
  if (currRing!=NULL)
  {
    idhdl h2 = currRing->idroot->get(n,myynest);
    idhdl h = IDROOT->get(n,myynest);
    if ((h!=NULL)&&(h2!=NULL)&&(IDLEV(h)==IDLEV(h2)))
    {
      Warn("SHADOW %s(%s) vs %s(%s) in %s\n",IDID(h),Tok2Cmdname(IDTYP(h)),IDID(h2),Tok2Cmdname(IDTYP(h2)),my_yylinebuf);
    }
    if ((h2!=NULL)&&(IDLEV(h2)==myynest)) return h2;
    if (h!=NULL) return h;
    if (h2!=NULL) return h2;
  }
  else
  {
    idhdl h = IDROOT->get(n,myynest);
    if (h!=NULL) return h;
  }
  if (basePack!=currPack)
    return basePack->idroot->get(n,myynest);
  return NULL;
}
#endif
#if 1
// try currRing before non-ring stuff
idhdl ggetid(const char *n)
{
  if (currRing!=NULL)
  {
    idhdl h2 = currRing->idroot->get(n,myynest);
    if ((h2!=NULL)&&(IDLEV(h2)==myynest)) return h2;
    idhdl h = IDROOT->get(n,myynest);
    if (h!=NULL) return h;
    if (h2!=NULL) return h2;
  }
  else
  {
    idhdl h = IDROOT->get(n,myynest);
    if (h!=NULL) return h;
  }
  if (basePack!=currPack)
    return basePack->idroot->get(n,myynest);
  return NULL;
}
#endif
#if 0
// try non-ring stuff before ring stuff
idhdl ggetid(const char *n)
{
  idhdl h = IDROOT->get(n,myynest);
  if ((h!=NULL)&&(IDLEV(h)==myynest)) return h;
  if (currRing!=NULL)
  {
    idhdl h2 = currRing->idroot->get(n,myynest);
    if (h2!=NULL) return h2;
  }
  if (h!=NULL) return h;
  if (basePack!=currPack)
    return basePack->idroot->get(n,myynest);
  return NULL;
}
#endif

void ipListFlag(idhdl h)
{
  if (hasFlag(h,FLAG_STD)) PrintS(" (SB)");
#ifdef HAVE_PLURAL
  if (hasFlag(h,FLAG_TWOSTD)) PrintS(" (2SB)");
#endif
}

lists ipNameList(idhdl root)
{
  idhdl h=root;
  /* compute the length */
  int l=0;
  while (h!=NULL) { l++; h=IDNEXT(h); }
  /* allocate list */
  lists L=(lists)omAllocBin(slists_bin);
  L->Init(l);
  /* copy names */
  h=root;
  l=0;
  while (h!=NULL)
  {
    /* list is initialized with 0 => no need to clear anything */
    L->m[l].rtyp=STRING_CMD;
    L->m[l].data=omStrDup(IDID(h));
    l++;
    h=IDNEXT(h);
  }
  return L;
}

lists ipNameListLev(idhdl root, int lev)
{
  idhdl h=root;
  /* compute the length */
  int l=0;
  while (h!=NULL) { if (IDLEV(h)==lev) l++; h=IDNEXT(h); }
  /* allocate list */
  lists L=(lists)omAllocBin(slists_bin);
  L->Init(l);
  /* copy names */
  h=root;
  l=0;
  while (h!=NULL)
  {
    if (IDLEV(h)==lev)
    {
      /* list is initialized with 0 => no need to clear anything */
      L->m[l].rtyp=STRING_CMD;
      L->m[l].data=omStrDup(IDID(h));
      l++;
    }
    h=IDNEXT(h);
  }
  return L;
}

/*
* move 'tomove' from root1 list to root2 list
*/
static int ipSwapId(idhdl tomove, idhdl &root1, idhdl &root2)
{
  idhdl h;
  /* search 'tomove' in root2 : if found -> do nothing */
  h=root2;
  while ((h!=NULL) && (h!=tomove)) h=IDNEXT(h);
  if (h!=NULL) return FALSE; /*okay */
  /* search predecessor of h in root1, remove 'tomove' */
  h=root1;
  if (tomove==h)
  {
    root1=IDNEXT(h);
  }
  else
  {
    while ((h!=NULL) && (IDNEXT(h)!=tomove)) h=IDNEXT(h);
    if (h==NULL) return TRUE; /* not in the list root1 -> do nothing */
    IDNEXT(h)=IDNEXT(tomove);
  }
  /* add to root2 list */
  IDNEXT(tomove)=root2;
  root2=tomove;
  return FALSE;
}

void  ipMoveId(idhdl tomove)
{
  if ((currRing!=NULL)&&(tomove!=NULL))
  {
    if (RingDependend(IDTYP(tomove))
    || ((IDTYP(tomove)==LIST_CMD) && (lRingDependend(IDLIST(tomove)))))
    {
      /*move 'tomove' to ring id's*/
      if (ipSwapId(tomove,IDROOT,currRing->idroot))
      ipSwapId(tomove,basePack->idroot,currRing->idroot);
    }
    else
    {
      /*move 'tomove' to global id's*/
      ipSwapId(tomove,currRing->idroot,IDROOT);
    }
  }
}

const char * piProcinfo(procinfov pi, const char *request)
{
  if((pi == NULL)||(pi->language==LANG_NONE)) return "empty proc";
  else if (strcmp(request, "libname")  == 0) return pi->libname;
  else if (strcmp(request, "procname") == 0) return pi->procname;
  else if (strcmp(request, "type")     == 0)
  {
    switch (pi->language)
    {
      case LANG_SINGULAR: return "singular"; break;
      case LANG_C:        return "object";   break;
      case LANG_NONE:     return "none";     break;
      default:            return "unknown language";
    }
  }
  else if (strcmp(request, "ref")      == 0)
  {
    char p[8];
    sprintf(p, "%d", pi->ref);
    return omStrDup(p);  // MEMORY-LEAK
  }
  return "??";
}

BOOLEAN piKill(procinfov pi)
{
  (pi->ref)--;
  if (pi->ref == 0)
  {
    if (pi->language==LANG_SINGULAR)
    {
      Voice *p=currentVoice;
      while (p!=NULL)
      {
        if (p->pi==pi && pi->ref <= 1)
        {
          Warn("`%s` in use, can not be killed",pi->procname);
          return TRUE;
        }
        p=p->next;
      }
    }
    if (pi->libname != NULL) // OB: ????
      omFree((ADDRESS)pi->libname);
    if (pi->procname != NULL) // OB: ????
      omFree((ADDRESS)pi->procname);

    if( pi->language == LANG_SINGULAR)
    {
      if (pi->data.s.body != NULL) // OB: ????
        omFree((ADDRESS)pi->data.s.body);
    }
    if( pi->language == LANG_C)
    {
    }
    memset((void *) pi, 0, sizeof(procinfo));
    //pi->language=LANG_NONE;
    omFreeBin((ADDRESS)pi,  procinfo_bin);
  }
  return FALSE;
}

void paCleanUp(package pack)
{
  (pack->ref)--;
  if (pack->ref < 0)
  {
#ifndef HAVE_STATIC
    if( pack->language == LANG_C)
    {
      Print("//dlclose(%s)\n",pack->libname);
#ifdef HAVE_DYNAMIC_LOADING
      dynl_close (pack->handle);
#endif /* HAVE_DYNAMIC_LOADING */
    }
#endif /* HAVE_STATIC */
    omFree((ADDRESS)pack->libname);
    memset((void *) pack, 0, sizeof(sip_package));
    pack->language=LANG_NONE;
  }
}

void proclevel::push(char *n)
{
  //Print("push %s\n",n);
  proclevel *p=(proclevel*)omAlloc0(sizeof(proclevel));
  p->name=n;
  p->cPackHdl=currPackHdl;
  p->cPack=currPack;
  p->next=this;
  procstack=p;
}
void proclevel::pop()
{
  //Print("pop %s\n",name);
  //if (currRing!=::currRing) PrintS("currRing wrong\n");;
  //::currRing=this->currRing;
  //if (r==NULL) Print("set ring to NULL at lev %d(%s)\n",myynest,name);
  //::currRingHdl=this->currRingHdl;
  //if((::currRingHdl==NULL)||(IDRING(::currRingHdl)!=(::currRing)))
  //  ::currRingHdl=rFindHdl(::currRing,NULL,NULL);
  //Print("restore pack=%s,1.obj=%s\n",IDID(currPackHdl),IDID(currPack->idroot));
  currPackHdl=this->cPackHdl;
  currPack=this->cPack;
  iiCheckPack(currPack);
  proclevel *p=this;
  procstack=next;
  omFreeSize(p,sizeof(proclevel));
}

idhdl packFindHdl(package r)
{
  idhdl h=basePack->idroot;
  while (h!=NULL)
  {
    if ((IDTYP(h)==PACKAGE_CMD)
        && (IDPACKAGE(h)==r))
      return h;
    h=IDNEXT(h);
  }
  return NULL;
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
  if ((h->Typ()!=p->Typ()) &&(p->Typ()!=DEF_CMD))
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
      case DEF_CMD:
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
         n_Delete(&IDNUMBER(pp),coeffs_BIGINT);
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
  int eff_typ=h->Typ();
  if ((RingDependend(eff_typ))
  || ((eff_typ==LIST_CMD) && (lRingDependend((lists)h->Data()))))
  {
    ipSwapId(pp,IDROOT,currRing->idroot);
  }
  h->CleanUp();
  omFreeBin((ADDRESS)h, sleftv_bin);
  return FALSE;
}

