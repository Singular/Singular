/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/* $Id: febase.inc,v 1.1 1997-09-09 09:15:03 Singular Exp $ */
/*
* ABSTRACT: handling of 'voices'
*/

class Voices
{
  private:
    void Init()
    {
      memset(this,0,sizeof(*this));
      v_lineno = 1;
    }
  public:
    int    v_lineno;        // lineno, to restore in recursion
    int    v_oldlineno;     // lineno, to restore at exit
    int    typ;             // buffer type: see BT_..
    int    v_echo;          // echo, to restore in recursion
    int    v_fileVoice;     // to be restored in recursion
    int    v_inputswitch;   // ??
    FILE * files;           // file handle
    long   fptr;            // file | buffer seek
    char*  filename;        // file name
    char * buffer;          // buffer pointer

  void Next();
  Voices() { Init(); }
  Voices * VFile(char* fname);
  Voices * Buffer(char* buf, int t);
  int Exit();
} ;


extern FILE* yyin;

#define START_LEVMAX 32
int     levmax       = START_LEVMAX;
Voices *currentVoice = NULL;
Voices *FileAttribs  =(Voices *)Alloc0(START_LEVMAX*sizeof(Voices));
short  *ifswitch     =(short *)Alloc0(START_LEVMAX*sizeof(short));
        /*1 ifswitch==0: no if statement, else is invalid
        *           ==1: if (0) processed, execute else
        *           ==2: if (1) processed, else allowed but not executed
        */
int     fileVoice = 0;
int     voice = 0;
int     inputswitch = 0;
int     blocklineno = 0;

/*2
* the name of the current 'voice': the procname (or filename)
*/
const char * VoiceName()
{
  if (FileAttribs[fileVoice].filename!=NULL)
    return FileAttribs[fileVoice].filename;
  return sNoName;
}

/*2
* the name of the 'voice' number 'i': the procname (or filename)
*/
const char * VoiceName(int i)
{
  if (FileAttribs[i].filename!=NULL)
    return FileAttribs[i].filename;
  return sNoName;
}

/*2
* the type of the current voice:BT_proc, BT_example, BT_file
*/
int VoiceType()
{
  int i=fileVoice;
  while ((FileAttribs[i].typ!=BT_proc)
  &&(FileAttribs[i].typ!=BT_example)
  &&(FileAttribs[i].typ!=BT_file)
  &&(i>0))
    i--;
  return FileAttribs[i].typ;
}
/*2
* start the file 'fname' (STDIN is stdin) in the current voice (cf.newVoice)
*/
Voices * Voices::VFile(char* fname)
{
  if (strcmp(fname,"STDIN") == 0)
  {
    yyin = stdin;
    v_inputswitch = 0;
  }
  else
  {
    yyin = feFopen(fname,"r",NULL,TRUE);
    v_inputswitch = -1;
  }
  files      = yyin;
  filename   = mstrdup(fname);
  v_echo     = si_echo;
  fileVoice  = voice;
  yylineno   = 1;
  if (files==NULL)
  {
    inputswitch = 0;
    exitVoice();
    return NULL;
  }
  inputswitch= v_inputswitch;
  return this;
}

/*3
* increment voice counter, allocate new memory
*/
static inline void inc_voice()
{
  voice++;
  if (voice >= levmax)
  {
    FileAttribs=(Voices *)ReAlloc(FileAttribs,
                          levmax*sizeof(Voices),
                          (levmax+16)*sizeof(Voices));
    memset(&FileAttribs[levmax],0,16*sizeof(Voices));
    ifswitch=(short *)ReAlloc(ifswitch,
                          levmax*sizeof(short),
                          (levmax+16)*sizeof(short));
    memset(&ifswitch[levmax],0,16*sizeof(short));
    levmax+=16;
  }
}

/*2
* init a new voice similiar to the current
*/
void Voices::Next()
{
  v_oldlineno = yylineno;
  v_echo      = si_echo;
  v_fileVoice = fileVoice;
  inc_voice();

  currentVoice = &FileAttribs[voice];
  currentVoice->Init();
}

/*2
* start the file 'fname' (STDIN is stdin) as a new voice (cf.VFile)
*/
int newVoice(char* s)
{
  currentVoice->Next();
  return (int)currentVoice->VFile((char *)s);
}

void newBuffer(char* s, int t, char* pname)
{
  currentVoice->Next();
  currentVoice->Buffer(s,t);
  if (pname) currentVoice->filename = mstrdup(pname);
  //printf("start buffer %d typ %d\n",voice,t);
}

Voices * Voices::Buffer(char* buf, int t)
{
  inputswitch = v_inputswitch = t;
  buffer      = buf;
  typ         = t;
  //si_echo        = 0;
  switch (t)
  {
    case BT_example:
    case BT_proc:    v_lineno = yylineno; ::yylineno = 3;     break;
    case BT_file:    v_lineno = yylineno; ::yylineno = 1;     break;
    case BT_if:
    case BT_else:    ::yylineno = v_lineno = blocklineno - 1; break;
    case BT_break:   ::yylineno = v_lineno = blocklineno - 2; break;
  }
  return this;
}

/*2
* after leaving a voice:
* setup everything from the this level
*/
int Voices::Exit()
{
  if (voice >= 0)
  {
    si_echo          = v_echo;
    fileVoice     = v_fileVoice;
    yyin          = files;
    yylineno      = v_oldlineno;
    inputswitch   = v_inputswitch;
    return 0;
  }
  //Print("Exit:%d\n",voice);
  return 1;
}

/*2
* exit Buffer of type 'typ':
* returns 1 if buffer type could not be found
*/
int exitBuffer(int typ)
{
  //printf("exitBuffer: %d\n",typ);
  if (typ == BT_break)  // valid inside for, while. may skip if, else
  {
    /*4 first check for valid buffer type, skip if/else*/
    for (int i=voice; i>0; i--)
    {
      if ((FileAttribs[i].typ == BT_if)
        ||(FileAttribs[i].typ == BT_else)) continue;
      if (FileAttribs[i].typ == BT_break /*typ*/)
      {
        while ((/*typ*/ BT_break != currentVoice->typ)
        && (voice > 0))
        {
          exitVoice();
        }
        return exitVoice();
      }
      else return 1;
    }
    /*4 break not inside a for/while: return an error*/
    if (/*typ*/ BT_break != currentVoice->typ) return 1;
    return exitVoice();
  }

  if ((typ == BT_proc)
  || (typ == BT_example))
  {
    for (int i=voice; i>0; i--)
    {
      if (FileAttribs[i].typ == 0) break;
      if ((FileAttribs[i].typ == BT_proc)
      || (FileAttribs[i].typ == BT_example))
      {
        while ((BT_proc != currentVoice->typ)
          && (BT_example != currentVoice->typ)
        && (voice > 0))
        {
          exitVoice();
        }
        return exitVoice();
      }
    }
  }
  /*4 return not inside a proc: return an error*/
  return 1;
}

/*2
* jump to the beginning of a buffer
*/
int contBuffer(int typ)
{
  if (typ == BT_break)  // valid inside for, while. may skip if, else
  {
    // first check for valid buffer type
    for (int i=voice; i>0; i--)
    {
      if ((FileAttribs[i].typ == BT_if)
        ||(FileAttribs[i].typ == BT_else)) continue;
      if (FileAttribs[i].typ == BT_break /*typ*/)
      {
        while (/*typ*/ BT_break != currentVoice->typ && (voice > i))
        {
          exitVoice();
        }
        currentVoice->fptr = 0L;
        yylineno = currentVoice->v_lineno;
        return 0;
      }
      else return 1;
    }
  }
  return 1;
}

/*2
* leave a voice: kill local variables
* setup everything from the previous level (via Exit)
*/
int exitVoice()
{
  if (voice <= 0)   
  {
    if (feBatch) return 1;
    else m2_end(0);
  }  
  //printf("exitVoice %d, typ %d\n",voice,FileAttribs[voice].typ);
  if (FileAttribs[voice].typ==BT_if)
  {
    ifswitch[voice-1]=2;
  }
  else
  {
    ifswitch[voice-1]=0;
    //if ((FileAttribs[voice].typ==BT_proc)
    //||(FileAttribs[voice].typ==BT_example)
    //||(FileAttribs[voice].typ==0))
    //{
    //  killlocals(myynest);
    //  printf("killlocals %d\n",myynest);
    //}
  }
  if (inputswitch == -1)
  {
    fclose(yyin);
  }
  else if (inputswitch > 0)
  {
    if (FileAttribs[voice].filename!=NULL)
    {
      FreeL((ADDRESS)FileAttribs[voice].filename);
      FileAttribs[voice].filename=NULL;
    }
    if (FileAttribs[voice].buffer!=NULL)
    {
      FreeL((ADDRESS)FileAttribs[voice].buffer);
      FileAttribs[voice].buffer=NULL;
    }
  }
  currentVoice = &FileAttribs[--voice];
  return currentVoice->Exit();
}

int readbuf(char* buf, int l)
{
  char *s;
  char * t = buf;
  int i = 0;
  long fl = currentVoice->fptr;
  if (fl == -1L)
  {
    t[0] = '\0';
    exitVoice();
    return 0;
  }

  s = currentVoice->buffer + fl;
  while (l > 0)
  {
    fl++;
    i++;
    l--;
    *t++ = *s;
    if (*s == '\n')
    {
      *t = '\0';
      if ((si_echo > voice) || (inputswitch == 0) || (traceit&TRACE_SHOW_LINE)
      || (traceit&TRACE_SHOW_LINE1))
      {
        if (currentVoice->filename==NULL)
          Print("(none) %3d%c ",yylineno,prompt_char);
        else if (VoiceType()!=BT_example)
          Print("%s %3d%c ",currentVoice->filename,yylineno,prompt_char);
        prompt_char = '.';
      }
      if (*(s+1) == '\0')
      {
        currentVoice->fptr = -1;
        FreeL((ADDRESS)(currentVoice->buffer));
        currentVoice->buffer=NULL;
        FileAttribs[voice].buffer = NULL;
        exitVoice();
      }
      else
        currentVoice->fptr = fl;
      return i;
    }
    else if (*s == '\0')
    {
      if ((si_echo > voice) || (inputswitch == 0) || (traceit&TRACE_SHOW_LINE)
      || (traceit&TRACE_SHOW_LINE1))
      {
        if (currentVoice->filename==NULL)
          Print("(none) %3d%c ",yylineno,prompt_char);
        else
          Print("%s %3d%c ",currentVoice->filename,yylineno,prompt_char);
        prompt_char = '.';
      }
      currentVoice->fptr = -1;
      FreeL((ADDRESS)(currentVoice->buffer));
      currentVoice->buffer = NULL;
      exitVoice();
      return i-1;
    }
    s++;
  }
  currentVoice->fptr = fl;
  return i;
}

/*2
* init all data structures
*/
void I_FEbase(void)
{
  currentVoice = FileAttribs[0].VFile("STDIN");
}
