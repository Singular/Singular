/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/* $Id: creat_top.cc,v 1.1 1999-11-23 21:30:20 krueger Exp $ */
/*
* ABSTRACT: lib parsing
*/

#include <mod2.h>

#include "modgen.h"

void enter_id(FILE *fp, char *name, char *value);
/*========================================================================*/
static void  mod_copy_tmp(
  FILE *fp_out,
  FILE *fp_in
  )
{
  int nread;
  char buf[1024];
  
  fseek(fp_in, 0L, 0);
  do {
    nread = fread (buf, sizeof (char), sizeof(buf)-1, fp_in);
    fwrite(buf, sizeof(char), nread, fp_out);
  } while(!feof(fp_in));
  fputs("\n", fp_out);
}

/*========================================================================*/
void write_enter_id(FILE *fp)
{
  fprintf(fp, "void enter_id(char *name, char *value)\n");
  fprintf(fp, "{\n  idhdl h;\n\n");
  fprintf(fp, "  h = enterid(mstrdup(name),0, STRING_CMD, &IDROOT, FALSE);\n");
  fprintf(fp, "  if(h!=NULL) {\n");
  fprintf(fp, "     IDSTRING(h) = mstrdup(value);\n");
  fprintf(fp, "  }\n}\n\n");
}

/*========================================================================*/
void write_mod_init(
  FILE *fp
  )
{
  fprintf(fp, "\n\n");
  fprintf(fp, "extern \"C\"\n");
  fprintf(fp, "int mod_init(int(*iiAddCproc)())\n{\n");
  fprintf(fp, "  idhdl h;\n\n");
}

/*========================================================================*/
void write_intro(
    moddefv module
    )
{
  char *filename;

  filename = (char *)malloc(strlen(module->name)+5+4);
  sprintf(filename, "tmp/%s.cc", module->name);
  
  fflush(module->fmtfp);

  module->modfp = fopen(filename, "w");
  printf("Creating %s, ", filename);fflush(stdout);
  mod_write_header(module->modfp, module->name);
  mod_copy_tmp(module->modfp, module->fmtfp);
  printf("  done.\n");fflush(stdout);
  fclose(module->fmtfp);
  create_tmpfile(module);

//  write_enter_id(module->modfp);
}
