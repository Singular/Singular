#include "kernel/mod2.h"

#ifdef STANDALONE_PARSER

#include "Singular/fegetopt.h"
#include "Singular/utils.h"
#include "Singular/libparse.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

EXTERN_VAR FILE *yylpin;
EXTERN_VAR char *optarg;
EXTERN_VAR int optind, opterr, optopt;
EXTERN_VAR int lpverbose, check;
EXTERN_VAR int texinfo_out;

EXTERN_VAR int category_out;

EXTERN_VAR int found_version, found_info, found_oldhelp, found_proc_in_proc;
VAR int warning_info = 0, warning_version = 0;

static void usage(char *progname)
{
  printf("libparse: a syntax-checker for Singular Libraries.\n");
  printf("USAGE: %s [options] singular-library\n", progname);
  printf("Options:\n");
  printf("   -f <singular library> : performs syntax-checks\n");
  printf("   -d [digit]            : digit=1,..,4 increases the verbosity of the checks\n");
  printf("   -s                    : turns on reporting about violations of unenforced syntax rules\n");
  printf("   -i                    : perl output of examples and help of procs\n");
  printf("   -c                    : print category of lib to stdout and exit\n");
  printf("   -h                    : print this message\n");
  exit(1);
}

STATIC_VAR char* lib_file = NULL;

/*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*/
void main_init(int argc, char *argv[])
{
  char c;

  while((c=fe_getopt(argc, argv, "ihdc:sf:"))>=0)
  {
    switch(c)
    {
        case 'd':
          lpverbose = 1;
          if(isdigit(argv[fe_optind-1][0])) sscanf(optarg, "%d", &lpverbose);
          else fe_optind--;
          break;
        case 'f': lib_file = argv[fe_optind-1];
          break;
        case 's':
          check++;
          break;
        case 'i':
          texinfo_out = 1;
          break;
        case 'c':
          category_out = 1;
          break;

        case 'h' :
          usage(argv[0]);
          break;
        case -1 : printf("no such option:%s\n", argv[fe_optind]);
          usage(argv[0]);
          break;
        default: printf("no such option.%x, %c %s\n", c&0xff, c, argv[fe_optind]);
          usage(argv[0]);
    }
  }
  if (texinfo_out || category_out) lpverbose = 0;

  if(lib_file!=NULL)
  {
    yylpin = fopen( lib_file, "rb" );
    if (! (texinfo_out || category_out))
      printf("Checking library '%s'\n", lib_file);
    else if (! category_out)
      printf("$library = \"%s\";\n", lib_file);
  }
  else
  {
    while(argc>fe_optind && yylpin==NULL)
    {
      yylpin = fopen( argv[fe_optind], "rb" );
      if(yylpin!=NULL)
      {
        lib_file = argv[fe_optind];
        if (! (texinfo_out || category_out) )
          printf("Checking library '%s'\n", argv[fe_optind]);
        else if (! category_out)
          printf("$library = \"%s\";\n", lib_file);
      }
      else fe_optind++;
    }
  }
  if(yylpin == NULL)
  {
    printf("No library found to parse.\n");
    usage(argv[0]);
  }
}

/*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*/
void main_result(char */*libname*/)
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
procinfo *iiInitSingularProcinfo(procinfo* pi, const char *libname,
                                 const char *procname, int line, long pos,
                                 BOOLEAN pstatic /*= FALSE*/)
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
  pi->data.s.help_chksum = 0;
  return(pi);
}

/*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*/
void pi_clear(procinfov pi)
{
  free(pi->libname);
  free(pi->procname);
  free(pi);
}

/*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*/

#ifndef SEEK_SET
#define SEEK_SET 0
#endif

static void PrintOut(FILE *fd, int pos_start, int pos_end)
{
  if (pos_start <= 0 || pos_end - pos_start <= 4) return;
  char c = 0;

  fseek(fd, pos_start, SEEK_SET);
  while (pos_start++ <= pos_end)
  {
    if (c == '\\')
    {
      c = fgetc(fd);
      if (c != '"') putchar('\\');
    }
    else
      c = fgetc(fd);
    if (c == '@' || c == '$') putchar('\\');
    if (c != '\r') putchar(c);
  }
  if (c == '\\') putchar('\\');
}


void printpi(procinfov pi)
{
  // char *buf, name[256];
  // int len1, len2;
  /* pi->libname is badly broken -- use file, instead */
  FILE *fp = fopen( lib_file, "rb");

  if (fp == NULL)
  {
    printf("Can not open %s\n", lib_file);
    return;
  }

  if(!found_info && !warning_info) warning_info++;
  if(!found_version && !warning_version) warning_version++;
  if(pi->data.s.body_end==0)
    pi->data.s.body_end = pi->data.s.proc_end;

  if (texinfo_out)
  {
   if ((! pi->is_static) &&
        (pi->data.s.body_start - pi->data.s.def_end > 10) &&
        (! found_proc_in_proc))
    {
      printf("push(@procs, \"%s\");\n", pi->procname);
      printf("$help{\"%s\"} = <<EOT;\n", pi->procname);
      PrintOut(fp, pi->data.s.help_start, pi->data.s.body_start-3);
      printf("\nEOT\n");
      if ((pi->data.s.example_start > 0) &&
          (pi->data.s.proc_end - pi->data.s.example_start > 10))
      {
        printf("$example{\"%s\"} = <<EOT;\n", pi->procname);
        PrintOut(fp, pi->data.s.example_start, pi->data.s.proc_end);
        printf("\nEOT\n");
      }
      printf("$chksum{\"%s\"} = %ld;\n", pi->procname, pi->data.s.help_chksum);
    }
  }
  else if (! category_out)
  {
    if(lpverbose) printf("//     ");
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
  }

  if (fp != NULL) fclose(fp);

}

/*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*/
#endif
