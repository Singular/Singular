/*
 *    put-vec.c
* 
*	See also get-vec.c
*/

#include <sys/types.h>
#include <fcntl.h>
#include <stdio.h>
#include "pvm3.h"
#include "MP.h"

EXTERN MP_TranspOps_t pvm_ops;

#define ENCODING  PvmDataRaw
#define SLAVENAME "get-vec"

main(argc, argv)
  int argc;
  char *argv[];
{
  int mytid;   /* my task id */
  int spawn_flag = PvmTaskDefault;
  int stid, n = 0, i, *vec, numpts, numiter;
  char *rhost = NULL, *c = NULL;
  MP_Link_pt link = NULL;
  MP_Env_pt  env = NULL;
  long start1, start2, end1, end2;
  FILE *fp;

  fp = fopen("mp-pvm.dat", "a");
  
  pvm_setopt(PvmAutoErr, 0); /* tell PVM not to print error to stdout */
  mytid = pvm_mytid();       /* enroll in pvm */
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
  fprintf(stderr, "put-vec: numpts = %d, numiter = %d\n", numpts, numiter);
  fflush(stderr);
  fprintf(fp, "%4d   \t %d \t %d \t ", numiter, numpts, numpts*sizeof(int));

  vec = (int *)malloc(numpts * sizeof(int));
  for (i = 0; i < numpts; i++) vec[i] = i;

  rhost = IMP_GetCmdlineArg(argc, argv, "-remhost");
  if (rhost != NULL) spawn_flag = PvmTaskHost;
  /* start up slave task */
  if (pvm_spawn(SLAVENAME, argv+1,spawn_flag, rhost, 1,&stid) < 0 || stid < 0) {
    fputs("can't initiate slave\n", stderr);
    goto bail;
    }

  env = MP_AllocateEnv();
  MP_SetEnvOption(env, MP_BUFFER_SIZE_OPT, pvm_getopt(PvmFragSize)-4);
  env = MP_InitializeEnv(env);
  if (env == NULL) {
    fprintf(stderr, "%s: MP_InitializeEnv() failed!\n", argv[0]);
    exit(1);
    }
  MP_AddEnvTranspDevice(env, MP_PvmTransportDev, &pvm_ops);
  if ((link = MP_OpenLink(env, argc, argv)) == NULL) {
    fprintf(stderr, "%s: MP_OpenLink() failed!\n", argv[0]);
    exit(1);
    }
/*  MP_SetLinkOption(link, MP_LINK_LOG_MASK_OPT, MP_LOG_INIT_EVENTS);
  MP_SetLinkOption(link, MP_LINK_LOG_MASK_OPT, MP_LOG_WRITE_EVENTS);
  MP_SetLinkOption(link, MP_LINK_LOG_MASK_OPT, MP_LOG_READ_EVENTS); */
  /* need the following to use PvmDataInPlace encoding mechanism */
/*  MP_SetLinkOption(link, MP_LINK_SEND_MODE_OPT, MP_SEND_WHOLE_MSG);   */

  /* Wait for slave task to start up */
  pvm_setopt(PvmRoute, PvmRouteDirect);
  pvm_recv( stid, 0 );

  MP_PvmSetTids(link, 1, &stid);
  MP_PvmSetSendMode(link, PvmDataInPlace);  
  clock();
 
  for (i = 0; i < numiter; i++) {
   /* pvm_initsend(PvmDataInPlace); */
    pvm_initsend(PvmDataRaw); 
    pvm_pkint(vec, numpts, 1);
    if (pvm_send(stid, 0)) { 
      fprintf(stderr, "can't send to \"%s\"\n", SLAVENAME); 
      goto bail; 
      } 
    MP_ResetLink(link); 
    pvm_recv( stid, 0 ); 
    } 
  end1 = clock();   
  for (i = 0; i < numiter; i++) {
  /*  pvm_initsend(PvmDataRaw);*/
/*    pvm_initsend(PvmDataInPlace);*/
    if (IMP_PutSint32Vector(link, vec, numpts) != MP_Success) {
      fprintf(stderr, "put-vec: problem putting the vector\n");
      MP_PrintError(link);
    }
    MP_EndMsg(link);
 /*   if (pvm_send(stid, 0)) {
      fprintf(stderr, "can't send to \"%s\"\n", SLAVENAME);
      goto bail;
    } */
    MP_ResetLink(link);
    pvm_recv( stid, 0 );
    }
  end2 = clock();
  fprintf(fp, "%14f\t", end1 / (float) numiter ); 
  fprintf(fp, "%14f\n", (end2 - end1) / (float) numiter );
  fflush(fp);
  printf("\nput-vec: Successful termination.\n");
  fclose(fp);
bail:
  free(vec);
  MP_CloseLink(link); /* cleanup and exit */
  MP_ReleaseEnv(env);
  pvm_exit();
}
