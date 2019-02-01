/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/*
* ABSTRACT: i/o system
*/
#include "kernel/mod2.h"

/* I need myfread in standalone_parser */
#ifndef STANDALONE_PARSER

#include "omalloc/omalloc.h"
#include "reporter/reporter.h"
#include "misc/options.h"

//#include "Singular/febase.h"

#include "misc/mylimits.h"

#ifdef HAVE_PWD_H
#include <pwd.h>
#endif

#define fePutChar(c) fputc((unsigned char)(c),stdout)
/*0 implementation */

// char fe_promptstr[] ="  ";

// output/print buffer:
// line buffer for reading:
// minimal value for MAX_FILE_BUFFER: 4*4096 - see Tst/Long/gcd0_l.tst
// this is an upper limit for the size of monomials/numbers read via the interpreter
#define MAX_FILE_BUFFER 4*4096

VAR int     si_echo = 0;
VAR int     printlevel = 0;
VAR int     colmax = 80;
VAR int     pagelength = 24;
VAR char    prompt_char = '>'; /*1 either '>' or '.'*/
VAR int     yylineno  = 0;
VAR int  myynest = -1;
VAR int  traceit = 0;
VAR char       my_yylinebuf[80];


#if 0
void monitor(char* s, int mode)
{
  if (feProt)
  {
    fclose(feProtFile);
    feProt = 0;
  }
  if ((s!=NULL) && (*s!='\0'))
  {
    feProtFile = myfopen(s,"w");
    if (feProtFile==NULL)
    {
      Werror("cannot open %s",s);
      feProt=0;
    }
    else
      feProt = mode;
  }
}
#else
void monitor(void *F, int mode)
{
  if (feProt)
  {
    fclose(feProtFile);
    feProt = 0;
  }
  if (F!=NULL)
  {
    feProtFile = (FILE *)F;
    feProt = mode;
  }
}
#endif

#endif

