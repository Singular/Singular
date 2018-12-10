#ifndef TOK_H
#define TOK_H
/*!
!
***************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/*!
!

* ABSTRACT: tokens, types for interpreter; general macros


*/

#ifndef UMINUS
#include "kernel/mod2.h"
#include "Singular/grammar.h"
#endif

extern int      yylineno;
extern char     my_yylinebuf[80];

#if defined(__cplusplus)
extern int  yyparse(void);
#endif

/*!
!
 the follwing defines for infix operators should not be changed: *
*  grammar.y does not use the symbolic names                       *
*  scanner.l uses the identies for some optimzations              

*/
#define LOGIC_OP         '&'
#define MULDIV_OP        '/'
#define COMP_OP          '<'

#define COMMAND           UMINUS+2 /*!
!
 in tok.h 

*/
#define ANY_TYPE          UMINUS+3
#define IDHDL             UMINUS+4

enum {
  ALIAS_CMD     = UMINUS + 15,
  ALIGN_CMD,
  ATTRIB_CMD,
  BAREISS_CMD,
  BIGINT_CMD,
  BRANCHTO_CMD,
  BRACKET_CMD,
  BREAKPOINT_CMD,
  CHARACTERISTIC_CMD,
  CHARSTR_CMD,
  CHAR_SERIES_CMD,
  CHINREM_CMD,
  CMATRIX_CMD,
  CNUMBER_CMD,
  CPOLY_CMD,
  CLOSE_CMD,
  COEFFS_CMD,
  COEF_CMD,
  COLS_CMD,
  CONTENT_CMD,
  CONTRACT_CMD,
  COUNT_CMD,
  CRING_CMD,
  DBPRINT_CMD,
  DEF_CMD,
  DEFINED_CMD,
  DEG_CMD,
  DEGREE_CMD,
  DELETE_CMD,
  DENOMINATOR_CMD,
  DET_CMD,
  DIFF_CMD,
  DIM_CMD,
  DIVISION_CMD,
  DUMP_CMD,
  ELIMINATION_CMD,
  END_GRAMMAR,
  ENVELOPE_CMD,
  ERROR_CMD,
  EXECUTE_CMD,
  EXPORTTO_CMD,
  EXTGCD_CMD,
  FAC_CMD,
  FAREY_CMD,
  FIND_CMD,
  FACSTD_CMD,
  FMD_CMD,
  FRES_CMD,
  FWALK_CMD,
  FGLM_CMD,
  FGLMQUOT_CMD,
  FINDUNI_CMD,
  GCD_CMD,
  GETDUMP_CMD,
  HIGHCORNER_CMD,
  HILBERT_CMD,
  HOMOG_CMD,
  HRES_CMD,
  IMPART_CMD,
  IMPORTFROM_CMD,
  INDEPSET_CMD,
  INSERT_CMD,
  INT_CMD,
  INTDIV_CMD,
  INTERPOLATE_CMD,
  INTERRED_CMD,
  INTERSECT_CMD,
  INTVEC_CMD,
  IS_RINGVAR,
  JACOB_CMD,
  JANET_CMD,
  JET_CMD,
  KBASE_CMD,
  KERNEL_CMD,
  KILLATTR_CMD,
  KRES_CMD,
  LAGSOLVE_CMD,
  LEAD_CMD,
  LEADCOEF_CMD,
  LEADEXP_CMD,
  LEADMONOM_CMD,
  LIFTSTD_CMD,
  LIFT_CMD,
  LINK_CMD,
  LIST_CMD,
  LOAD_CMD,
  LRES_CMD,
  LU_CMD,
  LUI_CMD,
  LUS_CMD,
  MEMORY_CMD,
  MINBASE_CMD,
  MINOR_CMD,
  MINRES_CMD,
  MODULO_CMD,
  MONITOR_CMD,
  MPRES_CMD,
  MRES_CMD,
  MSTD_CMD,
  MULTIPLICITY_CMD,
  NAMEOF_CMD,
  NAMES_CMD,
  NEWSTRUCT_CMD,
  NCALGEBRA_CMD,
  NC_ALGEBRA_CMD,
  NEWTONPOLY_CMD,
  NPARS_CMD,
  NUMERATOR_CMD,
  NVARS_CMD,
  ORD_CMD,
  OPEN_CMD,
  OPPOSE_CMD,
  OPPOSITE_CMD,
  OPTION_CMD,
  ORDSTR_CMD,
  PACKAGE_CMD,
  PARDEG_CMD,
  PARENT_CMD,
  PARSTR_CMD,
  PFAC_CMD,
  PRIME_CMD,
  PRINT_CMD,
  PRUNE_CMD,
  QHWEIGHT_CMD,
  QRING_CMD,
  QRDS_CMD,
  QUOTIENT_CMD,
  RANDOM_CMD,
  RANK_CMD,
  READ_CMD,
  REDUCE_CMD,
  REGULARITY_CMD,
  REPART_CMD,
  RES_CMD,
  RESERVEDNAME_CMD,
  RESTART_CMD,
  RESULTANT_CMD,
  RINGLIST_CMD,
  RING_LIST_CMD,
  ROWS_CMD,
  SBA_CMD,
  SIMPLEX_CMD,
  SIMPLIFY_CMD,
  SLIM_GB_CMD,
  SORTVEC_CMD,
  SQR_FREE_CMD,
  SRES_CMD,
  STATUS_CMD,
  STD_CMD,
  STRING_CMD,
  SUBST_CMD,
  SYSTEM_CMD,
  SYZYGY_CMD,
  TENSOR_CMD,
  TEST_CMD,
  TRANSPOSE_CMD,
  TRACE_CMD,
  TWOSTD_CMD,
  TYPEOF_CMD,
  UNIVARIATE_CMD,
  UNLOAD_CMD, /*!
!
 unused

*/
  URSOLVE_CMD,
  VANDER_CMD,
  VARIABLES_CMD,
  VARSTR_CMD,
  VDIM_CMD,
  WAIT1ST_CMD,
  WAITALL_CMD,
  WEDGE_CMD,
  WEIGHT_CMD,
  WRITE_CMD,
  /*!
!
 start system var section: VECHO 

*/
  VECHO,
  VCOLMAX,
  VTIMER,
  VRTIMER,
  TRACE,
  VOICE,
  VSHORTOUT,
  VPRINTLEVEL,
  /*!
!
 end system var section: VPRINTLEVEL 

*/

  MAX_TOK /*!
!
 must be the last, biggest token number 

*/
};

#define NONE END_RING
#define UNKNOWN 0

#endif
