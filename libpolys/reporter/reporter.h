#ifndef OUTPUT_H
#define OUTPUT_H
/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/*
* ABSTRACT: basic output
*/
#include <stdio.h>
#include <string.h>
#include "misc/auxiliary.h"
#include "resources/feFopen.h"

EXTERN_VAR char*  feErrors;
EXTERN_VAR int    feErrorsLen;
EXTERN_VAR FILE*  feProtFile;
EXTERN_VAR int    pagelength, colmax;
EXTERN_VAR int    yy_blocklineno;
EXTERN_VAR int    yy_noeof;
extern const char feNotImplemented[];
EXTERN_VAR int     feProt;
EXTERN_VAR BOOLEAN feWarn;
EXTERN_VAR BOOLEAN feOut;
EXTERN_VAR int  traceit ;
EXTERN_VAR int  traceit_stop ;
EXTERN_VAR void (*WarnS_callback)(const char *s);

// show entering/leaving proc:
#define TRACE_SHOW_PROC   1
// show current line-no:
#define TRACE_SHOW_LINENO 2
// show current line and wait for <RET>:
#define TRACE_SHOW_LINE   4
// show basering for all levels of the proc-stack at enteringing/leaving proc,
// requires RDEBUG to be defined:
#define TRACE_SHOW_RINGS  8
// show current line and do not wait for <RET>:
#define TRACE_SHOW_LINE1  16
//
#define TRACE_BREAKPOINT  32
//
#define TRACE_TMP_BREAKPOINT  64
// show all calls to kernel routines (via iparith):
#define TRACE_CALL       128
// show all assigns (via ipassign):
#define TRACE_ASSIGN     256
// show all automatic type conversions (via ipconv):
#define TRACE_CONV       512
// profiling: print line-no to smon.out:
#define TRACE_PROFILING 1024


#define SI_PROT_I    1
#define SI_PROT_O    2
#define SI_PROT_IO   3

/* the C-part: */
#define mflush() fflush(stdout)

#ifdef __cplusplus
extern "C" {
#endif

void    Werror(const char *fmt, ...) __attribute__((format(printf,1,2)));
void    WerrorS_batch(const char *s);
void    WarnS(const char *s);
void    Print(const char* fmt, ...) __attribute__((format(printf,1,2)));
/* Print should not produce more than strlen(fmt)+510 characters! */

void    PrintNSpaces(const int n);
void    PrintLn();
void    PrintS(const char* s);

#ifdef __cplusplus
}
/* the C++-part: */

// a new output buffer will be allocated by StringSetS,
// used by several calls to StringAppend/StringAppendS
// and closed by StringEndS:
// StringEndS() returns this buffer which must be freed by omFree
// several buffer may be active at the same time
// (for example in subroutines)
void  StringAppend(const char *fmt, ...);
void  StringAppendS(const char *s);
void  StringSetS(const char* s);
char * StringEndS();
void  Warn(const char *fmt, ...);

const char *  eati(const char *s, int *i);

// Prints resources into string with StringAppend, etc
void feStringAppendResources(int warn = -1);
#endif /* c++ only */

/* everything in between calls to these procedures is printed into a string
 * which is returned by SprintEnd()
 * Shall ONLY be used for a temporary redirection of the standard output
 * (i.e. if Singular runs as a server)
 */
// unlike the StringSet/StringEndS stuff:
// only one SPrintStart/SPrintEnd buffer may be active
// the returned string must be free via omFree
void SPrintStart();
char* SPrintEnd();

/* error reporting */
#ifdef __cplusplus
extern "C"
{
#endif
extern int dReportError(const char* fmt, ...);
#define dReportBug(s) \
  dReportError("Bug reported: %s\n occurred at %s,%d\n", s, __FILE__, __LINE__)

// this is just a dummy procedure which is called after the error
// has been reported. Within the debugger, set a breakpoint on this
// proc.
extern void dErrorBreak();
#ifdef __cplusplus
}
#endif

#ifdef SING_NDEBUG
#define assume(x) do {} while (0)
#define r_assume(x) do {} while (0)
#else /* !SING_NDEBUG */

#define assume_violation(s,f,l) \
  dReportError("assume violation at %s:%d condition: %s", f,l,s)

#define assume(x)   _assume(x, __FILE__, __LINE__)
#define r_assume(x) _r_assume(x, __FILE__, __LINE__)

#define _assume(x, f, l)                        \
do                                              \
{                                               \
  if (! (x))                                    \
  {                                             \
    assume_violation(#x, f, l);                 \
  }                                             \
}                                               \
while (0)

#define _r_assume(x, f, l)                      \
do                                              \
{                                               \
  if (! (x))                                    \
  {                                             \
    assume_violation(#x, f, l);                 \
    return 0;                                   \
  }                                             \
}                                               \
while (0)
#endif /* !SING_NDEBUG */

#endif /* ifndef OUTPUT_H */
