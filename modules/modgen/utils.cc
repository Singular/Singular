/*
 * $Id: utils.cc,v 1.12 2002-07-03 12:42:51 anne Exp $
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

int modlineno;    /* lineno within module */

#include "modgen.h"

void init_system_type();

/*========================================================================*/
int init_modgen(
  moddefv module_def,
  char *filename
)
{
  char tmpfile[64];
  char *p, *q;
  
  modlineno = 0;
  
  if(module_def == NULL) return -1;
  memset(module_def, '\0', sizeof(moddef));

  if ( (q=strrchr(filename, '/')) == NULL) q = filename;
  else q++;
  
  module_def->filename = (char *)malloc(strlen(q)+1);
  if(module_def->filename != NULL ) {
    memset(module_def->filename, '\0', strlen(q)+1);
    memcpy(module_def->filename, q, strlen(q));
  }
  strncpy(tmpfile, q, sizeof(tmpfile));
  p=strrchr(tmpfile, '.');
  if(p!=NULL) *p='\0';
  module_def->name = (char *)malloc(strlen(tmpfile)+1);
  memset(module_def->name, '\0', strlen(tmpfile)+1);
  memcpy(module_def->name, tmpfile, strlen(tmpfile));
  
  init_system_type();
  
  return (create_tmpfile(module_def));
}

/*========================================================================*/
int create_tmpfile(
  moddefv module_def,
  int which
)
{
  char tmpfile[64];
  FILE *fp;
  
  memset(tmpfile, '\0', sizeof(tmpfile));
  snprintf(tmpfile, sizeof(tmpfile), "modgen.tmpXXXXXX");
  mktemp(tmpfile);

  if(debug)printf("create_tmpfile '%s'\n", tmpfile );
  
  if (close(creat(tmpfile, 0600)) < 0) {
    (void) unlink (tmpfile);        /*  Blow it away!!  */
    return -1;
  } else if ((fp = fopen(tmpfile, "a+")) == NULL) {
    (void) unlink (tmpfile);        /*  Blow it away!!  */
    return -1;
  } else {
    (void) unlink (tmpfile); /* delete now to avoid turds... */
  }

  switch(which) {
      case 0: module_def->fmtfp  = fp; break;
      case 1: module_def->fmtfp2 = fp; break;
      case 2: module_def->fmtfp3 = fp; break;
      default:
        fclose(fp);
        return -1;
  }
  
  return 0;
}

/*========================================================================*/
char *build_filename(
  moddefv module,
  char *text,
  int what
)
{
  static char p[512];

  switch(what) 
    {
        case 1:
          snprintf(p, sizeof(p), "%s/%s.cc", module->name, text);
          break;
        case 2:
          snprintf(p, sizeof(p), "%s/%s.h", module->name, text);
          break;
        case 3:
          snprintf(p, sizeof(p), "%s/%s.bin", module->name, text);
          break;
        case 4:
	  snprintf(p, sizeof(p), "%s/%s.pl", module->name, text);
	  break;
        default:
          snprintf(p, sizeof(p), "%s/%s", module->name, text); 
          break;
    }
  
  return p;
}

/*========================================================================*/
int myyyerror(
  char *fmt, ...
  )
{
  va_list ap;
  va_start(ap, fmt);
  vprintf(fmt, ap);
  va_end(ap);
  return 2;
}
