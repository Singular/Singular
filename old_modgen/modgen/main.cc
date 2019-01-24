/*
 *
 */

#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <unistd.h>

#include "config.h"

#ifdef HAVE_GETOPT_LONG
#include <getopt.h>
#else
#include "my_getopt.h"
#endif

#include "modgen.h"
#include "pathnames.h"
#include "stype.h"

/*
 *    Syntax of a module file:
 *    %{
 *    //here may come c/C++ code
 *    %}
 *    // start  of section 1
 *    module="module-name";
 *    version="";
 *    info="";
 *
 *    cxxsource=misc.cc
 *
 *    %% start of section 2
 *
 *    proc [<return-type>] <procname>[(argtypelist)] {
 *
 *    }
 *
 *    %% start of section 3
 *    // some other c/C++ code
 *
 */

/*
 *   How does the module generator works:
 *   1.    start parsing the module-definition file
 *   1.1   write c/C++  code into a temporary file
 *   1.2   get modulename in section 1
 *   2     arrived at the beginnig of section
 *   2.1   write header of .cc and .h files
 *   2.2   append tmpfile to .cc file
 *   2.3   parse section 2
 *   3.    generate code for each proc definition
 *   4.    append section 3 to .cc file
 *   5.    generate Makefile
 *   6.    finished
 */

EXTERN_VAR FILE* yyin;
EXTERN_VAR int   yylineno;
EXTERN_VAR char *yytext;

INST_VAR moddef module_def;

extern int iiInitArithmetic();
int init_modgen(
  moddefv module_def,
  char *filename
  );

extern int yyparse (void);
extern void init_type_conv();
VAR int debug = 0;
VAR int trace = 0;
VAR int do_create_makefile = 1;
VAR int do_create_srcdir = 1;
VAR char* inst_dir = "${SINGULARROOT}/${SINGUNAME}";

#ifdef IRIX_6
struct option
{
  const char *name;
  /* has_arg can't be an enum because some compilers complain about
     type mismatches in all the code that assumes it is an int.  */
  int has_arg;
  int *flag;
  int val;
};
#endif
STATIC_VAR struct option long_options[] =
{
  {"debug", 0, 0, 'd'},
  {"help", 0, 0, '?'},
  {"install-dir",1,0,'i'},
  {"nocreate-makefile", 0, 0, 'm'},
  {"nocreate-srcdir", 0, 0, 's'},
  {"verbose", 0, 0, 'v'},
  {0, 0, 0, 0}
};


void usage(char *name)
{
  int i;
  printf("usage: %s [args] filename\n", name);
  for(i=0; long_options[i].name != NULL; i++)
  {
    if(long_options[i].has_arg!=0)
    {
      switch(long_options[i].val) {
         case 'i':
                   printf("\t-%c (--%s) %s\n", long_options[i].val,
                      long_options[i].name, "<destination-dir>");
                   break;
         default:
                   printf("\t-%c (--%s) %s\n", long_options[i].val,
                      long_options[i].name, "<argument>");
      }
    } else
    {
      printf("\t-%c (--%s) %s\n", long_options[i].val,
                 long_options[i].name, "");
    }
  }
}

main( int argc, char *argv[] )
{
  int i;
  int c;
  int option_index = 0;
  unsigned long cksm;

#ifdef IRIX_6
  while( (c=getopt (argc,argv, "dmvsi:")) != -1)
#else
  while( (c=getopt_long (argc, argv, "dmvsi:",
                         long_options, &option_index))>=0)
#endif
 {
    switch (c)
    {
        case 'd' : debug++; break;
        case 'v' : trace=1; break;
        case 'm' : do_create_makefile = 0; break;
        case 'i' : inst_dir=optarg; break;
        case 's' : do_create_srcdir = 0; break;

        case '?' : usage(argv[0]);
          return 0;
          break;

        default:
          printf ("?? getopt returned character code 0%o ??\n", c);
    }
  }

  if (optind < argc) yyin = fopen( argv[optind], "rb" );
  else {
    printf("no filename given\n");
    usage(argv[0]);
    return 1;
  }

  iiInitArithmetic();
  if(init_modgen(&module_def, argv[optind])) return 1;
  init_type_conv();
  do {
      i=yyparse();
      switch(i) {
          case 0:
            printf("FINISH? %s (%d)\n", yytext, yylineno); break;
          case 1:
            if(debug>2)
              printf("NEXT LOOP at line %d (%s) %d\n", yylineno, yytext, i);
            if(strlen(yytext))
              printf("something went wrong at line %d\n", yylineno);
            break;
          default:
            break;
      }
  }
  while (!i);
  if(trace)printf("files for module '%s' created.\n", argv[optind]);

  //fflush(module_def.fmtfp);
  //PrintProclist(&module_def);
  //mod_create_makefile(&module_def);
  if(module_def.fmtfp   != NULL) fclose(module_def.fmtfp);
  if(module_def.modfp   != NULL) fclose(module_def.modfp);
  if(module_def.docfp   != NULL) {
    fprintf(module_def.docfp,"1;");
    fclose(module_def.docfp);
  }
  if(module_def.binfp   != NULL) {
    //we have been writing to it previously
    fclose(module_def.binfp);
    cksm = crccheck(build_filename(&module_def,module_def.targetname,3));
  } else {
    //we have not been writing to it or we could not open it
    if((module_def.binfp=fopen(
             build_filename(&module_def,module_def.targetname,3),"w"))!=NULL) {
      fprintf(module_def.binfp,"// no Singular procedure for this module\n");
      fclose(module_def.binfp);
      cksm = crccheck(build_filename(&module_def,module_def.targetname,3));
    } else {
      printf("Cannot open .bin file!!\n");
    }
  }
  if(module_def.modfp_h != NULL) {
    fprintf(module_def.modfp_h,"unsigned long crcsum=%lu;\n",cksm);
    write_crctable(module_def.modfp_h);
    fclose(module_def.modfp_h);
  }
  return 0;
}
