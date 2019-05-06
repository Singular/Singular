/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/*
* ABSTRACT: interpreter: LIB and help
*/

#include "kernel/mod2.h"

#include "Singular/tok.h"
#include "misc/options.h"
#include "Singular/ipid.h"
#include "polys/monomials/ring.h"
#include "Singular/subexpr.h"
#include "Singular/ipid.h"
#include "Singular/ipshell.h"
#include "Singular/fevoices.h"
#include "Singular/lists.h"

#include <ctype.h>

#if SIZEOF_LONG == 8
#define SI_MAX_NEST 500
#elif defined(__CYGWIN__)
#define SI_MAX_NEST 480
#else
#define SI_MAX_NEST 1000
#endif

#if defined(ix86Mac_darwin) || defined(x86_64Mac_darwin) || defined(ppcMac_darwin)
#  define MODULE_SUFFIX bundle
#elif defined(__CYGWIN__)
#  define MODULE_SUFFIX dll
#else
#  define MODULE_SUFFIX so
#endif

#define MODULE_SUFFIX_STRING EXPANDED_STRINGIFY(MODULE_SUFFIX)


#ifdef HAVE_DYNAMIC_LOADING
BOOLEAN load_modules(const char *newlib, char *fullname, BOOLEAN autoexport);
#endif

#ifdef HAVE_LIBPARSER
#  include "libparse.h"
#else /* HAVE_LIBPARSER */
procinfo *iiInitSingularProcinfo(procinfov pi, const char *libname,
              const char *procname, int line, long pos, BOOLEAN pstatic=FALSE);
#endif /* HAVE_LIBPARSER */

extern int iiArithAddCmd(const char *szName, short nAlias, short nTokval,
                         short nToktype, short nPos);

#include "Singular/mod_lib.h"

#ifdef HAVE_LIBPARSER
void yylprestart (FILE *input_file );
int current_pos(int i=0);
EXTERN_VAR int yylp_errno;
EXTERN_VAR int yylplineno;
extern const char *yylp_errlist[];
void print_init();
VAR libstackv library_stack;
#endif

//int IsCmd(char *n, int tok);
char mytolower(char c);

/*2
* return TRUE if the libray libname is already loaded
*/
BOOLEAN iiGetLibStatus(const char *lib)
{
  idhdl hl;

  char *plib = iiConvName(lib);
  hl = basePack->idroot->get(plib,0);
  omFree(plib);
  if((hl==NULL) ||(IDTYP(hl)!=PACKAGE_CMD))
  {
    return FALSE;
  }
  if ((IDPACKAGE(hl)->language!=LANG_C)&&(IDPACKAGE(hl)->libname!=NULL))
    return (strcmp(lib,IDPACKAGE(hl)->libname)==0);
  return FALSE;
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
      return omStrDup("parameter list #;");
    }
    else
    {
      // empty list
      return omStrDup("");
    }
  }
  BOOLEAN in_args;
  BOOLEAN args_found;
  char *s;
  char *argstr=(char *)omAlloc(127); // see ../omalloc/omTables.inc
  int argstrlen=127;
  *argstr='\0';
  int par=0;
  do
  {
    args_found=FALSE;
    s=e; // set s to the starting point of the arg
         // and search for the end
    // skip leading spaces:
    loop
    {
      if ((*s==' ')||(*s=='\t'))
        s++;
      else if ((*s=='\n')&&(*(s+1)==' '))
        s+=2;
      else // start of new arg or \0 or )
        break;
    }
    e=s;
    while ((*e!=',')
    &&((par!=0) || (*e!=')'))
    &&(*e!='\0'))
    {
      if (*e=='(') par++;
      else if (*e==')') par--;
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
        char *a=(char *)omAlloc( argstrlen);
        strcpy(a,argstr);
        omFree((ADDRESS)argstr);
        argstr=a;
      }
      // copy the result to argstr
      if(strncmp(s,"alias ",6)!=0)
      {
        strcat(argstr,"parameter ");
      }
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
    if (procbuflen<5)
    {
      fclose(fp);
      return NULL; // help part does not exist
    }
    //Print("Help=%ld-%ld=%d\n", pi->data.s.body_start,
    //    pi->data.s.proc_start, procbuflen);
    s = (char *)omAlloc(procbuflen+head+3);
    myfread(s, head, 1, fp);
    s[head] = '\n';
    fseek(fp, pi->data.s.help_start, SEEK_SET);
    myfread(s+head+1, procbuflen, 1, fp);
    fclose(fp);
    s[procbuflen+head+1] = '\n';
    s[procbuflen+head+2] = '\0';
    offset=0;
    for(i=0;i<=procbuflen+head+2; i++)
    {
      if(s[i]=='\\' &&
         (s[i+1]=='"' || s[i+1]=='{' || s[i+1]=='}' || s[i+1]=='\\'))
      {
        i++;
        offset++;
      }
      if(offset>0) s[i-offset] = s[i];
    }
    return(s);
  }
  else if(part==1)
  { // load proc part - must exist
    procbuflen = pi->data.s.def_end - pi->data.s.proc_start;
    char *ss=(char *)omAlloc(procbuflen+2);
    //fgets(buf, sizeof(buf), fp);
    myfread( ss, procbuflen, 1, fp);
    char ct;
    char *e;
    s=iiProcName(ss,ct,e);
    char *argstr=NULL;
    *e=ct;
    argstr=iiProcArgs(e,TRUE);

    assume(pi->data.s.body_end > pi->data.s.body_start);

    procbuflen = pi->data.s.body_end - pi->data.s.body_start;
    pi->data.s.body = (char *)omAlloc( strlen(argstr)+procbuflen+15+
                                      strlen(pi->libname) );
    //Print("Body=%ld-%ld=%d\n", pi->data.s.body_end,
    //    pi->data.s.body_start, procbuflen);
    assume(pi->data.s.body != NULL);
    fseek(fp, pi->data.s.body_start, SEEK_SET);
    strcpy(pi->data.s.body,argstr);
    myfread( pi->data.s.body+strlen(argstr), procbuflen, 1, fp);
    fclose( fp );
    procbuflen+=strlen(argstr);
    omFree(argstr);
    omFree(ss);
    pi->data.s.body[procbuflen] = '\0';
    strcat( pi->data.s.body+procbuflen, "\n;return();\n\n" );
    strcat( pi->data.s.body+procbuflen+13,pi->libname);
    s=(char *)strchr(pi->data.s.body,'{');
    if (s!=NULL) *s=' ';
    return NULL;
  }
  else if(part==2)
  { // example
    if ( pi->data.s.example_lineno == 0)
      return NULL; // example part does not exist
    // load example
    fseek(fp, pi->data.s.example_start, SEEK_SET);
    /*char *dummy=*/ (void) fgets(buf, sizeof(buf), fp); // skip line with "example"
    procbuflen = pi->data.s.proc_end - pi->data.s.example_start - strlen(buf);
    //Print("Example=%ld-%ld=%d\n", pi->data.s.proc_end,
    //  pi->data.s.example_start, procbuflen);
    s = (char *)omAlloc(procbuflen+14);
    myfread(s, procbuflen, 1, fp);
    s[procbuflen] = '\0';
    strcat(s+procbuflen-3, "\n;return();\n\n" );
    p=(char *)strchr(s,'{');
    if (p!=NULL) *p=' ';
    return(s);
  }
  return NULL;
}

BOOLEAN iiAllStart(procinfov pi, char *p,feBufferTypes t, int l)
{
  // see below:
  BITSET save1=si_opt_1;
  BITSET save2=si_opt_2;
  newBuffer( omStrDup(p /*pi->data.s.body*/), t /*BT_proc*/,
               pi, l );
  BOOLEAN err=yyparse();
  if (sLastPrinted.rtyp!=0)
  {
    sLastPrinted.CleanUp();
  }
  // the access to optionStruct and verboseStruct do not work
  // on x86_64-Linux for pic-code
  if ((TEST_V_ALLWARN) &&
  (t==BT_proc) &&
  ((save1!=si_opt_1)||(save2!=si_opt_2)) &&
  (pi->libname!=NULL) && (pi->libname[0]!='\0'))
  {
    if ((pi->libname!=NULL) && (pi->libname[0]!='\0'))
      Warn("option changed in proc %s from %s",pi->procname,pi->libname);
    else
      Warn("option changed in proc %s",pi->procname);
    int i;
    for (i=0; optionStruct[i].setval!=0; i++)
    {
      if ((optionStruct[i].setval & si_opt_1)
      && (!(optionStruct[i].setval & save1)))
      {
          Print(" +%s",optionStruct[i].name);
      }
      if (!(optionStruct[i].setval & si_opt_1)
      && ((optionStruct[i].setval & save1)))
      {
          Print(" -%s",optionStruct[i].name);
      }
    }
    for (i=0; verboseStruct[i].setval!=0; i++)
    {
      if ((verboseStruct[i].setval & si_opt_2)
      && (!(verboseStruct[i].setval & save2)))
      {
          Print(" +%s",verboseStruct[i].name);
      }
      if (!(verboseStruct[i].setval & si_opt_2)
      && ((verboseStruct[i].setval & save2)))
      {
          Print(" -%s",verboseStruct[i].name);
      }
    }
    PrintLn();
  }
  return err;
}
/*2
* start a proc
* parameters are built as exprlist
* TODO:interrupt
* return FALSE on success, TRUE if an error occurs
*/
BOOLEAN iiPStart(idhdl pn, leftv v)
{
  procinfov pi=NULL;
  int old_echo=si_echo;
  BOOLEAN err=FALSE;
  char save_flags=0;

  /* init febase ======================================== */
  /* we do not enter this case if filename != NULL !! */
  if (pn!=NULL)
  {
    pi = IDPROC(pn);
    if(pi!=NULL)
    {
      save_flags=pi->trace_flag;
      if( pi->data.s.body==NULL )
      {
        iiGetLibProcBuffer(pi);
        if (pi->data.s.body==NULL) return TRUE;
      }
//      omUpdateInfo();
//      int m=om_Info.UsedBytes;
//      Print("proc %s, mem=%d\n",IDID(pn),m);
    }
  }
  else return TRUE;
  /* generate argument list ======================================*/
  //iiCurrArgs should be NULL here, as the assignment for the parameters
  // of the prevouis call are already done befor calling another routine
  if (v!=NULL)
  {
    iiCurrArgs=(leftv)omAllocBin(sleftv_bin);
    memcpy(iiCurrArgs,v,sizeof(sleftv)); // keeps track of v->next etc.
    memset(v,0,sizeof(sleftv));
  }
  else
  {
    iiCurrArgs=NULL;
  }
  iiCurrProc=pn;
  /* start interpreter ======================================*/
  myynest++;
  if (myynest > SI_MAX_NEST)
  {
    WerrorS("nesting too deep");
    err=TRUE;
  }
  else
  {
    err=iiAllStart(pi,pi->data.s.body,BT_proc,pi->data.s.body_lineno-(v!=NULL));

    if (iiLocalRing[myynest-1] != currRing)
    {
      if (iiRETURNEXPR.RingDependend())
      {
        //idhdl hn;
        const char *n;
        const char *o;
        idhdl nh=NULL, oh=NULL;
        if (iiLocalRing[myynest-1]!=NULL)
          oh=rFindHdl(iiLocalRing[myynest-1],NULL);
        if (oh!=NULL)          o=oh->id;
        else                   o="none";
        if (currRing!=NULL)
          nh=rFindHdl(currRing,NULL);
        if (nh!=NULL)          n=nh->id;
        else                   n="none";
        Werror("ring change during procedure call %s: %s -> %s (level %d)",pi->procname,o,n,myynest);
        iiRETURNEXPR.CleanUp();
        err=TRUE;
      }
      currRing=iiLocalRing[myynest-1];
    }
    if ((currRing==NULL)
    && (currRingHdl!=NULL))
      currRing=IDRING(currRingHdl);
    else
    if ((currRing!=NULL) &&
      ((currRingHdl==NULL)||(IDRING(currRingHdl)!=currRing)
       ||(IDLEV(currRingHdl)>=myynest-1)))
    {
      rSetHdl(rFindHdl(currRing,NULL));
      iiLocalRing[myynest-1]=NULL;
    }
    //Print("kill locals for %s (level %d)\n",IDID(pn),myynest);
    killlocals(myynest);
#ifndef SING_NDEBUG
    checkall();
#endif
    //Print("end kill locals for %s (%d)\n",IDID(pn),myynest);
  }
  myynest--;
  si_echo=old_echo;
  if (pi!=NULL)
    pi->trace_flag=save_flags;
//  omUpdateInfo();
//  int m=om_Info.UsedBytes;
//  Print("exit %s, mem=%d\n",IDID(pn),m);
  return err;
}

VAR ring    *iiLocalRing;
INST_VAR sleftv  iiRETURNEXPR;
VAR int     iiRETURNEXPR_len=0;

#ifdef RDEBUG
static void iiShowLevRings()
{
  int i;
  for (i=0;i<=myynest;i++)
  {
    Print("lev %d:",i);
    if (iiLocalRing[i]==NULL) PrintS("NULL");
    else                      Print("%lx",(long)iiLocalRing[i]);
    PrintLn();
  }
  if (currRing==NULL) PrintS("curr:NULL\n");
  else                Print ("curr:%lx\n",(long)currRing);
}
#endif /* RDEBUG */

static void iiCheckNest()
{
  if (myynest >= iiRETURNEXPR_len-1)
  {
    iiLocalRing=(ring *)omreallocSize(iiLocalRing,
                                   iiRETURNEXPR_len*sizeof(ring),
                                   (iiRETURNEXPR_len+16)*sizeof(ring));
    memset(&(iiLocalRing[iiRETURNEXPR_len]),0,16*sizeof(ring));
    iiRETURNEXPR_len+=16;
  }
}
BOOLEAN iiMake_proc(idhdl pn, package pack, leftv sl)
{
  int err;
  procinfov pi = IDPROC(pn);
  if(pi->is_static && myynest==0)
  {
    Werror("'%s::%s()' is a local procedure and cannot be accessed by an user.",
           pi->libname, pi->procname);
    return TRUE;
  }
  iiCheckNest();
  iiLocalRing[myynest]=currRing;
  //Print("currRing(%d):%s(%x) in %s\n",myynest,IDID(currRingHdl),currRing,IDID(pn));
  iiRETURNEXPR.Init();
  procstack->push(pi->procname);
  if ((traceit&TRACE_SHOW_PROC)
  || (pi->trace_flag&TRACE_SHOW_PROC))
  {
    if (traceit&TRACE_SHOW_LINENO) PrintLn();
    Print("entering%-*.*s %s (level %d)\n",myynest*2,myynest*2," ",IDID(pn),myynest);
  }
#ifdef RDEBUG
  if (traceit&TRACE_SHOW_RINGS) iiShowLevRings();
#endif
  switch (pi->language)
  {
    default:
    case LANG_NONE:
                 WerrorS("undefined proc");
                 err=TRUE;
                 break;

    case LANG_SINGULAR:
                 if ((pi->pack!=NULL)&&(currPack!=pi->pack))
                 {
                   currPack=pi->pack;
                   iiCheckPack(currPack);
                   currPackHdl=packFindHdl(currPack);
                   //Print("set pack=%s\n",IDID(currPackHdl));
                 }
                 else if ((pack!=NULL)&&(currPack!=pack))
                 {
                   currPack=pack;
                   iiCheckPack(currPack);
                   currPackHdl=packFindHdl(currPack);
                   //Print("set pack=%s\n",IDID(currPackHdl));
                 }
                 err=iiPStart(pn,sl);
                 break;
    case LANG_C:
                 leftv res = (leftv)omAlloc0Bin(sleftv_bin);
                 err = (pi->data.o.function)(res, sl);
                 memcpy(&iiRETURNEXPR,res,sizeof(iiRETURNEXPR));
                 omFreeBin((ADDRESS)res,  sleftv_bin);
                 break;
  }
  if ((traceit&TRACE_SHOW_PROC)
  || (pi->trace_flag&TRACE_SHOW_PROC))
  {
    if (traceit&TRACE_SHOW_LINENO) PrintLn();
    Print("leaving %-*.*s %s (level %d)\n",myynest*2,myynest*2," ",IDID(pn),myynest);
  }
  //const char *n="NULL";
  //if (currRingHdl!=NULL) n=IDID(currRingHdl);
  //Print("currRing(%d):%s(%x) after %s\n",myynest,n,currRing,IDID(pn));
#ifdef RDEBUG
  if (traceit&TRACE_SHOW_RINGS) iiShowLevRings();
#endif
  if (err)
  {
    iiRETURNEXPR.CleanUp();
    //iiRETURNEXPR.Init(); //done by CleanUp
  }
  if (iiCurrArgs!=NULL)
  {
    if (!err) Warn("too many arguments for %s",IDID(pn));
    iiCurrArgs->CleanUp();
    omFreeBin((ADDRESS)iiCurrArgs, sleftv_bin);
    iiCurrArgs=NULL;
  }
  procstack->pop();
  if (err)
    return TRUE;
  return FALSE;
}
static void iiCallLibProcBegin()
{
  idhdl tmp_ring=NULL;
  if (currRing!=NULL)
  {
    if (IDRING(currRingHdl)!=currRing)
    {
      // clean up things depending on currRingHdl:
      sLastPrinted.CleanUp(IDRING(currRingHdl));
      sLastPrinted.Init();
      // need to define a ring-hdl for currRingHdl
      tmp_ring=enterid(" tmpRing",myynest,RING_CMD,&IDROOT,FALSE);
      IDRING(tmp_ring)=currRing;
      currRing->ref++;
      rSetHdl(tmp_ring);
    }
  }
}
static void iiCallLibProcEnd(idhdl save_ringhdl, ring save_ring)
{
  if ((currRing!=NULL)
  &&(currRing!=save_ring))
  {
    currRing->ref--;
    idhdl hh=IDROOT;
    idhdl prev=NULL;
    while((hh!=currRingHdl) && (hh!=NULL)) { prev=hh; hh=hh->next; }
    if (hh!=NULL)
    {
      if (prev==NULL) IDROOT=hh->next;
      else prev->next=hh->next;
      omFree((ADDRESS)IDID(hh));
      omFreeBin((ADDRESS)hh, idrec_bin);
    }
    else
    {
      WarnS("internal: lost ring in iiCallLib");
    }
  }
  currRingHdl=save_ringhdl;
  currRing=save_ring;
}

void* iiCallLibProc1(const char*n, void *arg, int arg_type, BOOLEAN &err)
{
  idhdl h=ggetid(n);
  if ((h==NULL)
  || (IDTYP(h)!=PROC_CMD))
  {
    err=2;
    return NULL;
  }
  // ring handling
  idhdl save_ringhdl=currRingHdl;
  ring save_ring=currRing;
  iiCallLibProcBegin();
  // argument:
  sleftv tmp;
  tmp.Init();
  tmp.data=arg;
  tmp.rtyp=arg_type;
  // call proc
  err=iiMake_proc(h,currPack,&tmp);
  // clean up ring
  iiCallLibProcEnd(save_ringhdl,save_ring);
  // return
  if (err==FALSE)
  {
    void*r=iiRETURNEXPR.data;
    iiRETURNEXPR.data=NULL;
    iiRETURNEXPR.CleanUp();
    return r;
  }
  return NULL;
}

// return NULL on failure
ideal iiCallProcId2Id(const char *lib,const char *proc, ideal arg)
{
  char *plib = iiConvName(lib);
  idhdl h=ggetid(plib);
  omFree(plib);
  if (h==NULL)
  {
    BOOLEAN bo=iiLibCmd(omStrDup(lib),TRUE,TRUE,FALSE);
    if (bo) return NULL;
  }
  BOOLEAN err;
  ideal I=(ideal)iiCallLibProc1(proc,idCopy(arg),IDEAL_CMD,err);
  if (err) return NULL;
  return I;
}

/// args: NULL terminated array of arguments
/// arg_types: 0 terminated array of corresponding types
void* iiCallLibProcM(const char*n, void **args, int* arg_types, BOOLEAN &err)
{
  idhdl h=ggetid(n);
  if ((h==NULL)
  || (IDTYP(h)!=PROC_CMD))
  {
    err=2;
    return NULL;
  }
  // ring handling
  idhdl save_ringhdl=currRingHdl;
  ring save_ring=currRing;
  iiCallLibProcBegin();
  // argument:
  if (arg_types[0]!=0)
  {
    sleftv tmp;
    leftv tt=&tmp;
    int i=1;
    tmp.Init();
    tmp.data=args[0];
    tmp.rtyp=arg_types[0];
    while(arg_types[i]!=0)
    {
      tt->next=(leftv)omAlloc0(sizeof(sleftv));
      tt=tt->next;
      tt->rtyp=arg_types[i];
      tt->data=args[i];
      i++;
    }
    // call proc
    err=iiMake_proc(h,currPack,&tmp);
  }
  else
  // call proc
    err=iiMake_proc(h,currPack,NULL);
  // clean up ring
  iiCallLibProcEnd(save_ringhdl,save_ring);
  // return
  if (err==FALSE)
  {
    void*r=iiRETURNEXPR.data;
    iiRETURNEXPR.data=NULL;
    iiRETURNEXPR.CleanUp();
    return r;
  }
  return NULL;
}
/*2
* start an example (as a proc),
* destroys the string 'example'
*/
BOOLEAN iiEStart(char* example, procinfo *pi)
{
  BOOLEAN err;
  int old_echo=si_echo;

  iiCheckNest();
  procstack->push(example);
  iiLocalRing[myynest]=currRing;
  if (traceit&TRACE_SHOW_PROC)
  {
    if (traceit&TRACE_SHOW_LINENO) printf("\n");
    printf("entering example (level %d)\n",myynest);
  }
  myynest++;

  err=iiAllStart(pi,example,BT_example,(pi != NULL ? pi->data.s.example_lineno: 0));

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
      rSetHdl(rFindHdl(iiLocalRing[myynest],NULL));
      iiLocalRing[myynest]=NULL;
    }
    else
    {
      currRingHdl=NULL;
      currRing=NULL;
    }
  }
  procstack->pop();
  return err;
}


extern "C"
{
#  define SI_GET_BUILTIN_MOD_INIT0(name) int SI_MOD_INIT0(name)(SModulFunctions*);
          SI_FOREACH_BUILTIN(SI_GET_BUILTIN_MOD_INIT0)
#  undef  SI_GET_BUILTIN_MOD_INIT0
};


SModulFunc_t
iiGetBuiltinModInit(const char* libname)
{
#  define SI_GET_BUILTIN_MOD_INIT(name) if (strcmp(libname, #name ".so") == 0){ return SI_MOD_INIT0(name); }
          SI_FOREACH_BUILTIN(SI_GET_BUILTIN_MOD_INIT)
#  undef  SI_GET_BUILTIN_MOD_INIT

  return NULL;
}




/*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*/
BOOLEAN iiTryLoadLib(leftv v, const char *id)
{
  BOOLEAN LoadResult = TRUE;
  char libnamebuf[1024];
  char *libname = (char *)omAlloc(strlen(id)+5);
  const char *suffix[] = { "", ".lib", ".so", ".sl", NULL };
  int i = 0;
  // FILE *fp;
  // package pack;
  // idhdl packhdl;
  lib_types LT;
  for(i=0; suffix[i] != NULL; i++)
  {
    sprintf(libname, "%s%s", id, suffix[i]);
    *libname = mytolower(*libname);
    if((LT = type_of_LIB(libname, libnamebuf)) > LT_NOTFOUND)
    {
      char *s=omStrDup(libname);
      #ifdef HAVE_DYNAMIC_LOADING
      char libnamebuf[1024];
      #endif

      if (LT==LT_SINGULAR)
        LoadResult = iiLibCmd(s, FALSE, FALSE,TRUE);
      #ifdef HAVE_DYNAMIC_LOADING
      else if ((LT==LT_ELF) || (LT==LT_HPUX))
        LoadResult = load_modules(s,libnamebuf,FALSE);
      #endif
      else if (LT==LT_BUILTIN)
      {
        LoadResult=load_builtin(s,FALSE, iiGetBuiltinModInit(s));
      }
      if(!LoadResult )
      {
        v->name = iiConvName(libname);
        break;
      }
    }
  }
  omFree(libname);
  return LoadResult;
}

/*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*/
/* check, if library lib has already been loaded
   if yes, writes filename of lib into where and returns TRUE,
      no, returns FALSE
*/
BOOLEAN iiLocateLib(const char* lib, char* where)
{
  char *plib = iiConvName(lib);
  idhdl pl = basePack->idroot->get(plib,0);
  if( (pl!=NULL) && (IDTYP(pl)==PACKAGE_CMD) &&
    (IDPACKAGE(pl)->language == LANG_SINGULAR))
  {
    strncpy(where,IDPACKAGE(pl)->libname,127);
    return TRUE;
  }
  else
    return FALSE;;
}

BOOLEAN iiLibCmd( char *newlib, BOOLEAN autoexport, BOOLEAN tellerror, BOOLEAN force )
{
  char libnamebuf[1024];
  // procinfov pi;
  // idhdl h;
  idhdl pl;
  // idhdl hl;
  // long pos = 0L;
  char *plib = iiConvName(newlib);
  FILE * fp = feFopen( newlib, "r", libnamebuf, tellerror );
  // int lines = 1;
  BOOLEAN LoadResult = TRUE;

  if (fp==NULL)
  {
    return TRUE;
  }
  pl = basePack->idroot->get(plib,0);
  if (pl==NULL)
  {
    pl = enterid( plib,0, PACKAGE_CMD,
                  &(basePack->idroot), TRUE );
    IDPACKAGE(pl)->language = LANG_SINGULAR;
    IDPACKAGE(pl)->libname=omStrDup(newlib);
  }
  else
  {
    if(IDTYP(pl)!=PACKAGE_CMD)
    {
      WarnS("not of type package.");
      fclose(fp);
      return TRUE;
    }
    if (!force) return FALSE;
  }
  LoadResult = iiLoadLIB(fp, libnamebuf, newlib, pl, autoexport, tellerror);
  omFree((ADDRESS)newlib);

  if(!LoadResult) IDPACKAGE(pl)->loaded = TRUE;
  omFree((ADDRESS)plib);

 return LoadResult;
}
/*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*/
static void iiCleanProcs(idhdl &root)
{
  idhdl prev=NULL;
  loop
  {
    if (root==NULL) return;
    if (IDTYP(root)==PROC_CMD)
    {
      procinfo *pi=(procinfo*)IDDATA(root);
      if ((pi->language == LANG_SINGULAR)
      && (pi->data.s.body_start == 0L))
      {
        // procinfo data incorrect:
        // - no proc body can start at the beginning of the file
        killhdl(root);
        if (prev==NULL)
          root=IDROOT;
        else
        {
          root=prev;
          prev=NULL;
        }
        continue;
      }
    }
    prev=root;
    root=IDNEXT(root);
  }
}
static void iiRunInit(package p)
{
  idhdl h=p->idroot->get("mod_init",0);
  if (h==NULL) return;
  if (IDTYP(h)==PROC_CMD)
  {
    int save=yylineno;
    myynest++;
    // procinfo *pi=(procinfo*)IDDATA(h);
    //PrintS("mod_init found\n");
    iiMake_proc(h,p,NULL);
    myynest--;
    yylineno=save;
  }
}
/*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*/
BOOLEAN iiLoadLIB(FILE *fp, const char *libnamebuf, const char*newlib,
             idhdl pl, BOOLEAN autoexport, BOOLEAN tellerror)
{
  EXTERN_VAR FILE *yylpin;
  libstackv ls_start = library_stack;
  lib_style_types lib_style;

  yylpin = fp;
  #if YYLPDEBUG > 1
  print_init();
  #endif
  EXTERN_VAR int lpverbose;
  if (BVERBOSE(V_DEBUG_LIB)) lpverbose=1;
  else lpverbose=0;
  // yylplex sets also text_buffer
  if (text_buffer!=NULL) *text_buffer='\0';
  yylplex(newlib, libnamebuf, &lib_style, pl, autoexport);
  if(yylp_errno)
  {
    Werror("Library %s: ERROR occurred: in line %d, %d.", newlib, yylplineno,
         current_pos(0));
    if(yylp_errno==YYLP_BAD_CHAR)
    {
      Werror(yylp_errlist[yylp_errno], *text_buffer, yylplineno);
      omFree((ADDRESS)text_buffer);
      text_buffer=NULL;
    }
    else
      Werror(yylp_errlist[yylp_errno], yylplineno);
    WerrorS("Cannot load library,... aborting.");
    reinit_yylp();
    fclose( yylpin );
    iiCleanProcs(IDROOT);
    return TRUE;
  }
  if (BVERBOSE(V_LOAD_LIB))
    Print( "// ** loaded %s %s\n", libnamebuf, text_buffer);
  if( (lib_style == OLD_LIBSTYLE) && (BVERBOSE(V_LOAD_LIB)))
  {
    Warn( "library %s has old format. This format is still accepted,", newlib);
    WarnS( "but for functionality you may wish to change to the new");
    WarnS( "format. Please refer to the manual for further information.");
  }
  reinit_yylp();
  fclose( yylpin );
  fp = NULL;
  iiRunInit(IDPACKAGE(pl));

  {
    libstackv ls;
    for(ls = library_stack; (ls != NULL) && (ls != ls_start); )
    {
      if(ls->to_be_done)
      {
        ls->to_be_done=FALSE;
        iiLibCmd(ls->get(),autoexport,tellerror,FALSE);
        ls = ls->pop(newlib);
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

  if(fp != NULL) fclose(fp);
  return FALSE;
}


/*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*/
procinfo *iiInitSingularProcinfo(procinfov pi, const char *libname,
              const char *procname, int, long pos, BOOLEAN pstatic)
{
  memset(pi,0,sizeof(*pi));
  pi->libname = omStrDup(libname);
  pi->procname = omStrDup(procname);
  pi->language = LANG_SINGULAR;
  pi->ref = 1;
  pi->is_static = pstatic;
  pi->data.s.proc_start = pos;
  return(pi);
}

/*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*/
int iiAddCproc(const char *libname, const char *procname, BOOLEAN pstatic,
               BOOLEAN(*func)(leftv res, leftv v))
{
  procinfov pi;
  idhdl h;

  #ifndef SING_NDEBUG
  int dummy;
  if (IsCmd(procname,dummy))
  {
    Werror(">>%s< is a reserved name",procname);
    return 0;
  }
  #endif

  h=IDROOT->get(procname,0);
  if ((h!=NULL)
  && (IDTYP(h)==PROC_CMD))
  {
    pi = IDPROC(h);
    #if 0
    if ((pi->language == LANG_SINGULAR)
    &&(BVERBOSE(V_REDEFINE)))
      Warn("extend `%s`",procname);
    #endif
  }
  else
  {
    h = enterid(procname,0, PROC_CMD, &IDROOT, TRUE);
  }
  if ( h!= NULL )
  {
    pi = IDPROC(h);
    if((pi->language == LANG_SINGULAR)
    ||(pi->language == LANG_NONE))
    {
      omfree(pi->libname);
      pi->libname = omStrDup(libname);
      omfree(pi->procname);
      pi->procname = omStrDup(procname);
      pi->language = LANG_C;
      pi->ref = 1;
      pi->is_static = pstatic;
      pi->data.o.function = func;
    }
    else if(pi->language == LANG_C)
    {
      if(pi->data.o.function == func)
      {
        pi->ref++;
      }
      else
      {
        omfree(pi->libname);
        pi->libname = omStrDup(libname);
        omfree(pi->procname);
        pi->procname = omStrDup(procname);
        pi->language = LANG_C;
        pi->ref = 1;
        pi->is_static = pstatic;
        pi->data.o.function = func;
      }
    }
    else
      Warn("internal error: unknown procedure type %d",pi->language);
    if (currPack->language==LANG_SINGULAR) currPack->language=LANG_MIX;
    return(1);
  }
  else
  {
    WarnS("iiAddCproc: failed.");
  }
  return(0);
}

int iiAddCprocTop(const char *libname, const char *procname, BOOLEAN pstatic,
               BOOLEAN(*func)(leftv res, leftv v))
{
  int r=iiAddCproc(libname,procname,pstatic,func);
  package s=currPack;
  currPack=basePack;
  if (r) r=iiAddCproc(libname,procname,pstatic,func);
  currPack=s;
  return r;
}

/*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*/
#ifdef HAVE_DYNAMIC_LOADING
#include <map>
#include <string>
#include <pthread.h>

THREAD_VAR std::map<std::string, void *> *dyn_modules;

bool registered_dyn_module(char *fullname) {
  if (dyn_modules == NULL)
    return false;
  std::string fname = fullname;
  return dyn_modules->count(fname) != 0;
}

void register_dyn_module(char *fullname, void * handle) {
  std::string fname = fullname;
  if (dyn_modules == NULL)
    dyn_modules = new std::map<std::string, void *>();
  dyn_modules->insert(std::pair<std::string, void *>(fname, handle));
}

void close_all_dyn_modules() {
  for (std::map<std::string, void *>::iterator it = dyn_modules->begin();
       it != dyn_modules->end();
       it++)
  {
    dynl_close(it->second);
  }
  delete dyn_modules;
  dyn_modules = NULL;
}
BOOLEAN load_modules_aux(const char *newlib, char *fullname, BOOLEAN autoexport)
{
#ifdef HAVE_STATIC
  WerrorS("mod_init: static version can not load modules");
  return TRUE;
#else
/*
  typedef int (*fktn_t)(int(*iiAddCproc)(const char *libname, const char *procname,
                               BOOLEAN pstatic,
                               BOOLEAN(*func)(leftv res, leftv v)));
*/
  SModulFunc_t fktn;
  idhdl pl;
  char *plib = iiConvName(newlib);
  BOOLEAN RET=TRUE;
  int token;
  char FullName[256];

  memset(FullName,0,256);

  if( *fullname != '/' &&  *fullname != '.' )
    sprintf(FullName, "./%s", newlib);
  else strncpy(FullName, fullname,255);


  if(IsCmd(plib, token))
  {
    Werror("'%s' is resered identifier\n", plib);
    goto load_modules_end;
  }
  pl = basePack->idroot->get(plib,0); /* packages only in top level
                                        (see enterid) */
  if ((pl!=NULL)
  &&(IDTYP(pl)==PACKAGE_CMD))
  {
    if(IDPACKAGE(pl)->language==LANG_C)
    {
      if (BVERBOSE(V_LOAD_LIB)) Warn( "%s already loaded as package", newlib);
      omFree(plib);
      return FALSE;
    }
    else if(IDPACKAGE(pl)->language==LANG_MIX)
    {
      if (BVERBOSE(V_LOAD_LIB)) Warn( "%s contain binary parts, cannot load", newlib);
      omFree(plib);
      return FALSE;
    }
  }
  else
  {
    pl = enterid( plib,0, PACKAGE_CMD, &IDROOT, TRUE );
    omFree(plib); /* enterid copied plib*/
    IDPACKAGE(pl)->libname=omStrDup(newlib);
  }
  IDPACKAGE(pl)->language = LANG_C;
  if (dynl_check_opened(FullName))
  {
    if (BVERBOSE(V_LOAD_LIB)) Warn( "%s already loaded as C library", fullname);
    return FALSE;
  }
  if((IDPACKAGE(pl)->handle=dynl_open(FullName))==(void *)NULL)
  {
    Werror("dynl_open failed:%s", dynl_error());
    Werror("%s not found", newlib);
    killhdl2(pl,&(basePack->idroot),NULL); // remove package
    goto load_modules_end;
  }
  else
  {
    SModulFunctions sModulFunctions;

    package s=currPack;
    currPack=IDPACKAGE(pl);
    fktn = (SModulFunc_t)dynl_sym(IDPACKAGE(pl)->handle, "mod_init");
    if( fktn!= NULL)
    {
      sModulFunctions.iiArithAddCmd = iiArithAddCmd;
      if (autoexport) sModulFunctions.iiAddCproc = iiAddCprocTop;
      else            sModulFunctions.iiAddCproc = iiAddCproc;
      int ver=(*fktn)(&sModulFunctions);
      if (ver==MAX_TOK)
      {
        if (BVERBOSE(V_LOAD_LIB)) Print( "// ** loaded %s\n", fullname);
      }
      else
      {
        Warn("loaded %s for a different version of Singular(expected MAX_TOK: %d, got %d)",fullname,MAX_TOK,ver);
      }
      currPack->loaded=1;
      currPack=s; /* reset currPack to previous */
      register_dyn_module(fullname, IDPACKAGE(pl)->handle);
      RET=FALSE;
    }
    else
    {
      Werror("mod_init not found:: %s\nThis is probably not a dynamic module for Singular!\n", dynl_error());
      errorreported=0;
      if(IDPACKAGE(pl)->idroot==NULL)
        killhdl2(pl,&(basePack->idroot),NULL); // remove package
    }
  }

  load_modules_end:
  return RET;
#endif /*STATIC */
}

BOOLEAN load_modules(const char *newlib, char *fullname, BOOLEAN autoexport) {
  GLOBAL_VAR static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
  pthread_mutex_lock(&mutex);
  BOOLEAN r = load_modules_aux(newlib, fullname, autoexport);
  pthread_mutex_unlock(&mutex);
  return r;
}
#endif /* HAVE_DYNAMIC_LOADING */
/*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*/
BOOLEAN load_builtin(const char *newlib, BOOLEAN autoexport, SModulFunc_t init)
{
  int iiAddCproc(const char *libname, const char *procname, BOOLEAN pstatic,
                 BOOLEAN(*func)(leftv res, leftv v));
/*
  typedef int (*fktn_t)(int(*iiAddCproc)(const char *libname, const char *procname,
                               BOOLEAN pstatic,
                               BOOLEAN(*func)(leftv res, leftv v)));
*/
  // SModulFunc_t fktn;
  idhdl pl;
  char *plib = iiConvName(newlib);
  // BOOLEAN RET=TRUE;
  // int token;

  pl = basePack->idroot->get(plib,0); // search PACKAGE only in Top
  if ((pl!=NULL)
  &&(IDTYP(pl)==PACKAGE_CMD))
  {
    if(IDPACKAGE(pl)->language==LANG_C)
    {
      if (BVERBOSE(V_LOAD_LIB)) Warn( "(builtin) %s already loaded", newlib);
      omFree(plib);
      return FALSE;
    }
  }
  else
  {
    pl = enterid( plib,0, PACKAGE_CMD, &IDROOT, TRUE );
    IDPACKAGE(pl)->libname=omStrDup(newlib);
  }
  IDPACKAGE(pl)->language = LANG_C;

  IDPACKAGE(pl)->handle=(void *)NULL;
  SModulFunctions sModulFunctions;

  package s=currPack;
  currPack=IDPACKAGE(pl);
  if( init!= NULL)
  {
    sModulFunctions.iiArithAddCmd = iiArithAddCmd;
    if (autoexport) sModulFunctions.iiAddCproc = iiAddCprocTop;
    else            sModulFunctions.iiAddCproc = iiAddCproc;
    (*init)(&sModulFunctions);
  }
  if (BVERBOSE(V_LOAD_LIB)) Print( "// ** loaded (builtin) %s \n", newlib);
  currPack->loaded=1;
  currPack=s;

  return FALSE;
}
/*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*/
void module_help_main(const char *newlib,const char *help)
{
  char *plib = iiConvName(newlib);
  idhdl pl = basePack->idroot->get(plib,0);
  if ((pl==NULL)||(IDTYP(pl)!=PACKAGE_CMD))
    Werror(">>%s<< is not a package (trying to add package help)",plib);
  else
  {
    package s=currPack;
    currPack=IDPACKAGE(pl);
    idhdl h=enterid("info",0,STRING_CMD,&IDROOT,FALSE);
    IDSTRING(h)=omStrDup(help);
    currPack=s;
  }
}
void module_help_proc(const char *newlib,const char *p, const char *help)
{
  char *plib = iiConvName(newlib);
  idhdl pl = basePack->idroot->get(plib,0);
  if ((pl==NULL)||(IDTYP(pl)!=PACKAGE_CMD))
    Werror(">>%s<< is not a package(trying to add help for %s)",plib,p);
  else
  {
    package s=currPack;
    currPack=IDPACKAGE(pl);
    char buff[256];
    buff[255]='\0';
    strncpy(buff,p,255);
    strncat(buff,"_help",255-strlen(p));
    idhdl h=enterid(buff,0,STRING_CMD,&IDROOT,FALSE);
    IDSTRING(h)=omStrDup(help);
    currPack=s;
  }
}
/*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*/

#ifdef HAVE_DYNAMIC_LOADING
// loads a dynamic module from the binary path and returns a named function
// returns NULL, if something fails
void* binary_module_function(const char* newlib, const char* funcname)
{
  void* result = NULL;

  const char* bin_dir = feGetResource('b');
  if (!bin_dir)  { return NULL; }

  char path_name[MAXPATHLEN];
  sprintf(path_name, "%s%s%s.%s", bin_dir, DIR_SEPP, newlib, MODULE_SUFFIX_STRING);

  void* openlib = dynl_open(path_name);
  if(!openlib)
  {
    Werror("dynl_open of %s failed:%s", path_name, dynl_error());
    return NULL;
  }
  result = dynl_sym(openlib, funcname);
  if (!result) Werror("%s: %s\n", funcname, dynl_error());

  return result;
}
#endif

/*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*/
char mytoupper(char c)
{
  if(c>=97 && c<=(97+26)) c-=32;
  return(c);
}

char mytolower(char c)
{
  if(c>=65 && c<=(65+26)) c+=32;
  return(c);
}

/*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*/
//#if defined(WINNT)
//#  define  FS_SEP '\\'
//#else
//#  define FS_SEP '/'
//#endif

char *iiConvName(const char *libname)
{
  char *tmpname = omStrDup(libname);
  char *p = strrchr(tmpname, DIR_SEP);
  char *r;
  if(p==NULL) p = tmpname; else p++;
  // p is now the start of the file name (without path)
  r=p;
  while(isalnum(*r)||(*r=='_')) r++;
  // r point the the end of the main part of the filename
  *r = '\0';
  r = omStrDup(p);
  *r = mytoupper(*r);
  // printf("iiConvName: '%s' '%s' => '%s'\n", libname, tmpname, r);
  omFree((ADDRESS)tmpname);

  return(r);
}

/*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*/
#if 0 /* debug only */
void piShowProcList()
{
  idhdl h;
  procinfo *proc;
  char *name;

  Print( "%-15s  %20s      %s,%s  %s,%s   %s,%s\n", "Library", "function",
         "line", "start", "line", "body", "line", "example");
  for(h = IDROOT; h != NULL; h = IDNEXT(h))
  {
    if(IDTYP(h) == PROC_CMD)
    {
      proc = IDPROC(h);
      if(strcmp(proc->procname, IDID(h))!=0)
      {
        name = (char *)omAlloc(strlen(IDID(h))+strlen(proc->procname)+4);
        sprintf(name, "%s -> %s", IDID(h), proc->procname);
        Print( "%d %-15s  %20s ", proc->is_static ? 1 : 0, proc->libname, name);
        omFree((ADDRESS)name);
      }
      else
        Print( "%d %-15s  %20s ", proc->is_static ? 1 : 0, proc->libname,
               proc->procname);
      if(proc->language==LANG_SINGULAR)
        Print("line %-5ld  %4d,%-5ld  %4d,%-5ld\n",
              proc->data.s.proc_start,
              proc->data.s.body_lineno, proc->data.s.body_start,
              proc->data.s.example_lineno, proc->data.s.example_start);
      else if(proc->language==LANG_C)
        PrintS("type: object\n");
    }
  }
}
#endif

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
void libstack::push(const char */*p*/, char *libn)
{
  libstackv lp;
  if( !iiGetLibStatus(libn))
  {
    for(lp = this;lp!=NULL;lp=lp->next)
    {
      if(strcmp(lp->get(), libn)==0) break;
    }
    if(lp==NULL)
    {
      libstackv ls = (libstack *)omAlloc0Bin(libstack_bin);
      ls->next = this;
      ls->libname = omStrDup(libn);
      ls->to_be_done = TRUE;
      if(library_stack != NULL) ls->cnt = library_stack->cnt+1; else ls->cnt = 0;
      library_stack = ls;
    }
  }
}

libstackv libstack::pop(const char */*p*/)
{
  libstackv ls = this;
  //omFree((ADDRESS)ls->libname);
  library_stack = ls->next;
  omFreeBin((ADDRESS)ls,  libstack_bin);
  return(library_stack);
}

#endif /* HAVE_LIBPARSER */
/*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*/
