#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include "utils.h"

/*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*/

procinfo *iiInitSingularProcinfo(procinfov pi, char *libname,
                                 char *procname, int line, long pos,
				 BOOLEAN pstatic = FALSE)
{
  pi->libname = (char *)malloc(strlen(libname)+1);
  memcpy(pi->libname, libname, strlen(libname));
  *(pi->libname+strlen(libname)) = '\0';

  pi->procname = (char *)malloc(strlen(procname)+1);
  strcpy(pi->procname, procname/*, strlen(procname)*/);
  pi->language = LANG_SINGULAR;
  pi->ref = 1;
  pi->is_static = pstatic;
  pi->data.s.proc_start = pos;
  pi->data.s.def_end    = 0L;
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
pi_clear(procinfov pi)
{
  free(pi->libname);
  free(pi->procname);
  free(pi);
}

/*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*/
printpi(procinfov pi)
{
  FILE *fp = fopen( pi->libname, "rb");
  char *buf, name[256];
  int len1, len2;

  if(pi->data.s.body_end==0) 
    pi->data.s.body_end = pi->data.s.proc_end;

  printf( "%c %-15s  %20s ", pi->is_static ? 'l' : 'g', pi->libname,
	  pi->procname);
  printf("line %4d,%5ld-%-5ld  %4d,%5ld-%-5ld  %4d,%5ld-%-5ld\n",
	 pi->data.s.proc_lineno, pi->data.s.proc_start, pi->data.s.def_end,
	 pi->data.s.body_lineno, pi->data.s.body_start, pi->data.s.body_end,
	 pi->data.s.example_lineno, pi->data.s.example_start,
	 pi->data.s.proc_end);

#if 0
  if( fp != NULL) { // loading body
    len1 = pi->data.s.def_end - pi->data.s.proc_start;
    if(pi->data.s.body_end==0) 
      len2 = pi->data.s.proc_end - pi->data.s.body_start;
    else len2 = pi->data.s.body_end - pi->data.s.body_start;
    buf = (char *)malloc(len1 + len2 + 1);
    fseek(fp, pi->data.s.proc_start, SEEK_SET);
    fread( buf, len1, 1, fp);
    *(buf+len1) = '\n';
    fseek(fp, pi->data.s.body_start, SEEK_SET);
    fread( buf+len1+1, len2, 1, fp);
    *(buf+len1+len2+1)='\0';
    printf("##BODY:'%s'##\n", buf);
    free(buf);

    // loading help
    len1 = pi->data.s.body_start - pi->data.s.proc_start;
    printf("len1=%d;\n", len1);
    buf = (char *)malloc(len1+1);
    fseek(fp, pi->data.s.proc_start, SEEK_SET);
    fread( buf, len1, 1, fp);
    *(buf+len1)='\0';
    printf("##HELP:'%s'##\n", buf);
    free(buf);

    if(pi->data.s.example_start>0) { // loading example
      fseek(fp, pi->data.s.example_start, SEEK_SET);
      len2 = pi->data.s.proc_end - pi->data.s.example_start;
      buf = (char *)malloc(len2+1);
      fread( buf, len2, 1, fp);
      *(buf+len2)='\0';
      printf("##EXAMPLE:'%s'##\n", buf);
      free(buf);
    }

    fclose(fp);
    //getchar();
  }
#endif
}

/*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*/
