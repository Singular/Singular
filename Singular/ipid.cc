/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/* $Id: ipid.cc,v 1.13 1998-07-21 17:18:19 Singular Exp $ */

/*
* ABSTRACT: identfier handling
*/

#include <string.h>

#include "mod2.h"
#include "tok.h"
#include "ipshell.h"
#include "intvec.h"
#include "febase.h"
#include "mmemory.h"
#include "numbers.h"
#include "polys.h"
#include "ring.h"
#include "ideals.h"
#include "matpol.h"
#include "lists.h"
#include "attrib.h"
#include "silink.h"
#include "syz.h"
#include "ipid.h"

#ifdef HAVE_NAMESPACES
namehdl namespaceroot = NULL;
#define TEST
#else /* HAVE_NAMESPACES */
idhdl idroot = NULL;
#endif /* HAVE_NAMESPACES */
idhdl currRingHdl = NULL;
ring  currRing = NULL;
ideal currQuotient = NULL;
char* iiNoName="_";

/*0 implementation*/

idhdl idrec::get(const char * s, int lev)
{
  idhdl h = this;
  idhdl found=NULL;
  int l;
  while (h!=NULL)
  {
    l=IDLEV(h);
    mmTestLP(IDID(h));
    if ((l==lev) && (0 == strcmp(s,IDID(h)))) return h;
    if ((l==0) && (found==NULL) && (0 == strcmp(s,IDID(h))))
    {
      found=h;
    }
    h = IDNEXT(h);
  }
  return found;
}

//idrec::~idrec()
//{
//  if (id!=NULL)
//  {
//    FreeL((ADDRESS)id);
//    id=NULL;
//  }
//  /* much more !! */
//}

idhdl idrec::set(char * s, int lev, idtyp t, BOOLEAN init)
{
  //printf("define %s, %x, lev: %d, typ: %d\n", s,s,lev,t);
  idhdl h = (idrec *)Alloc0(sizeof(idrec));
  int   len = 0;
  IDID(h)   = s;
  IDTYP(h)  = t;
  IDLEV(h)  = lev;
#ifdef HAVE_NAMESPACES
  h->ref    = 1;
#endif /* HAVE_NAMESPACES */
  IDNEXT(h) = this;
  if (init)
  {
    switch (t)
    {
    //the type with init routines:
      case INTVEC_CMD:
      case INTMAT_CMD:
        IDINTVEC(h) = new intvec();
        break;
      case NUMBER_CMD:
        IDNUMBER(h) = nInit(0);
        break;
      case IDEAL_CMD:
      case MODUL_CMD:
        IDFLAG(h) = Sy_bit(FLAG_STD);
      case MATRIX_CMD:
        IDIDEAL(h) = idInit(1,1);
        break;
      case MAP_CMD:
        IDIDEAL(h) = idInit(1,1);
        IDMAP(h)->preimage = mstrdup(IDID(currRingHdl));
        break;
      case STRING_CMD:
        IDSTRING(h) = mstrdup("");
        break;
      case LIST_CMD:
        IDLIST(h)=(lists)Alloc(sizeof(slists));
        IDLIST(h)->Init();
        break;
    //the types with the standard init: set the struct to zero
      case LINK_CMD:
        len=sizeof(ip_link);
        break;
      case RING_CMD:
      case QRING_CMD:
        len = sizeof(ip_sring);
        break;
      case PACKAGE_CMD:
        len = sizeof(ip_package);
        break;
      case RESOLUTION_CMD:
        len=sizeof(ssyStrategy);
        break;
      case PROC_CMD:
        len=sizeof(procinfo);
	break;
    //other types: without init (int,script,poly,def,package)
    }
    if (len!=0)
    {
      IDSTRING(h) = (char *)Alloc0(len);
    }
  }
  // additional settings:--------------------------------------
  if (t == QRING_CMD)
  {
    IDRING(h)=rCopy(currRing);
    /* QRING_CMD is ring dep => currRing !=NULL */
  }
  else if (t == PROC_CMD)
  {
    IDPROC(h)->language=LANG_NONE;
  }
  // --------------------------------------------------------
  return  h;
}

idhdl enterid(char * s, int lev, idtyp t, idhdl* root, BOOLEAN init)
{
  idhdl h;
  // is it the currRing - id ?
#ifdef HAVE_NAMESPACES
  namehdl topnsroot = namespaceroot->root;
#endif
  if ((currRingHdl!=NULL)
  &&(IDLEV(currRingHdl)!=lev)
  &&(s==IDID(currRingHdl)))
  {
    s=mstrdup(s);
  }
  // is it already defined in root ?
  else if ((h=(*root)->get(s,lev))!=NULL)
  {
    if (IDLEV(h)!=lev)
    {
      s=mstrdup(s);
    }
    else if ((IDTYP(h) == t)||(t==DEF_CMD))
    {
      if (BVERBOSE(V_REDEFINE))
        Warn("redefining %s **",s);
      if (s==IDID(h))
        IDID(h)=NULL;
      killhdl(h,root);
    }
    else
      goto errlabel;
  }
  // is it already defined in idroot ?
  else if (*root != IDROOT)
  {
#ifdef HAVE_NAMESPACES
    if ((h=namespaceroot->get(s,lev))!=NULL)
#else
    if ((h=idroot->get(s,lev))!=NULL)
#endif /* HAVE_NAMESPACES */
    {
      if (IDLEV(h)!=lev)
      {
        s=mstrdup(s);
      }
      else if ((IDTYP(h) == t)||(t==DEF_CMD))
      {
        if (BVERBOSE(V_REDEFINE))
          Warn("redefining %s **",s);
        IDID(h)=NULL;
        killhdl(h,&IDROOT);
      }
      else
      {
        goto errlabel;
      }
    }
  }
#ifdef HAVE_NAMESPACES
  // is it already defined in toplevel idroot ?
  else if (*root != NSROOT(topnsroot))
  {
    if ((h=topnsroot->get(s,lev))!=NULL)
    {
#if 0
      if (IDLEV(h)!=lev)
      {
        s=mstrdup(s);
      }
      else if ((IDTYP(h) == t)||(t==DEF_CMD))
      {
        if (BVERBOSE(V_REDEFINE))
          Warn("1 redefining %s **",s);
        IDID(h)=NULL;
        killhdl(h,&(NSROOT(topnsroot)));
      }
      else
      {
        goto errlabel;
      }
#else
      s=mstrdup(s);
#endif
    }
  }
#endif /* HAVE_NAMESPACES */
  // is it already defined in currRing->idroot ?
  else if ((currRing!=NULL)&&((*root) != currRing->idroot))
  {
    if ((h=currRing->idroot->get(s,lev))!=NULL)
    {
      if (IDLEV(h)!=lev)
      {
        s=mstrdup(s);
      }
      else if ((IDTYP(h) == t)||(t==DEF_CMD))
      {
        if (BVERBOSE(V_REDEFINE))
          Warn("redefining %s **",s);
        IDID(h)=NULL;
        killhdl(h,&currRing->idroot);
      }
      else
      {
        goto errlabel;
      }
    }
  }
  return *root = (*root)->set(s, lev, t, init);

  errlabel:
    Werror("identifier `%s` in use",s);
    return NULL;
}

void killid(char * id, idhdl * ih)
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
          killhdl(h,&(currRing->idroot));
          return;
        }
      }
      Werror("`%s` is not defined",id);
      return;
    }
    killhdl(h,ih);
  }
  else
    Werror("kill what ?");
}

void killhdl(idhdl h)
{
  int t=IDTYP(h);
  if ((BEGIN_RING<t) && (t<END_RING) && (t!=QRING_CMD))
    killhdl(h,&currRing->idroot);
  else
  {
    idhdl s=IDROOT;
    while ((s!=h) && (s!=NULL)) s=s->next;
    if (s==NULL) killhdl(h,&currRing->idroot);
    else killhdl(h,&IDROOT);
  }
}

void killhdl(idhdl h, idhdl * ih)
{
  //printf("kill %s, id %x, typ %d lev: %d\n",IDID(h),(int)IDID(h),IDTYP(h),IDLEV(h));
  idhdl hh;
  BOOLEAN killOtherRing = TRUE;
  BOOLEAN needResetRing = FALSE;

  if (h->attribute!=NULL)
  {
    atKillAll(h);
    //h->attribute=NULL;
  }
  // ring / qring  --------------------------------------------------------
  // package  -------------------------------------------------------------
  if ((IDTYP(h) == RING_CMD) || (IDTYP(h) == QRING_CMD)
  || (IDTYP(h) == PACKAGE_CMD) /*|| (IDTYP(h) == POINTER_CMD)*/)
  {
    idhdl savecurrRingHdl = currRingHdl;
    ring  savecurrRing = currRing;

    // any objects defined for this ring ?
    if (((IDTYP(h)==PACKAGE_CMD) || (IDRING(h)->ref<=0))
    &&  (IDRING(h)->idroot!=NULL))
    {
      idhdl * hd = &IDRING(h)->idroot;
      idhdl  hdh = IDNEXT(*hd);
      idhdl  temp;
      killOtherRing=(IDTYP(h)!=PACKAGE_CMD) && (IDRING(h)!=currRing);
      if (killOtherRing) //we are not killing the base ring, so switch
      {
        needResetRing=TRUE;
        rSetHdl(h,FALSE);
        /* no complete init*/
      }
      while (hdh!=NULL)
      {
        temp = IDNEXT(hdh);
        killhdl(hdh,&(IDRING(h)->idroot));
        hdh = temp;
      }
      killhdl(*hd,hd);
    }
    // reset currRing ?
    if (needResetRing) // && (killOtherRing)
    {
      //we have to switch back to the base ring
      //currRing = savecurrRing;
      //currRingHdl = savecurrRingHdl;
      if (savecurrRing!=NULL)
      {
        rSetHdl(savecurrRingHdl,TRUE);
      }
    }
#ifdef HAVE_NAMESPACES
    if((IDTYP(h)==PACKAGE_CMD) || (IDTYP(h)==POINTER_CMD))
      Print(">>>>>>Free package\n");
    else
#endif /* HAVE_NAMESPACES */
      rKill(h);
  }
  // poly / vector -------------------------------------------------------
  else if ((IDTYP(h) == POLY_CMD) || (IDTYP(h) == VECTOR_CMD))
  {
    pDelete(&IDPOLY(h));
  }
  // ideal / module/ matrix / map ----------------------------------------
  else if ((IDTYP(h) == IDEAL_CMD)
           || (IDTYP(h) == MODUL_CMD)
           || (IDTYP(h) == MATRIX_CMD)
           || (IDTYP(h) == MAP_CMD))
  {
    ideal iid = IDIDEAL(h);
    if (IDTYP(h) == MAP_CMD)
    {
      map im = IDMAP(h);
      FreeL((ADDRESS)im->preimage);
    }
    idDelete(&iid);
  }
  // string -------------------------------------------------------------
  else if (IDTYP(h) == STRING_CMD)
  {
    FreeL((ADDRESS)IDSTRING(h));
    //IDSTRING(h)=NULL;
  }
  // proc ---------------------------------------------------------------
  else if (IDTYP(h) == PROC_CMD)
  {
    piKill(IDPROC(h));
  }
  // number -------------------------------------------------------------
  else if (IDTYP(h) == NUMBER_CMD)
  {
    nDelete(&IDNUMBER(h));
  }
  // intvec / intmat  ---------------------------------------------------
  else if ((IDTYP(h) == INTVEC_CMD)||(IDTYP(h) == INTMAT_CMD))
  {
    delete IDINTVEC(h);
  }
  // list  -------------------------------------------------------------
  else if (IDTYP(h)==LIST_CMD)
  {
    IDLIST(h)->Clean();
    //Free((ADDRESS)IDLIST(h)->m, (IDLIST(h)->nr+1)*sizeof(sleftv));
    //Free((ADDRESS)IDLIST(h), sizeof(slists));
  }
  // link  -------------------------------------------------------------
  else if (IDTYP(h)==LINK_CMD)
  {
    slKill(IDLINK(h));
  }
  else if((IDTYP(h)==RESOLUTION_CMD)&&(IDDATA(h)!=NULL))
  {
    syKillComputation((syStrategy)IDDATA(h));
  }
#ifdef TEST
  else if ((IDTYP(h)!= INT_CMD)&&(IDTYP(h)!=DEF_CMD))
    Warn("unknown type to kill: %s(%d)",Tok2Cmdname(IDTYP(h)),IDTYP(h));
#endif

  //  general  -------------------------------------------------------------
  // now dechain it and delete idrec
  FreeL((ADDRESS)IDID(h));
  //IDID(h)=NULL;
  if (h == (*ih))
  {
    // h is at the beginning of the list
    *ih = IDNEXT(*ih);
  }
  else
  {
    // h is somethere in the list:
    hh = *ih;
    loop
    {
      idhdl hhh = IDNEXT(hh);
      if (hhh == h)
      {
        IDNEXT(hh) = IDNEXT(hhh);
        break;
      }
      hh = hhh;
    }
  }
  Free((ADDRESS)h,sizeof(idrec));
}

idhdl ggetid(const char *n)
{
#ifdef HAVE_NAMESPACES
  idhdl h =namespaceroot->get(n,myynest, TRUE); // search in toplevel namespace
  idhdl h2=namespaceroot->get(n,myynest); // search in current namespace
  idhdl h3=NULL;
  if ((currRing!=NULL) && ((h2==NULL)||(IDLEV(h2)!=myynest)))
  {
    h3 = currRing->idroot->get(n,myynest);
  }
  if (h3==NULL) {
    if (h2==NULL) return h; else return h2;
  }
  return h3;
#else /* HAVE_NAMESPACES */
  idhdl h = idroot->get(n,myynest);
  idhdl h2=NULL;
  if ((currRing!=NULL) && ((h==NULL)||(IDLEV(h)!=myynest)))
  {
    h2 = currRing->idroot->get(n,myynest);
  }
  if (h2==NULL) return h;
  return h2;
#endif /* HAVE_NAMESPACES */
}

void ipListFlag(idhdl h)
{
  if (hasFlag(h,FLAG_STD)) PrintS(" (SB)");
  if (hasFlag(h,FLAG_DRING)) PrintS(" (D)");
}

lists ipNameList(idhdl root)
{
  idhdl h=root;
  /* compute the length */
  int l=0;
  while (h!=NULL) { l++; h=IDNEXT(h); }
  /* allocate list */
  lists L=(lists)Alloc(sizeof(slists));
  L->Init(l);
  /* copy names */
  h=root;
  l=0;
  while (h!=NULL)
  {
    /* list is initialized with 0 => no need to clear anything */
    L->m[l].rtyp=STRING_CMD;
    L->m[l].data=mstrdup(IDID(h));
    l++;
    h=IDNEXT(h);
  }
  return L;
}

/*
* move 'tomove' from root1 list to root2 list
*/
static void ipSwapId(idhdl tomove, idhdl &root1, idhdl &root2)
{
  idhdl h;
  /* search 'tomove' in root2 : if found -> do nothing */
  h=root2;
  while ((h!=NULL) && (h!=tomove)) h=IDNEXT(h);
  if (h!=NULL) return;
  /* search predecessor of h in root1, remove 'tomove' */
  h=root1;
  if (tomove==h)
  {
    root1=IDNEXT(h);
  }
  else
  {
    while ((h!=NULL) && (IDNEXT(h)!=tomove)) h=IDNEXT(h);
    if (h==NULL) return; /* not in the list root1 -> do nothing */
    IDNEXT(h)=IDNEXT(tomove);
  }
  /* add to root2 list */
  IDNEXT(tomove)=root2;
  root2=tomove;
}

void  ipMoveId(idhdl tomove)
{
  if ((currRing!=NULL)&&(tomove!=NULL))
  {
    if (((QRING_CMD!=IDTYP(tomove)) && (BEGIN_RING<IDTYP(tomove)) && (IDTYP(tomove)<END_RING))
    || ((IDTYP(tomove)==LIST_CMD) && (lRingDependend(IDLIST(tomove)))))
    {
      /*move 'tomove' to ring id's*/
      ipSwapId(tomove,IDROOT,currRing->idroot);
    }
    else
    {
      /*move 'tomove' to global id's*/
      ipSwapId(tomove,currRing->idroot,IDROOT);
    }
  }
}

char * piProcinfo(procinfov pi, char *request)
{
  if(pi == NULL) return "empty proc";
  else if (strcmp(request, "libname")  == 0) return pi->libname;
  else if (strcmp(request, "procname") == 0) return pi->procname;
  else if (strcmp(request, "type")     == 0) {
    switch (pi->language) {
      case LANG_SINGULAR: return "singular"; break;
      case LANG_C:        return "object";   break;
      case LANG_NONE:     return "none";     break;
      default:            return "unknow language";
    }
  } else if (strcmp(request, "ref")      == 0) {
    char p[8];
    sprintf(p, "%d", pi->ref);
    return mstrdup(p);  // MEMORY-LEAK
  }
  return "??";
}

void piCleanUp(procinfov pi)
{
  (pi->ref)--;
  if (pi->ref <= 0)
  {
    FreeL((ADDRESS)pi->libname);
    FreeL((ADDRESS)pi->procname);
    if( pi->language == LANG_SINGULAR)
    {
      FreeL((ADDRESS)pi->data.s.body);
    }
    if( pi->language == LANG_C)
    {
    }
    memset((void *) pi, 0, sizeof(procinfo));
    pi->language=LANG_NONE;
  }
}

void piKill(procinfov pi)
{
  piCleanUp(pi);
  if (pi->ref <= 0)
    Free((ADDRESS)pi, sizeof(procinfo));
}

#ifdef HAVE_NAMESPACES
char *getnamelev()
{
  char buf[256];
  sprintf(buf, "(%s:%d)", namespaceroot->name,namespaceroot->lev);
  return(buf);
}

namehdl namerec::push(package pack, char *name, BOOLEAN init)
{
  //printf("PUSH: put entry (%s) on stack\n", name);
  namehdl ns = (namerec *)Alloc0(sizeof(namerec));
  ns->next   = this;
  if(this==NULL && !init)
  {
    printf("PUSH: this is NULL and init not set.\n");
    init=TRUE;
  }
  if(init)
  {
    ns->pack   = (ip_package *)Alloc0(sizeof(ip_package));
    ns->isroot = TRUE;
    ns->lev    = 1;
  }
  elses
  {
    ns->pack   = pack;
    ns->lev    = this->lev+1;
  }
  ns->name   = mstrdup(name);


  //if(ns->isroot) Print("PUSH: Add root NameSpace\n");
  if(ns->isroot) ns->root=ns; else ns->root = this->root;
  namespaceroot = ns;
  if(init && ns->isroot)
  {
    idhdl pl = enterid( mstrdup("toplevel"),0, PACKAGE_CMD,
                      &NSROOT(namespaceroot), TRUE );
  }
  return(namespaceroot);
}

namehdl namerec::pop()
{
  namehdl ns;
  //printf("POP: remove entry (%s)\n", this->name);
  if(this==NULL) return NULL;
  if(isroot) {
    //printf("POP: base. woul'd do it.\n");
    return this;
  }
  ns = this;
  namespaceroot = this->next;
  FreeL((ADDRESS)ns->name);
  Free((ADDRESS)ns, sizeof(namerec));
  return(namespaceroot);
}

idhdl namerec::get(const char * s, int lev, int root)
{
  namehdl ns;
  if(root) {
    ns = this->root;
  } else {
    ns = this;
  }
  //printf("//====== namerec::get(%s, %d) from '%s'\n", s, lev, ns->name);
  if(ns==NULL) {
    //printf("//======== namerec::get() from null\n");
    return NULL;
  }
  return( NSROOT(ns)->get(s, lev));
}
#endif /* HAVE_NAMESPACES */
