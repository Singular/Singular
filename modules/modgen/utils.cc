/*
 * $Id: utils.cc,v 1.1 1999-11-23 21:30:23 krueger Exp $
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include "modgen.h"

int init_modgen(
  moddefv module_def,
  char *filename
)
{
  char tmpfile[64];
  char *p, *q;
  
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
  
  return (create_tmpfile(module_def));
}

int create_tmpfile(
  moddefv module_def
)
{
  char tmpfile[64];

  memset(tmpfile, '\0', sizeof(tmpfile));
  snprintf(tmpfile, sizeof(tmpfile), "tmp/modgen.tmpXXXXXX");
  mktemp(tmpfile);

  if (close(creat(tmpfile, 0600)) < 0) {
    (void) unlink (tmpfile);        /*  Blow it away!!  */
    return -1;
  } else if ((module_def->fmtfp = fopen(tmpfile, "a+")) == NULL) {
    (void) unlink (tmpfile);        /*  Blow it away!!  */
    return -1;
  } else {
    (void) unlink (tmpfile); /* delete now to avoid turds... */
  }
  return 0;
  
}
