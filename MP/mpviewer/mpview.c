/*  File: mpview.c  
 *  Use:  mpview -infix data.bin
 *        mpview -latex data.bin
 *        mpview -prefix data.bin
 *  Author: Paul S. Wang
 *  Date:  9/21/96
 */

#include <stdio.h>
#include <fcntl.h>
#include "MP.h"
#include "node.h"
#include "latex.h"
#include "infix.h"
#include "prefix.h"

FILE         *fd;

int latex_nodes(root)
node_t *root;
{ /* check if finished */
  if ( root->type ==  MP_CommonOperatorType &&
       root->dtag == MP_MpDict && 
       *((MP_Common_t *)root->data) == MP_CopMpEndSession)
            return(0);
  printf("$$\n");
  display_node_latex(root, '+');
  printf("\n$$\n");
  return(1);
}

int infix_nodes(root)
node_t *root;
{ /* check if finished */
  if ( root->type ==  MP_CommonOperatorType &&
       root->dtag == MP_MpDict && 
       *((MP_Common_t *)root->data) == MP_CopMpEndSession)
            return(0);
  display_node_infix(root, '+');
  printf("\n");
  return(1);
}

int prefix_nodes(root)
node_t *root;
{ /* check if finished */
  if ( root->type ==  MP_CommonOperatorType &&
       root->dtag == MP_MpDict && 
       *((MP_Common_t *)root->data) == MP_CopMpEndSession)
            return(0);
  display_node_prefix(root);
  printf("\n");
  return(1);
}

typedef int (*dispfn)(/* note_t * */);

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


int main(argc, argv)
int  argc;
char *argv[];
{
  char *ofname = NULL;
  int          more = 0, n = -1;
  node_t       *root = NULL;
  MP_Link_pt link = NULL;
  MP_Env_pt  env = NULL;
  char *args[]= {NULL, "-MPtransp", "FILE", "-MPmode",
                 "read", "-MPfile", NULL, NULL};
/*  char *args[]= {argv[0], "-MPtransp", "FILE", "-MPmode",
                 "read", "-MPfile", argv[2], NULL}; */
  dispfn display;
 
  args[0] = argv[0]; args[6] = argv[2];
     if ( strcmp(argv[1], "-prefix") == 0 )
         display = prefix_nodes;
     else if ( strcmp(argv[1], "-latex") == 0 )
         display = latex_nodes;
     else
         display = infix_nodes;

  env = MP_AllocateEnv();

  MP_SetEnvOption(env, MP_BUFFER_SIZE_OPT, 512);
  MP_SetEnvOption(env, MP_MAX_FREE_BUFFERS_OPT, 46);
  MP_SetEnvOption(env, MP_INIT_FREE_BUFFERS_OPT, 24);
  
  env = MP_InitializeEnv(env);
  if (env == NULL) {
    fprintf(stderr, "%s: MP_EnvInitialize() failed!\n", argv[0]);
    exit(1);
    }

  if ((link = MP_OpenLink(env, 8, args)) == NULL) {
    fprintf(stderr, "%s: MP_OpenLink() failed!\n", argv[0]);
    exit(1);
    }
  MP_SetLinkOption(link, MP_LINK_RECV_MODE_OPT, MP_RECV_WHOLE_MSG); 

  mpf_set_default_prec(256);  /* for the GMP arb prec reals */

  do {
    if (MP_TestEofMsg(link))  MP_SkipMsg(link);
    /* STEP 1: get the request */
       root = m_get_node(link);

    /* STEP 2: process the request */
       more = display(root); 

    /* STEP 3: cleanup */
       free_tree(root);     /* free the annotated tree */
       root = NULL; 
       free_label_table();  /* free the label table    */

  } while (more);
  
  MP_CloseLink(link);
  MP_ReleaseEnv(env);

  return(0);
}
