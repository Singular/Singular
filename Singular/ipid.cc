/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/* $Id: ipid.cc,v 1.56 2002-01-07 17:20:48 Singular Exp $ */

/*
* ABSTRACT: identfier handling
*/

#include <string.h>

#include "mod2.h"
#include "omalloc.h"
#include "tok.h"
#include "ipshell.h"
#include "intvec.h"
#include "febase.h"
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


omBin sip_command_bin = omGetSpecBin(sizeof(sip_command));
omBin ip_command_bin = omGetSpecBin(sizeof(ip_command));
omBin sip_package_bin = omGetSpecBin(sizeof(sip_package));
omBin ip_package_bin = omGetSpecBin(sizeof(ip_package));
omBin idrec_bin = omGetSpecBin(sizeof(idrec));
#ifdef HAVE_NAMESPACES
omBin namerec_bin = omGetSpecBin(sizeof(namerec));
namehdl namespaceroot = NULL;
#endif

proclevel *procstack=NULL;
#define TEST
#ifndef HAVE_NAMESPACES
idhdl idroot = NULL;
#endif /* HAVE_NAMESPACES */

#ifdef HAVE_NS
idhdl currPackHdl = NULL;
idhdl basePackHdl = NULL;
package currPack =NULL;
package basePack =NULL;
#endif /* HAVE_NS */
idhdl currRingHdl = NULL;
ring  currRing = NULL;
ideal currQuotient = NULL;
char* iiNoName="_";

void paCleanUp(package pack);
#ifdef HAVE_NAMESPACES
BOOLEAN paKill(package pack, BOOLEAN force_top=FALSE);
#endif

/*0 implementation*/

idhdl idrec::get(const char * s, int lev)
{
  idhdl h = this;
  idhdl found=NULL;
  int l;
  char *id;
  if (s[1]=='\0')
  {
    while (h!=NULL)
    {
      omCheckAddr(IDID(h));
// =============================================================
#if 0
// timings: ratchwum: 515 s, wilde13: 373 s, nepomuck: 267 s, lukas 863 s
    id=IDID(h);
    l=IDLEV(h);
    if ((l==0) && (*(short *)s==*(short *)id) && (0 == strcmp(s+1,id+1)))
    {
      found=h;
    }
    else if ((l==lev) && (*(short *)s==*(short *)id) && (0 == strcmp(s+1,id+1)))
    {
      return h;
    }
#endif
// =============================================================
#if 0
// timings: ratchwum: 515 s, wilde13: 398 s, nepomuck: 269 s, lukas 834 s
    id=IDID(h);
    if (*(short *)s==*(short *)id)
    {
      l=IDLEV(h);
      if ((l==0) && (0 == strcmp(s+1,id+1)))
      {
        found=h;
      }
      else if ((l==lev) && (0 == strcmp(s+1,id+1)))
      {
        return h;
      }
    }
#endif
// =============================================================
#if 1
// timings: ratchwum: 501 s, wilde13: 357 s, nepomuck: 267 s, lukas 816 s
// timings bug4: ratchwum: s, wilde13: s, nepomuck: 379.74 s, lukas s
    l=IDLEV(h);
    if ((l==0)||(l==lev))
    {
      id=IDID(h);
      if (*(short *)s==*(short *)id)
      {
        if (0 == strcmp(s+1,id+1))
        {
          if (l==lev) return h;
          found=h;
        }
      }
    }
#endif
// =============================================================
#if 0
// timings: ratchwum: s, wilde13: s, nepomuck: s, lukas s
// timings bug4: ratchwum: s, wilde13: s, nepomuck: s, lukas s
    l=IDLEV(h);
    if ((l==0)||(l==lev))
    {
      id=IDID(h);
      if (*(short *)s==*(short *)id)
      {
        if (l==lev) return h;
        found=h;
      }
    }
#endif
// =============================================================
    h = IDNEXT(h);
  }
  }
  else
  {
  while (h!=NULL)
  {
    omCheckAddr(IDID(h));
// =============================================================
#if 0
// timings: ratchwum: 515 s, wilde13: 373 s, nepomuck: 267 s, lukas 863 s
    id=IDID(h);
    l=IDLEV(h);
    if ((l==0) && (*(short *)s==*(short *)id) && (0 == strcmp(s+1,id+1)))
    {
      found=h;
    }
    else if ((l==lev) && (*(short *)s==*(short *)id) && (0 == strcmp(s+1,id+1)))
    {
      return h;
    }
#endif
// =============================================================
#if 0
// timings: ratchwum: 515 s, wilde13: 398 s, nepomuck: 269 s, lukas 834 s
    id=IDID(h);
    if (*(short *)s==*(short *)id)
    {
      l=IDLEV(h);
      if ((l==0) && (0 == strcmp(s+1,id+1)))
      {
        found=h;
      }
      else if ((l==lev) && (0 == strcmp(s+1,id+1)))
      {
        return h;
      }
    }
#endif
// =============================================================
#if 0
// timings: ratchwum: 501 s, wilde13: 357 s, nepomuck: 267 s, lukas 816 s
// timings bug4: ratchwum: s, wilde13: s, nepomuck: 379.74 s, lukas s
    l=IDLEV(h);
    if ((l==0)||(l==lev))
    {
      id=IDID(h);
      if (*(short *)s==*(short *)id)
      {
        if (0 == strcmp(s+1,id+1))
        {
          if (l==lev) return h;
          found=h;
        }
      }
    }
#endif
// =============================================================
#if 1
// timings: ratchwum: s, wilde13: s, nepomuck: s, lukas s
// timings bug4: ratchwum: s, wilde13: s, nepomuck: s, lukas s
    l=IDLEV(h);
    if ((l==0)||(l==lev))
    {
      id=IDID(h);
      if (*(short *)s==*(short *)id)
      {
        if (0 == strcmp(s+2,id+2))
        {
          if (l==lev) return h;
          found=h;
        }
      }
    }
#endif
// =============================================================
    h = IDNEXT(h);
  }
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

idhdl idrec::set(char * s, int lev, idtyp t, BOOLEAN init)
{
  //printf("define %s, %x, lev: %d, typ: %d\n", s,s,lev,t);
  idhdl h = (idrec *)omAlloc0Bin(idrec_bin);
  int   len = 0;
  IDID(h)   = s;
  IDTYP(h)  = t;
  IDLEV(h)  = lev;
#ifdef HAVE_NAMESPACES
  h->ref    = 0;
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
        IDMAP(h)->preimage = omStrDup(IDID(currRingHdl));
        break;
      case STRING_CMD:
        IDSTRING(h) = omStrDup("");
        break;
      case LIST_CMD:
        IDLIST(h)=(lists)omAllocBin(slists_bin);
        IDLIST(h)->Init();
        break;
      case LINK_CMD:
        IDLINK(h)=(si_link) omAlloc0Bin(sip_link_bin);
        break;
      case RING_CMD:
      case QRING_CMD:
        IDRING(h) = (ring) omAlloc0Bin(sip_sring_bin);
        break;
      case PACKAGE_CMD:
        IDPACKAGE(h) = (package) omAlloc0Bin(sip_package_bin);
        break;
      case PROC_CMD:
        IDPROC(h) = (procinfo*) omAlloc0Bin(procinfo_bin);
        break;
        //the types with the standard init: set the struct to zero
      case RESOLUTION_CMD:
        len=sizeof(ssyStrategy);
        break;
    //other types: without init (int,script,poly,def,package)
    }
    if (len!=0)
    {
      IDSTRING(h) = (char *)omAlloc0(len);
    }
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

//#define KAI
idhdl enterid(char * s, int lev, idtyp t, idhdl* root, BOOLEAN init)
{
  idhdl h;
#ifdef HAVE_NAMESPACES
  namehdl topnsroot = namespaceroot->root;
#endif
  s=omStrDup(s);
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
#ifdef HAVE_NAMESPACES
      if(t==PACKAGE_CMD && strcmp(s,"Top")==0)
      {
        Warn("identifier `%s` in use",s);
        return(h);
      }
#endif /* HAVE_NAMESPACES */
      if (s==IDID(h)) IDID(h)=NULL;
      killhdl2(h,root);
    }
    else
      goto errlabel;
    }
  }
  // is it already defined in idroot ?
  else if (*root != IDROOT)
  {
    if ((h=IDROOT->get(s,lev))!=NULL)
    {
      if (IDLEV(h)==lev)
      {
      if ((IDTYP(h) == t)||(t==DEF_CMD))
      {
        if (BVERBOSE(V_REDEFINE))
          Warn("redefining %s **",s);
        if (s==IDID(h)) IDID(h)=NULL;
        killhdl2(h,&IDROOT);
      }
      else
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
    }
  }
#endif /* HAVE_NAMESPACES */
  // is it already defined in currRing->idroot ?
  else if ((currRing!=NULL)&&((*root) != currRing->idroot))
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
        killhdl2(h,&currRing->idroot);
      }
      else
        goto errlabel;
      }
    }
  }
  *root = (*root)->set(s, lev, t, init);
#ifdef HAVE_NS
  checkall();
#endif
  return *root;

  errlabel:
    //Werror("identifier `%s` in use(lev h=%d,typ=%d,t=%d, curr=%d)",s,IDLEV(h),IDTYP(h),t,lev);
    Werror("identifier `%s` in use",s);
#ifdef HAVE_NS
    //listall();
#endif
    omFree(s);
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
          killhdl2(h,&(currRing->idroot));
          return;
        }
      }
      Werror("`%s` is not defined",id);
      return;
    }
    killhdl2(h,ih);
  }
  else
    Werror("kill what ?");
}

void killhdl(idhdl h)
{
  int t=IDTYP(h);
  if ((BEGIN_RING<t) && (t<END_RING) && (t!=QRING_CMD))
    killhdl2(h,&currRing->idroot);
  else
  {
#ifdef HAVE_NAMESPACES
    if(t==PACKAGE_CMD)
    {
      killhdl2(h,&NSROOT(namespaceroot->root));
    }
    else
#endif /* HAVE_NAMESPACES */
#ifdef HAVE_NS
    if(t==PACKAGE_CMD)
    {
      killhdl2(h,&(basePack->idroot));
    }
    else
    {
      idhdl s=currPack->idroot;
      while ((s!=h) && (s!=NULL)) s=s->next;
      if (s!=NULL)
        killhdl2(h,&(currPack->idroot));
      else if (basePack!=currPack)
      {
        idhdl s=basePack->idroot;
        while ((s!=h) && (s!=NULL)) s=s->next;
        if (s!=NULL)
          killhdl2(h,&(basePack->idroot));
        else
          killhdl2(h,&(currRing->idroot));
       }
    }
#else /* HAVE_NS */
    {
      idhdl s=IDROOT;
      while ((s!=h) && (s!=NULL)) s=s->next;
      if (s==NULL) killhdl2(h,&(currRing->idroot));
      else killhdl2(h,&IDROOT);
    }
#endif /* HAVE_NAMESPACES */
  }
}

void killhdl2(idhdl h, idhdl * ih)
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
  if ((IDTYP(h) == PACKAGE_CMD) && (strcmp(IDID(h),"Top")==0))
  {
    WarnS("can not kill `Top`");
    return;
  }
  // ring / qring  --------------------------------------------------------
  if ((IDTYP(h) == RING_CMD) || (IDTYP(h) == QRING_CMD))
  {
    idhdl savecurrRingHdl = currRingHdl;
    ring  savecurrRing = currRing;
    // any objects defined for this ring ?
    // Hmm ... why onlyt for rings and not for qrings??
    // if (((IDTYP(h)==RING_CMD) && (IDRING(h)->ref<=0))
    if ((IDRING(h)->ref<=0)  &&  (IDRING(h)->idroot!=NULL))
    {
      idhdl * hd = &IDRING(h)->idroot;
      idhdl  hdh = IDNEXT(*hd);
      idhdl  temp;
      killOtherRing=(IDRING(h)!=currRing);
      if (killOtherRing) //we are not killing the base ring, so switch
      {
        needResetRing=TRUE;
        rSetHdl(h);
        /* no complete init*/
      }
      else
      {
        // we are killing the basering, so: make sure that
        // sLastPrinted is killed before this ring is destroyed
        if (((sLastPrinted.rtyp>BEGIN_RING) && (sLastPrinted.rtyp<END_RING))
        || ((sLastPrinted.rtyp==LIST_CMD)&&(lRingDependend((lists)sLastPrinted.data))))
        {
          sLastPrinted.CleanUp();
        }
      }
      while (hdh!=NULL)
      {
        temp = IDNEXT(hdh);
        killhdl2(hdh,&(IDRING(h)->idroot));
        hdh = temp;
      }
      killhdl2(*hd,hd);
    }
    // reset currRing ?
    if (needResetRing) // && (killOtherRing)
    {
      //we have to switch back to the base ring
      //currRing = savecurrRing;
      //currRingHdl = savecurrRingHdl;
      if (savecurrRingHdl!=NULL)
      {
        rSetHdl(savecurrRingHdl);
      }
      else if (savecurrRing!=NULL)
      {
        rChangeCurrRing(savecurrRing);
      }
    }
    rKill(h);
  }
#ifdef HAVE_NAMESPACES
  // package -------------------------------------------------------------
  else if (IDTYP(h) == PACKAGE_CMD)
  {
    if(IDPACKAGE(h)->language!=LANG_TOP)
    {
      if(!paKill(IDPACKAGE(h))) return;
    }
    else
    {
      if(strcmp(IDID(h), "Top")!=0)
      {
        if(!paKill(IDPACKAGE(h))) return;
      }
      else
      {
        if(!paKill(IDPACKAGE(h), TRUE)) return;
      }
    }
  }
  // pointer -------------------------------------------------------------
  else if(IDTYP(h)==POINTER_CMD)
  {
    PrintS(">>>>>>Free pointer\n");
  }
#endif /* HAVE_NAMESPACES */
#ifdef HAVE_NS
  // package -------------------------------------------------------------
  else if (IDTYP(h) == PACKAGE_CMD)
  {
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
        killhdl2(hdh,&(IDPACKAGE(h)->idroot));
        hdh = temp;
      }
      killhdl2(*hd,hd);
    }
    paKill(IDPACKAGE(h));
    if (currPackHdl==h) currPackHdl=packFindHdl(currPack);
  }
#endif /* HAVE_NS */
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
      omFree((ADDRESS)im->preimage);
    }
    idDelete(&iid);
  }
  // string -------------------------------------------------------------
  else if (IDTYP(h) == STRING_CMD)
  {
    omFree((ADDRESS)IDSTRING(h));
    //IDSTRING(h)=NULL;
  }
  // proc ---------------------------------------------------------------
  else if (IDTYP(h) == PROC_CMD)
  {
    if (piKill(IDPROC(h))) return;
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
    //omFreeSize((ADDRESS)IDLIST(h)->m, (IDLIST(h)->nr+1)*sizeof(sleftv));
    //omFreeBin((ADDRESS)IDLIST(h),  slists_bin);
  }
  // link  -------------------------------------------------------------
  else if (IDTYP(h)==LINK_CMD)
  {
    slKill(IDLINK(h));
  }
  else if(IDTYP(h)==RESOLUTION_CMD)
  {
    if (IDDATA(h)!=NULL)
      syKillComputation((syStrategy)IDDATA(h));
  }
#ifdef TEST
  else if ((IDTYP(h)!= INT_CMD)&&(IDTYP(h)!=DEF_CMD) && (IDTYP(h)!=NONE))
    Warn("unknown type to kill: %s(%d)",Tok2Cmdname(IDTYP(h)),IDTYP(h));
#endif

  //  general  -------------------------------------------------------------
  // now dechain it and delete idrec
#ifdef KAI
  if(h->next != NULL)
    Print("=======>%s(%x) -> %s<====\n", IDID(h), IDID(h), IDID(h->next));
  else
    Print("=======>%s(%x)<====\n", IDID(h), IDID(h));
#endif

  if (IDID(h)) // OB: ?????
    omFree((ADDRESS)IDID(h));
  IDID(h)=NULL;
  if (h == (*ih))
  {
    // h is at the beginning of the list
    *ih = IDNEXT(h) /* ==*ih */;
  }
  else
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
#ifdef HAVE_NAMESPACES
  idhdl h =namespaceroot->get(n,myynest, TRUE); // search in toplevel namespace
  idhdl h2=namespaceroot->get(n,myynest); // search in current namespace
  idhdl h3=NULL;
  if ((currRing!=NULL) && ((h2==NULL)||(IDLEV(h2)!=myynest)))
  {
    h3 = currRing->idroot->get(n,myynest);
  }
  if (h3==NULL) {
    if (h2!=NULL) {
      *packhdl=namespaceroot->get(namespaceroot->name,0, TRUE);
      return h2;
    }
    if(!local) {
      if(h!=NULL)*packhdl=namespaceroot->get("Top",0, TRUE);
      return h;
    }
  }
  if(h3!=NULL) *packhdl = currRingHdl;
  else *packhdl = NULL;
  return h3;
#else /* HAVE_NAMESPACES */
  idhdl h = IDROOT->get(n,myynest);
  idhdl h2=NULL;
  *packhdl = NULL;
  if ((currRing!=NULL) && ((h==NULL)||(IDLEV(h)!=myynest)))
  {
    h2 = currRing->idroot->get(n,myynest);
  }
  if (h2==NULL) return h;
  return h2;
#endif /* HAVE_NAMESPACES */
}

idhdl ggetid(const char *n, BOOLEAN local)
{
#ifdef HAVE_NAMESPACES
  idhdl h =namespaceroot->get(n,myynest, TRUE); // search in toplevel namespace
  idhdl h2=namespaceroot->get(n,myynest); // search in current namespace
  idhdl h3=NULL;
  if ((currRing!=NULL) && ((h2==NULL)||(IDLEV(h2)!=myynest)))
  {
    h3 = currRing->idroot->get(n,myynest);
  }
  if (h3==NULL)
  {
    if (h2!=NULL) return h2;
    if(!local) return h;
  }
  return h3;
#else /* HAVE_NAMESPACES */
  idhdl h = IDROOT->get(n,myynest);
  if ((h!=NULL)&&(IDLEV(h)==myynest)) return h;
  idhdl h2=NULL;
  if (currRing!=NULL)
  {
    h2 = currRing->idroot->get(n,myynest);
  }
  if (h2!=NULL) return h2;
  if (h!=NULL) return h;
#ifdef HAVE_NS  
  if (basePack!=currPack)
    return basePack->idroot->get(n,myynest);
#endif      
  return NULL; 
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
#ifdef HAVE_NS
      if (ipSwapId(tomove,IDROOT,currRing->idroot))
      ipSwapId(tomove,basePack->idroot,currRing->idroot);
#else
      ipSwapId(tomove,IDROOT,currRing->idroot);
#endif
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
    if( pack->language == LANG_C)
    {
      Print("//dlclose(%s)\n",pack->libname);
#ifdef HAVE_DYNAMIC_LOADING
      extern int dynl_close (void *handle);
      dynl_close (pack->handle);
#endif /* HAVE_DYNAMIC_LOADING */
    }
    omFree((ADDRESS)pack->libname);
    memset((void *) pack, 0, sizeof(sip_package));
    pack->language=LANG_NONE;
  }
}

#ifdef HAVE_NAMESPACES
BOOLEAN paKill(package pack, BOOLEAN force_top)
{
  if (pack->ref <= 0 || force_top) {
    idhdl hdh = pack->idroot;
    idhdl temp;
    while (hdh!=NULL)
    {
      temp = IDNEXT(hdh);
      if((IDTYP(hdh)!=PACKAGE_CMD) ||
         (IDTYP(hdh)==PACKAGE_CMD && IDPACKAGE(hdh)->language!=LANG_TOP) ||
         (IDTYP(hdh)==PACKAGE_CMD && IDPACKAGE(hdh)->language==LANG_TOP &&
         IDPACKAGE(hdh)->ref>0 ))
        killhdl2(hdh,&(pack->idroot));
      hdh = temp;
    }
    if(checkPackage(pack))
    {
      paCleanUp(pack);
      omFreeBin((ADDRESS)pack,  sip_package_bin);
    }
    else return FALSE;
  }
  else paCleanUp(pack);
  return TRUE;
}
#endif /* HAVE_NAMESPACES */

char *idhdl2id(idhdl pck, idhdl h)
{
  char *name = (char *)omAlloc(strlen(pck->id) + strlen(h->id) + 3);
  sprintf(name, "%s::%s", pck->id, h->id);
  return(name);
}

void iiname2hdl(const char *name, idhdl *pck, idhdl *h)
{
  const char *q = strchr(name, ':');
  char *p, *i;

  if(q==NULL)
  {
    p = omStrDup("");
    i = (char *)omAlloc(strlen(name)+1);
    *i = '\0';
    sscanf(name, "%s", i);
#ifdef HAVE_NAMESPACES
    *h = namespaceroot->get(i, myynest);
    if(*h == NULL) { *h = namespaceroot->get(i, myynest, TRUE); }
#else /* HAVE_NAMESPACES */
#endif /* HAVE_NAMESPACES */
  }
  else {
    if( *(q+1) != ':') return;
    i = (char *)omAlloc(strlen(name)+1);
    *i = '\0';
    if(name == q)
    {
      p = omStrDup("");
      sscanf(name, "::%s", i);
#ifdef HAVE_NAMESPACES
      *h =namespaceroot->get(i, myynest, TRUE); // search in toplevel namespace
#else /* HAVE_NAMESPACES */
#endif /* HAVE_NAMESPACES */
    }
    else
    {
      p = (char *)omAlloc(strlen(name)+1);
      sscanf(name, "%[^:]::%s", p, i);
#ifdef HAVE_NAMESPACES
      *pck =namespaceroot->get(p, myynest, TRUE); // search in toplevel namespace
      if((*pck!=NULL)&&(IDTYP(*pck)==PACKAGE_CMD))
      {
        namespaceroot->push(IDPACKAGE(*pck), IDID(*pck));
        *h =namespaceroot->get(i, myynest); // search in toplevel namespace
        namespaceroot->pop();
      }
#else /* HAVE_NAMESPACES */
#endif /* HAVE_NAMESPACES */
    }
  }
  //printf("Package: '%s'\n", p);
  //printf("Id Rec : '%s'\n", i);
  omFree(p);
  omFree(i);
}

#if 0
char *getnamelev()
{
  char buf[256];
  sprintf(buf, "(%s:%d)", namespaceroot->name,namespaceroot->lev);
  return(buf);
}
// warning: address of local variable `buf' returned
#endif

void proclevel::push(char *n)
{
  //Print("push %s\n",n);
  proclevel *p=(proclevel*)omAlloc0(sizeof(proclevel));
  //p->currRing=::currRing;
  //p->currRingHdl=::currRingHdl;
  p->name=n;
  #ifdef HAVE_NS
  p->currPackHdl=::currPackHdl;
  p->currPack=::currPack;
  #endif
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
  #ifdef HAVE_NS
  //Print("restore pack=%s,1.obj=%s\n",IDID(currPackHdl),IDID(currPack->idroot));
  ::currPackHdl=this->currPackHdl;
  ::currPack=this->currPack;
  #endif
  proclevel *p=this;
  procstack=next;
  omFreeSize(p,sizeof(proclevel));
}
#ifdef HAVE_NAMESPACES
namehdl namerec::push(package pack, char *name, int nesting, BOOLEAN init)
{
  //printf("PUSH: put entry (%s) on stack\n", name);
  namehdl ns = (namerec *)omAlloc0Bin(namerec_bin);
  extern int myynest;
  if(nesting<0) nesting = myynest;
  ns->next   = this;
  if(this==NULL && !init)
  {
    printf("PUSH: this is NULL and init not set.\n");
    init=TRUE;
  }
  if(init)
  {
    ns->next    = NULL;
    ns->pack    = (ip_package *)omAlloc0Bin(ip_package_bin);
    ns->isroot  = TRUE;
    ns->lev     = 1;
    //ns->myynest = 0;
  }
  else
  {
    extern ring currRing;
    ns->pack   = pack;
    ns->lev    = this->lev+1;
    //ns->myynest = myynest+1;
    this->currRing = currRing;
    //printf("Saving Ring %x, %x\n", this->currRing, currRing);
  }
  ns->name    = omStrDup(name);
  ns->myynest = nesting;

  //ns->currRing = currRing;
  //ns->currRingHdl = currRingHdl;
  if(ns->isroot) ns->root=ns; else ns->root = this->root;
  namespaceroot = ns;
#if 0
  if(init && ns->isroot) {
    idhdl pl = enterid( "Top",0, PACKAGE_CMD,
                      &NSROOT(namespaceroot), TRUE );
    if(pl != NULL) {
      omFreeBin((ADDRESS)IDPACKAGE(pl),  ip_package_bin);
      IDPACKAGE(pl) = ns->pack;
    }
  }
#endif
  //Print("NSPUSH: done\n");
  return(namespaceroot);
}
#endif /* HAVE_NAMESPACES */

#ifdef HAVE_NAMESPACES
namehdl namerec::pop(BOOLEAN change_nesting)
{
  namehdl ns;
  //printf("POP: remove entry (%s)\n", this->name);
  if(this==NULL) return NULL;
  if(isroot) {
    //printf("POP: base. woul'd do it.\n");
    return this;
  }
  if(!change_nesting && this->myynest!=this->next->myynest) {
    return(this);
  }
  ns = this;
  namespaceroot = this->next;
  omFree((ADDRESS)ns->name);
  omFreeBin((ADDRESS)ns,  namerec_bin);
  return(namespaceroot);
}
#endif /* HAVE_NAMESPACES */

#ifdef HAVE_NAMESPACES
idhdl namerec::get(const char * s, int lev, BOOLEAN root)
{
  namehdl ns;
  if(root)
  {
    ns = this->root;
  }
  else
  {
    ns = this;
  }
  //printf("//====== namerec::get(%s, %d) from '%s'\n", s, lev, ns->name);
  if(ns==NULL)
  {
    //printf("//======== namerec::get() from null\n");
    return NULL;
  }
  return( NSROOT(ns)->get(s, lev));
}

BOOLEAN checkPackage(package pack)
{
  namehdl nshdl = namespaceroot;

  for(nshdl=namespaceroot; nshdl->isroot != TRUE; nshdl = nshdl->next) {
    //Print("NSstack: %s:%d, nesting=%d\n", nshdl->name, nshdl->lev, nshdl->myynest);
    if (nshdl->pack==pack)
    {
      Warn("package '%s' still in use on level %d",nshdl->name, nshdl->lev);
      return FALSE;
    }
  }
  if (nshdl->pack==pack)
  {
    //Print("NSstack: %s:%d, nesting=%d\n", nshdl->name, nshdl->lev, nshdl->myynest);
    Warn("package '%s' still in use on level %d",nshdl->name, nshdl->lev);
    return FALSE;
  }
  return TRUE;

}
#endif /* HAVE_NAMESPACES */

#ifdef HAVE_NS
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
#endif
