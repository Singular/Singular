/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/*
* ABSTRACT: lib parsing
*/

#include <kernel/mod2.h>
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

#define NEW_PARAM 1

#define SYSTYP_NONE   0
#define SYSTYP_LINUX  1
#define SYSTYP_HPUX9  2
#define SYSTYP_HPUX10 3

#if 1
#  define logx printf
#else
#  define logx
#endif

VAR char *DYNAinclude[] = {
   "",
   "#include <dlfcn.h>",
   "#include <dl.h>",
   "#include <dl.h>",
   "#include <>",
   "#include <>",
   "#include <>",
   NULL
};

VAR int systyp = SYSTYP_NONE;

VAR BOOLEAN    expected_parms;
VAR int        cmdtok;
VAR BOOLEAN siq=FALSE;
VAR char *lastreserved=NULL;
#define SELF_CMD MAX_TOK+1

void enter_id(FILE *fp, char *name, char *value);
void mod_write_enter_id(FILE *fp);


/*=============== types =====================*/
struct sValCmdTab
{
  short cmd;
  short start;
};

#include "iparith.inc"

/*=================== general utilities ============================*/
int IsCmd(char *n, int & tok)
{
  int an=1;
  int i,v;
  int en=LAST_IDENTIFIER;

  if( strcmp(n, "SELF") == 0) {
    tok = SELF_CMD;
    logx("IsCmd: [%d] %s\n", tok, n);
    return tok;
  }

  if( strcmp(n, "none") == 0) {
    tok = NONE;
    logx("IsCmd: [%d] %s\n", tok, n);
    return tok;
  }

  loop
  {
    if(an>=en-1)
    {
      if (strcmp(n, cmds[an].name) == 0)
      {
        i=an;
        break;
      }
      else if (strcmp(n, cmds[en].name) == 0)
      {
        i=en;
        break;
      }
      else
      {
        return 0;
      }
    }
    i=(an+en)/2;
    v=strcmp(n,cmds[i].name);
    if(v<0)
    {
      en=i;
    }
    else if(v>0)
    {
      an=i;
    }
    else /*v==0*/
    {
      break;
    }
  }
  lastreserved=cmds[i].name;
  tok=cmds[i].tokval;
  if(cmds[i].alias==2)
  {
    printf("outdated identifier `%s` used - please change your code",
    cmds[i].name);
    cmds[i].alias=1;
  }
#if 0
  if (!expected_parms)
  {
    switch (tok)
    {
      case IDEAL_CMD:
      case INT_CMD:
      case INTVEC_CMD:
      case MAP_CMD:
      case MATRIX_CMD:
      case MODUL_CMD:
      case POLY_CMD:
      case PROC_CMD:
      case RING_CMD:
      case STRING_CMD:
        cmdtok = tok;
        break;
    }
  }
#endif
  logx("IsCmd: [%d] %s\n", tok, n);

  if( (cmds[i].toktype==ROOT_DECL) ||
      (cmds[i].toktype==ROOT_DECL_LIST) ||
      (cmds[i].toktype==RING_DECL) ||
      ((cmds[i].toktype>=DRING_CMD) && (cmds[i].toktype<=VECTOR_CMD)))
    return cmds[i].toktype;
  return 0;
}

char * decl2str(int n, char *name)
{
  switch(n)
    {
#   include "decl.inc"

    /* first and last entry of tok.h cannot be grepped */
      //case ATTRIB_CMD: strcpy(name,"ATTRIB_CMD"); break;
    case MAX_TOK: strcpy(name,"MAX_TOK");       break;
    default: strcpy(name,"(null)");
  }
#if 0
  printf("[%d=%s]", n, name);
  fflush(stdout);
#endif
  return(name);
}

#if 0
static int iiTabIndex(const jjValCmdTab dArithTab, const int len, const int op)
{
  int a=0;
  int e=len;
  int p=len/2;
  while ( a!=e)
  {
     if (op==dArithTab[p].cmd) return dArithTab[p].start;
     else if (op<dArithTab[p].cmd)
     { e=p; p=a+(e-a)/2;}
     else
     { a=p; p=p+(e-p)/2; }
  }
  return -1;
}
#endif

/*========================================================================*/
void PrintProc(
  procdef pi
  )
{
  int i;

  printf("proc: %s(", pi.procname);

  for(i=0; i<pi.paramcnt; i++) {
    printf("%s", pi.param[i].name);
    if(i < (pi.paramcnt-1)) printf(",");
  }
  printf(")\n");
}

/*========================================================================*/
void make_version(char *p, moddefv module)
{
  char ver[10];
  char date[16];
  char libnamebuf[128];

  module->major = 0;
  module->minor = 0;
  module->level = 0;

  ver[0]='0'; ver[1]='.'; ver[2]='0'; ver[3]='.'; ver[4]='0'; ver[5]='\0';
  date[0]='?'; date[1]='\0';
  //if(what) sscanf(p,"%*[^=]= %*s %*s %10s %16s",ver,date);
  sscanf(p,"%*s %*s %10s %16s",ver,date);
  sscanf(ver, "%d.%d.%d", &module->major, &module->minor, &module->level);

  sprintf(libnamebuf,"(%s,%s)", ver, date);
  if(strcmp(libnamebuf, "(0.0.0,?)")==0)
  {
    sscanf(p,"%*[^\"]\"%[^\"]\"",libnamebuf);
  }
  module->revision = (char *)malloc(strlen(libnamebuf)+1);
  memset(module->revision, '\0', strlen(libnamebuf)+1);
  memcpy(module->revision, libnamebuf, strlen(libnamebuf));
}

/*========================================================================*/
void Add2files(
  moddefv module,
  char *name
  )
{
  cfiles cfnew;
  memset((void *)&cfnew, '\0', sizeof(cfiles));

  cfnew.filename = (char *)malloc(strlen(name)+1);
  memset(cfnew.filename, '\0', strlen(name)+1);
  memcpy(cfnew.filename, name, strlen(name));

  if(module->filecnt==0) {
    module->files = (cfilesv)malloc(sizeof(cfiles)+1);
  }
  else {
    module->files = (cfilesv)realloc(module->files,
                                   (module->filecnt+1)*sizeof(cfiles));
  }
  if(module->files == NULL) { printf("ERROR\n"); return; }

  memset((void *) &module->files[module->filecnt], '\0', sizeof(cfiles));
  memcpy((void *)(&(module->files[module->filecnt])),
         (void *)&cfnew, sizeof(cfiles));
  (module->filecnt)++;
}

/*========================================================================*/
void Add2proclist(
  moddefv module,
  char *name,
  char *ret_val,
  char *ret_typname,
  int ret_typ
  )
{
  procdef pnew;
  logx("Add2proclist(%s, %s)\n", name, ret_val);

  memset((void *)&pnew, '\0', sizeof(procdef));

  pnew.procname = (char *)malloc(strlen(name)+1);
  if(pnew.procname==NULL) printf("Error 1\n");
  memset(pnew.procname, '\0', strlen(name)+1);
  memcpy(pnew.procname, name, strlen(name));

  pnew.funcname = (char *)malloc(strlen(name)+1);
  if(pnew.funcname==NULL) printf("Error 1\n");
  memset(pnew.funcname, '\0', strlen(name)+1);
  memcpy(pnew.funcname, name, strlen(name));

  pnew.param = NULL;
  (pnew).is_static = 0;
  (pnew).paramcnt = 0;
  pnew.c_code = NULL;

  pnew.return_val.name = (char *)malloc(strlen(ret_val)+1);
  memset(pnew.return_val.name, '\0', strlen(ret_val)+1);
  memcpy(pnew.return_val.name, ret_val, strlen(ret_val));

  pnew.return_val.typname = (char *)malloc(strlen(ret_typname)+1);
  memset(pnew.return_val.typname, '\0', strlen(ret_typname)+1);
  memcpy(pnew.return_val.typname, ret_typname, strlen(ret_typname));
  pnew.return_val.typ = ret_typ;

  if(module->proccnt==0) {
    module->procs = (procdefv)malloc(sizeof(procdef)+1);
  }
  else {
    module->procs = (procdefv)realloc(module->procs,
                                   (module->proccnt+1)*sizeof(procdef));
  }
  if(module->procs == NULL) { printf("ERROR\n"); return; }

  memset((void *) &module->procs[module->proccnt], '\0', sizeof(procdef));
  memcpy((void *)(&(module->procs[module->proccnt])),
         (void *)&pnew, sizeof(procdef));
  (module->proccnt)++;
}

/*========================================================================*/
void AddParam(
  moddefv module,
  char *name,
  char *typname,
  int typ
  )
{
  paramdef pnew;
  int proccnt = module->proccnt-1;
  int paramcnt = 0;

  logx("AddParam(%d, %s, %s, %d)\n", module->procs[proccnt].paramcnt,
       module->procs[proccnt].procname,
       typname, typ);
  memset((void *)&pnew, '\0', sizeof(paramdef));
  pnew.name = (char *)malloc(strlen(name)+1);
  memset(pnew.name, '\0', strlen(name)+1);
  pnew.next = NULL;
  memcpy(pnew.name, name, strlen(name));
  pnew.typname = (char *)malloc(strlen(typname)+1);
  memset(pnew.typname, '\0', strlen(typname)+1);
  memcpy(pnew.typname, typname, strlen(typname));
  pnew.typ = typ;

  paramcnt = module->procs[proccnt].paramcnt;
 if(module->procs[proccnt].paramcnt==0) {
    module->procs[proccnt].param = (paramdefv)malloc(sizeof(paramdef));
 }
  else {
    module->procs[proccnt].param =
      (paramdefv)realloc(module->procs[proccnt].param,
                         (paramcnt+1)*sizeof(paramdef));
  }

  memcpy((void *)(&module->procs[proccnt].param[paramcnt]),
         (void *)&pnew, sizeof(paramdef));
  (module->procs[proccnt].paramcnt)++;
  logx("AddParam() done\n");
}

/*========================================================================*/
void PrintProclist(
  moddefv module
  )
{
  logx("PrintProclist()\n");
  int j;
  for(j=0; j<module->proccnt; j++) {
    PrintProc(module->procs[j]);
  }
}

/*========================================================================*/
void generate_mod(
  moddefv module
  )
{
  procdefv v = NULL;
  cfilesv c_filelist = NULL;
  int proccnt;
  FILE *fp_c, *fp_h;
  char *filename;

  if(strcmp(S_UNAME, "ix86-Linux") == 0) {
    systyp = SYSTYP_LINUX;
  } else if (strcmp(S_UNAME, "HPUX-9")==0) {
    systyp = SYSTYP_HPUX9;
  } else if (strcmp(S_UNAME, "HPUX-10")==0) {
    systyp = SYSTYP_HPUX10;
  }
  init_type_conv();
  printf("SYSTYP:%d\n", systyp);

  filename = (char *)malloc(strlen(module->name)+5);
  sprintf(filename, "%s.cc", module->name);
  fp_c = fopen(filename, "w");
  printf("Creating %s, ", filename);fflush(stdout);
  mod_write_header(fp_c, module->name);
  mod_write_enter_id(fp_c);

  sprintf(filename, "%s.h", module->name);
  fp_h = fopen(filename, "w");
  write_header(fp_h, module->name);
  printf("%s  ...", filename);fflush(stdout);

  /* building mod_init() */
  fprintf(fp_c, "extern \"C\"\n");
  fprintf(fp_c, "int mod_init(int(*iiAddCproc)())\n{\n");
  fprintf(fp_c, "  idhdl h;\n\n");

  if(module->version != NULL) enter_id(fp_c, "version", module->version);
  if(module->info != NULL) enter_id(fp_c, "info", module->info);
  if(module->helpfile != NULL) enter_id(fp_c, "helpfile", module->helpfile);

  for(proccnt=0; proccnt<module->proccnt; proccnt++) {
    printf("->%s, %s\n", module->procs[proccnt].procname,
           module->procs[proccnt].funcname);
    fprintf(fp_c, "  iiAddCproc(\"%s\",\"%s\",%s, mod_%s);\n",
	    module->name, module->procs[proccnt].procname,
	    module->procs[proccnt].is_static ? "TRUE" : "FALSE",
            module->procs[proccnt].funcname);
  }
  fprintf(fp_c, "  return 0;\n}\n\n");

  /* building entry-functions */
  for(proccnt=0; proccnt<module->proccnt; proccnt++) {
    generate_function(&module->procs[proccnt], fp_c);
    generate_header(&module->procs[proccnt], fp_h);
  }
  printf("  done.\n");fflush(stdout);
  fclose(fp_c);
  fclose(fp_h);
}

/*========================================================================*/
void generate_header(procdefv pi, FILE *fp)
{
  int i;

  fprintf(fp, "BOOLEAN mod_%s(leftv res, leftv h);\n", pi->funcname);
  switch( pi->return_val.typ) {
      case SELF_CMD:
        fprintf(fp, "BOOLEAN %s(res, ", pi->funcname);
        break;

      default:
        fprintf(fp, "%s %s(", type_conv[pi->return_val.typ],
                pi->funcname);
  }
  for (i=0;i<pi->paramcnt; i++) {
    fprintf(fp, "%s res%d", type_conv[pi->param[i].typ], i);
    if(i<pi->paramcnt-1) fprintf(fp, ", ");
  }
  fprintf(fp, ");\n\n");
}

void gen_func_param_check(
  FILE *fp,
  procdefv pi,
  int i
  )
{
  fprintf(fp, "  if(v==NULL) goto mod_%s_error;\n", pi->funcname);
  fprintf(fp, "  tok = v->Typ();\n");
  fprintf(fp, "  if((index=iiTestConvert(tok, %s))==0)\n",
          pi->param[i].typname);
  fprintf(fp, "     goto mod_%s_error;\n", pi->funcname);
  fprintf(fp, "  v_save = v->next;\n");
  fprintf(fp, "  v->next = NULL;\n");
  fprintf(fp, "  if(iiConvert(tok, %s, index, v, res%d))\n",
          pi->param[i].typname, i);
  fprintf(fp, "     goto mod_%s_error;\n", pi->funcname);
  fprintf(fp, "  v = v_save;\n");
}

void generate_function(procdefv pi, FILE *fp)
{
  int cnt = 0, i;
  printf("%s has %d paramters\n", pi->funcname, pi->paramcnt);

  fprintf(fp, "BOOLEAN mod_%s(leftv res, leftv h)\n{\n", pi->funcname);
  if(pi->paramcnt>0) {
    if(pi->param[0].typ==SELF_CMD) {
      if(pi->c_code != NULL) fprintf(fp, "%s\n", pi->c_code);

      fprintf(fp, "  return(%s(res,h));\n", pi->funcname);
      fprintf(fp, "}\n\n");
    }
    else {
      fprintf(fp, "  leftv v = h, v_save;\n");
      fprintf(fp, "  int tok = NONE, index = 0;\n");
      for (i=0;i<pi->paramcnt; i++)
        fprintf(fp, "  leftv res%d = (leftv)Alloc0(sizeof(sleftv));\n", i);

      fprintf(fp, "\n");

      if(pi->c_code != NULL) fprintf(fp, "%s\n", pi->c_code);

      for (i=0;i<pi->paramcnt; i++) gen_func_param_check(fp, pi, i);

      fprintf(fp, "  if(v!=NULL) { tok = v->Typ(); goto mod_%s_error; }\n",
              pi->funcname);

      fprintf(fp, "\n");
      switch( pi->return_val.typ) {
          case SELF_CMD:
            fprintf(fp, "    return(%s(res", pi->funcname);
            for (i=0;i<pi->paramcnt; i++)
              fprintf(fp, ", (%s) res%d->Data()",
                      type_conv[pi->param[i].typ], i);
            fprintf(fp, "));\n\n");
           break;

          default:
            fprintf(fp, "  res->rtyp = %s;\n", pi->return_val.typname);
            fprintf(fp, "  res->data = (void *)%s(", pi->funcname);
            for (i=0;i<pi->paramcnt; i++) {
              fprintf(fp, "(%s) res%d->Data()",
                      type_conv[pi->param[i].typ], i);
              if(i<pi->paramcnt-1) fprintf(fp, ", ");
            }
            fprintf(fp, ");\n  return FALSE;\n\n");
      }

      fprintf(fp, "  mod_%s_error:\n", pi->funcname);
      fprintf(fp, "    Werror(\"%s(`%%s`) is not supported\", Tok2Cmdname(tok));\n",
              pi->procname);
      fprintf(fp, "    Werror(\"expected %s(", pi->procname);
      for (i=0;i<pi->paramcnt; i++) {
        fprintf(fp, "'%s'", pi->param[i].name);
        if(i!=pi->paramcnt-1) fprintf(fp, ",");
      }
      fprintf(fp, ")\");\n");
      fprintf(fp, "    return TRUE;\n}\n\n");
    }
  } else {
      switch( pi->return_val.typ) {
          case SELF_CMD:
            fprintf(fp, "  return(%s(res));\n}\n\n", pi->funcname);
           break;

          case NONE:
            fprintf(fp, "  res->rtyp = %s;\n", pi->return_val.typname);
            fprintf(fp, "  res->data = NULL;\n");
            fprintf(fp, "  %s();\n", pi->funcname);
            fprintf(fp, "  return FALSE;\n}\n\n");
            break;

          default:
            fprintf(fp, "  res->rtyp = %s;\n", pi->return_val.typname);
            fprintf(fp, "  res->data = (void *)%s();\n", pi->funcname);
            fprintf(fp, "  return FALSE;\n}\n\n");
      }

  }


}

/*========================================================================*/
void  mod_write_header(FILE *fp, char *module)
{
#if 0
  FILE *fp;
  char buf[BUFLEN];

  regex_t preg;
  regmatch_t   pmatch[1];
  size_t  nmatch = 0;
  char *regex = "@MODULE_NAME@";

  rc = regcomp(&preg, regex, REG_NOSUB);
  if(rc) return -1;

  if(!regexec(&preg, d_entry->d_name, nmatch, pmatch, REG_NOTBOL))
    cert_count++;
  regfree(&preg);

#else
  write_header(fp, module);
  fprintf(fp, "#include <stdlib.h>\n");
  fprintf(fp, "#include <stdio.h>\n");
  fprintf(fp, "#include <string.h>\n");
  fprintf(fp, "#include <ctype.h>\n");
  fprintf(fp, "%s\n", DYNAinclude[systyp]);
  fprintf(fp, "\n");
  fprintf(fp, "#include <locals.h>\n");
  fprintf(fp, "#include \"%s.h\"\n", module);
  fprintf(fp, "\n");
#endif
}

/*========================================================================*/
void write_header(FILE *fp, char *module, char *comment)
{
  fprintf(fp, "%s/*\n%s * This was automaticly generated by modgen\n",
          comment, comment);
  fprintf(fp, "%s * version %s\n", comment, MOD_GEN_VERSION);
  fprintf(fp, "%s * module %s\n", comment, module);
  fprintf(fp, "%s * Don't edit this file\n%s */\n", comment, comment);
  fprintf(fp, "%s\n", comment);
  fprintf(fp, "%s\n", comment);

}

/*========================================================================*/
void enter_id(FILE *fp, char *name, char *value)
{
  fprintf(fp, "  enter_id(\"%s\",\"%s\");\n",name, value);
}

void mod_write_enter_id(FILE *fp)
{
  fprintf(fp, "void enter_id(char *name, char *value)\n");
  fprintf(fp, "{\n  idhdl h;\n\n");
  fprintf(fp, "  h = enterid(mstrdup(name),0, STRING_CMD, &IDROOT, FALSE);\n");
  fprintf(fp, "  if(h!=NULL) {\n");
  fprintf(fp, "     IDSTRING(h) = mstrdup(value);\n");
  fprintf(fp, "  }\n}\n\n");
}

static char *object_name(char *p)
{
  char *q = (char *)strrchr(p, '.');
  if(q==NULL) return "";
  *q = '\0';
  char *r = (char *)malloc(strlen(p)+4);
  switch(systyp) {
      case SYSTYP_LINUX:
        sprintf(r, "%s.lo", p);
        break;

      case SYSTYP_HPUX9:
        sprintf(r, "%s.o", p);
        break;
  }

  *q = '.';
  return(r);
}

/*========================================================================*/
void mod_create_makefile(moddefv module)
{
  FILE *fp;
  fp = fopen("Makefile", "w");
  cfilesv cf = module->files;
  int i;

  printf("Creating Makefile  ...");fflush(stdout);
  write_header(fp, module->name, "#");
  fprintf(fp, "CC\t= gcc\n");
  fprintf(fp, "CXX\t= gcc\n");
  fprintf(fp, "CFLAGS\t= -DSING_NDEBUG -DBUILD_MODULE -I. -I../include\n");
  fprintf(fp, "#LD\t=\n");
  fprintf(fp, "\n");
  fprintf(fp, "SRCS\t= ");

  for(i=0; i<module->filecnt; i++)
    fprintf(fp, "%s ", cf[i].filename);

  fprintf(fp, "\nOBJS\t= ");
  for(i=0; i<module->filecnt; i++)
    fprintf(fp, "%s ", object_name(cf[i].filename));

  fprintf(fp, "\n\n");
  switch(systyp) {
      case SYSTYP_LINUX:
        fprintf(fp, "all:\t%s.so\n", module->name);
        fprintf(fp, "\n");
        fprintf(fp, "%%.lo: %%.cc Makefile\n");
        fprintf(fp, "\t${CC} ${CFLAGS} -c -fPIC -DPIC $< -o $*.lo\n");
        fprintf(fp, "\n");

        fprintf(fp, "%%.la: %%.cc Makefile\n");
        fprintf(fp, "\t${CC} ${CFLAGS} -c $< -o $*.la\n");
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
        break;

      case SYSTYP_HPUX9:
        fprintf(fp, "all:\t%s.sl\n", module->name);
        fprintf(fp, "\n");
        fprintf(fp, "%%.o: %%.cc Makefile\n");
        fprintf(fp, "\t${CC} ${CFLAGS} -c -fPIC -DPIC $< -o $*.o\n");
        fprintf(fp, "\n");
        fprintf(fp, "%s.sl: ${OBJS}\n", module->name);
        fprintf(fp, "\t${LD} -b -o %s.sl \\\n", module->name);
        fprintf(fp, "\t\t${OBJS}\n");
        break;

  }

  fprintf(fp, "clean:\n");
  fprintf(fp, "\trm -f *.o *.lo *.so* *.sl *.la *~ core\n\n");

  fprintf(fp, "distclean: clean\n");
  fprintf(fp, "\trm -f %s.cc %s.h Makefile\n\n", module->name, module->name);

  fprintf(fp, "\n");
  fprintf(fp, "\n");

  fclose(fp);
  printf("  done.\n");
}

/*========================================================================*/
