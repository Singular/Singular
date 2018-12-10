#include "kernel/mod2.h"

#include "resources/feFopen.h"
#include "reporter/reporter.h"
#include "polys/mod_raw.h"

#include "Singular/mod_lib.h"

#include <ctype.h>
#include <sys/stat.h>
#include <errno.h>


#define SI_BUILTIN_LIBSTR(name) (char*) #name ".so",

const char * const si_builtin_libs[] = { SI_FOREACH_BUILTIN(SI_BUILTIN_LIBSTR) NULL };

#undef SI_BUILTIN_LIBSTR

#define BYTES_TO_CHECK 7

lib_types type_of_LIB(const char *newlib, char *libnamebuf)
{
  const unsigned char mach_o[]={0xfe,0xed,0xfa,0xce,0};
  const unsigned char mach_O[]={0xce,0xfa,0xed,0xfe,0};

  const unsigned char mach_o64[]={0xfe,0xed,0xfa,0xcf,0};
  const unsigned char mach_O64[]={0xcf,0xfa,0xed,0xfe,0};

  const unsigned char mach_FAT[]={0xca,0xfe,0xba,0xbe,0};
  const unsigned char mach_fat[]={0xbe,0xba,0xfe,0xca,0};

  const unsigned char utf16be[]={0xfe,0xff,0};
  const unsigned char utf16le[]={0xff,0xfe,0};
  const unsigned char utf8ms[]={0xEF,0xBB,0xBF,0};

  const unsigned char dll[]={'M','Z',0};
  int i=0;
  while(si_builtin_libs[i]!=NULL)
  {
    if (strcmp(newlib,si_builtin_libs[i])==0)
    {
      if(libnamebuf!=NULL) strcpy(libnamebuf,newlib);
      return LT_BUILTIN;
    }
    i++;
  }
  char        buf[BYTES_TO_CHECK+1];        /*!
!
 one extra for terminating '\0' 

*/
  struct stat sb;
  int nbytes = 0;
  int ret;
  lib_types LT=LT_NONE;

  FILE * fp = feFopen( newlib, "r", libnamebuf, FALSE );

  do
  {
    ret = stat(libnamebuf, &sb);
  } while((ret < 0) and (errno == EINTR));

  if (fp==NULL)
  {
    return LT_NOTFOUND;
  }
  if((sb.st_mode & S_IFMT) != S_IFREG)
  {
    goto lib_type_end;
  }
  if ((nbytes = fread((char *)buf, sizeof(char), BYTES_TO_CHECK, fp)) == -1)
  {
    goto lib_type_end;
    /*!
!
NOTREACHED

*/
  }
  if (nbytes == 0)
    goto lib_type_end;
  else
  {
    buf[nbytes++] = '\0';        /*!
!
 null-terminate it 

*/
  }
  if( (strncmp(buf, "\177ELF", 4)==0)) /*!
!
 generic ELF 

*/
  {
    LT = LT_ELF;
    //omFree(newlib);
    //newlib = omStrDup(libnamebuf);
    goto lib_type_end;
  }

  if( (strncmp(buf, (const char *)mach_o, 4)==0) || (strncmp(buf, (const char *)mach_O, 4)==0)) /*!
!
 generic Mach-O module 

*/
  {
    LT = LT_MACH_O;
    //omFree(newlib);
    //newlib = omStrDup(libnamebuf);
    goto lib_type_end;
  }

  if( (strncmp(buf, (const char *)mach_o64, 4)==0) || (strncmp(buf, (const char *)mach_O64, 4)==0)) /*!
!
 generic Mach-O 64-bit module 

*/
  {
    LT = LT_MACH_O;
    //omFree(newlib);
    //newlib = omStrDup(libnamebuf);
    goto lib_type_end;
  }

  if( (strncmp(buf, (const char *)mach_FAT, 4)==0) || (strncmp(buf, (const char *)mach_fat, 4)==0)) /*!
!
 generic Mach-O fat universal module 

*/
  {
    LT = LT_MACH_O;
    //omFree(newlib);
    //newlib = omStrDup(libnamebuf);
    goto lib_type_end;
  }

  if( (strncmp(buf, "\02\020\01\016\05\022@", 7)==0))
  {
    LT = LT_HPUX;
    //omFree(newlib);
    //newlib = omStrDup(libnamebuf);
    goto lib_type_end;
  }
  if ((strncmp(buf,(const char *)utf16be,2)==0)
  ||(strncmp(buf,(const char *)utf16le,2)==0))
  {
    WerrorS("UTF-16 not supported");
    LT=LT_NOTFOUND;
    goto lib_type_end;
  }
  if (strncmp(buf,(const char *)utf8ms,3)==0)
  {
    WarnS("UTF-8 detected - may not work");
    LT=LT_SINGULAR;
    goto lib_type_end;
  }
  if (strncmp(buf,(const char *)dll,2)==0)
  {
    LT=LT_DLL;
    goto lib_type_end;
  }
  if(isprint(buf[0]) || buf[0]=='\n')
  { LT = LT_SINGULAR; goto lib_type_end; }

  lib_type_end:
  fclose(fp);
  return LT;
}
