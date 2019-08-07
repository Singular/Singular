/*
 *
 */

#ifndef _MODGEN_H
#define _MODGEN_H
#define MOD_GEN_VERSION "0.3"

#define BUFLEN 128
#define TMPL_HEAD ""
#define TMPL_FOOT ""

#include <stdio.h>
#include <string.h>

/* Basic definitions */
#ifndef BOOLEAN
  typedef int BOOLEAN;
#endif

#ifndef TRUE
#  define TRUE 1
#endif

#ifndef FALSE
#  define FALSE 0
#endif

/*
 * LANG_TOP     : Toplevel package only
 * LANG_SINGULAR:
 * LANG_C       :
 */
typedef enum { LANG_NONE, LANG_TOP, LANG_SINGULAR, LANG_C, LANG_MAX}
  language_defs;

/* id type */
typedef int idtyp;



class paramdef;
class procdef;
class moddef;
class cfiles;
class procflags;
class stringdef;

typedef paramdef * paramdefv;
typedef procdef * procdefv;
typedef cfiles * cfilesv;
typedef moddef * moddefv;
typedef procflags *procflagsv;
typedef stringdef *stringdefv;

class procflags {
  public:
  char start_of_code;
  char declaration_done;
  char typecheck_done;

  char auto_header;
  char do_declaration;
  char do_typecheck;
  char do_return;
  char result_done;
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
  int          lineno_other;
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
  FILE * fmtfp2;          /* temporary file */
  FILE * fmtfp3;          /* temporary file */
  FILE * binfp;           /* include singular procedures are stored
                             in an extra file */
  FILE * docfp;		  /* help/example is stored in this file
                             for building the reference manual */
  char * filename;        /* inputfile to parse */
  char * name;            /* name of the module directory*/
  char * targetname;      /* name of dynamic module + package */
  unsigned int major, minor, level;
  char * version;
  char * category;
  char * revision;
  char * info;
  char * c_code;
  procdefv procs;
  int      proccnt;
  cfilesv  files;
  int      filecnt;
};

class stringdef
{
  public:
  char *string;
  int lineno;
};

typedef enum { VAR_UNKNOWN, VAR_BOOL, VAR_NUM, VAR_STRING,
               VAR_FILE, VAR_FILES
} var_type;

typedef enum { CMD_NONE, CMD_BADSYNTAX, CMD_DECL, CMD_CHECK, CMD_RETURN,
               CMD_SINGULAR, CMD_ERROR, CMD_NODECL
} cmd_token;

typedef enum { CMDT_SINGLE, CMDT_0, CMDT_ANY, CMDT_EQ
} cmd_type;


typedef enum { VAR_NONE, VAR_MODULE, VAR_HELP, VAR_INFO, VAR_VERSION,
               VAR_TYPECHECK, VAR_RETURN, VAR_FUNCTION, VAR_CATEGORY
} var_token;

/*
 *
 */
THREAD_VAR extern int modlineno;    /* lineno within module */
THREAD_VAR extern int debug;
THREAD_VAR extern int trace;
THREAD_VAR extern int xyz;

extern int IsCmd(char *n, int & tok);
extern char * decl2str(int n, char *name);

extern int myyyerror(char *fmt, ...);


extern void PrintProclist(moddefv module);
extern void generate_mod(moddefv module, int section);
extern void mod_create_makefile(moddefv module);
extern void Add2files(moddefv module, char *buff);

extern void mod_copy_tmp(FILE *fp_out, FILE *fp_in);
extern void mod_write_header(FILE *fp, char *module, char what);
extern void generate_header(procdefv pi, FILE *fp);
extern void write_header(FILE *fp, char *module, char *comment="");
extern void make_version(char *p, moddefv module);
extern void make_module_name(char *p, moddefv module);
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

/* from makefile.cc */
extern void mod_create_makefile(moddefv module);
extern void build_head_section(FILE *fp, moddefv module);
extern void build_clean_section(FILE *fp, moddefv module);
extern void build_install_section(FILE *fp, moddefv module);
static char *object_name(char *p);
extern void build_compile_section(FILE *fp, moddefv module);

/* from utils.cc */
extern int create_tmpfile(moddefv module_def, int which = 0);
extern char *build_filename(moddefv module, char *text, int what);
extern unsigned long crccheck(char *file);
extern void write_crccheck(FILE *fp);
extern void write_crctable(FILE *fp);

/* from proc_setup.cc */
extern int check_reseverd(char *name);

/* from proc.cc */
extern void write_function_header(moddefv module, procdefv proc);
extern void write_procedure_typecheck(moddefv module, procdefv pi, FILE *fmtfp);
extern void write_procedure_return(moddefv module, procdefv pi, FILE *fmtfp);
extern void write_function_declaration(moddefv module, procdefv pi, void *arg = NULL);
extern void write_function_nodecl(moddefv module, procdefv pi, void *arg = NULL);
extern void write_function_error(moddefv module, procdefv pi, void *arg = NULL);
extern void write_function_typecheck(moddefv module, procdefv pi, void *arg = NULL);
extern void write_function_result(moddefv module, procdefv pi, void *arg = NULL);
extern void write_function_return(moddefv module, procdefv pi, void *arg = NULL);
extern void write_function_singularcmd(moddefv module, procdefv pi, void *arg = NULL);
extern void write_function_errorhandling(moddefv module, procdefv pi);
extern void write_help(moddefv module, procdefv pi);
extern void write_example(moddefv module, procdefv pi);
extern int  write_singular_procedures(moddefv module, procdefv proc);
extern void write_singular_parameter(moddefv module,int lineno,
                                     char *typname, char *varname);
extern void write_singular_end(moddefv module, procdefv proc,int lineno);
extern int write_helpfile_help(moddefv module, procdefv proc);

extern void write_codeline(moddefv module, procdefv proc,
                           char *line, int lineno = -1);

/* from misc.cc */
extern cmd_token checkcmd(
  char *cmdname,
  void(**write_cmd)(moddefv module, procdefv pi, void *arg),
  cmd_type  type,
  int args);
extern var_token checkvar(
  char *varname, var_type type,
  void (**write_cmd)(moddefv module, var_token type, idtyp t,
                     void *arg1, void *arg2));
void write_main_variable(moddefv module, var_token type,
                         idtyp t, void *arg1, void *arg2);

#endif /* _MODGEN_H */
