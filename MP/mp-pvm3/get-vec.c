/*
*     get-vec.c
*
*     Receive a vector of integers from put-vec and return them.
*
*/

#include <stdio.h>

#include <sys/types.h>
#include <fcntl.h>
#include <stdio.h>
#include <math.h>
#include "pvm3.h"
#include "MP.h"
#include "MP_PvmTransp.h"

EXTERN MP_TranspOps_t pvm_ops;

#define ENCODING  PvmDataRaw
#define SIZE 1000

FILE         *fd;

main(argc, argv)
  int argc;
  char *argv[];
{
  int mytid;                  /* my task id */
  int stid = 0;		      /* slave task id */
  int n, i, more = 0, *vec = NULL, numpts, numiter;
  char filename[128], *c = NULL;
  MP_NumAnnot_t na   = 0;
  MP_NumChild_t nc   = SIZE;
  MP_Env_pt     env  = NULL;
  MP_Link_pt    link = NULL; 

  pvm_setopt(PvmAutoErr, 0); /* tell PVM not to print error to stdout */
/*  pvm_setopt(PvmDebugMask, 3); */
  pvm_setopt(PvmRoute, PvmRouteDirect);
  if ((mytid = pvm_mytid()) < 0) {
		exit(1);
  }
  if ((c = IMP_GetCmdlineArg(argc, argv, "-iters")) == NULL){
    fprintf(stderr, "missing -iters argument\n"); 
    goto bail;
    }
  numiter = atoi(c); free(c); c = NULL;
  if ((c = IMP_GetCmdlineArg(argc, argv, "-pts")) == NULL){
    fprintf(stderr, "missing -pts argument\n"); 
    goto bail;
    }
  numpts = atoi(c); free(c); c = NULL;
  fprintf(stderr, "get-vec: numpts = %d, numiter = %d\n", numpts, numiter);
  fflush(stderr);
  env = MP_AllocateEnv();
  MP_SetEnvOption(env, MP_BUFFER_SIZE_OPT, pvm_getopt(PvmFragSize)-4);
  if ((env = MP_InitializeEnv(env)) == NULL) {
    fprintf(stderr, "%s: MP_EnvInitialize() failed!\n", argv[0]);
    exit(1);
  }
  MP_AddEnvTranspDevice(env, MP_PvmTransportDev, &pvm_ops);
  if ((link = MP_OpenLink(env, argc, argv)) == NULL) {
    fprintf(stderr, "%s: MP_OpenLink() failed!\n", argv[0]);
    exit(1);
    }
/*  MP_SetLinkOption(link, MP_LINK_LOG_MASK_OPT, MP_LOG_INIT_EVENTS);
  MP_SetLinkOption(link, MP_LINK_LOG_MASK_OPT, MP_LOG_READ_EVENTS);
  MP_SetLinkOption(link, MP_LINK_LOG_MASK_OPT, MP_LOG_WRITE_EVENTS); */

  pvm_initsend(ENCODING);
  stid = pvm_parent();
  pvm_send(stid, 0 );  /* let the parent know I am ready */
  vec = IMP_MemAllocFnc(numpts * sizeof(int));
  if (vec == NULL) fprintf(stderr, "ARGH!! vec is null!!\n");
/*   fd = fopen("/tmp/test.out", "w"); */
  for (i = 0; i < numiter; i++) {
    pvm_recv(-1, -1);
    if (pvm_upkint(vec, numpts, 1) < 0) 
      pvm_perror("can't unpack the lousy array using pvm: ");

    pvm_initsend(ENCODING);
    pvm_send(stid, 0); 
    }
    
  for (i = 0; i < numiter; i++) {
    if (MP_TestEofMsg(link)) {
      if (MP_SkipMsg(link) != MP_Success) {
	fprintf(stderr,"Problem with MP_SkipMsg()\n");
	MP_PrintError(link);
        }
      }
     else if (MP_SkipMsg(link) != MP_Success) {
	fprintf(stderr,"Problem with MP_SkipMsg()\n");
	MP_PrintError(link);
        }
 
    if (IMP_GetSint32Vector(link, &vec, numpts) != MP_Success) {
      fprintf(stderr,"Problem getting the vector with MP\n");
      MP_PrintError(link);
      }

    pvm_initsend(ENCODING);
    pvm_send(stid, 0);  /* let the parent know I am done */
    }

/*    for (i = 0; i < numpts; i++) {
      if (!(i % 10)) fprintf(fd, "\n");
      fprintf(fd, "%d ", vec[i]);
    }
*/
  IMP_MemFreeFnc(vec, numpts * sizeof(int));
/*   fflush(fd); fclose(fd); */
  MP_CloseLink(link);
  MP_ReleaseEnv(env);
  printf("\nget-vec: Successful termination.\n");
bail:
  pvm_exit();
}


