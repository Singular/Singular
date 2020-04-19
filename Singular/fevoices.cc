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
#include "misc/options.h"
#include "reporter/reporter.h"
#include "kernel/oswrapper/feread.h"
#include "Singular/fevoices.h"
#include "Singular/subexpr.h"
#include "Singular/ipshell.h"
#include "Singular/sdb.h"

#include "misc/mylimits.h"
#include <unistd.h>

#ifdef HAVE_PWD_H
#include <pwd.h>
#endif

#define fePutChar(c) fputc((unsigned char)(c),stdout)
/*0 implementation */


VAR char fe_promptstr[] ="  ";
VAR FILE *File_Profiling=NULL;

// line buffer for reading:
// minimal value for MAX_FILE_BUFFER: 4*4096 - see Tst/Long/gcd0_l.tst
// this is an upper limit for the size of monomials/numbers read via the interpreter
#define MAX_FILE_BUFFER 4*4096

/**************************************************************************
* handling of 'voices'
**************************************************************************/

EXTERN_VAR int blocknest; /* scaner.l internal */

VAR int    yy_noeof=0;     // the scanner "state"
VAR int    yy_blocklineno; // to get the lineno of the block start from scanner
VAR Voice  *currentVoice = NULL;
// FILE   *feFilePending; /*temp. storage for grammar.y */

//static const char * BT_name[]={"BT_none","BT_break","BT_proc","BT_example",
//                               "BT_file","BT_execute","BT_if","BT_else"};
/*2
* the name of the current 'Voice': the procname (or filename)
*/
const char sNoName_fe[]="_";
const char * VoiceName()
{
  if ((currentVoice!=NULL)
  && (currentVoice->filename!=NULL))
    return currentVoice->filename;
  return sNoName_fe;
}

int VoiceLine()
{
  if ((currentVoice!=NULL)
  && (currentVoice->curr_lineno>=0))
    return currentVoice->curr_lineno;
  return -1;
}

/*2
* the calling chain of Voices
*/
void VoiceBackTrack()
{
  Voice *p=currentVoice;
  while (p->prev!=NULL)
  {
    p=p->prev;
    char *s=p->filename;
    if (s==NULL)
      PrintS("-- called from ? --\n");
    else
      Print("-- called from %s --\n",s);
  }
}

/*2
* init a new voice similar to the current
*/
void Voice::Next()
{
  Voice *p=new Voice;
  // OB: ???
  // Hmm... when Singular is used as batch file
  // then this voice is never freed
  omMarkAsStaticAddr(p);
  if (currentVoice != NULL)
  {
    currentVoice->curr_lineno=yylineno;
    currentVoice->next=p;
  }
  p->prev=currentVoice;
  currentVoice=p;
  //PrintS("Next:");
}

feBufferTypes Voice::Typ()
{
  switch(typ)
  {
    case BT_proc:
    case BT_example:
    case BT_file:
      return typ;
    default:
      if (prev==NULL) return (feBufferTypes)0;
      return prev->Typ();
  }
}

/*2
* start the file 'fname' (STDIN is stdin) as a new voice (cf.VFile)
* return FALSE on success, TRUE if an error occurs (file cannot be opened)
*/
BOOLEAN newFile(char *fname)
{
  currentVoice->Next();
  //Print(":File%d(%s):%s(%x)\n",
  //  currentVoice->typ,BT_name[currentVoice->typ],fname,currentVoice);
  currentVoice->filename   = omStrDup(fname);
  omMarkAsStaticAddr(currentVoice->filename);
  if (strcmp(fname,"STDIN") == 0)
  {
    currentVoice->files = stdin;
    currentVoice->sw = BI_stdin;
    currentVoice->start_lineno = 1;
  }
  else
  {
    currentVoice->sw = BI_file; /* needed by exitVoice below */
    currentVoice->files = feFopen(fname,"r",NULL,TRUE);
    if (currentVoice->files==NULL)
    {
      exitVoice();
      return TRUE;
    }
    currentVoice->start_lineno = 0;
  }
  yylineno=currentVoice->start_lineno;
  //Voice *p=currentVoice;
  //PrintS("-----------------\ncurr:");
  //do
  //{
  //Print("voice fn:%s\n",p->filename);
  //p=p->prev;
  //}
  //while (p!=NULL);
  //PrintS("----------------\n");
  return FALSE;
}

void newBuffer(char* s, feBufferTypes t, procinfo* pi, int lineno)
{
  currentVoice->Next();
  //Print(":Buffer%d(%s):%s(%x)\n",
  //  t,BT_name[t],pname,currentVoice);
  if (pi!=NULL)
  {
    long l=strlen(pi->procname);
    if (pi->libname!=NULL) l+=strlen(pi->libname);
    currentVoice->filename = (char *)omAlloc(l+3);
    *currentVoice->filename='\0';
    if (pi->libname!=NULL) strcat(currentVoice->filename,pi->libname);
    strcat(currentVoice->filename,"::");
    strcat(currentVoice->filename,pi->procname);
    currentVoice->pi       = pi;
  }
  else
  {
    if(currentVoice->prev!=NULL)
    {
      currentVoice->filename = omStrDup(currentVoice->prev->filename);
      currentVoice->pi       = currentVoice->prev->pi;
    }
    else
    {
      currentVoice->filename = omStrDup("");
      currentVoice->pi       = pi;
    }
  }
  currentVoice->buffer   = s;
  currentVoice->sw       = BI_buffer;
  currentVoice->typ      = t;
  switch (t)
  {
    case BT_execute:
                     yylineno-=2;
                     break;
    case BT_proc:
    case BT_example:
                     currentVoice->oldb=myynewbuffer();
                     yylineno = lineno+1;
                     break;
    case BT_if:
    case BT_else:
    case BT_break:
                     yylineno = yy_blocklineno-1;
                     break;
    //case BT_file:
    default:
                     yylineno = 1;
                     break;
  }
  //Print("start body (%s) at line %d\n",BT_name[t],yylineno);
  currentVoice->start_lineno = yylineno;
  //printf("start buffer typ %d\n",t);
  //Voice *p=currentVoice;
  //PrintS("-----------------\ncurr:");
  //do
  //{
  //Print("voice fn:%s\n",p->filename);
  //p=p->prev;
  //}
  //while (p!=NULL);
  //PrintS("----------------\n");
}

/*2
* exit Buffer of type 'typ':
* returns 1 if buffer type could not be found
*/
BOOLEAN exitBuffer(feBufferTypes typ)
{
  //printf("exitBuffer: %d(%s),(%x)\n",
  //  typ,BT_name[typ], currentVoice);
  //Voice *p=currentVoice;
  //PrintS("-----------------\ncurr:");
  //do
  //{
  //Print("voice fn:%s\n",p->filename);
  //p=p->prev;
  //}
  //while (p!=NULL);
  //PrintS("----------------\n");
  if (typ == BT_break)  // valid inside for, while. may skip if, else
  {
    /*4 first check for valid buffer type, skip if/else*/
    Voice *p=currentVoice;
    loop
    {
      if ((p->typ != BT_if)
      &&(p->typ != BT_else))
      {
        if (p->typ == BT_break /*typ*/)
        {
          while (p != currentVoice)
          {
            exitVoice();
          }
          exitVoice();
          return FALSE;
        }
        else return TRUE;
      }
      if (p->prev==NULL) break;
      p=p->prev;
    }
    /*4 break not inside a for/while: return an error*/
    if (/*typ*/ BT_break != currentVoice->typ) return 1;
    return exitVoice();
  }

  if ((typ == BT_proc)
  || (typ == BT_example))
  {
    Voice *p=currentVoice;
    loop
    {
      if ((p->typ == BT_proc)
      || (p->typ == BT_example))
      {
        while (p != currentVoice)
        {
          exitVoice();
        }
        exitVoice();
        return FALSE;
      }
      if (p->prev==NULL) break;
      p=p->prev;
    }
  }
  /*4 return not inside a proc: return an error*/
  return TRUE;
}

/*2
* jump to the beginning of a buffer
*/
BOOLEAN contBuffer(feBufferTypes typ)
{
  //printf("contBuffer: %d(%s),(%x)\n",
  //  typ,BT_name[typ], currentVoice);
  if (typ == BT_break)  // valid inside for, while. may skip if, else
  {
    // first check for valid buffer type
    Voice *p=currentVoice;
    loop
    {
      if ((p->typ != BT_if)
        &&(p->typ != BT_else))
      {
        if (p->typ == BT_break /*typ*/)
        {
          while (p != currentVoice)
          {
            exitVoice();
          }
          yylineno = currentVoice->start_lineno;
          currentVoice->fptr=0;
          return FALSE;
        }
        else return TRUE;
      }
      if (p->prev==NULL) break;
      p=p->prev;
    }
  }
  return TRUE;
}

/*2
* leave a voice: kill local variables
* setup everything from the previous level
* return 1 if leaving the top level, 0 otherwise
*/
BOOLEAN exitVoice()
{
  //printf("exitVoice: %d(%s),(%x)\n",
  //  currentVoice->typ,BT_name[currentVoice->typ], currentVoice);
  //{
  //Voice *p=currentVoice;
  //PrintS("-----------------\ncurr:");
  //do
  //{
  //Print("voice fn:%s\n",p->filename);
  //p=p->prev;
  //}
  //while (p!=NULL);
  //PrintS("----------------\n");
  //}
  if (currentVoice!=NULL)
  {
    if (currentVoice->oldb!=NULL)
    {
      myyoldbuffer(currentVoice->oldb);
      currentVoice->oldb=NULL;
    }
    if (currentVoice->filename!=NULL)
    {
      omFree((ADDRESS)currentVoice->filename);
      currentVoice->filename=NULL;
    }
    if (currentVoice->buffer!=NULL)
    {
      omFree((ADDRESS)currentVoice->buffer);
      currentVoice->buffer=NULL;
    }
    if ((currentVoice->prev==NULL)
    &&(currentVoice->sw==BI_file)
    &&(currentVoice->files!=stdin))
    {
      currentVoice->prev=feInitStdin(currentVoice);
    }
    if (currentVoice->prev!=NULL)
    {
      //printf("exitVoice typ %d(%s)\n",
      //  currentVoice->typ,BT_name[currentVoice->typ]);
      if (currentVoice->typ==BT_if)
      {
        currentVoice->prev->ifsw=2;
      }
      else
      {
        currentVoice->prev->ifsw=0;
      }
      if ((currentVoice->sw == BI_file)
      && (currentVoice->files!=NULL))
      {
        fclose(currentVoice->files);
      }
      yylineno=currentVoice->prev->curr_lineno;
      currentVoice->prev->next=NULL;
    }
    Voice *p=currentVoice->prev;
    delete currentVoice;
    currentVoice=p;
  }
  return currentVoice==NULL;
}

/*2
* set prompt_char
* only called with currentVoice->sw == BI_stdin
*/
static void feShowPrompt(void)
{
  fe_promptstr[0]=prompt_char;
}

/*2
* print echo (si_echo or TRACE), set my_yylinebuf
*/
static int fePrintEcho(char *anf, char */*b*/)
{
  char *ss=strrchr(anf,'\n');
  int len_s;
  if (ss==NULL)
  {
    len_s=strlen(anf);
  }
  else
  {
    len_s=ss-anf+1;
  }
  // my_yylinebuf:
  int mrc=si_min(len_s,79)-1;
  strcpy(my_yylinebuf,anf+(len_s-1)-mrc);
  if (my_yylinebuf[mrc] == '\n') my_yylinebuf[mrc] = '\0';
  mrc--;
  // handle echo:
  if (((si_echo>myynest)
    && ((currentVoice->typ==BT_proc)
      || (currentVoice->typ==BT_example)
      || (currentVoice->typ==BT_file)
      || (currentVoice->typ==BT_none)
    )
    && (strncmp(anf,";return();",10)!=0)
   )
  || (traceit&TRACE_SHOW_LINE)
  || (traceit&TRACE_SHOW_LINE1))
  {
    if (currentVoice->typ!=BT_example)
    {
      if (currentVoice->filename==NULL)
        Print("(none) %3d%c ",yylineno,prompt_char);
      else
        Print("%s %3d%c ",currentVoice->filename,yylineno,prompt_char);
     }
    {
      fwrite(anf,1,len_s,stdout);
      mflush();
    }
    if (traceit&TRACE_SHOW_LINE)
    {
      char c;
      do
      {
        c=fgetc(stdin);
	if (c=='n') traceit_stop=1;
      }
      while(c!='\n');
    }
  }
  else if (traceit&TRACE_SHOW_LINENO)
  {
    Print("{%d}",yylineno);
    mflush();
  }
  else if (traceit&TRACE_PROFILING)
  {
    if (File_Profiling==NULL)
      File_Profiling=fopen("smon.out","a");
    if (File_Profiling==NULL)
      traceit &= (~TRACE_PROFILING);
    else
    {
      if (currentVoice->filename==NULL)
        fprintf(File_Profiling,"(none) %d\n",yylineno);
      else
        fprintf(File_Profiling,"%s %d\n",currentVoice->filename,yylineno);
    }
  }
#ifdef HAVE_SDB
  if ((blocknest==0)
  && (currentVoice->pi!=NULL)
  && (currentVoice->pi->trace_flag!=0))
  {
    sdb(currentVoice, anf, len_s);
  }
#endif
  prompt_char = '.';
  return len_s;
}

int feReadLine(char* b, int l)
{
  char *s=NULL;
  int offset = 0; /* will not be used if s==NULL*/
  // try to read from the buffer into b, max l chars
  if (currentVoice!=NULL)
  {
    if((currentVoice->buffer!=NULL)
    && (currentVoice->buffer[currentVoice->fptr]!='\0'))
    {
  NewBuff:
      REGISTER int i=0;
      long startfptr=currentVoice->fptr;
      long tmp_ptr=currentVoice->fptr;
      l--;
      loop
      {
        REGISTER char c=
        b[i]=currentVoice->buffer[tmp_ptr/*currentVoice->fptr*/];
        i++;
        if (yy_noeof==noeof_block)
        {
          if (c<' ')  yylineno++;
          else if (c=='}') break;
        }
        else
        {
          if ((c<' ') ||
          (c==';') ||
          (c==')')
          )
            break;
        }
        if (i>=l) break;
        tmp_ptr++;/*currentVoice->fptr++;*/
        if(currentVoice->buffer[tmp_ptr/*currentVoice->fptr*/]=='\0') break;
      }
      currentVoice->fptr=tmp_ptr;
      b[i]='\0';
      if (currentVoice->sw==BI_buffer)
      {
        BOOLEAN show_echo=FALSE;
        char *anf;
        long len;
        if (startfptr==0)
        {
          anf=currentVoice->buffer;
          const char *ss=strchr(anf,'\n');
          if (ss==NULL) len=strlen(anf);
          else          len=ss-anf;
          show_echo=TRUE;
        }
        else if (/*(startfptr>0) &&*/
        (currentVoice->buffer[startfptr-1]=='\n'))
        {
          anf=currentVoice->buffer+startfptr;
          const char *ss=strchr(anf,'\n');
          if (ss==NULL) len=strlen(anf);
          else          len=ss-anf;
          yylineno++;
          show_echo=TRUE;
        }
        if (show_echo)
        {
          char *s=(char *)omAlloc(len+2);
          strncpy(s,anf,len+2);
          s[len+1]='\0';
          fePrintEcho(s,b);
          omFree((ADDRESS)s);
        }
      }
      currentVoice->fptr++;
      return i;
    }
    // no buffer there or e-o-buffer or eoln:
    if (currentVoice->sw!=BI_buffer)
    {
      currentVoice->fptr=0;
      if (currentVoice->buffer==NULL)
      {
        currentVoice->buffer=(char *)omAlloc(MAX_FILE_BUFFER-sizeof(ADDRESS));
        omMarkAsStaticAddr(currentVoice->buffer);
      }
    }
    offset=0;
  NewRead:
    yylineno++;
    if (currentVoice->sw==BI_stdin)
    {
      feShowPrompt();
      s=fe_fgets_stdin(fe_promptstr,
                       &(currentVoice->buffer[offset]),
                       omSizeOfAddr(currentVoice->buffer)-1-offset);
      //int i=0;
      //if (s!=NULL)
      //  while((s[i]!='\0') /*&& (i<MAX_FILE_BUFFER)*/) {s[i] &= (char)127;i++;}
    }
    else if (currentVoice->sw==BI_file)
    {
      fseek(currentVoice->files,currentVoice->ftellptr,SEEK_SET);
      s=fgets(currentVoice->buffer+offset,(MAX_FILE_BUFFER-1-sizeof(ADDRESS))-offset,
              currentVoice->files);
      if (s!=NULL)
      {
        currentVoice->ftellptr=ftell(currentVoice->files);
        // ftell returns -1 for non-seekable streams, such as pipes
        if (currentVoice->ftellptr<0)
          currentVoice->ftellptr=0;
      }
    }
    //else /* BI_buffer */ s==NULL  => return 0
    // done by the default return
  }
  if (s!=NULL)
  {
    // handle prot:
    if (feProt&SI_PROT_I)
    {
      fputs(s,feProtFile);
    }
    int rc=fePrintEcho(s,b)+1;
    //s[strlen(s)+1]='\0'; add an second \0 at the end of the string
    s[rc]='\0';
    // handel \\ :
    rc-=3; if (rc<0) rc=0;
    if ((s[rc]=='\\')&&(currentVoice->sw!=BI_buffer))
    {
      s[rc]='\0';
      offset+=rc;
      if (offset<(int)omSizeOfAddr(currentVoice->buffer)) goto NewRead;
    }
    goto NewBuff;
  }
  /* else if (s==NULL) */
  {
    const char *err;
    switch(yy_noeof)
    {
      case noeof_brace:
      case noeof_block:
        err="{...}";
        break;
      case noeof_asstring:
        err="till `.`";
        break;
      case noeof_string:
        err="string";
        break;
      case noeof_bracket:
        err="(...)";
        break;
      case noeof_procname:
        err="proc";
        break;
      case noeof_comment:
        err="/*...*/";
        break;
      default:
        return 0;
    }
    Werror("premature end of file while reading %s",err);
    return 0;
  }
}

/*2
* init all data structures
*/
#ifndef STDIN_FILENO
#define STDIN_FILENO 0
#endif
Voice * feInitStdin(Voice *pp)
{
  Voice *p = new Voice;
  p->files = stdin;
  p->sw = (isatty(STDIN_FILENO)) ? BI_stdin : BI_file;
  if ((pp!=NULL) && (pp->sw==BI_stdin) && (pp->files==stdin))
  {
    p->files=freopen("/dev/tty","r",stdin);
    //stdin=p->files;
    if (p->files==NULL)
    {
      p->files = stdin;
      p->sw = BI_file;
    }
    else
      p->sw = BI_stdin;
  }
  p->filename   = omStrDup("STDIN");
  p->start_lineno   = 1;
  omMarkAsStaticAddr(p);
  omMarkAsStaticAddr(p->filename);
  return p;
}
#endif

