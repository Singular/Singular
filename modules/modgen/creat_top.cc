/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/* $Id: creat_top.cc,v 1.5 2000-02-14 21:44:39 krueger Exp $ */
/*
* ABSTRACT: lib parsing
*/

#include <mod2.h>

#include "modgen.h"

void enter_id(FILE *fp, char *name, char *value);
/*========================================================================*/
void  mod_copy_tmp(
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
  fprintf(fp, "idhdl enter_id(char *name, char *value, idtyp t)\n");
  fprintf(fp, "{\n");
  fprintf(fp, "  idhdl h;\n");
  fprintf(fp, "\n");
  fprintf(fp, "  h=enterid(mstrdup(name),0, t, &IDROOT, FALSE);\n");
  fprintf(fp, "  if(h!=NULL) {\n");
  fprintf(fp, "     switch(t) {\n");
  fprintf(fp, "       case STRING_CMD: IDSTRING(h) = mstrdup(value);break\n");
  fprintf(fp, "       case PACKAGE_CMD: break;\n");
  fprintf(fp, "       case PROC_CMD: break;\n");
  fprintf(fp, "     }\n");
  fprintf(fp, "  }\n");
  fprintf(fp, "  return(h);\n");
  fprintf(fp, "}\n");
}

/*========================================================================*/
void write_add_singular_proc(FILE *fp)
{
  fprintf(fp, "idhdl add_singular_proc(char *procname, int line,\n");
  fprintf(fp, "                       long pos, long end, BOOLEAN pstatic)\n");
  fprintf(fp, "{\n");
  fprintf(fp, "  idhdl h;\n");
  fprintf(fp, "  procinfov pi;\n\n");
  fprintf(fp, "  h = enter_id(name, NULL, PROC_CMD);\n");
  fprintf(fp, "  if(h == NULL) return NULL;\n");
  fprintf(fp, "\n");
//  fprintf(fp, "  pi->libname = mstrdup(libname);\n");
  fprintf(fp, "  pi->procname = mstrdup(procname);\n");
  fprintf(fp, "  pi->language = LANG_SINGULAR;\n");
  fprintf(fp, "  pi->ref = 1;\n");
  fprintf(fp, "  pi->is_static = pstatic;\n");
  fprintf(fp, "  pi->data.s.proc_start = pos;\n");
  fprintf(fp, "  pi->data.s.def_end    = pos;\n");
  fprintf(fp, "  pi->data.s.help_start = 0L;\n");
  fprintf(fp, "  pi->data.s.help_end   = 0L;\n");
  fprintf(fp, "  pi->data.s.body_start = pos;\n");
  fprintf(fp, "  pi->data.s.body_end   = end;\n");
  fprintf(fp, "  pi->data.s.proc_end   = end;\n");
  fprintf(fp, "  pi->data.s.example_start = 0L;\n");
  fprintf(fp, "  pi->data.s.proc_lineno = line;\n");
  fprintf(fp, "  pi->data.s.body_lineno = line;\n");
  fprintf(fp, "  pi->data.s.example_lineno = 0;\n");
  fprintf(fp, "  pi->data.s.body = NULL;\n");
  fprintf(fp, "  pi->data.s.help_chksum = 0;\n");
  fprintf(fp, "  \n");
  fprintf(fp, "  return(h);\n");
  fprintf(fp, "}\n");
}

/*========================================================================*/
void write_mod_init(
  FILE *fp
  )
{
  fprintf(fp, "\n\n");
  fprintf(fp, "extern \"C\"\n");
  fprintf(fp, "int mod_init(int(*iiAddCproc)())\n{\n");
  fprintf(fp, "  idhdl h;\n");
  fprintf(fp, "  idhdl helphdl = enter_id(\"help\", NULL, PACKAGE_CMD);\n");
  fprintf(fp, "  idhdl examplehdl = enter_id(\"example\", NULL, PACKAGE_CMD);\n\n");
}

/*========================================================================*/
int write_intro(
    moddefv module
    )
{
  char *filename;

  filename = (char *)malloc(strlen(module->name)+5+4);
  sprintf(filename, "tmp/%s.cc", module->name);
  
  fflush(module->fmtfp);

  if( (module->modfp = fopen(filename, "w")) == NULL) {
    free(filename);
    return -1;
  }
  printf("Creating %s, ", filename);fflush(stdout);
  mod_write_header(module->modfp, module->name, 'c');
  mod_copy_tmp(module->modfp, module->fmtfp);
  printf("  done.\n");fflush(stdout);
  fclose(module->fmtfp);
  if(create_tmpfile(module)) return -1;

  sprintf(filename, "tmp/%s.h", module->name);
  if( (module->modfp_h = fopen(filename, "w")) == NULL) {
    free(filename);
    return -1;
  }
  printf("Creating %s, ", filename);fflush(stdout);
  mod_write_header(module->modfp_h, module->name, 'h');

  free(filename);
//  write_enter_id(module->modfp);
}
