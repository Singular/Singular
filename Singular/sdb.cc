/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/* $Id: sdb.cc,v 1.3 1999-04-29 16:57:18 Singular Exp $ */
/*
* ABSTRACT: Singular debugger
*/

#include "mod2.h"
#include "tok.h"
#include "mmemory.h"
#include "febase.h"
#include "ipshell.h"
#include "ipid.h"
#include "sdb.h"

int sdb_lines[]={-1,-1,-1,-1,-1,-1,-1,-1};

int sdb_checkline(char f)
{
  int i;
  char ff=f>>1;
  for(i=0;i<7;i++)
  {
    if((ff & 1) && (yylineno==sdb_lines[i]))
      return i+1;
    ff>>=1;
    if (ff==0) return 0;
  }
  return 0;
}

static char sdb_lastcmd='c';

void sdb(Voice * currentVoice, char * currLine, int len, char *b)
{
  int bp=0;
  if ((len>1)
  && ((currentVoice->pi->trace_flag & 1)
    || (bp=sdb_checkline(currentVoice->pi->trace_flag)))
  )
  {
    loop
    {
      char gdb[80];
      char *p=currLine+len-1;
      while ((*p<=' ') && (p!=currLine))
      {
        p--; len--;
      }
      if (p==currLine) return;

      currentVoice->pi->trace_flag&= ~1; // delete flag for "all lines"
      fprintf(stdout,"(%s,%d) >>",currentVoice->filename,yylineno);mflush();
      fwrite(currLine,1,len,stdout);
      fprintf(stdout,"<<\nbreakpoint %d (p..,f,b,c,n,q,e..)\n",bp);mflush();
      fgets(gdb,80,stdin);
      p=gdb;
      while (*p==' ') p++;
      if (*p >' ')
      {
        sdb_lastcmd=*p;
      }
      printf("command:%c\n",sdb_lastcmd);
      switch(sdb_lastcmd)
      {
        case 'd':
        {
          fprintf(stdout,"delete break point %d\n",bp);
          currentVoice->pi->trace_flag &= (~Sy_bit(bp));
          if (bp!=0)
          {
            sdb_lines[bp-1]=-1;
          }
          int f=currentVoice->pi->trace_flag;
          fprintf(stdout,"active breakpoints: \n");
          int i;
          for(i=1;i<=7;i++)
          {
            f=f>>1;
            if (f&1)
              fprintf(stdout,"%d:line %d", i,  sdb_lines[i-1]);
          }
          fprintf(stdout,"\n");
          break;
        }
        case 'n':
          currentVoice->pi->trace_flag|= 1;
          return;
        case 'e':
        {
          int i=strlen(b);
          while ((i>=0) && (b[i]<=' ')) i--;
          if (i<0)
          {
            fprintf(stdout,"cannot set ~ at empty line\n");
            break;
          }
          if (b[i]!=';')
          {
            fprintf(stdout,"cannot set ~ at char `%c`\n",b[i]);
            break;
          }
          b[i+1]='~';
          b[i+2]=';';
          b[i+3]='\n';
          b[i+4]='\0';
          return;
        }
//          sdb_lastcmd='c';
//          if (*(p+1)!=' ')
//          {
//            printf("?? no string to execute\n");
//          }
//          else
//          {
//            p+=2;
//            char *s=(char *)AllocL(strlen(p) + 4);
//            strcpy( s,p);
//            strcat( s,"\n;\n");
//            newBuffer(s,BT_execute);
//          }
//          return;
//        }
        case 'p':
        {
          p+=2;
          char *pp=p+1;
          while (*pp>' ') pp++;
          *pp='\0';
          printf("request `%s`",p);
          idhdl h=ggetid(p,TRUE);
          if (h==NULL) printf("NULL\n");
          else
          {
            sleftv tmp;
            memset(&tmp,0,sizeof(tmp));
            tmp.rtyp=IDHDL;
            tmp.data=h;
            printf("(type %s):",Tok2Cmdname(tmp.Typ()));
            tmp.Print();
          }
          break;
        }
        case 'b':
          VoiceBackTrack();
          break;
        case 'q':
          m2_end(999);
        case 'c':
        default:
          return;
      }
    }
  }
}

