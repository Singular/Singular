/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/*
* ABSTRACT
*/
/* $Log: not supported by cvs2svn $
// Revision 1.2  1997/03/20  16:59:58  obachman
// Various minor bug-fixes in mpsr interface
//
// Thu Mar 20 11:57:00 1997  Olaf Bachmann  <obachman@schlupp.mathematik.uni-kl.de (Olaf Bachmann)>
//
// 	* sing_mp.cc (slInitBatchLink): initialized silink such that
// 	  l->argv[0] == "MP:connect" (otherwise, slInitMP failed)
//
// Wed Mar 19 15:38:08 1997  Olaf Bachmann  <obachman@schlupp.mathematik.uni-kl.de (Olaf Bachmann)>
//
// 	* hannes fixed maFindPerm to reflect new names <->parameter scheme
//
// 	* sing_mp.cc (mpsr_IsMPLink): fixed it
//
// 	* Makefile (tags): added target tags
//
// Revision 1.1.1.1  1997/03/19  13:18:41  obachman
// Imported Singular sources
//
*/

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "mod2.h"
#include "tok.h"
#include "mmemory.h"
#include "febase.h"
#include "subexpr.h"
#include "ipid.h"
#include "silink.h"


/* =============== general utilities ====================================== */
void FreeCmdArgs(int argc, char** argv)
{
  int i;
  for (i=0; i<argc; i++)
    FreeL(argv[i]);

  Free(argv, argc*sizeof(char *));
}
/*-------------------------------------------------------------------*/
#ifdef HAVE_MPSR
/***************************************************************
 *
 * MP stuff
 *
 ***************************************************************/
#include"mpsr.h"

#ifdef MPSR_DEBUG
#define MP_SET_LINK_OPTIONS(link) \
if (link != NULL) MP_SetLinkOption(link,MP_LINK_LOG_MASK_OPT,MP_LOG_ALL_EVENTS)
#else
#define MP_SET_LINK_OPTIONS(link) ((void *) 0)
#endif

MP_Env_pt mp_Env = NULL;

void slInitBatchLink(si_link l, int argc, char** argv)
{
  int i;
  idhdl id;
  l->argv = (char **) Alloc0((argc +1) * sizeof(char *));
  l->argv[0] = mstrdup("MP:connect");
  for (i=1; i<=argc; i++)
    l->argv[i] = mstrdup(argv[i]);
  l->argc = argc + 1;
  l->name = mstrdup("MP:connect");
  slInit(l,NULL);
  id = enterid(mstrdup("mp_ll"), 0, LINK_CMD, &idroot, FALSE);
  IDLINK(id) = l;
}

BOOLEAN slOpenWriteMPFile(si_link l)
{
  char **argv;
  int argc;

  GetCmdArgs(&argc, &argv, "-MPtransp FILE -MPmode write -MPfile /tmp/mpout");
  MP_Link_pt link = NULL;
  short mode = 0; // 0 -- write, 1 -- append, 2 -- read
  BOOLEAN status = TRUE;
  
  if (l->argc > 2)
  {
    if (strcmp(l->argv[2], "mode:a") == 0)
    {
      mode = 1;
      FreeL(argv[3]);
      argv[3] = mstrdup("append");
    }
    else if (strcmp(l->argv[2], "mode:r") == 0)
    {
      mode = 2;
      FreeL(argv[3]);
      argv[3] = mstrdup("read");
    }
  }
        
  if (mode < 2 && SI_LINK_R_OPEN_P(l))
  {
    Werror("can not open an MP write link to file %s as read link also",
           l->name);
    status = FALSE;
  }
  else if (mode == 2 && SI_LINK_W_OPEN_P(l))
  {
   Werror("can not open an MP read link to file %s as write link also",
           l->name);
   status = FALSE;
  }

  if (status)
  {
    if (l->name != NULL)
    {
      FreeL(argv[5]);
      argv[5] = mstrdup(l->name);
    }

    if (mp_Env == NULL)
      mp_Env = MP_InitializeEnv(MP_AllocateEnv());

    if (mp_Env == NULL)
    {
      Werror("Initialization of MP Environment");
      status = FALSE;
    }
    else
    {
      if ((link = MP_OpenLink(mp_Env, argc, argv)) == NULL)
      {
        Werror("Opening of MP Write to file %s", l->name);
        status = FALSE;
      }
      else
        MP_SET_LINK_OPTIONS(link);
    }
  }
  
  FreeCmdArgs(argc, argv);
  if (status)
  {
    if (mode < 2) 
      SI_LINK_SET_W_OPEN_P(l);
    else
      SI_LINK_SET_R_OPEN_P(l);
    
    l->data = (void *) link;
    return FALSE;
  }
  else
    return TRUE;
}

MP_Link_pt OpenMPFile(char *fn, short mode)
{
  char **argv;
  int argc;
  MP_Link_pt link = NULL;

  GetCmdArgs(&argc, &argv, "-MPtransp FILE -MPmode write -MPfile /tmp/mpout");

  if (fn != NULL)
  {
    FreeL(argv[5]);
    argv[5] = mstrdup(fn);
  }

  if (mode)
  {
    FreeL(argv[3]);
    argv[3] = mstrdup("read");
  }
  
  if (mp_Env == NULL)
    mp_Env = MP_InitializeEnv(MP_AllocateEnv());

  if (mp_Env == NULL)
  {
    Werror("Initialization of MP Environment");
    FreeCmdArgs(argc, argv);
    return NULL;
  }

  if ((link = MP_OpenLink(mp_Env, argc, argv)) == NULL)
  {
    Werror("Opening of MP file %s for %s", fn, (mode ? "writing" : "reading"));
  }
  MP_SET_LINK_OPTIONS(link);
  
  FreeCmdArgs(argc, argv);
  return link;
}

  
BOOLEAN slOpenReadMPFile(si_link l)
{
  if (SI_LINK_W_OPEN_P(l))
  {
    Werror("can not open an MP write link to file %s as read link also",
           l->name);
    return TRUE;
  }

  char argvstr[] = "-MPtransp FILE -MPmode read  -MPfile /tmp/mpout";
  char **argv;
  int argc;
  MP_Link_pt link = NULL;

  GetCmdArgs(&argc, &argv, argvstr);
  if (l->name != NULL)
  {
    FreeL(argv[5]);
    argv[5] = mstrdup(l->name);
  }
  if (mp_Env == NULL)
    mp_Env = MP_InitializeEnv(MP_AllocateEnv());

  if (mp_Env == NULL)
  {
    Werror("Initialization of MP Environment");
    return TRUE;
  }

  if ((link = MP_OpenLink(mp_Env, 6, argv)) == NULL)
  {
    Werror("Opening of MP Read to file %s", l->name);
    FreeCmdArgs(argc, argv);
    return TRUE;
  }
  MP_SET_LINK_OPTIONS(link);

  FreeCmdArgs(argc, argv);
  SI_LINK_SET_R_OPEN_P(l);
  l->data = (void *) link;
  return FALSE;
}

MP_Link_pt slOpenMPConnect(si_link l)
{
  char argvstr[] = "-MPtransp TCP -MPmode connect -MPport 1025 -MPhost localhost";
  char *port = IMP_GetCmdlineArg(l->argc, l->argv, "-MPport");
  char *host = IMP_GetCmdlineArg(l->argc, l->argv, "-MPhost");
  char **argv;
  int argc;
  MP_Link_pt link = NULL;

  GetCmdArgs(&argc, &argv, argvstr);

  if (port == NULL)
    Warn("No port number specified for MP:connect; We try 1025");
  else
  {
    FreeL(argv[5]);
    argv[5] = mstrdup(port);
  }

  if (host == NULL)
  {
    char *hn = (char *) AllocL(64*sizeof(char*));
    FreeL(argv[7]);
    gethostname(hn, 64);
    argv[7] = hn;
    Warn("No host specified for MP:connect; We try %s", hn);
  }
  else
  {
    FreeL(argv[7]);
    argv[7] = mstrdup(host);
  }

  link = MP_OpenLink(mp_Env, 8, argv);
  MP_SET_LINK_OPTIONS(link);

  FreeCmdArgs(argc, argv);

  return link;
}

MP_Link_pt slOpenMPListen(si_link l)
{
  char argvstr[] = "-MPtransp TCP -MPmode listen -MPport 1025";
  char *port = IMP_GetCmdlineArg(l->argc, l->argv, "-MPport");
  char **argv;
  int argc;
  MP_Link_pt link;

  GetCmdArgs(&argc, &argv, argvstr);

  if (port == NULL)
    Warn("No port number specified for MP:listen; We try 1025");
  else
  {
    FreeL(argv[5]);
    argv[5] = mstrdup(port);
  }
  link = MP_OpenLink(mp_Env, 6, argv);
  MP_SET_LINK_OPTIONS(link);

  FreeCmdArgs(argc, argv);
  return link;
}

MP_Link_pt slOpenMPLaunch(si_link l)
{
  char *host = IMP_GetCmdlineArg(l->argc, l->argv, "-MPhost");
  char *app  = IMP_GetCmdlineArg(l->argc, l->argv, "-MPapplication");
  char *appargs = NULL;
  char **argv;
  int argc;
  MP_Link_pt link=NULL;

  GetCmdArgs(&argc, &argv,
             "-MPtransp TCP -MPmode launch -MPhost localhost -MPapplication s");

  FreeL(argv[5]);
  if (host == NULL)
  {
    char *hn = (char *) AllocL(64*sizeof(char*));
    FreeL(argv[5]);
    gethostname(hn, 64);
    argv[5] = hn;
    Warn("No host specified for MP:connect; We try %s", hn);
  }
  else
  {
    argv[5] = mstrdup(host);
  }

  if (app  == NULL)
  {
    Warn("No application specified for MP:launch; We try: Singular -b");
    FreeL(argv[7]);
    argv[7] = mstrdup("Singular -b");
  }
  else
  {
    FreeL(argv[7]);
    argv[7] = mstrdup(app);
  }

  link = MP_OpenLink(mp_Env, 8, argv);
  MP_SET_LINK_OPTIONS(link);

  FreeCmdArgs(argc, argv);
  return link;
}

BOOLEAN slOpenMPTcp(si_link l)
{
  MP_Link_pt link = NULL;

  if (l->argc < 1)
    return TRUE;

  if (mp_Env == NULL)
    mp_Env = MP_InitializeEnv(MP_AllocateEnv());

  if (mp_Env == NULL)
  {
    Werror("Initialization of MP Environment");
    return TRUE;
  }

  if (strcmp(l->name, "MP:launch") == 0)
    link = slOpenMPLaunch(l);
  else if (strcmp(l->name, "MP:connect") == 0)
    link = slOpenMPConnect(l);
  else if (strcmp(l->name, "MP:listen") == 0)
    link = slOpenMPListen(l);
  else
  {
    Werror("link is not MP:launch, MP:connect or MP:listen");
    return TRUE;
  }

  if (link == NULL)
  {
    Werror("Could not open link %s",l->name);
    return TRUE;
  }
  SI_LINK_SET_RW_OPEN_P(l);
  l->data = (void *) link;
  return FALSE;
}

BOOLEAN slWriteMP(si_link l, leftv v)
{
  mpsr_ClearError();
  if (mpsr_PutMsg((MP_Link_pt) l->data, v) != mpsr_Success)
  {
    mpsr_PrintError();
    return TRUE;
  }
  else
    return FALSE;
}

void SentQuitMsg(si_link l)
{
  leftv v = (leftv) Alloc0(sizeof(sleftv));

  v->rtyp = STRING_CMD;
  v->data = (void *) mstrdup("MP:Quit");
  slWriteMP(l, v);
  FreeL(v->data);
  Free(v, sizeof(sleftv));
}

BOOLEAN slCloseMP(si_link l)
{
  if (l->data != NULL)
  {
    if (strcmp(l->name, "MP:launch") == 0)
      SentQuitMsg(l);
    MP_CloseLink((MP_Link_pt) l->data);
    l->data = NULL;
    SI_LINK_SET_CLOSE_P(l);
    return FALSE;
  }
  else
    return TRUE;
}

leftv slReadMP(si_link l)
{
  leftv v = NULL;
  mpsr_ClearError();
  if (mpsr_GetMsg((MP_Link_pt) l->data, v) != mpsr_Success)
  {
    mpsr_PrintError();
    return NULL;
  }
  else
    return v;
}
BOOLEAN slInitMP(si_link l,si_link_extension s)
{
  if (strncmp(l->argv[0], s->name,3) == 0)
  {
    return FALSE;
  }
  return TRUE;
}
si_link_extension slInitMPFile()
{
  si_link_extension s=(si_link_extension)Alloc0(sizeof(*s));
  s->Init=slInitALink;
  s->OpenRead=slOpenReadMPFile;
  s->OpenWrite=slOpenWriteMPFile;
  s->Close=slCloseMP;
  s->Read=slReadMP;
  //s->Read2=NULL;
  s->Write=slWriteMP;
  s->name="MP:file";
  return s;
}
si_link_extension slInitMPTcp()
{
  si_link_extension s=(si_link_extension)Alloc0(sizeof(*s));
  s->Init=slInitMP;
  s->OpenRead=slOpenMPTcp;
  s->OpenWrite=slOpenMPTcp;
  s->Close=slCloseMP;
  s->Read=slReadMP;
  //s->Read2=NULL;
  s->Write=slWriteMP;
  s->name="MP:tcp";
  return s;
}

BOOLEAN mpsr_IsMPLink(si_link l)
{
  return strcmp(l->name, "MP") > 0;
}
#endif

#ifdef MPSR_BATCH_DEBUG
BOOLEAN stop = 1;
#endif

int Batch_do(int argc, char **argv)
{
#ifdef HAVE_MPSR
  si_link silink = (si_link) Alloc0(sizeof(sip_link));
  leftv v = NULL;

#ifdef MPSR_BATCH_DEBUG
  fprintf(stderr, "Was started with pid %d\n", getpid());
  while (stop){};
#endif
  
  // connect to a listening application
  slInitBatchLink(silink, argc, argv);
  if (slOpenMPTcp(silink))
    return 1;

  // the main read-eval-write loop
  while(1)
  {
    v = slRead(silink, v);
    if (feErrors != NULL)
    {
      if (v != NULL) v->CleanUp();
      v = mpsr_InitLeftv(STRING_CMD, (void *) feErrors);
      feErrors = NULL;
    }

    // no need to evaluate -- it is done in the read
    if (v->Typ() == STRING_CMD && (strcmp((char *)v->Data(), "MP:Quit") == 0))
    {
      slKill(silink);
      return 0;
    }

    slWriteMP(silink, v);

    if (v != NULL)
    {
      v->CleanUp();
      Free(v, sizeof(sleftv));
      v = NULL;
    }
  }
  // should never get here
  return 1;
#else
  printf("Option -b not supported in this version\n");
  return 0;
#endif
}
