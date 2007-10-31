#ifndef TOK_H
#define TOK_H
/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/*
* ABSTRACT: tokens, types for interpreter; general macros
*/
/* $Id: tok.h,v 1.67 2007-10-31 17:24:10 Singular Exp $ */

#ifndef MYYSTYPE
#include "structs.h"
#endif
#ifndef UMINUS
#include "grammar.h"
#endif

extern int      yylineno;
extern char     my_yylinebuf[80];

#if defined(__cplusplus)
extern int  yyparse(void);
#endif

/* Define to use old mechanismen for saving currRing with procedures
 * Does work with HAVE_NAMESPACES enabled
 */
#define USE_IILOCALRING 1


/* the follwing defines for infix operators should not be changed: *
*  grammar.y does not use the symbolic names                       *
*  scanner.l uses the identies for some optimzations              */
#define LOGIC_OP         '&'
#define MULDIV_OP        '/'
#define COMP_OP          '<'

#define COMMAND           UMINUS+2 /* in tok.h */
#define ANY_TYPE          UMINUS+3
#define IDHDL             UMINUS+4
#define NSHDL             UMINUS+5

enum {
  ALIAS_CMD     = UMINUS + 15,
  ATTRIB_CMD,
  BAREISS_CMD,
  BIGINT_CMD,
  BRACKET_CMD,
  BREAKPOINT_CMD,
  CHARACTERISTIC_CMD,
  CHARSTR_CMD,
  CHAR_SERIES_CMD,
  CHINREM_CMD,
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
  END_GRAMMAR,
  ENVELOPE_CMD,
  ERROR_CMD,
  EXECUTE_CMD,
  EXPORTTO_CMD,
  EXTGCD_CMD,
  FAC_CMD,
  FIND_CMD,
  FACSTD_CMD,
  FWALK_CMD,
  FGLM_CMD,
  FGLMQUOT_CMD,
  FINDUNI_CMD,
  GCD_CMD,
  GETDUMP_CMD,
  HIGHCORNER_CMD,
  HRES_CMD,
  IMPART_CMD,
  IMPORTFROM_CMD,
  INSERT_CMD,
  INT_CMD,
  INTDIV_CMD,
  INTERPOLATE_CMD,
  INTMOD_CMD,
  INTVEC_CMD,
  IS_RINGVAR,
  JANET_CMD,
  KILLATTR_CMD,
  KRES_CMD,
  LAGSOLVE_CMD,
  LINK_CMD,
  LIST_CMD,
  LOAD_CMD,
  LRES_CMD,
  MEMORY_CMD,
  MONITOR_CMD,
  MPRES_CMD,
  MSTD_CMD,
  NAMEOF_CMD,
  NAMES_CMD,
  NCALGEBRA_CMD,
  NC_ALGEBRA_CMD,
  NEWTONPOLY_CMD,
  NPARS_CMD,
  NVARS_CMD,
  OPEN_CMD,
  OPPOSE_CMD,
  OPPOSITE_CMD,
  OPTION_CMD,
  ORDSTR_CMD,
  PACKAGE_CMD,
  PARSTR_CMD,
  POINTER_CMD,
  PRIME_CMD,
  PRINT_CMD,
  PRUNE_CMD,
  QRING_CMD,
  RANDOM_CMD,
  READ_CMD,
  REPART_CMD,
  RESERVEDNAME_CMD,
  RESULTANT_CMD,
  RINGLIST_CMD,
  ROWS_CMD,
  SIMPLEX_CMD,
  SLIM_GB_CMD,
  SQR_FREE_DEC_CMD,
  STATUS_CMD,
  STRING_CMD,
  SYSTEM_CMD,
  TEST_CMD,
  TRANSPOSE_CMD,
  TRACE_CMD,
  TWOSTD_CMD,
  TYPEOF_CMD,
  UNLOAD_CMD,
  URSOLVE_CMD,
  VANDER_CMD,
  VARSTR_CMD,
  WRITE_CMD,
  /* start system var section: VECHO */
  VECHO,
  VPAGELENGTH,
  VCOLMAX,
  VTIMER,
  VRTIMER,
  TRACE,
  VOICE,
  VSHORTOUT,
  VPRINTLEVEL,
  /* end system var section: VPRINTLEVEL */

  MAX_TOK /* must be the last, biggest token number */
};

#define NONE END_RING
#define UNKNOWN 0

/*
**  Set operations (small sets only)
*/

#define Sy_bit(x)     ((unsigned)1<<(x))
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
#define V_CONTENTSB 15
#define V_CANCELUNIT 16
#define V_DEG_STOP  31


#define OPT_PROT           0
#define OPT_REDSB          1
#define OPT_NOT_BUCKETS    2
#define OPT_NOT_SUGAR      3
#define OPT_INTERRUPT      4
#define OPT_SUGARCRIT      5
#define OPT_DEBUG          6
#define OPT_REDTHROUGH     7
#define OPT_RETURN_SB      9
#define OPT_FASTHC        10
#define OPT_OLDSTD        20
#define OPT_KEEPVARS      21
#define OPT_STAIRCASEBOUND 22
#define OPT_MULTBOUND     23
#define OPT_DEGBOUND      24
#define OPT_REDTAIL       25
#define OPT_INTSTRATEGY   26
#define OPT_INFREDTAIL    28
#define OPT_SB_1          29
#define OPT_NOTREGULARITY 30
#define OPT_WEIGHTM       31

/* define ring dependent options */ 
#define TEST_RINGDEP_OPTS \
 (Sy_bit(OPT_INTSTRATEGY) | Sy_bit(OPT_REDTHROUGH) | Sy_bit(OPT_REDTAIL))

#define TEST_OPT_PROT              BTEST1(OPT_PROT)
#define TEST_OPT_REDSB             BTEST1(OPT_REDSB)
#define TEST_OPT_NOT_BUCKETS       BTEST1(OPT_NOT_BUCKETS)
#define TEST_OPT_NOT_SUGAR         BTEST1(OPT_NOT_SUGAR)
#define TEST_OPT_SUGARCRIT         BTEST1(OPT_SUGARCRIT)
#define TEST_OPT_DEBUG             BTEST1(OPT_DEBUG)
#define TEST_OPT_FASTHC            BTEST1(OPT_FASTHC)
#define TEST_OPT_INTSTRATEGY       BTEST1(OPT_INTSTRATEGY)
#define TEST_OPT_RETURN_SB         BTEST1(OPT_RETURN_SB)
#define TEST_OPT_KEEPVARS          BTEST1(OPT_KEEPVARS)
#define TEST_OPT_DEGBOUND          BTEST1(OPT_DEGBOUND)
#define TEST_OPT_MULTBOUND         BTEST1(OPT_MULTBOUND)
#define TEST_OPT_STAIRCASEBOUND    BTEST1(OPT_STAIRCASEBOUND)
#define TEST_OPT_REDTAIL           BTEST1(OPT_REDTAIL)
#define TEST_OPT_INFREDTAIL        BTEST1(OPT_INFREDTAIL)
#define TEST_OPT_SB_1              BTEST1(OPT_SB_1)
#define TEST_OPT_NOTREGULARITY     BTEST1(OPT_NOTREGULARITY)
#define TEST_OPT_WEIGHTM           BTEST1(OPT_WEIGHTM)
#define TEST_OPT_REDTHROUGH        BTEST1(OPT_REDTHROUGH)
#define TEST_OPT_OLDSTD            BTEST1(OPT_OLDSTD)
#define TEST_OPT_CONTENTSB         BVERBOSE(V_CONTENTSB)
#define TEST_OPT_CANCELUNIT        BVERBOSE(V_CANCELUNIT)

#define TEST_VERB_NSB              BVERBOSE(V_NSB)
#define TEST_V_DEG_STOP            BVERBOSE(V_DEG_STOP)

#endif
