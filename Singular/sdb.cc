/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/* $Id: sdb.cc,v 1.1 1999-04-15 17:28:47 Singular Exp $ */
/*
* ABSTRACT: Singular debugger
*/

#include "mod2.h"
#include "tok.h"
#include "febase.h"
#include "ipshell.h"
#include "sdb.h"

void sdb(Voice * currentVoice, char * currLine, int len)
{
  loop
  {
    char gdb[80];
    char *p=currLine+len-1;
    while ((*p<=' ') && (p!=currLine))
    {
      p--; len--;
    }
    if (p!=currLine)
    {
      fprintf(stdout,"(%s,%d) >>",currentVoice->filename,yylineno);mflush();
      fwrite(currLine,1,len,stdout);
      fprintf(stdout,"<< (p, f-finish,b,...)\n");mflush();
      fgets(gdb,80,stdin);
      p=gdb;
      while (*p==' ') p++;
      if (*p >' ')
        printf("command:%c\n",*p);
      switch(*p)
      {
        case 'f':
          currentVoice->pi->trace_flag&= ~TRACE_BREAKPOINT;
          return;
        case 'p':
          printf(".....\n");
          break;
        case 'b':
          VoiceBackTrack();
	  break;
        case 'q':
          m2_end(999);
        default:
          return;
      }
    }
  }
}

