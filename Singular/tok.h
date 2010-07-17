#ifndef TOK_H
#define TOK_H
/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/*
* ABSTRACT: tokens, types for interpreter; general macros
*/
/* $Id$ */

#ifndef MYYSTYPE
#include <kernel/structs.h>
#endif
#ifndef UMINUS
#include <Singular/grammar.h>
#endif

extern int      yylineno;
extern char     my_yylinebuf[80];

#if defined(__cplusplus)
extern int  yyparse(void);
#endif

/* Define to use old mechanismen for saving currRing with procedures
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
  KERNEL_CMD,
  KILLATTR_CMD,
  KRES_CMD,
  LAGSOLVE_CMD,
  LINK_CMD,
  LIST_CMD,
  LOAD_CMD,
  LRES_CMD,
  LU_CMD,
  LUI_CMD,
  LUS_CMD,
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
  PFAC_CMD,
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
  SQR_FREE_CMD,
  STATUS_CMD,
  STRING_CMD,
  SYSTEM_CMD,
  TEST_CMD,
  TRANSPOSE_CMD,
  TRACE_CMD,
  TWOSTD_CMD,
  TYPEOF_CMD,
  UNIVARIATE_CMD,
  UNLOAD_CMD,
  URSOLVE_CMD,
  VANDER_CMD,
  VARIABLES_CMD,
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

#endif
