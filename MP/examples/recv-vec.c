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
  long i, *ivec = NULL;
  char *s=NULL, cop;
  float *fvec = NULL;
  float tvec[5];
  double *dvec = NULL;
  MP_NumChild_t nc;
  MP_NumAnnot_t na;
  MP_Common_t comval;
  MP_DictTag_t dtag;
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
  printf("reading extra stuff now\n");

  if (IMP_GetBytes(link, (char *)&tvec[0], 20) != MP_Success){
      printf("couldn't getBytes() \n");
      MP_PrintError(link);
      exit(1);
      }

  for (i = 0; i < 5; i++) { 
    printf("tvec[%d] = %f\n", i, tvec[i]); 
    tvec[i] = 0;
    }
  printf("\nnow do it using GetLong()\n");
  for (i = 0; i < 5; i++)
    if (IMP_GetLong(link, (long *)&tvec[i]) != MP_Success){
      printf("couldn't getlong() \n");
      MP_PrintError(link);
      exit(1);
      }

  for (i = 0; i < 5; i++) printf("tvec[%d] = %f\n", i, tvec[i]);

  if (MP_TestEofMsg(link)) MP_SkipMsg(link);

  if (IMP_GetSint32Vector(link, (MP_Uint32_t *)&ivec, 5) != MP_Success) {
    printf("couldn't get the int vector\n");
    MP_PrintError(link);
    exit(1);
    }
  else
    for (i = 0; i < 5; i++) printf("ivec[%d] = %d\n", i, ivec[i]);

  if (IMP_GetReal32Vector(link, &fvec, 5) != MP_Success){
    printf("couldn't get the float vector\n");
    MP_PrintError(link);
    exit(1);
    }
  else
    for (i = 0; i < 5; i++) printf("fvec[%d] = %f\n", i, fvec[i]);

  if (IMP_GetReal64Vector(link, &dvec, 5) != MP_Success){
    printf("couldn't get the double vector\n");
    MP_PrintError(link);
    }
  else
    for (i = 0; i < 5; i++) printf("dvec[%d] = %f\n", i, dvec[i]);


  if (MP_TestEofMsg(link)) MP_SkipMsg(link);
  printf("\nDOING THE IMP GETS NOW\n");
  if (IMP_GetSint32Vector(link, &ivec, 5) != MP_Success) {
    printf("couldn't get the int vector\n");
    MP_PrintError(link);
    }
  else
    for (i = 0; i < 5; i++) printf("ivec[%d] = %d\n", i, ivec[i]);

  if (IMP_GetReal32Vector(link, &fvec, 5) != MP_Success){
    printf("couldn't get the float vector\n");
    MP_PrintError(link);
    }
  else
    for (i = 0; i < 5; i++) printf("fvec[%d] = %f\n", i, fvec[i]);

  if (IMP_GetReal64Vector(link, &dvec, 5) != MP_Success){
    printf("couldn't get the double vector\n");
    MP_PrintError(link);
    }
  else
    for (i = 0; i < 5; i++) printf("dvec[%d] = %f\n", i, dvec[i]);

  MP_CloseLink(link);
  MP_ReleaseEnv(env);
}
