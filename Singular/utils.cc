#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include "utils.h"

extern FILE *yylpin;
extern char *optarg;
extern int optind, opterr, optopt;
extern int lpverbose, check;
extern int found_version, found_info, found_oldhelp, found_proc_in_proc;
warning_info = 0, warning_version = 0;
/*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*/
void main_init(int argc, char *argv[])
{
  char c, *file=NULL;

  while((c=getopt(argc, argv, "d:sf:"))>=0) {
    
    switch(c)
      {
      case 'd':
	lpverbose = 1;
	sscanf(optarg, "%d", &lpverbose);
	break;
      case 'f': file = argv[optind-1];
	//	printf("opening:%d %s\n", optind, file);
	break;
      case 's':
	check++;
	break;
      case -1 : printf("no such option:%s\n", argv[optind]);
	break;
      default: printf("no such option.%x, %s\n", c&0xff, argv[optind]);
      }
  }

  if(file!=NULL)
    yylpin = fopen( file, "rb" );
  else
    yylpin = stdin;
}

/*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*/
void main_result(char *libname)
{
  if(!found_info)    printf("*** No info-string found!\n");
  if(!found_version) printf("*** No version-string found!\n");
  if(found_oldhelp)  printf("*** Library has stil OLD library-format.\n");
  if(found_info && warning_info)
    printf("*** INFO-string should come before every procedure definition.\n");
  if(found_version && warning_version)
    printf("*** VERSION-string should come before every procedure definition.\n");
}
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

  if(!found_info && !warning_info) warning_info++;
  if(!found_version && !warning_version) warning_version++;
  if(pi->data.s.body_end==0) 
    pi->data.s.body_end = pi->data.s.proc_end;

  printf( "%c %-15s  %20s ", pi->is_static ? 'l' : 'g', pi->libname,
	  pi->procname);
  printf("line %4d,%5ld-%-5ld  %4d,%5ld-%-5ld  %4d,%5ld-%-5ld\n",
	 pi->data.s.proc_lineno, pi->data.s.proc_start, pi->data.s.def_end,
	 pi->data.s.body_lineno, pi->data.s.body_start, pi->data.s.body_end,
	 pi->data.s.example_lineno, pi->data.s.example_start,
	 pi->data.s.proc_end);
  if(check) {
    if(!pi->is_static && (pi->data.s.body_start-pi->data.s.def_end)<4)
      printf("*** Procedure '%s' is global and has no help-section.\n",
	     pi->procname);
    if(!pi->is_static && !pi->data.s.example_start)
      printf("*** Procedure '%s' is global and has no example-section.\n",\
	     pi->procname);
    if(found_proc_in_proc)
      printf("*** found proc within procedure '%s'.\n", pi->procname);
  }

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
