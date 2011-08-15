/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/* $Id$ */
/*
* ABSTRACT: i/o system
*/

/* I need myfread in standalone_parser */
#ifndef STANDALONE_PARSER
#include <kernel/mod2.h>

#include <stdlib.h>
#include <stdio.h>
#include <omalloc/mylimits.h>
#include <stdarg.h>
#include <sys/stat.h>
#include <ctype.h>
#include <unistd.h>

#ifdef HAVE_PWD_H
#include <pwd.h>
#endif

#include <kernel/febase.h>
#include <omalloc/omalloc.h>
#include <reporter/reporter.h>
#include <resources/feFopen.h>
#include <misc/options.h>

#define fePutChar(c) fputc((unsigned char)(c),stdout)
/*0 implementation */

// char fe_promptstr[] ="  ";

// output/print buffer:
// line buffer for reading:
// minimal value for MAX_FILE_BUFFER: 4*4096 - see Tst/Long/gcd0_l.tst
// this is an upper limit for the size of monomials/numbers read via the interpreter
#define MAX_FILE_BUFFER 4*4096

int     si_echo = 0;
int     printlevel = 0;
int     colmax = 80;
char    prompt_char = '>'; /*1 either '>' or '.'*/
int     yylineno  = 0;
int  myynest = -1; 
int  traceit = 0;
char       my_yylinebuf[80];


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


#else /* ! STANDALONE_PARSER */
#include <stdio.h>

#endif

