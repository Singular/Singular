/*
  send-vec.c - simple client process 
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
  int i;
  float fvec[5];
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

  MP_SetLinkOption(link, MP_LINK_LOG_MASK_OPT, MP_LOG_INIT_EVENTS);
  MP_SetLinkOption(link, MP_LINK_LOG_MASK_OPT, MP_LOG_WRITE_EVENTS);
  MP_SetLinkOption(link, MP_LINK_LOG_MASK_OPT, MP_LOG_READ_EVENTS);

  printf("\n\nSEND-VEC: putting the vectors now\n\n");

  IMP_PutBytes(link, (char *)&fvec[0], 20);

  for (i = 0; i < 5; i++)
    if (IMP_PutLong(link, (long *)&fvec[i]) != MP_Success)
	printf("couldn't do the putlongs !!\n");
  MP_EndMsgReset(link);

  fprintf(stderr,"Now calling IMP_PutBasicVector()\n");fflush(stderr);

  if (IMP_PutBasicVector(link, intvec, MP_Sint32Type, 5) != MP_Success){
    printf("couldn't put the int vector!!\n");
    MP_PrintError(link);
    }
  else 
    printf("SEND-VEC: Successfully put the int vector!!\n");
  
  if (IMP_PutBasicVector(link, fvec, MP_Real32Type, 5) != MP_Success)
    printf("couldn't put the float vector!!\n");
  else 
    printf("Successfully put the float vector!!\n");

  if (IMP_PutBasicVector(link, dvec, MP_Real64Type, 5) != MP_Success)
    printf("couldn't put the double vector!!\n");
  else 
    printf("Successfully put the double vector!!\n");
  MP_EndMsgReset(link);

  if (IMP_PutSint32Vector(link, intvec, 5) != MP_Success)
    printf("couldn't put the int vector!!\n");
  else 
    printf("Successfully put the int vector!!\n");

  if (IMP_PutReal32Vector(link, fvec, 5) != MP_Success)
    printf("couldn't put the float vector!!\n");
  else 
    printf("Successfully put the float vector!!\n");

  if (IMP_PutReal64Vector(link, dvec, 5) != MP_Success)
    printf("couldn't put the double vector!!\n");
  else 
    printf("Successfully put the double vector!!\n");
  MP_EndMsgReset(link);

  MP_CloseLink(link);
  MP_ReleaseEnv(env);
} 
