/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/*
* ABSTRACT: identfier handling
*/

#include <string.h>

#include <kernel/mod2.h>
#include <Singular/static.h>
#include <omalloc/omalloc.h>
#include <Singular/tok.h>
#include <kernel/options.h>
#include <Singular/ipshell.h>
#include <kernel/intvec.h>
#include <kernel/febase.h>
#include <kernel/numbers.h>
#include <kernel/longrat.h>
#include <kernel/polys.h>
#include <kernel/ring.h>
#include <kernel/ideals.h>
#include <kernel/matpol.h>
#include <Singular/lists.h>
#include <Singular/attrib.h>
#include <Singular/silink.h>
#include <kernel/syz.h>
#include <Singular/ipid.h>
#include <Singular/blackbox.h>

#ifdef HAVE_DYNAMIC_LOADING
#include <kernel/mod_raw.h>
#endif /* HAVE_DYNAMIC_LOADING */

omBin sip_command_bin = omGetSpecBin(sizeof(sip_command));
omBin sip_package_bin = omGetSpecBin(sizeof(sip_package));
//omBin ip_package_bin = omGetSpecBin(sizeof(ip_package));
omBin idrec_bin = omGetSpecBin(sizeof(idrec));

proclevel *procstack=NULL;
#define TEST
//idhdl idroot = NULL;

idhdl currPackHdl = NULL;
idhdl basePackHdl = NULL;
package currPack =NULL;
package basePack =NULL;
idhdl currRingHdl = NULL;
ring  currRing = NULL;
ideal currQuotient = NULL;
const char* iiNoName="_";

void paCleanUp(package pack);

/*0 implementation*/

int iiS2I(const char *s)
{
  int i;
  i=s[0];
  if (s[1]!='\0')
  {
    i=(i<<8)+s[1];
    if (s[2]!='\0')
    {
      i=(i<<8)+s[2];
      if (s[3]!='\0')
      {
        i=(i<<8)+s[3];
      }
    }
  }
  return i;
}

idhdl idrec::get(const char * s, int level)
{
  assume(s!=NULL);
  assume((level>=0) && (level<=1000)); //not really, but if it isnt in that bounds..
  idhdl h = this;
  idhdl found=NULL;
  int l;
  const char *id_;
  int i=iiS2I(s);
  int less4=(i < (1<<24));
  while (h!=NULL)
  {
    omCheckAddr((ADDRESS)IDID(h));
    l=IDLEV(h);
    if ((l==0)||(l==level))
    {
      if (i==h->id_i)
      {
        id_=IDID(h);
        if (less4 || (0 == strcmp(s+4,id_+4)))
        {
          if (l==level) return h;
          found=h;
        }
      }
    }
    h = IDNEXT(h);
  }
  return found;
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
    case INTVEC_CMD:
    case INTMAT_CMD:
      return (void *)new intvec();
    case NUMBER_CMD:
      return (void *) nInit(0);
    case BIGINT_CMD:
      return (void *) nlInit(0, NULL /* dummy for nlInit*/);
    case IDEAL_CMD:
    case MODUL_CMD:
    case MATRIX_CMD:
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
      return (void*) omAlloc0Bin(sip_sring_bin);
    case PACKAGE_CMD:
      return (void*) omAlloc0Bin(sip_package_bin);
    case PROC_CMD:
      return (void *) omAlloc0Bin(procinfo_bin);
    case RESOLUTION_CMD:
      return  (void *)omAlloc0(sizeof(ssyStrategy));
    //other types: without init (int,script,poly,def,package)
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
          Werror("unknown type %d",t);
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
  h->id_i=iiS2I(s);
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
    else
#endif
    if (t == PROC_CMD)
    {
      IDPROC(h)->language=LANG_NONE;
    }
    else if (t == PACKAGE_CMD)
    {
      IDPACKAGE(h)->language=LANG_NONE;
      IDPACKAGE(h)->loaded = FALSE;
    }
  }
  // --------------------------------------------------------
  return  h;
}

char * idrec::String()
{
  sleftv tmp;
  memset(&tmp,0,sizeof(sleftv));
  tmp.rtyp=IDTYP(this);
  tmp.data=IDDATA(this);
  tmp.name=IDID(this);
  return tmp.String();
}

idhdl enterid(const char * s, int lev, int t, idhdl* root, BOOLEAN init, BOOLEAN search)
{
  if (s==NULL) return NULL;
  idhdl h;
  s=omStrDup(s);
  if (t==PACKAGE_CMD)
  {
    if (root!=&(basePack->idroot))
    {
      root=&(basePack->idroot);
    }
  }
  // is it already defined in root ?
  if ((h=(*root)->get(s,lev))!=NULL)
  {
    if (IDLEV(h)==lev)
    {
      if ((IDTYP(h) == t)||(t==DEF_CMD))
      {
        if ((IDTYP(h)==PACKAGE_CMD)
        && (strcmp(s,"Top")==0))
        {
          goto errlabel;
        }
        if (BVERBOSE(V_REDEFINE))
          Warn("redefining %s **",s);
        if (s==IDID(h)) IDID(h)=NULL;
        killhdl2(h,root,currRing);
      }
      else
        goto errlabel;
    }
  }
  // is it already defined in currRing->idroot ?
  else if (search && (currRing!=NULL)&&((*root) != currRing->idroot))
  {
    if ((h=currRing->idroot->get(s,lev))!=NULL)
    {
      if (IDLEV(h)==lev)
      {
        if ((IDTYP(h) == t)||(t==DEF_CMD))
        {
          if (BVERBOSE(V_REDEFINE))
            Warn("redefining %s **",s);
          IDID(h)=NULL;
          killhdl2(h,&currRing->idroot,currRing);
        }
        else
          goto errlabel;
      }
    }
  }
  // is it already defined in idroot ?
  else if (search && (*root != IDROOT))
  {
    if ((h=IDROOT->get(s,lev))!=NULL)
    {
      if (IDLEV(h)==lev)
      {
        if ((IDTYP(h) == t)||(t==DEF_CMD))
        {
          if (BVERBOSE(V_REDEFINE))
            Warn("redefining `%s` **",s);
          if (s==IDID(h)) IDID(h)=NULL;
          killhdl2(h,&IDROOT,NULL);
        }
        else
          goto errlabel;
      }
    }
  }
  *root = (*root)->set(s, lev, t, init);
#ifndef NDEBUG
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
    Werror("kill what ?");
}

void killhdl(idhdl h, package proot)
{
  int t=IDTYP(h);
  if (((BEGIN_RING<t) && (t<END_RING) && (t!=QRING_CMD))
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

  if (h->attribute!=NULL)
  {
    //h->attribute->killAll(r); MEMORY LEAK!
    h->attribute=NULL;
  }
  if (IDTYP(h) == PACKAGE_CMD)
  {
    if (strcmp(IDID(h),"Top")==0)
    {
      WarnS("can not kill `Top`");
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
  else if ((IDTYP(h)==RING_CMD)||(IDTYP(h)==QRING_CMD))
    rKill(h);
  else
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

idhdl ggetid(const char *n, BOOLEAN local, idhdl *packhdl)
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

idhdl ggetid(const char *n)
{
  idhdl h = IDROOT->get(n,myynest);
  if ((h!=NULL)&&(IDLEV(h)==myynest)) return h;
  idhdl h2=NULL;
  if (currRing!=NULL)
  {
    h2 = currRing->idroot->get(n,myynest);
  }
  if (h2!=NULL) return h2;
  if (h!=NULL) return h;
  if (basePack!=currPack)
    return basePack->idroot->get(n,myynest);
  return NULL;
}

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
    if (((QRING_CMD!=IDTYP(tomove)) && RingDependend(IDTYP(tomove)))
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
  if(pi == NULL) return "empty proc";
  else if (strcmp(request, "libname")  == 0) return pi->libname;
  else if (strcmp(request, "procname") == 0) return pi->procname;
  else if (strcmp(request, "type")     == 0)
  {
    switch (pi->language)
    {
      case LANG_SINGULAR: return "singular"; break;
      case LANG_C:        return "object";   break;
      case LANG_NONE:     return "none";     break;
      default:            return "unknow language";
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

void piCleanUp(procinfov pi)
{
  (pi->ref)--;
  if (pi->ref <= 0)
  {
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
    pi->language=LANG_NONE;
  }
}

BOOLEAN piKill(procinfov pi)
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
  piCleanUp(pi);
  if (pi->ref <= 0)
    omFreeBin((ADDRESS)pi,  procinfo_bin);
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
    omfree((ADDRESS)pack->libname);
    memset((void *) pack, 0, sizeof(sip_package));
    pack->language=LANG_NONE;
  }
}

void proclevel::push(char *n)
{
  //Print("push %s\n",n);
  proclevel *p=(proclevel*)omAlloc0(sizeof(proclevel));
  p->cRing=currRing;
  p->cRingHdl=currRingHdl;
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
