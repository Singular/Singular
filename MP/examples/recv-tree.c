/* 
   recv-tree.c - simple server process using AF_UNIX sockets
	      taken from Paul Wang's book on Unix - p. 341
*/

#include <stdio.h>
#include <fcntl.h>
#include "MP.h"
#include "gmp.h"
#include "node.h"

FILE         *fd;

int 
process_request(root)
node_t *root;
{
  int more;

    more = print_node(root, fd);
    fprintf(fd, "\n"); fflush(fd);
    return more;
}

int get_infilename(fname)
char *fname;
{
  int fd;
  printf("get_infilename: fname = %s\n", fname);
  if ((fd = open(fname, O_RDONLY)) < 0)
    fprintf(stderr, "can't open %s for reading\n", fname);
  printf("get_infilename: fd = %d\n", fd);
  return fd;
}


main(argc, argv)
int  argc;
char *argv[];
{
  char *ofname = NULL;
  int          more = 0, n = -1;
  node_t       *root = NULL;
  MP_Link_pt link = NULL;
  MP_Env_pt  env = NULL;

  if ((ofname = IMP_GetCmdlineArg(argc, argv, "-outfile")) == NULL){
    fprintf(stderr,"%s: missing -outfile argument\n", argv[0]);
    return 1;
    }

  env = MP_AllocateEnv();

  MP_SetEnvOption(env, MP_BUFFER_SIZE_OPT, 512);
  MP_SetEnvOption(env, MP_MAX_FREE_BUFFERS_OPT, 46);
  MP_SetEnvOption(env, MP_INIT_FREE_BUFFERS_OPT, 24);
  
  env = MP_InitializeEnv(env);
  if (env == NULL) {
    fprintf(stderr, "%s: MP_EnvInitialize() failed!\n", argv[0]);
    exit(1);
    }
  fprintf(stderr,"%s: MP_EnvInitialize() succeeded\n", argv[0]);

  if ((link = MP_OpenLink(env, argc, argv)) == NULL) {
    fprintf(stderr, "%s: MP_OpenLink() failed!\n", argv[0]);
    exit(1);
    }
  fprintf(stderr,"%s: MP_OpenLink() succeeded\n", argv[0]);
  MP_SetLinkOption(link, MP_LINK_LOG_MASK_OPT, MP_LOG_READ_EVENTS);
  fprintf(stderr, "done setting first link options\n");
  MP_SetLinkOption(link, MP_LINK_RECV_MODE_OPT, MP_RECV_WHOLE_MSG); 
  fprintf(stderr, "done setting second link options\n");fflush(stderr);

  printf("ofname = %s\n", ofname);

  fd = fopen(ofname, "w");
  mpf_set_default_prec(256);  /* for the GMP arb prec reals */
  fprintf(stderr, "done setting mpf value\n");fflush(stderr);

  printf("server's annotated tree(s):\n");
  do {
    if (MP_TestEofMsg(link))  MP_SkipMsg(link);
    printf("\n\n%-20s %-5s %-30s %-7s %-9s\n", "Type", "Dict", "Value", 
             "#Annots", "#Children");
    /* STEP 1: get the request */
       root = m_get_node(link);

    /* STEP 2: process the request */
       more = process_request(root); 

    /* STEP 3: cleanup */
       free_tree(root);     /* free the annotated tree */
       root = NULL; 
       free_label_table();  /* free the label table    */

  } while (more);
  
  fflush(fd); fclose(fd);

  MP_CloseLink(link);
  MP_ReleaseEnv(env);

  fprintf(stderr, "%s: All done!\n", argv[0]);
}
