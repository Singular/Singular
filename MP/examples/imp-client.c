/*
  imp-client.c - simple client process 
*/

#include <fcntl.h>
#include "MP.h"
#include "gmp.h"

/*
   imp_get_input() - routine to read sample data from a file.  The data is
		 assumed to be correct.  This routine knows nothing 
		 about tree structure.  It simply creates and sends
		 packets based on the information in the input file.
		 However the server examples knows something of trees
		 and may croak if the input isn't in the correct format.
*/
   
void 
#ifdef _ANSI_
imp_get_input(MP_Link_pt link, char *fname)
#else
imp_get_input(link, fname)
MP_Link_pt link;
char *fname;
#endif
{
  FILE *fd;
  char type[32], str[64], raw[512], c;
  MP_ApInt_t apint;
  MP_ApReal_t apreal;
  MP_NodeHeader_t hdr;
  MP_AnnotType_t atype;
  unsigned long dtag,cval;
  int i, annots;
  unsigned int  my_uint, flags;
  int sint, done = 0;
  float  r32;
  double r64;

  if (strlen(fname) == 0) {
    fname = (char *)malloc(32);
    printf("Enter name of input file: ");
    scanf("%s", fname);
    }

  if ((fd = fopen(fname, "r")) == NULL) {
    fprintf(stderr, "Can't open %s for reading\n", fname);
    return;
    }
 
  while (1) {
    atype = 0;
    fscanf(fd, "%s", type); 
    if (strcmp(type, "MP_Sint32Type") == 0) {
      fscanf(fd, "%d %d", &sint, &annots);
      IMP_PutNodeHeader(link, MP_Sint32Type, 0, 0, annots, 0);
      IMP_PutSint32(link, sint);
      }
    else if (strcmp(type, "MP_Uint32Type") == 0) {
      fscanf(fd, "%u %d", &my_uint, &annots);
      IMP_PutNodeHeader(link, MP_Uint32Type, 0, 0, annots, 0);
      IMP_PutUint32(link, my_uint);
      }
    else if (strcmp(type, "MP_Sint8Type") == 0) {
      fscanf(fd, "%d %d", &sint, &annots);
      MP_PutSint8Packet(link, sint, annots);
       }
    else if (strcmp(type, "MP_Uint8Type") == 0) {
      fscanf(fd, "%d %d", &my_uint, &annots);
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
      fscanf(fd, "%d ", &annots);
      IMP_PutNodeHeader(link, MP_ApIntType, 0, 0, annots, 0);
      IMP_PutApInt(link, &apint);
      mpz_clear((mpz_ptr)&apint); 
      }
     else if (strcmp(type, "MP_ApRealType") == 0) {
      mpf_init((mpf_ptr)&apreal);
      /* now get the apreal from the file */
      mpf_inp_str((mpf_ptr)&apreal, fd, 10);
      fscanf(fd, "%d ", &annots);
      IMP_PutNodeHeader(link, MP_ApRealType, 0, 0, annots, 0);
      IMP_PutApReal(link, &apreal);
      mpf_clear((mpf_ptr)&apreal); 
      }
    else if (strcmp(type, "MP_Real32Type") == 0) {
      fscanf(fd, "%f %d", &r32, &annots);
      IMP_PutNodeHeader(link, MP_Real32Type, 0, 0, annots, 0);
      IMP_PutReal32(link, r32);
      }
    else if (strcmp(type, "MP_Real64Type") == 0) {
      fscanf(fd, "%lG %d", &r64, &annots);
      IMP_PutNodeHeader(link, MP_Real64Type, 0, 0, annots, 0);
      IMP_PutReal64(link, r64);
      }
    else if (strcmp(type, "MP_StringType") == 0) {
      fscanf(fd, "%s %d", str, &annots);
      IMP_PutNodeHeader(link, MP_StringType, 0, 0, annots, 0);
      IMP_PutString(link, str);
      }
    else if (strcmp(type, "MP_IdentifierType") == 0) {
      fscanf(fd, "%d %s %d", &dtag, str, &annots);
      IMP_PutNodeHeader(link, MP_IdentifierType, dtag, 0, annots, 0);
      IMP_PutIdentifier(link, str);
      }
    /* there are no IMP routines for common types */
    else if (strcmp(type, "MP_CommonLatinIdentifierType") == 0) {
      fscanf(fd, "%d %s %d", &dtag, str, &annots);
      MP_PutCommonLatinIdentifierPacket(link, dtag, *str, annots);
      }
    else if (strcmp(type, "MP_CommonGreekIdentifierType") == 0) {
      fscanf(fd, "%d %s %d", &dtag, str, &annots);
      MP_PutCommonGreekIdentifierPacket(link, dtag, *str, annots);
      }
    else if (strcmp(type, "MP_ConstantType") == 0) {
      fscanf(fd, "%d %s %d", &dtag, str, &annots); 
      IMP_PutNodeHeader(link, MP_ConstantType, dtag, 0, annots, 0);
      IMP_PutString(link, str);
     }
    else if (strcmp(type, "MP_CommonConstantType") == 0) {
      fscanf(fd, "%d %d %d", &dtag, &cval, &annots);
      IMP_PutNodeHeader(link, MP_CommonConstantType, dtag, cval, annots, 0);
      }
    else if (strcmp(type, "MP_MetaType") == 0) {
      fscanf(fd, "%d %s %d", &dtag, str, &annots);
      IMP_PutNodeHeader(link, MP_MetaType, dtag, 0, annots, 0);
      IMP_PutMetaType(link, str);
      }
    else if (strcmp(type, "MP_RawType") == 0) {
      fscanf(fd, "%s %d", raw, &annots);
      IMP_PutNodeHeader(link, MP_RawType, annots, 0, 0, 0);
      IMP_PutRaw(link, raw, strlen(raw));
      }
    else if (strcmp(type, "MP_OperatorType") == 0) {
      fscanf(fd, "%d %s %d %u",&dtag, str, &annots, &my_uint);
      IMP_PutNodeHeader(link, MP_OperatorType, dtag, 0, annots, my_uint);
      IMP_PutOperator(link, str);
      }
    else if (strcmp(type, "MP_CommonOperatorType") == 0) {
      fscanf(fd, "%d %d %d %u", &dtag, &cval, &annots, &my_uint);
      IMP_PutNodeHeader(link, MP_CommonOperatorType, dtag, cval, annots, 
			 my_uint);
      if (dtag == MP_MpDict  && cval == MP_CopMpEndSession) {
	MP_EndMsgReset(link); /* make sure stuff gets flushed */
	return;
	}
      }
    else if(strcmp(type, "Label_AP") == 0) atype = MP_AnnotMpLabel;
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
      printf("Error in input file\n");
      exit (1);
      }
    if (atype != 0) {
      fscanf(fd, "%d  %X", &dtag, &flags);
      MP_PutAnnotationPacket(link, dtag, atype, flags);
      }
   }
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

  if ((link = MP_OpenLink(env, argc, argv)) == NULL) {
    fprintf(stderr, "%s: MP_OpenLink() failed!\n", argv[0]);
    exit(1);
    }
  MP_SetLinkOption(link, MP_LINK_LOG_MASK_OPT, MP_LOG_INIT_EVENTS);
  MP_SetLinkOption(link, MP_LINK_LOG_MASK_OPT, MP_LOG_WRITE_EVENTS);
  MP_SetLinkOption(link, MP_LINK_LOG_MASK_OPT, MP_LOG_READ_EVENTS);

  ifname = IMP_GetCmdlineArg(argc, argv, "-infile");

  if (ifname == NULL)  {
    MP_LogEvent(link, MP_ERROR_EVENT, "missing -infile argument");
    exit(-1);
    }

  mpf_set_default_prec(256);  /* for the GMP arb prec reals */

  imp_get_input(link, ifname); 

  MP_CloseLink(link);
  MP_ReleaseEnv(env);
}
