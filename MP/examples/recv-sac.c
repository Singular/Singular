/* 
   recv-sac.c - simple server process to receive a big integer.
                If the argument -usesac is given on the commandline,
		SAC big ints will be used, otherwise GMP format is used.
*/

#include <stdio.h>
#include <fcntl.h>
#include "MP.h"
#include "saclib.h"
#include "MP_SacBigInt.h"

EXTERN MP_BigIntOps_t  imp_sac_bigint_ops;

main(argc, argv)
int  argc;
char *argv[];
{
  Word sac2, sac_stack;
  int          more = 0, n = -1, usesac = 0;
  MP_Link_pt link = NULL;
  MP_Env_pt  env = NULL;
  char *msg = NULL;
  MP_INT  gmp;
  mpz_ptr gmp1 = &gmp;
  Word *sac1 = &sac2;
  MP_NumAnnot_t nannots;

  BEGINSACLIB(&sac_stack);
  env = MP_AllocateEnv();

  sac1 = (Word *)malloc(sizeof(Word));
  MP_SetEnvOption(env, MP_BUFFER_SIZE_OPT, 512);
  MP_SetEnvOption(env, MP_MAX_FREE_BUFFERS_OPT, 46);
  MP_SetEnvOption(env, MP_INIT_FREE_BUFFERS_OPT, 24);
  
  env = MP_InitializeEnv(env);
  if (env == NULL) {
    fprintf(stderr, "%s: MP_EnvInitialize() failed!\n", argv[0]);
    exit(1);
    }

  if(IMP_GetCmdlineArg(argc, argv, "-usesac") != NULL) {
    usesac = 1;
    /* indicate we want SAC ints */
    MP_SetEnvBigIntFormat(env, &imp_sac_bigint_ops, MP_SAC); 
    }

  if ((link = MP_OpenLink(env, argc, argv)) == NULL) {
    fprintf(stderr, "%s: MP_OpenLink() failed!\n", argv[0]);
    exit(1);
    }

  MP_SetLinkOption(link, MP_LINK_LOG_MASK_OPT, MP_LOG_INIT_EVENTS);
  MP_SetLinkOption(link, MP_LINK_LOG_MASK_OPT, MP_LOG_READ_EVENTS);
/*  MP_SetLinkOption(link, MP_LINK_RECV_MODE_OPT, MP_RECV_WHOLE_MSG); */
 
  MP_SkipMsg(link);
  if (usesac) {
    printf("SAC: calling IMP_GetApInt\n");
    IMP_GetApInt(link, (MP_ApInt_t)&sac1);
    printf("\nrecv-sac: from IMP_GetApInt(), sac1 = "); 
    IWRITE(*sac1); printf("\n"); fflush(stdout);
    MP_GetApIntPacket(link, (MP_ApInt_t)&sac1, &nannots);
    printf("\nrecv-sac: from MP_GetApIntPacket(), sac1 = "); 
    IWRITE(*sac1); printf("\n"); fflush(stdout);
  }
  else {
    mpz_init(&gmp);
    IMP_GetApInt(link, (MP_ApInt_t)&gmp1);
    printf("GMP: here is what IMP_GetApInt() returned: ");fflush(stdout);
    mpz_out_str(stdout, 10, gmp1);
    mpz_clear(&gmp);
    MP_GetApIntPacket(link, (MP_ApInt_t)&gmp1, &nannots);
    printf("\nGMP: here is what MP_GetApIntPacket() returned: ");fflush(stdout);
    mpz_out_str(stdout, 10, gmp1);
    mpz_clear(&gmp);
  }
  printf("\n");

  MP_CloseLink(link);
  MP_ReleaseEnv(env);
  ENDSACLIB(SAC_FREEMEM);

  fprintf(stderr, "%s: All done!\n", argv[0]);
}
