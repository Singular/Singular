/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/* $Id: ipid.cc,v 1.65 2002-12-13 16:20:05 Singular Exp $ */

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

proclevel *procstack=NULL;
#define TEST
idhdl idroot = NULL;

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

/*0 implementation*/

idhdl idrec::get(const char * s, int lev)
{
  assume(s!=NULL);
  assume((lev>=0) && (lev<1000)); //not really, but if it isnt in that bounds..
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
      if (s==IDID(h)) IDID(h)=NULL;
      killhdl2(h,root,currRing);
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
        killhdl2(h,&IDROOT,NULL);
      }
      else
        goto errlabel;
      }
    }
  }
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
        killhdl2(h,&currRing->idroot,currRing);
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

void killhdl(idhdl h)
{
  int t=IDTYP(h);
  if ((BEGIN_RING<t) && (t<END_RING) && (t!=QRING_CMD))
    killhdl2(h,&currRing->idroot,currRing);
  else
  {
#ifdef HAVE_NS
    if(t==PACKAGE_CMD)
    {
      killhdl2(h,&(basePack->idroot),NULL);
    }
    else
    {
      idhdl s=currPack->idroot;
      while ((s!=h) && (s!=NULL)) s=s->next;
      if (s!=NULL)
        killhdl2(h,&(currPack->idroot),NULL);
      else if (basePack!=currPack)
      {
        idhdl s=basePack->idroot;
        while ((s!=h) && (s!=NULL)) s=s->next;
        if (s!=NULL)
          killhdl2(h,&(basePack->idroot),currRing);
        else
          killhdl2(h,&(currRing->idroot),currRing);
       }
    }
#else /* HAVE_NS */
    {
      idhdl s=IDROOT;
      while ((s!=h) && (s!=NULL)) s=s->next;
      if (s==NULL) killhdl2(h,&(currRing->idroot),currRing);
      else killhdl2(h,&IDROOT,currRing);
    }
#endif /* HAVE_NS */
  }
}

#ifdef HAVE_NS
void killhdl(idhdl h, package proot)
{
  int t=IDTYP(h);
  if ((BEGIN_RING<t) && (t<END_RING) && (t!=QRING_CMD))
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
#endif /* HAVE_NS */

void killhdl2(idhdl h, idhdl * ih, ring r)
{
  //printf("kill %s, id %x, typ %d lev: %d\n",IDID(h),(int)IDID(h),IDTYP(h),IDLEV(h));
  idhdl hh;

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
    // any objects defined for this ring ?
    // Hmm ... why only for rings and not for qrings??
    // if (((IDTYP(h)==RING_CMD) && (IDRING(h)->ref<=0))
    if ((IDRING(h)->ref<=0)  &&  (IDRING(h)->idroot!=NULL))
    {
      idhdl * hd = &IDRING(h)->idroot;
      idhdl  hdh = IDNEXT(*hd);
      idhdl  temp;
      if (IDRING(h)==currRing) //we are not killing the base ring, so switch
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
        killhdl2(hdh,&(IDRING(h)->idroot),IDRING(h));
        hdh = temp;
      }
      killhdl2(*hd,hd,IDRING(h));
    }
    rKill(h);
  }
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
        killhdl2(hdh,&(IDPACKAGE(h)->idroot),NULL);
        hdh = temp;
      }
      killhdl2(*hd,hd,NULL);
    }
    paKill(IDPACKAGE(h));
    if (currPackHdl==h) currPackHdl=packFindHdl(currPack);
    iiCheckPack(currPack);
  }
#endif /* HAVE_NS */
  // poly / vector -------------------------------------------------------
  else if ((IDTYP(h) == POLY_CMD) || (IDTYP(h) == VECTOR_CMD))
  {
    assume(r!=NULL);
    p_Delete(&IDPOLY(h),r);
  }
  // ideal / module/ matrix / map ----------------------------------------
  else if ((IDTYP(h) == IDEAL_CMD)
           || (IDTYP(h) == MODUL_CMD)
           || (IDTYP(h) == MATRIX_CMD)
           || (IDTYP(h) == MAP_CMD))
  {
    assume(r!=NULL);
    ideal iid = IDIDEAL(h);
    if (IDTYP(h) == MAP_CMD)
    {
      map im = IDMAP(h);
      omFree((ADDRESS)im->preimage);
    }
    id_Delete(&iid,r);
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
    assume(r!=NULL);
    n_Delete(&IDNUMBER(h),r);
  }
  // intvec / intmat  ---------------------------------------------------
  else if ((IDTYP(h) == INTVEC_CMD)||(IDTYP(h) == INTMAT_CMD))
  {
    delete IDINTVEC(h);
  }
  // list  -------------------------------------------------------------
  else if (IDTYP(h)==LIST_CMD)
  {
    IDLIST(h)->Clean(r);
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
    assume(r!=NULL);
    if (IDDATA(h)!=NULL)
      syKillComputation((syStrategy)IDDATA(h),r);
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

idhdl ggetid(const char *n, BOOLEAN local)
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
#ifdef HAVE_NS  
  if (basePack!=currPack)
    return basePack->idroot->get(n,myynest);
#endif      
  return NULL; 
}

void ipListFlag(idhdl h)
{
  if (hasFlag(h,FLAG_STD)) PrintS(" (SB)");
  if (hasFlag(h,FLAG_DRING)) PrintS(" (D)");
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
    omfree((ADDRESS)pack->libname);
    memset((void *) pack, 0, sizeof(sip_package));
    pack->language=LANG_NONE;
  }
}

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
  }
  else {
    if( *(q+1) != ':') return;
    i = (char *)omAlloc(strlen(name)+1);
    *i = '\0';
    if(name == q)
    {
      p = omStrDup("");
      sscanf(name, "::%s", i);
    }
    else
    {
      p = (char *)omAlloc(strlen(name)+1);
      sscanf(name, "%[^:]::%s", p, i);
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
  p->cRing=currRing;
  p->cRingHdl=currRingHdl;
  p->name=n;
  #ifdef HAVE_NS
  p->cPackHdl=currPackHdl;
  p->cPack=currPack;
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
  currPackHdl=this->cPackHdl;
  currPack=this->cPack;
  iiCheckPack(currPack);
  #endif
  proclevel *p=this;
  procstack=next;
  omFreeSize(p,sizeof(proclevel));
}

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
