/*
 */

typedef short BOOLEAN;

#ifndef FALSE
#define FALSE       0
#endif

#ifndef TRUE
#define TRUE        1
#endif

class procinfo;
typedef procinfo *         procinfov;

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
  long  help_chksum;
};

struct proc_object
{
  int (*function)( void );
};

union uprocinfodata;

union uprocinfodata
{
public:
  proc_singular  s;        // data of Singular-procedure
  struct proc_object    o; // pointer to binary-function
};

typedef union uprocinfodata procinfodata;

typedef enum { LANG_NONE, LANG_SINGULAR, LANG_C, LANG_MAX } language_defs;

class procinfo
{
public:
  char          *libname;
  char          *procname;
  language_defs language;
  short         ref;
  char          is_static;        // if set, proc not accessible for user
  procinfodata  data;
};


typedef void * idhdl;

