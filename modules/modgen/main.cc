/*
 *
 */

#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <mylimits.h>
#include <unistd.h>
#include <getopt.h>

#include "modgen.h"
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

extern FILE* yyin;
extern int   yylineno;
extern char *yytext;

moddef module_def;

int init_modgen(
  moddefv module_def,
  char *filename
  );

extern int yyparse (void);
extern void init_type_conv();
int debug = 0;
int trace = 0;
int do_create_makefile = 1;
char* inst_dir = ".";

static struct option long_options[] =
{
  {"debug", 0, 0, 'd'},
  {"verbose", 0, 0, 'v'},
  {"nocreate-makefile", 0, 0, 'm'},
  {"install-dir",1,0,'i'},
  {"help", 0, 0, '?'},
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

  while( (c=getopt_long (argc, argv, "dmvi:",
                         long_options, &option_index))>=0) {
    switch (c)
    {
        case 'd' : debug++; break;
        case 'v' : trace=1; break;
        case 'm' : do_create_makefile = 0; break;
	case 'i' : inst_dir=optarg; break;
          
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
  if(module_def.modfp_h != NULL) fclose(module_def.modfp_h);
  if(module_def.binfp   != NULL) fclose(module_def.binfp);
}
