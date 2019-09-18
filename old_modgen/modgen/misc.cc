/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/*
* ABSTRACT: lib parsing
*/

#include <stdlib.h>
#include <regex.h>

#include "modgen.h"
#include "typmap.h"

#define NEW_PARAM 1

#if 1
#  define logx printf
#else
#  define logx
#endif

/* This is the same structure as in Singular's kernel/structs.h.
 * Duplicating the following code sniplets is save, because it is used
 * independently here.
 * Moreover, it is recommended not to include structs.h here, because otherwise
 * changing the latter could break modgen also.
 */
struct _scmdnames
{
  char *name;
  short alias;
  short tokval;
  short toktype;
};
typedef struct _scmdnames cmdnames;




typedef struct {
  unsigned  nCmdUsed;          /**< number of commands used */
  unsigned nCmdAllocated;      /**< number of commands-slots allocated */
  unsigned nLastIdentifier;
  cmdnames *sCmds;             /**< array of existing commands */

#ifndef GENTABLE
  struct sValCmd1 *psValCmd1;
  struct sValCmd2 *psValCmd2;
  struct sValCmd3 *psValCmd3;
  struct sValCmdM *psValCmdM;
#endif /* GENTABLE */
} SArithBase;


/*---------------------------------------------------------------------*
 * File scope Variables (Variables share by several functions in
 *                       the same file )
 *
 *---------------------------------------------------------------------*/
THREAD_VAR static SArithBase sArithBase;  /**< Base entry for arithmetic */


THREAD_VAR BOOLEAN    expected_parms;
THREAD_VAR int        cmdtok;
THREAD_VAR BOOLEAN siq=FALSE;
const char *lastreserved=NULL;
#define SELF_CMD MAX_TOK+1

/*---------------------------------------------------------------------*
 * Extern Functions declarations
 *
 *---------------------------------------------------------------------*/
static int _gentable_sort_cmds(const void *a, const void *b);
extern int iiArithRemoveCmd(char *szName);
extern int iiArithAddCmd(char *szName, short nAlias, short nTokval,
                         short nToktype, short nPos=-1);
extern void enter_id(FILE *fp, idtyp t, char *name, char *value,
                     int lineno, char *file);
extern void write_enter_id(FILE *fp);
extern void write_add_singular_proc(FILE *fp);

static void  mod_write_ctext(FILE *fp, FILE *fp_in);
THREAD_VAR char type_conv[MAX_TOK][32];

extern void write_intro(moddefv module);

/*=============== types =====================*/
struct sValCmdTab
{
  short cmd;
  short start;
};

/* todo: ensure compiler picks the right header, without using paths here */
/*#include "iparith.inc"*/
#include "../../Singular/iparith.inc"

/*=================== general utilities ============================*/
int IsCmd(char *n, int & tok)
{
  int an=1;
  int i,v;
  int en=sArithBase.nLastIdentifier;

  if( strcmp(n, "SELF") == 0)
  {
    tok = SELF_CMD;
    logx("IsCmd: [%d] %s\n", tok, n);
    return tok;
  }

  if( strcmp(n, "none") == 0)
  {
    tok = NONE;
    logx("IsCmd: [%d] %s\n", tok, n);
    return tok;
  }

  for(an=0; an<sArithBase.nCmdUsed; )
  {
    if(an>=en-1)
    {
      if (strcmp(n, sArithBase.sCmds[an].name) == 0)
      {
        i=an;
        break;
      }
      else if ((an!=en) && (strcmp(n, sArithBase.sCmds[en].name) == 0))
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
    if (*n < *(sArithBase.sCmds[i].name))
    {
      en=i-1;
    }
    else if (*n > *(sArithBase.sCmds[i].name))
    {
      an=i+1;
    }
    else
    {
      v=strcmp(n,sArithBase.sCmds[i].name);
    if(v<0)
    {
        en=i-1;
    }
    else if(v>0)
    {
        an=i+1;
    }
    else /*v==0*/
    {
      break;
    }
  }
  }
  lastreserved=sArithBase.sCmds[i].name;
  tok=sArithBase.sCmds[i].tokval;
  if(sArithBase.sCmds[i].alias==2)
    {
    if(trace)printf("outdated identifier `%s` used - please change your code",
    sArithBase.sCmds[i].name);
    sArithBase.sCmds[i].alias=1;
  }

  logx("IsCmd: [%d] %s\n", tok, n);

  if( (sArithBase.sCmds[i].toktype==ROOT_DECL) ||
      (sArithBase.sCmds[i].toktype==ROOT_DECL_LIST) ||
      (sArithBase.sCmds[i].toktype==RING_DECL) ||
      (sArithBase.sCmds[i].toktype==IDEAL_CMD) ||
      (sArithBase.sCmds[i].toktype==INTMAT_CMD) ||
      (sArithBase.sCmds[i].toktype==MODUL_CMD) ||
      (sArithBase.sCmds[i].toktype==MATRIX_CMD))// ||
//      ((csArithBase.sCds[i].toktype>=DRING_CMD) && (cmds[i].toktype<=VECTOR_CMD)))
    return sArithBase.sCmds[i].toktype;
  return 0;
}


char * decl2str(int n, char *name)
{
  switch(n)
  {
#   include "decl.inc"

    /* first and last entry of tok.h cannot be grepped */
    case MAX_TOK: strcpy(name,"MAX_TOK");       break;
    case ALIAS_CMD: strcpy(name,"ALIAS_CMD");       break;
    default: strcpy(name,"(null)");
  }
  return(name);
}

/*========================================================================*/
struct valid_cmds_def
{
  char *name;
  void (*write_cmd)(moddefv module, procdefv pi, void *arg);
  cmd_token id;
  cmd_type  type;
  int args;
} valid_cmds[] =
{
  { "declaration",  write_function_declaration, CMD_DECL,   CMDT_SINGLE, 0 },
  { "error",        write_function_error,       CMD_ERROR,  CMDT_ANY,    0 },
  { "nodeclaration",write_function_nodecl,      CMD_NODECL, CMDT_SINGLE, 0 },
  { "typecheck",    write_function_typecheck,   CMD_CHECK,  CMDT_SINGLE, 0 },
  { "return",       write_function_result,      CMD_RETURN, CMDT_EQ,     0 },
//{ "return",       write_function_return,      CMD_RETURN, CMDT_SINGLE, 1 },
  { "return",       write_function_return,      CMD_RETURN, CMDT_0,     1 },
  { "return",       write_function_return,      CMD_RETURN, CMDT_ANY,   1 },
  { "singularcmd",  write_function_singularcmd, CMD_SINGULAR, CMDT_ANY, 1 },
  { NULL,           0, CMD_NONE, CMDT_ANY, 0 }
};

cmd_token checkcmd(
  char *cmdname,
  void (**write_cmd)(moddefv module, procdefv pi, void *arg),
  cmd_type  type,
  int args
  )
{
  int i;
  cmd_token rc = CMD_NONE;

  for(i=0; valid_cmds[i].name!=NULL; i++)
  {
    if(strcmp(valid_cmds[i].name, cmdname)==0)
    {
      rc = CMD_BADSYNTAX;
      if(valid_cmds[i].type == type)
      {
        *write_cmd = valid_cmds[i].write_cmd;
        return valid_cmds[i].id;
      }
    }
  }
  return rc;
}

/*========================================================================*/
struct valid_vars_def
{
  char *name;
  var_type type;
  var_token id;
  void (*write_cmd)(moddefv module, var_token type,
                    idtyp t, void *arg1, void *arg2);
} valid_vars[] =
{
  { "help",         VAR_STRING,  VAR_HELP,     write_main_variable },
  { "info",         VAR_STRING,  VAR_INFO,     write_main_variable },
  { "package",      VAR_STRING,  VAR_MODULE,   0 },
  { "version",      VAR_STRING,  VAR_VERSION,  write_main_variable },
  { "category",     VAR_STRING,  VAR_CATEGORY, write_main_variable },
  { NULL,           VAR_UNKNOWN, VAR_NONE, 0 }
};

var_token checkvar(
  char *varname,
  var_type type,
  void (**write_cmd)(moddefv module, var_token type,
                    idtyp t, void *arg1, void *arg2)
  )
{
  int i;
  for(i=0; valid_vars[i].name!=NULL; i++)
    if((strcmp(valid_vars[i].name, varname)==0) &&
       (valid_vars[i].type == type) ) {
      *write_cmd = valid_vars[i].write_cmd;
      return valid_vars[i].id;
    }
  return VAR_NONE;
}

/*========================================================================*/
void write_main_variable(
  moddefv module,
  var_token type,
  idtyp t,
  void *arg1,
  void *arg2
  )
{
  enter_id(module->fmtfp, t, (char *)arg1, (char *)arg2, yylineno,
           module->filename);
  switch(type)
  {
    case VAR_INFO:
          module->info = (char *)malloc(strlen((char *)arg2)+1);
          memset(module->info, '\0', strlen((char *)arg2)+1);
          memcpy(module->info,(char *)arg2,strlen((char *)arg2));
          break;
    case VAR_VERSION:
          module->version = (char *)malloc(strlen((char *)arg2)+1);
          memset(module->version, '\0', strlen((char *)arg2)+1);
          memcpy(module->version,(char *)arg2,strlen((char *)arg2));
          break;
    case VAR_CATEGORY:
          module->category = (char *)malloc(strlen((char *)arg2)+1);
          memset(module->category, '\0', strlen((char *)arg2)+1);
          memcpy(module->category,(char *)arg2,strlen((char *)arg2));
          break;
    default: break;
  }
}

/*========================================================================*/
void PrintProc(
  procdefv pi
  )
{
  int i;

  printf("%4d proc: %s(", pi->lineno, pi->procname);

  for(i=0; i<pi->paramcnt; i++)
  {
    printf("%s (%s)", (pi->param[i]).name, (pi->param[i]).typname);
    if(i < (pi->paramcnt-1)) printf(",");
  }
  printf(")\n");
  if(pi->return_val.typ!=0)
  {
    printf("\treturn = %s (%s)\n", pi->return_val.name,
           pi->return_val.typname);
  }
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
void make_module_name(char *p, moddefv module)
{
  if(strlen(p)>=1)
  {
    module->targetname = (char *)malloc(strlen(p)+1);
    memset(module->targetname, '\0', strlen(p)+1);
    memcpy(module->targetname,p,strlen(p));
  }
  else
  {
    module->targetname = (char *)malloc(strlen(module->name)+1);
    memset(module->targetname, '\0', strlen(module->name)+1);
    memcpy(module->targetname,module->name,strlen(module->name));
  }

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

  if(module->filecnt==0)
  {
    module->files = (cfilesv)malloc(sizeof(cfiles)+1);
  }
  else
  {
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
void PrintProclist(
  moddefv module
  )
{
  logx("PrintProclist()\n");
  int j;
  for(j=0; j<module->proccnt; j++)
  {
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

  switch(section)
  {
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

  for(proccnt=0; proccnt<module->proccnt; proccnt++)
  {
    printf("->%s, %s\n", module->procs[proccnt].procname,
           module->procs[proccnt].funcname);
    fprintf(module->modfp, "  psModulFunctions->iiAddCproc(\"%s\",\"%s\",%s, mod_%s);\n",
            module->name, module->procs[proccnt].procname,
            module->procs[proccnt].is_static ? "TRUE" : "FALSE",
            module->procs[proccnt].funcname);
  }
  fprintf(module->modfp, "  return 0;\n}\n\n");

  /* building entry-functions */
  for(proccnt=0; proccnt<module->proccnt; proccnt++)
  {
    //generate_function(&module->procs[proccnt], module->modfp);
    //generate_header(&module->procs[proccnt], fp_h);
  }
  printf("  done.\n");fflush(stdout);
  fclose(module->modfp);
  fclose(fp_h);
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

#if 0
  if(pi->paramcnt>0)
  {
  }
  else
  {
    switch( pi->return_val.typ)
    {
        case SELF_CMD:
          fprintf(module->fmtfp, "  return(%s(res));", pi->funcname);
          break;

        case NONE:
          fprintf(module->fmtfp, "  return FALSE;");
          break;

        default:
          fprintf(module->fmtfp, "  res->rtyp = %s;\n",
                  pi->return_val.typname);
          fprintf(module->fmtfp, "  res->data = (void *)%s();\n",
                  pi->funcname);
          fprintf(module->fmtfp, "  return FALSE;");
    }
  }
#endif
  fprintf(module->fmtfp, "/*\n}\n\n*/");

}


/*========================================================================*/
void  mod_write_header(FILE *fp, char *module, char what)
{

  write_header(fp, module);
  fprintf(fp, "#line %d \"%s.cc\"\n", modlineno++, module);
  if(what != 'h')
  {
    fprintf(fp, "#include <stdlib.h>\n");
    fprintf(fp, "#include <stdio.h>\n");
    fprintf(fp, "#include <string.h>\n");
    fprintf(fp, "#include <ctype.h>\n");

    fprintf(fp, "#if defined(HPUX_9) || defined(HPUX_10)\n");
    fprintf(fp, "#  include <dl.h>\n");
    fprintf(fp, "#else\n");
    fprintf(fp, "#  include <dlfcn.h>\n");
    fprintf(fp, "#endif\n");

    fprintf(fp, "#include <unistd.h>\n");
    fprintf(fp, "#include <sys/stat.h>");
    fprintf(fp, "\n");
    fprintf(fp, "#include <kernel/mod2.h>\n");
    fprintf(fp, "#include <Singular/tok.h>\n");
    fprintf(fp, "#include <kernel/structs.h>\n");
    fprintf(fp, "#include <Singular/ipid.h>\n\n");
    fprintf(fp, "#include <Singular/locals.h>\n");
    fprintf(fp, "#include <omalloc.h>\n");
    fprintf(fp, "#include \"%s.h\"\n", module);
    modlineno+=8;

    fprintf(fp, "#line %d \"%s.cc\"\n", modlineno++, module);
    write_enter_id(fp);
    fprintf(fp, "\n");    modlineno+=1;
    fprintf(fp, "#line %d \"%s.cc\"\n", modlineno++, module);
    write_add_singular_proc(fp);
    write_crccheck(fp);
    fprintf(fp, "\n");

    fprintf(fp, "void fill_help_package();\n");
    fprintf(fp, "void fill_example_package();\n");
    modlineno+=3;
  }
  fprintf(fp, "\n");

}

/*========================================================================*/
void write_header(FILE *fp, char *module, char *comment)
{
  fprintf(fp, "%s/*\n%s * This was automatically generated by modgen\n",
          comment, comment);
  fprintf(fp, "%s * version %s\n", comment, MOD_GEN_VERSION);
  fprintf(fp, "%s * module %s\n", comment, module);
  fprintf(fp, "%s * Don't edit this file\n%s */\n", comment, comment);
  fprintf(fp, "%s\n", comment);
  fprintf(fp, "%s\n", comment);
  if(strlen(comment)==1)modlineno+=10;
}

/*========================================================================*/
void enter_id(
  FILE *fp,
  idtyp t,
  char *name,
  char *value,
  int lineno,
  char *file
  )
{
  unsigned int i;
  char tname[32];

  if(lineno)
    fprintf(fp, "#line %d \"%s\"\n", lineno, file);
  if(strcmp(decl2str(t, tname),"STRING_CMD")==0)
  {
    fprintf(fp, "  enter_id(\"%s\",\"",name);
    for(i=0;i<strlen(value);i++)
    {
       if(value[i]=='\n') fprintf(fp,"\\n");
       else fprintf(fp,"%c",value[i]);
    }
    fprintf(fp,"\", %s);\n",decl2str(t,tname));
  }
  else
  {
    fprintf(fp, "  enter_id(\"%s\",\"%s\", %s);\n",name, value,
          decl2str(t, tname));
  }
}

/*========================================================================*/
void init_type_conv()
{
  strcpy(type_conv[NONE], "none");
//  strcpy(type_conv[NONE], "void");
  strcpy(type_conv[INT_CMD], "int");
  strcpy(type_conv[RING_CMD], "ring");
  strcpy(type_conv[QRING_CMD], "ring");
  strcpy(type_conv[POLY_CMD], "poly");
  strcpy(type_conv[NUMBER_CMD], "number");
  strcpy(type_conv[MODUL_CMD], "ideal");
  strcpy(type_conv[VECTOR_CMD], "poly");
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
  strcpy(type_conv[RESOLUTION_CMD], "syStrategy");
/*
  strcpy(type_conv[], "");
  strcpy(type_conv[], "");
  strcpy(type_conv[], "");
  strcpy(type_conv[], "");
  strcpy(type_conv[], "");
  strcpy(type_conv[], "");
  strcpy(type_conv[], "");
  printf("[%d] %s\n", INT_CMD, type_conv[INT_CMD]);
  printf("[%d] %s\n", MODUL_CMD, type_conv[MODUL_CMD]);
  printf("[%d] %s\n", STRING_CMD, type_conv[STRING_CMD]);
  printf("[%d] %s\n", LINK_CMD, type_conv[LINK_CMD]);
  printf("[%d] %s\n", PACKAGE_CMD, type_conv[PACKAGE_CMD]);
  / **/
}

/*========================================================================*/
/*---------------------------------------------------------------------*/
/**
 * @brief compares to entry of cmdsname-list

 @param[in] a
 @param[in] b

 @return <ReturnValue>
**/
/*---------------------------------------------------------------------*/
static int _gentable_sort_cmds(
  const void *a,
  const void *b
  )
{
  cmdnames *pCmdL = (cmdnames*)a;
  cmdnames *pCmdR = (cmdnames*)b;

  if(a==NULL || b==NULL)             return 0;

  /* empty entries goes to the end of the list for later reuse */
  if(pCmdL->name==NULL) return 1;
  if(pCmdR->name==NULL) return -1;

  /* $INVALID$ must come first */
  if(strcmp(pCmdL->name, "$INVALID$")==0) return -1;
  if(strcmp(pCmdR->name, "$INVALID$")==0) return  1;

  /* tokval=-1 are reserved names at the end */
  if( (pCmdL->tokval==-1) && pCmdR->tokval==-1)
    return strcmp(pCmdL->name, pCmdR->name);

  /* pCmdL->tokval==-1, pCmdL goes at the end */
  if(pCmdL->tokval==-1) return 1;
  /* pCmdR->tokval==-1, pCmdR goes at the end */
  if(pCmdR->tokval==-1) return -1;

  return strcmp(pCmdL->name, pCmdR->name);
}

/*---------------------------------------------------------------------*/
/**
 * @brief initialisation of arithmetic structured data

 @retval 0 on success

**/
/*---------------------------------------------------------------------*/
int iiInitArithmetic()
{
  int i;
  //printf("iiInitArithmetic()\n");
#ifndef GENTABLE
  memset(&sArithBase, 0, sizeof(sArithBase));
  iiInitCmdName();
  /* fix last-identifier */
#endif   /* !GENTABLE */
}

int iiArithFindCmd(const char *szName)
{
  int an=0;
  int i = 0,v = 0;
#ifndef GENTABLE
  int en=sArithBase.nLastIdentifier;

  for(an=0; an<sArithBase.nCmdUsed; )
  {
    if(an>=en-1)
    {
      if (strcmp(szName, sArithBase.sCmds[an].name) == 0)
      {
        //Print("RET-an=%d %s\n", an, sArithBase.sCmds[an].name);
        return an;
      }
      else if (strcmp(szName, sArithBase.sCmds[en].name) == 0)
      {
        //Print("RET-en=%d %s\n", en, sArithBase.sCmds[en].name);
        return en;
      }
      else
      {
        //Print("RET- 1\n");
        return -1;
      }
    }
    i=(an+en)/2;
    if (*szName < *(sArithBase.sCmds[i].name))
    {
      en=i-1;
    }
    else if (*szName > *(sArithBase.sCmds[i].name))
    {
      an=i+1;
    }
    else
    {
      v=strcmp(szName,sArithBase.sCmds[i].name);
      if(v<0)
      {
        en=i-1;
      }
      else if(v>0)
      {
        an=i+1;
      }
      else /*v==0*/
      {
        //Print("RET-i=%d %s\n", i, sArithBase.sCmds[i].name);
        return i;
      }
    }
  }
  //if(i>=0 && i<sArithBase.nCmdUsed)
  //  return i;
  //Print("RET-2\n");
  return -2;
#else
  return 0;
#endif
}

int iiArithAddCmd(
  char *szName,
  short nAlias,
  short nTokval,
  short nToktype,
  short nPos
  )
{
  //printf("AddCmd(%s, %d, %d, %d, %d)\n", szName, nAlias,
  //       nTokval, nToktype, nPos);
  if(nPos>=0)
  {
    if(nPos>=sArithBase.nCmdAllocated) return -1;
    if(szName!=NULL) sArithBase.sCmds[nPos].name    = strdup(szName);
    else sArithBase.sCmds[nPos].name = NULL;
    sArithBase.sCmds[nPos].alias   = nAlias;
    sArithBase.sCmds[nPos].tokval  = nTokval;
    sArithBase.sCmds[nPos].toktype = nToktype;
    sArithBase.nCmdUsed++;
    //if(nTokval>0) sArithBase.nLastIdentifier++;
  }
  else
  {
    if(szName==NULL) return -1;
    int nIndex = iiArithFindCmd(szName);
    if(nIndex>=0)
    {
      printf("'%s' already exists at %d\n", szName, nIndex);
      return -1;
    }

    if(sArithBase.nCmdUsed>=sArithBase.nCmdAllocated)
    {
      /* needs to create new slots */
      unsigned long nSize = (sArithBase.nCmdAllocated+1)*sizeof(cmdnames);
      sArithBase.sCmds = (cmdnames *)realloc(sArithBase.sCmds, nSize);
      if(sArithBase.sCmds==NULL) return -1;
      sArithBase.nCmdAllocated++;
    }
    /* still free slots available */
    sArithBase.sCmds[sArithBase.nCmdUsed].name    = strdup(szName);
    sArithBase.sCmds[sArithBase.nCmdUsed].alias   = nAlias;
    sArithBase.sCmds[sArithBase.nCmdUsed].tokval  = nTokval;
    sArithBase.sCmds[sArithBase.nCmdUsed].toktype = nToktype;
    sArithBase.nCmdUsed++;

    qsort(sArithBase.sCmds, sArithBase.nCmdUsed, sizeof(cmdnames),
          (&_gentable_sort_cmds));
    for(sArithBase.nLastIdentifier=sArithBase.nCmdUsed-1;
        sArithBase.nLastIdentifier>0; sArithBase.nLastIdentifier--)
    {
      if(sArithBase.sCmds[sArithBase.nLastIdentifier].tokval>=0) break;
    }
    //Print("L=%d\n", sArithBase.nLastIdentifier);
  }
  return 0;
}

