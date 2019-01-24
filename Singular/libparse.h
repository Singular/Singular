#ifndef LIBPARSE_H
#define LIBPARSE_H
/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/*
* ABSTRACT: lib parsing
*/
typedef enum { OLD_LIBSTYLE, NEW_LIBSTYLE } lib_style_types;
typedef enum { LOAD_LIB, GET_INFO } lp_modes;

#ifdef STANDALONE_PARSER


#define idhdl void*
#define leftv void*
#define package void*
#define BOOLEAN int

typedef enum { LANG_NONE, LANG_TOP, LANG_SINGULAR, LANG_C, LANG_MIX, LANG_MAX} language_defs;
// LANG_TOP     : Toplevel package only
// LANG_SINGULAR:
// LANG_C       :
//

class proc_singular
{
public:
  long   proc_start;       // position where proc is starting
  long   def_end;          // position where proc header is ending
  long   help_start;       // position where help is starting
  long   help_end;         // position where help is starting
  long   body_start;       // position where proc-body is starting
  long   body_end;         // position where proc-body is ending
  long   example_start;    // position where example is starting
  long   proc_end;         // position where proc is ending
  int    proc_lineno;
  int    body_lineno;
  int    example_lineno;
  char   *body;
  long help_chksum;
};

struct proc_object
{
//public:
  BOOLEAN (*function)(leftv res, leftv v);
};
union uprocinfodata
{
public:
  proc_singular  s;        // data of Singular-procedure
  struct proc_object    o; // pointer to binary-function
};

typedef union uprocinfodata procinfodata;

class procinfo;
typedef procinfo *         procinfov;

class procinfo
{
public:
  char          *libname;
  char          *procname;
  package       pack;
  language_defs language;
  short         ref;
  char          is_static;        // if set, proc not accessible for user
  char          trace_flag;
  procinfodata  data;
};
#endif

procinfo *iiInitSingularProcinfo(procinfo* pi, const char *libname,
              const char *procname, int line, long pos, BOOLEAN pstatic=FALSE);

int yylplex(const char *libname, const char *libfile, lib_style_types *lib_style,
           idhdl pl, BOOLEAN autoexport=FALSE, lp_modes=LOAD_LIB);

void reinit_yylp();

EXTERN_VAR char * text_buffer;

#  define YYLP_ERR_NONE    0
#  define YYLP_DEF_BR2     1
#  define YYLP_BODY_BR2    2
#  define YYLP_BODY_BR3    3
#  define YYLP_BODY_TMBR2  4
#  define YYLP_BODY_TMBR3  5
#  define YYLP_EX_BR2      6
#  define YYLP_EX_BR3      7
#  define YYLP_BAD_CHAR    8
#  define YYLP_MISSQUOT    9
#  define YYLP_MISS_BR1   10
#  define YYLP_MISS_BR2   11
#  define YYLP_MISS_BR3   12

#  ifdef STANDALONE_PARSER
#define myfopen fopen
#define myfread fread
#  endif

#endif /* LIBPARSE_H */


