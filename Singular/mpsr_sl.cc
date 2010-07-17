/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/***************************************************************
 *  File:    mpsr_sl.cc
 *  Purpose: implementation of sl_link routines for MP
 *  Author:  obachman (Olaf Bachmann)
 *  Created: 12/00
 *  Version: $Id$
 *******************************************************************/

#include <Singular/mod2.h>

#ifdef HAVE_MPSR

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#ifdef HPUX_9
#include <signal.h>
#endif
#include <sys/wait.h>
#include <Singular/mpsr.h>
#include <Singular/tok.h>
#include <omalloc.h>
#include <kernel/febase.h>
#include <Singular/subexpr.h>
#include <Singular/ipid.h>
#include <Singular/silink.h>
#include <Singular/feOpt.h>


static int Batch_ReadEval(si_link silink);

#ifdef MPSR_DEBUG
#define MP_SET_LINK_OPTIONS(link) \
  if (link != NULL) \
     MP_SetLinkOption(link,MP_LINK_LOG_MASK_OPT,MP_LOG_ALL_EVENTS)
#else
#define MP_SET_LINK_OPTIONS(link) ((void *) 0)
#endif

static MP_Env_pt mp_Env=NULL;
extern void (*MP_Exit_Env_Ptr)();

/* =============== general utilities ====================================== */
static void MP_Exit_Env_sl()
{ MP_ReleaseEnv(mp_Env); mp_Env=NULL; }
static void FreeCmdArgs(int argc, char** argv)
{
  int i;
  for (i=0; i<argc; i++)
    omFree(argv[i]);

  if (argv) omFreeSize(argv, argc*sizeof(char *));
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
    char *s2=omStrDup(str);

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
      *argv = (char **) omAlloc0(i*sizeof(char *));
      if (appl != NULL) i -= 2;
      if (i>0)
      {
        strcpy(s2,str);
        *argv[0] = omStrDup(strtok(s2, " "));
        for(j = 1; j <i; j++)
          (*argv)[j] = omStrDup(strtok(NULL, " "));
      }
    }
    else
      *argv = NULL;

    if (appl != NULL)
    {
      (*argv)[*argc -2] = omStrDup("--MPapplication");
      (*argv)[*argc -1] = omStrDup(&(appl[15]));
    }

    omFree(s2);
  }

}

/***************************************************************
 *
 * MPfile  specific stuff
 *
 ***************************************************************/
LINKAGE BOOLEAN slOpenMPFile(si_link l, short flag)
{
  const char *argv[] = {"--MPtransp", "FILE", "--MPmode", "append",
                        "--MPfile", "/tmp/mpout"};
  const char *mode;

  MP_Link_pt link = NULL;

  if (flag == SI_LINK_OPEN)
  {
   if (l->mode[0] != '\0' && (strcmp(l->mode, "r") == 0))
      flag = SI_LINK_READ;
    else flag = SI_LINK_WRITE;
  }

  if (l->name[0] != '\0') argv[5] = l->name;
  else l->name = omStrDup(argv[5]);


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

  if (mp_Env == NULL || (MPT_Init(mp_Env) != MPT_Success))
  {
    WerrorS("Open: Error in initialization of MP environment");
    return TRUE;
  }
  MP_Exit_Env_Ptr=MP_Exit_Env_sl;


  if ((link = MP_OpenLink(mp_Env, 6, argv)) == NULL)
    return TRUE;

  MP_SET_LINK_OPTIONS(link);
  l->data = (void *) link;
  SI_LINK_SET_OPEN_P(l, flag);
  omFree(l->mode);
  l->mode = omStrDup(mode);
  return FALSE;
}

/***************************************************************
 *
 * MPtcp  specific stuff
 *
 ***************************************************************/

LINKAGE MP_Link_pt slOpenMPConnect(int n_argc, char **n_argv)
{
  const char *argv[] = {"--MPtransp", "TCP", "--MPmode", "connect", "--MPport",
                       "1025",  "--MPhost", "localhost","--MPrsh","ssh"};

  const char *port = IMP_GetCmdlineArg(n_argc, n_argv, "--MPport");
  const char *host = IMP_GetCmdlineArg(n_argc, n_argv, "--MPhost");
  const char *rsh = IMP_GetCmdlineArg(n_argc, n_argv, "--MPrsh");

  if (port == NULL) port = (char*) feOptValue(FE_OPT_MPPORT);
  if (host == NULL) host = (char*) feOptValue(FE_OPT_MPHOST);
  if (rsh == NULL) rsh = (char*) feOptValue(FE_OPT_MPRSH);

  if (port != NULL)
    argv[5] = port;
  if (host != NULL)
    argv[7] = host;
  else
    argv[7] = mp_Env->thishost;
  if (rsh != NULL)
    argv[9] = rsh;

  return MP_OpenLink(mp_Env, 10, argv);
}

LINKAGE MP_Link_pt slOpenMPListen(int n_argc, char **n_argv)
{
  const char *argv[] = {"--MPtransp", "TCP", "--MPmode", "listen",
                       "--MPport", "1025"};
  char *port = IMP_GetCmdlineArg(n_argc, n_argv, "--MPport");

  if (port == NULL) port = (char*) feOptValue(FE_OPT_MPHOST);

  if (port != NULL) argv[5] = port;

  return MP_OpenLink(mp_Env, 6, argv);
}

MP_Link_pt slOpenMPLaunch(int n_argc, char **n_argv)
{
  const char *argv[] = {"--MPtransp", "TCP", "--MPmode", "launch",
                       "--MPhost", "localhost",
                       "--MPapplication", "Singular -bq  --no-warn --no-out --no-rc",
                       "--MPrsh", "rsh"};
  const char *appl = IMP_GetCmdlineArg(n_argc, n_argv, "--MPapplication");
  const char *host = IMP_GetCmdlineArg(n_argc, n_argv, "--MPhost");
  const char *rsh = IMP_GetCmdlineArg(n_argc, n_argv, "--MPrsh");
  char* nappl = NULL;
  MP_Link_pt link;
  int argc = 8;

  if (appl == NULL && (host == NULL ||
                       strcmp(host, "localhost") == 0))
  {
    appl = feResource("Singular");

    if (appl != NULL)
    {
      nappl = (char*) omAlloc(MAXPATHLEN + 50);
      strcpy(nappl, appl);
      strcat(nappl, " -bq --no-warn --no-out --no-rc");
      appl = nappl;
    }
  }

  if ((host == NULL)||(strcmp(host, "localhost") == 0))
  {
    argv[5] = mp_Env->thishost;
  }
  else
    argv[5] = host;

  if (appl != NULL)
    argv[7] = appl;


  if (rsh != NULL)
  {
    argv[9] = rsh;
    argc = 10;
  }

  link = MP_OpenLink(mp_Env, argc, argv);
  if (nappl != NULL) omFreeSize(nappl, MAXPATHLEN + 50);
  return link;
}

LINKAGE MP_Link_pt slOpenMPFork(si_link l, int n_argc, char **n_argv)
{
  MP_Link_pt link = NULL;
  const char *argv[] = {"--MPtransp", "TCP", "--MPmode", "fork", "--MPport", "1703"};
  char *port = IMP_GetCmdlineArg(n_argc, n_argv, (char *)"--MPport");

  if (port != NULL) argv[5] = port;

  link = MP_OpenLink(mp_Env, 6, argv);
  if (link != NULL)
  {
    if (MP_GetLinkStatus(link, MP_LinkIsParent))
    {
    /* parent's business */
      if (l->name != NULL) omFree(l->name);
      l->name = omStrDup("parent");
      return link;
    }
    else
    {
      /* child's business -- go into batch mode */
      if (l->name != NULL) omFree(l->name);
      l->name = omStrDup("child");
      MP_SET_LINK_OPTIONS(link);
      SI_LINK_SET_RW_OPEN_P(l);
      l->data = (void *) link;
      fe_fgets_stdin=fe_fgets_dummy;
      _exit(Batch_ReadEval(slCopy(l)));
    }
  }
  else
  {
    /* only parent can get here */
    return NULL;
  }
}



LINKAGE BOOLEAN slOpenMPTcp(si_link l, short flag)
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
  MP_Exit_Env_Ptr=MP_Exit_Env_sl;

  if (strcmp(l->mode, "connect") == 0) link = slOpenMPConnect(argc, argv);
  else if (strcmp(l->mode, "listen") == 0) link = slOpenMPListen(argc, argv);
  else if (strcmp(l->mode, "launch") == 0) link = slOpenMPLaunch(argc, argv);
  else
  {
    if (strcmp(l->mode, "fork") != 0)
    {
      if (l->mode != NULL) omFree(l->mode);
      l->mode = omStrDup("fork");
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

LINKAGE BOOLEAN slWriteMP(si_link l, leftv v)
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

LINKAGE leftv slReadMP(si_link l)
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
  leftv v = (leftv) omAlloc0Bin(sleftv_bin);

  v->rtyp = STRING_CMD;
  v->data = (void *)MPSR_QUIT_STRING;
  slWriteMP(l, v);
  omFreeBin(v, sleftv_bin);
}

LINKAGE BOOLEAN slCloseMP(si_link l)
{
#ifdef HPUX_9
  signal(SIGCHLD, (void (*)(int))SIG_DFL);
#endif  
  if ((strcmp(l->mode, "launch") == 0 || strcmp(l->mode, "fork") == 0) &&
      (MP_GetLinkStatus((MP_Link_pt)l->data,MP_LinkReadyWriting) == MP_TRUE))
  {
    SentQuitMsg(l);
    //wait(NULL);
  }
  MP_CloseLink((MP_Link_pt) l->data);
#ifdef HPUX_9
  signal(SIGCHLD, (void (*)(int))SIG_IGN);
#endif  
  SI_LINK_SET_CLOSE_P(l);
  return FALSE;
}

LINKAGE BOOLEAN slKillMP(si_link l)
{
  MP_KillLink((MP_Link_pt) l->data);
  SI_LINK_SET_CLOSE_P(l);
  return FALSE;
}

LINKAGE BOOLEAN slDumpMP(si_link l)
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

LINKAGE BOOLEAN slGetDumpMP(si_link l)
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

LINKAGE const char* slStatusMP(si_link l, const char* request)
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

static int Batch_ReadEval(si_link silink)
{
  leftv v = NULL;
  // establish top-level identifier for link
  idhdl id = enterid(omStrDup("mp_ll"), 0, LINK_CMD, &IDROOT, FALSE);
  IDLINK(id) = silink;

  // the main read-eval-write loop
  loop
  {
    errorreported = FALSE;
    v = slRead(silink, v);
    if (feErrors != NULL && *feErrors != '\0')
    {
      if (v != NULL) v->CleanUp();
      v = mpsr_InitLeftv(STRING_CMD, (void *) omStrDup(feErrors));
      *feErrors = '\0';
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
      omFreeBin(v, sleftv_bin);
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


LINKAGE int Batch_do(const char* port, const char* host)
{
#ifdef MPSR_BATCH_DEBUG
  fprintf(stderr, "Was started with pid %d\n", getpid());
  while (stop){};
#endif
  si_link silink = (si_link) omAlloc0Bin(sip_link_bin);
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
  istr = (char *) omAlloc((strlen(port) + strlen(host) + 40)*sizeof(char));
  sprintf(istr, "MPtcp:connect --MPport %s --MPhost %s", port, host);
  slInit(silink, istr);
  omFree(istr);
  // open link
  if (slOpen(silink, SI_LINK_OPEN))
  {
    fprintf(stderr, "Batch side could not connect on port %s and host %s\n",
            port, host);
    return 1;
  }

  return Batch_ReadEval(silink);
}
#endif
