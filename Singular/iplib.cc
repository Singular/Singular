/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/* $Id: iplib.cc,v 1.43 1998-11-19 16:19:54 krueger Exp $ */
/*
* ABSTRACT: interpreter: LIB and help
*/

//#include <stdlib.h>
#include <stdio.h>
#include <string.h>
//#include <ctype.h>
#include <sys/stat.h>

#include "mod2.h"
#include "tok.h"
#include "ipid.h"
#include "mmemory.h"
#include "febase.h"
#include "ring.h"
#include "subexpr.h"
#include "ipshell.h"
#include "lists.h"

#ifdef HAVE_LIBPARSER
#  include "libparse.h"
#else /* HAVE_LIBPARSER */
   procinfo *iiInitSingularProcinfo(procinfov pi, char *libname,
                                    char *procname, int line, long pos,
                                    BOOLEAN pstatic = FALSE);
#endif /* HAVE_LIBPARSER */
#define NS_LRING namespaceroot->next->currRing

#ifdef HAVE_DYNAMIC_LOADING
void *dynl_open(char *filename);
void *dynl_sym(void *handle, char *symbol);
int dynl_close (void *handle);
char *dynl_error();
#endif /* HAVE_DYNAMIC_LOADING */

char *iiConvName(char *p);
#ifdef HAVE_LIBPARSER
void yylprestart (FILE *input_file );
int current_pos(int i=0);
extern int yylp_errno;
extern int yylplineno;
extern char *yylp_errlist[];
void print_init();
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
  while ((*e==' ') || (*e=='\t') || (*e=='(')) e++;
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
  char *argstr=(char *)AllocL(124);
  int argstrlen=124;
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
      // check for space:
      if ((int)strlen(argstr)+12 /* parameter + ;*/ +(int)strlen(s)>= argstrlen)
      {
        argstrlen*=2;
        char *a=(char *)AllocL( argstrlen);
        strcpy(a,argstr);
        FreeL((ADDRESS)argstr);
        argstr=a;
      }
      // copy the result to argstr
      strcat(argstr,"parameter ");
      strcat(argstr,s);
      strcat(argstr,"; ");
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
    int i, offset=0;
    long head = pi->data.s.def_end - pi->data.s.proc_start;
    procbuflen = pi->data.s.help_end - pi->data.s.help_start;
    //Print("Help=%ld-%ld=%d\n", pi->data.s.body_start,
    //    pi->data.s.proc_start, procbuflen);
    s = (char *)AllocL(procbuflen+head+3);
    myfread(s, head, 1, fp);
    s[head] = '\n';
    fseek(fp, pi->data.s.help_start, SEEK_SET);
    myfread(s+head+1, procbuflen, 1, fp);
    s[procbuflen+head+1] = '\n';
    s[procbuflen+head+2] = '\0';
    offset=0;
    for(i=0;i<=procbuflen+head+2; i++) {
      if(s[i]=='\\' &&
         (s[i+1]=='"' || s[i+1]=='{' || s[i+1]=='}' || s[i+1]=='\\')) {
        i++;
        offset++;
      }
      if(offset>0) s[i-offset] = s[i];
    }
    return(s);
  }
  if(part==1)
  { // load proc part
    procbuflen = pi->data.s.def_end - pi->data.s.proc_start;
    //fgets(buf, sizeof(buf), fp);
    myfread( buf, procbuflen, 1, fp);
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
    myfread( pi->data.s.body+strlen(argstr), procbuflen, 1, fp);
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
    myfread(s, procbuflen, 1, fp);
    s[procbuflen] = '\0';
    strcat(s+procbuflen-3, "\n;return();\n\n" );
    p=strchr(s,'{');
    if (p!=NULL) *p=' ';
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
  myynest++;
  //Print("%-*.*s PStart <<%s>>, level:%d\n",2*myynest,2*myynest," ",IDID(pn),myynest);
  err=yyparse();
  killlocals(myynest);
  myynest--;
  si_echo=old_echo;
  //Print("%-*.*s PEnd  <<%s>>, level:%d\n",2*myynest,2*myynest," ",IDID(pn),myynest);

  return err;
}

#ifdef USE_IILOCALRING
ring    *iiLocalRing
#ifdef TEST
                    =NULL
#endif
                   ;
#endif
sleftv  *iiRETURNEXPR
#ifdef TEST
                    =NULL
#endif
                   ;
int     iiRETURNEXPR_len=0;

#ifdef RDEBUG
static void iiShowLevRings()
{
#ifdef USE_IILOCALRING
  int i;
  for (i=1;i<=myynest;i++)
  {
    Print("lev %d:",i);
    if (iiLocalRing[i]==NULL) PrintS("NULL");
    else                      Print("%d",iiLocalRing[i]);
    PrintLn();
  }
#endif
  {
    namehdl nshdl;
    for(nshdl=namespaceroot; nshdl->isroot != TRUE; nshdl = nshdl->next) {
      Print("%d lev %d:",nshdl->lev, nshdl->myynest);
      if (nshdl->currRing==NULL) PrintS("NULL");
      else                       Print("%d",nshdl->currRing);
      PrintLn();
    }
    Print("%d lev %d:",nshdl->lev, nshdl->myynest);
    if (nshdl->currRing==NULL) PrintS("NULL");
    else                       Print("%d",nshdl->currRing);
    PrintLn();
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
#ifdef USE_IILOCALRING
    iiLocalRing=(ring *)ReAlloc(iiLocalRing,
                                   iiRETURNEXPR_len*sizeof(ring),
                                   (iiRETURNEXPR_len+16)*sizeof(ring));
#endif
    iiRETURNEXPR_len+=16;
  }
}
#ifdef HAVE_NAMESPACES
sleftv * iiMake_proc(idhdl pn, sleftv* slpn, sleftv* sl)
#else /* HAVE_NAMESPACES */
sleftv * iiMake_proc(idhdl pn, sleftv* sl)
#endif /* HAVE_NAMESPACES */
{
  int err;
  procinfov pi = IDPROC(pn);
  char *plib = iiConvName(pi->libname);
#ifdef HAVE_NAMESPACES
//  printf("iiMake_proc: %s %s cur=%s\n", pi->libname, plib, namespaceroot->name);
  idhdl ns = namespaceroot->get(plib,0, TRUE);
  if((ns==NULL) && (slpn!=NULL) && (slpn->packhdl != NULL)) ns=slpn->packhdl;
  if(pi->is_static)
  {
    if(ns==NULL)
    {
      Werror("'%s::%s()' 1 is a local procedure and cannot be accessed by an user.",
             plib, pi->procname);
      FreeL(plib);
      return NULL;
    }
    if(strcmp(plib, namespaceroot->name)!= 0)
    {
      Werror("'%s::%s()' 2 is a local procedure and cannot be accessed by an user.",
             plib, pi->procname);
      FreeL(plib);
      return NULL;
    }
  }
  FreeL((ADDRESS)plib);
  if(ns != NULL)
  {
    namespaceroot->push(IDPACKAGE(ns), IDID(ns), myynest+1);
    //printf("iiMake_proc: namespace found.\n");
  }
  else
  {
    namespaceroot->push(namespaceroot->root->pack, "Top", myynest+1);
    //printf("iiMake_proc: staying in TOP-LEVEL\n");
  }
#else /* HAVE_NAMESPACES */
  if(pi->is_static && myynest==0)
  {
    Werror("'%s::%s()' is a local procedure and cannot be accessed by an user.",
           pi->libname, pi->procname);
    FreeL((ADDRESS)plib);
    return NULL;
  }
  namespaceroot->push(NULL, plib, myynest+1);
  FreeL((ADDRESS)plib);
#endif /* HAVE_NAMESPACES */
  iiCheckNest();
#ifdef USE_IILOCALRING
  iiLocalRing[myynest]=currRing;
#endif
  iiRETURNEXPR[myynest+1].Init();
  if (traceit&TRACE_SHOW_PROC)
  {
    if (traceit&TRACE_SHOW_LINENO) PrintLn();
    Print("entering%-*.*s %s (level %d)\n",myynest*2,myynest*2," ",IDID(pn),myynest);
  }
#ifdef RDEBUG
  if (traceit&TRACE_SHOW_RINGS) iiShowLevRings();
#endif
  switch (pi->language)
  {
    case LANG_NONE:
                 err=TRUE;
                 break;

    case LANG_SINGULAR:
                 err=iiPStart(pn,sl);
                 break;
    case LANG_C:
                 leftv res = (leftv)Alloc0(sizeof(sleftv));
                 err = (pi->data.o.function)(res, sl);
                 iiRETURNEXPR[myynest+1].Copy(res);
                 Free((ADDRESS)res, sizeof(sleftv));
                 break;
  }
  if (traceit&TRACE_SHOW_PROC)
  {
    if (traceit&TRACE_SHOW_LINENO) PrintLn();
    Print("leaving %-*.*s %s (level %d)\n",myynest*2,myynest*2," ",IDID(pn),myynest);
  }
#ifdef RDEBUG
  if (traceit&TRACE_SHOW_RINGS) iiShowLevRings();
#endif
  if (err)
  {
    iiRETURNEXPR[myynest+1].CleanUp();
    iiRETURNEXPR[myynest+1].Init();
  }
#ifdef USE_IILOCALRING
  if(namespaceroot->next->currRing != iiLocalRing[myynest]) printf("iiMake_proc: 1 ring not saved\n");
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
      if (iiLocalRing[myynest]!=NULL) o=rFindHdl(iiLocalRing[myynest],NULL, NULL)->id;
      else                            o="none";
      if (currRing!=NULL)             n=rFindHdl(currRing,NULL, NULL)->id;
      else                            n="none";
      Werror("ring change during procedure call: %s -> %s",o,n);
      iiRETURNEXPR[myynest+1].CleanUp();
      err=TRUE;
    }
    if (iiLocalRing[myynest]!=NULL)
    {
      rSetHdl(rFindHdl(iiLocalRing[myynest],NULL, NULL),TRUE);
      iiLocalRing[myynest]=NULL;
      namespaceroot->next->currRing = NULL;
    }
    else
    { currRingHdl=NULL; currRing=NULL; }
  }
#else /* USE_IILOCALRING */
  if (NS_LRING != currRing)
  {
    if (((iiRETURNEXPR[myynest+1].Typ()>BEGIN_RING)
      && (iiRETURNEXPR[myynest+1].Typ()<END_RING))
    || ((iiRETURNEXPR[myynest+1].Typ()==LIST_CMD)
      && (lRingDependend((lists)iiRETURNEXPR[myynest+1].Data()))))
    {
      char *n;
      char *o;
      if (NS_LRING!=NULL) o=rFindHdl(NS_LRING,NULL, NULL)->id;
      else                            o="none";
      if (currRing!=NULL)             n=rFindHdl(currRing,NULL, NULL)->id;
      else                            n="none";
      Werror("ring change during procedure call: %s -> %s",o,n);
      iiRETURNEXPR[myynest+1].CleanUp();
      err=TRUE;
    }
    if (NS_LRING!=NULL)
    {
      rSetHdl(rFindHdl(NS_LRING,NULL, NULL),TRUE);
      NS_LRING=NULL;
    }
    else
    { currRingHdl=NULL; currRing=NULL; }
  }
#endif /* USE_IILOCALRING */
  if (iiCurrArgs!=NULL)
  {
    if (!err) Warn("too many arguments for %s",IDID(pn));
    iiCurrArgs->CleanUp();
    Free((ADDRESS)iiCurrArgs,sizeof(sleftv));
    iiCurrArgs=NULL;
  }
  namespaceroot->pop(TRUE);
  if (err)
    return NULL;
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
#ifdef HAVE_NAMESPACES
  char *plib = iiConvName(pi->libname);
  idhdl ns = namespaceroot->get(plib,0, TRUE);
  FreeL((ADDRESS)plib);
#endif /* HAVE_NAMESPACES */

  newBuffer( example, BT_example, pi, pi->data.s.example_lineno );
  iiCheckNest();
#ifdef HAVE_NAMESPACES
  if(ns != NULL)  namespaceroot->push(IDPACKAGE(ns), IDID(ns), myynest+1);
  else            namespaceroot->push(namespaceroot->root->pack, "Top", myynest+1);
#else /* HAVE_NAMESPACES */
  namespaceroot->push(NULL, "", myynest+1);
#endif /* HAVE_NAMESPACES */
#ifdef USE_IILOCALRING
  iiLocalRing[myynest]=currRing;
#endif
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
#ifdef USE_IILOCALRING
  if (iiLocalRing[myynest] != currRing)
  {
    if (iiLocalRing[myynest]!=NULL)
    {
      rSetHdl(rFindHdl(iiLocalRing[myynest],NULL, NULL),TRUE);
      iiLocalRing[myynest]=NULL;
    }
    else
    {
      currRingHdl=NULL;
      currRing=NULL;
    }
  }
#else /* USE_IILOCALRING */
  if (NS_LRING != currRing)
  {
    if (NS_LRING!=NULL)
    {
      rSetHdl(rFindHdl(NS_LRING,NULL, NULL),TRUE);
      NS_LRING=NULL;
    }
    else
    {
      currRingHdl=NULL;
      currRing=NULL;
    }
  }
#endif /* USE_IILOCALRING */
  namespaceroot->pop(TRUE);
  return err;
}

/*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*/
#ifdef HAVE_NAMESPACES
BOOLEAN iiLibCmd( char *newlib, BOOLEAN autoexport, BOOLEAN tellerror )
#else /* HAVE_NAMESPACES */
BOOLEAN iiLibCmd( char *newlib, BOOLEAN tellerror )
#endif /* HAVE_NAMESPACES */
{
  char buf[256];
  char libnamebuf[128];
  idhdl h,hl;
#ifdef HAVE_NAMESPACES
  idhdl pl;
#endif /* HAVE_NAMESPACES */
  int lines = 1;
  long pos = 0L;
  procinfov pi;
#ifdef HAVE_NAMESPACES
  char *plib = iiConvName(newlib);
#endif /* HAVE_NAMESPACES */
  FILE * fp = feFopen( newlib, "r", libnamebuf, tellerror );
  if (fp==NULL)
  {
    return TRUE;
  }
#ifdef HAVE_NAMESPACES
  int token;

  if(IsCmd(plib, &token))
  {
    Werror("'%s' is resered identifier\n", plib);
    return TRUE;
  }
  hl = namespaceroot->get("LIB",0, TRUE);
#else /* HAVE_NAMESPACES */
  hl = idroot->get("LIB",0);
#endif /* HAVE_NAMESPACES */
  if (hl==NULL)
  {
#ifdef HAVE_NAMESPACES
    hl = enterid( mstrdup("LIB"),0, STRING_CMD,
                  &NSROOT(namespaceroot->root), FALSE );
#else /* HAVE_NAMESPACES */
    hl = enterid( mstrdup("LIB"),0, STRING_CMD, &idroot, FALSE );
#endif /* HAVE_NAMESPACES */
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
        FreeL((ADDRESS)s);
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
#ifdef HAVE_TCL
  if (tclmode)
  {
    PrintTCLS('L',newlib);
  }
#endif
#ifdef HAVE_NAMESPACES
  pl = namespaceroot->get(plib,0, TRUE);
  if (pl==NULL)
  {
    pl = enterid( mstrdup(plib),0, PACKAGE_CMD,
                  &NSROOT(namespaceroot->root), TRUE );
    IDPACKAGE(pl)->language = LANG_SINGULAR;
    IDPACKAGE(pl)->libname=mstrdup(newlib);
  }
  else
  {
    if(IDTYP(pl)!=PACKAGE_CMD)
    {
      Warn("not of typ package.");
      return TRUE;
    }
  }
  namespaceroot->push(IDPACKAGE(pl), IDID(pl));
#endif /* HAVE_NAMESPACES */

#ifdef HAVE_LIBPARSER
  extern FILE *yylpin;
  libstackv ls_start = library_stack;
  lib_style_types lib_style;

  yylpin = fp;
# if YYLPDEBUG > 1
  print_init();
#  endif
  extern int lpverbose;
  if (BVERBOSE(V_DEBUG_LIB)) lpverbose=1; else lpverbose=0;
# ifdef HAVE_NAMESPACES
   yylplex(newlib, libnamebuf, &lib_style, pl, autoexport);
# else /* HAVE_NAMESPACES */
  yylplex(newlib, libnamebuf, &lib_style);
# endif /* HAVE_NAMESPACES */
  if(yylp_errno)
  {
    Werror("Library %s: ERROR occured: in line %d, %d.", newlib, yylplineno,
         current_pos(0));
    if(yylp_errno==YYLP_BAD_CHAR)
    {
      Werror(yylp_errlist[yylp_errno], *text_buffer, yylplineno);
      FreeL((ADDRESS)text_buffer);
    }
    else
      Werror(yylp_errlist[yylp_errno], yylplineno);
    Werror("Cannot load library,... aborting.");
    reinit_yylp();
    fclose( yylpin );
    FreeL((ADDRESS)newlib);
# ifdef HAVE_NAMESPACES
    FreeL((ADDRESS)plib);
# endif /* HAVE_NAMESPACES */
    return TRUE;
  }
  if (BVERBOSE(V_LOAD_LIB)) Print( "// ** loaded %s %s\n", libnamebuf,
                                   text_buffer);
  if( (lib_style == OLD_LIBSTYLE) && (BVERBOSE(V_LOAD_LIB)))
  {
    Warn( "library %s has old format. This format is still accepted,", newlib);
    Warn( "but for functionality you may wish to change to the new");
    Warn( "format. Please refer to the manual for further information.");
  }
  reinit_yylp();
  fclose( yylpin );
#ifdef HAVE_NAMESPACES
  namespaceroot->pop();
#endif /* HAVE_NAMESPACES */
  {
    libstackv ls;
    for(ls = library_stack; (ls != NULL) && (ls != ls_start); )
    {
      if(ls->to_be_done)
      {
        //Print("// Processing id %d LIB:%s\n", ls->cnt, ls->get());
        ls->to_be_done=FALSE;
#ifdef HAVE_NAMESPACES
        iiLibCmd(ls->get(), autoexport);
#else /* HAVE_NAMESPACES */
        iiLibCmd(ls->get());
#endif /* HAVE_NAMESPACES */
        ls = ls->pop(newlib);
        //Print("Done\n");
      }
    }
#if 0
    PrintS("--------------------\n");
    for(ls = library_stack; ls != NULL; ls = ls->next)
    {
      Print("%s: LIB-stack:(%d), %s %s\n", newlib, ls->cnt, ls->get(),
        ls->to_be_done ? "not loaded" : "loaded");
    }
    PrintS("--------------------\n");
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
#ifdef HAVE_NAMESPACES
             h = enterid( mstrdup(p), myynest, PROC_CMD, IDPACKAGE(pl), FALSE );
#else /* HAVE_NAMESPACES */
             h = enterid( mstrdup(p), myynest, PROC_CMD, &idroot, FALSE );
#endif /* HAVE_NAMESPACES */
            FreeL((ADDRESS)p);
          }
          else
#endif
#ifdef HAVE_NAMESPACES
            h = enterid(mstrdup(proc), myynest, PROC_CMD,
                      &IDPACKAGE(pl)->idroot, FALSE);
#else /* HAVE_NAMESPACES */
            h = enterid( mstrdup(proc), myynest, PROC_CMD, &idroot, FALSE);
#endif /* HAVE_NAMESPACES */
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
#ifdef HAVE_NAMESPACES
  namespaceroot->pop();
#endif /* HAVE_NAMESPACES */

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
#ifdef HAVE_NAMESPACES
   FreeL((ADDRESS)plib);
#endif /* HAVE_LIBPARSER */
  return FALSE;
}

/*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*/
procinfo *iiInitSingularProcinfo(procinfov pi, char *libname, char *procname,
                                 int line, long pos, BOOLEAN pstatic)
{
  pi->libname = mstrdup(libname);

  if( strcmp(procname,"_init")==0)
  {
    pi->procname = iiConvName(libname);
  }
  else
    pi->procname = mstrdup(procname);
  pi->language = LANG_SINGULAR;
  pi->ref = 1;
  pi->is_static = pstatic;
  pi->data.s.proc_start = pos;
  pi->data.s.def_end    = 0L;
  pi->data.s.help_start = 0L;
  pi->data.s.help_end   = 0L;
  pi->data.s.body_start = 0L;
  pi->data.s.body_end   = 0L;
  pi->data.s.example_start = 0L;
  pi->data.s.proc_lineno = line;
  pi->data.s.body_lineno = 0;
  pi->data.s.example_lineno = 0;
  pi->data.s.body = NULL;
  return(pi);
}

#ifdef HAVE_DYNAMIC_LOADING
/*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*/
int iiAddCproc(char *libname, char *procname, BOOLEAN pstatic,
               BOOLEAN(*func)(leftv res, leftv v))
{
  procinfov pi;
  idhdl h;

  h = enterid(mstrdup(procname),0, PROC_CMD, &IDROOT, TRUE);
  if ( h!= NULL )
  {
    pi = IDPROC(h);
    pi->libname = mstrdup(libname);
    pi->procname = mstrdup(procname);
    pi->language = LANG_C;
    pi->ref = 1;
    pi->is_static = pstatic;
    pi->data.o.function = func;
    return(1);
  }
  else
  {
    PrintS("iiAddCproc: failed.\n");
  }
  return(0);
}

/*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*/
BOOLEAN load_modules(char *newlib, char *fullname, BOOLEAN tellerror)
{
  int iiAddCproc(char *libname, char *procname, BOOLEAN pstatic,
                 BOOLEAN(*func)(leftv res, leftv v));
  int (*fktn)(int(*iiAddCproc)(char *libname, char *procname,
                               BOOLEAN pstatic,
                               BOOLEAN(*func)(leftv res, leftv v)));
  idhdl pl;
  char *plib = iiConvName(newlib);
  BOOLEAN RET=TRUE;
  int token;
  char FullName[256];
  
  if( *fullname != '/' &&  *fullname != '.' )
    sprintf(FullName, "./%s", newlib);
  else strcpy(FullName, fullname);
  

  if(IsCmd(plib, &token))
  {
    Werror("'%s' is resered identifier\n", plib);
    goto load_modules_end;
  }

  pl = namespaceroot->get(plib,0, TRUE);
  if (pl==NULL)
  {
    pl = enterid( mstrdup(plib),0, PACKAGE_CMD,
                  &NSROOT(namespaceroot->root), TRUE );
    IDPACKAGE(pl)->language = LANG_C;
    IDPACKAGE(pl)->libname=mstrdup(newlib);
  }
  else
  {
    if(IDTYP(pl)!=PACKAGE_CMD)
    {
      Warn("not of typ package.");
      goto load_modules_end; 
    }
  }
  namespaceroot->push(IDPACKAGE(pl), IDID(pl));

  if((IDPACKAGE(pl)->handle=dynl_open(FullName))==(void *)NULL)
  {
    WerrorS("dynl_open failed");
    Werror("%s not found", newlib);
    goto load_modules_end; 
  }
  else
  {
    fktn = dynl_sym(IDPACKAGE(pl)->handle, "mod_init");
    if( fktn!= NULL) (*fktn)(iiAddCproc);
    else Werror("mod_init: %s\n", dynl_error());
    if (BVERBOSE(V_LOAD_LIB)) Print( "// ** loaded %s \n", fullname);
  }
  RET=FALSE;

  load_modules_end:
  namespaceroot->pop();
  return RET;
  
}
#endif /* HAVE_DYNAMIC_LOADING */

/*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*/
char mytoupper(char c)
{
  if(c>=97 && c<=(97+26)) c-=32;
  return(c);
}

/*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*/
#if defined(WINNT)
#  define  FS_SEP '\\'
#elif defined(macintosh)
#  define FS_SEP ','
#else
#  define FS_SEP '/'
#endif

char *iiConvName(char *libname)
{
  char *tmpname = mstrdup(libname);
  char *p = strrchr(tmpname, FS_SEP);
  char *r;
  if(p==NULL) p = tmpname;
  else p++;
  r = strchr(p, '.');
  if( r!= NULL) *r = '\0';
  r = mstrdup(p);
  *r = mytoupper(*r);
  // printf("iiConvName: '%s' '%s' => '%s'\n", libname, tmpname, r);
  FreeL((ADDRESS)tmpname);

  return(r);
}

/*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*/
void piShowProcList()
{
  idhdl h;
#ifdef HAVE_NAMESPACES
  idhdl pl;
#endif /* HAVE_NAMESPACES */
  procinfo *proc;
  char *name;

  Print( "%-15s  %20s      %s,%s  %s,%s   %s,%s\n", "Library", "function",
         "line", "start", "line", "body", "line", "example");
#ifdef HAVE_NAMESPACES
//  for(pl = IDROOT; pl != NULL; pl = IDNEXT(pl)) {
    for(pl = NSROOT(namespaceroot->root); pl != NULL; pl = IDNEXT(pl)) {
    if(IDTYP(pl) == PACKAGE_CMD) {
      for(h = IDPACKAGE(pl)->idroot; h != NULL; h = IDNEXT(h))
#else /* HAVE_NAMESPACES */
  for(h = IDROOT; h != NULL; h = IDNEXT(h))
#endif /* HAVE_NAMESPACES */
  {
    if(IDTYP(h) == PROC_CMD)
    {
      proc = IDPROC(h);
      if(strcmp(proc->procname, IDID(h))!=0)
      {
        name = (char *)AllocL(strlen(IDID(h))+strlen(proc->procname)+4);
        sprintf(name, "%s -> %s", IDID(h), proc->procname);
        Print( "%d %-15s  %20s ", proc->is_static ? 1 : 0, proc->libname, name);
        FreeL((ADDRESS)name);
      }
      else
        Print( "%d %-15s  %20s ", proc->is_static ? 1 : 0, proc->libname,
               proc->procname);
      if(proc->language==LANG_SINGULAR)
        Print("line %4d,%-5ld  %4d,%-5ld  %4d,%-5ld\n",
              proc->data.s.proc_lineno, proc->data.s.proc_start,
              proc->data.s.body_lineno, proc->data.s.body_start,
              proc->data.s.example_lineno, proc->data.s.example_start);
      else if(proc->language==LANG_C)
        Print("type: object\n");
#ifdef HAVE_NAMESPACES
          }
      }
#endif /* HAVE_NAMESPACES */
    }
  }
}

/*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*/
//char *iiLineNo(char *procname, int lineno)
//{
//  char buf[256];
//  idhdl pn = ggetid(procname);
//  procinfo *pi = IDPROC(pn);
//
//  sprintf(buf, "%s %3d\0", procname, lineno);
//  //sprintf(buf, "%s::%s %3d\0", pi->libname, pi->procname,
//  //  lineno + pi->data.s.body_lineno);
//  return(buf);
//}
/*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*/
#ifdef HAVE_LIBPARSER
void libstack::push(char *p, char *libname)
{
#  ifdef HAVE_NAMESPACES
  idhdl hl = namespaceroot->get("LIB",0, TRUE);
#  else /* HAVE_NAMESPACES */
  idhdl hl = idroot->get("LIB",0);
#  endif /* HAVE_NAMESPACES */
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
  //FreeL((ADDRESS)ls->libname);
  library_stack = ls->next;
  Free((ADDRESS)ls, sizeof(libstack));
  return(library_stack);
}

#endif /* HAVE_LIBPARSER */
/*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*/
#ifndef HOWMANY
# define HOWMANY 8192           /* how much of the file to look at */
#endif

lib_types type_of_LIB(char *newlib, char *libnamebuf)
{
  unsigned char	buf[HOWMANY+1];	/* one extra for terminating '\0' */
  struct stat sb;
  int nbytes = 0;
  int ret;
  lib_types LT=LT_NONE;

  FILE * fp = feFopen( newlib, "r", libnamebuf, FALSE );
  ret = stat(libnamebuf, &sb);
  
  if (fp==NULL)
  {
    return LT;
  }
  if((sb.st_mode & S_IFMT) != S_IFREG) {
    goto lib_type_end;
  }
  if ((nbytes = fread((char *)buf, sizeof(char), HOWMANY, fp)) == -1) {
    goto lib_type_end;
    /*NOTREACHED*/
  }
  if (nbytes == 0)
    goto lib_type_end;
  else {
    buf[nbytes++] = '\0';	/* null-terminate it */
  }
  if( (strncmp(buf, "\177ELF\01\01\01", 7)==0) && buf[16]=='\03') {
    LT = LT_ELF;
    FreeL(newlib);
    newlib = mstrdup(libnamebuf);
    goto lib_type_end;
  }
  if(isprint(buf[0])) { LT = LT_SINGULAR; goto lib_type_end; }
  
  lib_type_end:
  close(fp);
  return LT;
}
/*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*/
