/*
  send-sac.c - simple client process to send a big integer.
               If the argument -usesac is given on the commandline,
	       SAC big ints will be used, otherwise GMP format is used.

*/

#include <sys/types.h>
#include <string.h>
#include <fcntl.h>
#include "MP.h"
#include "saclib.h"
#include "MP_SacBigInt.h"

EXTERN MP_BigIntOps_t  imp_sac_bigint_ops;

void main(argc, argv)
  int argc;
  char *argv[];
{
  char *ifname = NULL;
  MP_Link_pt link = NULL;
  MP_Env_pt  env = NULL;
  int usesac = 0;
  Word sac1, sac2, sac_stack;
  MP_INT gmp;

  BEGINSACLIB(&sac_stack);        /* initialize SACLIB */
  env = MP_InitializeEnv(NULL);   /* create and initialize an MP environment */
  if (env == NULL) {
    fprintf(stderr, "%s: MP_InitializeEnv() failed!\n", argv[0]);
    exit(1);
    }
  if(IMP_GetCmdlineArg(argc, argv, "-usesac") != NULL) {
    usesac = 1;
    /* indicate we want SAC ints */
    MP_SetEnvBigIntFormat(env, &imp_sac_bigint_ops, MP_SAC); 
    }
  /* now create the link */
  if ((link = MP_OpenLink(env, argc, argv)) == NULL) { 
    fprintf(stderr, "%s: MP_OpenLink() failed!\n", argv[0]);
    exit(1);
    }
  MP_SetLinkOption(link, MP_LINK_LOG_MASK_OPT, MP_LOG_INIT_EVENTS);
  MP_SetLinkOption(link, MP_LINK_LOG_MASK_OPT, MP_LOG_WRITE_EVENTS);
/*  MP_SetLinkOption(link, MP_LINK_SEND_MODE_OPT, MP_SEND_WHOLE_MSG);*/

  printf("BETA = %d\n\n", BETA);
  printf("Enter big positive or negative integer: ");
  if (usesac) {
    sac1 = IREAD();
    printf("the integer just read is "); 
    IWRITE(sac1); printf("\n");  fflush(stdout);
    if (!ISATOM(sac1)) LWRITE(sac1); printf("\n");  fflush(stdout);
    IMP_PutApInt(link, (MP_ApInt_t)&sac1);
    MP_PutApIntPacket(link, (MP_ApInt_t)&sac1, 0);
    }
  else {
    mpz_init(&gmp);
    mpz_inp_str(&gmp, stdin, 10);
    IMP_PutApInt(link, (MP_ApInt_t)&gmp);
    MP_PutApIntPacket(link, (MP_ApInt_t)&gmp, 0);
    }
  MP_EndMsg(link);

  /* cleanup */
  MP_CloseLink(link);
  MP_ReleaseEnv(env);
  ENDSACLIB(SAC_FREEMEM);
}
