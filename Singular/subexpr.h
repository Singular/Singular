#ifndef SUBEXPR_H
#define SUBEXPR_H
/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/* $Id: subexpr.h,v 1.34 2003-12-09 18:15:26 Singular Exp $ */
/*
* ABSTRACT: handling of leftv
*/

#include <string.h>
#include "grammar.h"
#include "tok.h"
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
extern char *iiSleftv2name(leftv v);

class sleftv;
typedef sleftv * leftv;
class sleftv
{
  public:
  /* !! do not change the first 6 entries !! (see ipid.h: idrec) */
    leftv       next;
    char *      name;
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
#ifdef HAVE_NS
    package     packhdl;
    package     req_packhdl;
#endif /* HAVE_NS */
    inline void Init() { memset(this,0,sizeof(*this)); }
    void Set(int val);
    void Print(leftv store=NULL,int spaces=0);
    void CleanUp(ring r=currRing);
    char * String(void *d=NULL, BOOLEAN typed = FALSE, int dim = 1);
    void Copy(leftv e);
    attr CopyA();
    void * CopyD(int t);
    void * CopyD() { return CopyD(Typ()); }
    inline const char * Name()
    {
      if ((name!=NULL) && (e==NULL)) return name;
      else return sNoName;
    }
    inline const char * Fullname()
    {
      if ((name!=NULL) && (e==NULL)) return(iiSleftv2name(this));
      else return sNoName;
    }
    int  Typ();
    int  LTyp(); /* returns LIST_CMD for l[i], otherwise returns Typ() */
    void * Data();
    void SetData(void* value);
    leftv LData(); /* returns &(l[i]) for l[i], otherwise returns this */
    leftv LHdl();
    attr * Attribute();
    inline leftv Next() { return next; }
    int listLength();
    int Eval(); /*replace a COMMAND by its result otherwise by CopyD*/
    BOOLEAN RingDependend();
};

inline BOOLEAN RingDependend(int t) { return (BEGIN_RING<t)&&(t<END_RING); }
extern sleftv sLastPrinted;

struct _sssym
{
  idhdl   h;
  Subexpr e;
};
typedef struct _sssym ssym;
typedef ssym * sym;

void syMake(leftv v,char * name, idhdl packhdl = NULL);
BOOLEAN assumeStdFlag(leftv h);

inline procinfov piCopy(procinfov pi)
{
  pi->ref++;
  return pi;
}
BOOLEAN piKill(procinfov l);
char *piProcinfo(procinfov pi, char *request);
void piShowProcinfo(procinfov pi, char *txt);
#ifdef HAVE_LIBPARSER
class libstack
{
 public:
  libstackv next;
  char      *libname;
  BOOLEAN   to_be_done;
  int       cnt;
  void      push(char *p, char * libname);
  libstackv pop(char *p);
  inline char *get() { return(libname); }
};
#endif /* HAVE_LIBPARSER */

typedef enum { LT_NONE, LT_NOTFOUND, LT_SINGULAR, LT_ELF, LT_HPUX} lib_types;
lib_types type_of_LIB(char *newlib, char *fullname);

#ifndef OM_ALLOC_H
struct omBin_s;
#endif

extern omBin_s* sSubexpr_bin;
extern omBin_s* procinfo_bin;
extern omBin_s* libstack_bin;

#endif
