#ifndef OUTPUT_H
#define OUTPUT_H
/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/* $Id$ */
/*
* ABSTRACT: basic output
*/
#include <stdio.h>
#include <string.h>
#include <misc/auxiliary.h>

extern char*  feErrors;
extern int    feErrorsLen;
extern FILE*  feProtFile;
extern int    printlevel;
extern int    pagelength, colmax;
extern int    yy_blocklineno;
extern int    yy_noeof;
extern const char feNotImplemented[];
extern BOOLEAN errorreported;
extern int     feProt;
extern BOOLEAN feWarn;
extern BOOLEAN feOut;
extern int  traceit ;
#define TRACE_SHOW_PROC   1
#define TRACE_SHOW_LINENO 2
#define TRACE_SHOW_LINE   4
#define TRACE_SHOW_RINGS  8
#define TRACE_SHOW_LINE1  16
#define TRACE_BREAKPOINT  32
#define TRACE_TMP_BREAKPOINT  64

#define PROT_NONE 0
#define PROT_I    1
#define PROT_O    2
#define PROT_IO   3

/* the C-part: */
#define mflush() fflush(stdout)

#ifdef __cplusplus
extern "C" {
#endif

void    Werror(const char *fmt, ...) __attribute__((format(printf,1,2)));
void    WerrorS(const char *s);
void    WarnS(const char *s);
void    Print(const char* fmt, ...) __attribute__((format(printf,1,2)));
/* Print should not produce more than strlen(fmt)+510 characters! */

void    PrintNSpaces(const int n);
void    PrintLn();
void    PrintS(const char* s);

#ifdef __cplusplus
}
/* the C++-part: */

char *  StringAppend(const char *fmt, ...);
char *  StringAppendS(const char *s);
char *  StringSetS(const char* s);
void    Warn(const char *fmt, ...);

const char *  eati(const char *s, int *i);

#endif /* c++ only */

/* everything in between calls to these procedures is printed into a string
 * which is returned by SprintEnd()
 * Shall ONLY be used for a temporary redirection of the standard output
 * (i.e. if Singular runs as a server)
 */
void SPrintStart();
char* SPrintEnd();

/* error reporting */
#ifdef __cplusplus
extern "C"
{
#endif
extern int dReportError(const char* fmt, ...);
#define dReportBug(s) \
  dReportError("Bug reported: %s\n occured at %s,%d\n", s, __FILE__, __LINE__)

// this is just a dummy procedure which is called after the error
// has been reported. Within the debugger, set a breakpoint on this
// proc.
extern void dErrorBreak();
#ifdef __cplusplus
}
#endif

#ifndef HAVE_ASSUME
#define assume(x) ((void) 0)
#define r_assume(x) ((void) 0)
#else /* ! HAVE_ASSUME */

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
#endif /* HAVE_ASSUME */

#endif /* ifndef OUTPUT_H */
