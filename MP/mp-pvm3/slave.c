/*
*	mp-pvm-slave.c
* 
*	See mp-pvm-master.c
*/

#include <sys/types.h>
#include <fcntl.h>
#include <stdio.h>
#include "pvm3.h"
#include "MP.h"
#include "gmp.h"
#include "MP_PvmTransp.h"

#define ENCODING  PvmDataRaw
#define SLAVENAME "slave"

EXTERN MP_TranspOps_t pvm_ops;

#include "pvm_imp_getdata.c"

/**********************************************************************
 * FUNCTION: imp_file2tree
 * ARGUMENT: link - pointer to a stream structure
 *           fname - If not empty, this is a string giving the name 
 *                   of the input file (as given from the command line).  
 *                   If the string is empty, the routine asks for a
 *                   filename.  If the filename cannot be opened for
 *                   reading we print an error message and croak.
 *           stid  - tids to which to send the data.
 * RETURN:   None.
 * PURPOSE:  Read successive trees from an input file and send them to
 *           the data stream associated with link.  
 *
 * COMMENT:  Note that MP_EndMsg() is called after writing each
 *           tree to the buffer.  This flushes the buffer to the 
 *           network and sets the last fragment bit in the buffer 
 *           header.  The receiver must do a corrsponding m_skiprecord()
 *           PRIOR to retrieving EACH tree from the data stream.
 **********************************************************************/
void
#ifdef __STDC__
imp_file2tree(MP_Link_pt link, char *fname, int *stid )
#else
imp_file2tree(link, fname, stid)
MP_Link_pt link;
char *fname;
int *stid;
#endif
{
  FILE *fd;
  int more = 0;

  if (fname == NULL) {
    fname = malloc(64);
    printf("Enter name of input file: ");
    scanf("%s", fname);
    }

  if ((fd = fopen(fname, "r")) == NULL) {
    fprintf(stderr, "Can't open %s for reading\n", fname);
    return;
    }

  do {
    pvm_initsend(PvmDataRaw);
/*    pvm_initsend(PvmDataInPlace);*/
    more = imp_get_term(link, fd);
    MP_EndMsg(link);
    if (pvm_send(*stid, 1)) {
      fprintf(stderr, "can't send to \"%s\"\n", SLAVENAME);
      goto bail;
      }
   MP_ResetLink(link);
  } while (more);
  free (fname);
  return;

bail:
  pvm_exit(); exit(1);
  free (fname);
  fclose(fd);
}




main(argc, argv)
  int argc;
  char *argv[];
{
  int mytid;   /* my task id */
  int stid;
  int i;
  int n = 0;
  char filename[128];
  MP_Link_pt link = NULL;
  MP_Env_pt  env = NULL;

  pvm_setopt(PvmAutoErr, 0); /* tell PVM not to print error to stdout */
  mytid = pvm_mytid();       /* enroll in pvm */

  pvm_setopt(PvmRoute, PvmRouteDirect);
  pvm_recv(-1, 5);       /* get the name of the input file from the master */
  pvm_upkstr(filename); 

  env = MP_AllocateEnv();
  MP_SetEnvOption(env, MP_BUFFER_SIZE_OPT, pvm_getopt(PvmFragSize));
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
  MP_SetLinkOption(link, MP_LINK_LOG_MASK_OPT, MP_LOG_INIT_EVENTS);
  MP_SetLinkOption(link, MP_LINK_LOG_MASK_OPT, MP_LOG_WRITE_EVENTS);
  MP_SetLinkOption(link, MP_LINK_LOG_MASK_OPT, MP_LOG_READ_EVENTS);
  /* need the following to use PvmDataInPlace encoding mechanism */
  MP_SetLinkOption(link, MP_LINK_SEND_MODE_OPT, MP_SEND_WHOLE_MSG);
  printf("slave: MP buffer size is %d\n", MP_GetEnvOption(env, MP_BUFFER_SIZE_OPT));
  printf("slave: PVM buffer size is %d\n", pvm_getopt(PvmFragSize));

  pvm_initsend(ENCODING);
  stid = pvm_parent();
  pvm_send(stid, 0 );  /* let the parent know I am ready */

  imp_file2tree(link, filename, &stid);  /* do the work */

  MP_CloseLink(link); /* cleanup and exit */
  MP_ReleaseEnv(env);
  pvm_exit();
  exit(0);

}
