/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/*
* ABSTRACT: lib parsing
*/

#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>

#include <regex.h>

#include "modgen.h"
#include "typmap.h"
#include "pathnames.h"

EXTERN_VAR char* inst_dir;

extern void mod_create_makefile(moddefv module);
extern void build_head_section(FILE *fp, moddefv module);
extern void build_clean_section(FILE *fp, moddefv module);
extern void build_install_section(FILE *fp, moddefv module);
extern void build_compile_section(FILE *fp, moddefv module);

EXTERN_VAR int do_create_srcdir;

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

  if(module->targetname==NULL)
  {
     module->targetname = (char *)malloc(strlen(module->name)+1);
     memset(module->targetname, '\0', strlen(module->name)+1);
     memcpy(module->targetname,module->name,strlen(module->name));
  }
  if(do_create_srcdir) mkdir(module->name, 0755);
  fp = fopen(build_filename(module, "Makefile", 0), "w");
  cfilesv cf = module->files;
  int i;

  if(trace)printf("Creating Makefile  ...");fflush(stdout);
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
  if(trace)printf("  done.\n");
}

/*========================================================================*/
void build_head_section(
  FILE *fp,
  moddefv module
  )
{
  fprintf(fp, "CC\t= gcc\n");
  fprintf(fp, "CXX\t= gcc\n");
  fprintf(fp, "SINGULARROOT\t= ../..\n");
  fprintf(fp, "SINGUNAME\t= %s\n",SINGUNAME);
#warning "PROBLEM: nice place for include file has to be found"
  fprintf(fp, "CFLAGS\t= -DSING_NDEBUG -DBUILD_MODULE -I. -I${SINGULARROOT} -I${SINGULARROOT}/${SINGUNAME}/include\n");
  fprintf(fp, "DCFLAGS\t= -DBUILD_MODULE -I. -I${SINGULARROOT} -I${SINGULARROOT}/${SINGUNAME}/include\n");
  fprintf(fp, "#LD\t=\n");
  fprintf(fp, "\n");
  fprintf(fp, "instdir          = %s\n", inst_dir );
  fprintf(fp, "MKINSTALLDIRS\t\t= ${SINGULARROOT}/dyn_modules/mkinstalldirs\n");
#warning "PROBLEM: do we also install install-sh when installing Singular?"
  fprintf(fp, "INSTALL\t\t= ${SINGULARROOT}/Singular/install-sh -c\n");
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
  fprintf(fp, "\trm -f %s.cc %s.h Makefile *.bin *.pl\n\n",
               module->name, module->name);
}

/*========================================================================*/
void build_install_section(
  FILE *fp,
  moddefv module
  )
{
  fprintf(fp, "install bindist: all\n");
  fprintf(fp, "\t${MKINSTALLDIRS} ${instdir}\n");
  fprintf(fp, "\t${MKINSTALLDIRS} ${instdir}/MOD\n");
  fprintf(fp, "\t${INSTALL_PROGRAM} %s.so ${instdir}/MOD/%s.so\n",
          module->targetname, module->targetname);
  fprintf(fp, "\t${INSTALL_PROGRAM} %s.bin ${instdir}/MOD/%s.bin\n",
          module->targetname, module->targetname);
  fprintf(fp, "install-bindist: all\n");
  fprintf(fp, "\t${MKINSTALLDIRS} ${install_bindir}\n");
  fprintf(fp, "\t${INSTALL_PROGRAM} %s.so ${install_bindir}/%s.so\n",
          module->targetname, module->targetname);
  fprintf(fp, "\t${INSTALL_PROGRAM} %s.bin ${install_bindir}/%s.bin\n",
          module->targetname, module->targetname);
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
// relying on gcc to define __ELF__, check with cpp -dM /dev/null
#if defined(__ELF__)
#define HAVE_ELF_SYSTEM
#endif

#if defined(HAVE_ELF_SYSTEM)
void build_compile_section(
  FILE *fp,
  moddefv module
  )
{
  fprintf(fp, "all:\t%s.so %s_g.so \n", module->targetname, module->targetname);
  fprintf(fp, "\n");
  fprintf(fp, "%%.o: %%.cc Makefile\n");
  fprintf(fp, "\t${CC} ${CFLAGS} -c -fPIC -DPIC $< -o $*.o\n");
  fprintf(fp, "\n");
  fprintf(fp, "%%.og: %%.cc Makefile\n");
  fprintf(fp, "\t${CC} ${DCFLAGS} -g -c -fPIC -DPIC $< -o $*.og\n");
  fprintf(fp, "\n");

  fprintf(fp, "%s.so: ${OBJS}\n", module->targetname);
  fprintf(fp, "\t${CC} ${CFLAGS} -shared -Wl,-soname -Wl,%s.so.%d \\\n",
          module->targetname, module->major);
  fprintf(fp, "\t\t-o %s.so.%d.%d.%d ${OBJS}\n", module->targetname,
          module->major, module->minor, module->level);
  fprintf(fp, "\trm -f %s.so\n", module->targetname);
  fprintf(fp, "\tln -s %s.so.%d.%d.%d %s.so\n", module->targetname, module->major,
          module->minor, module->level, module->targetname);
  fprintf(fp, "\n");

  fprintf(fp, "%s_g.so: ${DOBJS}\n", module->targetname);
  fprintf(fp, "\t${CC} ${DCFLAGS} -shared -Wl,-soname -Wl,%s_g.so.%d \\\n",
          module->targetname, module->major);
  fprintf(fp, "\t\t-o %s_g.so.%d.%d.%d ${DOBJS}\n", module->targetname,
          module->major, module->minor, module->level);
  fprintf(fp, "\trm -f %s_g.so\n", module->targetname);
  fprintf(fp, "\tln -s %s_g.so.%d.%d.%d %s_g.so\n", module->targetname,
          module->major, module->minor, module->level, module->targetname);
  fprintf(fp, "\n");
}

/*========================================================================*/
#else
#if defined(HPUX_9) || defined(HPUX_10)
void build_compile_section(
  FILE *fp,
  moddefv module
  )
{
  fprintf(fp, "all:\t%s.sl\n", module->targetname);
  fprintf(fp, "\n");
  fprintf(fp, "%%.o: %%.cc Makefile\n");
  fprintf(fp, "\t${CC} ${CFLAGS} -c -fPIC -DPIC $< -o $*.o\n");
  fprintf(fp, "\n");
  fprintf(fp, "%s.sl: ${OBJS}\n", module->targetname);
  fprintf(fp, "\t${LD} -b -o %s.sl \\\n", module->targetname);
  fprintf(fp, "\t\t${OBJS}\n");
}

#else /* HPUX_9  or HPUX_10 */
/*========================================================================*/
void build_compile_section(
  FILE *fp,
  moddefv module
  )
{
  fprintf(fp, "all:\t\n");
  fprintf(fp, "\techo \"don't know how to build library\"\n");
}
#endif
#endif

//#  ifdef ix86_Win
//void build_compile_section(
//  FILE *fp,
//  moddefv module
//  )
//{
//  fprintf(fp, "all:\t%s.dll %s_g.dll \n", module->name, module->name);
//  fprintf(fp, "\n");
//  fprintf(fp, "%%.o: %%.cc Makefile\n");
//  fprintf(fp, "\t${CC} ${CFLAGS} -c $< -o $*.o\n");
//  fprintf(fp, "\n");
//  fprintf(fp, "%%.og: %%.cc Makefile\n");
//  fprintf(fp, "\t${CC} ${DCFLAGS} -c $< -o $*.og\n");
//  fprintf(fp, "\n");
//
//  fprintf(fp, "%s.dll: ${OBJS}\n", module->name);
//  fprintf(fp, "\t${CC} ${CFLAGS} -Wl,--out-implib,lib%s.import.a -shared \\\n",
//          module->name);
//  fprintf(fp, "\t\t-o %s.dll ${OBJS}\n", module->name);
//  fprintf(fp, "\n");
//
//  fprintf(fp, "%s_g.so: ${DOBJS}\n", module->name);
//  fprintf(fp, "\t${CC} ${DCFLAGS} -Wl,--out-implib,lib%s_g.import.a -shared \\\n",
//          module->name);
//  fprintf(fp, "\t\t-o %s_g.dll ${DOBJS}\n", module->name);
//  fprintf(fp, "\n");
//
//  fprintf(fp, "all:\t\n");
//  fprintf(fp, "\techo \"don't know how to build library\"\n");
//}
//#  endif /* ix86_Win */

