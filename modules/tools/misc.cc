/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/* $Id: misc.cc,v 1.2 1999-03-17 12:41:30 krueger Exp $ */
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

#define SYSTYP_NONE   0
#define SYSTYP_LINUX  1
#define SYSTYP_HPUX9  2
#define SYSTYP_HPUX10 3

char *DYNAinclude[] = {
   "",
   "#include <dlfcn.h>",
   "#include <dl.h>",
   "#include <dl.h>",
   "#include <>",
   "#include <>",
   "#include <>",
   NULL
};

int systyp = SYSTYP_NONE;

BOOLEAN    expected_parms;
int        cmdtok;
BOOLEAN siq=FALSE;
char *lastreserved=NULL;
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
  strcpy(module->revision, libnamebuf);
}

/*========================================================================*/
cfilesv Add2files(cfilesv cf, char *name)
{
  cfilesv cfnew = (cfilesv)malloc(sizeof(cfiles));
  cfnew->filename = (char *)malloc(strlen(name+1));
  cfnew->next = NULL;
  strcpy(cfnew->filename, name);
  if(cf == NULL) cf = cfnew;
  else cfnew->next = cf;
  return(cfnew);
}

/*========================================================================*/
procdefv Add2proclist(procdefv pi, char *name)
{
  procdefv pnew = (procdefv)malloc(sizeof(procdef));
  pnew->procname = (char *)malloc(strlen(name+1));
  pnew->funcname = (char *)malloc(strlen(name+1));
  pnew->param = NULL;
  pnew->next = NULL;
  pnew->is_static = 0;
  pnew->paramcnt = 0;
  strcpy(pnew->procname, name);
  strcpy(pnew->funcname, name);
  if(pi==NULL) pi = pnew;
  else pnew->next = pi;
  return(pnew);
}

/*========================================================================*/
void AddParam(procdefv pi, char *name, char *typname, int typ)
{
  paramdefv pnew = (paramdefv)malloc(sizeof(paramdef));
  pnew->name = (char *)malloc(strlen(name+1));
  pnew->next = NULL;
  strcpy(pnew->name, name);
  pnew->typname = (char *)malloc(strlen(typname+1));
  strcpy(pnew->typname, typname);
  pnew->typ = typ;

  if(pi->param==NULL) pi->param = pnew;
  else {
    paramdefv pp = pi->param;
    while(pp->next != NULL) pp = pp->next;
    pp->next = pnew;
  }
  (pi->paramcnt)++;
}

/*========================================================================*/
void PrintProclist(procdefv pi)
{
  procdefv v = pi;
  paramdefv pp;
  for(v=pi; v!=NULL; v = v->next) {
//  while(v!=NULL) {
    pp = v->param;
    printf("proc: %s(", v->procname);
    if(pp!= NULL) {
      printf("%s", pp->name);
      while(pp->next!= NULL) {
	pp = pp->next;
	printf(",%s", pp->name);
      }
    }
    printf(")\n");
//    v =v ->next;
  }
}

/*========================================================================*/
void generate_mod(
  procdefv pi,
  moddefv module,
  cfilesv c_filelist
  )
{
  procdefv v = NULL;
  FILE *fp_c, *fp_h;
  char *filename;
  
  if(strcmp(S_UNAME, "ix86-Linux") == 0) {
    systyp = SYSTYP_LINUX;
  } else if (strcmp(S_UNAME, "HPUX-9")==0) {
    systyp = SYSTYP_HPUX9;
  } else if (strcmp(S_UNAME, "HPUX-10")==0) {
    systyp = SYSTYP_HPUX10;
  }
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
  for(v=pi; v!=NULL; v = v->next) {
//  while(v!=NULL) {
    printf("->%s, %s\n", v->procname, v->funcname);
    fprintf(fp_c, "  iiAddCproc(\"%s\",\"%s\",%s, mod_%s);\n",
	    module->name, v->procname, 
	    v->is_static ? "TRUE" : "FALSE", v->funcname);
//    v =v ->next;
  }
  fprintf(fp_c, "  return 0;\n}\n\n");
  v = pi;

  /* building entry-functions */
  for(v=pi; v!=NULL; v = v->next) {
    generate_function(v, fp_c);
    generate_header(v, fp_h);
  }
  printf("  done.\n");fflush(stdout);
  fclose(fp_c);
  fclose(fp_h);
}

/*========================================================================*/
void generate_header(procdefv pi, FILE *fp)
{
  fprintf(fp, "BOOLEAN %s(leftv res, leftv h);\n", pi->funcname);
  fprintf(fp, "BOOLEAN mod_%s(leftv res, leftv h);\n\n", pi->funcname);
}

void generate_function(procdefv pi, FILE *fp)
{
  int cnt = 0, i;
  printf("%s has %d paramters\n", pi->funcname, pi->paramcnt);
  
  paramdefv pp = pi->param;
  fprintf(fp, "BOOLEAN mod_%s(leftv res, leftv h)\n{\n", pi->funcname);
  if(pp!= NULL && pp->typ!=SELF_CMD) {
    fprintf(fp, "  leftv v = h;\n");
    fprintf(fp, "  int tok = NONE, index = 0;\n");
    for (i=0;i<pi->paramcnt; i++)
      fprintf(fp, "  leftv res%d = (leftv)Alloc0(sizeof(sleftv));\n", i);

    fprintf(fp, "\n");
    
    while(pp!=NULL) {
      fprintf(fp, "  if(v==NULL) goto mod_%s_error;\n", pi->funcname);
      fprintf(fp, "  tok = v->Typ();\n");
      fprintf(fp, "  printf(\"test %d.1\\n\");\n", cnt);
      fprintf(fp, "  if((index=iiTestConvert(tok, %s))==0)\n", pp->typname);
      fprintf(fp, "     goto mod_%s_error;\n", pi->funcname);
      fprintf(fp, "  printf(\"test %d.2\\n\");\n", cnt);
      fprintf(fp, "  if(iiConvert(tok, %s, index, v, res%d))\n",
              pp->typname, cnt);
      fprintf(fp, "     goto mod_%s_error;\n", pi->funcname);
      fprintf(fp, "  printf(\"test %d.3\\n\");\n", cnt);
      fprintf(fp, "  v = v->next;\n");
      pp = pp->next;
      cnt++;
    }
    fprintf(fp, "  if(v!=NULL) { tok = v->Typ(); goto mod_%s_error; }\n", pi->funcname);
    fprintf(fp, "  printf(\"test before return\\n\");\n");

    fprintf(fp, "\n");
    fprintf(fp, "    return(%s(res", pi->funcname);
    for (i=0;i<pi->paramcnt; i++)
      fprintf(fp, ", res%d", i);
    fprintf(fp, "));\n\n");
    fprintf(fp, "  mod_%s_error:\n", pi->funcname);
    fprintf(fp, "    Werror(\"%s(`%%s`) is not supported\", Tok2Cmdname(tok));\n",
            pi->procname);
    fprintf(fp, "    Werror(\"expected %s(", pi->procname);
    for(pp = pi->param; pp != NULL; pp = pp->next) {
      fprintf(fp, "'%s'", pp->name);
      if(pp->next != NULL) fprintf(fp, ",");
    }
    fprintf(fp, ")\");\n");
    fprintf(fp, "    return TRUE;\n}\n\n");
  }
  else {
    fprintf(fp, "  return(%s(res,h));\n", pi->funcname);
    fprintf(fp, "}\n\n");
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
void mod_create_makefile(moddefv module, cfilesv c_filelist)
{
  FILE *fp;
  fp = fopen("Makefile", "w");
  cfilesv cf = c_filelist;
  
  printf("Creating Makefile  ...");fflush(stdout);
  write_header(fp, module->name, "#");
  fprintf(fp, "CC\t= gcc\n");
  fprintf(fp, "CXX\t= gcc\n");
  fprintf(fp, "CFLAGS\t= -DNDEBUG -I. -I../include\n");
  fprintf(fp, "#LD\t=\n");
  fprintf(fp, "\n");
  fprintf(fp, "SRCS\t= ");
  for(cf = c_filelist; cf != NULL; cf = cf->next)
    fprintf(fp, "%s ", cf->filename);

  fprintf(fp, "\nOBJS\t= ");
  for(cf = c_filelist; cf != NULL; cf = cf->next)
    fprintf(fp, "%s ", object_name(cf->filename));

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
