/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/* $Id$ */

/*
* ABSTRACT: generate mpsr_Tok.inc
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


#define jjWRONG  1
#define jjWRONG2  1
#define jjWRONG3  1
#define XS(A) A


#define D(A)    2
#define IPARITH
#define GENTABLE

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
  // if (tok>MAX_TOK) return getBlackboxName(tok);
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

#ifdef HAVE_MPSR
#include <Singular/mpsr.h>
#include <Singular/mpsr_Tok.h>
#define MAX_COP 256 // there may be at most 256 cops

// this is the main data struct for storing the relation
// Singular token <-> (Dict, OP)
typedef struct mpsr_cmd
{

  short         tok;    // The Singular token encoding

  // The MP Dict tag in which this token is defined,
  MP_DictTag_t  dict;

  // The MP operator corresponding to this token
  MP_Common_t   cop; // operator
} mpsr_cmd;

#define MAX_SR_DICT     5

// this provides a mapping from MP dict tags to more useful (small)
// array indicies
inline short mpdict2srdict(MP_DictTag_t dict)
{
  if (dict == MP_SingularDict) return 0;
  else if (dict == MP_BasicDict) return 1;
  else if (dict == MP_PolyDict) return 2;
  else if (dict == MP_MatrixDict) return 3;
  else if (dict == MP_NumberDict) return 4;
  else return MAX_SR_DICT;
}

// This is the array which holds all mpsr_cmds
// It is initialized in mpsr_tok.inc
static mpsr_cmd mpsr_cmds[MAX_TOK];

// This is the array which stores the mapping from token to an mpsr_cmd
// A value is either an index into mpsr_cmds, or MAX_TOK
static short tok2mp[MAX_TOK];

// This is the array which stores the mapping from (dict, cop) to a
// mpsr_cmd. First index mpdict2srdict(dict), second is cop
static short mp2tok[MAX_SR_DICT][MAX_COP];

// This the list of all tokens which have an MP representation as a
// cop in the Singular dictionary
short sr_cmds[] =
{
  OPTION_CMD,
  NAMES_CMD,
  ATTRIB_CMD,
  CHARSTR_CMD,
  CLOSE_CMD,
  DEF_CMD,
  DEGREE_CMD,
  DEFINED_CMD,
  E_CMD,
  EXECUTE_CMD,
  FREEMODULE_CMD,
  INT_CMD,
  INTERRED_CMD,
  INTMAT_CMD,
  INTVEC_CMD,
  IS_RINGVAR,
  KILLATTR_CMD,
  MAP_CMD,
  MEMORY_CMD,
  MONITOR_CMD,
  NAMEOF_CMD,
  NUMBER_CMD,
  NPARS_CMD,
  NVARS_CMD,
  OPEN_CMD,
  ORDSTR_CMD,
  PAR_CMD,
  PARSTR_CMD,
  PARDEG_CMD,
  POLY_CMD,
  PRINT_CMD,
  READ_CMD,
  SORTVEC_CMD,
  STRING_CMD,
  SYSTEM_CMD,
  TYPEOF_CMD,
  VECTOR_CMD,
  VAR_CMD,
  VARSTR_CMD,
  WEIGHT_CMD,
  '(',
  COEF_CMD,
  DELETE_CMD,
  FETCH_CMD,
  FIND_CMD,
  IMAP_CMD,
  INSERT_CMD,
  SIMPLIFY_CMD,
  SRES_CMD,
  DBPRINT_CMD,
  TEST_CMD,
  PROC_CMD,
  MSTD_CMD,
  RESERVEDNAME_CMD,
  WRITE_CMD,
  QRING_CMD,
  FGLM_CMD,
  FGLMQUOT_CMD,
  DUMP_CMD,
  GETDUMP_CMD,
  STATUS_CMD,
  LIB_CMD,
  PACKAGE_CMD
};

// struct used for specifying the cmd <-> cop relations
typedef struct cmd_cop
{
  short cmd;
  MP_Common_t cop;
} cmd_op;

typedef struct cmd_dictcop
{
  MP_DictTag_t  dict;
  cmd_op        cmd_ops[255];
} cmd_dictcop;

cmd_dictcop cmd_dictcops[] =
{
  {
    MP_PolyDict,
    // This is the list of all tokens which have an MP representation as a
    // cop in the Poly dictionary
    {
      {BETTI_CMD, MP_CopPolyBetti},
      {CHARACTERISTIC_CMD, MP_CopPolyChar},
      {CHAR_SERIES_CMD, MP_CopPolyCharSeries},
      {CONTENT_CMD, MP_CopPolyClearDenom },
      {DEG_CMD, MP_CopPolyDeg},
      {DIM_CMD, MP_CopPolyDim},
      {FAC_CMD, MP_CopPolyFactorize},
      {FACSTD_CMD, MP_CopPolyFacStd},
      {HILBERT_CMD, MP_CopPolyHilb},
      {HOMOG_CMD, MP_CopPolyHomog},
      {INDEPSET_CMD, MP_CopPolyInDepSet},
      {IDEAL_CMD, MP_CopPolyIdeal},
      {KBASE_CMD, MP_CopPolyKbase},
      {LEAD_CMD, MP_CopPolyLead},
      {LEADCOEF_CMD, MP_CopPolyLeadCoef},
      {LEADEXP_CMD, MP_CopPolyLeadExp},
      {MAXID_CMD, MP_CopPolyMaxIdeal},
      {MINBASE_CMD, MP_CopPolyMinBase},
      {MINRES_CMD, MP_CopPolyMinRes},
      {MODUL_CMD, MP_CopPolyModule},
      {MULTIPLICITY_CMD, MP_CopPolyMultiplicity},
      {ORD_CMD, MP_CopPolyOrder},
      {PRUNE_CMD, MP_CopPolyPrune},
      {QHWEIGHT_CMD, MP_CopPolyQHWeight},
      {REGULARITY_CMD, MP_CopPolyRegularity},
      {RESULTANT_CMD, MP_CopPolyResultant},
      {STD_CMD, MP_CopPolyStd},
      {SYZYGY_CMD, MP_CopPolySyz},
      {VDIM_CMD, MP_CopPolyVdim},
      {COEFFS_CMD,  MP_CopPolyCoeffs},
      {CONTRACT_CMD, MP_CopPolyContract},
      {ELIMINATION_CMD, MP_CopPolyEliminate},
      {JET_CMD, MP_CopPolyJet},
      {LIFT_CMD, MP_CopPolyLift},
      {LIFTSTD_CMD, MP_CopPolyLiftstd},
      {MODULO_CMD, MP_CopPolyModulo},
      {MRES_CMD, MP_CopPolyMres},
      {QUOTIENT_CMD, MP_CopPolyQuotient},
      {REDUCE_CMD, MP_CopPolyReduce},
      {PREIMAGE_CMD, MP_CopPolyPreimage},
      {RES_CMD, MP_CopPolyRes},
      {RING_CMD, MP_CopPolyRing},
      {MAX_TOK, 0}
    }
  },
  {
    MP_NumberDict,
    // This is the list of all tokens which have an MP representation as a
    // cop in the Number dictionary
    {
      {PRIME_CMD, MP_CopNumberPrime},
      {EXTGCD_CMD, MP_CopNumberExtGcd},
      {GCD_CMD, MP_CopNumberGcd},
      {RANDOM_CMD, MP_CopNumberRandom},
      {MAX_TOK, 0}
    }
  },
  {
    MP_MatrixDict,
    // This is the list of all tokens which have an MP representation as a
    // cop in the Matrix dictionary
    {
      {BAREISS_CMD, MP_CopMatrixBareiss},
      {COLS_CMD, MP_CopMatrixCols},
      {DET_CMD, MP_CopMatrixDet},
      {JACOB_CMD, MP_CopMatrixJacobi},
      {MATRIX_CMD, MP_CopMatrixDenseMatrix},
      {ROWS_CMD, MP_CopMatrixRows},
      {TRACE_CMD, MP_CopMatrixTrace},
      {TRANSPOSE_CMD, MP_CopMatrixTranspose},
      {KOSZUL_CMD, MP_CopMatrixKoszul},
      {MINOR_CMD, MP_CopMatrixMinor},
      {WEDGE_CMD, MP_CopMatrixWedge},
      {MAX_TOK, 0}
    }
  },
  {
    MP_BasicDict,
    // This is the list of all tokens which have an MP representation as a
    // cop in the MP Basic dictionary
    {
      {PLUSPLUS, MP_CopBasicInc},
      {MINUSMINUS,  MP_CopBasicDec},
      {COUNT_CMD, MP_CopBasicSize},
      {LIST_CMD, MP_CopBasicList},
      {'+', MP_CopBasicAdd},
      {'-', MP_CopBasicMinus},
      {'*', MP_CopBasicMult},
      {'/', MP_CopBasicDiv},
      {'%', MP_CopBasicMod},
      {'^', MP_CopBasicPow},
      {GE, MP_CopBasicGreaterEqual},
      {'<', MP_CopBasicGreater},
      {LE, MP_CopBasicLessEqual},
      {'>', MP_CopBasicLess},
      {'&', MP_CopBasicAnd},
      {'|', MP_CopBasicOr},
      {'=', MP_CopBasicAssign},
      {EQUAL_EQUAL, MP_CopBasicEqual},
      {NOTEQUAL, MP_CopBasicNotEqual},
      {DOTDOT, MP_CopBasicRange},
      {'[', MP_CopBasicIndex},
      {DIFF_CMD, MP_CopBasicDiff},
      {INTERSECT_CMD, MP_CopBasicInterSect},
      {SUBST_CMD, MP_CopBasicSubst},
      {NOT, MP_CopBasicNot},
      {COLONCOLON, MP_CopBasicPackage},
      {MAX_TOK, 0}
    }
  }
};


// Given a Singular token, find matching (dict,op): Return 1 if one is
// found, 0, otherwise
static short GetMPDictTok(short tok, MP_DictTag_t *dict, MP_Common_t *cop)
{
  short i, l, j;
  cmd_op *cmd_ops;

  // first, look through Singular specific commands
  l = sizeof(sr_cmds)/sizeof(short);
  if (l > MAX_COP)
  {
    fprintf(stderr,
            "Error: There are more than 256 entries in MP_SingularDict\n");
    exit(1);
  }
  for (i=0; i<l; i++)
    if (sr_cmds[i] == tok)
    {
      *dict = MP_SingularDict;
      *cop = i;
      return 1;
    }

  // look through all the other dicts
  for (j=0; j<MAX_SR_DICT-1; j++)
  {
    cmd_ops = cmd_dictcops[j].cmd_ops;
    for (i=0; (cmd_ops[i]).cmd != MAX_TOK; i++)
    {
      if (i > MAX_COP)
      {
        fprintf(stderr,
                "Error: There are more than 256 entries in dict %d's\n",j);
        exit(1);
      }
      if (cmd_ops[i].cmd == tok)
      {
        *dict = cmd_dictcops[j].dict;
        *cop = cmd_ops[i].cop;
        return 1;
      }
    }
  }
  return 0;
}


// This actually generates the tables of mpsr_tok.inc
char *mpsr_Tok_inc;
void mpsr_ttGen()
{
  mpsr_cmd mpsrcmds[MAX_TOK];
  short tok2mp[MAX_TOK];
  short mp2tok[MAX_SR_DICT][MAX_COP];
  short max_cmd = 0, i, j;
  MP_Common_t cop;
  FILE *outfile;
  MP_DictTag_t dict;


  // init all arrays
  for (i=0; i<MAX_TOK; i++)
  {
    mpsrcmds[i].tok = MAX_TOK;
    tok2mp[i] = MAX_TOK;
  }
  for (i=0; i<MAX_SR_DICT; i++)
    for (j=0; j<MAX_COP; j++)
      mp2tok[i][j] = MAX_TOK;

  // Now go through all the token and test them
  for (i=0; i<MAX_TOK; i++)
  {
    if (IsCmdToken(i))
    {
      if (GetMPDictTok(i, &dict, &cop))
      {
        mpsrcmds[max_cmd].tok = i;
        mpsrcmds[max_cmd].dict = dict;
        mpsrcmds[max_cmd].cop = cop;
        tok2mp[i] = max_cmd;
        mp2tok[mpdict2srdict(dict)][cop] = i;
        max_cmd++;
      }
     //else
     //{
     //  fprintf(stderr, "Warning: mpsr_ttGen: Unknown Cmd Token: %d(%s)\n",
     //                  i, iiTwoOps(i));
     // }
    }
  }

  // Generate the template file
  mpsr_Tok_inc=strdup("mpsr_Tok.xxxxxxxx");
  int pid=getpid();
  mpsr_Tok_inc[8]=(pid %10)+'0'; pid/=10;
  mpsr_Tok_inc[9]=(pid %10)+'0'; pid/=10;
  mpsr_Tok_inc[10]=(pid %10)+'0'; pid/=10;
  mpsr_Tok_inc[11]=(pid %10)+'0'; pid/=10;
  mpsr_Tok_inc[12]=(pid %10)+'0'; pid/=10;
  mpsr_Tok_inc[13]=(pid %10)+'0';

  outfile = fopen(mpsr_Tok_inc, "w");
  if (outfile == NULL)
  {
    fprintf(stderr, "Error: mpsr_ttGen: Cannot open file mpsr_Tok.inc\n");
    exit(1);
  }

  // header
  fprintf(outfile,
   "/***************************************************************\n"
   "*\n"
   "* File:       mpsr_tok.inc\n"
   "* Purpose:    tables for mapping Singular cmds to/from MP (dict, op)\n"
   "*\n"
   "* THIS FILE WAS AUTOMATICALLY GENERATED BY mpsr_ttGen(). DO NOT EDIT!\n"
   "*\n"
   "***************************************************************/\n"
   "#ifndef MPSR_STRING_TABLES\n"
   "mpsr_cmd mpsr_cmds[] =\n"
   "{\n"
   "  { %d,\t %d,\t %d }", mpsrcmds[0].tok, mpsrcmds[0].dict, mpsrcmds[0].cop);

  // mpsrcmds
  for (i=1; i<max_cmd; i++)
  {
    fprintf(outfile, ",\n  { %d,\t %d,\t %d }",
            mpsrcmds[i].tok, mpsrcmds[i].dict, mpsrcmds[i].cop);
  }
  fprintf(outfile,"\n};\n\n");

  // tok2mp
  fprintf(outfile, "short tok2mp[] = { %d", tok2mp[0]);
  for (i=1; i<MAX_TOK; i++)
    fprintf(outfile, ", %d", tok2mp[i]);
  fprintf(outfile, "};\n\n");

  // mp2tok
  fprintf(outfile, "short mp2tok[MAX_SR_DICT][MAX_COP] = \n{");
  for (i=0; i<MAX_SR_DICT; i++)
  {
    fprintf(outfile, "\n{\n");
    for (j=0; j<MAX_COP; j++)
    {
      fprintf(outfile, " %d",mp2tok[i][j]);
      if  (j!=MAX_COP-1) fprintf(outfile, ",");
    }
    if (i!=MAX_SR_DICT-1) fprintf(outfile, "},");
    else                  fprintf(outfile, "}");
  }
  fprintf(outfile,"\n};\n\n");

  fprintf(outfile, "#else /* MPSR_STRING_TABLE */\n"
    "mpsr_cmd mpsr_cmds[] =\n"
    "{\n"
    "{ \"%s\",\t %d,\t %d }",
    iiTwoOps(mpsrcmds[0].tok), mpsrcmds[0].dict, mpsrcmds[0].cop);

  for (i=1; i<max_cmd; i++)
  {
    fprintf(outfile, ",\n  { \"%s\",\t %d,\t %d }",
            iiTwoOps(mpsrcmds[i].tok), mpsrcmds[i].dict, mpsrcmds[i].cop);
  }
  fprintf(outfile, ",\n { NULL, \t 0, \t 0}");
  fprintf(outfile,"\n};\n\n#endif /* ! MPSR_STRING_TABLE */");

  fclose(outfile);
} // That's all

#else
void mpsr_ttGen()
{
  system("touch mpsr_Tok.xx");
}
#endif

int main()
{
  mpsr_ttGen();
  #ifdef HAVE_MPSR
  rename(mpsr_Tok_inc,"mpsr_Tok.inc");
  #endif
  return 0;
}
