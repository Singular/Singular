/*
 *  $Id: modgen.h,v 1.6 2000-02-14 21:43:32 krueger Exp $
 *
 */

#ifndef _MODGEN_H
#define _MODGEN_H
#define MOD_GEN_VERSION "0.2"

#define BUFLEN 128
#define TMPL_HEAD ""
#define TMPL_FOOT ""

#include <stdio.h>
#include <string.h>
#include <mod2.h>
#include <subexpr.h>

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
  char *varname;
};

class procdef {
 public:
  char       * procname;
  char       * funcname;
  language_defs language;
  int          lineno;
  int          is_static;
  paramdef     return_val;
  paramdefv    param;
  int          paramcnt;
  procflags    flags;
  char       * c_code;
  char       * help_string;
  long         example_len;
  char       * example_string;
  long         sing_start;
  long         sing_end;
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
  FILE * fmtfp;           /* temporary file */
  FILE * binfp;           /* include singular procedures are stored
                             in an extra file */
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

typedef enum { CMD_NONE, CMD_DECL, CMD_CHECK, CMD_RETURN,
               CMD_SINGULAR
} cmd_token;

typedef enum { VAR_NONE, VAR_MODULE, VAR_HELP, VAR_INFO, VAR_VERSION,
               VAR_TYPECHECK, VAR_RETURN, VAR_FUNCTION
} var_token;

/*
 *
 */


extern int IsCmd(char *n, int & tok);
extern char * decl2str(int n, char *name);

extern void myyyerror(char *fmt, ...);


extern void PrintProclist(moddefv module);
extern void generate_mod(moddefv module, int section);
extern void mod_create_makefile(moddefv module);
extern void Add2files(moddefv module, char *buff);

extern void mod_copy_tmp(FILE *fp_out, FILE *fp_in);
extern void mod_write_header(FILE *fp, char *module, char what);
extern void generate_header(procdefv pi, FILE *fp);
extern void write_header(FILE *fp, char *module, char *comment="");
extern void make_version(char *p, moddefv module);
extern void write_procedure_text(moddefv module, int lineno);
/*extern void write_procedure_header(moddefv module);*/

extern int init_proc(procdefv p, char *procname, paramdefv ret, int line,
                     language_defs language = LANG_C);
extern void setup_proc(moddefv module, procdefv p);
extern void proc_set_var(procdefv p, var_type type, var_token varid,
                         char *varname, void *varvalue);
void proc_set_default_var(var_type type, var_token varid, char *varname,
                          void *varvalue);
void write_finish_functions(moddefv module, procdefv proc);
void AddParam(procdefv p, paramdefv vnew, char *name = NULL);

extern int create_tmpfile(moddefv module_def);

/* from proc_setup.cc */
extern int check_reseverd(char *name);

/* from proc.cc */
extern void write_example(moddefv module, procdefv proc);

extern void write_procedure_typecheck(moddefv module, procdefv pi, FILE *fmtfp);
extern void write_procedure_return(moddefv module, procdefv pi, FILE *fmtfp);
extern void write_function_declaration(moddefv module, procdefv pi, void *arg = NULL);
extern void write_function_typecheck(moddefv module, procdefv pi, void *arg = NULL);
extern void write_function_return(moddefv module, procdefv pi, void *arg = NULL);
extern void write_function_errorhandling(moddefv module, procdefv pi);
extern int  write_singular_procedures(moddefv module, procdefv proc);
extern void write_singular_parameter(moddefv module,int lineno,
                                     char *typname, char *varname);
extern void write_codeline(moddefv module, procdefv proc,
                           char *line, int lineno = -1);

/* from misc.cc */
extern cmd_token checkcmd(
  char *cmdname,
  void(**write_cmd)(moddefv module, procdefv pi, void *arg),
  int args);
extern var_token checkvar(
  char *varname, var_type type,
  void (**write_cmd)(moddefv module, var_token type, idtyp t,
                     void *arg1, void *arg2));
void write_main_variable(moddefv module, var_token type,
                         idtyp t, void *arg1, void *arg2);

#endif /* _MODGEN_H */
