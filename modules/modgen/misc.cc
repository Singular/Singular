/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/* $Id: misc.cc,v 1.2 1999-12-21 12:15:41 krueger Exp $ */
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

#define NEW_PARAM 1

#define SYSTYP_NONE   0
#define SYSTYP_LINUX  1
#define SYSTYP_HPUX9  2
#define SYSTYP_HPUX10 3

#if 0
#  define logx printf
#else
#  define logx
#endif

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
static void  mod_write_ctext(FILE *fp, FILE *fp_in);
char type_conv[MAX_TOK][32];

extern void write_intro(moddefv module);

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
char *valid_vars[] = {
  "module",
  "version",
  "info",
  NULL
};

int checkvar(
  char *varname
  )
{
  int i;
  for(i=0; valid_vars[i]!=NULL; i++)
    if(strcmp(valid_vars[i], varname)==0) return 1;
  return 0;
}

  
/*========================================================================*/
void PrintProc(
  procdefv pi
  )
{
  int i;
  
  printf("%4d proc: %s(", pi->lineno, pi->procname);
  
  for(i=0; i<pi->paramcnt; i++) {
    printf("%s (%s)", (pi->param[i]).name, (pi->param[i]).typname);
    if(i < (pi->paramcnt-1)) printf(",");
  }
  printf(")\n");
  if(pi->return_val.typ!=0)
    printf("\treturn = %s (%s)\n", pi->return_val.name, pi->return_val.typname);
  printf("{%s}\n", pi->c_code);
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
void PrintProclist(
  moddefv module
  )
{
  logx("PrintProclist()\n");
  int j;
  for(j=0; j<module->proccnt; j++) {
    PrintProc(&(module->procs[j]));
  }
}

/*========================================================================*/
void generate_mod(
  moddefv module,
  int section
  )
{
  procdefv v = NULL;
  cfilesv c_filelist = NULL;
  int proccnt;
  FILE *fp_h;
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
  
  switch(section) {
      case 1:
        write_intro(module);
        return;
        
      case 2:
        printf("Writing %s, section %d", filename, section);fflush(stdout);
        break;
  }
  
  sprintf(filename, "%s.h", module->name);
  fp_h = fopen(filename, "w");
  write_header(fp_h, module->name);
  printf("%s  ...", filename);fflush(stdout);
  
  /* building mod_init() */
  
  for(proccnt=0; proccnt<module->proccnt; proccnt++) {
    printf("->%s, %s\n", module->procs[proccnt].procname,
           module->procs[proccnt].funcname);
    fprintf(module->modfp, "  iiAddCproc(\"%s\",\"%s\",%s, mod_%s);\n",
	    module->name, module->procs[proccnt].procname, 
	    module->procs[proccnt].is_static ? "TRUE" : "FALSE",
            module->procs[proccnt].funcname);
  }
  fprintf(module->modfp, "  return 0;\n}\n\n");

  /* building entry-functions */
  for(proccnt=0; proccnt<module->proccnt; proccnt++) {
    generate_function(&module->procs[proccnt], module->modfp);
    //generate_header(&module->procs[proccnt], fp_h);
  }
  printf("  done.\n");fflush(stdout);
  fclose(module->modfp);
  fclose(fp_h);
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
void  write_procedure_text(
  moddefv module,
  int lineno
)
{
  int i;
  procdefv pi = &module->procs[module->proccnt-1];

  fprintf(module->fmtfp, "#line %d \"%s\"\n", lineno, module->filename);
  fprintf(module->fmtfp, "%s\n", module->procs[module->proccnt-1].c_code);
  free(module->procs[module->proccnt-1].c_code);
  module->procs[module->proccnt-1].c_code = NULL;

  if(pi->paramcnt>0) {
    if(pi->param[0].typ!=SELF_CMD) {
      switch( pi->return_val.typ) {
          case SELF_CMD:
            fprintf(module->fmtfp, "    return(%s(res", pi->funcname);
            for (i=0;i<pi->paramcnt; i++)
              fprintf(module->fmtfp, ", (%s) res%d->Data()",
                      type_conv[pi->param[i].typ], i);
            fprintf(module->fmtfp, "));\n\n");
           break;

          default:
            fprintf(module->fmtfp, "  res->rtyp = %s;\n", pi->return_val.typname);
            fprintf(module->fmtfp, "  res->data = (void *)%s(", pi->funcname);
            for (i=0;i<pi->paramcnt; i++) {
              fprintf(module->fmtfp, "(%s) res%d->Data()",
                      type_conv[pi->param[i].typ], i);
              if(i<pi->paramcnt-1) fprintf(module->fmtfp, ", ");
            }
            fprintf(module->fmtfp, ");\n  return FALSE;\n\n");
      }
      
      fprintf(module->fmtfp, "  mod_%s_error:\n", pi->funcname);
      fprintf(module->fmtfp, "    Werror(\"%s(`%%s`) is not supported\", Tok2Cmdname(tok));\n",
              pi->procname);
      fprintf(module->fmtfp, "    Werror(\"expected %s(", pi->procname);
      for (i=0;i<pi->paramcnt; i++) {
        fprintf(module->fmtfp, "'%s'", pi->param[i].name);
        if(i!=pi->paramcnt-1) fprintf(module->fmtfp, ",");
      }
      fprintf(module->fmtfp, ")\");\n");
      fprintf(module->fmtfp, "    return TRUE;");
    }
  }
  else {
    switch( pi->return_val.typ) {
        case SELF_CMD:
          fprintf(module->fmtfp, "  return(%s(res));", pi->funcname);
          break;

        case NONE:
          fprintf(module->fmtfp, "  return FALSE;");
          break;
            
        default:
          fprintf(module->fmtfp, "  res->rtyp = %s;\n", pi->return_val.typname);
          fprintf(module->fmtfp, "  res->data = (void *)%s();\n", pi->funcname);
          fprintf(module->fmtfp, "  return FALSE;");
    }
  }
  fprintf(module->fmtfp, "\n}\n\n");
  
}

/*========================================================================*/
void  write_procedure_header(
  moddefv module
)
{
  int cnt = 0, i;
  procdefv pi = &module->procs[module->proccnt-1];
  
  printf("%s has %d paramters\n", pi->funcname, pi->paramcnt);
  
  fprintf(module->fmtfp, "BOOLEAN mod_%s(leftv res, leftv h)\n{\n", pi->funcname);
  if(pi->paramcnt>0) {
    if(pi->param[0].typ!=SELF_CMD) {
      fprintf(module->fmtfp, "  leftv v = h, v_save;\n");
      fprintf(module->fmtfp, "  int tok = NONE, index = 0;\n");
      for (i=0;i<pi->paramcnt; i++)
        fprintf(module->fmtfp, "  leftv res%d = (leftv)Alloc0(sizeof(sleftv));\n", i);

      fprintf(module->fmtfp, "\n");
    
      if(pi->c_code != NULL) fprintf(module->fmtfp, "%s\n", pi->c_code);
  
      for (i=0;i<pi->paramcnt; i++) gen_func_param_check(module->fmtfp, pi, i);

      fprintf(module->fmtfp, "  if(v!=NULL) { tok = v->Typ(); goto mod_%s_error; }\n",
              pi->funcname);

      fprintf(module->fmtfp, "\n");
    }
  }
  else {
    if( pi->return_val.typ == NONE ) {
          fprintf(module->fmtfp, "  res->rtyp = %s;\n", pi->return_val.typname);
          fprintf(module->fmtfp, "  res->data = NULL;\n");
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

  fprintf(fp, "void enter_id(char *name, char *value);\n");
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
void enter_id(FILE *fp, char *name, char *value, int lineno, char *file)
{
  fprintf(fp, "  enter_id(\"%s\",\"%s\");\n",name, value);
  if(lineno)
    fprintf(fp, "#line %d \"%s\"\n", lineno, file);
}

/*========================================================================*/
void init_type_conv() 
{
  strcpy(type_conv[NONE], "none");
  strcpy(type_conv[NONE], "void");
  strcpy(type_conv[INT_CMD], "int");
  strcpy(type_conv[RING_CMD], "ring");
  strcpy(type_conv[QRING_CMD], "ring");
  strcpy(type_conv[POLY_CMD], "poly");
  strcpy(type_conv[NUMBER_CMD], "number");
  strcpy(type_conv[MODUL_CMD], "ideal");
  strcpy(type_conv[VECTOR_CMD], "ideal");
  strcpy(type_conv[IDEAL_CMD], "ideal");
  strcpy(type_conv[MAP_CMD], "map");
  strcpy(type_conv[MATRIX_CMD], "matrix");
  strcpy(type_conv[STRING_CMD], "char *");
  strcpy(type_conv[INTMAT_CMD], "intvec *");
  strcpy(type_conv[INTVEC_CMD], "intvec *");
  strcpy(type_conv[LIST_CMD], "lists");
  strcpy(type_conv[LINK_CMD], "si_link");
  strcpy(type_conv[PACKAGE_CMD], "package");
  strcpy(type_conv[PROC_CMD], "procinfo *");
/*
  strcpy(type_conv[], "");
  strcpy(type_conv[], "");
  strcpy(type_conv[], "");
  strcpy(type_conv[], "");
  strcpy(type_conv[], "");
  strcpy(type_conv[], "");
  strcpy(type_conv[], "");
*/
  printf("[%d] %s\n", INT_CMD, type_conv[INT_CMD]);
  printf("[%d] %s\n", MODUL_CMD, type_conv[MODUL_CMD]);
  printf("[%d] %s\n", STRING_CMD, type_conv[STRING_CMD]);
  printf("[%d] %s\n", LINK_CMD, type_conv[LINK_CMD]);
  printf("[%d] %s\n", PACKAGE_CMD, type_conv[PACKAGE_CMD]);
  /**/
}

/*========================================================================*/
