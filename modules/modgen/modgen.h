/*
 *  $Id: modgen.h,v 1.1 1999-11-23 21:30:22 krueger Exp $
 *
 */

#ifndef _MODGEN_H
#define _MODGEN_H
#define MOD_GEN_VERSION "0.2"

#define BUFLEN 128
#define TMPL_HEAD ""
#define TMPL_FOOT ""

#include <stdio.h>

class paramdef;
class procdef;
class moddef;
class cfiles;

typedef paramdef * paramdefv;
typedef procdef * procdefv;
typedef cfiles * cfilesv;
typedef moddef * moddefv;

class paramdef {
 public:
  paramdefv next;
  char *name;
  char *typname;
  int  typ;
};

class procdef {
 public:
  procdefv next;
  char *procname;
  char *funcname;
  int is_static;
  paramdef  return_val;
  paramdefv param;
  int       paramcnt;
  char *c_code;
};

class cfiles {
  public:
  cfilesv next;
  char *filename;
};

class moddef {
 public:
  FILE * modfp;
  FILE * fmtfp;
  char * filename;        /* inputfile to parse */
  char * name;            /* name of the module */
  unsigned int major, minor, level;
  char * version;
  char * revision;
  char * info;
  char * helpfile;
  char * c_code;
  procdefv procs;
  int      proccnt;
  cfilesv  files;
  int      filecnt;
};

/*
 *
 */
extern int IsCmd(char *n, int & tok);
extern char * decl2str(int n, char *name);

extern void PrintProclist(moddefv module);
extern void Add2proclist(moddefv module, char *name, char *ret_val,
                           char *ret_typname, int ret_typ);
extern void generate_mod(moddefv module, int section);
extern void AddParam(moddefv module, char *name, char *typname, int typ);
extern void mod_create_makefile(moddefv module);
extern void Add2files(moddefv module, char *buff);

extern void generate_function(procdefv pi, FILE *fp);
extern void mod_write_header(FILE *fp, char *module);
extern void generate_header(procdefv pi, FILE *fp);
extern void write_header(FILE *fp, char *module, char *comment="");
extern void make_version(char *p, moddefv module);
extern void write_procedure_text(moddefv module, int lineno);
extern void write_procedure_header(moddefv module);


extern int create_tmpfile(moddefv module_def);
#endif /* _MODGEN_H */
