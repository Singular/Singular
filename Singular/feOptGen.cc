/*!
!
***************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/*!
!

* ABSTRACT: Implementation of option business


*/


#include "kernel/mod2.h"

#define FE_OPT_STRUCTURE

#include "feOptGen.h"

#include "fehelp.h"

const char SHORT_OPTS_STRING[] = "bdhqstvxec:r:u:";

//////////////////////////////////////////////////////////////
//
// Generation of feOptIndex
//
#include <stdio.h>
#include <stdlib.h>
int main()
{
  FILE* fd;
#ifdef ESINGULAR
  fd = fopen("feOptES.xx", "w");
#elif defined(TSINGULAR)
  fd = fopen("feOptTS.xx", "w");
#else
  fd = fopen("feOpt.xx", "w");
#endif

  if (fd == NULL) exit(1);

  int i = 0;

  fputs("typedef enum\n{\n", fd);

  while (feOptSpec[i].name != NULL)
  {
    const char* name = feOptSpec[i].name;
    fputs("FE_OPT_", fd);
    while (*name != 0)
    {
      if (*name == '-')
      {
        putc('_', fd);
      }
      else if (*name >= 97 && *name <= 122)
      {
        putc(*name - 32, fd);
      }
      else
      {
        putc(*name, fd);
      }
      name++;
    }
    if (i == 0)
    {
      fputs("=0", fd);
    }
    i++;
    fputs(",\n  ", fd);
  }

  fprintf(fd, "FE_OPT_UNDEF\n} feOptIndex;\n");
  fclose(fd);
#ifdef ESINGULAR
  rename("feOptES.xx", "feOptES.inc");
#elif defined(TSINGULAR)
  rename("feOptTS.xx", "feOptTS.inc");
#else
  rename("feOpt.xx", "feOpt.inc");
#endif
  return(0);
}
