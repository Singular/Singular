/*
*      slave.c *ACTUALLY, this file is named master.c*
*
*       Send an MP tree to a single master.
*
*   9 Dec 1991  Manchek
*  14 Oct 1992  Geist  - revision to pvm3
*   6 Mar 1994  Geist  - synch tasks and add direct route
*/

#include <stdio.h>

#include <sys/types.h>
#include <fcntl.h>
#include <stdio.h>
#include <math.h>
#include "pvm3.h"
#include "MP.h"
#include "MP_PvmTransp.h"

#define ENCODING  PvmDataRaw
#define SLAVENAME "slave"

EXTERN MP_TranspOps_t pvm_ops;

FILE         *fd;

#include "node.c"
#include "mp_pvm.c"


int 
process_request(root, lfd)
node_t *root;
FILE *lfd;
{
  int more;

    more = print_node(root, lfd);
    return more;
}

main(argc, argv)
	int argc;
	char *argv[];
{
  int mytid;                  /* my task id */
  int stid = 0;		      /* slave task id */
  int n, i, more = 0;
  int spawn_flag = PvmTaskDefault;
  char filename[128], *rhost = NULL;
  MP_Env_pt  env = NULL;
  MP_Link_pt link = NULL; 
  node_t       *root = NULL;

  pvm_setopt(PvmAutoErr, 0); /* tell PVM not to print error to stdout */
  if ((mytid = pvm_mytid()) < 0) {
		exit(1);
  }

  rhost = IMP_GetCmdlineArg(argc, argv, "-remhost");
  if (rhost != NULL) spawn_flag = PvmTaskHost;
  /* start up slave task */
  if (pvm_spawn(SLAVENAME, argv, spawn_flag, rhost, 1, &stid) < 0 || stid < 0) {
    fputs("can't initiate slave\n", stderr);
    goto bail;
  }

  printf("name of data file: ");
  scanf("%s", filename);

  pvm_initsend(PvmDataDefault);
  pvm_pkstr(filename);
  pvm_send(stid, 5);

  env = MP_AllocateEnv();
  MP_SetEnvOption(env, MP_BUFFER_SIZE_OPT, pvm_getopt(PvmFragSize));
  if ((env = MP_InitializeEnv(env)) == NULL) {
    fprintf(stderr, "%s: MP_EnvInitialize() failed!\n", argv[0]);
    exit(1);
  }
  MP_AddEnvTranspDevice(env, MP_PvmTransportDev, &pvm_ops);

  printf("MP buffer size is %d\n", MP_GetEnvOption(env, MP_BUFFER_SIZE_OPT));
  printf("PVM buffer size is %d\n", pvm_getopt(PvmFragSize));

  if ((link = MP_OpenLink(env, argc, argv)) == NULL) {
    MP_ReleaseEnv(env);
    pvm_exit();
    fprintf(stderr, "%s: MP_OpenLink() failed!\n", argv[0]);
    exit(1);
    }
  
  MP_SetLinkOption(link, MP_LINK_LOG_MASK_OPT, MP_LOG_INIT_EVENTS);
  MP_SetLinkOption(link, MP_LINK_LOG_MASK_OPT, MP_LOG_READ_EVENTS);
  MP_SetLinkOption(link, MP_LINK_LOG_MASK_OPT, MP_LOG_WRITE_EVENTS);

    /* Wait for slave task to start up */
  pvm_setopt(PvmRoute, PvmRouteDirect);
  pvm_recv(stid, 0);

  fd = fopen("/tmp/test.out", "w");
  puts("Doing MP-PVM test\n");

  printf("server's annotated tree(s):\n");
  do {
    if (MP_TestEofMsg(link))  MP_SkipMsg(link);
    printf("\n\ntype\t\t  #annots      value\t#children\n");
    /* STEP 1: get the request */
       root = m_get_node(link);
    /* STEP 2: process the request */
       more = process_request(root, fd); 

    /* STEP 3: cleanup */
       free_tree(root);     /* free the annotated tree */
       root = NULL; 
       free_label_table();  /* free the label table    */
  } while (more);

  fflush(fd); fclose(fd);
  MP_CloseLink(link);
  MP_ReleaseEnv(env);
  printf("\nmaster: Successful termination.\n");
  pvm_exit();
  exit(0);

bail: 
  if (stid > 0)
    pvm_kill(stid);
  pvm_exit();
  exit(1);
}


