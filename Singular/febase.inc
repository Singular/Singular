/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/* $Id: febase.inc,v 1.10 1998-05-07 17:53:25 Singular Exp $ */
/*
* ABSTRACT: handling of 'voices'
*/
#include <ctype.h>

int    yy_noeof=0;     // the scanner "state"
int    yy_blocklineno; // to get the lineno of the block start from scanner
Voice  *currentVoice = NULL;
FILE   *feFilePending; /*temp. storage for grammar.y */

static char * BT_name[]={"BT_none","BT_break","BT_proc","BT_example",
                       "BT_file","BT_execute","BT_if","BT_else"};
/*2
* the name of the current 'Voice': the procname (or filename)
*/
const char * VoiceName()
{
  if ((currentVoice!=NULL)
  && (currentVoice->filename!=NULL))
    return currentVoice->filename;
  return sNoName;
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
* init a new voice similiar to the current
*/
void Voice::Next()
{
  Voice *p=new Voice;
  if (currentVoice != NULL)
  {
    currentVoice->curr_lineno=yylineno;
    currentVoice->next=p;
  }
  p->prev=currentVoice;
  currentVoice=p;
  //Print("Next:");
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
BOOLEAN newFile(char *fname,FILE* f)
{
  currentVoice->Next();
  //Print(":File%d(%s):%s(%x)\n",
  //  currentVoice->typ,BT_name[currentVoice->typ],fname,currentVoice);
  currentVoice->filename   = mstrdup(fname);
  if (strcmp(fname,"STDIN") == 0)
  {
    currentVoice->files = stdin;
    currentVoice->sw = BI_stdin;
    currentVoice->start_lineno = 1;
  }
  else
  {
    currentVoice->sw = BI_file; /* needed by exitVoice below */
    if (f!=NULL)
      currentVoice->files = f;
    else
    {
      currentVoice->files = feFopen(fname,"r",NULL,TRUE);
      if (currentVoice->files==NULL)
      {
        exitVoice();
        return TRUE;
      }
    }
    currentVoice->start_lineno = 0;
  }
  yylineno=currentVoice->start_lineno;
  //Voice *p=currentVoice;
  //Print("-----------------\ncurr:");
  //do
  //{
  //Print("voice fn:%s\n",p->filename);
  //p=p->prev;
  //}
  //while (p!=NULL);
  //Print("----------------\n");
  return FALSE;
}

void newBuffer(char* s, feBufferTypes t, procinfo* pi, int lineno)
{
  currentVoice->Next();
  //Print(":Buffer%d(%s):%s(%x)\n",
  //  t,BT_name[t],pname,currentVoice);
  if (pi!=NULL)
  {
    int l=strlen(pi->procname);
    if (pi->libname!=NULL) l+=strlen(pi->libname);
    currentVoice->filename = (char *)AllocL(l+3);
    *currentVoice->filename='\0';
    if (pi->libname!=NULL) strcat(currentVoice->filename,pi->libname);
    strcat(currentVoice->filename,"::");
    strcat(currentVoice->filename,pi->procname);
    currentVoice->pi       = pi;
  }
  else
  {
    currentVoice->filename = mstrdup(currentVoice->prev->filename);
    currentVoice->pi       = currentVoice->prev->pi;
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
                     yylineno = yy_blocklineno;
                     break;
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
  //Print("-----------------\ncurr:");
  //do
  //{
  //Print("voice fn:%s\n",p->filename);
  //p=p->prev;
  //}
  //while (p!=NULL);
  //Print("----------------\n");
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
  //Print("-----------------\ncurr:");
  //do
  //{
  //Print("voice fn:%s\n",p->filename);
  //p=p->prev;
  //}
  //while (p!=NULL);
  //Print("----------------\n");
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
  //Print("-----------------\ncurr:");
  //do
  //{
  //Print("voice fn:%s\n",p->filename);
  //p=p->prev;
  //}
  //while (p!=NULL);
  //Print("----------------\n");
  //}
  if (currentVoice->oldb!=NULL)
  {
    myyoldbuffer(currentVoice->oldb);
    currentVoice->oldb=NULL;
  }
  if ((currentVoice->prev==NULL)&&(currentVoice->sw==BI_file))
  {
    currentVoice->prev=feInitStdin();
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
    if (currentVoice->filename!=NULL)
    {
      FreeL((ADDRESS)currentVoice->filename);
      currentVoice->filename=NULL;
    }
    if (currentVoice->buffer!=NULL)
    {
      FreeL((ADDRESS)currentVoice->buffer);
      currentVoice->buffer=NULL;
    }
    yylineno=currentVoice->prev->curr_lineno;
  }
  Voice *p=currentVoice->prev;
  delete currentVoice;
  currentVoice=p;
  return currentVoice==0;
}

/*2
* set prompt_char 
* only called with currentVoice->sw == BI_stdin
*/
static void feShowPrompt(void)
{
  //if (currentVoice->sw == BI_stdin)
  {
#ifdef HAVE_TCL
    if (tclmode)
    {
       PrintTCL('P',(prompt_char=='>')? 0 : 1,NULL);
    }
    else
#endif
    if ((BVERBOSE(V_PROMPT))&&(!feBatch))
    {
      fe_promptstr[0]=prompt_char;
#ifndef HAVE_READLINE
      PrintS(fe_promptstr);
#endif
    }
    mflush();
#ifdef macintosh
    cols = 0;
#endif
  }
}

/*2
* print echo (si_echo or TRACE), set my_yylinebuf
*/
static int fePrintEcho(char *anf)
{
  char *ss=strchr(anf,'\n');
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
  int mrc=min(len_s,80)-1;
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
    fwrite(anf,1,len_s,stdout);
    mflush();
    if (traceit&TRACE_SHOW_LINE)
    {
      while(fgetc(stdin)!='\n');
    }
  }
  else if (traceit&TRACE_SHOW_LINENO)
  {
    printf("{%d}",yylineno);
    mflush();
  }
  prompt_char = '.';
  return len_s;
}

int feReadLine(char* b, int l)
{
  // try to read from the buffer into b, max l chars
  if ((currentVoice->buffer!=NULL)
  && (currentVoice->buffer[currentVoice->fptr]!='\0'))
  {
NewBuff:
    int i=0;
    int startfptr=currentVoice->fptr;
    l--;
    loop
    {
      char c=
      b[i]=currentVoice->buffer[currentVoice->fptr];
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
      currentVoice->fptr++;
      if(currentVoice->buffer[currentVoice->fptr]=='\0') break;
    }
    b[i]='\0';
    if (currentVoice->sw==BI_buffer)
    {
      if (startfptr==0)
      {
        char *anf=currentVoice->buffer;
        char *ss=strchr(anf,'\n');
        int len;
        if (ss==NULL) len=strlen(anf);
        else          len=ss-anf;
        char *s=(char *)AllocL(len+2);
        strncpy(s,anf,len+2);
        s[len+1]='\0';
        fePrintEcho(s);
        FreeL((ADDRESS)s);
      }
      else if (/*(startfptr>0) &&*/
      (currentVoice->buffer[startfptr-1]=='\n'))
      {
        char *anf=currentVoice->buffer+startfptr;
        char *ss=strchr(anf,'\n');
        int len;
        if (ss==NULL) len=strlen(anf);
        else          len=ss-anf;
        char *s=(char *)AllocL(len+2);
        strncpy(s,anf,len+2);
        s[len+1]='\0';
        yylineno++;
        fePrintEcho(s);
        FreeL((ADDRESS)s);
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
      currentVoice->buffer=(char *)AllocL(4096-sizeof(ADDRESS));
  }
  char *s;
  int offset=0;
NewRead:
  yylineno++;
  if (currentVoice->sw==BI_stdin)
  {
    feShowPrompt();
    s=fe_fgets_stdin(currentVoice->buffer+offset,(4096-1-sizeof(ADDRESS))-offset);
  }
  else if (currentVoice->sw==BI_file)
  {
    s=fgets(currentVoice->buffer+offset,(4096-1-sizeof(ADDRESS))-offset,
            currentVoice->files);
  }
  else /* BI_buffer */
  {
    return 0;
  }
  if (s!=NULL)
  {
    // handle prot:
    if (feProt&PROT_I)
    {
      fputs(s,feProtFile);
    }
    int rc=fePrintEcho(s)+1;
    //s[strlen(s)+1]='\0'; add an second \0 at the end of the string
    s[rc]='\0';
    // handel \\ :
    rc-=3;
    if ((s[rc]=='\\')&&(currentVoice->sw!=BI_buffer))
    {
      s[rc]='\0';
      offset+=rc;
      if (offset<(int)mmSizeL(currentVoice->buffer)) goto NewRead;
    }
    goto NewBuff;
  }
  /* else if (s==NULL) */
  {
    switch(yy_noeof)
    {
      case 0:
        return 0;
      case noeof_brace:
      case noeof_block:
        WerrorS("premature end of file while reading {...}");
        break;
      case noeof_asstring:
        WerrorS("premature end of file while reading till `.`");
        break;
      case noeof_string:
        WerrorS("premature end of file while reading string");
        break;
      case noeof_bracket:
        WerrorS("premature end of file while reading (...)");
        break;
      case noeof_procname:
        WerrorS("premature end of file while reading proc");
        break;
    }
    return 0;
  }
}

#ifdef __MWERKS__
#ifdef __cplusplus
extern "C" {
#endif
#ifdef macintosh
int    isatty(int filedes);
#else
int    _isatty(int filedes);
#define isatty  _isatty
#endif /* macintosh */
#ifdef __cplusplus
}
#endif
#endif
/*2
* init all data structures
*/
#ifndef STDIN_FILENO
#define STDIN_FILENO 0
#endif
Voice * feInitStdin()
{
  Voice *p = new Voice;
  p->files = stdin;
  p->sw = isatty(STDIN_FILENO) ? BI_stdin : BI_file;
  p->filename   = mstrdup("STDIN");
  p->start_lineno   = 1;
  return p;
}
