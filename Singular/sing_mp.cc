/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/* $Id: sing_mp.cc,v 1.17 1998-05-25 21:28:32 obachman Exp $ */

/*
* ABSTRACT: interface to MP links
*/

#include "mod2.h"

#ifdef HAVE_MPSR

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "tok.h"
#include "mmemory.h"
#include "febase.h"
#include "subexpr.h"
#include "ipid.h"
#include "silink.h"
#include"mpsr.h"

static int Batch_ReadEval(si_link silink);

#ifndef MAXPATHLEN
#define MAXPATHLEN 1024
#endif

#define MPSR_DEBUG

#ifdef MPSR_DEBUG
#define MP_SET_LINK_OPTIONS(link) \
  if (link != NULL) \
     MP_SetLinkOption(link,MP_LINK_LOG_MASK_OPT,MP_LOG_ALL_EVENTS)
#else
#define MP_SET_LINK_OPTIONS(link) ((void *) 0)
#endif

MP_Env_pt mp_Env = NULL;

/* =============== general utilities ====================================== */
static void FreeCmdArgs(int argc, char** argv)
{
  int i;
  for (i=0; i<argc; i++)
    FreeL(argv[i]);

  Free(argv, argc*sizeof(char *));
}

static void GetCmdArgs(int *argc, char ***argv, char *str)
{
  if (str == NULL || str[0] == '\0')
  {
    *argc = 0;
    *argv = NULL;
  }
  else
  {
    int i = 0, sl = strlen(str)+1, j;
    char *s2=mstrdup(str);

    char *appl = strstr(s2, "--MPapplication");
    if (appl != NULL)
    {
      if (appl != s2) *(appl-1) = '\0';
      i = 2;
    }

    if (appl != s2)
    {
      if (strtok(s2, " ") != NULL)
      {
        i++;
        while (strtok(NULL," ") != NULL) i++;
      }
    }
    
    *argc = i;
    if (i>0)
    {
      *argv = (char **) Alloc0(i*sizeof(char *));
      if (appl != NULL) i -= 2;
      if (i>0)
      {
        strcpy(s2,str);
        *argv[0] = mstrdup(strtok(s2, " "));
        for(j = 1; j <i; j++)
          (*argv)[j] = mstrdup(strtok(NULL, " "));
      }
    }
    else
      *argv = NULL;

    if (appl != NULL)
    {
      (*argv)[*argc -2] = mstrdup("--MPapplication");
      (*argv)[*argc -1] = mstrdup(&(appl[15]));
    }

    FreeL(s2);
  }
  
}

/***************************************************************
 *
 * MPfile  specific stuff
 *
 ***************************************************************/
static BOOLEAN slOpenMPFile(si_link l, short flag)
{
  char *argv[] = {"--MPtransp", "FILE", "--MPmode", "append",
                  "--MPfile", "/tmp/mpout"};
  char *mode;
  
  MP_Link_pt link = NULL;
  
  if (flag == SI_LINK_OPEN)
  {
   if (l->mode[0] != '\0' && (strcmp(l->mode, "r") == 0))
      flag = SI_LINK_READ;
    else flag = SI_LINK_WRITE;
  }

  if (l->name[0] != '\0') argv[5] = l->name;
  else l->name = mstrdup(argv[5]);
    

  if (flag == SI_LINK_READ)
  {
    argv[3] = "read";
    mode = "r";
  }
  else if (strcmp(l->mode, "w") == 0)
  {
    argv[3] = "write";
    mode = "w";
  }
  else
  {
    mode = "a";
  }

  if (mp_Env == NULL)
    mp_Env = MP_InitializeEnv(MP_AllocateEnv());

  if (mp_Env == NULL)
  {
    WerrorS("Open: Error in initialization of MP environment");
    return TRUE;
  }

  if ((link = MP_OpenLink(mp_Env, 6, argv)) == NULL)
    return TRUE;

  MP_SET_LINK_OPTIONS(link);
  l->data = (void *) link;
  SI_LINK_SET_OPEN_P(l, flag);
  FreeL(l->mode);
  l->mode = mstrdup(mode);
  return FALSE;
}

/***************************************************************
 *
 * MPtcp  specific stuff
 *
 ***************************************************************/
extern BOOLEAN mainGetSingOptionValue(const char* name, char** val);


static MP_Link_pt slOpenMPConnect(int n_argc, char **n_argv)
{
  char *argv[] = {"--MPtransp", "TCP", "--MPmode", "connect", "--MPport",
                  "1025",  "--MPhost", "localhost"};

  char *port = IMP_GetCmdlineArg(n_argc, n_argv, "--MPport");
  char *host = IMP_GetCmdlineArg(n_argc, n_argv, "--MPhost");

  if (port == NULL) mainGetSingOptionValue("--MPport", &port);
  if (host == NULL) mainGetSingOptionValue("--MPhost", &host);

  if (port != NULL) 
    argv[5] = port;
  if (host != NULL)
    argv[7] = host;
  else
    argv[7] = mp_Env->thishost;
    
  return MP_OpenLink(mp_Env, 8, argv);
}

static MP_Link_pt slOpenMPListen(int n_argc, char **n_argv)
{
  char *argv[] = {"--MPtransp", "TCP", "--MPmode", "listen", 
                  "--MPport", "1025"};
  char *port = IMP_GetCmdlineArg(n_argc, n_argv, "--MPport");

  if (port == NULL) mainGetSingOptionValue("--MPport", &port);

  if (port != NULL) argv[5] = port;

  return MP_OpenLink(mp_Env, 6, argv);
}

static MP_Link_pt slOpenMPLaunch(int n_argc, char **n_argv)
{
  char *argv[] = {"--MPtransp", "TCP", "--MPmode", "launch",
                  "--MPhost", "localhost", 
                  "--MPapplication", "Singular -bq  --no-stdlib --no-rc"};
  char *appl = IMP_GetCmdlineArg(n_argc, n_argv, "--MPapplication");
  char *host = IMP_GetCmdlineArg(n_argc, n_argv, "--MPhost");
  char* nappl = NULL;
  MP_Link_pt link;


  if (appl == NULL && (host == NULL || 
                       strcmp(host, "localhost") == 0))
  {
    appl = feGetExpandedExecutable();
    
    if (appl != NULL)
    {
      nappl = (char*) Alloc(MAXPATHLEN + 24);
      strcpy(nappl, appl);
      strcat(nappl, " -bq");
      appl = nappl;
    }
  }
  
  if (appl != NULL)  
    argv[7] = appl;

  if (host == NULL)
  {
    argv[5] = mp_Env->thishost;
  }
  else
    argv[5] = host;

  link = MP_OpenLink(mp_Env, 8, argv);
  if (nappl != NULL) Free(nappl, MAXPATHLEN + 24);
  return link;
}

static MP_Link_pt slOpenMPFork(si_link l, int n_argc, char **n_argv)
{
  MP_Link_pt link = NULL;
  char *argv[] = {"--MPtransp", "TCP", "--MPmode", "fork", "--MPport", "1703"};
  char *port = IMP_GetCmdlineArg(n_argc, n_argv, "--MPport");

  if (port != NULL) argv[5] = port;

  link = MP_OpenLink(mp_Env, 6, argv);    
  if (link != NULL)
  {
    if (MP_GetLinkStatus(link, MP_LinkIsParent))
    {
    /* parent's business */
      if (l->name != NULL) FreeL(l->name);
      l->name = mstrdup("parent");
      return link;
    }
    else
    {
      /* child's business -- go into batch mode */
      if (l->name != NULL) FreeL(l->name);
      l->name = mstrdup("child");
      MP_SET_LINK_OPTIONS(link);
      SI_LINK_SET_RW_OPEN_P(l);
      l->data = (void *) link;
      feBatch=TRUE;
      _exit(Batch_ReadEval(slCopy(l)));
    }
  }
  else
  {
    /* only parent can get here */
    return NULL;
  }
}

    
                                
static BOOLEAN slOpenMPTcp(si_link l, short flag)
{
  MP_Link_pt link = NULL;
  char **argv;
  int argc;
  
  GetCmdArgs(&argc, &argv, l->name);
  
  if (mp_Env == NULL)
    mp_Env = MP_InitializeEnv(MP_AllocateEnv());

  if (mp_Env == NULL)
  {
    WerrorS("Open: Error in initialization of MP environment");
    return TRUE;
  }

  if (strcmp(l->mode, "connect") == 0) link = slOpenMPConnect(argc, argv);
  else if (strcmp(l->mode, "listen") == 0) link = slOpenMPListen(argc, argv);
  else if (strcmp(l->mode, "launch") == 0) link = slOpenMPLaunch(argc, argv);
  else
  {
    if (strcmp(l->mode, "fork") != 0)
    {
      if (l->mode != NULL) FreeL(l->mode);
      l->mode = mstrdup("fork");
    }
    link = slOpenMPFork(l, argc, argv);
  }

  FreeCmdArgs(argc, argv);
  
  if (link != NULL)
  {
    MP_SET_LINK_OPTIONS(link);
    SI_LINK_SET_RW_OPEN_P(l);
    l->data = (void *) link;
    return FALSE;
  }
  else return TRUE;
}

/***************************************************************
 *
 * MP general stuff
 *
 ***************************************************************/

static BOOLEAN slWriteMP(si_link l, leftv v)
{
  leftv next = (v != NULL ? v->next : (leftv) NULL);
  mpsr_ClearError();

  // writing is done with one leftv at a time
  if (v != NULL) v->next = NULL; // hence, we need to set next to NULL
  if (mpsr_PutMsg((MP_Link_pt) l->data, v) != mpsr_Success)
  {
    mpsr_PrintError((MP_Link_pt) l->data);
    if (v != NULL) v->next = next;
    return TRUE;
  }

  // take care of the remaining leftv's 
  while (next != NULL)
  {
    v->next = next;
    v = next;
    next = v->next;
    v->next = NULL;
    if (mpsr_PutMsg((MP_Link_pt) l->data, v) != mpsr_Success)
    {
      mpsr_PrintError((MP_Link_pt) l->data);
      v->next = next;
      return TRUE;
    }
  }
  return FALSE;
}

leftv slReadMP(si_link l)
{
  leftv v = NULL;
  mpsr_ClearError();
  if (mpsr_GetMsg((MP_Link_pt) l->data, v) != mpsr_Success)
  {
    mpsr_PrintError((MP_Link_pt) l->data);
    return NULL;
  }
  else
    return v;
}

static void SentQuitMsg(si_link l)
{
  leftv v = (leftv) Alloc0(sizeof(sleftv));

  v->rtyp = STRING_CMD;
  v->data = MPSR_QUIT_STRING;
  slWriteMP(l, v);
  Free(v, sizeof(sleftv));
}

static BOOLEAN slCloseMP(si_link l)
{
  if ((strcmp(l->mode, "launch") == 0 || strcmp(l->mode, "fork") == 0) &&
      (MP_GetLinkStatus((MP_Link_pt)l->data,MP_LinkReadyWriting) == MP_TRUE))
    SentQuitMsg(l);
  MP_CloseLink((MP_Link_pt) l->data);
  SI_LINK_SET_CLOSE_P(l);
  return FALSE;
}


static BOOLEAN slDumpMP(si_link l)
{
  mpsr_ClearError();
  if (mpsr_PutDump((MP_Link_pt) l->data) != mpsr_Success)
  {
    mpsr_PrintError((MP_Link_pt) l->data);
    return TRUE;
  }
  else
    return FALSE;
}

static BOOLEAN slGetDumpMP(si_link l)
{
  mpsr_ClearError();
  if (mpsr_GetDump((MP_Link_pt) l->data) != mpsr_Success)
  {
    mpsr_PrintError((MP_Link_pt) l->data);
    return TRUE;
  }
  else
    return FALSE;
}

static char* slStatusMP(si_link l, char* request)
{
  if (strcmp(request, "read") == 0)
  {
    if (SI_LINK_R_OPEN_P(l) &&
        (MP_GetLinkStatus((MP_Link_pt)l->data,MP_LinkReadyReading) == MP_TRUE))
        return "ready";
    else return "not ready";
  }
  else if (strcmp(request, "write") == 0)
  {
    if (SI_LINK_W_OPEN_P(l) &&
        (MP_GetLinkStatus((MP_Link_pt)l->data,MP_LinkReadyWriting) == MP_TRUE))
        return "ready";
    else return "not ready";
  }
  else return "unknown status request";
}

/***************************************************************
 *
 * MP batch stuff
 *
 ***************************************************************/

int Batch_ReadEval(si_link silink)
{
  leftv v = NULL;
  // establish top-level identifier for link
  idhdl id = enterid(mstrdup("mp_ll"), 0, LINK_CMD, &idroot, FALSE);
  IDLINK(id) = silink;

  // the main read-eval-write loop
  while(1)
  {
    v = slRead(silink, v);
    if (feErrors != NULL)
    {
      if (v != NULL) v->CleanUp();
      v = mpsr_InitLeftv(STRING_CMD, (void *) mstrdup(feErrors));
      Free(feErrors, feErrorsLen);
      feErrors = NULL;
    }

    // no need to evaluate -- it is done in the read
    if (v->Typ() == STRING_CMD &&
        (strcmp((char *)v->Data(), MPSR_QUIT_STRING)  == 0))
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
}

// #define MPSR_BATCH_DEBUG
#ifdef MPSR_BATCH_DEBUG
static BOOLEAN stop = 1;
#endif

  
int Batch_do(const char* port, const char* host)
{
#ifdef MPSR_BATCH_DEBUG
  fprintf(stderr, "Was started with pid %d\n", getpid());
  while (stop){};
#endif
  si_link silink = (si_link) Alloc0(sizeof(sip_link));
  char *istr;

  // parse argv to get port and host
  if (port == NULL)
  {
    fprintf(stderr,
            "Need '--MPport portnumber' command line argument in batch modus\n");
    return 1;
  }
  if (host == NULL)
  {
    fprintf(stderr,
            "Need '--MPhost hostname' command line argument in batch modus\n");
    return 1;
  }

  // initialize si_link
  istr = (char *) AllocL((strlen(port) + strlen(host) + 40)*sizeof(char));
  sprintf(istr, "MPtcp:connect --MPport %s --MPhost %s", port, host);
  slInit(silink, istr);
  FreeL(istr);
  // open link
  if (slOpen(silink, SI_LINK_OPEN))
  {
    fprintf(stderr, "Batch side could not connect on port %s and host %s\n",
            port, host);
    return 1;
  }

  return Batch_ReadEval(silink);
}

/***************************************************************
 *
 * MP link Extension inits
 *
 ***************************************************************/

void slInitMPFileExtension(si_link_extension s)
{
  s->Open=slOpenMPFile;
  s->Close=slCloseMP;
  s->Read=slReadMP;
  //s->Read2=NULL;
  s->Dump=slDumpMP;
  s->GetDump=slGetDumpMP;
  s->Write=slWriteMP;
  s->Status=slStatusMP;
  s->type="MPfile";
}

void slInitMPTcpExtension(si_link_extension s)
{
  s->Open=slOpenMPTcp;
  s->Close=slCloseMP;
  s->Read=slReadMP;
  //s->Read2=NULL;
  s->Dump=slDumpMP;
  s->GetDump=slGetDumpMP;
  s->Write=slWriteMP;
  s->Status=slStatusMP;
  s->type="MPtcp";
}

#endif

