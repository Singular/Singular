/* 
   server.c - simple server process
*/

#include <stdio.h>
#include <fcntl.h>
#include "MP.h"
#include "gmp.h"

extern char * annot_to_str();   /* from util.c */

void * my_alloc(size_t n)
{
   printf("in my_alloc allocating %d bytes\n", n);
   return malloc(n);
}

void * my_realloc(void * oldptr, size_t olds, size_t news)
{
   void * newptr = NULL;

   printf("in my_realloc: old size = %d, new size = %d\n", olds, news); 
   newptr = realloc(oldptr, news);
   printf("\told address of ptr is 0x%X, new address is 0x%X\n", oldptr,
	newptr);
   return newptr;
}

void my_free(void * ptr, size_t n)
{
   printf("in my_free, freeing %d bytes at address 0x%X\n", n, ptr);
   free(ptr);
}


/*
   get_tree() - routine to read nodes from the stream.  This routine
		knows about the structure of a tree (that is, that 
		operators have children and non-operators do not),
		but doesn't do any error checking.  It recursively
		retrieves subtrees and the trees of valuated 
		annotations.
*/

int
#ifdef _ANSIC_
get_tree(MP_Link_pt link, FILE *fd)
#else
get_tree(link, fd)
MP_Link_pt link;
FILE *fd;
#endif
{
  MP_NodeType_t node_type = 0;
  MP_AnnotType_t annot_type = 0;
  MP_NumAnnot_t num_annots = 0;
  MP_NumChild_t num_child = 0;
  MP_NodeHeader_t hdr;
  MP_DictTag_t dtag = 0; 
  MP_Common_t cval;
  MP_ApInt_t apint = NULL;
  MP_ApReal_t apreal = NULL;
  MP_Boolean_t b;
  MP_Sint8_t sin8;
  MP_Uint8_t uin8;
  MP_Sint32_t n;
  unsigned long len;
  MP_Uint32_t un;
  MP_Real32_t sf;
  MP_Real64_t df;
  MP_AnnotFlags_t flags;
  char c, *s = NULL, *s2 = NULL, *annotstr = NULL;
  int  i, more = 1;
  if (MP_PeekHeader(link, &node_type, &dtag, &cval) == MP_Failure) {
      MP_PrintError(link);
      exit(1);
      }
  switch(node_type) {
  case MP_Sint32Type:
    MP_GetSint32Packet(link, &n, &num_annots);
    printf("%-20s       %-30d %d\n", "MP_Sint32", n, num_annots);
    fprintf(fd,"MP_Sint32Type %d %d\n", n, num_annots);
    break;
  case MP_Uint32Type:
    MP_GetUint32Packet(link, &un, &num_annots);
    printf("%-20s       %-30u %d\n", "MP_Uint32", un, num_annots);
    fprintf(fd, "MP_Uint32Type %u %d\n", un, num_annots);
    break;
  case MP_Sint8Type:
    MP_GetSint8Packet(link, &sin8, &num_annots);
    printf("%-20s       %-30d %d\n", "MP_Sint8", sin8, num_annots);
    fprintf(fd,"MP_Sint8Type %d %d\n", sin8, num_annots);
    break;
  case MP_Uint8Type:
    MP_GetUint8Packet(link, &uin8, &num_annots);
    printf("%-20s       %-30d %d\n", "MP_Uint8", uin8, num_annots);
    fprintf(fd, "MP_Uint8Type %u %d\n", uin8, num_annots);
    break;
  case MP_BooleanType:
    MP_GetBooleanPacket(link, &b, &num_annots);
    c = (b == 1) ? 'T' : 'F';
    printf("%-20s       %-30c %d\n", "MP_Boolean", c, num_annots);
    fprintf(fd, "MP_BooleanType %c %d\n", c, num_annots);
    break;
  case MP_Real32Type:
    MP_GetReal32Packet(link, &sf, &num_annots);
    printf("%-20s       %-30.10G %d\n", "MP_Real32", sf, num_annots);
    fprintf(fd, "MP_Real32Type %20.10G %d\n", sf, num_annots);
    break;
  case MP_Real64Type:
    MP_GetReal64Packet(link, &df, &num_annots);
    printf("%-20s       %-30.15lG %d\n", "MP_Real64", df, num_annots);
    fprintf(fd, "MP_Real64Type %20.15lG %d\n", df, num_annots);
    break;
  case MP_CommonLatinIdentifierType:
    MP_GetCommonLatinIdentifierPacket(link, &dtag, (MP_Common_t *)&c,  
	&num_annots);
    printf("%-20s %-5d %-30c %-7d\n", "MP_LatinIdentifier", dtag, c, 
	   num_annots);
    fprintf(fd, "MP_CommonLatinIdentifierType %d %c %d\n",
	    dtag, c, num_annots);
    break;
  case MP_CommonGreekIdentifierType:
    MP_GetCommonGreekIdentifierPacket(link, &dtag, (MP_Common_t *)&c, 
	&num_annots);
    printf("%-20s %-5d %-30c %-7d\n", "MP_GreekIdentifier", dtag, c, 
	   num_annots);
    fprintf(fd, "MP_CommonGreekIdentifierType %d %c %d\n", dtag,
	     c, num_annots);
    break;
  case MP_IdentifierType:
    MP_GetIdentifierPacket(link,&dtag,  &s, &num_annots);
    printf("%-20s %-5d %-30s %-7d\n", "MP_Identifier", dtag, s,  num_annots);
    fprintf(fd, "MP_IdentifierType %d %s %d\n", dtag, s, num_annots);
    free(s);
    break;
  case MP_ConstantType:
    MP_GetConstantPacket(link, &dtag, &s, &num_annots);
    printf("%-20s %-5d %-30s %-7d\n", "MP_Constant", dtag, s,  num_annots);
    fprintf(fd, "MP_ConstantType %d %s %d\n", dtag, s, num_annots);
    free(s);
    break;
  case MP_CommonConstantType:
    MP_GetCommonConstantPacket(link, &dtag, &cval, &num_annots);
    printf("%-20s %-5d %-30d %-7d\n", "MP_CommonCnst", dtag, cval, 
	   num_annots);
    fprintf(fd, "MP_CommonConstantType %d %d %d\n", dtag, cval, num_annots);
    break;
  case MP_StringType:
    MP_GetStringPacket(link, &s, &num_annots);
    printf("%-20s       %-20s %d\n", "MP_String", s, num_annots);
    fprintf(fd, "MP_StringType %s %d\n", s, num_annots);
    free(s);
    break;
  case MP_MetaType:
    MP_GetMetaTypePacket(link, &dtag, &s, &num_annots);
    printf("%-20s %-5d %-30s %-7d\n", "MP_Meta", dtag, s,  num_annots);
    fprintf(fd, "MP_MetaType %d %s %d\n", dtag, s, num_annots);
    free(s);
    break;
  case MP_RawType:
    MP_GetRawPacket(link, &s, &len, &num_annots);
    s2 = malloc(len+1);
    memcpy(s2, s, len);
    s2[len] = '\0';
    printf("%-20s       %-30s %d\n", "MP_Raw", s2, num_annots);
    fprintf(fd, "MP_RawType %s %d\n", s2, num_annots);
    free(s); free(s2);
    break;
  case MP_ApIntType:
    MP_GetApIntPacket(link, &apint, &num_annots);
    printf("%-20s       ", "MP_ApInt");
    fprintf(fd, "MP_ApIntType ");
    mpz_out_str(stdout, 10, (mpz_srcptr)apint);
    mpz_out_str(fd, 10, (mpz_srcptr)apint);
    free(s);mpz_clear((mpz_ptr)apint);
    printf(" %d\n", num_annots);
    fprintf(fd, " %d\n", num_annots);
    break;
  case MP_ApRealType:
    MP_GetApRealPacket(link, &apreal, &num_annots);
    printf("%-20s       ", "MP_ApReal");
    fprintf(fd, "MP_ApRealType "); fflush(fd);
    mpf_out_str(stdout, 10, 0, (mpf_srcptr)apreal);
    mpf_out_str(fd, 10, 0, (mpf_srcptr)apreal); 
    free(s);mpf_clear((mpf_ptr)apreal);
    printf(" %d\n",num_annots);
    fprintf(fd, " %d\n",num_annots); fflush(fd);
    break;
  case MP_OperatorType:
    MP_GetOperatorPacket(link, &dtag, &s, &num_annots, &num_child);
    printf("%-20s %-5d %-30s %-7d %-9d\n", "MP_Op",
	   dtag, s, num_annots, num_child);
    fprintf(fd, "MP_OperatorType %d %s %d %d\n", dtag, s, num_annots,
	    num_child);
    IMP_RawMemFreeFnc(s);
    break;
  case MP_CommonOperatorType:
    MP_GetCommonOperatorPacket(link, &dtag, &cval, &num_annots,
			       &num_child);
    printf("%-20s %-5d %-30d %-7d% -5d\n", "MP_CommonOp", dtag, cval, 
	   num_annots, num_child);
    fprintf(fd, "MP_CommonOperatorType %d %d %d %d\n", dtag, cval,
	    num_annots, num_child);
    if (!(more = !(dtag == MP_MpDict && cval == MP_CopMpEndSession)))
      return 0;
    break;
  default : fprintf(stderr, "server: unknown type %d\n", node_type);
    return 0;
  }

  /* first we get the annotations, if there are any */
  while (num_annots-- > 0) {
    MP_GetAnnotationPacket(link, &dtag, &annot_type, &flags);
    annotstr = annot_to_str(dtag, annot_type);
    printf("%-20s %-5d flags: 0x%X\n", annotstr, dtag, flags);
    fprintf(fd,"%s  %d  %X\n", annotstr, dtag, flags);
    if (flags & MP_AnnotValuated) get_tree(link, fd);
  }

  /* now get the children */
  while (num_child-- > 0) get_tree(link, fd);

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
  long n = -1;
  unsigned long ui;
  long i, *ivec = NULL;
  char *s=NULL, cop;
  float f;
  double d;
  MP_NumChild_t nc;
  MP_NumAnnot_t na;
  MP_DictTag_t dtag;
  MP_Link_pt link = NULL;
  MP_Env_pt  env = NULL;
  FILE         *fd;
  MP_FILE_t *file;


#ifdef __WIN32__
   if (WinSockInitialize() != 0)
      exit(1);
#endif

 if ((env = MP_InitializeEnv(env)) == NULL) {
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

  mpf_set_default_prec(256);  /* for the GMP arb prec reals */

  fd = fopen("test.out", "w");

  if (MP_TestEofMsg(link))  MP_SkipMsg(link);
  printf("server's annotated tree(s):\n");
  do {
    if (MP_TestEofMsg(link)) MP_SkipMsg(link);
    printf("\n\n%-20s %-5s %-30s %-7s %-9s\n", "Type", "Dict", "Value", 
             "#Annots", "#Children");
  } while (get_tree(link, fd));

  fflush(fd); fclose(fd);

  MP_CloseLink(link);
  MP_ReleaseEnv(env);

#ifdef __WIN32__
  WSACleanup();
#endif
}
