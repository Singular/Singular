/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/*
* ABSTRACT: lib parsing
*/

#include "modgen.h"
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>


EXTERN_VAR int yylineno;
EXTERN_VAR int do_create_srcdir;

void enter_id(FILE *fp, char *name, char *value);
/*========================================================================*/
void  mod_copy_tmp(
  FILE *fp_out,
  FILE *fp_in
  )
{
  int nread;
  char buf[1024];

  fseek(fp_in, 0L, 0);
  do
  {
    memset(buf, 0, sizeof(buf));
    fgets(buf, sizeof(buf), fp_in);
    modlineno++;
    //nread = fread (buf, sizeof (char), sizeof(buf)-1, fp_in);
    if(strncmp(buf, "#line @d ", 8)==0)
    {
      buf[6]='%';
      fprintf(fp_out, buf, modlineno);
    }
    else
      fwrite(buf, sizeof(char), strlen(buf)/*nread*/, fp_out);
  } while(!feof(fp_in));
  fputs("\n", fp_out);
}

/*========================================================================*/
void write_enter_id(FILE *fp)
{
  fprintf(fp, "\ntypedef int idtyp;\n");
  fprintf(fp, "\nidhdl enter_id(char *name, char *value, idtyp t)\n");
  fprintf(fp, "{\n");
  fprintf(fp, "  idhdl h;\n");
  fprintf(fp, "\n");
  fprintf(fp, "  h=enterid(name,0, t, &(currPack->idroot), TRUE/*FALSE*/);\n");
  fprintf(fp, "  if(h!=NULL)\n");
  fprintf(fp, "  {\n");
  fprintf(fp, "    switch(t)\n");
  fprintf(fp, "    {\n");
  fprintf(fp, "      case STRING_CMD: \n");
  fprintf(fp, "        omFree(IDSTRING(h));\n");
  fprintf(fp, "        IDSTRING(h) = omStrDup(value);\n");
  fprintf(fp, "        break;\n");
  fprintf(fp, "      case PACKAGE_CMD: break;\n");
  fprintf(fp, "      case PROC_CMD: break;\n");
  fprintf(fp, "    }\n");
  fprintf(fp, "  }\n");
  fprintf(fp, "  else\n");
  fprintf(fp, "    Warn(\"Cannot create '%%s'\\n\", name);\n");
  fprintf(fp, "  return(h);\n");
  fprintf(fp, "}\n");
  modlineno+=19;
}

/*========================================================================*/
 void write_add_singular_proc(FILE *fp)
 {
   fprintf(fp, "\nidhdl add_singular_proc(FILE* binfp, char *procname,int line,\n");
   fprintf(fp, "                       long pos, long end, BOOLEAN pstatic)\n");
   fprintf(fp, "{\n");
   fprintf(fp, "  idhdl h;\n");
   fprintf(fp, "  procinfov pi;\n");
   fprintf(fp, "  char *tempstr;\n");
   fprintf(fp, "  h = enter_id(procname, NULL, PROC_CMD);\n");
   fprintf(fp, "  if(h == NULL) return NULL;\n");
   fprintf(fp, "\n");
   fprintf(fp, "  pi = IDPROC(h);\n");
   fprintf(fp, "\n");
   fprintf(fp, "  pi->libname = omStrDup(currPack->libname);\n");
   fprintf(fp, "  pi->procname = omStrDup(procname);\n");
   fprintf(fp, "  pi->language = LANG_SINGULAR;\n");
   fprintf(fp, "  pi->ref = 1;\n");
   fprintf(fp, "  pi->is_static = pstatic;\n");
   fprintf(fp, "  pi->data.s.proc_start = pos;\n");
   fprintf(fp, "  pi->data.s.def_end    = pos;\n");
   fprintf(fp, "  pi->data.s.help_start = 0L;\n");
   fprintf(fp, "  pi->data.s.help_end   = 0L;\n");
   fprintf(fp, "  pi->data.s.body_start = pos;\n");
   fprintf(fp, "  pi->data.s.body_end   = end;\n");
   fprintf(fp, "  pi->data.s.proc_end   = end;\n");
   fprintf(fp, "  pi->data.s.example_start = 0L;\n");
   fprintf(fp, "  pi->data.s.proc_lineno = line;\n");
   fprintf(fp, "  pi->data.s.body_lineno = line;\n");
   fprintf(fp, "  pi->data.s.example_lineno = 0;\n");
   fprintf(fp, "  pi->data.s.help_chksum = 0;\n");
   fprintf(fp, "  \n");
   fprintf(fp, "  rewind(binfp);\n");
   fprintf(fp, "  fseek(binfp,pos,SEEK_CUR);\n");
   fprintf(fp, "  tempstr=(char *)omAlloc(end-pos+2);\n");
   fprintf(fp, "  memset(tempstr,0,end-pos+2);\n");
   fprintf(fp, "  fread(tempstr,sizeof(char),end-pos+1,binfp);\n");
   fprintf(fp, "  pi->data.s.body = omStrDup(tempstr);\n");
   fprintf(fp, "  omFree(tempstr);\n\n");
   fprintf(fp, "  return(h);\n");
   fprintf(fp, "}\n");
   modlineno+=40;
 }

/*========================================================================*/
void write_mod_init( moddefv module, FILE *fp)
{
  fprintf(fp, "\n\n");
  fprintf(fp, "#line @d \"%s.cc\"\n", module->name);
  fprintf(fp, "extern \"C\" {\n");
#if 0
  fprintf(fp, "int mod_init(\n  int (*iiAddCproc)(");
#if 1
  fprintf(fp, "char *libname, char *procname, BOOLEAN pstatic,\n");
  fprintf(fp, "              BOOLEAN(*func)(leftv res, leftv v)");
#endif
  fprintf(fp, ")\n  )\n{\n");
#else
  fprintf(fp, "int mod_init(\n  SModulFunctions* psModulFunctions");
  fprintf(fp, "\n  )\n{\n");
#endif
  fprintf(fp, "  idhdl h;\n");
  fprintf(fp, "  char * tempstr;\n");
  fprintf(fp, "  char * tailstr;\n");
  fprintf(fp, "  FILE * binfp; \n");
  fprintf(fp, "  int ret;\n");
  fprintf(fp, "  struct stat sb; \n\n");
  fprintf(fp, "  tempstr = (char *)omAlloc(strlen(currPack->libname)+5);\n");
  fprintf(fp, "  memset(tempstr,0,strlen(currPack->libname)+5);\n");
  fprintf(fp, "  tailstr = strrchr(currPack->libname,'.');\n");
  fprintf(fp, "  memcpy(tempstr,currPack->libname,strlen(currPack->libname)-strlen(tailstr));\n");
  fprintf(fp, "  memcpy(tempstr+strlen(tempstr),\".bin\",4);\n");
  fprintf(fp, "  ret=stat(tempstr,&sb);\n");
  fprintf(fp, "  if(ret==0)\n");
  fprintf(fp, "  {\n");
  fprintf(fp, "    if ((sb.st_mode & S_IFMT) == S_IFREG)\n");
  fprintf(fp, "    {\n");
  fprintf(fp, "      if (crccheck(tempstr)!=crcsum)\n");
  fprintf(fp, "      {   Warn(\"file %%s does not agree with module version - ignoring file\",tempstr);\n");
  fprintf(fp, "          ret=-1;\n      }\n");
  fprintf(fp, "      if ((binfp = fopen(tempstr,\"r\")) == NULL) return -1;\n");
  fprintf(fp, "    } \n    else \n      ret=-1; \n  }\n\n");
  fprintf(fp, "  fill_help_package();\n");
  fprintf(fp, "  fill_example_package();\n\n");
}

/*========================================================================*/
int write_intro(
    moddefv module
    )
{
  char filename[512];

  if(do_create_srcdir) mkdir(module->name, 0755);
  strcpy(filename, build_filename(module, module->name, 1));

  fflush(module->fmtfp);

  if( (module->modfp = fopen(filename, "w")) == NULL) {
    //free(filename);
    return -1;
  }
  if(trace)printf("Creating %s, ", filename);fflush(stdout);
  mod_write_header(module->modfp, module->name, 'c');
  mod_copy_tmp(module->modfp, module->fmtfp);
  if(trace)printf("\n");fflush(stdout);
  fclose(module->fmtfp); module->fmtfp = NULL;
  if(create_tmpfile(module)) return -1;
  if(create_tmpfile(module, 1)) return -1;
  if(create_tmpfile(module, 2)) return -1;
  if(module->fmtfp2 == NULL) { printf("Cannot write HELP\n"); return -1; }
  if(module->fmtfp3 == NULL) { printf("Cannot write EXAMPLE\n"); return -1; }

  strcpy(filename, build_filename(module, module->name, 2));
  //sprintf(filename, "%s/%s.h", module->name, module->name);
  if( (module->modfp_h = fopen(filename, "w")) == NULL) {
    //free(filename);
    return -1;
  }
  if(trace)printf("Creating %s, ", filename);fflush(stdout);
  mod_write_header(module->modfp_h, module->name, 'h');
  if(trace)printf("\n");fflush(stdout);

  //free(filename);
//  write_enter_id(module->modfp);
  return 0;
}
