/*
  send-tree.c - simple client process 
*/

#include <sys/types.h>
#include <string.h>
#include <fcntl.h>
#include "MP.h"
#include "gmp.h"
/*
   imp_file2tree() - Routine to read sample data from a file.  The routine
		     does some simple checking (e.g., tells you if it
		     expects an annotation and doesn't get one or gets
		     a type it doesn't understand).
*/
   
/* global to return an error message when we read something we 
   think should be an annotation, but we don't recognize it */

char *badstring;

/**********************************************************************
 * FUNCTION: imp_get_annotation
 * ARGUMENT: link - pointer to a stream structure
 *           fd    - file descriptor for the input file
 * RETURN:   MP_AnnotFlags_t - flags associated with the annotation.
 * PURPOSE:  Read a single annotation and associated flags from the
 *           input file associated with fd and "put" the annotation
 *           to the stream associated with link.  We don't handle
 *           valuated annotations here.  That is done by the caller
 *           (which is why we return the annotations flags.
 **********************************************************************/
MP_AnnotFlags_t
#ifdef _ANSI_
imp_get_annotation(MP_Link_pt link, FILE *fd)
#else
imp_get_annotation(link, fd)
MP_Link_pt link;
FILE *fd;
#endif
{
  char type[32];
  MP_AnnotType_t atype = 0;
  long   dtag, flags;

  fscanf(fd, "%s", type); 
  if(strcmp(type, "Label_AP") == 0) atype = MP_AnnotMpLabel;
  else if(strcmp(type, "Reference_AP") == 0) atype = MP_AnnotMpRef;
  else if(strcmp(type, "Prototype_AP") == 0)  atype = MP_AnnotProtoPrototype;
  else if(strcmp(type, "Store_AP") == 0) atype = MP_AnnotMpStore;
  else if(strcmp(type, "Retrieve_AP") == 0) atype = MP_AnnotMpRetrieve;
  else if(strcmp(type, "Stored_AP") == 0)  atype = MP_AnnotMpStored;
  else if(strcmp(type, "Source_AP") == 0)  atype = MP_AnnotMpSource;
  else if(strcmp(type, "Comment_AP") == 0)  atype = MP_AnnotMpComment;
  else if(strcmp(type, "Timing_AP") == 0)  atype = MP_AnnotMpTiming;
  else if(strcmp(type, "Type_AP") == 0)  atype = MP_AnnotMpType;
  else if(strcmp(type, "Units_AP") == 0)  atype = MP_AnnotMpUnits;
  else {
    badstring = type;
    return 99;
  }
  if (atype != 0) {
    fscanf(fd, "%d  %X", &dtag, &flags);
    MP_PutAnnotationPacket(link, dtag, atype, flags);
  }
  return flags;
}


/**********************************************************************
 * FUNCTION: imp_get_term
 * ARGUMENT: link - pointer to a stream structure
 *           fd    - file descriptor for the input file
 * RETURN:   int - 0 means there is no more data to read 
 *                 1 means there are more trees to be read.
 * PURPOSE:  Read a term of a tree from the input file associated 
 *           with fd and "put" the term (along with associated
 *           annotations and children) to the stream associated with 
 *           link.  
 **********************************************************************/
int
#ifdef _ANSI_
imp_get_term(MP_Link_pt link, FILE *fd)
#else
imp_get_term(link, fd)
MP_Link_pt link;
FILE *fd;
#endif
{
  char type[32], str[64], raw[512], c;
  MP_ApInt_t apint;
  MP_ApReal_t  apreal;
  MP_AnnotFlags_t flags = 0;
  MP_NumAnnot_t   annots = 0;
  MP_NumChild_t   num_children = 0;
  unsigned long   dtag, cval;
  MP_NodeHeader_t hdr;
  unsigned int    my_uint;
  int i, sint;
  float  r32;
  double r64;

  fscanf(fd, "%s", type); 
  if (strcmp(type, "MP_Sint32Type") == 0) {
    fscanf(fd, "%d %u", &sint, &annots);
    IMP_PutNodeHeader(link, MP_Sint32Type, 0, 0, annots, 0);
    IMP_PutSint32(link, sint);
  }
  else if (strcmp(type, "MP_Uint32Type") == 0) {
    fscanf(fd, "%u %u", &my_uint, &annots);
    IMP_PutNodeHeader(link, MP_Uint32Type, 0, 0, annots, 0);
    IMP_PutUint32(link, my_uint);
  }
  else if (strcmp(type, "MP_Sint8Type") == 0) {
    fscanf(fd, "%d %u", &sint, &annots);
    MP_PutSint8Packet(link, sint, annots);
  }
  else if (strcmp(type, "MP_Uint8Type") == 0) {
    fscanf(fd, "%d %u", &my_uint, &annots);
    MP_PutUint8Packet(link, my_uint, annots);
  }
  else if (strcmp(type, "MP_BooleanType") == 0) {
    fscanf(fd, "%1s %d", &c, &annots);
    my_uint = (c == 'T') ? 1 : 0;
    MP_PutBooleanPacket(link, my_uint, annots);
  }
  else if (strcmp(type, "MP_ApIntType") == 0) {
    mpz_init((mpz_ptr)&apint);
    /* now get the apint from the file */
    mpz_inp_str((mpz_ptr)&apint, fd, 10);
    fscanf(fd, "%u ", &annots);
    IMP_PutNodeHeader(link, MP_ApIntType, 0, 0, annots, 0);
    IMP_PutApInt(link, &apint);
    mpz_clear((mpz_ptr)&apint);
  }
  else if (strcmp(type, "MP_ApRealType") == 0) {
    mpf_init((mpf_ptr)&apreal);
    /* now get the apreal from the file */
    mpf_inp_str((mpf_ptr)&apreal, fd, 10);
    fscanf(fd, "%u ", &annots);
    IMP_PutNodeHeader(link, MP_ApRealType, 0, 0, annots, 0);
    IMP_PutApReal(link, &apreal);
    mpf_clear((mpf_ptr)&apreal); 
  }
  else if (strcmp(type, "MP_Real32Type") == 0) {
    fscanf(fd, "%f %u", &r32, &annots);
    IMP_PutNodeHeader(link, MP_Real32Type, 0, 0, annots, 0);
    IMP_PutReal32(link, r32);
  }
  else if (strcmp(type, "MP_Real64Type") == 0) {
    fscanf(fd, "%lG %u", &r64, &annots);
    IMP_PutNodeHeader(link, MP_Real64Type, 0, 0, annots, 0);
    IMP_PutReal64(link, r64);
  }
  else if (strcmp(type, "MP_StringType") == 0) {
    fscanf(fd, "%s %u", str, &annots);
    IMP_PutNodeHeader(link, MP_StringType, 0, 0, annots, 0);
    IMP_PutString(link, str);
  }
  else if (strcmp(type, "MP_IdentifierType") == 0) {
    fscanf(fd, "%d %s %u", &dtag, str, &annots);
    IMP_PutNodeHeader(link, MP_IdentifierType, dtag, 0, annots, 00);
    IMP_PutIdentifier(link, str);
  }
  else if (strcmp(type, "MP_CommonLatinIdentifierType") == 0) {
    fscanf(fd, "%d %c %u", &dtag, &c, &annots);
    IMP_PutNodeHeader(link, MP_CommonLatinIdentifierType, dtag,c,annots,0);
  }
  else if (strcmp(type, "MP_CommonGreekIdentifierType") == 0) {
    fscanf(fd, "%d %c %u", &dtag, &c, &annots);
    IMP_PutNodeHeader(link, MP_CommonGreekIdentifierType, dtag,c,annots,0);
  }
  else if (strcmp(type, "MP_CommonConstantType") == 0) {
    fscanf(fd, "%d %d %u", &dtag, &cval, &annots);
    IMP_PutNodeHeader(link, MP_CommonConstantType, dtag, cval, annots, 0);
    }
  else if (strcmp(type, "MP_ConstantType") == 0) {
    fscanf(fd, "%d %s %u", &dtag, str, &annots); 
    MP_PutConstantPacket(link, dtag, str, annots);
  }
  else if (strcmp(type, "MP_MetaType") == 0) {
    fscanf(fd, "%d %s %u", &dtag, str, &annots);
    IMP_PutNodeHeader(link, MP_MetaType, 0, 0, annots, 0);
    IMP_PutMetaType(link, str);
  }
  else if (strcmp(type, "MP_RawType") == 0) {
    fscanf(fd, "%s %u", raw, &annots);
    IMP_PutNodeHeader(link, MP_RawType, 0, 0, annots, 0);
    IMP_PutRaw(link, raw, strlen(raw));
  }
  else if (strcmp(type, "MP_OperatorType") == 0) {
    fscanf(fd, "%d %s %u %u",&dtag, str, &annots, &num_children);
    IMP_PutNodeHeader(link, MP_OperatorType, dtag,0, annots, num_children);
    IMP_PutOperator(link, str);
  }
  else if (strcmp(type, "MP_CommonOperatorType") == 0) {
    fscanf(fd, "%d %d %u %u", &dtag, &cval, &annots, &num_children);
    IMP_PutNodeHeader(link, MP_CommonOperatorType, dtag, cval, annots, 
		      num_children);
      if (dtag == MP_MpDict  && cval == MP_CopMpEndSession) {
	return 0;
	}
    }
  /* first we get the annotations, if there are any */
  while (annots-- > 0) {
    if ((flags = imp_get_annotation(link, fd)) == 99) {
      fprintf(stderr,"Error in input file, annotation expected,");
      fprintf(stderr," received %s\n", badstring);
      exit(1);
    }
    if (flags & MP_AnnotValuated) 
      imp_get_term(link, fd);
  }
  
  /* now get the children */
  while (num_children-- > 0) imp_get_term(link, fd);
  return 1;
}


/**********************************************************************
 * FUNCTION: imp_file2tree
 * ARGUMENT: link - pointer to a stream structure
 *           fname - If not empty, this is a string giving the name 
 *                   of the input file (as given from the command line).  
 *                   If the string is empty, the routine asks for a
 *                   filename.  If the filename cannot be opened for
 *                   reading we print an error message and croak.
 * RETURN:   None.
 * PURPOSE:  Read successive trees from an input file and send them to
 *           the data stream associated with link.  
 *
 * COMMENT:  Note that m_endofrecord() is called after writing each
 *           tree to the buffer.  This flushes the buffer to the 
 *           network and sets the last fragment bit in the buffer 
 *           header.  The receiver must do a corrsponding m_skiprecord()
 *           PRIOR to retrieving EACH tree from the data stream.
 **********************************************************************/
void 
#ifdef _ANSI_
imp_file2tree(MP_Link_pt link, char *fname)
#else
imp_file2tree(link, fname)
MP_Link_pt link;
char *fname;
#endif
{
  FILE *fd;
  int more = 0;

  if (strlen(fname) == 0) {
    fname = malloc(32);
    printf("Enter name of input file: ");
    scanf("%s", fname);
    }

  if ((fd = fopen(fname, "r")) == NULL) {
    fprintf(stderr, "Can't open %s for reading\n", fname);
    return;
    }

  do {
    more = imp_get_term(link, fd);
    MP_EndMsgReset(link);
  } while (more);

  fclose(fd);
}

int get_outfilename(fname)
char *fname;
{
  int fd;

  if ((fd = open(fname, O_WRONLY | O_CREAT, 0600)) < 0)
    fprintf(stderr, "can't open %s for writing\n", fname);

  return fd;
}

main(argc, argv)
  int argc;
  char *argv[];
{
  char *ifname = NULL;
  MP_Link_pt link = NULL;
  MP_Env_pt  env = NULL;

  env = MP_InitializeEnv(NULL);
  if (env == NULL) {
    fprintf(stderr, "%s: MP_InitializeEnv() failed!\n", argv[0]);
    exit(1);
    }
  fprintf(stderr,"%s: MP_InitializeEnv() succeeded\n", argv[0]);

  if ((link = MP_OpenLink(env, argc, argv)) == NULL) {
    fprintf(stderr, "%s: MP_OpenLink() failed!\n", argv[0]);
    exit(1);
    }
  MP_SetLinkOption(link, MP_LINK_LOG_MASK_OPT, MP_LOG_INIT_EVENTS);
  MP_SetLinkOption(link, MP_LINK_LOG_MASK_OPT, MP_LOG_WRITE_EVENTS);
  MP_SetLinkOption(link, MP_LINK_SEND_MODE_OPT, MP_SEND_WHOLE_MSG);

  mpf_set_default_prec(256);  /* for the GMP arb prec reals */

  ifname = IMP_GetCmdlineArg(argc, argv, "-infile");

  if (ifname == NULL)  {
    MP_LogEvent(link, MP_ERROR_EVENT, "missing -infile argument");
    exit(-1);
    }
  fprintf(stderr, "ifname = %s\n", ifname);

  imp_file2tree(link, ifname); 

  MP_CloseLink(link);
  MP_ReleaseEnv(env);

  fprintf(stderr, "%s: All done!\n", argv[0]);

}
