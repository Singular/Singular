/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/* $Id$ */

/*
* ABSTRACT: generate iparith.inc etc.
*/

#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdio.h>
#include <time.h>
#include <unistd.h>

#include <Singular/mod2.h>
#include <Singular/tok.h>
#include <Singular/grammar.h>

  #define ALLOW_PLURAL     1
  #define NO_PLURAL        0
  #define COMM_PLURAL      2
  #define  PLURAL_MASK     3

#define ALLOW_RING       4
#define NO_RING          0


/*=============== types =====================*/
struct _scmdnames
{
  const char *name;
  short alias;
  short tokval;
  short toktype;
};
typedef struct _scmdnames cmdnames;


struct sValCmd2
{
  int p;
  short cmd;
  short res;
  short arg1;
  short arg2;
  short valid_for;
};
struct sValCmd1
{
  int p;
  short cmd;
  short res;
  short arg;
  short valid_for;
};
struct sValCmd3
{
  int p;
  short cmd;
  short res;
  short arg1;
  short arg2;
  short arg3;
  short valid_for;
};
struct sValCmdM
{
  int p;
  short cmd;
  short res;
  short number_of_args; /* -1: any, -2: any >0, .. */
  short valid_for;
};
struct sValAssign_sys
{
  int p;
  short res;
  short arg;
};

struct sValAssign
{
  int p;
  short res;
  short arg;
};

struct sConvertTypes
{
  int i_typ;
  int o_typ;
  int p;
  int pl;
};


#define jjWRONG  1
#define jjWRONG2  1
#define jjWRONG3  1
#define XS(A) A


#define D(A)    2
#define IPARITH
#define GENTABLE
#define IPCONV
#define IPASSIGN

#include "table.h"

const char * Tok2Cmdname(int tok)
{
  int i = 0;
  if (tok < 0)
  {
    return cmds[0].name;
  }
  if (tok==COMMAND) return "command";
  if (tok==ANY_TYPE) return "any_type";
  if (tok==NONE) return "nothing";
  //if (tok==IFBREAK) return "if_break";
  //if (tok==VECTOR_FROM_POLYS) return "vector_from_polys";
  //if (tok==ORDER_VECTOR) return "ordering";
  //if (tok==REF_VAR) return "ref";
  //if (tok==OBJECT) return "object";
  //if (tok==PRINT_EXPR) return "print_expr";
  if (tok==IDHDL) return "identifier";
  // we do not blackbox objects during table generation:
  //if (tok>MAX_TOK) return getBlackboxName(tok);
  while (cmds[i].tokval!=0)
  {
    if ((cmds[i].tokval == tok)&&(cmds[i].alias==0))
    {
      return cmds[i].name;
    }
    i++;
  }
  return cmds[0].name;
}
/*---------------------------------------------------------------------*/
/**
 * @brief compares to entry of cmdsname-list

 @param[in] a
 @param[in] b

 @return <ReturnValue>
**/
/*---------------------------------------------------------------------*/
static int _gentable_sort_cmds( const void *a, const void *b )
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
  if (pCmdL->tokval==-1)
  {
    if (pCmdR->tokval==-1)
       return strcmp(pCmdL->name, pCmdR->name);
    /* pCmdL->tokval==-1, pCmdL goes at the end */
    return 1;
  }
  /* pCmdR->tokval==-1, pCmdR goes at the end */
  if(pCmdR->tokval==-1) return -1;

  return strcmp(pCmdL->name, pCmdR->name);
}

/*generic*/
const char * iiTwoOps(int t)
{
  if (t<127)
  {
    static char ch[2];
    switch (t)
    {
      case '&':
        return "and";
      case '|':
        return "or";
      default:
        ch[0]=t;
        ch[1]='\0';
        return ch;
    }
  }
  switch (t)
  {
    case COLONCOLON:  return "::";
    case DOTDOT:      return "..";
    //case PLUSEQUAL:   return "+=";
    //case MINUSEQUAL:  return "-=";
    case MINUSMINUS:  return "--";
    case PLUSPLUS:    return "++";
    case EQUAL_EQUAL: return "==";
    case LE:          return "<=";
    case GE:          return ">=";
    case NOTEQUAL:    return "<>";
    default:          return Tok2Cmdname(t);
  }
}
//
// automatic conversions:
//
/*2
* try to convert 'inputType' in 'outputType'
* return 0 on failure, an index (<>0) on success
*/
int iiTestConvert (int inputType, int outputType)
{
  if ((inputType==outputType)
  || (outputType==DEF_CMD)
  || (outputType==IDHDL)
  || (outputType==ANY_TYPE))
  {
    return -1;
  }

  // search the list
  int i=0;
  while (dConvertTypes[i].i_typ!=0)
  {
    if((dConvertTypes[i].i_typ==inputType)
    &&(dConvertTypes[i].o_typ==outputType))
    {
      //Print("test convert %d to %d (%s -> %s):%d\n",inputType,outputType,
      //Tok2Cmdname(inputType), Tok2Cmdname(outputType),i+1);
      return i+1;
    }
    i++;
  }
  //Print("test convert %d to %d (%s -> %s):0\n",inputType,outputType,
  // Tok2Cmdname(inputType), Tok2Cmdname(outputType));
  return 0;
}
char *iparith_inc;
void ttGen1()
{
  iparith_inc=strdup("iparith.xxxxxx");
  int pid=getpid();
  iparith_inc[8]=(pid %10)+'0'; pid/=10;
  iparith_inc[9]=(pid %10)+'0'; pid/=10;
  iparith_inc[10]=(pid %10)+'0'; pid/=10;
  iparith_inc[11]=(pid %10)+'0'; pid/=10;
  iparith_inc[12]=(pid %10)+'0'; pid/=10;
  iparith_inc[13]=(pid %10)+'0';
  FILE *outfile = fopen(iparith_inc,"w");
  int i,j,l1=0,l2=0;
  fprintf(outfile,
  "/****************************************\n"
  "*  Computer Algebra System SINGULAR     *\n"
  "****************************************/\n\n");
/*-------------------------------------------------------------------*/
  fprintf(outfile,"// syntax table for Singular\n//\n");
  fprintf(outfile,"// - search for an exact match of the argument types\n");
  fprintf(outfile,"// - otherwise search for the first possibility\n");
  fprintf(outfile,"//   with converted types of the arguments\n");
  fprintf(outfile,"// - otherwise report an error\n//\n");

  int op;
  i=0;
  while ((op=dArith1[i].cmd)!=0)
  {
    if (dArith1[i].p==jjWRONG)
      fprintf(outfile,"// DUMMY ");
    const char *s = iiTwoOps(op);
    fprintf(outfile,"// operation: %s (%s)  ->  %s\n",
          s,
          Tok2Cmdname(dArith1[i].arg),
          Tok2Cmdname(ABS(dArith1[i].res)));
    i++;
  }
  fprintf(outfile,"/*---------------------------------------------*/\n");
  i=0;
  while ((op=dArith2[i].cmd)!=0)
  {
    if (dArith2[i].p==jjWRONG2)
      fprintf(outfile,"// DUMMY ");
    const char *s = iiTwoOps(op);
    fprintf(outfile,"// operation: %s (%s, %s)  ->  %s\n",
          s,
          Tok2Cmdname(dArith2[i].arg1),
          Tok2Cmdname(dArith2[i].arg2),
          Tok2Cmdname(dArith2[i].res));
    i++;
  }
  fprintf(outfile,"/*---------------------------------------------*/\n");
  i=0;
  while ((op=dArith3[i].cmd)!=0)
  {
    const char *s = iiTwoOps(op);
    if (dArith3[i].p==jjWRONG3)
      fprintf(outfile,"// DUMMY ");
    fprintf(outfile,"// operation: %s (%s, %s, %s)  ->  %s\n",
          s,
          Tok2Cmdname(dArith3[i].arg1),
          Tok2Cmdname(dArith3[i].arg2),
          Tok2Cmdname(dArith3[i].arg3),
          Tok2Cmdname(dArith3[i].res));
    i++;
  }
  fprintf(outfile,"/*---------------------------------------------*/\n");
  i=0;
  while ((op=dArithM[i].cmd)!=0)
  {
    const char *s = iiTwoOps(op);
    fprintf(outfile,"// operation: %s (...)  ->  %s",
          s,
          Tok2Cmdname(dArithM[i].res));
    switch(dArithM[i].number_of_args)
    {
      case -2:
         fprintf(outfile," ( number of arguments >0 )\n");
         break;
      case -1:
         fprintf(outfile," ( any number of arguments )\n");
         break;
      default:
         fprintf(outfile," ( %d arguments )\n",dArithM[i].number_of_args);
         break;
    }
    i++;
  }
  fprintf(outfile,"/*---------------------------------------------*/\n");
  i=0;
  while ((op=dAssign[i].res)!=0)
  {
    fprintf(outfile,"// assign: %s =  %s\n",
          Tok2Cmdname(op/*dAssign[i].res*/),
          Tok2Cmdname(dAssign[i].arg));
    i++;
  }
/*-------------------------------------------------------------------*/
  fprintf(outfile,"/*---------------------------------------------*/\n");
  for (j=257;j<=MAX_TOK+1;j++)
  {
    for(i=257;i<=MAX_TOK+1;i++)
    {
      if ((i!=j) && (j!=IDHDL) && (j!=DEF_CMD) && (j!=ANY_TYPE)
      && iiTestConvert(i,j))
      {
        fprintf(outfile,"// convert %s -> %s\n",
          Tok2Cmdname(i), Tok2Cmdname(j));
        if (j==ANY_TYPE) break;
      }
    }
  }
  fprintf(outfile,"/*---------------------------------------------*/\n");
  char ops[]="=><+*/[.^,%(;";
  for(i=0;ops[i]!='\0';i++)
    fprintf(outfile,"// token %d : %c\n", (int)ops[i], ops[i]);
  for (i=257;i<=MAX_TOK;i++)
  {
    const char *s=iiTwoOps(i);
    if (s[0]!='$')
    {
      fprintf(outfile,"// token %d : %s\n", i, s);
    }
  }
/*-------------------------------------------------------------------*/
  fprintf(outfile,"/*--max. token: %d, gr: %d --*/\n",MAX_TOK,UMINUS);
/*-------------------------------------------------------------------*/
  fprintf(outfile,"/*---------------------------------------------*/\n");
  fprintf(outfile,
  "struct sValCmdTab dArithTab1[]=\n"
  "{\n");
  for (j=1;j<=MAX_TOK+1;j++)
  {
    for(i=0;dArith1[i].cmd!=0;i++)
    {
      if (dArith1[i].cmd==j)
      {
        fprintf(outfile," { %d,%d },\n",j,i);
        l1++;
        break;
      }
    }
  }
  fprintf(outfile," { 10000,0 }\n};\n");
  fprintf(outfile,"#define JJTAB1LEN %d\n",l1);
/*-------------------------------------------------------------------*/
  fprintf(outfile,
  "struct sValCmdTab dArithTab2[]=\n"
  "{\n");
  for (j=1;j<=MAX_TOK+1;j++)
  {
    for(i=0;dArith2[i].cmd!=0;i++)
    {
      if (dArith2[i].cmd==j)
      {
        fprintf(outfile," { %d,%d },\n",j,i);
        l2++;
        break;
      }
    }
  }
  fprintf(outfile," { 10000,0 }\n};\n");
  fprintf(outfile,"#define JJTAB2LEN %d\n",l2);
  fclose(outfile);
}
/*-------------------------------------------------------------------*/
#if 0
void ttGen2()
{
  FILE *outfile = fopen(iparith_inc,"a");
  fprintf(outfile,
  "/****************************************\n"
  "*  Computer Algebra System SINGULAR     *\n"
  "****************************************/\n\n");
/*-------------------------------------------------------------------*/
  fprintf(outfile,"// identifier table for Singular\n//\n");

  fprintf(outfile,
  "cmdnames OLDcmds[] =\n"
  "{  // name-string     alias  tokval toktype\n"
  "{ \"$INVALID$\",            0,  -1, 0},\n");
  int i=1;
  int m=-1;
  int id_nr=0;
  BOOLEAN f=FALSE;
  loop
  {
    while (cmds[i].tokval!=0)
    {
      if ((cmds[i].tokval!=-1)&&(cmds[i].name!=NULL))
      {
        if(m==-1)
        {
          m=i;
          f=TRUE;
        }
        else if(strcmp(cmds[m].name,cmds[i].name)>0)
        {
          m=i;
          f=TRUE;
        }
      }
      i++;
    }
    if(f)
    {
      id_nr++;
      fprintf(outfile,"  {\"%s\", %*d, %3d, ",cmds[m].name,
                                             20-strlen(cmds[m].name),
                                             cmds[m].alias,
                                             cmds[m].tokval);
      switch(cmds[m].toktype)
      {
        case CMD_1:            fprintf(outfile,"CMD_1 },\n"); break;
        case CMD_2:            fprintf(outfile,"CMD_2 },\n"); break;
        case CMD_3:            fprintf(outfile,"CMD_3 },\n"); break;
        case CMD_12:           fprintf(outfile,"CMD_12 },\n"); break;
        case CMD_123 :         fprintf(outfile,"CMD_123 },\n"); break;
        case CMD_23:           fprintf(outfile,"CMD_23 },\n"); break;
        case CMD_M:            fprintf(outfile,"CMD_M },\n"); break;
        case SYSVAR:           fprintf(outfile,"SYSVAR },\n"); break;
        case ROOT_DECL:        fprintf(outfile,"ROOT_DECL },\n"); break;
        case ROOT_DECL_LIST:   fprintf(outfile,"ROOT_DECL_LIST },\n"); break;
        case RING_DECL:        fprintf(outfile,"RING_DECL },\n"); break;
        case NONE:             fprintf(outfile,"NONE },\n"); break;
        default:               if((cmds[m].toktype>' ')
                               &&(cmds[m].toktype<127))
                               {
                                 fprintf(outfile,"'%c' },\n",cmds[m].toktype);
                               }
                               else
                               {
                                 fprintf(outfile,"%d },\n",cmds[m].toktype);
                               }
                               break;
      }
      cmds[m].name=NULL;
      m=-1;
      i=1;
      f=FALSE;
    }
    else break;
  }
  fprintf(outfile,
"/* list of scanner identifiers/only for feread/reservedName */\n");
  f=FALSE;
  i=1;m=-1;
  loop
  {
    while (cmds[i].tokval!=0)
    {
      if (cmds[i].name!=NULL)
      {
        if(m==-1)
        {
          m=i;
          f=TRUE;
        }
        else if(strcmp(cmds[m].name,cmds[i].name)>0)
        {
          m=i;
          f=TRUE;
        }
      }
      i++;
    }
    if(f)
    {
      fprintf(outfile,"  {\"%s\", %*d,  -1, 0 },\n",cmds[m].name,
                                             20-strlen(cmds[m].name),
                                             0/*cmds[m].alias*/
                                             /*-1 cmds[m].tokval*/
                                             /*0 cmds[m].toktype*/);
      cmds[m].name=NULL;
      m=-1;
      i=1;
      f=FALSE;
    }
    else break;
  }
  fprintf(outfile,
"/* end of list marker */\n"
"  { NULL, 0, 0, 0}\n"
"};\n"
"#define LAST_IDENTIFIER %d\n"
  ,id_nr);
  fclose(outfile);
}
#endif
/*---------------------------------------------------------------------*/
/**
 * @brief generate cmds initialisation
**/
/*---------------------------------------------------------------------*/

void ttGen2b()
{
  int cmd_size = (sizeof(cmds)/sizeof(cmdnames))-1;

  FILE *outfile = fopen(iparith_inc,"a");
  fprintf(outfile,
  "/****************************************\n"
  "*  Computer Algebra System SINGULAR     *\n"
  "****************************************/\n\n");
/*-------------------------------------------------------------------*/
  fprintf(outfile,"// identifier table for Singular\n//\n");

  fprintf(
    outfile,
    "#ifdef MODULE_GENERATOR\n"
    "#define omAlloc0(A) malloc(A)\n"
    "#endif\n"
    "void iiInitCmdName()\n{\n"
    "  sArithBase.nCmdUsed      = 0;\n"
    "  sArithBase.nCmdAllocated = %d;\n"
    "  sArithBase.sCmds = (cmdnames*)omAlloc0(sArithBase.nCmdAllocated*sizeof(cmdnames));\n"
    "\n"
    "  // name-string                   alias  tokval toktype index\n",
    cmd_size);
  int m=0;
  int id_nr=0;

  qsort(&cmds, cmd_size, sizeof(cmdnames), (&_gentable_sort_cmds));

  for(m=0; m<cmd_size; m++)
  {
    if(cmds[m].tokval>0) id_nr++;
    fprintf(outfile,"  iiArithAddCmd(\"%s\", %*d, %3d, ",cmds[m].name,
            (int)(20-strlen(cmds[m].name)),
            cmds[m].alias,
            cmds[m].tokval);
    switch(cmds[m].toktype)
    {
        case CMD_1:            fprintf(outfile,"CMD_1"); break;
        case CMD_2:            fprintf(outfile,"CMD_2"); break;
        case CMD_3:            fprintf(outfile,"CMD_3"); break;
        case CMD_12:           fprintf(outfile,"CMD_12"); break;
        case CMD_123 :         fprintf(outfile,"CMD_123"); break;
        case CMD_23:           fprintf(outfile,"CMD_23"); break;
        case CMD_M:            fprintf(outfile,"CMD_M"); break;
        case SYSVAR:           fprintf(outfile,"SYSVAR"); break;
        case ROOT_DECL:        fprintf(outfile,"ROOT_DECL"); break;
        case ROOT_DECL_LIST:   fprintf(outfile,"ROOT_DECL_LIST"); break;
        case RING_DECL:        fprintf(outfile,"RING_DECL"); break;
        case NONE:             fprintf(outfile,"NONE"); break;
        default:
          if((cmds[m].toktype>' ') &&(cmds[m].toktype<127))
          {
            fprintf(outfile,"'%c'",cmds[m].toktype);
          }
          else
          {
            fprintf(outfile,"%d",cmds[m].toktype);
          }
          break;
#if 0
          fprintf(outfile,"  iiArithAddCmd(\"%s\", %*d,  -1, 0 );\n",
              cmds[m].name, 20-strlen(cmds[m].name),
              0/*cmds[m].alias*/
              /*-1 cmds[m].tokval*/
              /*0 cmds[m].toktype*/);
#endif
    }
    fprintf(outfile,", %d);\n", m);
  }
  fprintf(outfile, "/* end of list marker */\n");
  fprintf(outfile,
          "  sArithBase.nLastIdentifier = %d;\n",
          id_nr);


  fprintf(outfile,
"}\n"
"#define LAST_IDENTIFIER %d\n"
  ,id_nr);
  fclose(outfile);
}
/*-------------------------------------------------------------------*/
#if 0
void ttGen3()
{
  FILE *outfile = fopen("mpsr_tok.inc","w");
  fprintf(outfile,
  "/****************************************\n"
  "*  Computer Algebra System SINGULAR     *\n"
  "****************************************/\n\n");
/*-------------------------------------------------------------------*/
  fprintf(outfile,"// token table for Singular\n//\n");

  fprintf(outfile,
  "short vtok[] =\n"
  "{\n");
  // operations with 1 arg: ===========================================
  int i=0;
  while (dArith1[i].cmd!=0)
  {
    if ((dArith1[i].p!=jjWRONG)
    &&((i==0)||(dArith1[i].cmd!=dArith1[i-1].cmd)))
    {
      fprintf(outfile,"  %d,\n",dArith1[i].cmd);
    }
    i++;
  }
  // operations with 2 args: ===========================================
  i=0;
  while (dArith2[i].cmd!=0)
  {
    if ((dArith2[i].p!=jjWRONG2)
    &&((i==0)||(dArith2[i].cmd!=dArith2[i-1].cmd)))
    {
      fprintf(outfile,"  %d,\n",dArith2[i].cmd);
    }
    i++;
  }
  // operations with 3 args: ===========================================
  i=0;
  while (dArith3[i].cmd!=0)
  {
    if (
    ((i==0)||(dArith3[i].cmd!=dArith3[i-1].cmd)))
    {
      fprintf(outfile,"  %d,\n",dArith3[i].cmd);
    }
    i++;
  }
  // operations with many args: ===========================================
  i=0;
  while (dArithM[i].cmd!=0)
  {
    if (
    ((i==0)||(dArithM[i].cmd!=dArithM[i-1].cmd)))
    {
      fprintf(outfile,"  %d,\n",dArithM[i].cmd);
    }
    i++;
  }
  // ====================================================================
  fprintf(outfile,
  "/* end of list marker */\n"
  " %d };\n",MAX_TOK);
  fclose(outfile);
}
#endif
void ttGen4()
{
  FILE *outfile = fopen("plural_cmd.xx","w");
  int i;
  const char *old_s="";
  fprintf(outfile,
  "@c *****************************************\n"
  "@c *  Computer Algebra System SINGULAR     *\n"
  "@c *****************************************\n\n");
/*-------------------------------------------------------------------*/
  fprintf(outfile,"@multicolumn .45 .45\n");
  int op;
  i=0;
  while ((op=dArith1[i].cmd)!=0)
  {
    if (dArith1[i].p!=jjWRONG)
    {
      const char *s = iiTwoOps(op);
      if ((s!=NULL) && (isalpha(s[0])) && (strcmp(s,old_s)!=0))
      {
        old_s=s;
        #ifdef HAVE_PLURAL
        switch (dArith1[i].valid_for & PLURAL_MASK)
        {
          case NO_PLURAL:
            fprintf(outfile,"@item @ref{%s} @tab @code{---}\n",s);
            break;
          case ALLOW_PLURAL:
            fprintf(outfile,"@item @ref{%s} @tab @ref{%s (plural)}\n",s,s);
            break;
          case COMM_PLURAL:
            fprintf(outfile,"@item @ref{%s} @tab %s\n",s,s);
            break;
        }
        #endif
        #ifdef HAVE_RINGS
        #endif
      }
    }
    i++;
  }
  fprintf(outfile,"@c ---------------------------------------------\n");
  i=0;
  while ((op=dArith2[i].cmd)!=0)
  {
    if (dArith2[i].p!=jjWRONG2)
    {
      const char *s = iiTwoOps(op);
      if ((s!=NULL) && (isalpha(s[0])) && (strcmp(s,old_s)!=0))
      {
        old_s=s;
        #ifdef HAVE_PLURAL
        switch (dArith2[i].valid_for & PLURAL_MASK)
        {
          case NO_PLURAL:
            fprintf(outfile,"@item @ref{%s} @tab @code{---}\n",s);
            break;
          case ALLOW_PLURAL:
            fprintf(outfile,"@item @ref{%s} @tab @ref{%s (plural)}\n",s,s);
            break;
          case COMM_PLURAL:
            fprintf(outfile,"@item @ref{%s} @tab %s\n",s,s);
            break;
        }
        #endif
        #ifdef HAVE_RINGS
        #endif
      }
    }
    i++;
  }
  fprintf(outfile,"@c ---------------------------------------------\n");
  i=0;
  while ((op=dArith3[i].cmd)!=0)
  {
    const char *s = iiTwoOps(op);
    if (dArith3[i].p!=jjWRONG3)
    {
      if ((s!=NULL) && (isalpha(s[0])) && (strcmp(s,old_s)!=0))
      {
        old_s=s;
        #ifdef HAVE_PLURAL
        switch (dArith3[i].valid_for & PLURAL_MASK)
        {
          case NO_PLURAL:
            fprintf(outfile,"@item @ref{%s} @tab @code{---}\n",s);
            break;
          case ALLOW_PLURAL:
            fprintf(outfile,"@item @ref{%s} @tab @ref{%s (plural)}\n",s,s);
            break;
          case COMM_PLURAL:
            fprintf(outfile,"@item @ref{%s} @tab %s\n",s,s);
            break;
        }
        #endif
        #ifdef HAVE_RINGS
        #endif
      }
    }
    i++;
  }
  fprintf(outfile,"@c ---------------------------------------------\n");
  i=0;
  while ((op=dArithM[i].cmd)!=0)
  {
    const char *s = iiTwoOps(op);
    if ((s!=NULL) && (isalpha(s[0])) && (strcmp(s,old_s)!=0))
    {
        old_s=s;
        #ifdef HAVE_PLURAL
        switch (dArithM[i].valid_for & PLURAL_MASK)
        {
          case NO_PLURAL:
            fprintf(outfile,"@item @ref{%s} @tab @code{---}\n",s);
            break;
          case ALLOW_PLURAL:
            fprintf(outfile,"@item @ref{%s} @tab @ref{%s (plural)}\n",s,s);
            break;
          case COMM_PLURAL:
            fprintf(outfile,"@item @ref{%s} @tab %s\n",s,s);
            break;
        }
        #endif
        #ifdef HAVE_RINGS
        #endif
    }
    i++;
  }
  fprintf(outfile,"@c ---------------------------------------------\n");
  fprintf(outfile,"@end table\n");
  fclose(outfile);
}
/*-------------------------------------------------------------------*/

  // some special cmds which do not fit in with the others, and
  // nevertheless need to be transmitted
short ExtraCmds[] =
{
  OPTION_CMD,
  NAMES_CMD,
//  RESERVEDNAME_CMD,
  PROC_CMD,
  MAP_CMD,
  PACKAGE_CMD,
  '=',
  0
};

// This returns 1 if tok is a token which can appear in a Singular
// (quoted) command, and 0 otherwise
short IsCmdToken(short tok)
{
  int i = 0;
  // cmds with one arg
  while (dArith1[i].cmd != 0)
    if (dArith1[i].cmd == tok) return 1;
    else i++;

  // cmds with two args
  i=0;
  while (dArith2[i].cmd != 0)
    if (dArith2[i].cmd == tok) return 1;
    else i++;

  // cmds with three args
  i=0;
  while (dArith3[i].cmd != 0)
    if (dArith3[i].cmd == tok) return 1;
    else i++;

  // cmds with many args
  i=0;
  while (dArithM[i].cmd != 0)
    if (dArithM[i].cmd == tok) return 1;
    else i++;

  // cmds which are somewhat special (like those having 0 args)
  i=0;
  while (ExtraCmds[i] != 0)
    if (ExtraCmds[i] == tok) return 1;
    else i++;

  return 0;
}

int main()
{
  ttGen4();
  ttGen1();
  ttGen2b();
  rename(iparith_inc,"iparith.inc");
  rename("plural_cmd.xx","plural_cmd.inc");
  return 0;
}
