/*
 *  $Id: modgen.h,v 1.4 2000-01-21 09:23:21 krueger Exp $
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
class procflags;

typedef paramdef * paramdefv;
typedef procdef * procdefv;
typedef cfiles * cfilesv;
typedef moddef * moddefv;
typedef procflags *procflagsv;

class procflags {
  public:
  char declaration_done;
  char typecheck_done;
  
  char do_typecheck;
  char do_return;
};

class paramdef {
 public:
  char *name;
  char *typname;
  int  typ;
};

class procdef {
 public:
  char *procname;
  char *funcname;
  int lineno;
  int is_static;
  paramdef  return_val;
  paramdefv param;
  int       paramcnt;
  procflags flags;
  char *c_code;
  char *help_string;
};

class cfiles {
  public:
  cfilesv next;
  char *filename;
};

class moddef {
 public:
  FILE * modfp;           /* module file */
  FILE * modfp_h;         /* header file */
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

typedef enum { VAR_UNKNOWN, VAR_BOOL, VAR_NUM, VAR_STRING,
               VAR_FILE, VAR_FILES
} var_type;

typedef enum { CMD_NONE, CMD_DECL, CMD_CHECK, CMD_RETURN
} cmd_token;

typedef enum { VAR_NONE, VAR_MODULE, VAR_HELP, VAR_INFO, VAR_VERSION,
               VAR_TYPECHECK, VAR_RETURN, VAR_FUNCTION
} var_token;

/*
 *
 */
extern cmd_token checkcmd(char *cmdname,
                          void(**write_cmd)(moddefv module, procdefv pi));
extern var_token checkvar(char *varname, var_type type);


extern int IsCmd(char *n, int & tok);
extern char * decl2str(int n, char *name);

extern void myyyerror(char *fmt, ...);


extern void PrintProclist(moddefv module);
extern void Add2proclist(moddefv module, char *name, char *ret_val,
                           char *ret_typname, int ret_typ);
extern void generate_mod(moddefv module, int section);
extern void mod_create_makefile(moddefv module);
extern void Add2files(moddefv module, char *buff);

extern void generate_function(procdefv pi, FILE *fp);
extern void  mod_copy_tmp(FILE *fp_out, FILE *fp_in);
extern void mod_write_header(FILE *fp, char *module, char what);
extern void generate_header(procdefv pi, FILE *fp);
extern void write_header(FILE *fp, char *module, char *comment="");
extern void make_version(char *p, moddefv module);
extern void write_procedure_text(moddefv module, int lineno);
/*extern void write_procedure_header(moddefv module);*/

extern int init_proc(procdefv p, char *procname, paramdefv ret, int line);
extern void setup_proc(moddefv module, procdefv p);
extern void proc_set_var(procdefv p, var_type type, var_token varid,
                         char *varname, void *varvalue);
void proc_set_default_var(var_type type, var_token varid, char *varname,
                          void *varvalue);
void write_finish_functions(moddefv module, procdefv proc);
void AddParam(procdefv p, paramdefv vnew);

extern int create_tmpfile(moddefv module_def);

extern void write_procedure_typecheck(moddefv module, procdefv pi, FILE *fmtfp);
extern void write_procedure_return(moddefv module, procdefv pi, FILE *fmtfp);
extern void write_function_declaration(moddefv module, procdefv pi);
extern void write_function_typecheck(moddefv module, procdefv pi);
extern void write_function_return(moddefv module, procdefv pi);

#endif /* _MODGEN_H */
