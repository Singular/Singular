/* 
   imp-server.c - simple server process using AF_UNIX sockets
	      taken from Paul Wang's book on Unix - p. 341
*/

#include <stdio.h>
#include <fcntl.h>
#include "MP.h"
#include "gmp.h"

extern char * annot_to_str();

/*
   The imp_get_tree routines read nodes from the stream.  They routine
   know about the structure of a tree (that is, that operators have
   children and non-operators do not), but don't do any error
   checking.  imp_get_tree() recursively retrieves subtrees and the
   trees of valuated annotations.  
*/

int
#ifdef _ANSIC_
imp_get_tree(MP_Link_pt link, FILE *fd)
#else
imp_get_tree(link, fd)
MP_Link_pt link;
FILE *fd;
#endif
{
  MP_NodeType_t node_type = 0;
  MP_AnnotType_t annot_type = 0;
  MP_NumAnnot_t num_annots = 0;
  MP_NumChild_t num_child = 0;
  MP_NodeHeader_t hdr;
  MP_DictTag_t dtag;
  MP_Common_t cval;
  MP_ApInt_t apint = NULL;
  MP_ApInt_t apreal = NULL;
  MP_Sint8_t sin8;
  MP_Uint8_t uin8;
  MP_Sint32_t n;
  MP_Uint32_t un, len;
  MP_Real32_t sf;
  MP_Real64_t df;
  MP_AnnotFlags_t flags;
  char c, *s = NULL, *s2 = NULL, *annotstr = NULL;
  int  i, more = 1;

  if (IMP_GetNodeHeader(link, &node_type, &dtag, &cval, &num_annots, 
			&num_child) != MP_Success) {
    fprintf(stderr, "imp-server: IMP_GetNodeHeader failed\n");
    MP_PrintError(link);
  }
  switch(node_type) {
  case MP_Sint32Type: 
    IMP_GetSint32(link, &n);
    printf("%-20s       %-30d %d\n", "MP_Sint32", n, num_annots);
    fprintf(fd,"MP_Sint32Type %d %d\n", n, num_annots);
    break;
  case MP_Uint32Type: 
    IMP_GetUint32(link, &un);
    printf("%-20s       %-30u %d\n", "MP_Uint32", un, num_annots);
    fprintf(fd, "MP_Uint32Type %u %d\n", un, num_annots);
    break;
  case MP_Sint8Type: 
    sin8 = cval;
    printf("%-20s       %-30d %d\n", "MP_Sint8", sin8, num_annots);
    fprintf(fd,"MP_Sint8Type %d %d\n", sin8, num_annots);
    break;
  case MP_Uint8Type: 
    uin8 = cval;
    printf("%-20s       %-30d %d\n", "MP_Uint8", uin8, num_annots);
    fprintf(fd, "MP_Uint8Type %u %d\n", uin8, num_annots);
    break;
  case MP_BooleanType: 
    c = (cval == 1) ? 'T': 'F';
    printf("%-20s       %-30c %d\n", "MP_Boolean", c, num_annots);
    fprintf(fd, "MP_BooleanType %c %d\n", c, num_annots);
    break;
  case MP_Real32Type: 
    IMP_GetReal32(link, &sf);
    printf("%-20s       %-30.10G %d\n", "MP_Real32", sf, num_annots);
    fprintf(fd, "MP_Real32Type %20.10G %d\n", sf, num_annots);
    break;
  case MP_Real64Type: 
    IMP_GetReal64(link, &df);
    printf("%-20s       %-30.15lG %d\n", "MP_Real64", df, num_annots);
    fprintf(fd, "MP_Real64Type %20.15lG %d\n", df, num_annots);
    break;
  case MP_CommonLatinIdentifierType: 
    printf("%-20s %-5d %-30c %-7d\n", "MP_LatinIdentifier", dtag, cval, 
	   num_annots);
    fprintf(fd, "MP_CommonLatinIdentifierType %d %c %d\n",
	    dtag, cval, num_annots);
    break;
  case MP_CommonGreekIdentifierType: 
    printf("%-20s %-5d %-30c %-7d\n", "MP_GreekIdentifier", dtag, cval, 
	   num_annots);
    fprintf(fd, "MP_CommonGreekIdentifierType %d %c %d\n", dtag,
	     cval, num_annots);
    break;
  case MP_IdentifierType: 
    IMP_GetIdentifier(link, &s);
    printf("%-20s %-5d %-30s %-7d\n", "MP_Identifier", dtag, s,  num_annots);
    fprintf(fd, "MP_IdentifierType %d %s %d\n", dtag, s, num_annots);
    free(s); s = NULL;
    break;
  case MP_CommonConstantType:	
    printf("%-20s %-5d %-30d %-7d\n", "MP_CommonCnst", dtag, cval, 
	   num_annots);
    fprintf(fd, "MP_CommonConstantType %d %d %d\n", dtag, cval, num_annots);
    break;
  case MP_ConstantType: 
    IMP_GetString(link, &s);
    printf("%-20s %-5d %-30s %-7d\n", "MP_Constant", dtag, s,  num_annots);
    fprintf(fd, "MP_ConstantType %d %s %d\n", dtag, s, num_annots);
    free(s); s = NULL;
    break;
  case MP_StringType: 
    IMP_GetString(link, &s);
    printf("%-20s       %-20s %d\n", "MP_String", s, num_annots);
    fprintf(fd, "MP_StringType %s %d\n", s, num_annots);
    free(s); s = NULL;
    break;
  case MP_MetaType: 
    IMP_GetMetaType(link, &s);
    printf("%-20s %-5d %-30s %-7d\n", "MP_Meta", dtag, s,  num_annots);
    fprintf(fd, "MP_MetaType %d %s %d\n", dtag, s, num_annots);
    free(s); s = NULL;
    break;
  case MP_RawType: 
    IMP_GetRaw(link, &s, &len);
    s2 = malloc(len+1);
    memcpy(s2, s, len);
    s2[len] = '\0';
    printf("%-20s       %-30s %d\n", "MP_Raw", s2, num_annots);
    fprintf(fd, "MP_RawType %s %d\n", s2, num_annots);
    free(s); free(s2); s = NULL;
    break;
  case MP_ApIntType: 
    IMP_GetApInt(link, &apint);
    printf("%-20s       ", "MP_ApInt");
    fprintf(fd, "MP_ApIntType ");
    mpz_out_str(stdout,10, (mpz_ptr)apint); 
    mpz_out_str(fd, 10, (mpz_ptr)apint); 
    free(s); s = NULL;mpz_clear((mpz_ptr)apint);
    printf(" %d\n", num_annots);
    fprintf(fd, " %d\n", num_annots);
    break;
  case MP_ApRealType: 
    IMP_GetApReal(link, &apreal);
    printf("%-20s       ", "MP_ApReal");
    fprintf(fd, "MP_ApRealType "); 
    mpf_out_str(stdout, 10, 0, (mpf_ptr)apreal); 
    mpf_out_str(fd, 10, 0, (mpf_ptr)apreal); ;
    free(s); s = NULL; mpf_clear((mpf_ptr)apreal);
    printf(" %d\n",num_annots);
    fprintf(fd, " %d\n",num_annots); fflush(fd);
    break;
  case MP_OperatorType: 
    IMP_GetOperator(link, &s);
    printf("%-20s %-5d %-30s %-7d %-9d\n", "MP_Op",
	   dtag, s, num_annots, num_child);
    fprintf(fd, "MP_OperatorType %d %s %d %d\n", dtag, s, num_annots,
	    num_child);
    IMP_RawMemFreeFnc(s); s = NULL;
    break;
  case MP_CommonOperatorType:	
    printf("%-20s %-5d %-30d %-7d% -5d\n", "MP_CommonOp", dtag, cval, 
	   num_annots, num_child);
    fprintf(fd, "MP_CommonOperatorType %d %d %d %d\n", dtag, cval,
	    num_annots, num_child);
    if (!(more = !(dtag == MP_MpDict && cval == MP_CopMpEndSession)))
      return 0;
    break;
  default: fprintf(stderr, "server: unknown type %d\n", node_type);
    return 0;
  }

      /* first we get the annotations, if there are any */
  while (num_annots-- > 0) {
    MP_GetAnnotationPacket(link,  &dtag, &annot_type,&flags);
    annotstr = annot_to_str(dtag, annot_type);
    printf("%-20s %-5d flags: 0x%X\n", annotstr, dtag, flags);
    fprintf(fd,"%s  %d  %X\n", annotstr, dtag, flags);
    if (flags & MP_AnnotValuated) imp_get_tree(link, fd);
  }
  
  /* now get the children */
  while (num_child-- > 0) imp_get_tree(link, fd);
  fflush(fd);
  return 1;
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
  int   n = -1;
  FILE  *fd;
  MP_Link_pt link = NULL; 
  MP_Env_pt  env = NULL;

  env = MP_InitializeEnv(env);
  if (env == NULL) {
    fprintf(stderr, "%s: MP_EnvInitialize() failed!\n", argv[0]);
    exit(1);
    }
 
  if ((link = MP_OpenLink(env, argc, argv)) == NULL) {
    fprintf(stderr, "%s: MP_OpenLink() failed!\n", argv[0]);
    exit(1);
    }
  MP_SetLinkOption(link, MP_LINK_LOG_MASK_OPT, MP_LOG_INIT_EVENTS);
  MP_SetLinkOption(link, MP_LINK_LOG_MASK_OPT, MP_LOG_READ_EVENTS);
  MP_SetLinkOption(link, MP_LINK_LOG_MASK_OPT, MP_LOG_WRITE_EVENTS);

  fd = fopen("test.out", "w");

  printf("server's annotated tree(s):\n");
  MP_SkipMsg(link);
  do {
    if (MP_TestEofMsg(link)) MP_SkipMsg(link);
    printf("\n\n%-20s %-5s %-30s %-7s %-9s\n", "Type", "Dict", "Value", 
             "#Annots", "#Children");
  } while (imp_get_tree(link, fd));
  
  fflush(fd); fclose(fd);
  MP_CloseLink(link);
  MP_ReleaseEnv(env);
}
