/*
 *
 */

#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <limits.h>

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

main( int argc, char *argv[] )
{
  int i;
  
  if ( argc > 1 )
     yyin = fopen( argv[1], "rb" );
  else {
    printf("no filename given\n");
    printf("usage: %s <filename>.mod\n", argv[0]);
    return 1;
  }
  
  if(init_modgen(&module_def, argv[1])) return 1;
  init_type_conv();
  do {
      i=yyparse();
      if(i)printf("NEXT LOOP at line %d (%s) %d\n", yylineno, yytext, i);
      else printf("FINISH? (%d)\n", i);
  }
  while (!i);
  printf("ENDE\n");
  
  //fflush(module_def.fmtfp);
  //PrintProclist(&module_def);
  //generate_mod(&module_def, 2);
  //mod_create_makefile(&module_def);
  if(module_def.fmtfp != NULL) fclose(module_def.fmtfp);
  if(module_def.modfp != NULL) fclose(module_def.modfp);
  if(module_def.modfp_h != NULL) fclose(module_def.modfp_h);
}
