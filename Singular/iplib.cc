/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/* $Id: iplib.cc,v 1.12 1998-03-16 16:21:55 Singular Exp $ */
/*
* ABSTRACT: interpreter: LIB and help
*/

//#include <stdlib.h>
#include <stdio.h>
#include <string.h>
//#include <ctype.h>

#include "mod2.h"
#include "tok.h"
#include "ipid.h"
#include "mmemory.h"
#include "febase.h"
#include "ring.h"
#include "subexpr.h"
#include "ipshell.h"
#include "lists.h"

procinfo *iiInitSingularProcinfo(procinfo *pi, char *libname, char *procname,
                                 int line, long pos, BOOLEAN pstatic=FALSE);
char *iiConvName(char *p);
#ifdef HAVE_LIBPARSER
int yylplex(char *libname, char *libfile);
void yylprestart (FILE *input_file );
int current_pos(int i=0);
extern int yylp_errno;
extern int yylplineno;
extern char *yylp_errlist[];
void print_init();
void reinit_yylp();
libstackv library_stack;
#endif

/*2
* find the library of an proc:
*  => return (pi->libname)
*/
char * iiGetLibName(procinfov pi)
{
  char *res=NULL;

  res = pi->libname;
  return res;
}
/*2
* given a line 'proc[ ]+{name}[ \t]*'
* return a pointer to name and set the end of '\0'
* changes the input!
* returns: e: pointer to 'end of name'
*          ct: changed char at the end of s
*/
char* iiProcName(char *buf, char & ct, char* &e)
{
  char *s=buf+5;
  while (*s==' ') s++;
  e=s+1;
  while ((*e>' ') && (*e!='(')) e++;
  ct=*e;
  *e='\0';
  return s;
}

/*2
* given a line with args, return the argstr
*/
char * iiProcArgs(char *e,BOOLEAN withParenth)
{
  while ((*e==' ') || (*e=='(')) e++;
  if (*e<' ')
  {
    if (withParenth)
    {
      // no argument list, allow list #
      return mstrdup("parameter list #;");
    }
    else
    {
      // empty list
      return mstrdup("");
    }
  }
  BOOLEAN in_args;
  BOOLEAN args_found;
  char *s;
  char *argstr=(char *)AllocL(200);
  *argstr='\0';
  do
  {
    args_found=FALSE;
    s=e; // set s to the starting point of the arg
         // and search for the end
    while ((*e!=',')&&(*e!=')')&&(*e!='\0'))
    {
      args_found=args_found || (*e>' ');
      e++;
    }
    in_args=(*e==',');
    if (args_found)
    {
      *e='\0';
      // copy the result to argstr
      strcat(argstr,"parameter ");
      strcat(argstr,s);
      strcat(argstr,";\n");
      e++; // e was pointing to ','
    }
  } while (in_args);
  return argstr;
}

/*2
* locate `procname` in lib `libname` and find the part `part`:
*  part=0: help, between, but excluding the line "proc ..." and "{...":
*    => return
*  part=1: body, between "{ ..." and "}", including the 1. line, w/o "{"
*    => set pi->data.s.body, return NULL
*  part=2: example, between, but excluding the line "exapmle {..." and "}":
*    => return
*/
char* iiGetLibProcBuffer(procinfo *pi, int part )
{
  char buf[256], *s = NULL, *p;
  long procbuflen;

  FILE * fp = feFopen( pi->libname, "rb", NULL, TRUE );
  if (fp==NULL)
  {
    return NULL;
  }

  fseek(fp, pi->data.s.proc_start, SEEK_SET);
  if(part==0)
  { // load help string
    procbuflen = pi->data.s.body_start - pi->data.s.proc_start;
    //Print("Help=%ld-%ld=%d\n", pi->data.s.body_start,
    //    pi->data.s.proc_start, procbuflen);
    s = (char *)AllocL(procbuflen);
    fread(s, procbuflen, 1, fp);
    s[procbuflen] = '\0';
    return(s);
  }
  if(part==1)
  { // load proc part
    procbuflen = pi->data.s.def_end - pi->data.s.proc_start;
    //fgets(buf, sizeof(buf), fp);
    fread( buf, procbuflen, 1, fp);
    char ct;
    char *e;
    s=iiProcName(buf,ct,e);
    char *argstr=NULL;
    *e=ct;
    argstr=iiProcArgs(e,TRUE);
    procbuflen = pi->data.s.body_end - pi->data.s.body_start;
    pi->data.s.body = (char *)AllocL( strlen(argstr)+procbuflen+15+
                                      strlen(pi->libname) );
    //Print("Body=%ld-%ld=%d\n", pi->data.s.body_end,
    //    pi->data.s.body_start, procbuflen);
    if (pi->data.s.body==NULL)
    {
      Werror( "unable to allocate proc buffer `%s`", pi->procname );
      return NULL;
    }
    fseek(fp, pi->data.s.body_start, SEEK_SET);
    strcpy(pi->data.s.body,argstr);
    fread( pi->data.s.body+strlen(argstr), procbuflen, 1, fp);
    procbuflen+=strlen(argstr);
    FreeL(argstr);
    fclose( fp );
    pi->data.s.body[procbuflen] = '\0';
    strcat( pi->data.s.body+procbuflen, "\n;return();\n\n" );
    strcat( pi->data.s.body+procbuflen+13,pi->libname);
    s=strchr(pi->data.s.body,'{');
    if (s!=NULL) *s=' ';
    return NULL;
  }
  if(part==2)
  { // load example
    fseek(fp, pi->data.s.example_start, SEEK_SET);
    fgets(buf, sizeof(buf), fp);
    procbuflen = pi->data.s.proc_end - pi->data.s.example_start - strlen(buf);
    //Print("Example=%ld-%ld=%d\n", pi->data.s.proc_end,
    //  pi->data.s.example_start, procbuflen);
    s = (char *)AllocL(procbuflen+14);
    fread(s, procbuflen, 1, fp);
    s[procbuflen] = '\0';
    strcat(s+procbuflen-3, "\n;return();\n\n" );
    p=strchr(s,'{');
    if (p!=NULL) *s=' ';
    return(s);
  }
  return NULL;
}

/*2
* start a proc
* parameters are built as exprlist
* TODO:interrupt
* return FALSE on success, TRUE if an error occurs
*/
BOOLEAN iiPStart(idhdl pn, sleftv  * v)
{
  char * str;
  BOOLEAN err=FALSE;
  int old_echo=si_echo;

  /* init febase ======================================== */
  /* we do not enter this case if filename != NULL !! */
  if (pn!=NULL)
  {
    procinfov pi;
    pi = IDPROC(pn);
    if(pi!=NULL)
    {
      if( pi->data.s.body==NULL )
      {
        iiGetLibProcBuffer(IDPROC(pn));
        if (IDPROC(pn)->data.s.body==NULL) return TRUE;
      }
      newBuffer( mstrdup(IDPROC(pn)->data.s.body), BT_proc,
                 pi, IDPROC(pn)->data.s.body_lineno );
    }
    //else
    //{ // for security only
    //  newBuffer( mstrdup(IDSTRING(pn)), BT_proc, IDID(pn) );
    //}
  }
  /* generate argument list ======================================*/
  if (v!=NULL)
  {
    iiCurrArgs=(leftv)Alloc(sizeof(sleftv));
    memcpy(iiCurrArgs,v,sizeof(sleftv));
    memset(v,0,sizeof(sleftv));
  }
  else
  {
    iiCurrArgs=NULL;
  }
  /* start interpreter ======================================*/
  //Print("PStart <<%s>>\n",IDID(pn));
  myynest++;
  err=yyparse();
  killlocals(myynest);
  myynest--;
  si_echo=old_echo;
  //Print("PEnd <<%s>>\n",IDID(pn));

  return err;
}

ring    *iiLocalRing
#ifdef TEST
                    =NULL
#endif
                   ;
sleftv  *iiRETURNEXPR
#ifdef TEST
                    =NULL
#endif
                   ;
int     iiRETURNEXPR_len=0;

#ifdef RDEBUG
static void iiShowLevRings()
{
  int i;
  for (i=1;i<=myynest;i++)
  {
    Print("lev %d:",i);
    if (iiLocalRing[i]==NULL) PrintS("NULL");
    else                      Print("%d",iiLocalRing[i]);
    Print("\n");
  }
  if (currRing==NULL) PrintS("curr:NULL\n");
  else                Print ("curr:%d\n",currRing->no);
}
#endif

static void iiCheckNest()
{
  if (myynest >= iiRETURNEXPR_len-1)
  {
    iiRETURNEXPR=(sleftv *)ReAlloc(iiRETURNEXPR,
                                   iiRETURNEXPR_len*sizeof(sleftv),
                                   (iiRETURNEXPR_len+16)*sizeof(sleftv));
    iiLocalRing=(ring *)ReAlloc(iiLocalRing,
                                   iiRETURNEXPR_len*sizeof(ring),
                                   (iiRETURNEXPR_len+16)*sizeof(ring));
    iiRETURNEXPR_len+=16;
  }
}
sleftv * iiMake_proc(idhdl pn, sleftv* sl)
{
  int err;
  procinfov pi = IDPROC(pn);
  iiCheckNest();
  iiLocalRing[myynest]=currRing;
  iiRETURNEXPR[myynest+1].Init();
  if (traceit&TRACE_SHOW_PROC)
  {
    if (traceit&TRACE_SHOW_LINENO) printf("\n");
    printf("entering %s (level %d)\n",IDID(pn),myynest);
  }
#ifdef RDEBUG
  if (traceit&TRACE_SHOW_RINGS) iiShowLevRings();
#endif
#if 1
  if(pi->language == LANG_SINGULAR) err=iiPStart(pn,sl);
  if(pi->language == LANG_C)
  {
    leftv res = (leftv)Alloc0(sizeof(sleftv));
    err = (pi->data.o.function)(res, sl);
    iiRETURNEXPR[myynest+1].Copy(res);
    Free((ADDRESS)res, sizeof(sleftv));
  }
#else
  switch (pi->language)
  {
    case LANG_SINGULAR: err=iiPStart(pn,sl); break;
    case LANG_C: leftv res = (leftv)Alloc0(sizeof(sleftv));
      err = (pi->data.o.function)(res, sl);
      iiRETURNEXPR[myynest+1].Copy(res);
      Free((ADDRESS)res, sizeof(sleftv));
      break;
    default: err=TRUE;
  }
#endif
  if (traceit&TRACE_SHOW_PROC)
  {
    if (traceit&TRACE_SHOW_LINENO) printf("\n");
    printf("leaving  %s (level %d)\n",IDID(pn),myynest);
  }
#ifdef RDEBUG
  if (traceit&TRACE_SHOW_RINGS) iiShowLevRings();
#endif
  if (err)
  {
    iiRETURNEXPR[myynest+1].CleanUp();
    iiRETURNEXPR[myynest+1].Init();
  }
  if (iiLocalRing[myynest] != currRing)
  {
    if (((iiRETURNEXPR[myynest+1].Typ()>BEGIN_RING)
      && (iiRETURNEXPR[myynest+1].Typ()<END_RING))
    || ((iiRETURNEXPR[myynest+1].Typ()==LIST_CMD)
      && (lRingDependend((lists)iiRETURNEXPR[myynest+1].Data()))))
    {
      //idhdl hn;
      char *n;
      char *o;
      if (iiLocalRing[myynest]!=NULL) o=rFindHdl(iiLocalRing[myynest],NULL)->id;
      else                            o="none";
      if (currRing!=NULL)             n=rFindHdl(currRing,NULL)->id;
      else                            n="none";
      Werror("ring change during procedure call: %s -> %s",o,n);
      iiRETURNEXPR[myynest+1].CleanUp();
      err=TRUE;
    }
    if (iiLocalRing[myynest]!=NULL)
    {
      rSetHdl(rFindHdl(iiLocalRing[myynest],NULL),TRUE);
      iiLocalRing[myynest]=NULL;
    }
    else
    { currRingHdl=NULL; currRing=NULL; }
  }
  if (iiCurrArgs!=NULL)
  {
    Warn("too many arguments for %s",IDID(pn));
    iiCurrArgs->CleanUp();
    Free((ADDRESS)iiCurrArgs,sizeof(sleftv));
    iiCurrArgs=NULL;
  }
  if (err) return NULL;
  return &iiRETURNEXPR[myynest+1];
}

/*2
* start an example (as a proc),
* destroys the string 'example'
*/
BOOLEAN iiEStart(char* example, procinfo *pi)
{
  BOOLEAN err;
  int old_echo=si_echo;

  newBuffer( example, BT_example, pi, pi->data.s.example_lineno );
  iiCheckNest();
  iiLocalRing[myynest]=currRing;
  if (traceit&TRACE_SHOW_PROC)
  {
    if (traceit&TRACE_SHOW_LINENO) printf("\n");
    printf("entering example (level %d)\n",myynest);
  }
  myynest++;
  err=yyparse();
  killlocals(myynest);
  myynest--;
  si_echo=old_echo;
  if (traceit&TRACE_SHOW_PROC)
  {
    if (traceit&TRACE_SHOW_LINENO) printf("\n");
    printf("leaving  -example- (level %d)\n",myynest);
  }
  if (iiLocalRing[myynest] != currRing)
  {
    if (iiLocalRing[myynest]!=NULL)
    {
      rSetHdl(rFindHdl(iiLocalRing[myynest],NULL),TRUE);
      iiLocalRing[myynest]=NULL;
    }
    else
    {
      currRingHdl=NULL;
      currRing=NULL;
    }
  }
  return err;
}

BOOLEAN iiLibCmd( char *newlib, BOOLEAN tellerror )
{
  char buf[256];
  char libnamebuf[128];
  idhdl h,hl;
  int lines = 1;
  long pos = 0L;
  procinfov pi;
  FILE * fp = feFopen( newlib, "r", libnamebuf, tellerror );
  if (fp==NULL)
  {
    return TRUE;
  }
  hl = idroot->get("LIB",0);
  if (hl==NULL)
  {
    hl = enterid( mstrdup("LIB"),0, STRING_CMD, &idroot, FALSE );
    IDSTRING(hl) = mstrdup(newlib);
  }
  else
  {
#ifdef TEST
    if (IDSTRING(hl) != NULL)
#endif
    {
      char *s = (char *)AllocL( strlen(newlib) + strlen(IDSTRING(hl)) + 2 );
      strcpy(s,IDSTRING(hl));
      BOOLEAN f=FALSE;
      if(strchr(s,',')==NULL)
      {
        if (strcmp(s,newlib)==0)
          f=TRUE;
      }
      else
      {
        char *p=strtok(s,",");
        do
        {
          if(strcmp(p,newlib)==0)
          {
            f=TRUE;
            break;
          }
          p=strtok(NULL,",");
        } while (p!=NULL);
      }
      if (f)
        FreeL(s);
      else
      {
        sprintf( s, "%s,%s", IDSTRING(hl), newlib);
        FreeL((ADDRESS)IDSTRING(hl));
        IDSTRING(hl) = s;
      }
    }
#ifdef TEST
    else
    {
      PrintS("## empty LIB string\n");
      IDSTRING(hl) = mstrdup(newlib);
    }
#endif
  }

#ifdef HAVE_LIBPARSER
  extern FILE *yylpin;
  libstackv ls_start = library_stack;

  yylpin = fp;
# if YYLPDEBUG > 1
  print_init();
#  endif
  yylplex(newlib, libnamebuf);
  if(yylp_errno) {
    Werror("Library %s: ERROR occured: in line %d, %d.", newlib, yylplineno,
         current_pos(0));
    Werror(yylp_errlist[yylp_errno], yylplineno);
    Werror("Cannot load library,... aborting.");
    reinit_yylp();
    fclose( yylpin );
    FreeL((ADDRESS)newlib);
    return TRUE;
  }
  reinit_yylp();
  fclose( yylpin );
  {
    libstackv ls;
    for(ls = library_stack; (ls != NULL) && (ls != ls_start); ) {
      if(ls->to_be_done) {
        //Print("// Processing id %d LIB:%s\n", ls->cnt, ls->get());
        ls->to_be_done=FALSE;
        iiLibCmd(ls->get());
        ls = ls->pop(newlib);
        //Print("Done\n");
      }
    }
#if 0
    Print("--------------------\n");
    for(ls = library_stack; ls != NULL; ls = ls->next) {
      Print("%s: LIB-stack:(%d), %s %s\n", newlib, ls->cnt, ls->get(),
        ls->to_be_done ? "not loaded" : "loaded");
    }
    Print("--------------------\n");
#endif
  }
#else /* HAVE_LIBPARSER */
  // processing head section
  if (fgets( buf, sizeof(buf), fp))
  {
    if (BVERBOSE(V_LOAD_LIB))
    {
      if (strncmp( buf, "// $Id", 5) == 0)
      {
        char ver[10];
        char date[16];
        ver[0]='?'; ver[1]='.'; ver[2]='?'; ver[3]='\0';
        date[0]='?'; date[1]='\0';
        sscanf(buf,"// %*s %*s %10s %16s",ver,date);
        strcat(libnamebuf,"(");
        strcat(libnamebuf,ver);
        strcat(libnamebuf,",");
        strcat(libnamebuf,date);
        strcat(libnamebuf,")");
      }
      else
      {
        strcat(libnamebuf,"(**unknown version**)");
      }
      Warn( "loading %s", libnamebuf );
    }
  }

  #define IN_HEADER 1
  #define IN_BODY   2
  #define IN_EXAMPLE      3
  #define IN_EXAMPLE_BODY 4
  #define IN_LIB_HEADER   5
  int v=-1,otherLines=0,inBlock=IN_LIB_HEADER;
  do /*while (fgets( buf, sizeof(buf), fp))*/
  {
    int  offset;
    if (buf[0]!='\n')
    {
      if ((inBlock==0)||(inBlock==IN_LIB_HEADER))
      {
        if (strncmp( buf, "LIB ", 4) == 0)
        {
          char *s=buf+5;
          char *f=strchr(s,'"');
          if (f!=NULL)
            *f='\0';
          else
            return TRUE;
          // if (BVERBOSE(V_LOAD_LIB)) Print("// requires %s",s);
          f=strstr(IDSTRING(hl),s);
          if (f == NULL)
          {
            // if (BVERBOSE(V_LOAD_LIB)) PrintLn();
            iiLibCmd(mstrdup(s));
            // if (BVERBOSE(V_LOAD_LIB)) Print( "// loading %s\n", newlib);
          }
          //else if (BVERBOSE(V_LOAD_LIB)) PrintS(" -> already loaded\n");
        }
        else if (strncmp( buf, "proc ", 5) == 0)
        {
          char proc[256];
          char ct1, *e;
          sscanf( buf, "proc %s", proc);
          offset = 2;
          char *ct=strchr(proc,'(');
          if (ct!=NULL) { *ct='\0'; offset=3; }
          sprintf( buf, "LIB:%s", newlib);
#if 0
          if(strcmp(proc, "_init")==0)
          {
            char *p =  iiConvName(newlib);
            Print("Init found:%s;\n", p);
            h = enterid( mstrdup(p), myynest, PROC_CMD, &idroot, FALSE );
            FreeL((ADDRESS)p);
          } else
#endif
            h = enterid( mstrdup(proc), myynest, PROC_CMD, &idroot, FALSE );
          if (h!=NULL)
          {
            iiInitSingularProcinfo(IDPROC(h),newlib,proc,lines,pos);
            if (BVERBOSE(V_LOAD_PROC)) Warn( "     proc %s loaded", proc );
          }
          inBlock=IN_HEADER;
        }
        else if (strncmp( buf, "// ver:", 7) == 0)
        {
          v=0;
          sscanf( buf+7, "%d", &v);
          if(v!=(SINGULAR_VERSION/100))
            Warn("version mismatch - library `%s` requires:%d.%d",
                  newlib,v/1000,(v%1000)/100);
        }
        else if (strncmp( buf, "example", 7) == 0)
        {
          IDPROC(h)->data.s.example_start = pos;
          IDPROC(h)->data.s.example_lineno = lines;
          inBlock=IN_EXAMPLE;
        }
        else if (strncmp( buf, "//", 2) != 0)
        {
          if (inBlock==0)
          {
            otherLines++;
          }
        }
      }
      else if ((inBlock==IN_HEADER) || (inBlock==IN_EXAMPLE))
      {
        if (buf[0]=='{')
        {
          if(inBlock==IN_HEADER)
          {
            IDPROC(h)->data.s.body_start = pos;
            IDPROC(h)->data.s.body_lineno = lines-offset;
            // Print("%s: %d-%d\n", pi->procname, lines, offset);
          }
          inBlock=IN_BODY;
        }
      }
      else if ((inBlock==IN_BODY) || (inBlock==IN_EXAMPLE_BODY))
      {
        if (buf[0]=='}')
        {
          if(IDPROC(h)->data.s.example_start==0)
            IDPROC(h)->data.s.example_start=pos;
          if(IDPROC(h)->data.s.body_end==0) IDPROC(h)->data.s.body_end=pos;
          IDPROC(h)->data.s.proc_end = pos;
          inBlock=0;
        }
      }
    }
    lines++;
    pos = ftell(fp);
  } while (fgets( buf, sizeof(buf), fp));
  fclose( fp );

  //if (h!=NULL) IDPROC(h) = pi;
  if (BVERBOSE(V_DEBUG_LIB))
  {
    if (inBlock!=0)
      Warn("LIB `%s` ends within block",newlib);
    if (otherLines!=0)
      Warn("%d lines not recognised in LIB `%s`",otherLines,newlib);
    if(v==-1)
      Warn("LIB `%s` has no version flag",newlib);
  }
#endif /* HAVE_LIBPARSER */
  FreeL((ADDRESS)newlib);
  return FALSE;
}

procinfo *iiInitSingularProcinfo(procinfov pi, char *libname, char *procname,
                                 int line, long pos, BOOLEAN pstatic)
{
  pi->libname = mstrdup(libname);

  if( strcmp(procname,"_init")==0)
  {
    char *p = iiConvName(libname);
    pi->procname = mstrdup(p);
    FreeL((ADDRESS)p);
  } else pi->procname = mstrdup(procname);
  pi->language = LANG_SINGULAR;
  pi->ref = 1;
  pi->is_static = pstatic;
  pi->data.s.proc_start = pos;
  pi->data.s.help_start = 0L;
  pi->data.s.body_start = 0L;
  pi->data.s.body_end   = 0L;
  pi->data.s.example_start = 0L;
  pi->data.s.proc_lineno = line;
  pi->data.s.body_lineno = 0;
  pi->data.s.example_lineno = 0;
  pi->data.s.body = NULL;
  return(pi);
}

/*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*/
char *iiConvName(char *libname)
{
  char *p = (char *)AllocL(strlen(libname)+7);
  char *q = mstrdup(libname);
  char *r = q;
  for(; *r!='\0'; r++)
  {
    if(*r=='.') *r='_';
    if(*r==':') *r='_';
  }
  sprintf(p, "%s_init\0", q);
  FreeL((ADDRESS)q);
  return(p);
}

/*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*/
int piShowProcList()
{
  idhdl h;
  procinfo *proc;
  char *name;

  Print( "%-15s  %20s      %s,%s  %s,%s   %s,%s\n", "Library", "function",
         "line", "start", "line", "body", "line", "example");
  for(h = idroot; h != NULL; h = IDNEXT(h))
  {
    if(IDTYP(h) == PROC_CMD)
    {
      proc = IDPROC(h);
      if(strcmp(proc->procname, IDID(h))!=0)
      {
        name = (char *)AllocL(strlen(IDID(h))+strlen(proc->procname)+4);
        sprintf(name, "%s -> %s", IDID(h), proc->procname);
        Print( "%-15s  %20s ", proc->libname, name);
        FreeL(name);
      } else Print( "%-15s  %20s ", proc->libname, proc->procname);
      if(proc->language==LANG_SINGULAR)
        Print("line %4d,%-5ld  %4d,%-5ld  %4d,%-5ld\n",
              proc->data.s.proc_lineno, proc->data.s.proc_start,
              proc->data.s.body_lineno, proc->data.s.body_start,
              proc->data.s.example_lineno, proc->data.s.example_start);
      else if(proc->language==LANG_C) Print("type: object\n");

    }
  }
}

/*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*/
char *iiLineNo(char *procname, int lineno)
{
  char buf[256];
  idhdl pn = ggetid(procname);
  procinfo *pi = IDPROC(pn);

  sprintf(buf, "%s %3d\0", procname, lineno);
  //sprintf(buf, "%s::%s %3d\0", pi->libname, pi->procname,
  //  lineno + pi->data.s.body_lineno);
  return(buf);
}
/*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*/
#ifdef HAVE_LIBPARSER
void libstack::push(char *p, char *libname)
{
  idhdl hl = idroot->get("LIB",0);
  libstackv lp;
  char *f = NULL;
  if(hl!=NULL) f = strstr(IDSTRING(hl),libname);
  if( (hl==NULL) || (f == NULL)) {
    for(lp = this;lp!=NULL;lp=lp->next) {
      if(strcmp(lp->get(), libname)==0) break;
    }
    if(lp==NULL) {
      libstackv ls = (libstack *)Alloc0(sizeof(libstack));
      ls->next = this;
      ls->libname = mstrdup(libname);
      ls->to_be_done = TRUE;
      if(this != NULL) ls->cnt = this->cnt+1; else ls->cnt = 0;
      library_stack = ls;
    }
  }
}

libstackv libstack::pop(char *p)
{
  libstackv ls = this;
  //FreeL(ls->libname);
  library_stack = ls->next;
  Free((ADDRESS)ls, sizeof(libstack));
  return(library_stack);
}

#endif /* HAVE_LIBPARSER */
/*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*/
