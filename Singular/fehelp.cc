/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/*
* ABSTRACT: help system
*/

#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#include <time.h>
#include <limits.h>

#include "mod2.h"
#include "tok.h"
#include "mmemory.h"
#include "febase.h"
#include "fehelp.h"
#include "ipid.h"
#include "ipshell.h"

#ifdef HAVE_LIBPARSER
#  include "libparse.h"
#endif /* HAVE_LIBPARSER */


/*0 implementation*/

void singular_help(char *str)
{
  /* cut leading and trailing white spaces: --------------------------------*/
  char *s=str;
  while (*s==' ') s++;
  char *ss=s;
  while (*ss!='\0') ss++;
  while (*ss<=' ')
  {
    *ss='\0';
    ss--;
  }
  /* try to find the help string >>s<<  in the index: ----------------------*/
  /* if found, call external help browser and return */

  /* try to find the help string as a loaded procedure: --------------------*/
  /* if found, display the help and return */
  #ifdef HAVE_NAMESPACES
  idhdl h, ns;
  iiname2hdl(s, &ns, &h);
  #else /* HAVE_NAMESPACES */
  idhdl h=idroot->get(s,myynest);
  #endif /* HAVE_NAMESPACES */
  if ((h!=NULL) && (IDTYP(h)==PROC_CMD)
  && (strcmp(IDPROC(h)->libname, "standard.lib")!=0))
  {
    char *lib=iiGetLibName(IDPROC(h));
    if((lib!=NULL)&&(*lib!='\0'))
    {
      Print("// proc %s from lib %s\n",s,lib);
      s=iiGetLibProcBuffer(IDPROC(h), 0);
      PrintS(s);
      FreeL((ADDRESS)s);
    }
    return;
  }

  /* try to find the help string as a library: ------------------------------*/
  char libnamebuf[128];
  FILE *fp=NULL;
  if ((str[1]!='\0')
  && ((fp=feFopen(str,"rb", libnamebuf))!=NULL))
  {
    #ifdef HAVE_LIBPARSER
      extern FILE *yylpin;
      lib_style_types lib_style; // = OLD_LIBSTYLE;
  
      yylpin = fp;
      #ifdef HAVE_NAMESPACES
      yylplex(str, libnamebuf, &lib_style, IDROOT, FALSE, GET_INFO);
      #else /* HAVE_NAMESPACES */
      yylplex(str, libnamebuf, &lib_style, GET_INFO);
      #endif /* HAVE_NAMESPACES */
      reinit_yylp();
      if(lib_style == OLD_LIBSTYLE)
      {
        char buf[256];
        fseek(fp, 0, SEEK_SET);
    #else /* HAVE_LIBPARSER */
      { char buf[256];
    #endif /* HAVE_LIBPARSER */
        Warn( "library %s has an old format. Please fix it for the next time",
              str);
        BOOLEAN found=FALSE;
        while (fgets( buf, sizeof(buf), fp))
        {
          if (strncmp(buf,"//",2)==0)
          {
            if (found) return;
          }
          else if ((strncmp(buf,"proc ",5)==0)||(strncmp(buf,"LIB ",4)==0))
          {
            if (!found) WarnS("no help part in library found");
            return;
          }
          else
          {
            found=TRUE;
            PrintS(buf);
          }
        }
      }
    #ifdef HAVE_LIBPARSER
      else
      {
        fclose( yylpin );
        PrintS(text_buffer);
        FreeL(text_buffer);
        text_buffer=NULL;
      }
    #endif /* HAVE_LIBPARSER */
    return;
  }
  /* try to find the help string as a substring: -----------------------------*/
  /* if found, call external help browser and return */

  /* the catch-all is: help index; -------------------------------------------*/
  /* find "index", call external help browser */

  #ifdef HAVE_TCL
  if(!tclmode)
  #endif
  {
    #ifdef buildin_help
    singular_manual(str);
    #else
    system(feGetInfoCall(str));
    #endif
  }
}

/* ========================================================================== */


#ifdef buildin_help

#define HELP_OK        0

#define FIN_INDEX    '\037'
#define not  !
#define HELP_NOT_OPEN  1
#define HELP_NOT_FOUND 2
#ifndef macintosh
#define Index_File     SINGULAR_INFODIR "/singular.hlp"
#define Help_File      SINGULAR_INFODIR "/singular.hlp"
#else
#define Index_File     SINGULAR_INFODIR "singular.hlp"
#define Help_File      SINGULAR_INFODIR "singular.hlp"
#endif
#define BUF_LEN        128
#define IDX_LEN        64
#define MAX_LINES      21

//static int compare(char *s1,char *s2)
//{
//  for(;*s1==*s2;s1++,s2++)
//     if(*s2=='\0') return(0);
//  return(*s2);
//}

#ifdef macintosh
static char tolow(char p)
#else
static inline char tolow(char p)
#endif
{
  if (('A'<=p)&&(p<='Z')) return p | 040;
  return p;
}

/*************************************************/
static int show(unsigned long offset,FILE *help, char *close)
{ char buffer[BUF_LEN+1];
  int  lines = 0;

  if( help== NULL)
    if( (help = feFopen(Help_File, "r")) == NULL)
      return HELP_NOT_OPEN;

  fseek(help,  (long)(offset+1), (int)0);
  while( !feof(help)
        && *fgets(buffer, BUF_LEN, help) != EOF
        && buffer[0] != FIN_INDEX)
  {
    printf("%s", buffer);
    if(lines++> MAX_LINES)
    {
#ifdef macintosh
      printf("\n Press <RETURN> to continue or x and <RETURN> to exit help.\n");
#else
      printf("\n Press <RETURN> to continue or x to exit help.\n");
#endif
      fflush(stdout);
      *close = (char)getchar();
      if(*close=='x')
      {
        getchar();
        break;
      }
      lines=0;
    }
  }
  if(*close!='x')
  {
#ifdef macintosh
    printf("\nEnd of part. Press <RETURN> to continue or x and <RETURN> to exit help.\n");
#else
    printf("\nEnd of part. Press <RETURN> to continue or x to exit help.\n");
#endif
    fflush(stdout);
    *close = (char)getchar();
    if(*close=='x')
      getchar();
  }
  return HELP_OK;
}

/*************************************************/
int singular_manual(char *str)
{ FILE *index=NULL,*help=NULL;
  unsigned long offset;
  char *p,close;
  int done = 0;
  char buffer[BUF_LEN+1],
       Index[IDX_LEN+1],
       String[IDX_LEN+1];

  if( (index = feFopen(Index_File, "r",NULL,TRUE)) == NULL)
  {
    return HELP_NOT_OPEN;
  }

  for(p=str; *p; p++) *p = tolow(*p);/* */
  do
  {
    p--;
  }
  while ((p != str) && (*p<=' '));
  p++;
  *p='\0';
  (void)sprintf(String, " %s ", str);

  while(!feof(index)
        && fgets(buffer, BUF_LEN, index) != (char *)0
        && buffer[0] != FIN_INDEX);

  while(!feof(index))
  {
    (void)fgets(buffer, BUF_LEN, index); /* */
    (void)sscanf(buffer, "Node:%[^\177]\177%ld\n", Index, &offset);
    for(p=Index; *p; p++) *p = tolow(*p);/* */
    (void)strcat(Index, " ");
    if( strstr(Index, String)!=NULL)
    {
      done++; (void)show(offset, help, &close);
    }
    Index[0]='\0';
    if(close=='x')
    break;
  }
  (void)fclose(index);
  (void)fclose(help);
  if(not done)
  {
    Warn("`%s` not found",String);
    return HELP_NOT_FOUND;
  }
  return HELP_OK;
}
#endif // buildin_help
/*************************************************/
