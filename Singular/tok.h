#ifndef TOK_H
#define TOK_H
/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/*
* ABSTRACT: token for interpreter, as types; general macros
*/
/* $Id: tok.h,v 1.8 1997-06-17 10:12:13 obachman Exp $ */

#ifndef MYYSTYPE
#include "structs.h"
#endif
#ifndef UMINUS
#include "grammar.h"
#endif

#ifndef YY_READ_BUF_SIZE
#define YY_READ_BUF_SIZE 8192
/*#define YY_READ_BUF_SIZE 16384*/
#endif

extern int      yylineno;
extern char     my_yylinebuf[YY_READ_BUF_SIZE];

#if defined(__cplusplus)
extern int  yyparse(void);
#endif

#define loop for(;;)

#ifndef ABS
#define ABS(x) ((x)<0?(-(x)):(x))
#endif

#if defined(__cplusplus)  
inline int max(const int a, const int b)  { return (a>b) ? a : b; }
inline int min(const int a, const int b)  { return (a<b) ? a : b; }
#else
#define max(A,B) ((A) > (B) ? (A) : (B))
#define min(A,B) ((A) < (B) ? (A) : (B))
#endif

#define COMMAND           UMINUS+2 /* in tok.h */
#define ANY_TYPE          UMINUS+3
#define IDHDL             UMINUS+4

enum {
  ATTRIB_CMD     = UMINUS + 15,
  BINARY_CMD,
  CHARACTERISTIC_CMD,
  CHARSTR_CMD,
  CHAR_SERIES_CMD,
  CLOSE_CMD,
  COLS_CMD,
  CONTENT_CMD,
  COUNT_CMD,
  DBPRINT_CMD,
  DEF_CMD,
  DEFINED_CMD,
  DELETE_CMD,
  DET_CMD,
  DUMP_CMD,
  EXTGCD_CMD,
  FAC_CMD,
  FIND_CMD,
  FACSTD_CMD,
  FGLM_CMD,
  GCD_CMD,
  GETDUMP_CMD,
  INSERT_CMD,
  INT_CMD,
  INTVEC_CMD,
  IS_RINGVAR,
  KILLATTR_CMD,
  LINK_CMD,
  LIST_CMD,
  MEMORY_CMD,
  MONITOR_CMD,
  MSTD_CMD,
  NAMEOF_CMD,
  NAMES_CMD,
  NPARS_CMD,
  NVARS_CMD,
  OPEN_CMD,
  OPTION_CMD,
  ORDSTR_CMD,
  PACKAGE_CMD,
  PARSTR_CMD,
  PRIME_CMD,
  PRINT_CMD,
  PRUNE_CMD,
  QRING_CMD,
  RANDOM_CMD,
  READ_CMD,
  RESERVEDNAME_CMD,
  RESULTANT_CMD,
  ROWS_CMD,
  STATUS_CMD,
  SQR_FREE_DEC_CMD,
  STRING_CMD,
  SYSTEM_CMD,
  TEST_CMD,
  TRANSPOSE_CMD,
  TRACE_CMD,
  TYPEOF_CMD,
  VARSTR_CMD,
  VERBOSE_CMD,
  WRITE_CMD,
  /* start system var section: VECHO */
  VECHO,
  VPAGELENGTH,
  VCOLMAX,
  VTIMER,
#ifdef HAVE_RTIMER
  VRTIMER,
#endif  
  TRACE,
  VOICE,
  VSHORTOUT,
  VPRINTLEVEL,
  /* end system var section: VPRINTLEVEL */

  /* start system var section: VECHO */
  /* end system var section: VPRINTLEVEL */
  MAX_TOK /* must be the last, biggest token number */
};

#define NONE END_RING
#define UNKNOWN 0
#define SIC_MASK 1024

/*
**  Set operations (small sets only)
*/

#define Sy_bit(x)     (1<<(x))
#define Sy_inset(x,s) ((Sy_bit(x)&(s))?TRUE:FALSE)
#define BTEST1(a)     Sy_inset((a), test)
#define BVERBOSE(a)   Sy_inset((a), verbose)

/*
** defines for BITSETs
*/

#define V_SHOW_MEM  2
#define V_YACC      3
#define V_REDEFINE  4
#define V_READING   5
#define V_LOAD_LIB  6
#define V_DEBUG_LIB 7
#define V_LOAD_PROC 8
#define V_DEF_RES   9
#define V_DEBUG_MEM 10
#define V_SHOW_USE  11
#define V_IMAP      12
#define V_PROMPT    13
#define V_NSB       14

#define TEST_VERB_NSB              BVERBOSE(V_NSB)

#define OPT_PROT           0
#define OPT_REDSB          1
#define OPT_NOT_SUGAR      3
#define OPT_INTERRUPT      4
#define OPT_SUGARCRIT      5
#define OPT_DEBUG          6
#define OPT_CANCELUNIT     7
#define OPT_MOREPAIRS      8
#define OPT_RETURN_SB      9
#define OPT_FASTHC        10
#define OPT_KEEPVARS      21
#define OPT_STAIRCASEBOUND 22
#define OPT_MULTBOUND     23
#define OPT_DEGBOUND      24
#define OPT_REDTAIL       25
#define OPT_INTSTRATEGY   26
#define OPT_MINRES        28
#define OPT_SB_1          29
#define OPT_NOTREGULARITY 30
#define OPT_WEIGHTM       31

#define TEST_OPT_PROT              BTEST1(OPT_PROT)
#define TEST_OPT_REDSB             BTEST1(OPT_REDSB)
#define TEST_OPT_NOT_SUGAR         BTEST1(OPT_NOT_SUGAR)
#define TEST_OPT_SUGARCRIT         BTEST1(OPT_SUGARCRIT)
#define TEST_OPT_DEBUG             BTEST1(OPT_DEBUG)
#define TEST_OPT_MOREPAIRS         BTEST1(OPT_MOREPAIRS)
#define TEST_OPT_FASTHC            BTEST1(OPT_FASTHC)
#define TEST_OPT_INTSTRATEGY       BTEST1(OPT_INTSTRATEGY)
#define TEST_OPT_CANCELUNIT        BTEST1(OPT_CANCELUNIT)
#define TEST_OPT_RETURN_SB         BTEST1(OPT_RETURN_SB)
#define TEST_OPT_KEEPVARS          BTEST1(OPT_KEEPVARS)
#define TEST_OPT_DEGBOUND          BTEST1(OPT_DEGBOUND)
#define TEST_OPT_MULTBOUND         BTEST1(OPT_MULTBOUND)
#define TEST_OPT_STAIRCASEBOUND    BTEST1(OPT_STAIRCASEBOUND)
#define TEST_OPT_REDTAIL           BTEST1(OPT_REDTAIL)
#define TEST_OPT_MINRES            BTEST1(OPT_MINRES)
#define TEST_OPT_SB_1              BTEST1(OPT_SB_1)
#define TEST_OPT_NOTREGULARITY     BTEST1(OPT_NOTREGULARITY)
#define TEST_OPT_WEIGHTM           BTEST1(OPT_WEIGHTM)

#endif
