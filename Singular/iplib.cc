/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
static char rcsid[] = "$Id: iplib.cc,v 1.2 1997-03-24 14:24:57 Singular Exp $";
/*
* ABSTRACT:
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

/*2
* find the library of an proc:
* first case: IDSTRING="LIB:xyz.lib"
*  => return (string+4)
* second case: IDSTRING=".....);\n"
*  => return NULL; (no lib known)
* else: IDSTRING="....\nxxx.lib"
*  => return (xxx.lib)
*/
char * iiGetLibName(char *procstr)
{
  char *res=NULL;

  if (strncmp(procstr,"LIB:",4)==0)
  {
    res=(procstr+4);
  }
  else
  {
    int l=strlen(procstr)-1; /* procstr[l] is the last character */
    while((l>0) && (procstr[l]!='\n')) l--;
    if (procstr[l]=='\n') res=(procstr+l+1);
  }
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
*    => Print, return NULL
*  part=1: body, between "{ ..." and "}", including the 1. line, w/o "{"
*    => FreeL(libname), return
*  part=2: example, between, but excluding the line "exapmle {..." and "}":
*    => return
*/
char* iiGetLibProcBuffer( char* libname, char* procname, int part )
{
  /* =========== open lib ==============*/
  char buf[256];
  if (part==1)
  {
    // libname comes as LIB:name
    libname += 4;
    //libname[strlen(libname)-1] = '\0';
  }
  FILE * fp = feFopen( libname, "rb", NULL, TRUE );
  if (fp==NULL)
  {
    if (part==1) FreeL((ADDRESS)(libname-4));
    return NULL;
  }

  /* =========== search lib =========== */
  while (fgets( buf, sizeof(buf), fp))
  {
    /* search for lines "proc ..." */
    if (strncmp( buf, "proc ", 5) == 0)
    {
      char ct;
      char *e;
      char *s=iiProcName(buf,ct,e);
      char *argstr=NULL;
      if (strcmp( s, procname ) == 0)
      {
        if (part==1)
        {
          // =========== processing of argument list =============
          *e=ct;
          argstr=iiProcArgs(e,TRUE);
        }
        int procbuflen;
        // =========== search beginning of proc body ============
        long startpos = ftell( fp );
        loop
        {
          if(!fgets( buf, sizeof(buf), fp)) return NULL; // search "{"
          //if (strchr(buf,'{')!=NULL)
          if (buf[0]=='{')
          {
             if (part==0) return NULL;
             procbuflen=strlen(buf);
             break;
          }
          if (part==0) PrintS(buf);
          startpos = ftell( fp );
        }

        // =========== search end of proc body ============
        // read till "}", to calculate length of procbuffer
        while (fgets( buf, sizeof(buf), fp))
        {
          if (buf[0] == '}') break;
          procbuflen += strlen(buf);
        }
        // =========== search example part ============
        if (part==2)
        {
          procbuflen = 0;
          loop
          {
            /* EOF ? */
            if (!fgets( buf, sizeof(buf), fp))
              return NULL;
            /* found ? */
            if (strncmp( buf, "example", 7) == 0)
              break;
            /* next proc ? */
            if (strncmp( buf, "proc ", 5) == 0)
              return NULL;
          }
          startpos = ftell( fp );
          while (fgets( buf, sizeof(buf), fp))
          {
            if (buf[0] == '}') break;
            procbuflen += strlen(buf);
          }
          //Print("start:%d, len:%d\n",startpos,procbuflen);
        }

        // =========== read the part ===========
        char* procbuf;
        if (part==1) procbuf = (char *)AllocL( strlen(argstr)+procbuflen+14+strlen(libname) );
        else         procbuf = (char *)AllocL( procbuflen+14+strlen(libname) );
        if (procbuf==NULL)
        {
          Werror( "unable to allocate proc buffer `%s`", procname );
          if (part==1) FreeL((ADDRESS)(libname-4));
          return NULL;
        }
        fseek( fp, startpos, SEEK_SET );
        if (part==1)
        {
          strcpy(procbuf,argstr);
          fread( procbuf+strlen(argstr), procbuflen, 1, fp);
          procbuflen+=strlen(argstr);
          FreeL(argstr);
        }
        else
        {
          fread( procbuf, procbuflen, 1, fp);
        }
        fclose( fp );
        procbuf[procbuflen] = '\0';

        // =========== modify text ===========
        //if ((part==1)||(part==2))
        {
          //strcpy( procbuf+procbuflen, "\n;RETURN();\n" );
          strcat( procbuf+procbuflen-3, "\n;RETURN();\n\n" );
          if (part==1)
          {
            strcat( procbuf+procbuflen+10,libname);
            FreeL((ADDRESS)(libname-4));
          }
        }
        s=strchr(procbuf,'{');
        if (s!=NULL) *s=' ';
        //Print("end iiGetLibProcBuffer, changed pos: %d, s=%x\n",procbuf-s,s);
        return procbuf;
      }
    }
  }
  Werror( "`%s` not found in LIB `%s`", procname, libname );
  if (part==1) FreeL((ADDRESS)(libname-4));
  return NULL;
}

/*2
* start either a proc or a file
* parameters are built as exprlist
* if both procname and filename are defined, it is an interrupt !
*/
BOOLEAN iiPStart(idhdl pn, char* filename, sleftv  * v)
{
  char * str;
  BOOLEAN err=FALSE;

  /* init febase ======================================== */
  if (filename!=NULL)
  {
    FILE *fp=feFopen(filename,"r",NULL,TRUE);
    if (fp==NULL)
    {
      return FALSE;
    }
    fseek(fp,0L,SEEK_END);
    long len=ftell(fp);
    fseek(fp,0L,SEEK_SET);
    char *filedata=(char *)AllocL((int)len+1);
    fread( filedata, len, 1, fp);
    filedata[len]='\0';
    char *currpos=filedata;
    char *found;
    while ((found=strstr(currpos,"\\\n"))!=NULL)
    {
      register char *np=found;
      register char *op;
      if (*(currpos-1)=='\\')
        op=np+1;
      else
        op=np+2;
      do
      {
        *(np++)=*(op++);
      }
      while (*np!='\0');
      currpos=found;
    }
    str  = filename;
    newBuffer( filedata, BT_file, filename );
    fileVoice = voice;
  }
  else
  {
    /* we do not enter this case if filename != NULL !! */
    if (pn!=NULL)
    {
      if (strncmp( IDSTRING(pn), "LIB:", 4 ) == 0)
      {
        IDSTRING(pn) = iiGetLibProcBuffer( IDSTRING(pn), IDID(pn) );
        if (IDSTRING(pn)==NULL) return TRUE;
      }
      newBuffer( mstrdup(IDSTRING(pn)), BT_proc, IDID(pn) );
      fileVoice = voice;
    }
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
  if (filename==NULL) //(pn) -> this is a proc call
  {
    void * oldb = myynewbuffer();
    myynest++;
    err=yyparse();
    killlocals(myynest);
    myynest--;
    myyoldbuffer(oldb);
  }
  else // -> this is file input or interrupt
  if (pn!=NULL) // interupt
  {
    myynest++;
    err=yyparse();
    killlocals(myynest);
    myynest--;
  }
  else // -> this is file input
  {
    void * oldb = myynewbuffer();
    err=yyparse();
    myyoldbuffer(oldb);
  }
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
  err=iiPStart(pn,NULL,sl);
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
BOOLEAN iiEStart(char* example)
{
  BOOLEAN err;

  newBuffer( example, BT_example, "example" );
  fileVoice = voice;
  void * oldb = myynewbuffer();
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
  myyoldbuffer(oldb);
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

BOOLEAN iiLibCmd( char *newlib )
{
  char buf[256];
  char libnamebuf[128];
  idhdl h,hl;
  FILE * fp = feFopen( newlib, "r", libnamebuf, TRUE );
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
  #define IN_LIB_HEADER 3
  int v=-1,otherLines=0,inBlock=IN_LIB_HEADER;
  do /*while (fgets( buf, sizeof(buf), fp))*/
  {
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
          sscanf( buf, "proc %s", proc);
          char *ct=strchr(proc,'(');
          if (ct!=NULL) *ct='\0';
          sprintf( buf, "LIB:%s", newlib);
          h = enterid( mstrdup(proc), myynest, PROC_CMD, &idroot, FALSE );
          if (h!=NULL)
          {
            IDSTRING(h) = mstrdup( buf );
            if (BVERBOSE(V_LOAD_PROC)) Warn( "     proc %s loaded", proc );
          }
          inBlock=IN_HEADER;
        }
        else if (strncmp( buf, "// ver:", 7) == 0)
        {
          v=0;
          sscanf( buf+7, "%d", &v);
          if(v!=(VERSION/10))
            Warn("version mismatch - library `%s` requires:%d.%d.%d",
                  newlib,v/100,(v%100)/10,v%10);
        }
        else if (strncmp( buf, "example", 7) == 0)
        {
          inBlock=IN_HEADER;
        }
        else if (strncmp( buf, "//", 2) != 0)
        {
          if (inBlock==0)
          {
            otherLines++;
          }
        }
      }
      else if (inBlock==IN_HEADER)
      {
        if (buf[0]=='{')
        {
          inBlock=IN_BODY;
        }
      }
      else if (inBlock==IN_BODY)
      {
        if (buf[0]=='}')
        {
          inBlock=0;
        }
      }
    }
  } while (fgets( buf, sizeof(buf), fp));
  fclose( fp );
  if (BVERBOSE(V_DEBUG_LIB))
  {
    if (inBlock!=0)
      Warn("LIB `%s` ends within block",newlib);
    if (otherLines!=0)
      Warn("%d lines not recognised in LIB `%s`",otherLines,newlib);
    if(v==-1)
      Warn("LIB `%s` has no version flag",newlib);
  }
  FreeL((ADDRESS)newlib);
  return FALSE;
}

