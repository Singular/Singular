/*
  send-vec.c - simple client process using AF_UNIX sockets.
	     taken from Paul Wang's Unix book - p. 242
*/

#include <fcntl.h>
#include "MP.h"
#include "gmp.h"

main(argc, argv)
int argc;
char *argv[];
{
  char *ifname = NULL;
  MP_Link_pt link = NULL;
  MP_Env_pt  env = NULL;
  float fvec[5], ftest = 103.016;
  double dvec[5];
  long intvec[5];

  fvec[0] = 0.0;  fvec[1] = 1.0; fvec[2] = 2.0; fvec[3] = 3.0; fvec[4] = 4.0;
  dvec[0] = 0.0;  dvec[1] = 1.0; dvec[2] = 2.0; dvec[3] = 3.0; dvec[4] = 4.0;
  intvec[0] = 0; intvec[1] = 1; intvec[2] = 2; intvec[3] = 3; intvec[4] = 4;

  env = MP_InitializeEnv(NULL);
  if (env == NULL) {
    fprintf(stderr, "%s: MP_InitializeEnv() failed!\n", argv[0]);
    exit(1);
    }
 
  if ((link = MP_OpenLink(env, argc, argv)) == NULL) {
    fprintf(stderr, "%s: MP_OpenLink() failed!\n", argv[0]);
    exit(1);
    }
  printf("Done creating the link\n");
  MP_SetLinkOption(link, 12, MP_LOG_WRITE_EVENTS);
  MP_SetLinkOption(link, MP_LINK_LOG_MASK_OPT, MP_LOG_INIT_EVENTS);
  MP_SetLinkOption(link, MP_LINK_LOG_MASK_OPT, MP_LOG_WRITE_EVENTS);
  MP_SetLinkOption(link, MP_LINK_LOG_MASK_OPT, MP_LOG_READ_EVENTS);

  printf("%s: ftest = %f\n", argv[0], ftest);
  IMP_PutBytes(link, (char *)&ftest, 4);
  MP_EndMsgReset(link);
  MP_CloseLink(link);
  MP_ReleaseEnv(env);
} 
