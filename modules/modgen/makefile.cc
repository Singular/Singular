/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/* $Id: makefile.cc,v 1.3 2000-03-28 07:14:25 krueger Exp $ */
/*
* ABSTRACT: lib parsing
*/

#include <mod2.h>
#include <febase.h>
#include <grammar.h>
#include <ipid.h>
#include <ipshell.h>
#include <mmemory.h>
#include <structs.h>
#include <subexpr.h>
#include <tok.h>
#include <regex.h>

#include "modgen.h"
#include "typmap.h"
#include "pathnames.h"

extern void mod_create_makefile(moddefv module);
extern void build_head_section(FILE *fp, moddefv module);
extern void build_clean_section(FILE *fp, moddefv module);
extern void build_install_section(FILE *fp, moddefv module);
extern void build_compile_section(FILE *fp, moddefv module);

static char *object_name(char *p);
/*========================================================================*/
/*
  run mod_create_makefile();
   
 */
/*========================================================================*/


/*========================================================================*/
void mod_create_makefile(moddefv module)
{
  FILE *fp;
  fp = fopen("tmp/Makefile", "w");
  cfilesv cf = module->files;
  int i;
  
  printf("Creating Makefile  ...");fflush(stdout);
  write_header(fp, module->name, "#");
  build_head_section(fp, module);
  fprintf(fp, "SRCS\t= %s.cc", module->name);
  
  for(i=0; i<module->filecnt; i++)
    fprintf(fp, " %s", cf[i].filename);

  fprintf(fp, "\nOBJS\t= %s.o", module->name);
  for(i=0; i<module->filecnt; i++)
    fprintf(fp, " %s", object_name(cf[i].filename));
  fprintf(fp, "\nDOBJS\t= %s.og", module->name);
  for(i=0; i<module->filecnt; i++)
    fprintf(fp, " %s", object_name(cf[i].filename));

  fprintf(fp, "\n\n");
  build_compile_section(fp, module);
  build_clean_section(fp, module);
  build_install_section(fp, module);
  
  fprintf(fp, "\n");
  fprintf(fp, "\n");

  fclose(fp);
  printf("  done.\n");
}

/*========================================================================*/
void build_head_section(
  FILE *fp,
  moddefv module
  )
{
  fprintf(fp, "CC\t= gcc\n");
  fprintf(fp, "CXX\t= gcc\n");
  fprintf(fp, "CFLAGS\t= -DNDEBUG -DBUILD_MODULE -I. -I../../include\n");
  fprintf(fp, "DCFLAGS\t= -DBUILD_MODULE -I. -I../../include\n");
  fprintf(fp, "#LD\t=\n");
  fprintf(fp, "\n");
  fprintf(fp, "libdir          = %s\n", LIBDIR);
  fprintf(fp, "INSTALL\t\t= %s/Singular/install-sh -c\n", TOPSRCDIR);
  fprintf(fp, "INSTALL_PROGRAM\t= ${INSTALL}\n");
  fprintf(fp, "INSTALL_DATA\t= ${INSTALL} -m 644\n");
}

/*========================================================================*/
void build_clean_section(
  FILE *fp,
  moddefv module
  )
{
  fprintf(fp, "clean:\n");
  fprintf(fp, "\trm -f *.o *.og *.lo *.so* *.sl *.la *~ core\n\n");
  
  fprintf(fp, "distclean: clean\n");
  fprintf(fp, "\trm -f %s.cc %s.h Makefile\n\n", module->name, module->name);
}

/*========================================================================*/
void build_install_section(
  FILE *fp,
  moddefv module
  )
{
  fprintf(fp, "install:\n");
  fprintf(fp, "\t${MKINSTALLDIRS} ${bindir}\n");
  fprintf(fp, "\t${INSTALL_PROGRAM} %s.so ${libdir}/%s.so\n",
          module->name, module->name);
}

/*========================================================================*/
static char *object_name(char *p)
{
  char *q = (char *)strrchr(p, '.');
  if(q==NULL) return "";
  *q = '\0';
  char *r = (char *)malloc(strlen(p)+4);
  sprintf(r, "%s.o", p);
  
  *q = '.';
  return(r);
}

/*========================================================================*/
/*===  Machine depend Makefile creation                                ===*/
/*========================================================================*/
#ifdef ix86_Linux
void build_compile_section(
  FILE *fp,
  moddefv module
  )
{
  fprintf(fp, "all:\t%s.so %s_g.so \n", module->name, module->name);
  fprintf(fp, "\n");
  fprintf(fp, "%%.o: %%.cc Makefile\n");
  fprintf(fp, "\t${CC} ${CFLAGS} -c -fPIC -DPIC $< -o $*.o\n");
  fprintf(fp, "\n");
  fprintf(fp, "%%.og: %%.cc Makefile\n");
  fprintf(fp, "\t${CC} ${DCFLAGS} -c -fPIC -DPIC $< -o $*.og\n");
  fprintf(fp, "\n");
  
  fprintf(fp, "%s.so: ${OBJS}\n", module->name);
  fprintf(fp, "\t${CC} ${CFLAGS} -shared -Wl,-soname -Wl,%s.so.%d \\\n",
          module->name, module->major);
  fprintf(fp, "\t\t-o %s.so.%d.%d.%d ${OBJS}\n", module->name,
          module->major, module->minor, module->level);
  fprintf(fp, "\trm -f %s.so\n", module->name);
  fprintf(fp, "\tln -s %s.so.%d.%d.%d %s.so\n", module->name, module->major,
          module->minor, module->level, module->name);
  fprintf(fp, "\n");

  fprintf(fp, "%s_g.so: ${DOBJS}\n", module->name);
  fprintf(fp, "\t${CC} ${DCFLAGS} -shared -Wl,-soname -Wl,%s_g.so.%d \\\n",
          module->name, module->major);
  fprintf(fp, "\t\t-o %s_g.so.%d.%d.%d ${DOBJS}\n", module->name,
          module->major, module->minor, module->level);
  fprintf(fp, "\trm -f %s_g.so\n", module->name);
  fprintf(fp, "\tln -s %s_g.so.%d.%d.%d %s_g.so\n", module->name, 
          module->major, module->minor, module->level, module->name);
  fprintf(fp, "\n");
}
#endif /* ix86_Linux */

/*========================================================================*/
#if defined(HPUX_9) || defined(HPUX_10)
void build_compile_section(
  FILE *fp,
  moddefv module
  )
{
  fprintf(fp, "all:\t%s.sl\n", module->name);
  fprintf(fp, "\n");
  fprintf(fp, "%%.o: %%.cc Makefile\n");
  fprintf(fp, "\t${CC} ${CFLAGS} -c -fPIC -DPIC $< -o $*.o\n");
  fprintf(fp, "\n");
  fprintf(fp, "%s.sl: ${OBJS}\n", module->name);
  fprintf(fp, "\t${LD} -b -o %s.sl \\\n", module->name);
  fprintf(fp, "\t\t${OBJS}\n");
}

#endif /* HPUX_9  or HPUX_10 */

/*========================================================================*/
#  ifdef m68k_MPW
void build_compile_section(
  FILE *fp,
  moddefv module
  )
{
  fprintf(fp, "all:\t\n");
  fprintf(fp, "\techo \"don't know how to build library\"\n");
}
#  endif /* 68k_MPW */

/*========================================================================*/
#  ifdef AIX_4
void build_compile_section(
  FILE *fp,
  moddefv module
  )
{
  fprintf(fp, "all:\t\n");
  fprintf(fp, "\techo \"don't know how to build library\"\n");
}
#  endif /* AIX_4 */

/*========================================================================*/
#  ifdef IRIX_6
void build_compile_section(
  FILE *fp,
  moddefv module
  )
{
  fprintf(fp, "all:\t\n");
  fprintf(fp, "\techo \"don't know how to build library\"\n");
}
#  endif /* IRIX_6 */

/*========================================================================*/
#  ifdef Sun3OS_4
void build_compile_section(
  FILE *fp,
  moddefv module
  )
{
  fprintf(fp, "all:\t\n");
  fprintf(fp, "\techo \"don't know how to build library\"\n");
}
#  endif /* Sun3OS_4 */

/*========================================================================*/
#  if defined(SunOS_4) || defined(SunOS_5)
void build_compile_section(
  FILE *fp,
  moddefv module
  )
{
  fprintf(fp, "all:\t\n");
  fprintf(fp, "\techo \"don't know how to build library\"\n");
}
#  endif /* SunOS_4 or SunOS_5 */

/*========================================================================*/
#  ifdef ix86_win
void build_compile_section(
  FILE *fp,
  moddefv module
  )
{
  fprintf(fp, "all:\t\n");
  fprintf(fp, "\techo \"don't know how to build library\"\n");
}
#  endif /* ix86_win */

/*========================================================================*/
#  ifdef ppc_MPW
void build_compile_section(
  FILE *fp,
  moddefv module
  )
{
  fprintf(fp, "all:\t\n");
  fprintf(fp, "\techo \"don't know how to build library\"\n");
}
#  endif /* ppc_MPW */

/*========================================================================*/
