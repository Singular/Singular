/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/*
* ABSTRACT: generate iparith.inc etc.
*/

#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/stat.h>

// need global defines:
#include "kernel/mod2.h"
// need to include all tokens: *_CMD:
#include "Singular/tok.h"

#ifdef HAVE_RINGS
  #define RING_MASK        4
  #define ZERODIVISOR_MASK 8
#else
  #define RING_MASK        0
  #define ZERODIVISOR_MASK 0
#endif

inline int RingDependend(int t) { return (BEGIN_RING<t)&&(t<END_RING); }

// to produce convert_table.texi for doc:
THREAD_VAR int produce_convert_table=0;

// bits 0,1 for PLURAL
#define NO_NC            0
#define ALLOW_PLURAL     1
#define COMM_PLURAL      2
// bit 6: non-commutative letterplace
#define ALLOW_LP         64
#define NC_MASK          (3+64)
#define ALLOW_NC         ALLOW_LP|ALLOW_PLURAL

// bit 2 for RING-CF
#define ALLOW_RING       4
#define NO_RING          0

// bit 3 for zerodivisors
#define NO_ZERODIVISOR   8
#define ALLOW_ZERODIVISOR  0
#define ZERODIVISOR_MASK 8

#define ALLOW_ZZ (ALLOW_RING|NO_ZERODIVISOR)

// bit 4 for warning, if used at toplevel
#define WARN_RING        16
// bit 5: do no try automatic conversions
#define NO_CONVERSION    32

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


#define jjWRONG   1
#define jjWRONG2  1
#define jjWRONG3  1

#define D(A)     2
#define NULL_VAL 0
#define IPARITH
#define GENTABLE
#define IPCONV
#define IPASSIGN

#include "table.h"

const char * Tok2Cmdname(int tok)
{
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
  int i = 0;
  while (cmds[i].tokval!=0)
  {
    if ((cmds[i].tokval == tok)&&(cmds[i].alias==0))
    {
      return cmds[i].name;
    }
    i++;
  }
  i=0;// try again for old/alias names:
  while (cmds[i].tokval!=0)
  {
    if (cmds[i].tokval == tok)
    {
      return cmds[i].name;
    }
    i++;
  }
  #if 0
  char *s=(char*)malloc(10);
  sprintf(s,"(%d)",tok);
  return s;
  #else
  return cmds[0].name;
  #endif
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

static int _texi_sort_cmds( const void *a, const void *b )
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
  char *ls=strdup(pCmdL->name);
  char *rs=strdup(pCmdR->name);
  char *s=ls;
  while (*s) { *s=tolower(*s); s++; }
  s=rs;
  while (*s) { *s=tolower(*s); s++; }

  /* tokval=-1 are reserved names at the end */
  if (pCmdL->tokval==-1)
  {
    if (pCmdR->tokval==-1)
       { int r=strcmp(ls,rs); free(ls); free(rs); return r; }
    /* pCmdL->tokval==-1, pCmdL goes at the end */
    free(ls);free(rs);
    return 1;
  }
  /* pCmdR->tokval==-1, pCmdR goes at the end */
  if(pCmdR->tokval==-1)
  { free(ls);free(rs);return -1;}

  { int r=strcmp(ls,rs); free(ls); free(rs); return r; }
}

/*generic*/
const char * iiTwoOps(int t)
{
  if (t<127)
  {
    THREAD_VAR static char ch[2];
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
* GENTABLE variant!
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
  if (inputType==UNKNOWN) return 0;

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
THREAD_VAR char *iparith_inc;
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
    fprintf(outfile,"// operation: %s (%s)  ->  %s",
          s,
          Tok2Cmdname(dArith1[i].arg),
          Tok2Cmdname(dArith1[i].res));
    if (RingDependend(dArith1[i].res) && (!RingDependend(dArith1[i].arg)))
      fprintf(outfile," requires currRing");
    if ((dArith1[i].valid_for & NC_MASK)==2)
      fprintf(outfile,", commutative subalgebra");
    else if ((dArith1[i].valid_for & NC_MASK)==ALLOW_LP)
      fprintf(outfile,", letterplace rings");
    else if ((dArith1[i].valid_for & NC_MASK)==0)
      fprintf(outfile,", only commutative rings");
    if ((dArith1[i].valid_for & RING_MASK)==0)
      fprintf(outfile,", field coeffs");
    else if ((dArith1[i].valid_for & ZERODIVISOR_MASK)==NO_ZERODIVISOR)
      fprintf(outfile,", domain coeffs");
    else if ((dArith1[i].valid_for & WARN_RING)==WARN_RING)
      fprintf(outfile,", QQ coeffs");

    fprintf(outfile,"\n");
    i++;
  }
  fprintf(outfile,"/*---------------------------------------------*/\n");
  i=0;
  while ((op=dArith2[i].cmd)!=0)
  {
    if (dArith2[i].p==jjWRONG2)
      fprintf(outfile,"// DUMMY ");
    const char *s = iiTwoOps(op);
    fprintf(outfile,"// operation: %s (%s, %s)  ->  %s",
          s,
          Tok2Cmdname(dArith2[i].arg1),
          Tok2Cmdname(dArith2[i].arg2),
          Tok2Cmdname(dArith2[i].res));
    if (RingDependend(dArith2[i].res)
       && (!RingDependend(dArith2[i].arg1))
       && (!RingDependend(dArith2[i].arg2)))
    {
      fprintf(outfile," requires currRing");
    }
    if ((dArith2[i].valid_for & NC_MASK)==COMM_PLURAL)
      fprintf(outfile,", commutative subalgebra");
    else if ((dArith2[i].valid_for & NC_MASK)==0)
      fprintf(outfile,", only commutative rings");
    if ((dArith2[i].valid_for & RING_MASK)==0)
      fprintf(outfile,", field coeffs");
    else if ((dArith2[i].valid_for & ZERODIVISOR_MASK)==NO_ZERODIVISOR)
      fprintf(outfile,", domain coeffs");
    else if ((dArith2[i].valid_for & WARN_RING)==WARN_RING)
      fprintf(outfile,", QQ coeffs");

    fprintf(outfile,"\n");
    i++;
  }
  fprintf(outfile,"/*---------------------------------------------*/\n");
  i=0;
  while ((op=dArith3[i].cmd)!=0)
  {
    const char *s = iiTwoOps(op);
    if (dArith3[i].p==jjWRONG3)
      fprintf(outfile,"// DUMMY ");
    fprintf(outfile,"// operation: %s (%s, %s, %s)  ->  %s",
          s,
          Tok2Cmdname(dArith3[i].arg1),
          Tok2Cmdname(dArith3[i].arg2),
          Tok2Cmdname(dArith3[i].arg3),
          Tok2Cmdname(dArith3[i].res));
    if (RingDependend(dArith3[i].res)
       && (!RingDependend(dArith3[i].arg1))
       && (!RingDependend(dArith3[i].arg2))
       && (!RingDependend(dArith3[i].arg3)))
    {
      fprintf(outfile," requires currRing");
    }
    if ((dArith3[i].valid_for & NC_MASK)==COMM_PLURAL)
      fprintf(outfile,", commutative subalgebra");
    else if ((dArith3[i].valid_for & NC_MASK)==0)
      fprintf(outfile,", only commutative rings");
    if ((dArith3[i].valid_for & RING_MASK)==0)
      fprintf(outfile,", field coeffs");
    else if ((dArith3[i].valid_for & ZERODIVISOR_MASK)==NO_ZERODIVISOR)
      fprintf(outfile,", domain coeffs");
    else if ((dArith3[i].valid_for & WARN_RING)==WARN_RING)
      fprintf(outfile,", QQ coeffs");

    fprintf(outfile,"\n");
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
  FILE *doctable=NULL; /*to silence "may be used uninitialized"*/
  if (produce_convert_table)
  {
    doctable=fopen("convert_table.texi","w");
    fprintf(doctable,"@multitable @columnfractions .05 .18 .81\n");
  }
  int doc_nr=1;
  for (j=257;j<=MAX_TOK+1;j++)
  {
    for(i=257;i<=MAX_TOK+1;i++)
    {
      if ((i!=j) && (j!=IDHDL) && (j!=DEF_CMD) && (j!=ANY_TYPE)
      && iiTestConvert(i,j))
      {
        fprintf(outfile,"// convert %s -> %s\n",
          Tok2Cmdname(i), Tok2Cmdname(j));
        if (produce_convert_table)
        {
          fprintf(doctable,
          "@item\n@   %d. @tab @code{%s}  @tab @expansion{} @code{%s}\n",
          doc_nr,Tok2Cmdname(i),Tok2Cmdname(j));
          doc_nr++;
        }
        if (j==ANY_TYPE) break;
      }
    }
  }
  if (produce_convert_table)
  {
    fprintf(doctable,"@end multitable\n");
    fclose(doctable);
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
  "const struct sValCmdTab dArithTab1[]=\n"
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
  "const struct sValCmdTab dArithTab2[]=\n"
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
int is_ref_cmd(cmdnames *c)
{
  if( c->tokval==0) return 0;
  if (c->alias > 0) return 0;
  if  ((c->toktype==CMD_1)
        || (c->toktype==CMD_2)
        || (c->toktype==CMD_3)
        || (c->toktype==CMD_M)
        || (c->toktype==CMD_12)
        || (c->toktype==CMD_13)
        || (c->toktype==CMD_23)
        || (c->toktype==CMD_123)) return 1;
  return 0;
}
void ttGen2c()
{
  int cmd_size = (sizeof(cmds)/sizeof(cmdnames))-1;

  FILE *outfile = fopen("reference_table.texi","w");
  fprintf(outfile, "@menu\n");
/*-------------------------------------------------------------------*/
  qsort(&cmds, cmd_size, sizeof(cmdnames), (&_texi_sort_cmds));

  int m;
  for(m=0; m<cmd_size; m++)
  {
    // assume that cmds[0].tokval== -1 and all others with tokval -1 at the end
    if(is_ref_cmd(&(cmds[m])))
    {
      fprintf(outfile,"* %s::\n",cmds[m].name);
    }
  }
  fprintf(outfile, "@end menu\n@c ---------------------------\n");
  for(m=0; m<cmd_size; m++)
  {
    // assume that cmds[0].tokval== -1 and all others with tokval -1 at the end
    if(is_ref_cmd(&(cmds[m])))
    {
      fprintf(outfile,"@node %s,",cmds[m].name);
      // next:
      int mm=m-1;
      while((mm>0)&& (is_ref_cmd(&cmds[mm]))) mm--;
      if((mm>0)&& (is_ref_cmd(&cmds[mm])))
        fprintf(outfile,"%s,",cmds[mm].name);
      else
        fprintf(outfile,",");
      // prev:
      mm=m+1;
      while((mm>0)&& (is_ref_cmd(&cmds[mm]))) mm++;
      if((mm>0)&& (is_ref_cmd(&cmds[mm])))
        fprintf(outfile,"%s,",cmds[m-1].name);
      else
        fprintf(outfile,",");
      // up:, and header
      fprintf(outfile,"Functions\n"
      "@subsection %s\n"
      "@cindex %s\n",cmds[m].name,cmds[m].name);
      fprintf(outfile,"@include %s.part\n",cmds[m].name);
      char partName[50];
      sprintf(partName,"%s.part",cmds[m].name);
      struct stat buf;
      if (lstat(partName,&buf)!=0)
      {
        int op,i;
        int only_field=0,only_comm=0,no_zerodiv=0;
        FILE *part=fopen(partName,"w");
        fprintf(part,"@table @code\n@item @strong{Syntax:}\n");
        if ((cmds[m].toktype==CMD_1)
        || (cmds[m].toktype==CMD_12)
        || (cmds[m].toktype==CMD_13)
        || (cmds[m].toktype==CMD_123))
        {
          op= cmds[m].tokval;
          i=0;
          while ((dArith1[i].cmd!=op) && (dArith1[i].cmd!=0)) i++;
          while (dArith1[i].cmd==op)
          {
            if (dArith1[i].p!=jjWRONG)
            {
              fprintf(part,"@code{%s (} %s @code{)}\n",cmds[m].name,Tok2Cmdname(dArith1[i].arg));
              fprintf(part,"@item @strong{Type:}\n%s\n",Tok2Cmdname(dArith1[i].res));
              if ((dArith1[i].valid_for & ALLOW_PLURAL)==0)
                only_comm=1;
              if ((dArith1[i].valid_for & ALLOW_RING)==0)
                only_field=1;
              if ((dArith1[i].valid_for & ZERODIVISOR_MASK)==NO_ZERODIVISOR)
                no_zerodiv=1;
            }
            i++;
          }
        }
        if ((cmds[m].toktype==CMD_23)
        || (cmds[m].toktype==CMD_12)
        || (cmds[m].toktype==CMD_2)
        || (cmds[m].toktype==CMD_123))
        {
          op= cmds[m].tokval;
          i=0;
          while ((dArith2[i].cmd!=op) && (dArith2[i].cmd!=0)) i++;
          while (dArith2[i].cmd==op)
          {
            if (dArith2[i].p!=jjWRONG)
            {
              fprintf(part,"@code{%s (} %s, %s @code{)}\n",cmds[m].name,Tok2Cmdname(dArith2[i].arg1),Tok2Cmdname(dArith2[i].arg2));
              fprintf(part,"@item @strong{Type:}\n%s\n",Tok2Cmdname(dArith2[i].res));
              if ((dArith2[i].valid_for & ALLOW_PLURAL)==0)
                 only_comm=1;
              if ((dArith2[i].valid_for & ALLOW_RING)==0)
                 only_field=1;
              if ((dArith2[i].valid_for & ZERODIVISOR_MASK)==NO_ZERODIVISOR)
                no_zerodiv=1;
            }
            i++;
          }
        }
        if ((cmds[m].toktype==CMD_23)
        || (cmds[m].toktype==CMD_13)
        || (cmds[m].toktype==CMD_3)
        || (cmds[m].toktype==CMD_123))
        {
          op= cmds[m].tokval;
          i=0;
          while ((dArith3[i].cmd!=op) && (dArith3[i].cmd!=0)) i++;
          while (dArith3[i].cmd==op)
          {
            if (dArith3[i].p!=jjWRONG)
            {
              fprintf(part,"@code{%s (} %s, %s, %s @code{)}\n",cmds[m].name,
                Tok2Cmdname(dArith3[i].arg1),
                Tok2Cmdname(dArith3[i].arg2),
                Tok2Cmdname(dArith3[i].arg3));
              fprintf(part,"@item @strong{Type:}\n%s\n",Tok2Cmdname(dArith3[i].res));
              if ((dArith3[i].valid_for & ALLOW_PLURAL)==0)
                only_comm=1;
              if ((dArith3[i].valid_for & ALLOW_RING)==0)
                only_field=1;
              if ((dArith3[i].valid_for & ZERODIVISOR_MASK)==NO_ZERODIVISOR)
                no_zerodiv=1;
            }
            i++;
          }
        }
        if (cmds[m].toktype==CMD_M)
        {
          op= cmds[m].tokval;
          i=0;
          while ((dArithM[i].cmd!=op) && (dArithM[i].cmd!=0)) i++;
          while (dArithM[i].cmd==op)
          {
            if (dArithM[i].p!=jjWRONG)
            {
              fprintf(part,"@code{%s (} ... @code{)}\n",cmds[m].name);
              fprintf(part,"@item @strong{Type:}\n%s\n",Tok2Cmdname(dArithM[i].res));
              if ((dArithM[i].valid_for & ALLOW_PLURAL)==0)
                only_comm=1;
              if ((dArithM[i].valid_for & ALLOW_RING)==0)
                only_field=1;
              if ((dArithM[i].valid_for & ZERODIVISOR_MASK)==NO_ZERODIVISOR)
                no_zerodiv=1;
            }
            i++;
          }
        }
        if (only_comm)
              fprintf(part,"@item @strong{Remark:}\n"
                           "only for commutive polynomial rings\n");
        if (only_field)
              fprintf(part,"@item @strong{Remark:}\n"
                           "only for polynomial rings over fields\n");
        if (no_zerodiv)
              fprintf(part,"@item @strong{Remark:}\n"
                           "only for polynomial rings over domains\n");
        fprintf(part,"@item @strong{Purpose:}\n"
                     "@item @strong{Example:}\n"
                     "@smallexample\n"
                     "@c example\n"
                     "@c example\n"
                     "@end smallexample\n"
                     "@c ref\n"
                     "@c See\n"
                     "@c ref{....};\n"
                     "@c ref{....}.\n"
                     "@c ref\n");
        fclose(part);
      }
    }
  }
  fclose(outfile);
}
/*-------------------------------------------------------------------*/
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
        switch (dArith1[i].valid_for & NC_MASK)
        {
          case NO_NC:
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
        switch (dArith2[i].valid_for & NC_MASK)
        {
          case NO_NC:
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
        switch (dArith3[i].valid_for & NC_MASK)
        {
          case NO_NC:
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
        switch (dArithM[i].valid_for & NC_MASK)
        {
          case NO_NC:
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
  rename("plural_cmd.xx","plural_cmd.inc");
}
/*-------------------------------------------------------------------*/

int main(int argc, char**)
{
  if (argc>1)
  {
    produce_convert_table=1; /* for ttGen1 */
    ttGen1();
    unlink(iparith_inc);
    ttGen4();
    ttGen2c();
  }
  else
  {
    ttGen1();
    ttGen2b();
    rename(iparith_inc,"iparith.inc");
  }
  return 0;
}
