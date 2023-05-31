/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/*
* ABSTRACT: Singular debugger
*/

#include "kernel/mod2.h"
#include "omalloc/omalloc.h"
#include "misc/options.h"
#include "reporter/si_signals.h"
#include "Singular/tok.h"
#include "Singular/ipshell.h"
#include "Singular/ipid.h"
#include "Singular/fevoices.h"
#include "kernel/oswrapper/feread.h"
#include "Singular/sdb.h"

#include <unistd.h>   // for unlink,fork,execlp,getpid
#include <sys/wait.h> // for wait


#ifdef HAVE_SDB
// We use 8 breakpoints - corresponding to a bit in a char variable in procinfo
// bit 1..7 force a breakpoint, if lineno==sdb_lines[i-1],
//                         (for displaying only: file sdb_files[i-1])
// bit 0 force a breakpoint in every line (used for 'n')

VAR int sdb_lines[]={-1,-1,-1,-1,-1,-1,-1,-1};
VAR char * sdb_files[6];
VAR int sdb_flags=0;

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

static char *sdb_find_arg(char *p)
{
  p++;
  while (*p==' ') p++;
  char *pp=p;
  while (*pp>' ') pp++;
  *pp='\0';
  return p;
}

void sdb_show_bp()
{
  for(int i=0; i<7;i++)
    if (sdb_lines[i]!= -1)
      Print("Breakpoint %d: %s::%d\n",i+1,sdb_files[i],sdb_lines[i]);
}

BOOLEAN sdb_set_breakpoint(const char *pp, int given_lineno)
{
  idhdl h=ggetid(pp);
  if ((h==NULL)||(IDTYP(h)!=PROC_CMD))
  {
    PrintS(" not found\n");
    return TRUE;
  }
  else
  {
    procinfov p=(procinfov)IDDATA(h);
    #ifdef HAVE_DYNAMIC_LOADING
    if (p->language!=LANG_SINGULAR)
    {
      PrintS("is not a Singular procedure\n");
      return TRUE;
    }
    #endif
    int lineno;
    if (given_lineno >0) lineno=given_lineno;
    else                 lineno=p->data.s.body_lineno;
    int i;
    if (given_lineno== -1)
    {
      i=p->trace_flag;
      p->trace_flag &=1;
      Print("breakpoints in %s deleted(%#x)\n",p->procname,i &255);
      return FALSE;
    }
    i=0;
    while((i<7) && (sdb_lines[i]!=-1)) i++;
    if (sdb_lines[i]!= -1)
    {
      PrintS("too many breakpoints set, max is 7\n");
      return TRUE;
    }
    sdb_lines[i]=lineno;
    sdb_files[i]=p->libname;
    i++;
    p->trace_flag|=(1<<i);
    Print("breakpoint %d, at line %d in %s\n",i,lineno,p->procname);
    return FALSE;
  }
}

void sdb_edit(procinfo *pi)
{
  char * filename = omStrDup("/tmp/sdXXXXXX");
  int f=mkstemp(filename);
  if (f==-1)
  {
    Print("cannot open %s\n",filename);
    omFree(filename);
    return;
  }
  if (pi->language!= LANG_SINGULAR)
  {
    Print("cannot edit type %d\n",pi->language);
    close(f);
  }
  else
  {
    const char *editor=getenv("EDITOR");
    if (editor==NULL)
      editor=getenv("VISUAL");
    if (editor==NULL)
      editor="vi";
    editor=omStrDup(editor);

    if (pi->data.s.body==NULL)
    {
      iiGetLibProcBuffer(pi);
      if (pi->data.s.body==NULL)
      {
        PrintS("cannot get the procedure body\n");
        close(f);
        si_unlink(filename);
        omFree(filename);
        return;
      }
    }

    write(f,pi->data.s.body,strlen(pi->data.s.body));
    close(f);

    int pid=fork();
    if (pid!=0)
    {
      si_wait(&pid);
    }
    else if(pid==0)
    {
      if (strchr(editor,' ')==NULL)
      {
        execlp(editor,editor,filename,NULL);
        Print("cannot exec %s\n",editor);
      }
      else
      {
        size_t len=strlen(editor)+strlen(filename)+2;
        char *p=(char *)omAlloc(len);
        snprintf(p,len,"%s %s",editor,filename);
        system(p);
      }
      exit(0);
    }
    else
    {
      PrintS("cannot fork\n");
    }

    FILE* fp=fopen(filename,"r");
    if (fp==NULL)
    {
      Print("cannot read from %s\n",filename);
    }
    else
    {
      fseek(fp,0L,SEEK_END);
      long len=ftell(fp);
      fseek(fp,0L,SEEK_SET);

      omFree((ADDRESS)pi->data.s.body);
      pi->data.s.body=(char *)omAlloc((int)len+1);
      myfread( pi->data.s.body, len, 1, fp);
      pi->data.s.body[len]='\0';
      fclose(fp);
    }
  }
  si_unlink(filename);
  omFree(filename);
}

STATIC_VAR char sdb_lastcmd='c';

void sdb(Voice * currentVoice, const char * currLine, int len)
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
      char *p=(char *)currLine+len-1;
      while ((*p<=' ') && (p!=currLine))
      {
        p--; len--;
      }
      if (p==currLine) return;

      currentVoice->pi->trace_flag&= ~1; // delete flag for "all lines"
      Print("(%s,%d) >>",currentVoice->filename,yylineno);
      fwrite(currLine,1,len,stdout);
      Print("<<\nbreakpoint %d (press ? for list of commands)\n",bp);
      p=fe_fgets_stdin(">>",gdb,80);
      while (*p==' ') p++;
      if (*p >' ')
      {
        sdb_lastcmd=*p;
      }
      Print("command:%c\n",sdb_lastcmd);
      switch(sdb_lastcmd)
      {
        case '?':
        case 'h':
        {
          PrintS(
          "b - print backtrace of calling stack\n"
          "B <proc> [<line>] - define breakpoint\n"
          "c - continue\n"
          "d - delete current breakpoint\n"
          "D - show all breakpoints\n"
          "e - edit the current procedure (current call will be aborted)\n"
          "h,? - display this help screen\n"
          "n - execute current line, break at next line\n"
          "p <var> - display type and value of the variable <var>\n"
          "q <flags> - quit debugger, set debugger flags(0,1,2)\n"
          "   0: stop debug, 1:continue, 2: throw an error, return to toplevel\n"
          "Q - quit Singular\n");
          int i;
          for(i=0;i<7;i++)
          {
            if (sdb_lines[i] != -1)
              Print("breakpoint %d at line %d in %s\n",
                i,sdb_lines[i],sdb_files[i]);
          }
          break;
        }
        case 'd':
        {
          Print("delete break point %d\n",bp);
          currentVoice->pi->trace_flag &= (~Sy_bit(bp));
          if (bp!=0)
          {
            sdb_lines[bp-1]=-1;
          }
          break;
        }
        case 'D':
          sdb_show_bp();
          break;
        #if 0
        case 'l':
        {
          extern void listall(int showproc);
          listall(FALSE);
          break;
        }
        #endif
        case 'n':
          currentVoice->pi->trace_flag|= 1;
          return;
        case 'e':
        {
          sdb_edit(currentVoice->pi);
          sdb_flags=2;
          return;
        }
        case 'p':
        {
          p=sdb_find_arg(p);
          EXTERN_VAR int myynest;
          Print("variable `%s`at level %d",p,myynest);
          idhdl h=ggetid(p);
          if (h==NULL)
            PrintS(" not found\n");
          else
          {
            sleftv tmp;
            memset(&tmp,0,sizeof(tmp));
            tmp.rtyp=IDHDL;
            tmp.data=h;
            Print("(type %s):\n",Tok2Cmdname(tmp.Typ()));
            tmp.Print();
          }
          break;
        }
        case 'b':
          VoiceBackTrack();
          break;
        case 'B':
        {
          p=sdb_find_arg(p);
          Print("procedure `%s` ",p);
          sdb_set_breakpoint(p);
          break;
        }
        case 'q':
        {
          p=sdb_find_arg(p);
          if (*p!='\0')
          {
            sdb_flags=atoi(p);
            Print("new sdb_flags:%d\n",sdb_flags);
          }
          return;
        }
        case 'Q':
          m2_end(999);
        case 'c':
        default:
          return;
      }
    }
  }
}
#endif
