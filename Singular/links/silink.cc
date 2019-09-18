/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/

/*
* ABSTRACT: general interface to links
*/

#include "kernel/mod2.h"

#include "misc/options.h"
#include "misc/intvec.h"
#include "reporter/si_signals.h"
#include "coeffs/numbers.h"

#include "polys/matpol.h"
#include "polys/monomials/ring.h"

#include "kernel/ideals.h"

#include "Singular/lists.h"
#include "Singular/cntrlc.h"
#include "Singular/links/ssiLink.h"
#include "Singular/links/pipeLink.h"
#include "Singular/tok.h"
#include "Singular/subexpr.h"
#include "Singular/ipid.h"
#include "Singular/links/silink.h"
#include "Singular/links/slInit.h"
#include "Singular/ipshell.h"
#include "feOpt.h"

// #ifdef HAVE_DBM
// #ifdef __CYGWIN__
// #define USE_GDBM
// #endif
// #endif

THREAD_VAR omBin s_si_link_extension_bin = omGetSpecBin(sizeof(s_si_link_extension));
THREAD_VAR omBin sip_link_bin = omGetSpecBin(sizeof(sip_link));
THREAD_VAR omBin ip_link_bin = omGetSpecBin(sizeof(ip_link));

/* ====================================================================== */
static si_link_extension slTypeInit(si_link_extension s, const char* type);
THREAD_VAR si_link_extension si_link_root=NULL;

BOOLEAN slInit(si_link l, char *istr)
{
  char *type = NULL, *mode = NULL, *name = NULL;
  int i = 0, j;

  // set mode and type
  if (istr != NULL)
  {
    // find the first colon char in istr
    i = 0;
    while (istr[i] != ':' && istr[i] != '\0') i++;
    if (istr[i] == ':')
    {
      // if found, set type
      if (i > 0)
      {
        istr[i] = '\0';
        type = omStrDup(istr);
        istr[i] = ':';
      }
      // and check for mode
      j = ++i;
      while (istr[j] != ' ' && istr[j] != '\0') j++;
      if (j > i)
      {
        mode = omStrDup(&(istr[i]));
        mode[j - i] = '\0';
      }
      // and for the name
      while (istr[j] == ' '&& istr[j] != '\0') j++;
      if (istr[j] != '\0') name = omStrDup(&(istr[j]));
    }
    else // no colon find -- string is entire name
    {
      j=0;
      while (istr[j] == ' '&& istr[j] != '\0') j++;
      if (istr[j] != '\0') name = omStrDup(&(istr[j]));
    }
  }

  // set the link extension
  if (type != NULL)
  {
    si_link_extension s = si_link_root;
    si_link_extension prev = s;

    while (strcmp(s->type, type) != 0)
    {
      if (s->next == NULL)
      {
        prev = s;
        s = NULL;
        break;
      }
      else
      {
        s = s->next;
      }
    }

    if (s != NULL)
      l->m = s;
    else
    {
      l->m = slTypeInit(prev, type);
    }
    omFree(type);
  }
  else
    l->m = si_link_root;

  if (l->m == NULL) return TRUE;

  l->name = (name != NULL ? name : omStrDup(""));
  l->mode = (mode != NULL ? mode : omStrDup(""));
  l->ref = 1;
  return FALSE;
}

void slCleanUp(si_link l)
{
  defer_shutdown++;
  (l->ref)--;
  if (l->ref == 0)
  {
    if (SI_LINK_OPEN_P(l))
    {
      if (l->m->Close != NULL) l->m->Close(l);
    }
    if ((l->data != NULL) && (l->m->Kill != NULL)) l->m->Kill(l);
    omFree((ADDRESS)l->name);
    omFree((ADDRESS)l->mode);
    memset((void *) l, 0, sizeof(ip_link));
  }
  defer_shutdown--;
  if (!defer_shutdown && do_shutdown) m2_end(1);
}

void slKill(si_link l)
{
  defer_shutdown++;
  slCleanUp(l);
  if ((l!=NULL) &&(l->ref == 0))
    omFreeBin((ADDRESS)l,  ip_link_bin);
  defer_shutdown--;
  if (!defer_shutdown && do_shutdown) m2_end(1);
}

const char* slStatus(si_link l, const char *request)
{
  if (l == NULL) return "empty link";
  else if (l->m == NULL) return "unknown link type";
  else if (strcmp(request, "type") == 0) return l->m->type;
  else if (strcmp(request, "mode") == 0) return l->mode;
  else if (strcmp(request, "name") == 0) return l->name;
  else if (strcmp(request, "exists") ==0)
  {
    struct stat buf;
    if (si_lstat(l->name,&buf)==0) return "yes";
    else return "no";
  }
  else if (strcmp(request, "open") == 0)
  {
    if (SI_LINK_OPEN_P(l)) return "yes";
    else return "no";
  }
  else if (strcmp(request, "openread") == 0)
  {
    if (SI_LINK_R_OPEN_P(l)) return "yes";
    else return "no";
  }
  else if (strcmp(request, "openwrite") == 0)
  {
    if (SI_LINK_W_OPEN_P(l)) return "yes";
    else return "no";
  }
  else if (l->m->Status == NULL) return "unknown status request";
  else return l->m->Status(l, request);
}

//--------------------------------------------------------------------------
BOOLEAN slSetRingDummy(si_link, ring r, BOOLEAN)
{
  if (currRing!=r) rChangeCurrRing(r);
  return FALSE;
}
BOOLEAN slOpen(si_link l, short flag, leftv h)
{
  BOOLEAN res = TRUE;
  if (l!=NULL)
  {

    if (l->m == NULL) slInit(l, ((char*)""));

    if (feOptValue(FE_OPT_NO_SHELL)) {WerrorS("no links allowed");return TRUE;}

    const char *c="_";;
    if (h!=NULL) c=h->Name();

    if (SI_LINK_OPEN_P(l))
    {
      Warn("open: link of type: %s, mode: %s, name: %s is already open",
         l->m->type, l->mode, l->name);
      return FALSE;
    }
    else if (l->m->Open != NULL)
    {
      res = l->m->Open(l, flag, h);
      if (res)
        Werror("open: Error for link %s of type: %s, mode: %s, name: %s",
             c, l->m->type, l->mode, l->name);
    }
    if (l->m->SetRing==NULL) l->m->SetRing=slSetRingDummy;
  }
  return res;
}

BOOLEAN slPrepClose(si_link l)
{

  if(! SI_LINK_OPEN_P(l))
    return FALSE;

  BOOLEAN res = TRUE;
  if (l->m->PrepClose != NULL)
  {
    res = l->m->PrepClose(l);
    if (res)
      Werror("close: Error for link of type: %s, mode: %s, name: %s",
           l->m->type, l->mode, l->name);
  }
  return res;
}

BOOLEAN slClose(si_link l)
{

  if(! SI_LINK_OPEN_P(l))
    return FALSE;

  defer_shutdown++;
  BOOLEAN res = TRUE;
  if (l->m->Close != NULL)
  {
    res = l->m->Close(l);
    if (res)
      Werror("close: Error for link of type: %s, mode: %s, name: %s",
           l->m->type, l->mode, l->name);
  }
  defer_shutdown--;
  if (!defer_shutdown && do_shutdown) m2_end(1);
  SI_LINK_SET_CLOSE_P(l);
  return res;
}

leftv slRead(si_link l, leftv a)
{
  leftv v = NULL;
  if( ! SI_LINK_R_OPEN_P(l)) // open r ?
  {
#ifdef HAVE_DBM
#ifdef USE_GDBM
    if (! SI_LINK_CLOSE_P(l))
      {
        if (slClose(l)) return NULL;
      }
#endif
#endif
    if (slOpen(l, SI_LINK_READ,NULL)) return NULL;
  }

  if (SI_LINK_R_OPEN_P(l))
  { // open r
    if (a==NULL)
    {
      if (l->m->Read != NULL) v = l->m->Read(l);
    }
    else
    {
      if (l->m->Read2 != NULL) v = l->m->Read2(l,a);
    }
  }
  else
  {
    Werror("read: Error to open link of type %s, mode: %s, name: %s for reading",
           l->m->type, l->mode, l->name);
    return NULL;
  }

  // here comes the eval:
  if (v != NULL)
  {
    if (v->Eval() && !errorreported)
      WerrorS("eval: failed");
  }
  else
    Werror("read: Error for link of type %s, mode: %s, name: %s",
           l->m->type, l->mode, l->name);
  return v;
}

BOOLEAN slWrite(si_link l, leftv v)
{
  BOOLEAN res;

  if(! SI_LINK_W_OPEN_P(l)) // open w ?
  {
#ifdef HAVE_DBM
#ifdef USE_GDBM
    if (! SI_LINK_CLOSE_P(l))
      {
        if (slClose(l)) return TRUE;
      }
#endif
#endif
    if (slOpen(l, SI_LINK_WRITE,NULL)) return TRUE;
  }

  if (SI_LINK_W_OPEN_P(l))
  { // now open w
    if (l->m->Write != NULL)
      res = l->m->Write(l,v);
    else
      res = TRUE;

    if (res)
      Werror("write: Error for link of type %s, mode: %s, name: %s",
             l->m->type, l->mode, l->name);
    return res;
  }
  else
  {
    Werror("write: Error to open link of type %s, mode: %s, name: %s for writing",
           l->m->type, l->mode, l->name);
    return TRUE;
  }
}

BOOLEAN slDump(si_link l)
{
  BOOLEAN res;

  if(! SI_LINK_W_OPEN_P(l)) // open w ?
  {
    if (slOpen(l, SI_LINK_WRITE,NULL)) return TRUE;
  }

  if(SI_LINK_W_OPEN_P(l))
  { // now open w
    if (l->m->Dump != NULL)
      res = l->m->Dump(l);
    else
      res = TRUE;

    if (res)
      Werror("dump: Error for link of type %s, mode: %s, name: %s",
             l->m->type, l->mode, l->name);
    if (!SI_LINK_R_OPEN_P(l)) slClose(l); // do not close r/w links
    return res;
  }
  else
  {
    Werror("dump: Error to open link of type %s, mode: %s, name: %s for writing",
           l->m->type, l->mode, l->name);
    return TRUE;
  }
}

BOOLEAN slGetDump(si_link l)
{
  BOOLEAN res;

  if(! SI_LINK_R_OPEN_P(l)) // open r ?
  {
    if (slOpen(l, SI_LINK_READ,NULL)) return TRUE;
  }

  if(SI_LINK_R_OPEN_P(l))
  { // now open r
    if (l->m->GetDump != NULL)
      res = l->m->GetDump(l);
    else
      res = TRUE;

    if (res)
      Werror("getdump: Error for link of type %s, mode: %s, name: %s",
             l->m->type, l->mode, l->name);
    //res|=slClose(l);
    return res;
  }
  else
  {
    Werror("dump: Error open link of type %s, mode: %s, name: %s for reading",
           l->m->type, l->mode, l->name);
    return TRUE;
  }
}

/*------------Initialization at Start-up time------------------------*/


static si_link_extension slTypeInit(si_link_extension s, const char* type)
{
  assume(s != NULL);
  s->next = NULL;
  si_link_extension ns = (si_link_extension)omAlloc0Bin(s_si_link_extension_bin);

  if (0)
    ; // dummy
#ifdef HAVE_DBM
  else if (strcmp(type, "DBM") == 0)
    s->next = slInitDBMExtension(ns);
#endif
#if 1
  else if (strcmp(type, "ssi") == 0)
    s->next = slInitSsiExtension(ns);
#endif
#if 1
  else if (strcmp(type, "|") == 0)
    s->next = slInitPipeExtension(ns);
#endif
  else
  {
    Warn("Found unknown link type: %s", type);
    Warn("Use default link type: %s", si_link_root->type);
    omFreeBin(ns, s_si_link_extension_bin);
    return si_link_root;
  }

  if (s->next == NULL)
  {
    Werror("Can not initialize link type %s", type);
    omFreeBin(ns, s_si_link_extension_bin);
    return NULL;
  }
  return s->next;
}

