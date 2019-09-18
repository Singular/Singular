#ifndef SUBEXPR_H
#define SUBEXPR_H
/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/*
* ABSTRACT: handling of leftv
*/

#include <string.h>

#include "polys/monomials/ring.h"

//#include "Singular/ipid.h"

#include "kernel/mod2.h"
#include "Singular/grammar.h"
#include "Singular/tok.h"
#include "Singular/attrib.h"
#include "Singular/fevoices.h" /* for sNoName_fe*/

typedef enum { LANG_NONE, LANG_TOP, LANG_SINGULAR, LANG_C, LANG_MIX, LANG_MAX} language_defs;
class proc_singular
{
public:
  long   proc_start;       // position where proc is starting
  long   proc_end;         // position where proc is ending
  long   def_end;          // position where proc header is ending
  long   help_start;       // position where help is starting
  long   help_end;         // position where help is starting
  long   body_start;       // position where proc-body is starting
  long   body_end;         // position where proc-body is ending
  long   example_start;    // position where example is starting
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

typedef procinfo *         procinfov;

struct _ssubexpr
{
  struct _ssubexpr * next;
  int start;
};

typedef struct _ssubexpr *Subexpr;

THREAD_VAR extern BOOLEAN siq;

class sleftv;
typedef sleftv * leftv;

/// Class used for (list of) interpreter objects
class sleftv
{
  public:
  /* !! do not change the first 6 entries !! (see ipid.h: idrec) */
    leftv       next;
    const char *name;
    void *      data;
    attr        attribute;
    BITSET      flag;
    int         rtyp;
                 /* the type of the expression, describes the data field
                  * (E) markes the type field in iparith
                  * (S) markes the rtyp in sleftv
                  * ANY_TYPE:   data is int: real type or 0    (E)
                  * DEF_CMD:    all types, no change in sleftv (E)
                  * IDHDL: existing variable         (E)
                  * IDHDL: variable, data is idhdl   (S)
                  * COMMAND: data is command         (S)
                  * INT_CMD:      int constant, data is int  (E,S)
                  * INTVEC_CMD:   intvec constant, data is intvec * (E,S)
                  * POLY_CMD:     poly constant, data is poly (E,S)
                  * ....
                  */
    Subexpr e;    /* holds the indices for indexed values */
    package     req_packhdl;
    inline void Init() { memset(this,0,sizeof(*this)); }
    void CleanUp(ring r=currRing);

    /// Called by type_cmd (e.g. "r;") or as default in jPRINT
    void Print(leftv store=NULL,int spaces=0);

    /// Called for conversion to string (used by string(..), write(..),..)
    char * String(void *d=NULL, BOOLEAN typed = FALSE, int dim = 1);

    void Copy(leftv e);
    attr CopyA();
    void * CopyD(int t);
    void * CopyD() { return CopyD(Typ()); }
    inline const char * Name()
    {
      if ((name!=NULL) && (e==NULL)) return name;
      else return sNoName_fe;
    }
    inline const char * Fullname()
    {
      if ((name!=NULL) && (e==NULL)) return(this->name);
      else return sNoName_fe;
    }
    int  Typ();
    int  LTyp(); /* returns LIST_CMD for l[i], otherwise returns Typ() */
    void * Data();
    leftv LData(); /* returns &(l[i]) for l[i], otherwise returns this */
    //leftv LHdl();
    attr * Attribute();
    inline leftv Next() { return next; }
    int listLength();
    int Eval(); /*replace a COMMAND by its result otherwise by CopyD*/
    BOOLEAN RingDependend();
};

inline BOOLEAN RingDependend(int t) { return (BEGIN_RING<t)&&(t<END_RING); }
THREAD_INST_VAR extern sleftv sLastPrinted;

void syMake(leftv v,const char * name, package pa = NULL);
void syMakeMonom(leftv v,const char * name);
BOOLEAN assumeStdFlag(leftv h);

inline procinfov piCopy(procinfov pi)
{
  pi->ref++;
  return pi;
}
BOOLEAN piKill(procinfov l);
const char *piProcinfo(procinfov pi, const char *request);
void piShowProcinfo(procinfov pi, char *txt);
#ifdef HAVE_LIBPARSER
class libstack;
typedef libstack *  libstackv;

class libstack
{
 public:
  libstackv next;
  char      *libname;
  BOOLEAN   to_be_done;
  int       cnt;
  void      push(const char *p, char * libname);
  libstackv pop(const char *p);
  inline char *get() { return(libname); }
};
#endif /* HAVE_LIBPARSER */

THREAD_VAR extern omBin sSubexpr_bin;
THREAD_VAR extern omBin procinfo_bin;
THREAD_VAR extern omBin libstack_bin;

void s_internalDelete(const int t,  void *d, const ring r);

#endif
