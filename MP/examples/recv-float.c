/* 
   recv-vec.c - simple server process using AF_UNIX sockets
	      taken from Paul Wang's book on Unix - p. 341
*/

#include <stdio.h>
#include <fcntl.h>
#include "MP.h"
#include "gmp.h"

main(argc, argv)
  int  argc;
  char *argv[];
{
  long n = -1;
  unsigned long ui;
  float fp;
  MP_Link_pt link = NULL; 
  MP_Env_pt  env = NULL;

 if ((env = MP_InitializeEnv(env)) == NULL) {
    fprintf(stderr, "%s: MP_EnvInitialize() failed!\n", argv[0]);
    exit(1);
    }

  if ((link = MP_OpenLink(env, argc, argv)) == NULL) {
    fprintf(stderr, "%s: MP_OpenLink() failed!\n", argv[0]);
    exit(1);
    }
  printf("%s: created the link\n", argv[0]);  
  MP_SetLinkOption(link, MP_LINK_LOG_MASK_OPT, MP_LOG_INIT_EVENTS);
  MP_SetLinkOption(link, MP_LINK_LOG_MASK_OPT, MP_LOG_READ_EVENTS);
  MP_SetLinkOption(link, MP_LINK_LOG_MASK_OPT, MP_LOG_WRITE_EVENTS);
  printf("%s: done setting the link options\n", argv[0]);  


  if (MP_TestEofMsg(link)) MP_SkipMsg(link);
  printf("RECV-VEC: reading the vectors now\n\n");
  IMP_GetBytes(link, &fp, 4);
  printf("%s: fp = %f\n", argv[0], fp);

  MP_CloseLink(link);
  MP_ReleaseEnv(env);
}
