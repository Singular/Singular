#ifndef SUBEXPR_H
#define SUBEXPR_H
/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/* $Id: subexpr.h,v 1.4 1998-01-16 14:29:58 krueger Exp $ */
/*
* ABSTRACT: handling of leftv
*/

#include <string.h>
#include "structs.h"

struct _ssubexpr
{
  struct _ssubexpr * next;
  int start;
};

typedef struct _ssubexpr sSubexpr;
typedef sSubexpr * Subexpr;

extern const char sNoName[];
extern BOOLEAN siq;

class sleftv;
typedef sleftv * leftv;
class sleftv
{
  public:
  /* !! do not change the first 6 entries !! (see ipid.h: idrec) */
    leftv       next;
    char *      name;
    void *      data;
    BITSET      flag;
    attr        attribute;
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
    inline void Init() { memset(this,0,sizeof(*this)); }
    void Set(int val);
    void Print(leftv store=NULL,int spaces=0);
    void CleanUp();
    char * String(void *d=NULL);
    void Copy(leftv e);
    void * CopyD();
    attr CopyA();
    void * CopyD(int t);
    inline const char * Name()
    {
      if ((name!=NULL) && (e==NULL)) return name;
      else return sNoName;
    }
    int  Typ();
    int  LTyp(); /* returns LIST_CMD for l[i], otherwise returns Typ() */
    void * Data();
    leftv LData(); /* returns &(l[i]) for l[i], otherwise returns this */
    leftv LHdl();
    attr * Attribute();
    inline leftv Next() { return next; }
    int listLength();
    int Eval(); /*replace a COMMAND by its result otherwise by CopyD*/
};

extern sleftv sLastPrinted;

struct _sssym
{
  idhdl   h;
  Subexpr e;
};
typedef struct _sssym ssym;
typedef ssym * sym;

void syMake(leftv v,char * name);
BOOLEAN assumeStdFlag(leftv h);

class proc_singular
{
public:
  long   proc_start;       // position where proc is starting
  long   help_start;       // position where help is starting
  long   body_start;       // position where proc-body is starting
  long   body_end;         // position where proc-body is ending
  long   example_start;    // position where example is starting
  long   proc_end;         // position where proc is ending
  int    proc_lineno;
  int    body_lineno;
  int    example_lineno;
  char   *body;
};

struct proc_object
{
//public:
  BOOLEAN (*function)(leftv res, leftv v);
};

union uprocinfodata;

union uprocinfodata
{
public:
  proc_singular  s;        // data of Singular-procedure
  struct proc_object    o; // pointer to binary-function
};

typedef union uprocinfodata procinfodata;

typedef enum { LANG_NONE, LANG_SINGULAR, LANG_C } language_defs;

class procinfo
{
public:
  char          *libname;
  char          *procname;
  language_defs language;
  short         ref;
  procinfodata  data;
};

inline procinfov piCopy(procinfov pi)
{
  pi->ref++;
  return pi;
}
void piKill(procinfov l);
char *piProcinfo(procinfov pi, char *request);
void piShowProcinfo(procinfov pi, char *txt);
#endif
