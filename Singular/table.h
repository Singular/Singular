#ifdef IPARITH
// additional to the usual types: INT_CMD etc.
// thre are special types:
// for the input:
// IDHDL: argument must have a name
// DEF_CMD: no restriktions on the argument
// ANY_TYPE: changes to pseudo data (for "defined", "typeof", etc.)
// with the following subfields
//        - name !=NULL
//        - data := original type id

// for the output:
// NONE: does not return a value
// ANY_TYPE: various types, the routines have to set it

// the procedures have to be wrapped into the macro D(...)
// with the exception of jjWRONG... (which always fails)
// -----------------------------------------------------------------------
// context: non-commutative rings and coefficient domain:
//
// commutativity:
// ALLOW_NC: also for plural rings and letterplace rings
// ALLOW_LP: also for letterplace rings, not for plural rings
// ALLOW_PLURAL: not for letterplace rings, but for plural rings
// COMM_PLURAL: only for commuative subrings of plural rings
// NO_NC: not for non-commutative rings
//
// coefficient domain:
// ALLOW_RING:  coefficient domain may be a ring
// NO_ZERODIVISOR: coefficient domain may be a ring without zero-divisors
// NO_RING: coefficient domain must be a field
//
// other stuff:
// NO_CONVERSION: arguments must have EXACTLY these types
// WARN_RING: warn, that the operation happens in the quotient field
/*=================== operations with 1 arg.: table =================*/
const struct sValCmd1 dArith1[]=
{
// operations:
// proc            cmd               res             arg            context
 {D(jjPLUSPLUS),   PLUSPLUS,        NONE,           IDHDL         , ALLOW_NC |ALLOW_RING}
,{D(jjPLUSPLUS),   MINUSMINUS,      NONE,           IDHDL         , ALLOW_NC |ALLOW_RING}
,{D(jjUMINUS_I),   '-',             INT_CMD,        INT_CMD       , ALLOW_NC |ALLOW_RING}
,{D(jjUMINUS_BI),   '-',            BIGINT_CMD,     BIGINT_CMD    , ALLOW_NC |ALLOW_RING}
,{D(jjUMINUS_N),   '-',             NUMBER_CMD,     NUMBER_CMD    , ALLOW_NC |ALLOW_RING}
#ifdef SINGULAR_4_2
,{D(jjNUMBER2_OP1),'-',             CNUMBER_CMD,    CNUMBER_CMD   , ALLOW_NC |ALLOW_RING}
,{D(jjPOLY2_OP1),  '-',             CPOLY_CMD,      CPOLY_CMD     , ALLOW_NC |ALLOW_RING}
#endif
,{D(jjUMINUS_P),   '-',             POLY_CMD,       POLY_CMD      , ALLOW_NC |ALLOW_RING}
,{D(jjUMINUS_P),   '-',             VECTOR_CMD,     VECTOR_CMD    , ALLOW_NC |ALLOW_RING}
,{D(jjUMINUS_MA),  '-',             MATRIX_CMD,     MATRIX_CMD    , ALLOW_NC |ALLOW_RING}
,{D(jjUMINUS_IV),  '-',             INTVEC_CMD,     INTVEC_CMD    , ALLOW_NC |ALLOW_RING}
,{D(jjUMINUS_IV),  '-',             INTMAT_CMD,     INTMAT_CMD    , ALLOW_NC |ALLOW_RING}
,{D(jjUMINUS_BIM), '-',             BIGINTMAT_CMD,  BIGINTMAT_CMD , ALLOW_NC |ALLOW_RING}
#ifdef SINGULAR_4_2
,{D(jjUMINUS_BIM), '-',             CMATRIX_CMD,    CMATRIX_CMD ,   ALLOW_NC |ALLOW_RING}
#endif
,{D(jjPROC1),      '(',             ANY_TYPE/*set by p*/,PROC_CMD , ALLOW_NC |ALLOW_RING}
// and the procedures with 1 argument:
,{D(atATTRIB1),    ATTRIB_CMD,      NONE,           DEF_CMD       , ALLOW_NC |ALLOW_RING}
,{D(jjBAREISS_BIM), BAREISS_CMD,    BIGINTMAT_CMD,  BIGINTMAT_CMD  , ALLOW_PLURAL |ALLOW_RING}
,{D(jjBAREISS),    BAREISS_CMD,     LIST_CMD,       MODUL_CMD     , NO_NC |ALLOW_RING | NO_ZERODIVISOR}
,{D(jjBETTI),      BETTI_CMD,       INTMAT_CMD,     LIST_CMD      , ALLOW_PLURAL |ALLOW_RING}
,{D(syBetti1),     BETTI_CMD,       INTMAT_CMD,     RESOLUTION_CMD, ALLOW_PLURAL |ALLOW_RING}
,{D(jjBETTI),      BETTI_CMD,       INTMAT_CMD,     IDEAL_CMD     , ALLOW_PLURAL |ALLOW_RING}
,{D(jjBETTI),      BETTI_CMD,       INTMAT_CMD,     MODUL_CMD     , ALLOW_PLURAL |ALLOW_RING}
,{D(jjDUMMY),      BIGINT_CMD,      BIGINT_CMD,     BIGINT_CMD    , ALLOW_NC |ALLOW_RING}
,{D(jjN2BI),       BIGINT_CMD,      BIGINT_CMD,     NUMBER_CMD    , ALLOW_NC |ALLOW_RING}
,{D(jjP2BI),       BIGINT_CMD,      BIGINT_CMD,     POLY_CMD      , ALLOW_NC |ALLOW_RING}
,{D(jjDUMMY),      BIGINTMAT_CMD,   BIGINTMAT_CMD,  BIGINTMAT_CMD , ALLOW_NC |ALLOW_RING}
,{D(jjCHAR),       CHARACTERISTIC_CMD, INT_CMD,     RING_CMD      , ALLOW_NC |ALLOW_RING}
,{D(jjCHARSERIES), CHAR_SERIES_CMD, MATRIX_CMD,     IDEAL_CMD     , NO_NC |NO_RING}
,{D(jjrCharStr),   CHARSTR_CMD,     STRING_CMD,     RING_CMD      , ALLOW_NC |ALLOW_RING}
,{D(jjOpenClose),  CLOSE_CMD,       NONE,           LINK_CMD      , ALLOW_NC |ALLOW_RING}
#ifdef SINGULAR_4_2
,{D(jjDUMMY),      CMATRIX_CMD,     CMATRIX_CMD,    CMATRIX_CMD ,   ALLOW_NC |ALLOW_RING}
#endif
//,{  jjWRONG ,       COLS_CMD,        0,              VECTOR_CMD  , ALLOW_NC |ALLOW_RING}
,{D(jjCOLS),       COLS_CMD,        INT_CMD,        MATRIX_CMD    , ALLOW_NC |ALLOW_RING|NO_CONVERSION}
,{D(jjCOLS),       COLS_CMD,        INT_CMD,        SMATRIX_CMD   , ALLOW_NC |ALLOW_RING|NO_CONVERSION}
,{D(jjCOLS),       COLS_CMD,        INT_CMD,        IDEAL_CMD     , ALLOW_NC |ALLOW_RING|NO_CONVERSION}
,{D(jjCOLS),       COLS_CMD,        INT_CMD,        MODUL_CMD     , ALLOW_NC |ALLOW_RING|NO_CONVERSION}
,{D(jjCOLS_IV),    COLS_CMD,        INT_CMD,        INTMAT_CMD    , ALLOW_NC |ALLOW_RING|NO_CONVERSION}
,{D(jjCOLS_BIM),   COLS_CMD,        INT_CMD,        BIGINTMAT_CMD , ALLOW_NC |ALLOW_RING|NO_CONVERSION}
#ifdef SINGULAR_4_2
,{D(jjCOLS_BIM),   COLS_CMD,        INT_CMD,        CMATRIX_CMD , ALLOW_NC |ALLOW_RING|NO_CONVERSION}
#endif
//,{  jjWRONG ,      COLS_CMD,        0,              INTVEC_CMD    , ALLOW_NC |ALLOW_RING}
,{D(jjCONTENT),    CONTENT_CMD,     POLY_CMD,       POLY_CMD      , ALLOW_NC |ALLOW_RING}
,{D(jjCONTENT),    CONTENT_CMD,     VECTOR_CMD,     VECTOR_CMD    , ALLOW_NC |ALLOW_RING}
,{D(jjCOUNT_BI),   COUNT_CMD,       INT_CMD,        BIGINT_CMD    , ALLOW_NC |ALLOW_RING}
,{D(jjCOUNT_N),    COUNT_CMD,       INT_CMD,        NUMBER_CMD    , ALLOW_NC |ALLOW_RING}
,{D(jjCOUNT_RES),  COUNT_CMD,       INT_CMD,        RESOLUTION_CMD, ALLOW_NC |ALLOW_RING}
,{D(jjstrlen),     COUNT_CMD,       INT_CMD,        STRING_CMD    , ALLOW_NC |ALLOW_RING}
,{D(jjpLength),    COUNT_CMD,       INT_CMD,        POLY_CMD      , ALLOW_NC |ALLOW_RING}
,{D(jjpLength),    COUNT_CMD,       INT_CMD,        VECTOR_CMD    , ALLOW_NC |ALLOW_RING}
,{D(jjidElem),     COUNT_CMD,       INT_CMD,        IDEAL_CMD     , ALLOW_NC |ALLOW_RING}
,{D(jjidElem),     COUNT_CMD,       INT_CMD,        MODUL_CMD     , ALLOW_NC |ALLOW_RING}
,{D(jjCOUNT_M),    COUNT_CMD,       INT_CMD,        MATRIX_CMD    , ALLOW_NC |ALLOW_RING}
,{D(jjCOUNT_IV),   COUNT_CMD,       INT_CMD,        INTVEC_CMD    , ALLOW_NC |ALLOW_RING}
,{D(jjCOUNT_IV),   COUNT_CMD,       INT_CMD,        INTMAT_CMD    , ALLOW_NC |ALLOW_RING}
,{D(jjCOUNT_BIM),  COUNT_CMD,       INT_CMD,        BIGINTMAT_CMD , ALLOW_NC |ALLOW_RING}
,{D(jjCOUNT_L),    COUNT_CMD,       INT_CMD,        LIST_CMD      , ALLOW_NC |ALLOW_RING}
,{D(jjCOUNT_RG),   COUNT_CMD,       INT_CMD,        RING_CMD      , ALLOW_NC |ALLOW_RING}
,{D(jjR2_CR),      CRING_CMD,       CRING_CMD,      RING_CMD      , ALLOW_NC |ALLOW_RING}
,{D(jjNULL),       DEF_CMD,         DEF_CMD,        INT_CMD       , ALLOW_NC |ALLOW_RING}
,{  jjWRONG ,      DEF_CMD,         0,              ANY_TYPE      , ALLOW_NC |ALLOW_RING}
,{D(jjDEG),        DEG_CMD,         INT_CMD,        POLY_CMD      , ALLOW_NC |ALLOW_RING}
,{D(jjDEG),        DEG_CMD,         INT_CMD,        VECTOR_CMD    , ALLOW_NC |ALLOW_RING}
,{D(jjDEG_M),      DEG_CMD,         INT_CMD,        MATRIX_CMD    , ALLOW_NC |ALLOW_RING}
,{D(jjDEGREE),     DEGREE_CMD,      STRING_CMD,     IDEAL_CMD     , NO_NC |ALLOW_RING | NO_ZERODIVISOR}
,{D(jjDEGREE),     DEGREE_CMD,      STRING_CMD,     MODUL_CMD     , NO_NC |ALLOW_RING | NO_ZERODIVISOR}
,{D(jjDEFINED),    DEFINED_CMD,     INT_CMD,        DEF_CMD       , ALLOW_NC |ALLOW_RING}
,{D(jjDENOMINATOR),DENOMINATOR_CMD, NUMBER_CMD,     NUMBER_CMD    , ALLOW_NC |ALLOW_RING}
,{D(jjNUMERATOR),  NUMERATOR_CMD,   NUMBER_CMD,     NUMBER_CMD    , ALLOW_NC |ALLOW_RING}
#ifdef SINGULAR_4_2
,{D(jjDET_N2),     DET_CMD,         CNUMBER_CMD,    CMATRIX_CMD   , ALLOW_PLURAL |ALLOW_RING}
#endif
,{D(jjDET_BI),     DET_CMD,         BIGINT_CMD,     BIGINTMAT_CMD , ALLOW_NC |ALLOW_RING}
,{D(jjDET_I),      DET_CMD,         INT_CMD,        INTMAT_CMD    , ALLOW_NC |ALLOW_RING}
,{D(jjDET_S),      DET_CMD,         POLY_CMD,       SMATRIX_CMD   , NO_NC |NO_RING}
,{D(jjDET),        DET_CMD,         POLY_CMD,       MATRIX_CMD    , NO_NC |ALLOW_RING}
,{D(jjDIM),        DIM_CMD,         INT_CMD,        IDEAL_CMD     , ALLOW_PLURAL |ALLOW_RING}
,{D(jjDIM),        DIM_CMD,         INT_CMD,        MODUL_CMD     , ALLOW_PLURAL |ALLOW_RING}
,{D(jjDIM_R),      DIM_CMD,         INT_CMD,        RESOLUTION_CMD, ALLOW_PLURAL |ALLOW_RING}
,{D(jjDUMP),       DUMP_CMD,        NONE,           LINK_CMD      , ALLOW_NC |ALLOW_RING}
,{D(jjE),          E_CMD,           VECTOR_CMD,     INT_CMD       , ALLOW_NC |ALLOW_RING}
,{D(jjEXECUTE),    EXECUTE_CMD,     NONE,           STRING_CMD    , ALLOW_NC |ALLOW_RING}
,{D(jjERROR),      ERROR_CMD,       NONE,           STRING_CMD    , ALLOW_NC |ALLOW_RING}
,{D(jjFAC_P),      FAC_CMD,         LIST_CMD,       POLY_CMD      , NO_NC |ALLOW_ZZ}
,{D(findUniProc),  FINDUNI_CMD,     IDEAL_CMD,      IDEAL_CMD     , NO_NC |NO_RING}
,{D(jjidFreeModule),FREEMODULE_CMD, MODUL_CMD,      INT_CMD       , ALLOW_NC |ALLOW_RING}
,{D(jjFACSTD),     FACSTD_CMD,      LIST_CMD,       IDEAL_CMD     , NO_NC |NO_RING}
,{D(jjGETDUMP),    GETDUMP_CMD,     NONE,           LINK_CMD      , ALLOW_NC |ALLOW_RING}
,{D(jjHIGHCORNER), HIGHCORNER_CMD,  POLY_CMD,       IDEAL_CMD     , NO_NC |ALLOW_RING}
,{D(jjHIGHCORNER_M), HIGHCORNER_CMD,VECTOR_CMD,     MODUL_CMD     , NO_NC |ALLOW_RING}
,{D(jjHILBERT),    HILBERT_CMD,     NONE,           IDEAL_CMD     , ALLOW_PLURAL |ALLOW_RING | NO_ZERODIVISOR}
,{D(jjHILBERT),    HILBERT_CMD,     NONE,           MODUL_CMD     , ALLOW_PLURAL |ALLOW_RING | NO_ZERODIVISOR}
,{D(jjHILBERT_IV), HILBERT_CMD,     INTVEC_CMD,     INTVEC_CMD    , ALLOW_PLURAL |ALLOW_RING | NO_ZERODIVISOR}
,{D(jjHOMOG1),     HOMOG_CMD,       INT_CMD,        IDEAL_CMD     , ALLOW_NC |ALLOW_RING}
,{D(jjHOMOG1),     HOMOG_CMD,       INT_CMD,        MODUL_CMD     , ALLOW_NC |ALLOW_RING}
,{D(jjDUMMY),      IDEAL_CMD,       IDEAL_CMD,      IDEAL_CMD     , ALLOW_NC |ALLOW_RING}
,{D(jjidVec2Ideal),IDEAL_CMD,       IDEAL_CMD,      VECTOR_CMD    , ALLOW_NC |ALLOW_RING}
,{D(jjIDEAL_Ma),   IDEAL_CMD,       IDEAL_CMD,      MATRIX_CMD    , ALLOW_NC |ALLOW_RING}
,{D(jjIDEAL_R),    IDEAL_CMD,       IDEAL_CMD,      RING_CMD      , ALLOW_NC |ALLOW_RING}
,{D(jjIDEAL_Map),  IDEAL_CMD,       IDEAL_CMD,      MAP_CMD       , ALLOW_NC |ALLOW_RING}
,{D(jjIMPART),     IMPART_CMD,      NUMBER_CMD,     NUMBER_CMD    , ALLOW_NC |ALLOW_RING}
,{D(jjINDEPSET),   INDEPSET_CMD,    INTVEC_CMD,     IDEAL_CMD     , NO_NC |NO_RING}
,{D(jjDUMMY),      INT_CMD,         INT_CMD,        INT_CMD       , ALLOW_NC |ALLOW_RING}
,{D(jjnlInt),      INT_CMD,         INT_CMD,        BIGINT_CMD    , ALLOW_NC |ALLOW_RING}
,{D(jjnInt),       INT_CMD,         INT_CMD,        NUMBER_CMD    , ALLOW_NC |ALLOW_RING}
,{D(jjP2I),        INT_CMD,         INT_CMD,        POLY_CMD      , ALLOW_NC |ALLOW_RING}
,{D(jjS2I),        INT_CMD,         INT_CMD,        STRING_CMD    , ALLOW_NC |ALLOW_RING}
,{D(jjINTERRED),   INTERRED_CMD,    IDEAL_CMD,      IDEAL_CMD     , ALLOW_PLURAL |NO_RING}
,{D(jjINTERRED),   INTERRED_CMD,    MODUL_CMD,      MODUL_CMD     , ALLOW_PLURAL |NO_RING}
,{D(jjBI2IM),      INTMAT_CMD,      INTMAT_CMD,     BIGINTMAT_CMD  , ALLOW_PLURAL |ALLOW_RING}
,{D(jjDUMMY),      INTMAT_CMD,      INTMAT_CMD,     INTMAT_CMD    , ALLOW_NC |ALLOW_RING}
,{D(jjIm2Iv),      INTVEC_CMD,      INTVEC_CMD,     INTMAT_CMD    , ALLOW_NC |ALLOW_RING}
,{D(jjDUMMY),      INTVEC_CMD,      INTVEC_CMD,     INTVEC_CMD    , ALLOW_NC |ALLOW_RING}
,{D(jjIS_RINGVAR_P), IS_RINGVAR,    INT_CMD,        POLY_CMD      , ALLOW_NC |ALLOW_RING}
,{D(jjIS_RINGVAR_S), IS_RINGVAR,    INT_CMD,        STRING_CMD    , ALLOW_NC |ALLOW_RING}
,{D(jjIS_RINGVAR0),IS_RINGVAR,      INT_CMD,        ANY_TYPE      , ALLOW_NC |ALLOW_RING}
,{D(jjJACOB_P),    JACOB_CMD,       IDEAL_CMD,      POLY_CMD      , ALLOW_PLURAL |ALLOW_RING}
,{D(mpJacobi),     JACOB_CMD,       MATRIX_CMD,     IDEAL_CMD     , ALLOW_PLURAL |ALLOW_RING}
,{D(jjJACOB_M),    JACOB_CMD,       MODUL_CMD,      MODUL_CMD     , ALLOW_PLURAL |ALLOW_RING}
,{D(jjJanetBasis), JANET_CMD,       IDEAL_CMD,      IDEAL_CMD     , ALLOW_PLURAL |NO_RING}
,{D(jjKBASE),      KBASE_CMD,       IDEAL_CMD,      IDEAL_CMD     , ALLOW_PLURAL |ALLOW_RING|WARN_RING} /*ring-cf: warning at top level*/
,{D(jjKBASE),      KBASE_CMD,       MODUL_CMD,      MODUL_CMD     , ALLOW_PLURAL |ALLOW_RING|WARN_RING} /*ring-cf: warning at top level*/
,{D(jjLU_DECOMP),  LU_CMD,          LIST_CMD,       MATRIX_CMD    , NO_NC |NO_RING}
,{D(jjPFAC1),      PFAC_CMD,        LIST_CMD,       BIGINT_CMD    , ALLOW_NC |ALLOW_RING}
,{D(jjPFAC1),      PFAC_CMD,        LIST_CMD,       NUMBER_CMD    , ALLOW_NC |ALLOW_RING}
,{D(atKILLATTR1),  KILLATTR_CMD,    NONE,           IDHDL         , ALLOW_NC |ALLOW_RING}
,{D(jjpHead),      LEAD_CMD,        POLY_CMD,       POLY_CMD      , ALLOW_NC |ALLOW_RING}
,{D(jjidHead),     LEAD_CMD,        IDEAL_CMD,      IDEAL_CMD     , ALLOW_NC |ALLOW_RING}
,{D(jjpHead),      LEAD_CMD,        VECTOR_CMD,     VECTOR_CMD    , ALLOW_NC |ALLOW_RING}
,{D(jjidHead),     LEAD_CMD,        MODUL_CMD,      MODUL_CMD     , ALLOW_NC |ALLOW_RING}
,{D(jjLEADCOEF),   LEADCOEF_CMD,    NUMBER_CMD,     POLY_CMD      , ALLOW_NC |ALLOW_RING}
,{D(jjLEADCOEF),   LEADCOEF_CMD,    NUMBER_CMD,     VECTOR_CMD    , ALLOW_NC |ALLOW_RING}
,{D(jjLEADEXP),    LEADEXP_CMD,     INTVEC_CMD,     POLY_CMD      , ALLOW_NC |ALLOW_RING}
,{D(jjLEADEXP),    LEADEXP_CMD,     INTVEC_CMD,     VECTOR_CMD    , ALLOW_NC |ALLOW_RING}
,{D(jjLEADMONOM),  LEADMONOM_CMD,   POLY_CMD,       POLY_CMD      , ALLOW_NC |ALLOW_RING}
,{D(jjLEADMONOM),  LEADMONOM_CMD,   VECTOR_CMD,     VECTOR_CMD    , ALLOW_NC |ALLOW_RING}
,{D(jjDUMMY),      LINK_CMD,        LINK_CMD,       LINK_CMD      , ALLOW_NC |ALLOW_RING}
,{D(jjCALL1MANY),  LIST_CMD,        LIST_CMD,       DEF_CMD       , ALLOW_NC |ALLOW_RING}
,{  jjWRONG ,      MAP_CMD,         0,              ANY_TYPE      , ALLOW_NC |ALLOW_RING}
,{D(jjDUMMY),      MATRIX_CMD,      MATRIX_CMD,     MATRIX_CMD    , ALLOW_NC |ALLOW_RING}
,{D(jjidMaxIdeal), MAXID_CMD,       IDEAL_CMD,      INT_CMD       , ALLOW_NC |ALLOW_RING}
,{D(jjMEMORY),     MEMORY_CMD,      BIGINT_CMD,     INT_CMD       , ALLOW_NC |ALLOW_RING}
,{D(jjidMinBase),  MINBASE_CMD,     IDEAL_CMD,      IDEAL_CMD     , NO_NC |NO_RING}
,{D(jjidMinBase),  MINBASE_CMD,     MODUL_CMD,      MODUL_CMD     , NO_NC |NO_RING}
,{D(jjMINRES),     MINRES_CMD,      LIST_CMD,       LIST_CMD      , ALLOW_PLURAL |ALLOW_RING}
,{D(jjMINRES_R),   MINRES_CMD,      RESOLUTION_CMD, RESOLUTION_CMD, ALLOW_PLURAL |ALLOW_RING}
,{D(jjDUMMY),      MODUL_CMD,       MODUL_CMD,      MODUL_CMD     , ALLOW_NC |ALLOW_RING}
,{D(jjMONITOR1),   MONITOR_CMD,     NONE,           LINK_CMD      , ALLOW_NC |ALLOW_RING}
,{D(jjMONOM),      MONOM_CMD,       POLY_CMD,       INTVEC_CMD    , ALLOW_NC |ALLOW_RING}
,{D(jjMULT),       MULTIPLICITY_CMD,  INT_CMD,      IDEAL_CMD     , NO_NC |ALLOW_RING}
,{D(jjMULT),       MULTIPLICITY_CMD,  INT_CMD,      MODUL_CMD     , NO_NC |ALLOW_RING}
,{D(jjMSTD),       MSTD_CMD,        LIST_CMD,       IDEAL_CMD     , NO_NC |ALLOW_RING}
,{D(jjMSTD),       MSTD_CMD,        LIST_CMD,       MODUL_CMD     , NO_NC |ALLOW_RING}
,{D(jjNAMEOF),     NAMEOF_CMD,      STRING_CMD,     ANY_TYPE      , ALLOW_NC |ALLOW_RING}
,{D(jjNAMES_I),    NAMES_CMD,       LIST_CMD,       INT_CMD       , ALLOW_NC |ALLOW_RING}
,{D(jjNAMES),      NAMES_CMD,       LIST_CMD,       PACKAGE_CMD   , ALLOW_NC |ALLOW_RING}
,{D(jjNAMES),      NAMES_CMD,       LIST_CMD,       RING_CMD      , ALLOW_NC |ALLOW_RING}
,{D(jjNOT),        NOT,             INT_CMD,        INT_CMD       , ALLOW_NC |ALLOW_RING}
#ifdef SINGULAR_4_2
,{D(jjN2_N),       NUMBER_CMD,      NUMBER_CMD,     CNUMBER_CMD   , ALLOW_NC |ALLOW_RING}
#endif
,{D(jjDUMMY),      NUMBER_CMD,      NUMBER_CMD,     NUMBER_CMD    , ALLOW_NC |ALLOW_RING}
,{D(jjP2N),        NUMBER_CMD,      NUMBER_CMD,     POLY_CMD      , ALLOW_NC |ALLOW_RING}
,{D(jjBI2N),       NUMBER_CMD,      NUMBER_CMD,     BIGINT_CMD    , ALLOW_NC |ALLOW_RING}
,{D(jjDUMMY),      CNUMBER_CMD,     CNUMBER_CMD,    CNUMBER_CMD   , ALLOW_NC |ALLOW_RING}
,{D(jjRPAR),       NPARS_CMD,       INT_CMD,        RING_CMD      , ALLOW_NC |ALLOW_RING}
,{D(jjNVARS),      NVARS_CMD,       INT_CMD,        RING_CMD      , ALLOW_NC |ALLOW_RING}
,{D(jjOpenClose),  OPEN_CMD,        NONE,           LINK_CMD      , ALLOW_NC |ALLOW_RING}
,{D(jjCALL1MANY),  OPTION_CMD,      NONE,           DEF_CMD       , ALLOW_NC |ALLOW_RING} /*libsing*/
,{D(jjORD),        ORD_CMD,         INT_CMD,        POLY_CMD      , ALLOW_NC |ALLOW_RING}
,{D(jjORD),        ORD_CMD,         INT_CMD,        VECTOR_CMD    , ALLOW_NC |ALLOW_RING}
,{D(jjrOrdStr),    ORDSTR_CMD,      STRING_CMD,     RING_CMD      , ALLOW_NC |ALLOW_RING}
,{D(jjPAR1),       PAR_CMD,         NUMBER_CMD,     INT_CMD       , ALLOW_NC |ALLOW_RING}
,{D(jjPARDEG),     PARDEG_CMD,      INT_CMD,        NUMBER_CMD    , ALLOW_NC |ALLOW_RING}
#ifdef SINGULAR_4_2
,{D(jjN2_CR),      PARENT_CMD,      CRING_CMD,      CNUMBER_CMD    , ALLOW_NC |ALLOW_RING}
,{D(jjP2_R),       PARENT_CMD,      RING_CMD,       CPOLY_CMD      , ALLOW_NC |ALLOW_RING}
,{D(jjCM_CR),      PARENT_CMD,      CRING_CMD,      CMATRIX_CMD    , ALLOW_NC |ALLOW_RING}
,{D(jjBIM2_CR),    PARENT_CMD,      CRING_CMD,      BIGINTMAT_CMD , ALLOW_NC |ALLOW_RING}
#endif
,{D(jjPARSTR1),    PARSTR_CMD,      STRING_CMD,     INT_CMD       , ALLOW_NC |ALLOW_RING}
,{D(jjrParStr),    PARSTR_CMD,      STRING_CMD,     RING_CMD      , ALLOW_NC |ALLOW_RING}
,{D(jjDUMMY),      POLY_CMD,        POLY_CMD,       POLY_CMD      , ALLOW_NC |ALLOW_RING}
,{D(jjBI2P),       POLY_CMD,        POLY_CMD,       BIGINT_CMD    , ALLOW_NC |ALLOW_RING}
,{D(jjPREIMAGE_R), PREIMAGE_CMD,    RING_CMD,       MAP_CMD       , NO_NC |ALLOW_RING}
,{D(jjPRIME),      PRIME_CMD,       INT_CMD,        INT_CMD       , ALLOW_NC |ALLOW_RING}
,{D(jjPRINT),      PRINT_CMD,       STRING_CMD,     LIST_CMD      , ALLOW_NC |ALLOW_RING}
,{D(jjPRINT),      PRINT_CMD,       STRING_CMD,     DEF_CMD       , ALLOW_NC |ALLOW_RING}
,{D(jjDUMMY),      PROC_CMD,        PROC_CMD,       PROC_CMD       , ALLOW_NC |ALLOW_RING}
,{D(jjPRUNE),      PRUNE_CMD,       MODUL_CMD,      MODUL_CMD     , ALLOW_PLURAL |ALLOW_RING}
,{D(kQHWeight),    QHWEIGHT_CMD,    INTVEC_CMD,     IDEAL_CMD     , ALLOW_PLURAL |ALLOW_RING}
,{D(kQHWeight),    QHWEIGHT_CMD,    INTVEC_CMD,     MODUL_CMD     , ALLOW_PLURAL |ALLOW_RING}
,{D(jjRANK1),      RANK_CMD,        INT_CMD,        MATRIX_CMD    , ALLOW_NC |NO_RING}
,{D(jjREAD),       READ_CMD,        STRING_CMD,     LINK_CMD      , ALLOW_NC |ALLOW_RING}
,{D(jjREGULARITY), REGULARITY_CMD,  INT_CMD,        LIST_CMD      , NO_NC |ALLOW_RING}
,{D(jjREPART),     REPART_CMD,      NUMBER_CMD,     NUMBER_CMD    , ALLOW_NC |ALLOW_RING}
,{D(jjRESERVEDNAME),RESERVEDNAME_CMD, INT_CMD,      STRING_CMD    , ALLOW_NC |ALLOW_RING}
,{D(jjL2R),        RESOLUTION_CMD,  RESOLUTION_CMD, LIST_CMD      , ALLOW_PLURAL |ALLOW_RING}
,{D(jjDUMMY),      RESOLUTION_CMD,  RESOLUTION_CMD, RESOLUTION_CMD, ALLOW_PLURAL |ALLOW_RING}
,{D(jjRESTART),    RESTART_CMD,     NONE,           INT_CMD,        ALLOW_NC |ALLOW_RING}
#if defined(HAVE_SHIFTBBA) || defined(HAVE_PLURAL)
,{D(jjRIGHTSTD),   RIGHTSTD_CMD,    IDEAL_CMD,      IDEAL_CMD     , ALLOW_NC |NO_RING}
#endif
,{D(jjRINGLIST),   RINGLIST_CMD,    LIST_CMD,       RING_CMD      , ALLOW_NC |ALLOW_RING}
,{D(jjRINGLIST_C), RING_LIST_CMD,   LIST_CMD,       CRING_CMD     , ALLOW_NC |ALLOW_RING}
,{D(jjRING_LIST),  RING_LIST_CMD,   LIST_CMD,       RING_CMD      , ALLOW_NC |ALLOW_RING}
,{D(jjDUMMY),      RING_CMD,        RING_CMD,       RING_CMD      , ALLOW_NC |ALLOW_RING}
,{D(jjLISTRING),   RING_CMD,        RING_CMD,       LIST_CMD      , ALLOW_NC |ALLOW_RING}
//,{  jjWRONG ,      ROWS_CMD,        0,              POLY_CMD    , ALLOW_NC |ALLOW_RING}
,{D(jjpMaxComp),   ROWS_CMD,        INT_CMD,        VECTOR_CMD    , ALLOW_NC |ALLOW_RING|NO_CONVERSION}
,{D(jjROWS),       ROWS_CMD,        INT_CMD,        MODUL_CMD     , ALLOW_NC |ALLOW_RING|NO_CONVERSION}
,{D(jjROWS),       ROWS_CMD,        INT_CMD,        MATRIX_CMD    , ALLOW_NC |ALLOW_RING|NO_CONVERSION}
,{D(jjROWS),       ROWS_CMD,        INT_CMD,        SMATRIX_CMD   , ALLOW_NC |ALLOW_RING|NO_CONVERSION}
,{D(jjROWS_IV),    ROWS_CMD,        INT_CMD,        INTMAT_CMD    , ALLOW_NC |ALLOW_RING|NO_CONVERSION}
,{D(jjROWS_BIM),   ROWS_CMD,        INT_CMD,        BIGINTMAT_CMD , ALLOW_NC |ALLOW_RING|NO_CONVERSION}
#ifdef SINGULAR_4_2
,{D(jjROWS_BIM),   ROWS_CMD,        INT_CMD,        CMATRIX_CMD , ALLOW_NC |ALLOW_RING|NO_CONVERSION}
#endif
,{D(jjCOUNT_IV),   ROWS_CMD,        INT_CMD,        INTVEC_CMD    , ALLOW_NC |ALLOW_RING|NO_CONVERSION}
,{D(jjSBA),        SBA_CMD,         IDEAL_CMD,      IDEAL_CMD     , ALLOW_PLURAL |ALLOW_RING}
,{D(jjSBA),        SBA_CMD,         MODUL_CMD,      MODUL_CMD     , ALLOW_PLURAL |ALLOW_RING}
,{D(jjSetRing),    SETRING_CMD,     NONE,           RING_CMD      , ALLOW_NC |ALLOW_RING}
,{D(jjSLIM_GB),    SLIM_GB_CMD,     IDEAL_CMD,      IDEAL_CMD     , ALLOW_PLURAL |NO_RING}
,{D(jjSLIM_GB),    SLIM_GB_CMD,     MODUL_CMD,      MODUL_CMD     , ALLOW_PLURAL |NO_RING}
,{D(jjDUMMY),      SMATRIX_CMD,     SMATRIX_CMD,    SMATRIX_CMD   , ALLOW_NC |ALLOW_RING}
,{D(jjSort_Id),    SORTVEC_CMD,     INTVEC_CMD,     IDEAL_CMD     , ALLOW_NC |ALLOW_RING}
,{D(jjSort_Id),    SORTVEC_CMD,     INTVEC_CMD,     MODUL_CMD     , ALLOW_NC |ALLOW_RING}
,{D(jjSQR_FREE),   SQR_FREE_CMD,    LIST_CMD,      POLY_CMD      , NO_NC |NO_RING}
,{D(jjSTD),        STD_CMD,         IDEAL_CMD,      IDEAL_CMD     , ALLOW_NC |ALLOW_RING}
,{D(jjSTD),        STD_CMD,         MODUL_CMD,      MODUL_CMD     , ALLOW_NC |ALLOW_RING}
,{D(jjSTD),        STD_CMD,         SMATRIX_CMD,    SMATRIX_CMD   , ALLOW_NC |ALLOW_RING}
,{D(jjDUMMY),      STRING_CMD,      STRING_CMD,     STRING_CMD    , ALLOW_NC |ALLOW_RING}
,{D(jjSYZYGY),     SYZYGY_CMD,      MODUL_CMD,      IDEAL_CMD     , ALLOW_NC |ALLOW_RING}
,{D(jjSYZYGY),     SYZYGY_CMD,      MODUL_CMD,      MODUL_CMD     , ALLOW_NC |ALLOW_RING}
#ifdef HAVE_PLURAL
,{D(jjENVELOPE),   ENVELOPE_CMD,    RING_CMD,       RING_CMD      , ALLOW_PLURAL |NO_RING}
,{D(jjOPPOSITE),   OPPOSITE_CMD,    RING_CMD,       RING_CMD      , ALLOW_PLURAL |NO_RING}
#endif
#if defined(HAVE_PLURAL) || defined(HAVE_SHIFTBBA)
,{D(jjTWOSTD),     TWOSTD_CMD,      IDEAL_CMD,      IDEAL_CMD     , ALLOW_NC |NO_RING}
#endif
//,{  jjWRONG ,      TRACE_CMD,       0,              INTVEC_CMD    , ALLOW_NC |ALLOW_RING}
//,{  jjWRONG ,      TRACE_CMD,       0,              IDEAL_CMD     , ALLOW_NC |ALLOW_RING}
,{D(jjTRACE_IV),   TRACE_CMD,       INT_CMD,        INTMAT_CMD    , ALLOW_PLURAL |ALLOW_RING|NO_CONVERSION}
,{D(jjmpTrace),    TRACE_CMD,       POLY_CMD,       MATRIX_CMD    , ALLOW_PLURAL |ALLOW_RING|NO_CONVERSION}
,{D(jjTRANSP_IV),  TRANSPOSE_CMD,   INTMAT_CMD,     INTVEC_CMD    , ALLOW_NC |ALLOW_RING}
,{D(jjTRANSP_IV),  TRANSPOSE_CMD,   INTMAT_CMD,     INTMAT_CMD    , ALLOW_NC |ALLOW_RING}
,{D(jjTRANSP_BIM), TRANSPOSE_CMD,   BIGINTMAT_CMD,  BIGINTMAT_CMD , ALLOW_NC |ALLOW_RING}
#ifdef SINGULAR_4_2
,{D(jjTRANSP_BIM), TRANSPOSE_CMD,   CMATRIX_CMD,    CMATRIX_CMD ,   ALLOW_NC |ALLOW_RING}
#endif
,{D(jjmpTransp),   TRANSPOSE_CMD,   MATRIX_CMD,     MATRIX_CMD    , ALLOW_NC |ALLOW_RING}
,{D(jjidTransp),   TRANSPOSE_CMD,   MODUL_CMD,      MODUL_CMD     , ALLOW_NC |ALLOW_RING}
,{D(jjidTransp),   TRANSPOSE_CMD,   SMATRIX_CMD,    SMATRIX_CMD   , ALLOW_NC |ALLOW_RING}
,{D(jjTYPEOF),     TYPEOF_CMD,      STRING_CMD,     ANY_TYPE      , ALLOW_NC |ALLOW_RING}
,{D(jjUNIVARIATE), UNIVARIATE_CMD,  INT_CMD,        POLY_CMD      , ALLOW_PLURAL |ALLOW_RING}
,{D(jjVARIABLES_P),VARIABLES_CMD,   IDEAL_CMD,      POLY_CMD      , ALLOW_PLURAL |ALLOW_RING}
,{D(jjVARIABLES_ID),VARIABLES_CMD,  IDEAL_CMD,      IDEAL_CMD     , ALLOW_PLURAL |ALLOW_RING}
,{D(jjVARIABLES_ID),VARIABLES_CMD,  IDEAL_CMD,      MATRIX_CMD    , ALLOW_PLURAL |ALLOW_RING}
,{D(jjDUMMY),      VECTOR_CMD,      VECTOR_CMD,     VECTOR_CMD    , ALLOW_NC |ALLOW_RING}
,{D(jjVDIM),       VDIM_CMD,        INT_CMD,        IDEAL_CMD     , ALLOW_PLURAL |ALLOW_RING |WARN_RING} /*ring-cf: warning at top level*/
,{D(jjVDIM),       VDIM_CMD,        INT_CMD,        MODUL_CMD     , ALLOW_PLURAL |ALLOW_RING |WARN_RING} /*ring-cf: warning at top level*/
,{D(jjVAR1),       VAR_CMD,         POLY_CMD,       INT_CMD       , ALLOW_NC |ALLOW_RING}
,{D(jjVARSTR1),    VARSTR_CMD,      STRING_CMD,     INT_CMD       , ALLOW_NC |ALLOW_RING}
,{D(jjrVarStr),    VARSTR_CMD,      STRING_CMD,     RING_CMD      , ALLOW_NC |ALLOW_RING}
,{D(kWeight),      WEIGHT_CMD,      INTVEC_CMD,     IDEAL_CMD     , ALLOW_PLURAL |ALLOW_RING}
,{D(kWeight),      WEIGHT_CMD,      INTVEC_CMD,     MODUL_CMD     , ALLOW_PLURAL |ALLOW_RING}
,{D(jjLOAD1),      LOAD_CMD,        NONE,           STRING_CMD    , ALLOW_NC |ALLOW_RING}
,{D(loNewtonP),    NEWTONPOLY_CMD,  IDEAL_CMD,      IDEAL_CMD     , ALLOW_PLURAL |ALLOW_RING}
,{D(jjWAIT1ST1),   WAIT1ST_CMD,     INT_CMD,        LIST_CMD      , ALLOW_NC |ALLOW_RING}
,{D(jjWAITALL1),   WAITALL_CMD,     INT_CMD,        LIST_CMD      , ALLOW_NC |ALLOW_RING}
,{NULL_VAL,        0,               0,              0             , NO_NC |NO_RING}
};
/*=================== operations with 2 arg.: table =================*/
const struct sValCmd2 dArith2[]=
{
// operations:
// proc           cmd              res             arg1        arg2   context
 {D(jjCOLCOL),    COLONCOLON,     ANY_TYPE,       DEF_CMD,    DEF_CMD, ALLOW_NC | ALLOW_RING}
,{D(jjPLUS_I),    '+',            INT_CMD,        INT_CMD,    INT_CMD, ALLOW_NC | ALLOW_RING}
,{D(jjPLUS_BI),   '+',            BIGINT_CMD,     BIGINT_CMD, BIGINT_CMD, ALLOW_NC | ALLOW_RING}
,{D(jjPLUS_N),    '+',            NUMBER_CMD,     NUMBER_CMD, NUMBER_CMD, ALLOW_NC | ALLOW_RING}
,{D(jjPLUS_B),    '+',            BUCKET_CMD,     POLY_CMD,   POLY_CMD, ALLOW_NC | ALLOW_RING}
,{D(jjPLUS_V),    '+',            VECTOR_CMD,     VECTOR_CMD, VECTOR_CMD, ALLOW_NC | ALLOW_RING}
,{D(jjPLUS_ID),   '+',            IDEAL_CMD,      IDEAL_CMD,  IDEAL_CMD, ALLOW_NC | ALLOW_RING}
,{D(jjPLUS_ID),   '+',            MODUL_CMD,      MODUL_CMD,  MODUL_CMD, ALLOW_NC | ALLOW_RING}
,{D(jjPLUS_P_MA), '+',            MATRIX_CMD,     POLY_CMD,   MATRIX_CMD, ALLOW_NC | ALLOW_RING}
,{D(jjPLUS_MA_P), '+',            MATRIX_CMD,     MATRIX_CMD, POLY_CMD, ALLOW_NC | ALLOW_RING}
,{D(jjPLUS_MA),   '+',            MATRIX_CMD,     MATRIX_CMD, MATRIX_CMD, ALLOW_NC | ALLOW_RING}
,{D(jjPLUS_SM),   '+',            SMATRIX_CMD,    SMATRIX_CMD,SMATRIX_CMD, ALLOW_NC | ALLOW_RING}
,{D(jjPLUS_S),    '+',            STRING_CMD,     STRING_CMD, STRING_CMD, ALLOW_NC | ALLOW_RING}
,{D(jjPLUS_BIM),  '+',            BIGINTMAT_CMD,  BIGINTMAT_CMD, BIGINTMAT_CMD, ALLOW_NC | ALLOW_RING}
,{D(jjOP_BIM_I),  '+',            BIGINTMAT_CMD,  BIGINTMAT_CMD, INT_CMD, ALLOW_NC | ALLOW_RING}
,{D(jjOP_I_BIM),  '+',            BIGINTMAT_CMD,  INT_CMD, BIGINTMAT_CMD, ALLOW_NC | ALLOW_RING}
,{D(jjOP_BIM_BI),  '+',           BIGINTMAT_CMD,  BIGINTMAT_CMD, BIGINT_CMD, ALLOW_NC | ALLOW_RING}
,{D(jjOP_BI_BIM),  '+',           BIGINTMAT_CMD,  BIGINT_CMD, BIGINTMAT_CMD, ALLOW_NC | ALLOW_RING}
,{D(jjOP_IV_I),   '+',            INTVEC_CMD,     INTVEC_CMD, INT_CMD, ALLOW_NC | ALLOW_RING}
,{D(jjOP_I_IV),   '+',            INTVEC_CMD,     INT_CMD,    INTVEC_CMD, ALLOW_NC | ALLOW_RING}
,{D(jjOP_IM_I),   '+',            INTMAT_CMD,     INTMAT_CMD, INT_CMD, ALLOW_NC | ALLOW_RING}
,{D(jjOP_I_IM),   '+',            INTMAT_CMD,     INT_CMD,    INTMAT_CMD, ALLOW_NC | ALLOW_RING}
,{D(jjPLUS_IV),   '+',            INTVEC_CMD,     INTVEC_CMD, INTVEC_CMD, ALLOW_NC | ALLOW_RING}
,{D(jjPLUS_IV),   '+',            INTMAT_CMD,     INTMAT_CMD, INTMAT_CMD, ALLOW_NC | ALLOW_RING}
#ifdef SINGULAR_4_2
,{D(jjNUMBER2_OP2),'+',           CNUMBER_CMD,    CNUMBER_CMD,CNUMBER_CMD, ALLOW_NC | ALLOW_RING}
,{D(jjNUMBER2_OP2),'+',           CNUMBER_CMD,    DEF_CMD,    CNUMBER_CMD, ALLOW_NC | ALLOW_RING}
,{D(jjNUMBER2_OP2),'+',           CNUMBER_CMD,    CNUMBER_CMD,DEF_CMD, ALLOW_NC | ALLOW_RING}
,{D(jjPOLY2_OP2), '+',            CPOLY_CMD,      CPOLY_CMD,  DEF_CMD, ALLOW_NC | ALLOW_RING}
,{D(jjPOLY2_OP2), '+',            CPOLY_CMD,      DEF_CMD,    CPOLY_CMD,  ALLOW_NC | ALLOW_RING}
,{D(jjPLUS_BIM),  '+',            CMATRIX_CMD,    CMATRIX_CMD, CMATRIX_CMD, ALLOW_NC | ALLOW_RING}
#endif
,{D(lAdd),        '+',            LIST_CMD,       LIST_CMD,   LIST_CMD, ALLOW_NC | ALLOW_RING}
,{D(jjRSUM),      '+',            RING_CMD,       RING_CMD,   RING_CMD, ALLOW_NC | ALLOW_RING}
,{D(jjRPLUS),     '+',            RING_CMD,       RING_CMD,   STRING_CMD, ALLOW_LP | ALLOW_RING}
,{D(jjRPLUS),     '+',            RING_CMD,       STRING_CMD, RING_CMD, ALLOW_LP | ALLOW_RING}
,{D(jjPLUS_B_P),  '+',            BUCKET_CMD,     BUCKET_CMD, POLY_CMD, ALLOW_NC | ALLOW_RING}
,{D(jjMINUS_I),   '-',            INT_CMD,        INT_CMD,    INT_CMD, ALLOW_NC | ALLOW_RING}
,{D(jjMINUS_BI),  '-',            BIGINT_CMD,     BIGINT_CMD, BIGINT_CMD, ALLOW_NC | ALLOW_RING}
,{D(jjMINUS_N),   '-',            NUMBER_CMD,     NUMBER_CMD, NUMBER_CMD, ALLOW_NC | ALLOW_RING}
,{D(jjMINUS_B),   '-',            BUCKET_CMD,     POLY_CMD,   POLY_CMD, ALLOW_NC | ALLOW_RING}
,{D(jjMINUS_V),   '-',            VECTOR_CMD,     VECTOR_CMD, VECTOR_CMD, ALLOW_NC | ALLOW_RING}
,{D(jjMINUS_V),   '-',            VECTOR_CMD,     VECTOR_CMD, VECTOR_CMD, ALLOW_NC | ALLOW_RING}
,{D(jjPLUS_MA_P), '-',            MATRIX_CMD,     MATRIX_CMD, POLY_CMD, ALLOW_NC | ALLOW_RING}
,{D(jjMINUS_MA),  '-',            MATRIX_CMD,     MATRIX_CMD, MATRIX_CMD, ALLOW_NC | ALLOW_RING}
,{D(jjMINUS_SM),  '-',            SMATRIX_CMD,    SMATRIX_CMD,SMATRIX_CMD, ALLOW_NC | ALLOW_RING}
,{D(jjMINUS_BIM), '-',            BIGINTMAT_CMD,  BIGINTMAT_CMD, BIGINTMAT_CMD, ALLOW_NC | ALLOW_RING}
,{D(jjOP_BIM_I),  '-',            BIGINTMAT_CMD,  BIGINTMAT_CMD, INT_CMD, ALLOW_NC | ALLOW_RING}
,{D(jjOP_I_BIM),  '-',            BIGINTMAT_CMD,  INT_CMD, BIGINTMAT_CMD, ALLOW_NC | ALLOW_RING}
,{D(jjOP_BIM_BI), '-',            BIGINTMAT_CMD,  BIGINTMAT_CMD, BIGINT_CMD, ALLOW_NC | ALLOW_RING}
,{D(jjOP_BI_BIM), '-',            BIGINTMAT_CMD,  BIGINT_CMD, BIGINTMAT_CMD, ALLOW_NC | ALLOW_RING}
,{D(jjOP_IV_I),   '-',            INTVEC_CMD,     INTVEC_CMD, INT_CMD, ALLOW_NC | ALLOW_RING}
,{D(jjOP_IM_I),   '-',            INTMAT_CMD,     INTMAT_CMD, INT_CMD, ALLOW_NC | ALLOW_RING}
,{D(jjMINUS_IV),  '-',            INTVEC_CMD,     INTVEC_CMD, INTVEC_CMD, ALLOW_NC | ALLOW_RING}
,{D(jjMINUS_IV),  '-',            INTMAT_CMD,     INTMAT_CMD, INTMAT_CMD, ALLOW_NC | ALLOW_RING}
#ifdef SINGULAR_4_2
,{D(jjNUMBER2_OP2),'-',           CNUMBER_CMD,    CNUMBER_CMD,CNUMBER_CMD, ALLOW_NALLOW_NC | ALLOW_RING}
,{D(jjNUMBER2_OP2),'-',           CNUMBER_CMD,    INT_CMD,    CNUMBER_CMD, ALLOW_NC | ALLOW_RING}
,{D(jjNUMBER2_OP2),'-',           CNUMBER_CMD,    CNUMBER_CMD,INT_CMD, ALLOW_NC | ALLOW_RING}
,{D(jjNUMBER2_OP2),'-',           CNUMBER_CMD,    BIGINT_CMD, CNUMBER_CMD, ALLOW_NC | ALLOW_RING}
,{D(jjNUMBER2_OP2),'-',           CNUMBER_CMD,    CNUMBER_CMD,BIGINT_CMD, ALLOW_NC | ALLOW_RING}
,{D(jjPOLY2_OP2), '-',            CPOLY_CMD,      CPOLY_CMD,  CPOLY_CMD, ALLOW_NC | ALLOW_RING}
,{D(jjPOLY2_OP2), '-',            CPOLY_CMD,      CPOLY_CMD,  INT_CMD, ALLOW_NC | ALLOW_RING}
,{D(jjPOLY2_OP2), '-',            CPOLY_CMD,      INT_CMD,    CPOLY_CMD,  ALLOW_NC | ALLOW_RING}
,{D(jjMINUS_BIM), '-',            CMATRIX_CMD,    CMATRIX_CMD,CMATRIX_CMD, ALLOW_NC | ALLOW_RING}
#endif
,{D(jjRMINUS),    '-',            RING_CMD,       RING_CMD,   STRING_CMD, ALLOW_LP | ALLOW_RING}
,{D(jjMINUS_B_P), '-',            BUCKET_CMD,     BUCKET_CMD, POLY_CMD, ALLOW_NC | ALLOW_RING}
,{  jjWRONG2 ,    '-',            NONE,           IDEAL_CMD,  IDEAL_CMD, ALLOW_NC | ALLOW_RING}
,{  jjWRONG2 ,    '-',            NONE,           MODUL_CMD,  MODUL_CMD, ALLOW_NC | ALLOW_RING}
,{D(jjTIMES_I),   '*',            INT_CMD,        INT_CMD,    INT_CMD, ALLOW_NC | ALLOW_RING}
,{D(jjTIMES_BI),  '*',            BIGINT_CMD,     BIGINT_CMD, BIGINT_CMD, ALLOW_NC | ALLOW_RING}
,{D(jjTIMES_N),   '*',            NUMBER_CMD,     NUMBER_CMD, NUMBER_CMD, ALLOW_NC | ALLOW_RING}
,{D(jjTIMES_P),   '*',            POLY_CMD,       POLY_CMD,   POLY_CMD, ALLOW_NC | ALLOW_RING}
,{D(jjTIMES_P),   '*',            VECTOR_CMD,     POLY_CMD,   VECTOR_CMD, ALLOW_NC | ALLOW_RING}
,{D(jjTIMES_P),   '*',            VECTOR_CMD,     VECTOR_CMD, POLY_CMD, ALLOW_NC | ALLOW_RING}
,{D(jjTIMES_MA_P1),'*',           IDEAL_CMD,      IDEAL_CMD,  POLY_CMD, ALLOW_NC | ALLOW_RING}
,{D(jjTIMES_MA_P2),'*',           IDEAL_CMD,      POLY_CMD,   IDEAL_CMD, ALLOW_NC | ALLOW_RING}
,{D(jjTIMES_MA_P1),'*',           MODUL_CMD,      MODUL_CMD,  POLY_CMD, ALLOW_NC | ALLOW_RING}
,{D(jjTIMES_MA_P2),'*',           MODUL_CMD,      POLY_CMD,   MODUL_CMD, ALLOW_NC | ALLOW_RING}
,{D(jjTIMES_ID),  '*',            IDEAL_CMD,      IDEAL_CMD,  IDEAL_CMD, ALLOW_NC | ALLOW_RING}
,{D(jjTIMES_MA_P1),'*',           MODUL_CMD,      IDEAL_CMD,  VECTOR_CMD, ALLOW_NC | ALLOW_RING}
,{D(jjTIMES_MA_P2),'*',           MODUL_CMD,      VECTOR_CMD, IDEAL_CMD, ALLOW_NC | ALLOW_RING}
,{D(jjTIMES_ID),  '*',            MODUL_CMD,      IDEAL_CMD,  MODUL_CMD, ALLOW_NC | ALLOW_RING}
,{D(jjTIMES_ID),  '*',            MODUL_CMD,      MODUL_CMD,  IDEAL_CMD, ALLOW_NC | ALLOW_RING}
,{D(jjTIMES_MA_P1),'*',           MATRIX_CMD,     MATRIX_CMD, POLY_CMD, ALLOW_NC | ALLOW_RING}
,{D(jjTIMES_MA_P2),'*',           MATRIX_CMD,     POLY_CMD,   MATRIX_CMD, ALLOW_NC | ALLOW_RING}
,{D(jjTIMES_MA_N1),'*',           MATRIX_CMD,     MATRIX_CMD, NUMBER_CMD, ALLOW_NC | ALLOW_RING}
,{D(jjTIMES_MA_N2),'*',           MATRIX_CMD,     NUMBER_CMD, MATRIX_CMD, ALLOW_NC | ALLOW_RING}
,{D(jjTIMES_MA_I1),'*',           MATRIX_CMD,     MATRIX_CMD, INT_CMD, ALLOW_NC | ALLOW_RING}
,{D(jjTIMES_MA_I2),'*',           MATRIX_CMD,     INT_CMD,    MATRIX_CMD, ALLOW_NC | ALLOW_RING}
,{D(jjTIMES_MA),  '*',            MATRIX_CMD,     MATRIX_CMD, MATRIX_CMD, ALLOW_NC | ALLOW_RING}
,{D(jjTIMES_MA_BI1),'*',          MATRIX_CMD,     MATRIX_CMD, BIGINT_CMD, ALLOW_NC | ALLOW_RING}
,{D(jjTIMES_MA_BI2),'*',          MATRIX_CMD,     BIGINT_CMD, MATRIX_CMD, ALLOW_NC | ALLOW_RING}
,{D(jjTIMES_SM),  '*',            SMATRIX_CMD,    SMATRIX_CMD,SMATRIX_CMD, ALLOW_NC | ALLOW_RING}
,{D(jjTIMES_BIM), '*',            BIGINTMAT_CMD,  BIGINTMAT_CMD, BIGINTMAT_CMD, ALLOW_NC | ALLOW_RING}
,{D(jjOP_BIM_I),  '*',            BIGINTMAT_CMD,  BIGINTMAT_CMD, INT_CMD, ALLOW_NC | ALLOW_RING}
,{D(jjOP_I_BIM),  '*',            BIGINTMAT_CMD,  INT_CMD, BIGINTMAT_CMD, ALLOW_NC | ALLOW_RING}
,{D(jjOP_BIM_BI),  '*',           BIGINTMAT_CMD,  BIGINTMAT_CMD, BIGINT_CMD, ALLOW_PLURAL | ALLOW_RING}
,{D(jjOP_BI_BIM),  '*',           BIGINTMAT_CMD,  BIGINT_CMD, BIGINTMAT_CMD, ALLOW_NC | ALLOW_RING}
,{D(jjOP_IV_I),   '*',            INTVEC_CMD,     INTVEC_CMD, INT_CMD, ALLOW_NC | ALLOW_RING}
,{D(jjOP_I_IV),   '*',            INTVEC_CMD,     INT_CMD,    INTVEC_CMD, ALLOW_NC | ALLOW_RING}
,{D(jjOP_IV_I),   '*',            INTMAT_CMD,     INTMAT_CMD, INT_CMD, ALLOW_NC | ALLOW_RING}
,{D(jjOP_I_IV),   '*',            INTMAT_CMD,     INT_CMD,    INTMAT_CMD, ALLOW_NC | ALLOW_RING}
,{D(jjTIMES_IV),  '*',            INTVEC_CMD,     INTMAT_CMD, INTVEC_CMD, ALLOW_NC | ALLOW_RING}
,{D(jjTIMES_IV),  '*',            INTMAT_CMD,     INTMAT_CMD, INTMAT_CMD, ALLOW_NC | ALLOW_RING}
,{D(jjTIMES_IV),  '*',            INTMAT_CMD,     INTVEC_CMD, INTMAT_CMD, ALLOW_NC | ALLOW_RING}
#ifdef SINGULAR_4_2
,{D(jjNUMBER2_OP2),'*',           CNUMBER_CMD,    CNUMBER_CMD,CNUMBER_CMD, ALLOW_NC | ALLOW_RING}
,{D(jjNUMBER2_OP2),'*',           CNUMBER_CMD,    INT_CMD,    CNUMBER_CMD, ALLOW_NC | ALLOW_RING}
,{D(jjNUMBER2_OP2),'*',           CNUMBER_CMD,    CNUMBER_CMD,INT_CMD, ALLOW_NC | ALLOW_RING}
,{D(jjNUMBER2_OP2),'*',           CNUMBER_CMD,    BIGINT_CMD, CNUMBER_CMD, ALLOW_NC | ALLOW_RING}
,{D(jjNUMBER2_OP2),'*',           CNUMBER_CMD,    CNUMBER_CMD,BIGINT_CMD, ALLOW_NC | ALLOW_RING}
,{D(jjPOLY2_OP2), '*',            CPOLY_CMD,      CPOLY_CMD,  CPOLY_CMD, ALLOW_NC | ALLOW_RING}
,{D(jjPOLY2_OP2), '*',            CPOLY_CMD,      CPOLY_CMD,  INT_CMD, ALLOW_NC | ALLOW_RING}
,{D(jjPOLY2_OP2), '*',            CPOLY_CMD,      INT_CMD,    CPOLY_CMD,  ALLOW_NC | ALLOW_RING}
,{D(jjTIMES_BIM), '*',            CMATRIX_CMD,    CMATRIX_CMD, CMATRIX_CMD, ALLOW_NC | ALLOW_RING}
#endif
,{  jjWRONG2,     '*',            0,              MODUL_CMD, MODUL_CMD, ALLOW_NC | ALLOW_RING|NO_CONVERSION}
,{D(jjDIV_N),     '/',            NUMBER_CMD,     NUMBER_CMD, NUMBER_CMD, ALLOW_NC | ALLOW_RING}
,{D(jjDIV_P),     '/',            POLY_CMD,       POLY_CMD,   POLY_CMD, ALLOW_NC | ALLOW_RING}
,{D(jjDIV_P),     '/',            VECTOR_CMD,     VECTOR_CMD, POLY_CMD, ALLOW_NC | ALLOW_RING}
,{D(jjDIV_Ma),    '/',            MATRIX_CMD,     MATRIX_CMD, POLY_CMD, ALLOW_PLURAL | NO_RING}
,{D(jjDIVMOD_I),  '/',            INT_CMD,        INT_CMD,    INT_CMD, ALLOW_NC | ALLOW_RING}
,{D(jjDIV_BI),    '/',            BIGINT_CMD,     BIGINT_CMD, BIGINT_CMD, ALLOW_NC | ALLOW_RING}
,{D(jjOP_IV_I),   '/',            INTVEC_CMD,     INTVEC_CMD, INT_CMD, ALLOW_NC | ALLOW_RING}
,{D(jjOP_IV_I),   '/',            INTMAT_CMD,     INTMAT_CMD, INT_CMD, ALLOW_NC | ALLOW_RING}
,{D(jjCRING_Zp),  '/',            CRING_CMD,      CRING_CMD,  INT_CMD, ALLOW_PLURAL | ALLOW_RING}
,{D(jjCRING_Zm),  '/',            CRING_CMD,      CRING_CMD,  BIGINT_CMD, ALLOW_PLURAL | ALLOW_RING}
#ifdef SINGULAR_4_2
,{D(jjNUMBER2_OP2),'/',           CNUMBER_CMD,    CNUMBER_CMD,CNUMBER_CMD, ALLOW_NC | ALLOW_RING}
,{D(jjNUMBER2_OP2),'/',           CNUMBER_CMD,    INT_CMD,    CNUMBER_CMD, ALLOW_NC | ALLOW_RING}
,{D(jjNUMBER2_OP2),'/',           CNUMBER_CMD,    CNUMBER_CMD,INT_CMD, ALLOW_NC | ALLOW_RING}
,{D(jjNUMBER2_OP2),'/',           CNUMBER_CMD,    BIGINT_CMD, CNUMBER_CMD, ALLOW_NC | ALLOW_RING}
,{D(jjNUMBER2_OP2),'/',           CNUMBER_CMD,    CNUMBER_CMD,BIGINT_CMD, ALLOW_NC | ALLOW_RING}
#endif
,{D(jjDIVMOD_I),  INTDIV_CMD,     INT_CMD,        INT_CMD,    INT_CMD, ALLOW_NC | ALLOW_RING}
,{D(jjDIV_BI),    INTDIV_CMD,     BIGINT_CMD,     BIGINT_CMD, BIGINT_CMD, ALLOW_NC | ALLOW_RING}
,{D(jjOP_IV_I),   INTDIV_CMD,     INTVEC_CMD,     INTVEC_CMD, INT_CMD, ALLOW_NC | ALLOW_RING}
,{D(jjOP_IV_I),   INTDIV_CMD,     INTMAT_CMD,     INTMAT_CMD, INT_CMD, ALLOW_NC | ALLOW_RING}
,{D(jjDIV_P),     INTDIV_CMD,     POLY_CMD,       POLY_CMD,   POLY_CMD, ALLOW_PLURAL | ALLOW_RING}
,{D(jjDIV_P),     INTDIV_CMD,     VECTOR_CMD,     VECTOR_CMD, POLY_CMD, ALLOW_PLURAL | ALLOW_RING}
,{D(jjDIVMOD_I),  '%',            INT_CMD,        INT_CMD,    INT_CMD, ALLOW_NC | ALLOW_RING}
,{D(jjMOD_BI),    '%',            BIGINT_CMD,     BIGINT_CMD, BIGINT_CMD, ALLOW_NC | ALLOW_RING}
,{D(jjOP_IV_I),   '%',            INTVEC_CMD,     INTVEC_CMD, INT_CMD, ALLOW_NC | ALLOW_RING}
,{D(jjOP_IV_I),   '%',            INTMAT_CMD,     INTMAT_CMD, INT_CMD, ALLOW_NC | ALLOW_RING}
,{D(jjMOD_N),     '%',            NUMBER_CMD,     NUMBER_CMD, NUMBER_CMD, ALLOW_NC | ALLOW_RING}
,{D(jjMOD_P),     '%',            POLY_CMD,       POLY_CMD,   POLY_CMD, NO_NC | NO_RING}
#ifdef SINGULAR_4_2
,{D(jjNUMBER2_OP2),'%',           CNUMBER_CMD,    CNUMBER_CMD,CNUMBER_CMD, ALLOW_NC | ALLOW_RING}
,{D(jjNUMBER2_OP2),'%',           CNUMBER_CMD,    INT_CMD,    CNUMBER_CMD, ALLOW_NC | ALLOW_RING}
,{D(jjNUMBER2_OP2),'%',           CNUMBER_CMD,    CNUMBER_CMD,INT_CMD, ALLOW_NC | ALLOW_RING}
,{D(jjNUMBER2_OP2),'%',           CNUMBER_CMD,    BIGINT_CMD, CNUMBER_CMD, ALLOW_NC | ALLOW_RING}
,{D(jjNUMBER2_OP2),'%',           CNUMBER_CMD,    CNUMBER_CMD,BIGINT_CMD, ALLOW_NC | ALLOW_RING}
#endif
,{D(jjPOWER_I),   '^',            INT_CMD,        INT_CMD,    INT_CMD, ALLOW_NC | ALLOW_RING}
,{D(jjPOWER_BI),   '^',           BIGINT_CMD,     BIGINT_CMD, INT_CMD, ALLOW_NC | ALLOW_RING}
,{D(jjPOWER_N),   '^',            NUMBER_CMD,     NUMBER_CMD, INT_CMD, ALLOW_NC | ALLOW_RING}
,{D(jjPOWER_P),   '^',            POLY_CMD,       POLY_CMD,   INT_CMD, ALLOW_NC | ALLOW_RING}
,{D(jjPOWER_ID),  '^',            IDEAL_CMD,      IDEAL_CMD,  INT_CMD, ALLOW_PLURAL | ALLOW_RING}
#ifdef SINGULAR_4_2
,{D(jjNUMBER2_POW),'^',           CNUMBER_CMD,    CNUMBER_CMD,INT_CMD, ALLOW_NC | ALLOW_RING}
,{D(jjPOLY2_POW), '^',            CPOLY_CMD,      CPOLY_CMD,  INT_CMD, ALLOW_PLURAL | ALLOW_RING}
#endif
,{D(jjLE_I),      LE,             INT_CMD,        INT_CMD,    INT_CMD, ALLOW_NC | ALLOW_RING}
,{D(jjLE_BI),     LE,             INT_CMD,        BIGINT_CMD, BIGINT_CMD, ALLOW_NC | ALLOW_RING}
,{D(jjLE_N),      LE,             INT_CMD,        NUMBER_CMD, NUMBER_CMD, ALLOW_NC | ALLOW_RING}
,{D(jjCOMPARE_S), LE,             INT_CMD,        STRING_CMD, STRING_CMD, ALLOW_NC | ALLOW_RING}
,{D(jjCOMPARE_IV_I),LE,           INT_CMD,        INTVEC_CMD, INT_CMD, ALLOW_NC | ALLOW_RING}
,{D(jjCOMPARE_IV),LE,             INT_CMD,        INTVEC_CMD, INTVEC_CMD, ALLOW_NC | ALLOW_RING}
,{D(jjCOMPARE_P), LE,             INT_CMD,        POLY_CMD,   POLY_CMD, ALLOW_NC | ALLOW_RING}
,{D(jjCOMPARE_P), LE,             INT_CMD,        VECTOR_CMD, VECTOR_CMD, ALLOW_NC | ALLOW_RING}
,{D(jjCOMPARE_MA),LE,             INT_CMD,        MATRIX_CMD, MATRIX_CMD, ALLOW_NC | ALLOW_RING | NO_CONVERSION }
,{D(jjLT_I),      '<',            INT_CMD,        INT_CMD,    INT_CMD, ALLOW_NC | ALLOW_RING}
,{D(jjLT_BI),     '<',            INT_CMD,        BIGINT_CMD, BIGINT_CMD, ALLOW_NC | ALLOW_RING}
,{D(jjLT_N),      '<',            INT_CMD,        NUMBER_CMD, NUMBER_CMD, ALLOW_NC | ALLOW_RING}
,{D(jjCOMPARE_IV_I),'<',          INT_CMD,        INTVEC_CMD, INT_CMD, ALLOW_NC | ALLOW_RING}
,{D(jjCOMPARE_IV),'<',            INT_CMD,        INTVEC_CMD, INTVEC_CMD, ALLOW_NC | ALLOW_RING}
,{D(jjCOMPARE_S), '<',            INT_CMD,        STRING_CMD, STRING_CMD, ALLOW_NC | ALLOW_RING}
,{D(jjCOMPARE_P), '<',            INT_CMD,        POLY_CMD,   POLY_CMD, ALLOW_NC | ALLOW_RING}
,{D(jjCOMPARE_P), '<',            INT_CMD,        VECTOR_CMD, VECTOR_CMD, ALLOW_NC | ALLOW_RING}
,{D(jjCOMPARE_MA),'<',            INT_CMD,        MATRIX_CMD, MATRIX_CMD, ALLOW_NC | ALLOW_RING | NO_CONVERSION }
,{D(jjGE_I),      GE,             INT_CMD,        INT_CMD,    INT_CMD, ALLOW_NC | ALLOW_RING}
,{D(jjGE_BI),     GE,             INT_CMD,        BIGINT_CMD, BIGINT_CMD, ALLOW_NC | ALLOW_RING}
,{D(jjGE_N),      GE,             INT_CMD,        NUMBER_CMD, NUMBER_CMD, ALLOW_NC | ALLOW_RING}
,{D(jjCOMPARE_S), GE,             INT_CMD,        STRING_CMD, STRING_CMD, ALLOW_NC | ALLOW_RING}
,{D(jjCOMPARE_IV_I),GE,           INT_CMD,        INTVEC_CMD, INT_CMD, ALLOW_NC | ALLOW_RING}
,{D(jjCOMPARE_IV),GE,             INT_CMD,        INTVEC_CMD, INTVEC_CMD, ALLOW_NC | ALLOW_RING}
,{D(jjCOMPARE_P), GE,             INT_CMD,        POLY_CMD,   POLY_CMD, ALLOW_NC | ALLOW_RING}
,{D(jjCOMPARE_P), GE,             INT_CMD,        VECTOR_CMD, VECTOR_CMD, ALLOW_NC | ALLOW_RING}
,{D(jjCOMPARE_MA),GE,             INT_CMD,        MATRIX_CMD, MATRIX_CMD, ALLOW_NC | ALLOW_RING | NO_CONVERSION }
,{D(jjGT_I),      '>',            INT_CMD,        INT_CMD,    INT_CMD, ALLOW_NC | ALLOW_RING}
,{D(jjGT_BI),     '>',            INT_CMD,        BIGINT_CMD, BIGINT_CMD, ALLOW_NC | ALLOW_RING}
,{D(jjGT_N),      '>',            INT_CMD,        NUMBER_CMD, NUMBER_CMD, ALLOW_NC | ALLOW_RING}
,{D(jjCOMPARE_S), '>',            INT_CMD,        STRING_CMD, STRING_CMD, ALLOW_NC | ALLOW_RING}
,{D(jjCOMPARE_IV_I),'>',          INT_CMD,        INTVEC_CMD, INT_CMD, ALLOW_NC | ALLOW_RING}
,{D(jjCOMPARE_IV),'>',            INT_CMD,        INTVEC_CMD, INTVEC_CMD, ALLOW_NC | ALLOW_RING}
,{D(jjCOMPARE_P), '>',            INT_CMD,        POLY_CMD,   POLY_CMD, ALLOW_NC | ALLOW_RING}
,{D(jjCOMPARE_P), '>',            INT_CMD,        VECTOR_CMD, VECTOR_CMD, ALLOW_NC | ALLOW_RING}
,{D(jjCOMPARE_MA),'>',            INT_CMD,        MATRIX_CMD, MATRIX_CMD, ALLOW_NC | ALLOW_RING | NO_CONVERSION }
,{D(jjAND_I),     '&',            INT_CMD,        INT_CMD,    INT_CMD, ALLOW_NC | ALLOW_RING}
,{D(jjOR_I),      '|',            INT_CMD,        INT_CMD,    INT_CMD, ALLOW_NC | ALLOW_RING}
,{D(jjEQUAL_I),   EQUAL_EQUAL,    INT_CMD,        INT_CMD,    INT_CMD, ALLOW_NC | ALLOW_RING}
,{D(jjEQUAL_I),   EQUAL_EQUAL,    INT_CMD,        RING_CMD,   INT_CMD, ALLOW_NC | ALLOW_RING}
,{D(jjEQUAL_I),   EQUAL_EQUAL,    INT_CMD,        RING_CMD,   RING_CMD, ALLOW_NC | ALLOW_RING}
,{D(jjEQUAL_BI),  EQUAL_EQUAL,    INT_CMD,        BIGINT_CMD, BIGINT_CMD, ALLOW_NC | ALLOW_RING}
,{D(jjEQUAL_N),   EQUAL_EQUAL,    INT_CMD,        NUMBER_CMD, NUMBER_CMD, ALLOW_NC | ALLOW_RING}
,{D(jjCOMPARE_S), EQUAL_EQUAL,    INT_CMD,        STRING_CMD, STRING_CMD, ALLOW_NC | ALLOW_RING}
,{D(jjEQUAL_P),   EQUAL_EQUAL,    INT_CMD,        POLY_CMD,   POLY_CMD, ALLOW_NC | ALLOW_RING}
,{D(jjEQUAL_P),   EQUAL_EQUAL,    INT_CMD,        VECTOR_CMD, VECTOR_CMD, ALLOW_NC | ALLOW_RING}
,{D(jjCOMPARE_IV_I),EQUAL_EQUAL,  INT_CMD,        INTVEC_CMD, INT_CMD, ALLOW_NC | ALLOW_RING}
,{D(jjCOMPARE_IV),EQUAL_EQUAL,    INT_CMD,        INTVEC_CMD, INTVEC_CMD, ALLOW_NC | ALLOW_RING}
,{D(jjCOMPARE_IV),EQUAL_EQUAL,    INT_CMD,        INTMAT_CMD, INTMAT_CMD, ALLOW_NC | ALLOW_RING}
,{D(jjCOMPARE_BIM),EQUAL_EQUAL,   INT_CMD,        BIGINTMAT_CMD, BIGINTMAT_CMD, ALLOW_NC | ALLOW_RING}
#ifdef SINGULAR_4_2
,{D(jjCOMPARE_BIM),EQUAL_EQUAL,   INT_CMD,        CMATRIX_CMD, CMATRIX_CMD, ALLOW_NC | ALLOW_RING}
#endif
,{D(jjEQUAL_CR),  EQUAL_EQUAL,    INT_CMD,        CRING_CMD,  CRING_CMD, ALLOW_NC | ALLOW_RING}
,{D(jjEQUAL_Ma),  EQUAL_EQUAL,    INT_CMD,        MATRIX_CMD, MATRIX_CMD, ALLOW_NC | ALLOW_RING}
,{D(jjEQUAL_SM),  EQUAL_EQUAL,    INT_CMD,        SMATRIX_CMD,SMATRIX_CMD, ALLOW_NC | ALLOW_RING}
,{D(jjEQUAL_R),   EQUAL_EQUAL,    INT_CMD,        RING_CMD,   RING_CMD, ALLOW_NC | ALLOW_RING}
,{  jjWRONG2 ,    EQUAL_EQUAL,    0,              IDEAL_CMD,  IDEAL_CMD, ALLOW_NC | ALLOW_RING}
,{  jjWRONG2 ,    EQUAL_EQUAL,    0,              MODUL_CMD,  MODUL_CMD, ALLOW_NC | ALLOW_RING}
,{  jjWRONG2 ,    EQUAL_EQUAL,    0,              IDEAL_CMD,  MODUL_CMD, ALLOW_NC | ALLOW_RING}
,{  jjWRONG2 ,    EQUAL_EQUAL,    0,              MODUL_CMD,  IDEAL_CMD, ALLOW_NC | ALLOW_RING}
,{D(jjEQUAL_I),   NOTEQUAL,       INT_CMD,        INT_CMD,    INT_CMD, ALLOW_NC | ALLOW_RING}
,{D(jjEQUAL_BI),  NOTEQUAL,       INT_CMD,        BIGINT_CMD, BIGINT_CMD, ALLOW_NC | ALLOW_RING}
,{D(jjEQUAL_N),   NOTEQUAL,       INT_CMD,        NUMBER_CMD, NUMBER_CMD, ALLOW_NC | ALLOW_RING}
,{D(jjCOMPARE_S), NOTEQUAL,       INT_CMD,        STRING_CMD, STRING_CMD, ALLOW_NC | ALLOW_RING}
,{D(jjEQUAL_P),   NOTEQUAL,       INT_CMD,        POLY_CMD,   POLY_CMD, ALLOW_NC | ALLOW_RING}
,{D(jjEQUAL_P),   NOTEQUAL,       INT_CMD,        VECTOR_CMD, VECTOR_CMD, ALLOW_NC | ALLOW_RING}
,{D(jjCOMPARE_IV),NOTEQUAL,       INT_CMD,        INTVEC_CMD, INTVEC_CMD, ALLOW_NC | ALLOW_RING}
,{D(jjCOMPARE_IV),NOTEQUAL,       INT_CMD,        INTMAT_CMD, INTMAT_CMD, ALLOW_NC | ALLOW_RING}
,{D(jjEQUAL_Ma),  NOTEQUAL,       INT_CMD,        MATRIX_CMD, MATRIX_CMD, ALLOW_NC | ALLOW_RING}
,{D(jjEQUAL_SM),  NOTEQUAL,       INT_CMD,        SMATRIX_CMD,SMATRIX_CMD, ALLOW_NC | ALLOW_RING}
,{  jjWRONG2 ,    NOTEQUAL,       0,              IDEAL_CMD,  IDEAL_CMD, ALLOW_NC | ALLOW_RING}
,{  jjWRONG2 ,    NOTEQUAL,       0,              MODUL_CMD,  MODUL_CMD, ALLOW_NC | ALLOW_RING}
,{  jjWRONG2 ,    NOTEQUAL,       0,              IDEAL_CMD,  MODUL_CMD, ALLOW_NC | ALLOW_RING}
,{  jjWRONG2 ,    NOTEQUAL,       0,              MODUL_CMD,  IDEAL_CMD, ALLOW_NC | ALLOW_RING}
,{D(jjEQUAL_R),   NOTEQUAL,       INT_CMD,        RING_CMD,   RING_CMD, ALLOW_NC | ALLOW_RING}
,{D(jjDOTDOT),    DOTDOT,         INTVEC_CMD,     INT_CMD,    INT_CMD, ALLOW_NC | ALLOW_RING}
,{D(jjINDEX_I),   '[',            INT_CMD,        INTVEC_CMD, INT_CMD, ALLOW_NC | ALLOW_RING}
,{D(jjINDEX_IV),  '[',            INT_CMD,        INTVEC_CMD, INTVEC_CMD, ALLOW_NC | ALLOW_RING}
,{D(jjINDEX_I),   '[',            POLY_CMD,       IDEAL_CMD,  INT_CMD, ALLOW_NC | ALLOW_RING}
,{D(jjINDEX_I),   '[',            POLY_CMD,       MAP_CMD,    INT_CMD, ALLOW_NC | ALLOW_RING}
,{D(jjINDEX_IV),  '[',            POLY_CMD,       IDEAL_CMD,  INTVEC_CMD, ALLOW_NC | ALLOW_RING}
,{D(jjINDEX_I),   '[',            VECTOR_CMD,     MODUL_CMD,  INT_CMD, ALLOW_NC | ALLOW_RING}
,{D(jjINDEX_IV),  '[',            VECTOR_CMD,     MODUL_CMD,  INTVEC_CMD, ALLOW_NC | ALLOW_RING}
,{D(jjINDEX_I),   '[',            STRING_CMD,     STRING_CMD, INT_CMD, ALLOW_NC | ALLOW_RING}
,{D(jjINDEX_IV),  '[',            STRING_CMD,     STRING_CMD, INTVEC_CMD, ALLOW_NC | ALLOW_RING}
,{D(jjINDEX_I),   '[',            ANY_TYPE/*set by p*/,LIST_CMD, INT_CMD, ALLOW_NC | ALLOW_RING}
,{D(jjINDEX_IV),  '[',            ANY_TYPE/*set by p*/,LIST_CMD, INTVEC_CMD, ALLOW_NC | ALLOW_RING}
,{D(jjINDEX_P),   '[',            POLY_CMD,       POLY_CMD,   INT_CMD, ALLOW_NC | ALLOW_RING}
,{D(jjINDEX_PBu), '[',            POLY_CMD,       BUCKET_CMD, INT_CMD, ALLOW_NC | ALLOW_RING}
,{D(jjINDEX_P_IV),'[',            POLY_CMD,       POLY_CMD,   INTVEC_CMD, ALLOW_NC | ALLOW_RING}
,{D(jjINDEX_V),   '[',            POLY_CMD,       VECTOR_CMD, INT_CMD, ALLOW_NC | ALLOW_RING}
,{D(jjINDEX_V_IV),'[',            VECTOR_CMD,     VECTOR_CMD, INTVEC_CMD, ALLOW_NC | ALLOW_RING}
,{D(jjRING_1),    '[',            RING_CMD,     CRING_CMD, ANY_TYPE, ALLOW_NC | ALLOW_RING}
,{D(jjPROC),      '(',            ANY_TYPE/*set by p*/,PROC_CMD, DEF_CMD, ALLOW_NC | ALLOW_RING}
,{D(jjMAP),       '(',            ANY_TYPE/*set by p*/,MAP_CMD, DEF_CMD, ALLOW_PLURAL | ALLOW_RING}
,{D(jjLOAD2),     '(',            NONE,             LIB_CMD,    STRING_CMD, ALLOW_NC |ALLOW_RING}
,{D(jjKLAMMER),   '(',            ANY_TYPE/*set by p*/,ANY_TYPE, INT_CMD, ALLOW_NC | ALLOW_RING}
,{D(jjKLAMMER_IV),'(',            ANY_TYPE/*set by p*/,ANY_TYPE, INTVEC_CMD, ALLOW_NC | ALLOW_RING}
,{D(jjCOLON),     ':',            INTVEC_CMD,     INT_CMD,    INT_CMD, ALLOW_NC | ALLOW_RING}
// and the procedures with 2 arguments:
,{D(jjALIGN_V),   ALIGN_CMD,      VECTOR_CMD,     VECTOR_CMD, INT_CMD, ALLOW_NC | ALLOW_RING}
,{D(jjALIGN_M),   ALIGN_CMD,      MODUL_CMD,      MODUL_CMD,  INT_CMD, ALLOW_NC | ALLOW_RING}
,{D(atATTRIB2),   ATTRIB_CMD,     NONE/*set by p*/,DEF_CMD,   STRING_CMD, ALLOW_NC | ALLOW_RING}
,{D(jjBETTI2),    BETTI_CMD,      INTMAT_CMD,     LIST_CMD,   INT_CMD, ALLOW_PLURAL | ALLOW_RING}
,{D(syBetti2),    BETTI_CMD,      INTMAT_CMD,     RESOLUTION_CMD, INT_CMD, ALLOW_PLURAL | ALLOW_RING}
,{D(jjBETTI2_ID), BETTI_CMD,      INTMAT_CMD,     IDEAL_CMD,  INT_CMD, ALLOW_PLURAL | ALLOW_RING}
,{D(jjBETTI2_ID), BETTI_CMD,      INTMAT_CMD,     MODUL_CMD,  INT_CMD, ALLOW_PLURAL | ALLOW_RING}
#if defined(HAVE_PLURAL) || defined(HAVE_SHIFTBBA)
,{D(jjBRACKET),   BRACKET_CMD,    POLY_CMD,       POLY_CMD,   POLY_CMD, ALLOW_NC | NO_RING}
#endif
,{D(jjCHINREM_BI),CHINREM_CMD,    BIGINT_CMD,     INTVEC_CMD, INTVEC_CMD, ALLOW_PLURAL |ALLOW_RING}
//,{D(jjCHINREM_P), CHINREM_CMD,    POLY_CMD,       LIST_CMD,   INTVEC_CMD, ALLOW_PLURAL}
,{D(jjCHINREM_ID),CHINREM_CMD,    ANY_TYPE/*set by p*/,LIST_CMD,INTVEC_CMD, ALLOW_NC |ALLOW_RING}
,{D(jjCHINREM_ID),CHINREM_CMD,    ANY_TYPE/*set by p*/,LIST_CMD,LIST_CMD, ALLOW_NC |ALLOW_RING}
#ifdef SINGULAR_4_2
,{D(jjNUMBER2CR), CNUMBER_CMD,    CNUMBER_CMD,     INT_CMD,    CRING_CMD, ALLOW_NC |ALLOW_RING}
,{D(jjNUMBER2CR), CNUMBER_CMD,    CNUMBER_CMD,     BIGINT_CMD, CRING_CMD, ALLOW_NC |ALLOW_RING}
,{D(jjNUMBER2CR), CNUMBER_CMD,    CNUMBER_CMD,     NUMBER_CMD, CRING_CMD, ALLOW_NC |ALLOW_RING}
#endif
,{D(jjCOEF),      COEF_CMD,       MATRIX_CMD,     POLY_CMD,   POLY_CMD, ALLOW_NC |ALLOW_RING}
,{D(jjCOEF_Id),   COEF_CMD,       MATRIX_CMD,     IDEAL_CMD,  POLY_CMD, ALLOW_NC |ALLOW_RING}
,{D(jjCOEFFS_Id), COEFFS_CMD,     MATRIX_CMD,     IDEAL_CMD,  POLY_CMD, ALLOW_NC |ALLOW_RING}
,{D(jjCOEFFS_Id), COEFFS_CMD,     MATRIX_CMD,     MODUL_CMD,  POLY_CMD, ALLOW_NC |ALLOW_RING}
,{D(jjCOEFFS2_KB),COEFFS_CMD,     MATRIX_CMD,     IDEAL_CMD,  IDEAL_CMD, ALLOW_NC |ALLOW_RING}
,{D(jjCOEFFS2_KB),COEFFS_CMD,     MATRIX_CMD,     MODUL_CMD,  MODUL_CMD, ALLOW_NC |ALLOW_RING}
,{D(jjCONTRACT),  CONTRACT_CMD,   MATRIX_CMD,     IDEAL_CMD,  IDEAL_CMD, ALLOW_PLURAL |ALLOW_RING}
,{D(jjDEG_IV),    DEG_CMD,        INT_CMD,        POLY_CMD,   INTVEC_CMD, ALLOW_NC |ALLOW_RING}
,{D(jjDEG_IV),    DEG_CMD,        INT_CMD,        VECTOR_CMD, INTVEC_CMD, ALLOW_NC |ALLOW_RING}
,{D(jjDEG_M_IV),  DEG_CMD,        INT_CMD,        MATRIX_CMD, INTVEC_CMD, ALLOW_NC |ALLOW_RING}
,{D(jjDelete_IV), DELETE_CMD,     INTVEC_CMD,     INTVEC_CMD, INT_CMD, ALLOW_NC |ALLOW_RING}
,{D(jjDelete_ID), DELETE_CMD,     IDEAL_CMD,      IDEAL_CMD,  INT_CMD, ALLOW_NC |ALLOW_RING}
,{D(jjDelete_ID), DELETE_CMD,     MODUL_CMD,      MODUL_CMD,  INT_CMD, ALLOW_NC |ALLOW_RING}
,{D(lDelete),     DELETE_CMD,     LIST_CMD,       LIST_CMD,   INT_CMD, ALLOW_NC |ALLOW_RING}
,{D(jjDET2_S),    DET_CMD,        POLY_CMD,       SMATRIX_CMD,STRING_CMD, NO_NC |NO_RING}
,{D(jjDET2),      DET_CMD,        POLY_CMD,       MATRIX_CMD, STRING_CMD, NO_NC |ALLOW_RING}
,{D(jjDIFF_P),    DIFF_CMD,       POLY_CMD,       POLY_CMD,   POLY_CMD, ALLOW_PLURAL |ALLOW_RING}
,{D(jjDIFF_P),    DIFF_CMD,       VECTOR_CMD,     VECTOR_CMD, POLY_CMD, ALLOW_PLURAL |ALLOW_RING}
,{D(jjDIFF_ID),   DIFF_CMD,       IDEAL_CMD,      IDEAL_CMD,  POLY_CMD, ALLOW_PLURAL |ALLOW_RING}
,{D(jjDIFF_ID_ID),DIFF_CMD,       MATRIX_CMD,     IDEAL_CMD,  IDEAL_CMD, ALLOW_PLURAL |ALLOW_RING}
,{D(jjDIFF_ID),   DIFF_CMD,       MODUL_CMD,      MODUL_CMD,  POLY_CMD, ALLOW_PLURAL |ALLOW_RING}
,{D(jjDIFF_ID),   DIFF_CMD,       MATRIX_CMD,     MATRIX_CMD, POLY_CMD, ALLOW_PLURAL |ALLOW_RING}
,{D(jjDIFF_COEF), DIFF_CMD,       NUMBER_CMD,     NUMBER_CMD, NUMBER_CMD, ALLOW_PLURAL |ALLOW_RING}
,{D(jjDIM2),      DIM_CMD,        INT_CMD,        IDEAL_CMD,  IDEAL_CMD, ALLOW_PLURAL |NO_RING}
,{D(jjDIM2),      DIM_CMD,        INT_CMD,        MODUL_CMD,  IDEAL_CMD, ALLOW_PLURAL |NO_RING}
,{D(jjDIVISION),  DIVISION_CMD,   LIST_CMD,       IDEAL_CMD,  IDEAL_CMD, ALLOW_PLURAL |ALLOW_RING}
,{D(jjDIVISION),  DIVISION_CMD,   LIST_CMD,       MODUL_CMD,  MODUL_CMD, ALLOW_PLURAL |ALLOW_RING}
,{D(jjELIMIN),    ELIMINATION_CMD,IDEAL_CMD,      IDEAL_CMD,  POLY_CMD, ALLOW_PLURAL |ALLOW_RING}
,{D(jjELIMIN),    ELIMINATION_CMD,MODUL_CMD,      MODUL_CMD,  POLY_CMD, ALLOW_PLURAL |ALLOW_RING}
,{D(jjELIMIN_IV), ELIMINATION_CMD,IDEAL_CMD,      IDEAL_CMD,  INTVEC_CMD, ALLOW_PLURAL |ALLOW_RING}
,{D(jjELIMIN_IV), ELIMINATION_CMD,MODUL_CMD,      MODUL_CMD,  INTVEC_CMD, ALLOW_PLURAL |ALLOW_RING}
,{D(jjEXPORTTO),  EXPORTTO_CMD,   NONE,           PACKAGE_CMD, IDHDL, ALLOW_NC |ALLOW_RING}
,{D(jjEXTGCD_I),  EXTGCD_CMD,     LIST_CMD,       INT_CMD,    INT_CMD, ALLOW_PLURAL |ALLOW_RING}
,{D(jjEXTGCD_BI), EXTGCD_CMD,     LIST_CMD,       BIGINT_CMD, BIGINT_CMD, ALLOW_PLURAL |ALLOW_RING}
,{D(jjEXTGCD_P),  EXTGCD_CMD,     LIST_CMD,       POLY_CMD,   POLY_CMD, NO_NC |NO_RING}
,{D(jjFAC_P2),    FAC_CMD,        IDEAL_CMD,      POLY_CMD,   INT_CMD, NO_NC |NO_RING}
,{D(jjFACSTD2),   FACSTD_CMD,     LIST_CMD,       IDEAL_CMD,  IDEAL_CMD, NO_NC |NO_RING}
,{D(jjFAREY_BI),  FAREY_CMD,      NUMBER_CMD,     BIGINT_CMD,  BIGINT_CMD, ALLOW_PLURAL |NO_RING}
,{D(jjFAREY_ID),  FAREY_CMD,      IDEAL_CMD,      IDEAL_CMD,   BIGINT_CMD, ALLOW_PLURAL |NO_RING}
,{D(jjFAREY_ID),  FAREY_CMD,      MODUL_CMD,      MODUL_CMD,   BIGINT_CMD, ALLOW_PLURAL |NO_RING}
,{D(jjFAREY_ID),  FAREY_CMD,      MATRIX_CMD,     MATRIX_CMD,  BIGINT_CMD, ALLOW_PLURAL |NO_RING}
,{D(jjFAREY_LI),  FAREY_CMD,      LIST_CMD,       LIST_CMD,    BIGINT_CMD, ALLOW_PLURAL |NO_RING}
,{D(jjFETCH),     FETCH_CMD,      ANY_TYPE/*set by p*/,RING_CMD,  ANY_TYPE, ALLOW_NC |ALLOW_RING}
,{D(fglmProc),    FGLM_CMD,       IDEAL_CMD,      RING_CMD,   DEF_CMD, NO_NC |NO_RING}
,{D(fglmQuotProc),FGLMQUOT_CMD,   IDEAL_CMD,      IDEAL_CMD,  POLY_CMD, NO_NC |NO_RING}
,{D(jjFIND2),     FIND_CMD,       INT_CMD,        STRING_CMD, STRING_CMD, ALLOW_NC |ALLOW_RING}
,{D(jjFRES),      FRES_CMD,       RESOLUTION_CMD, IDEAL_CMD,  INT_CMD, NO_NC |NO_RING}
,{D(jjFRES),      FRES_CMD,       RESOLUTION_CMD, MODUL_CMD,  INT_CMD, NO_NC |NO_RING}
,{D(jjFWALK),     FWALK_CMD,      IDEAL_CMD,      RING_CMD,   DEF_CMD, NO_NC |NO_RING}
,{D(jjGCD_I),     GCD_CMD,        INT_CMD,        INT_CMD,    INT_CMD, ALLOW_PLURAL |ALLOW_RING}
,{D(jjGCD_N),     GCD_CMD,        NUMBER_CMD,     NUMBER_CMD, NUMBER_CMD, ALLOW_PLURAL |ALLOW_RING}
,{D(jjGCD_BI),    GCD_CMD,        BIGINT_CMD,     BIGINT_CMD, BIGINT_CMD, ALLOW_PLURAL |ALLOW_RING}
,{D(jjGCD_P),     GCD_CMD,        POLY_CMD,       POLY_CMD,   POLY_CMD, NO_NC |ALLOW_RING}
,{D(jjHILBERT2),  HILBERT_CMD,    INTVEC_CMD,     IDEAL_CMD,  INT_CMD, ALLOW_PLURAL | ALLOW_RING | NO_ZERODIVISOR}
,{D(jjHILBERT2),  HILBERT_CMD,    INTVEC_CMD,     MODUL_CMD,  INT_CMD, ALLOW_PLURAL | ALLOW_RING | NO_ZERODIVISOR}
,{D(jjHOMOG1_W),  HOMOG_CMD,      INT_CMD,        IDEAL_CMD,  INTVEC_CMD, ALLOW_PLURAL |ALLOW_RING}
,{D(jjHOMOG1_W),  HOMOG_CMD,      INT_CMD,        MODUL_CMD,  INTVEC_CMD, ALLOW_PLURAL |ALLOW_RING}
,{D(jjHOMOG_P),   HOMOG_CMD,      POLY_CMD,       POLY_CMD,   POLY_CMD, ALLOW_PLURAL |ALLOW_RING}
,{D(jjHOMOG_P),   HOMOG_CMD,      VECTOR_CMD,     VECTOR_CMD, POLY_CMD, ALLOW_PLURAL |ALLOW_RING}
,{D(jjHOMOG_ID),  HOMOG_CMD,      IDEAL_CMD,      IDEAL_CMD,  POLY_CMD, ALLOW_PLURAL |ALLOW_RING}
,{D(jjHOMOG_ID),  HOMOG_CMD,      MODUL_CMD,      MODUL_CMD,  POLY_CMD, ALLOW_PLURAL |ALLOW_RING}
,{D(jjRES),       HRES_CMD,       RESOLUTION_CMD, IDEAL_CMD,  INT_CMD, NO_NC |NO_RING}
,{D(jjFETCH),     IMAP_CMD,       ANY_TYPE/*set by p*/,RING_CMD,  ANY_TYPE, ALLOW_NC |ALLOW_RING}
,{D(jjIMPORTFROM),IMPORTFROM_CMD, NONE,           PACKAGE_CMD, ANY_TYPE, ALLOW_NC |ALLOW_RING}
,{D(jjINDEPSET2), INDEPSET_CMD,   LIST_CMD,       IDEAL_CMD,  INT_CMD, NO_NC |NO_RING}
,{D(lInsert),     INSERT_CMD,     LIST_CMD,       LIST_CMD,   DEF_CMD, ALLOW_NC |ALLOW_RING}
,{D(jjINTERPOLATION),INTERPOLATE_CMD,IDEAL_CMD,   LIST_CMD,   INTVEC_CMD, NO_NC |NO_RING}
,{D(jjINTERSECT), INTERSECT_CMD,  IDEAL_CMD,      IDEAL_CMD,  IDEAL_CMD, ALLOW_PLURAL |ALLOW_RING}
,{D(jjINTERSECT), INTERSECT_CMD,  MODUL_CMD,      MODUL_CMD,  MODUL_CMD, ALLOW_PLURAL |ALLOW_RING}
,{D(jjJanetBasis2), JANET_CMD,    IDEAL_CMD,      IDEAL_CMD,  INT_CMD, ALLOW_PLURAL |NO_RING}
,{D(jjJET_P),     JET_CMD,        POLY_CMD,       POLY_CMD,   INT_CMD, ALLOW_PLURAL |ALLOW_RING}
,{D(jjJET_ID),    JET_CMD,        IDEAL_CMD,      IDEAL_CMD,  INT_CMD, ALLOW_PLURAL |ALLOW_RING}
,{D(jjJET_P),     JET_CMD,        VECTOR_CMD,     VECTOR_CMD, INT_CMD, ALLOW_PLURAL |ALLOW_RING}
,{D(jjJET_ID),    JET_CMD,        MODUL_CMD,      MODUL_CMD,  INT_CMD, ALLOW_PLURAL |ALLOW_RING}
,{D(jjJET_ID),    JET_CMD,        MATRIX_CMD,     MATRIX_CMD,  INT_CMD, ALLOW_PLURAL |ALLOW_RING}
,{D(jjKBASE2),    KBASE_CMD,      IDEAL_CMD,      IDEAL_CMD,  INT_CMD, ALLOW_PLURAL |ALLOW_RING |WARN_RING} /*ring-cf: warning at top level*/
,{D(jjKBASE2),    KBASE_CMD,      MODUL_CMD,      MODUL_CMD,  INT_CMD, ALLOW_PLURAL |ALLOW_RING |WARN_RING} /*ring-cf: warning at top level*/
,{D(jjKERNEL),    KERNEL_CMD,     IDEAL_CMD, RING_CMD,        ANY_TYPE, ALLOW_PLURAL |ALLOW_RING}
,{D(atKILLATTR2), KILLATTR_CMD,   NONE,           IDHDL,      STRING_CMD, ALLOW_NC |ALLOW_RING}
,{D(jjKoszul),    KOSZUL_CMD,     MATRIX_CMD,     INT_CMD,    INT_CMD, NO_NC |ALLOW_RING}
,{D(jjKoszul_Id), KOSZUL_CMD,     MATRIX_CMD,     INT_CMD,    IDEAL_CMD, NO_NC |ALLOW_RING}
,{D(jjRES),       KRES_CMD,       RESOLUTION_CMD, IDEAL_CMD,  INT_CMD, NO_NC |NO_RING}
,{D(jjLIFT),      LIFT_CMD,       MATRIX_CMD,     IDEAL_CMD,  IDEAL_CMD, ALLOW_NC |ALLOW_RING}
,{D(jjLIFT),      LIFT_CMD,       MATRIX_CMD,     MODUL_CMD,  MODUL_CMD, ALLOW_NC |ALLOW_RING}
,{D(jjLIFTSTD),   LIFTSTD_CMD,    IDEAL_CMD,      IDEAL_CMD,  MATRIX_CMD, ALLOW_NC |ALLOW_RING}
,{D(jjLIFTSTD),   LIFTSTD_CMD,    MODUL_CMD,      MODUL_CMD,  MATRIX_CMD, ALLOW_NC |ALLOW_RING}
,{D(jjLOAD_E),    LOAD_CMD,       NONE,           STRING_CMD, STRING_CMD, ALLOW_NC |ALLOW_RING}
,{D(jjRES),       LRES_CMD,       RESOLUTION_CMD, IDEAL_CMD,  INT_CMD, NO_NC |NO_RING}
,{D(jjMODULO),    MODULO_CMD,     MODUL_CMD,      IDEAL_CMD,  IDEAL_CMD, ALLOW_PLURAL |ALLOW_RING}
,{D(jjMODULO),    MODULO_CMD,     MODUL_CMD,      MODUL_CMD,  MODUL_CMD, ALLOW_PLURAL |ALLOW_RING}
,{D(jjMONITOR2),  MONITOR_CMD,    NONE,           LINK_CMD, STRING_CMD, ALLOW_NC |ALLOW_RING}
//,{D(jjRES),       MRES_CMD,       LIST_CMD,       IDEAL_CMD,  INT_CMD, NO_NC |ALLOW_RING}
//,{D(jjRES),       MRES_CMD,       LIST_CMD,       MODUL_CMD,  INT_CMD, NO_NC |ALLOW_RING}
,{D(nuMPResMat),  MPRES_CMD,      MODUL_CMD,      IDEAL_CMD,  INT_CMD, NO_NC |NO_RING}
,{D(jjNEWSTRUCT2),NEWSTRUCT_CMD,  NONE,           STRING_CMD, STRING_CMD, ALLOW_NC |ALLOW_RING}
,{D(jjRES),       MRES_CMD,       RESOLUTION_CMD, IDEAL_CMD,  INT_CMD, ALLOW_PLURAL |ALLOW_RING}
,{D(jjRES),       MRES_CMD,       RESOLUTION_CMD, MODUL_CMD,  INT_CMD, ALLOW_PLURAL |ALLOW_RING}
//,{D(nuMPResMat),  MPRES_CMD,      MODUL_CMD,      IDEAL_CMD,  INT_CMD, NO_NC |ALLOW_RING}
#ifdef SINGULAR_4_2
,{D(jjNUMBER2CR), CNUMBER_CMD,    CNUMBER_CMD,    INT_CMD,    CRING_CMD, ALLOW_NC |ALLOW_RING}
,{D(jjNUMBER2CR), CNUMBER_CMD,    CNUMBER_CMD,    BIGINT_CMD, CRING_CMD, ALLOW_NC |ALLOW_RING}
,{D(jjNUMBER2CR), CNUMBER_CMD,    CNUMBER_CMD,    NUMBER_CMD, CRING_CMD, ALLOW_NC |ALLOW_RING}
,{D(jjNUMBER2CR), CNUMBER_CMD,    CNUMBER_CMD,    CNUMBER_CMD,CRING_CMD, ALLOW_NC |ALLOW_RING}
#endif
,{D(jjPFAC2),     PFAC_CMD,       LIST_CMD,       BIGINT_CMD, INT_CMD, ALLOW_NC |ALLOW_RING}
,{D(jjPFAC2),     PFAC_CMD,       LIST_CMD,       NUMBER_CMD, INT_CMD, ALLOW_NC |ALLOW_RING}
#ifdef HAVE_PLURAL
,{D(jjPlural_num_poly), NCALGEBRA_CMD,NONE,       POLY_CMD,   POLY_CMD  , NO_NC |NO_RING}
,{D(jjPlural_num_mat),  NCALGEBRA_CMD,NONE,       POLY_CMD,   MATRIX_CMD, NO_NC |NO_RING}
,{D(jjPlural_mat_poly), NCALGEBRA_CMD,NONE,       MATRIX_CMD, POLY_CMD  , NO_NC |NO_RING}
,{D(jjPlural_mat_mat),  NCALGEBRA_CMD,NONE,       MATRIX_CMD, MATRIX_CMD, NO_NC |NO_RING}
,{D(jjPlural_num_poly), NC_ALGEBRA_CMD,RING_CMD,  POLY_CMD,   POLY_CMD  , NO_NC |NO_RING}
,{D(jjPlural_num_mat),  NC_ALGEBRA_CMD,RING_CMD,  POLY_CMD,   MATRIX_CMD, NO_NC |NO_RING}
,{D(jjPlural_mat_poly), NC_ALGEBRA_CMD,RING_CMD,  MATRIX_CMD, POLY_CMD  , NO_NC |NO_RING}
,{D(jjPlural_mat_mat),  NC_ALGEBRA_CMD,RING_CMD,  MATRIX_CMD, MATRIX_CMD, NO_NC |NO_RING}
,{D(jjOPPOSE),    OPPOSE_CMD,     ANY_TYPE/*set by p*/, RING_CMD,   DEF_CMD, ALLOW_PLURAL |NO_RING}
#endif
,{D(jjPARSTR2),   PARSTR_CMD,     STRING_CMD,     RING_CMD,   INT_CMD, ALLOW_NC |ALLOW_RING}
,{D(jjPRINT_FORMAT), PRINT_CMD,   STRING_CMD,     DEF_CMD,    STRING_CMD, ALLOW_NC |ALLOW_RING}
,{D(jjQUOT),      QUOTIENT_CMD,   IDEAL_CMD,      IDEAL_CMD,  IDEAL_CMD, ALLOW_PLURAL |ALLOW_RING}
,{D(jjQUOT),      QUOTIENT_CMD,   MODUL_CMD,      MODUL_CMD,  IDEAL_CMD, ALLOW_PLURAL |ALLOW_RING}
,{D(jjQUOT),      QUOTIENT_CMD,   IDEAL_CMD,      MODUL_CMD,  MODUL_CMD, ALLOW_PLURAL |ALLOW_RING}
,{D(jjRANDOM),    RANDOM_CMD,     INT_CMD,        INT_CMD,    INT_CMD, ALLOW_NC |ALLOW_RING}
,{D(jjRANK2),     RANK_CMD,       INT_CMD,        MATRIX_CMD, INT_CMD, ALLOW_NC |NO_RING}
,{D(jjREAD2),     READ_CMD,       STRING_CMD,     LINK_CMD,   STRING_CMD, ALLOW_NC |ALLOW_RING}
,{D(jjREDUCE_P),  REDUCE_CMD,     POLY_CMD,       POLY_CMD,   IDEAL_CMD, ALLOW_NC |ALLOW_RING}
,{D(jjREDUCE_P),  REDUCE_CMD,     VECTOR_CMD,     VECTOR_CMD, IDEAL_CMD, ALLOW_NC |ALLOW_RING}
,{D(jjREDUCE_P),  REDUCE_CMD,     VECTOR_CMD,     VECTOR_CMD, MODUL_CMD, ALLOW_NC |ALLOW_RING}
,{D(jjREDUCE_ID), REDUCE_CMD,     IDEAL_CMD,      IDEAL_CMD,  IDEAL_CMD, ALLOW_NC |ALLOW_RING}
,{D(jjREDUCE_ID), REDUCE_CMD,     MODUL_CMD,      MODUL_CMD,  IDEAL_CMD, ALLOW_NC |ALLOW_RING}
,{D(jjREDUCE_ID), REDUCE_CMD,     MODUL_CMD,      MODUL_CMD,  MODUL_CMD, ALLOW_NC |ALLOW_RING}
,{D(jjRES),       RES_CMD,        RESOLUTION_CMD, IDEAL_CMD,  INT_CMD, ALLOW_PLURAL |ALLOW_RING}
,{D(jjRES),       RES_CMD,        RESOLUTION_CMD, MODUL_CMD,  INT_CMD, ALLOW_PLURAL |ALLOW_RING}
,{D(jjSBA_1),     SBA_CMD,        IDEAL_CMD,      IDEAL_CMD,  INT_CMD, ALLOW_PLURAL |ALLOW_RING}
,{D(jjSBA_1),     SBA_CMD,        MODUL_CMD,      MODUL_CMD,  INT_CMD, ALLOW_PLURAL |ALLOW_RING}
,{D(jjSIMPL_P),   SIMPLIFY_CMD,   POLY_CMD,       POLY_CMD,   INT_CMD, ALLOW_NC |ALLOW_RING}
,{D(jjSIMPL_P),   SIMPLIFY_CMD,   VECTOR_CMD,     VECTOR_CMD, INT_CMD, ALLOW_NC |ALLOW_RING}
,{D(jjSIMPL_ID),  SIMPLIFY_CMD,   IDEAL_CMD,      IDEAL_CMD,  INT_CMD, ALLOW_NC |ALLOW_RING}
,{D(jjSIMPL_ID),  SIMPLIFY_CMD,   MODUL_CMD,      MODUL_CMD,  INT_CMD, ALLOW_NC |ALLOW_RING}
,{D(jjSQR_FREE2), SQR_FREE_CMD,   IDEAL_CMD,      POLY_CMD,   INT_CMD, NO_NC |ALLOW_RING}
,{D(jjSTATUS2),   STATUS_CMD,     STRING_CMD,     LINK_CMD,   STRING_CMD, ALLOW_NC |ALLOW_RING}
,{D(jjSTATUS2L),  STATUS_CMD,     INT_CMD,        LIST_CMD,   INT_CMD, ALLOW_NC |ALLOW_RING}
//,{D(jjRES),       SRES_CMD,       LIST_CMD,       IDEAL_CMD,  INT_CMD, NO_NC |ALLOW_RING}
//,{D(jjRES),       SRES_CMD,       LIST_CMD,       MODUL_CMD,  INT_CMD, NO_NC |ALLOW_RING}
,{D(jjRES),       SRES_CMD,       RESOLUTION_CMD, IDEAL_CMD,  INT_CMD, NO_NC |ALLOW_RING}
,{D(jjRES),       SRES_CMD,       RESOLUTION_CMD, MODUL_CMD,  INT_CMD, NO_NC |ALLOW_RING}
,{D(jjSTD_1),     STD_CMD,        IDEAL_CMD,      IDEAL_CMD,  POLY_CMD, ALLOW_PLURAL |ALLOW_RING}
,{D(jjSTD_1),     STD_CMD,        MODUL_CMD,      MODUL_CMD,  VECTOR_CMD, ALLOW_PLURAL |ALLOW_RING}
,{D(jjSTD_1),     STD_CMD,        IDEAL_CMD,      IDEAL_CMD,  IDEAL_CMD, ALLOW_PLURAL |ALLOW_RING}
,{D(jjSTD_1),     STD_CMD,        MODUL_CMD,      MODUL_CMD,  MODUL_CMD, ALLOW_PLURAL |ALLOW_RING}
,{D(jjSTD_HILB),  STD_CMD,        IDEAL_CMD,      IDEAL_CMD,  INTVEC_CMD, ALLOW_PLURAL |NO_RING}
,{D(jjSTD_HILB),  STD_CMD,        MODUL_CMD,      MODUL_CMD,  INTVEC_CMD, ALLOW_PLURAL |NO_RING}
,{D(jjSYZ_2),     SYZYGY_CMD,     MODUL_CMD,      IDEAL_CMD,  STRING_CMD, ALLOW_PLURAL |ALLOW_RING}
,{D(jjSYZ_2),     SYZYGY_CMD,     MODUL_CMD,      MODUL_CMD,  STRING_CMD, ALLOW_PLURAL |ALLOW_RING}
,{D(jjTENSOR_Ma), TENSOR_CMD,     MATRIX_CMD,     MATRIX_CMD, MATRIX_CMD, ALLOW_PLURAL |ALLOW_RING}
,{D(jjTENSOR),    TENSOR_CMD,     MODUL_CMD,      MODUL_CMD,  MODUL_CMD, ALLOW_PLURAL |ALLOW_RING|NO_CONVERSION}
,{D(jjTENSOR),    TENSOR_CMD,     SMATRIX_CMD,    SMATRIX_CMD,SMATRIX_CMD, ALLOW_PLURAL |ALLOW_RING|NO_CONVERSION}
,{D(jjVARSTR2),   VARSTR_CMD,     STRING_CMD,     RING_CMD,   INT_CMD, ALLOW_NC |ALLOW_RING}
,{D(jjWAIT1ST2),  WAIT1ST_CMD,    INT_CMD,        LIST_CMD,   INT_CMD, ALLOW_NC |ALLOW_RING}
,{D(jjWAITALL2),  WAITALL_CMD,    INT_CMD,        LIST_CMD,   INT_CMD, ALLOW_NC |ALLOW_RING}
,{D(jjWEDGE),     WEDGE_CMD,      MATRIX_CMD,     MATRIX_CMD, INT_CMD, NO_NC |ALLOW_RING}
,{NULL_VAL,       0,              0,              0,          0, NO_NC |NO_RING}
};
/*=================== operations with 3 args.: table =================*/
const struct sValCmd3 dArith3[]=
{
// operations:
// proc                cmd          res         arg1        arg2        arg3   context
 {D(jjBRACK_S),        '[',        STRING_CMD, STRING_CMD, INT_CMD,    INT_CMD, ALLOW_NC |ALLOW_RING}
,{D(jjBRACK_Im),       '[',        INT_CMD,    INTMAT_CMD, INT_CMD,    INT_CMD, ALLOW_NC |ALLOW_RING}
,{D(jjBRACK_Bim),      '[',        BIGINT_CMD, BIGINTMAT_CMD, INT_CMD, INT_CMD, ALLOW_NC |ALLOW_RING}
,{D(jjBRACK_Ma_I_IV),  '[',        INT_CMD,    INTMAT_CMD, INT_CMD,    INTVEC_CMD, ALLOW_NC |ALLOW_RING}
,{D(jjBRACK_Ma_I_IV),  '[',        BIGINT_CMD, BIGINTMAT_CMD, INT_CMD,    INTVEC_CMD, ALLOW_NC |ALLOW_RING}
,{D(jjBRACK_Ma_IV_I),  '[',        INT_CMD,    INTMAT_CMD, INTVEC_CMD, INT_CMD, ALLOW_NC |ALLOW_RING}
,{D(jjBRACK_Ma_IV_I),  '[',        BIGINT_CMD, BIGINTMAT_CMD, INTVEC_CMD, INT_CMD, ALLOW_NC |ALLOW_RING}
,{D(jjBRACK_Ma_IV_IV), '[',        INT_CMD,    INTMAT_CMD, INTVEC_CMD, INTVEC_CMD, ALLOW_NC |ALLOW_RING}
,{D(jjBRACK_Ma_IV_IV), '[',        BIGINT_CMD, BIGINTMAT_CMD, INTVEC_CMD, INTVEC_CMD, ALLOW_NC |ALLOW_RING}
,{D(jjBRACK_Ma),       '[',        POLY_CMD,   MATRIX_CMD, INT_CMD,    INT_CMD, ALLOW_NC |ALLOW_RING}
,{D(jjBRACK_SM),       '[',        POLY_CMD,   SMATRIX_CMD, INT_CMD,   INT_CMD, ALLOW_NC |ALLOW_RING}
,{D(jjBRACK_Ma_I_IV),  '[',        POLY_CMD,   MATRIX_CMD, INT_CMD,    INTVEC_CMD, ALLOW_NC |ALLOW_RING}
,{D(jjBRACK_Ma_IV_I),  '[',        POLY_CMD,   MATRIX_CMD, INTVEC_CMD, INT_CMD, ALLOW_NC |ALLOW_RING}
,{D(jjBRACK_Ma_IV_IV), '[',        POLY_CMD,   MATRIX_CMD, INTVEC_CMD, INTVEC_CMD, ALLOW_NC |ALLOW_RING}
,{D(jjRING_2),         '[',        RING_CMD,   CRING_CMD,  ANY_TYPE,   ANY_TYPE, ALLOW_NC |ALLOW_RING}
#ifdef SINGULAR_4_2
,{D(jjBRACK_Bim),      '[',        CNUMBER_CMD, CMATRIX_CMD, INT_CMD, INT_CMD, ALLOW_NC |ALLOW_RING}
#endif
,{D(jjPROC3),          '(',        ANY_TYPE,   PROC_CMD,   DEF_CMD,    DEF_CMD, ALLOW_NC |ALLOW_RING}
,{D(atATTRIB3),        ATTRIB_CMD, NONE,       IDHDL,      STRING_CMD, DEF_CMD, ALLOW_NC |ALLOW_RING}
,{D(jjBAREISS3),       BAREISS_CMD,LIST_CMD,   MODUL_CMD,  INT_CMD,    INT_CMD, NO_NC |ALLOW_RING|NO_ZERODIVISOR}
#if defined(HAVE_SHIFTBBA) || defined(HAVE_PLURAL)
,{D(jjBRACKET_REC),    BRACKET_CMD,POLY_CMD,   POLY_CMD,   POLY_CMD,   INT_CMD, ALLOW_NC | NO_RING}
#endif
,{D(jjCOEFFS3_P),      COEFFS_CMD, MATRIX_CMD, POLY_CMD,   POLY_CMD,   MATRIX_CMD, ALLOW_PLURAL |ALLOW_RING}
,{D(jjCOEFFS3_P),      COEFFS_CMD, MATRIX_CMD, VECTOR_CMD, POLY_CMD,   MATRIX_CMD, ALLOW_PLURAL |ALLOW_RING}
,{D(jjCOEFFS3_Id),     COEFFS_CMD, MATRIX_CMD, IDEAL_CMD,  POLY_CMD,   MATRIX_CMD, ALLOW_PLURAL |ALLOW_RING}
,{D(jjCOEFFS3_Id),     COEFFS_CMD, MATRIX_CMD, MODUL_CMD,  POLY_CMD,   MATRIX_CMD, ALLOW_PLURAL |ALLOW_RING}
,{D(jjCOEFFS3_KB),     COEFFS_CMD, MATRIX_CMD, IDEAL_CMD,  IDEAL_CMD,  POLY_CMD, ALLOW_PLURAL |ALLOW_RING}
,{D(jjCOEFFS3_KB),     COEFFS_CMD, MATRIX_CMD, MODUL_CMD,  MODUL_CMD,  POLY_CMD, ALLOW_PLURAL |ALLOW_RING}
#ifdef SINGULAR_4_2
,{D(jjCMATRIX_3),     CMATRIX_CMD, CMATRIX_CMD,INT_CMD,    INT_CMD,   CRING_CMD, ALLOW_PLURAL |ALLOW_RING}
#endif
,{D(jjELIMIN_HILB),    ELIMINATION_CMD,IDEAL_CMD, IDEAL_CMD, POLY_CMD, INTVEC_CMD, NO_NC |ALLOW_RING}
,{D(jjELIMIN_HILB),    ELIMINATION_CMD,MODUL_CMD, MODUL_CMD, POLY_CMD, INTVEC_CMD, NO_NC |ALLOW_RING}
,{D(jjELIMIN_ALG),     ELIMINATION_CMD,IDEAL_CMD, IDEAL_CMD,  POLY_CMD, STRING_CMD, ALLOW_PLURAL |ALLOW_RING}
,{D(jjELIMIN_ALG),     ELIMINATION_CMD,MODUL_CMD, MODUL_CMD,  POLY_CMD, STRING_CMD, ALLOW_PLURAL |ALLOW_RING}
,{D(jjFIND3),          FIND_CMD,   INT_CMD,    STRING_CMD, STRING_CMD, INT_CMD, ALLOW_NC |ALLOW_RING}
,{D(jjFRES3),          FRES_CMD,   RESOLUTION_CMD, IDEAL_CMD, INT_CMD, STRING_CMD, NO_NC |NO_RING}
,{D(jjFRES3),          FRES_CMD,   RESOLUTION_CMD, MODUL_CMD, INT_CMD, STRING_CMD, NO_NC |NO_RING}
,{D(jjFWALK3),         FWALK_CMD,  IDEAL_CMD,  RING_CMD,   DEF_CMD,    INT_CMD, NO_NC |ALLOW_RING}
,{D(jjHILBERT3),       HILBERT_CMD,INTVEC_CMD, IDEAL_CMD,  INT_CMD,    INTVEC_CMD, ALLOW_PLURAL | ALLOW_RING | NO_ZERODIVISOR}
,{D(jjHILBERT3),       HILBERT_CMD,INTVEC_CMD, MODUL_CMD,  INT_CMD,    INTVEC_CMD, ALLOW_PLURAL | ALLOW_RING | NO_ZERODIVISOR}
,{D(jjHOMOG_P_W),      HOMOG_CMD,  POLY_CMD,   POLY_CMD,   POLY_CMD,   INTVEC_CMD, ALLOW_PLURAL |ALLOW_RING}
,{D(jjHOMOG_P_W),      HOMOG_CMD,  VECTOR_CMD, VECTOR_CMD, POLY_CMD,   INTVEC_CMD, ALLOW_PLURAL |ALLOW_RING}
,{D(jjHOMOG_ID_W),     HOMOG_CMD,  IDEAL_CMD,  IDEAL_CMD,  POLY_CMD,   INTVEC_CMD, ALLOW_PLURAL |ALLOW_RING}
,{D(jjHOMOG_ID_W),     HOMOG_CMD,  MODUL_CMD,  MODUL_CMD,  POLY_CMD,   INTVEC_CMD, ALLOW_PLURAL |ALLOW_RING}
,{D(lInsert3),         INSERT_CMD, LIST_CMD,   LIST_CMD,   DEF_CMD,    INT_CMD, ALLOW_NC |ALLOW_RING}
,{D(jjINTERSEC3S),     INTERSECT_CMD,IDEAL_CMD,IDEAL_CMD,  IDEAL_CMD,  STRING_CMD, ALLOW_PLURAL |ALLOW_RING}
,{D(jjINTERSEC3S),     INTERSECT_CMD,MODUL_CMD,MODUL_CMD,  MODUL_CMD,  STRING_CMD, ALLOW_PLURAL |ALLOW_RING}
,{D(jjINTERSECT3),     INTERSECT_CMD,IDEAL_CMD,IDEAL_CMD,  IDEAL_CMD,  IDEAL_CMD, ALLOW_PLURAL |ALLOW_RING}
,{D(jjINTERSECT3),     INTERSECT_CMD,MODUL_CMD,MODUL_CMD,  MODUL_CMD,  MODUL_CMD, ALLOW_PLURAL |ALLOW_RING}
,{D(jjINTMAT3),        INTMAT_CMD, INTMAT_CMD, INTMAT_CMD, INT_CMD,    INT_CMD, ALLOW_NC |ALLOW_RING}
,{D(jjJET_P_IV),       JET_CMD,    POLY_CMD,   POLY_CMD,   INT_CMD,    INTVEC_CMD, ALLOW_PLURAL }
,{D(jjJET_ID_IV),      JET_CMD,    IDEAL_CMD,  IDEAL_CMD,  INT_CMD,    INTVEC_CMD, ALLOW_PLURAL }
,{D(jjJET_P_IV),       JET_CMD,    VECTOR_CMD, VECTOR_CMD, INT_CMD,    INTVEC_CMD, ALLOW_PLURAL }
,{D(jjJET_ID_IV),      JET_CMD,    MODUL_CMD,  MODUL_CMD,  INT_CMD,    INTVEC_CMD, ALLOW_PLURAL }
,{D(jjJET_P_P),        JET_CMD,    POLY_CMD,   POLY_CMD,   POLY_CMD,   INT_CMD, ALLOW_PLURAL }
,{D(jjJET_P_P),        JET_CMD,    VECTOR_CMD, VECTOR_CMD, POLY_CMD,   INT_CMD, ALLOW_PLURAL }
,{D(jjJET_ID_M),       JET_CMD,    IDEAL_CMD,  IDEAL_CMD,  MATRIX_CMD, INT_CMD, ALLOW_PLURAL }
,{D(jjJET_ID_M),       JET_CMD,    MODUL_CMD,  MODUL_CMD,  MATRIX_CMD, INT_CMD, ALLOW_PLURAL }
,{  jjWRONG3 ,         JET_CMD,    POLY_CMD,   POLY_CMD,   INT_CMD,    INT_CMD, ALLOW_PLURAL }
,{D(mpKoszul),         KOSZUL_CMD, MATRIX_CMD, INT_CMD,    INT_CMD,    IDEAL_CMD, NO_NC |NO_RING}
,{D(jjLIFT3),          LIFT_CMD,   MATRIX_CMD, IDEAL_CMD,  IDEAL_CMD,  MATRIX_CMD, ALLOW_NC |ALLOW_RING}
,{D(jjLIFT3),          LIFT_CMD,   MATRIX_CMD, MODUL_CMD,  MODUL_CMD,  MATRIX_CMD, ALLOW_NC |ALLOW_RING}
,{D(jjLIFTSTD3),       LIFTSTD_CMD,IDEAL_CMD,  IDEAL_CMD,  MATRIX_CMD, MODUL_CMD, ALLOW_NC |ALLOW_RING}
,{D(jjLIFTSTD3),       LIFTSTD_CMD,MODUL_CMD,  MODUL_CMD,  MATRIX_CMD, MODUL_CMD, ALLOW_NC |ALLOW_RING}
,{D(jjMATRIX_Id),      MATRIX_CMD, MATRIX_CMD, IDEAL_CMD,  INT_CMD,    INT_CMD, ALLOW_NC |ALLOW_RING}
,{D(jjMATRIX_Mo),      MATRIX_CMD, MATRIX_CMD, MODUL_CMD,  INT_CMD,    INT_CMD, ALLOW_NC |ALLOW_RING}
,{D(jjMATRIX_Ma),      MATRIX_CMD, MATRIX_CMD, MATRIX_CMD, INT_CMD,    INT_CMD, ALLOW_NC |ALLOW_RING}
,{D(jjSMATRIX_Mo),     SMATRIX_CMD,SMATRIX_CMD,MODUL_CMD,  INT_CMD,    INT_CMD, ALLOW_NC |ALLOW_RING}
,{D(jjSMATRIX_Mo),     SMATRIX_CMD,SMATRIX_CMD,SMATRIX_CMD,INT_CMD,    INT_CMD, ALLOW_NC |ALLOW_RING}
#ifdef OLD_RES
,{D(jjRES3),           MRES_CMD,   NONE,       IDEAL_CMD,  INT_CMD,    ANY_TYPE, ALLOW_PLURAL |ALLOW_RING}
,{D(jjRES3),           MRES_CMD,   NONE,       MODUL_CMD,  INT_CMD,    ANY_TYPE, ALLOW_PLURAL |ALLOW_RING}
#endif
,{D(jjNEWSTRUCT3),     NEWSTRUCT_CMD, NONE,     STRING_CMD, STRING_CMD, STRING_CMD, ALLOW_NC |ALLOW_RING}
,{D(jjPREIMAGE),       PREIMAGE_CMD, IDEAL_CMD, RING_CMD,  ANY_TYPE,   ANY_TYPE, ALLOW_PLURAL |ALLOW_RING}
,{D(jjRANDOM_Im),      RANDOM_CMD, INTMAT_CMD, INT_CMD,    INT_CMD,    INT_CMD, ALLOW_NC |ALLOW_RING}
#ifdef SINGULAR_4_2
,{D(jjRANDOM_CF),      RANDOM_CMD, CNUMBER_CMD, CNUMBER_CMD, CNUMBER_CMD, CRING_CMD, ALLOW_NC |ALLOW_RING}
#endif
,{D(jjREDUCE3_P),      REDUCE_CMD, POLY_CMD,   POLY_CMD,   IDEAL_CMD,  INT_CMD, ALLOW_NC |ALLOW_RING}
,{D(jjREDUCE3_P),      REDUCE_CMD, VECTOR_CMD, VECTOR_CMD, IDEAL_CMD,  INT_CMD, ALLOW_NC |ALLOW_RING}
,{D(jjREDUCE3_P),      REDUCE_CMD, VECTOR_CMD, VECTOR_CMD, MODUL_CMD,  INT_CMD, ALLOW_NC |ALLOW_RING}
,{D(jjREDUCE3_ID),     REDUCE_CMD, IDEAL_CMD,  IDEAL_CMD,  IDEAL_CMD,  INT_CMD, ALLOW_NC |ALLOW_RING}
,{D(jjREDUCE3_ID),     REDUCE_CMD, MODUL_CMD,  MODUL_CMD,  MODUL_CMD,  INT_CMD, ALLOW_NC |ALLOW_RING}
,{D(jjREDUCE3_ID),     REDUCE_CMD, MODUL_CMD,  MODUL_CMD,  IDEAL_CMD,  INT_CMD, ALLOW_NC |ALLOW_RING}
,{D(jjREDUCE3_CP),     REDUCE_CMD, POLY_CMD,   POLY_CMD,   POLY_CMD,   IDEAL_CMD, ALLOW_PLURAL |ALLOW_RING}
,{D(jjREDUCE3_CP),     REDUCE_CMD, VECTOR_CMD, VECTOR_CMD, POLY_CMD,   MODUL_CMD, ALLOW_PLURAL |ALLOW_RING}
,{D(jjREDUCE3_CID),    REDUCE_CMD, IDEAL_CMD,  IDEAL_CMD,  IDEAL_CMD,  MATRIX_CMD, ALLOW_PLURAL |ALLOW_RING}
,{D(jjREDUCE3_CID),    REDUCE_CMD, MODUL_CMD,  MODUL_CMD,  MODUL_CMD,  MATRIX_CMD, ALLOW_PLURAL |ALLOW_RING}
#ifdef OLD_RES
,{D(jjRES3),           RES_CMD,    NONE,       IDEAL_CMD,  INT_CMD,    ANY_TYPE, ALLOW_PLURAL |ALLOW_RING}
,{D(jjRES3),           RES_CMD,    NONE,       MODUL_CMD,  INT_CMD,    ANY_TYPE, ALLOW_PLURAL |ALLOW_RING}
#endif
,{D(jjRESULTANT),      RESULTANT_CMD, POLY_CMD,POLY_CMD,   POLY_CMD,   POLY_CMD, NO_NC |ALLOW_RING}
,{D(jjRING3),          RING_CMD,   RING_CMD,   DEF_CMD,    DEF_CMD,    DEF_CMD, ALLOW_PLURAL |ALLOW_RING}
#ifdef OLD_RES
,{D(jjRES3),           SRES_CMD,   NONE,       IDEAL_CMD,  INT_CMD,    ANY_TYPE, NO_NC |ALLOW_RING}
,{D(jjRES3),           SRES_CMD,   NONE,       MODUL_CMD,  INT_CMD,    ANY_TYPE, NO_NC |ALLOW_RING}
#endif
,{D(jjSBA_2),          SBA_CMD,    IDEAL_CMD,  IDEAL_CMD,  INT_CMD, INT_CMD, ALLOW_PLURAL |ALLOW_RING}
,{D(jjSBA_2),          SBA_CMD,    MODUL_CMD,  MODUL_CMD,  INT_CMD, INT_CMD, ALLOW_PLURAL |ALLOW_RING}
,{D(jjSTATUS3),        STATUS_CMD, INT_CMD,    LINK_CMD,   STRING_CMD, STRING_CMD, ALLOW_NC |ALLOW_RING}
,{D(jjSTD_HILB_W),     STD_CMD,    IDEAL_CMD,  IDEAL_CMD,  INTVEC_CMD, INTVEC_CMD, ALLOW_PLURAL |NO_RING}
,{D(jjSTD_HILB_W),     STD_CMD,    MODUL_CMD,  MODUL_CMD,  INTVEC_CMD, INTVEC_CMD, ALLOW_PLURAL |NO_RING}
,{D(jjSUBST_Bu),       SUBST_CMD,  ANY_TYPE,   DEF_CMD,    POLY_CMD,   BUCKET_CMD , ALLOW_PLURAL |ALLOW_RING}
,{D(jjSUBST_P),        SUBST_CMD,  POLY_CMD,   POLY_CMD,   POLY_CMD,   POLY_CMD , ALLOW_PLURAL |ALLOW_RING}
,{D(jjSUBST_P),        SUBST_CMD,  VECTOR_CMD, VECTOR_CMD, POLY_CMD,   POLY_CMD , ALLOW_PLURAL |ALLOW_RING}
,{D(jjSUBST_Id),       SUBST_CMD,  IDEAL_CMD,  IDEAL_CMD,  POLY_CMD,   POLY_CMD , ALLOW_PLURAL |ALLOW_RING}
,{D(jjSUBST_Id),       SUBST_CMD,  MODUL_CMD,  MODUL_CMD,  POLY_CMD,   POLY_CMD , ALLOW_PLURAL |ALLOW_RING}
,{D(jjSUBST_Id),       SUBST_CMD,  MATRIX_CMD, MATRIX_CMD, POLY_CMD,   POLY_CMD , ALLOW_PLURAL |ALLOW_RING}
,{D(jjSUBST_Id_I),     SUBST_CMD,  MATRIX_CMD, MATRIX_CMD, POLY_CMD,   INT_CMD  , ALLOW_PLURAL |ALLOW_RING}
,{D(jjSUBST_Id_N),     SUBST_CMD,  MATRIX_CMD, MATRIX_CMD, POLY_CMD,   NUMBER_CMD , ALLOW_PLURAL |ALLOW_RING}
,{D(nuLagSolve),       LAGSOLVE_CMD,LIST_CMD,  POLY_CMD,   INT_CMD,    INT_CMD  , NO_NC |NO_RING}
,{D(nuVanderSys),      VANDER_CMD, POLY_CMD,   IDEAL_CMD,  IDEAL_CMD,  INT_CMD  , NO_NC |NO_RING}
,{NULL_VAL,            0,          0,          0,          0,          0        , NO_NC |NO_RING}
};
/*=================== operations with many arg.: table =================*/
/* number_of_args:  -1: any, -2: any >0, 1: 1, 2: 2, .. */
const struct sValCmdM dArithM[]=
{
// operations:
// proc            cmd               res        number_of_args   context
 {D(jjKLAMMER_PL),  '(',           ANY_TYPE,           -2      , ALLOW_NC |ALLOW_RING}
,{D(jjRING_PL),   '[',             RING_CMD,           -2      , ALLOW_NC |ALLOW_RING}
,{D(jjBREAK0),    BREAKPOINT_CMD,  NONE,               0       , ALLOW_NC |ALLOW_RING}
,{D(jjBREAK1),    BREAKPOINT_CMD,  NONE,               -2      , ALLOW_NC |ALLOW_RING}
,{D(iiBranchTo),  BRANCHTO_CMD,    NONE,               -2      , ALLOW_NC |ALLOW_RING}
#ifdef SINGULAR_4_2
,{D(jjCALL3ARG),  CMATRIX_CMD,     CMATRIX_CMD,        3       , ALLOW_NC |ALLOW_RING}
,{D(jjCALL2ARG),  CNUMBER_CMD,     CNUMBER_CMD,        2       , ALLOW_NC |ALLOW_RING}
#endif
,{D(jjCALL2ARG),  COEF_CMD,        MATRIX_CMD,         2       , ALLOW_PLURAL |ALLOW_RING}
,{D(jjCOEF_M),    COEF_CMD,        NONE,               4       , ALLOW_PLURAL |ALLOW_RING}
,{D(jjCALL2ARG),  DIVISION_CMD,    ANY_TYPE/*or set by p*/,2   , ALLOW_PLURAL |ALLOW_RING}
,{D(jjDIVISION4), DIVISION_CMD,    ANY_TYPE/*or set by p*/,3   , NO_NC |NO_RING}
,{D(jjDIVISION4), DIVISION_CMD,    ANY_TYPE/*or set by p*/,4   , NO_NC |NO_RING}
,{D(jjDBPRINT),   DBPRINT_CMD,     NONE,               -2      , ALLOW_NC |ALLOW_RING}
//,{D(jjEXPORTTO_M),  EXPORTTO_CMD,    NONE,             -2      , ALLOW_NC |ALLOW_RING}
,{D(jjCALL2ARG),  FETCH_CMD,       ANY_TYPE/*or set by p*/,2   , ALLOW_PLURAL |ALLOW_RING}
,{D(jjFETCH_M),   FETCH_CMD,       ANY_TYPE/*or set by p*/,3   , ALLOW_PLURAL |ALLOW_RING}
,{D(jjFETCH_M),   FETCH_CMD,       ANY_TYPE/*or set by p*/,4   , ALLOW_PLURAL |ALLOW_RING}
,{D(jjCALL1ARG),  IDEAL_CMD,       IDEAL_CMD,          1       , ALLOW_NC |ALLOW_RING}
,{D(jjIDEAL_PL),  IDEAL_CMD,       IDEAL_CMD,          -1      , ALLOW_NC |ALLOW_RING}
,{D(jjCALL2ARG),  INTERSECT_CMD,   IDEAL_CMD,          2       , ALLOW_PLURAL |ALLOW_RING}
,{D(jjCALL3ARG),  INTERSECT_CMD,   IDEAL_CMD,          3       , ALLOW_PLURAL |ALLOW_RING}
,{D(jjINTERSECT_PL),INTERSECT_CMD, IDEAL_CMD,          -2      , ALLOW_PLURAL |ALLOW_RING}
,{D(jjCALL1ARG),  INTVEC_CMD,      INTVEC_CMD,         1       , ALLOW_NC |ALLOW_RING}
,{D(jjINTVEC_PL), INTVEC_CMD,      INTVEC_CMD,         -2      , ALLOW_NC |ALLOW_RING}
,{D(jjCALL2ARG),  JET_CMD,         POLY_CMD,/*or set by p*/ 2  , ALLOW_PLURAL |ALLOW_RING}
,{D(jjCALL3ARG),  JET_CMD,         POLY_CMD,/*or set by p*/ 3  , ALLOW_PLURAL |ALLOW_RING}
,{D(jjJET4),      JET_CMD,         POLY_CMD,/*or set by p*/ 4  , ALLOW_PLURAL |ALLOW_RING}
,{D(jjCALL1ARG),  LIB_CMD,         NONE,                1  , ALLOW_NC |ALLOW_RING}
,{D(jjCALL2ARG),  LIFT_CMD,        MATRIX_CMD,          2  , ALLOW_NC |ALLOW_RING}
,{D(jjCALL3ARG),  LIFT_CMD,        MATRIX_CMD,          3  , ALLOW_NC |ALLOW_RING}
,{D(jjLIFT_4),    LIFT_CMD,        MATRIX_CMD,          4  , ALLOW_PLURAL |ALLOW_RING}
,{D(jjCALL2ARG),  LIFTSTD_CMD,     IDEAL_CMD,/*or MODUL*/2  , ALLOW_NC |ALLOW_RING}
,{D(jjCALL3ARG),  LIFTSTD_CMD,     IDEAL_CMD,/*or MODUL*/3  , ALLOW_NC |ALLOW_RING}
,{D(jjLIFTSTD_4), LIFTSTD_CMD,     IDEAL_CMD,/*or MODUL*/4  , ALLOW_PLURAL |ALLOW_RING}
,{D(jjLIST_PL),   LIST_CMD,        LIST_CMD,           -1      , ALLOW_NC |ALLOW_RING}
,{D(jjLU_INVERSE),LUI_CMD,         LIST_CMD,           -2      , NO_NC |NO_RING}
,{D(jjLU_SOLVE),  LUS_CMD,         LIST_CMD,           -2      , NO_NC |NO_RING}
,{  jjWRONG ,     MINOR_CMD,       NONE,               1       , ALLOW_PLURAL |ALLOW_RING}
,{D(jjMINOR_M),   MINOR_CMD,       IDEAL_CMD,          -2      , ALLOW_PLURAL |ALLOW_RING}
,{D(jjCALL1ARG),  MODUL_CMD,       MODUL_CMD,          1       , ALLOW_NC |ALLOW_RING}
,{D(jjIDEAL_PL),  MODUL_CMD,       MODUL_CMD,          -1      , ALLOW_NC |ALLOW_RING}
,{D(jjCALL1ARG),  NAMES_CMD,       LIST_CMD,            1      , ALLOW_NC |ALLOW_RING}
,{D(jjNAMES0),    NAMES_CMD,       LIST_CMD,            0      , ALLOW_NC |ALLOW_RING}
,{D(jjCALL2ARG),  CNUMBER_CMD,     CNUMBER_CMD,         2      , ALLOW_NC |ALLOW_RING}
,{D(jjOPTION_PL), OPTION_CMD,      STRING_CMD/*or set by p*/,-1, ALLOW_NC |ALLOW_RING}
,{D(jjCALL2ARG),  REDUCE_CMD,      IDEAL_CMD/*or set by p*/,  2, ALLOW_NC |ALLOW_RING}
,{D(jjCALL3ARG),  REDUCE_CMD,      IDEAL_CMD/*or set by p*/,  3, ALLOW_NC |ALLOW_RING}
,{D(jjREDUCE4),   REDUCE_CMD,      IDEAL_CMD/*or set by p*/,  4, ALLOW_PLURAL |ALLOW_RING}
,{D(jjREDUCE5),   REDUCE_CMD,      IDEAL_CMD/*or set by p*/,  5, ALLOW_PLURAL |ALLOW_RING}
,{D(jjCALL1ARG),  RESERVEDNAME_CMD, INT_CMD,            1      , ALLOW_NC |ALLOW_RING}
,{D(jjRESERVED0), RESERVEDNAME_CMD, NONE,               0      , ALLOW_NC |ALLOW_RING}
//,{D(jjCALL1ARG),  RESERVEDNAMELIST_CMD, LIST_CMD,            1      , ALLOW_NC |ALLOW_RING}
,{D(jjRESERVEDLIST0), RESERVEDNAMELIST_CMD, LIST_CMD,               0      , ALLOW_NC |ALLOW_RING}
,{D(jjSTRING_PL), STRING_CMD,      STRING_CMD,         -1      , ALLOW_NC |ALLOW_RING}
,{D(jjCALL3ARG),  SUBST_CMD,       NONE/*set by p*/,   3       , ALLOW_PLURAL |ALLOW_RING}
,{D(jjSUBST_M),   SUBST_CMD,       NONE/*set by p*/,   -2      , ALLOW_PLURAL |ALLOW_RING}
,{D(jjSYSTEM),    SYSTEM_CMD,      NONE/*or set by p*/,-2      , ALLOW_NC |ALLOW_RING}
,{D(jjTEST),      TEST_CMD,        NONE,               -2      , ALLOW_NC |ALLOW_RING}
,{D(iiWRITE),     WRITE_CMD,       NONE,               -2      , ALLOW_NC |ALLOW_RING}
,{D(jjCALL2ARG),  STATUS_CMD,      STRING_CMD,          2      , ALLOW_NC |ALLOW_RING}
,{D(jjCALL3ARG),  STATUS_CMD,      INT_CMD,             3      , ALLOW_NC |ALLOW_RING}
,{D(jjSTATUS_M),  STATUS_CMD,      INT_CMD,             4      , ALLOW_NC |ALLOW_RING}
,{D(loSimplex),   SIMPLEX_CMD,     LIST_CMD,            6      , NO_NC |NO_RING}
,{D(nuUResSolve), URSOLVE_CMD,     LIST_CMD,            4      , NO_NC |NO_RING}
,{D(jjCALL1ARG),  STD_CMD,         IDEAL_CMD/* or set by p*/,1 , ALLOW_NC |ALLOW_RING}
,{D(jjCALL2ARG),  STD_CMD,         IDEAL_CMD,           2      , ALLOW_PLURAL |ALLOW_RING}
,{D(jjCALL3ARG),  STD_CMD,         IDEAL_CMD,           3      , NO_NC |ALLOW_RING}
,{D(jjSTD_HILB_WP), STD_CMD,       IDEAL_CMD,           4      , NO_NC |NO_RING}
,{D(jjQRDS),      QRDS_CMD,        LIST_CMD,            4      , ALLOW_PLURAL |ALLOW_RING}
,{D(jjFactModD_M),FMD_CMD,         LIST_CMD,           -2      , NO_NC |NO_RING}
,{NULL_VAL,       0,               0,                   0      , NO_NC |NO_RING}
};
#ifdef GENTABLE
// this table MUST be order alphabetically by its first entry:
// cannot be declared const because it will be sorted by qsort
VAR cmdnames cmds[] =
// alias: 0: real name, 1: this is an alias, 2: this is an outdated alias
{  // name-string alias tokval          toktype
  { "$INVALID$",   0, -1,                 0},
  { "ASSUME",      0, ASSUME_CMD,         ASSUME_CMD},
  { "LIB",         0, LIB_CMD ,           SYSVAR},
  { "alias",       0, ALIAS_CMD ,         PARAMETER},
  { "align",       0, ALIGN_CMD ,         CMD_2},
  { "and",         0, '&' ,               LOGIC_OP},
  { "apply",       0, APPLY,              APPLY},
  { "attrib",      0, ATTRIB_CMD ,        CMD_123},
  { "bareiss",     0, BAREISS_CMD ,       CMD_13},
  { "betti",       0, BETTI_CMD ,         CMD_12},
  { "bigint",      0, BIGINT_CMD ,        ROOT_DECL},
  { "bigintmat",   0, BIGINTMAT_CMD ,     BIGINTMAT_CMD},
  { "branchTo",    0, BRANCHTO_CMD ,      CMD_M},
  #if defined(HAVE_SHIFTBBA) || defined(HAVE_PLURAL)
  { "bracket",     0, BRACKET_CMD ,       CMD_23},
  #endif
  { "break",       0, BREAK_CMD ,         BREAK_CMD},
  { "breakpoint",  0, BREAKPOINT_CMD ,    CMD_M},
  { "char",        0, CHARACTERISTIC_CMD ,CMD_1},
  { "char_series", 0, CHAR_SERIES_CMD ,   CMD_1},
  { "charstr",     0, CHARSTR_CMD ,       CMD_1},
  { "chinrem",     0, CHINREM_CMD ,       CMD_2},
  { "cleardenom",  0, CONTENT_CMD ,       CMD_1},
  { "close",       0, CLOSE_CMD ,         CMD_1},
#ifdef SINGULAR_4_2
  { "Matrix",     0, CMATRIX_CMD ,       ROOT_DECL_LIST},
  { "Number",     0, CNUMBER_CMD ,       ROOT_DECL_LIST},
  { "parent",     0, PARENT_CMD ,        CMD_1},
  { "Poly",       0, CPOLY_CMD ,         ROOT_DECL_LIST},
#endif
  { "coef",        0, COEF_CMD ,          CMD_M},
  { "coeffs",      0, COEFFS_CMD ,        CMD_23},
  { "continue",    0, CONTINUE_CMD ,      CONTINUE_CMD},
  { "contract",    0, CONTRACT_CMD ,      CMD_2},
  { "convhull",    0, NEWTONPOLY_CMD,     CMD_1},
  { "cring",       0, CRING_CMD,          ROOT_DECL},
  { "dbprint",     0, DBPRINT_CMD ,       CMD_M},
  { "def",         0, DEF_CMD ,           ROOT_DECL},
  { "defined",     0, DEFINED_CMD ,       CMD_1},
  { "deg",         0, DEG_CMD ,           CMD_12},
  { "degree",      0, DEGREE_CMD ,        CMD_1},
  { "delete",      0, DELETE_CMD ,        CMD_2},
  { "denominator", 0, DENOMINATOR_CMD ,   CMD_1},
  { "det",         0, DET_CMD ,           CMD_12},
  { "diff",        0, DIFF_CMD ,          CMD_2},
  { "dim",         0, DIM_CMD ,           CMD_12},
  { "div",         0, INTDIV_CMD ,        MULDIV_OP},
  { "division",    0, DIVISION_CMD ,      CMD_M},
  { "dump",        0, DUMP_CMD,           CMD_1},
  { "extgcd",      0, EXTGCD_CMD ,        CMD_2},
  { "ERROR",       0, ERROR_CMD ,         CMD_1},
  { "eliminate",   0, ELIMINATION_CMD,    CMD_23},
  { "else",        0, ELSE_CMD ,          ELSE_CMD},
  #ifdef HAVE_PLURAL
  { "envelope",    0, ENVELOPE_CMD ,      CMD_1},
  #endif
  { "eval",        0, EVAL ,              EVAL},
  { "example",     0, EXAMPLE_CMD ,       EXAMPLE_CMD},
  { "execute",     0, EXECUTE_CMD ,       CMD_1},
  { "export",      0, EXPORT_CMD ,        EXPORT_CMD},
  { "exportto",    0, EXPORTTO_CMD ,      CMD_2},
  { "facstd",      0, FACSTD_CMD ,        CMD_12},
  { "factmodd",    0, FMD_CMD ,           CMD_M},
  { "factorize",   0, FAC_CMD ,           CMD_12},
  { "farey",       0, FAREY_CMD ,         CMD_2},
  { "fetch",       0, FETCH_CMD ,         CMD_M},
  { "fglm",        0, FGLM_CMD ,          CMD_2},
  { "fglmquot",    0, FGLMQUOT_CMD,       CMD_2},
  { "find",        0, FIND_CMD ,          CMD_23},
  { "finduni",     0, FINDUNI_CMD,        CMD_1},
  { "forif",       0, IF_CMD ,            IF_CMD},
  { "freemodule",  0, FREEMODULE_CMD ,    CMD_1},
  { "fres",        0, FRES_CMD ,          CMD_23},
  { "frwalk",      0, FWALK_CMD ,         CMD_23},
  { "gen",         0, E_CMD ,             CMD_1},
  { "getdump",     0, GETDUMP_CMD,        CMD_1},
  { "gcd",         0, GCD_CMD ,           CMD_2},
  { "GCD",         2, GCD_CMD ,           CMD_2},
  { "hilb",        0, HILBERT_CMD ,       CMD_123},
  { "highcorner",  0, HIGHCORNER_CMD,     CMD_1},
  { "homog",       0, HOMOG_CMD ,         CMD_123},
  { "hres",        0, HRES_CMD ,          CMD_2},
  { "ideal",       0, IDEAL_CMD ,         RING_DECL_LIST},
  { "if",          0, IF_CMD ,            IF_CMD},
  { "imap",        0, IMAP_CMD ,          CMD_2},
  { "impart",      0, IMPART_CMD ,        CMD_1},
  { "importfrom",  0, IMPORTFROM_CMD ,    CMD_2},
  { "indepSet",    0, INDEPSET_CMD ,      CMD_12},
  { "insert",      0, INSERT_CMD ,        CMD_23},
  { "int",         0, INT_CMD ,           ROOT_DECL},
  { "interpolation",0,INTERPOLATE_CMD ,   CMD_2},
  { "interred",    0, INTERRED_CMD ,      CMD_1},
  { "intersect",   0, INTERSECT_CMD ,     CMD_M},
  { "intmat",      0, INTMAT_CMD ,        INTMAT_CMD},
  { "intvec",      0, INTVEC_CMD ,        ROOT_DECL_LIST},
  { "jacob",       0, JACOB_CMD ,         CMD_1},
  { "janet",       0, JANET_CMD ,         CMD_12},
  { "jet",         0, JET_CMD ,           CMD_M},
  { "kbase",       0, KBASE_CMD ,         CMD_12},
  { "keepring",    0, KEEPRING_CMD ,      KEEPRING_CMD},
  { "kernel",      0, KERNEL_CMD ,        CMD_2},
  { "kill",        0, KILL_CMD ,          KILL_CMD},
  { "killattrib",  0, KILLATTR_CMD ,      CMD_12},
  { "koszul",      0, KOSZUL_CMD ,        CMD_23},
  { "kres",        0, KRES_CMD ,          CMD_2},
  { "laguerre",    0, LAGSOLVE_CMD,       CMD_3},
  { "lead",        0, LEAD_CMD ,          CMD_1},
  { "leadcoef",    0, LEADCOEF_CMD ,      CMD_1},
  { "leadexp",     0, LEADEXP_CMD ,       CMD_1},
  { "leadmonom",   0, LEADMONOM_CMD ,     CMD_1},
  { "lift",        0, LIFT_CMD ,          CMD_M},
  { "liftstd",     0, LIFTSTD_CMD ,       CMD_M},
  { "link",        0, LINK_CMD ,          ROOT_DECL},
  { "listvar",     0, LISTVAR_CMD ,       LISTVAR_CMD},
  { "list",        0, LIST_CMD ,          ROOT_DECL_LIST},
  { "load",        0, LOAD_CMD ,          CMD_12},
  { "lres",        0, LRES_CMD ,          CMD_2},
  { "ludecomp",    0, LU_CMD ,            CMD_1},
  { "luinverse",   0, LUI_CMD ,           CMD_M},
  { "lusolve",     0, LUS_CMD ,           CMD_M},
  { "map",         0, MAP_CMD ,           RING_DECL},
  { "matrix",      0, MATRIX_CMD ,        MATRIX_CMD},
  { "maxideal",    0, MAXID_CMD ,         CMD_1},
  { "memory",      0, MEMORY_CMD ,        CMD_1},
  { "minbase",     0, MINBASE_CMD ,       CMD_1},
  { "minor",       0, MINOR_CMD ,         CMD_M},
  { "minres",      0, MINRES_CMD ,        CMD_1},
  { "mod",         0, '%' ,               MULDIV_OP},
  { "module",      0, MODUL_CMD ,         RING_DECL_LIST},
  { "modulo",      0, MODULO_CMD ,        CMD_2},
  { "monitor",     0, MONITOR_CMD ,       CMD_12},
  { "monomial",    0, MONOM_CMD ,         CMD_1},
  { "mpresmat",    0, MPRES_CMD,          CMD_2},
  { "mult",        0, MULTIPLICITY_CMD ,  CMD_1},
  #ifdef OLD_RES
  { "mres",        0, MRES_CMD ,          CMD_23},
  #else
  { "mres",        0, MRES_CMD ,          CMD_2},
  #endif
  { "mstd",        0, MSTD_CMD ,          CMD_1},
  { "nameof",      0, NAMEOF_CMD ,        CMD_1},
  { "names",       0, NAMES_CMD ,         CMD_M},
  { "newstruct",   0, NEWSTRUCT_CMD ,     CMD_23},
  #ifdef HAVE_PLURAL
  { "ncalgebra",   2, NCALGEBRA_CMD ,     CMD_2},
  { "nc_algebra",  0, NC_ALGEBRA_CMD ,    CMD_2},
  #endif
  { "ncols",       0, COLS_CMD ,          CMD_1},
  { "not",         0, NOT ,               NOT},
  { "npars",       0, NPARS_CMD ,         CMD_1},
  #ifdef OLD_RES
  { "nres",        0, RES_CMD ,           CMD_23},
  #else
  { "nres",        0, RES_CMD ,           CMD_2},
  #endif
  { "nrows",       0, ROWS_CMD ,          CMD_1},
  { "number",      0, NUMBER_CMD ,        RING_DECL},
  { "numerator",   0, NUMERATOR_CMD ,     CMD_1},
  { "nvars",       0, NVARS_CMD ,         CMD_1},
  { "open",        0, OPEN_CMD ,          CMD_1},
  #ifdef HAVE_PLURAL
  { "oppose",      0, OPPOSE_CMD ,        CMD_2},
  { "opposite",    0, OPPOSITE_CMD ,      CMD_1},
  #endif
  { "option",      0, OPTION_CMD ,        CMD_M},
  { "or",          0, '|' ,               LOGIC_OP},
  { "ord",         0, ORD_CMD ,           CMD_1},
  { "ordstr",      0, ORDSTR_CMD ,        CMD_1},
  { "package",     0, PACKAGE_CMD ,       ROOT_DECL},
  { "par",         0, PAR_CMD ,           CMD_1},
  { "parameter",   0, PARAMETER ,         PARAMETER},
  { "pardeg",      0, PARDEG_CMD ,        CMD_1},
  { "parstr",      0, PARSTR_CMD ,        CMD_12},
  { "poly",        0, POLY_CMD ,          RING_DECL},
  { "polyBucket",  0, BUCKET_CMD ,        RING_DECL},
  { "preimage",    0, PREIMAGE_CMD ,      CMD_13},
  { "prime",       0, PRIME_CMD ,         CMD_1},
  { "primefactors",0, PFAC_CMD ,          CMD_12},
  { "print",       0, PRINT_CMD ,         CMD_12},
  { "prune",       0, PRUNE_CMD ,         CMD_1},
  { "proc",        0, PROC_CMD ,          PROC_CMD},
  { "qhweight",    0, QHWEIGHT_CMD ,      CMD_1},
  { "qrds",        0, QRDS_CMD ,          CMD_M},
  { "qring",       0, QRING_CMD ,         ROOT_DECL},
  { "quote",       0, QUOTE ,             QUOTE},
  { "quotient",    0, QUOTIENT_CMD ,      CMD_2},
  { "random",      0, RANDOM_CMD ,        CMD_23},
  { "rank",        0, RANK_CMD ,          CMD_12},
  { "read",        0, READ_CMD ,          CMD_12},
  { "reduce",      0, REDUCE_CMD ,        CMD_M},
  { "regularity",  0, REGULARITY_CMD ,    CMD_1},
  { "repart",      0, REPART_CMD ,        CMD_1},
  { "reservedName",0, RESERVEDNAME_CMD ,  CMD_M},
  { "reservedNameList",0, RESERVEDNAMELIST_CMD ,  CMD_M},
  { "resolution",  0, RESOLUTION_CMD ,    RING_DECL},
  { "resultant",   0, RESULTANT_CMD,      CMD_3},
  { "restart",     0, RESTART_CMD,        CMD_1},
  { "return",      0, RETURN ,            RETURN},
  { "RETURN",      0, END_GRAMMAR ,       RETURN},
#if defined(HAVE_SHIFTBBA) || defined(HAVE_PLURAL)
  { "rightstd",    0, RIGHTSTD_CMD ,      CMD_1},
#endif /* HAVE_PLURAL */
  { "ring",        0, RING_CMD ,          RING_CMD},
  { "ringlist",    0, RINGLIST_CMD ,      CMD_1},
  { "ring_list",   0, RING_LIST_CMD ,     CMD_1},
  { "rvar",        0, IS_RINGVAR ,        CMD_1},
  { "sba",         0, SBA_CMD ,           CMD_123},
  { "setring",     0, SETRING_CMD ,       SETRING_CMD},
  { "simplex",     0, SIMPLEX_CMD,        CMD_M},
  { "simplify",    0, SIMPLIFY_CMD ,      CMD_2},
  { "size",        0, COUNT_CMD ,         CMD_1},
  { "slimgb",      0, SLIM_GB_CMD ,       CMD_1},
  { "smatrix",     0, SMATRIX_CMD ,       SMATRIX_CMD},
  { "sortvec",     0, SORTVEC_CMD ,       CMD_1},
  { "sqrfree",     0, SQR_FREE_CMD ,      CMD_12},
#ifdef OLD_RES
  { "sres",        0, SRES_CMD ,          CMD_23},
#else /* OLD_RES */
  { "sres",        0, SRES_CMD ,          CMD_2},
#endif /* OLD_RES */
  { "status",      0, STATUS_CMD,         CMD_M},
  { "std",         0, STD_CMD ,           CMD_M},
  { "string",      0, STRING_CMD ,        ROOT_DECL_LIST},
  { "subst",       0, SUBST_CMD ,         CMD_M},
  { "system",      0, SYSTEM_CMD,         CMD_M},
  { "syz",         0, SYZYGY_CMD ,        CMD_12},
  { "tensor",      0, TENSOR_CMD ,        CMD_2},
  { "test",        0, TEST_CMD ,          CMD_M},
  { "trace",       0, TRACE_CMD ,         CMD_1},
  { "transpose",   0, TRANSPOSE_CMD ,     CMD_1},
#if defined(HAVE_PLURAL) || defined(HAVE_SHIFTBBA)
  { "twostd",      0, TWOSTD_CMD ,        CMD_1},
#endif /* HAVE_PLURAL */
  { "type",        0, TYPE_CMD ,          TYPE_CMD},
  { "typeof",      0, TYPEOF_CMD ,        CMD_1},
  { "univariate",  0, UNIVARIATE_CMD,     CMD_1},
  { "uressolve",   0, URSOLVE_CMD,        CMD_M},
  { "vandermonde", 0, VANDER_CMD,         CMD_3},
  { "var",         0, VAR_CMD ,           CMD_1},
  { "variables",   0, VARIABLES_CMD,      CMD_1},
  { "varstr",      0, VARSTR_CMD ,        CMD_12},
  { "vdim",        0, VDIM_CMD ,          CMD_1},
  { "vector",      0, VECTOR_CMD ,        RING_DECL},
  { "waitall",     0, WAITALL_CMD ,       CMD_12},
  { "waitfirst",   0, WAIT1ST_CMD ,       CMD_12},
  { "wedge",       0, WEDGE_CMD ,         CMD_2},
  { "weight",      0, WEIGHT_CMD ,        CMD_1},
  { "whileif",     0, IF_CMD ,            IF_CMD},
  { "write",       0, WRITE_CMD ,         CMD_M},
/* delete for next version:*/
  { "IN",          1, LEAD_CMD ,          CMD_1},
  { "NF",          1, REDUCE_CMD ,        CMD_M},
  { "multiplicity",1, MULTIPLICITY_CMD ,  CMD_1},
  { "verbose",     2, OPTION_CMD ,        CMD_M},
//  { "rank",        1, ROWS_CMD ,          CMD_1},
//  { "Current",     0, -1 ,                SYSVAR},
//  { "Top",         0, -1 ,                SYSVAR},
//  { "Up",          0, -1 ,                SYSVAR},

/* set sys vars*/
  { "degBound",    0, VMAXDEG ,           SYSVAR},
  { "echo",        0, VECHO ,             SYSVAR},
  { "minpoly",     0, VMINPOLY ,          SYSVAR},
  { "multBound",   0, VMAXMULT ,          SYSVAR},
  { "noether",     0, VNOETHER ,          SYSVAR},
  { "pagewidth",   0, VCOLMAX ,           SYSVAR},
  { "printlevel",  0, VPRINTLEVEL ,       SYSVAR},
  { "short",       0, VSHORTOUT ,         SYSVAR},
  { "timer",       0, VTIMER ,            SYSVAR},
  { "rtimer",      0, VRTIMER,            SYSVAR},
  { "TRACE",       0, TRACE ,             SYSVAR},
  { "voice",       0, VOICE ,             SYSVAR},

/* other reserved words:scanner.l */
  { "pause",       2, -1 ,             0},
  { "while",       0, -1 ,             0},
  { "for",         0, -1 ,             0},
  { "help",        0, -1 ,             0},
  { "newline",     0, -1 ,             0},
  { "exit",        0, -1 ,             0},
  { "quit",        0, -1 ,             0},
/* end of list marker */
  { NULL, 0, 0, 0}
};
#endif /* GENTABLE */
#endif

#ifdef IPCONV
const struct sConvertTypes dConvertTypes[] =
{
//   input type       output type     convert procedure(destr.)/(non-destr.)
//  int -> bigint
   { INT_CMD,         BIGINT_CMD,     D(iiI2BI) , NULL_VAL },
//  int -> number
   { INT_CMD,         NUMBER_CMD,     D(iiI2N) , NULL_VAL },
   { BIGINT_CMD,      NUMBER_CMD,     D(iiBI2N) , NULL_VAL },
//  int -> poly
   { INT_CMD,         POLY_CMD,       D(iiI2P) , NULL_VAL },
   { BIGINT_CMD,      POLY_CMD,       D(iiBI2P) , NULL_VAL },
   { BUCKET_CMD,      POLY_CMD,       NULL_VAL,   D(iiBu2P) },
//  int -> vector
   { INT_CMD,         VECTOR_CMD,     D(iiI2V) , NULL_VAL },
   { BIGINT_CMD,      VECTOR_CMD,     D(iiBI2V) , NULL_VAL },
//  int -> ideal
   { INT_CMD,         IDEAL_CMD,      D(iiI2Id) , NULL_VAL },
   { BIGINT_CMD,      IDEAL_CMD,      D(iiBI2Id) , NULL_VAL },
//  int -> matrix
   { INT_CMD,         MATRIX_CMD,     D(iiI2Id) , NULL_VAL },
   { BIGINT_CMD,      MATRIX_CMD,     D(iiBI2Id) , NULL_VAL },
//  int -> intvec
   { INT_CMD,         INTVEC_CMD,     D(iiI2Iv) , NULL_VAL },
//  intvec -> intmat
   { INTVEC_CMD,      INTMAT_CMD,     D(iiDummy), NULL_VAL },
//  intvec -> matrix
   { INTVEC_CMD,      MATRIX_CMD,     D(iiIm2Ma) , NULL_VAL },
//  intmat -> bigintmat
   { INTMAT_CMD,      BIGINTMAT_CMD,  D(iiIm2Bim) , NULL_VAL },
//  intmat -> matrix
   { INTMAT_CMD,      MATRIX_CMD,     D(iiIm2Ma) , NULL_VAL },
//  number -> poly
   { NUMBER_CMD,      POLY_CMD,       D(iiN2P)  , NULL_VAL },
//  number -> matrix
   { NUMBER_CMD,      MATRIX_CMD,     D(iiN2Ma)  , NULL_VAL },
//  number -> ideal
//  number -> vector
//  number -> module
//  poly -> number
//  poly -> ideal
   { POLY_CMD,        IDEAL_CMD,      D(iiP2Id) , NULL_VAL },
//  poly -> vector
   { POLY_CMD,        VECTOR_CMD,     D(iiP2V) , NULL_VAL },
//  poly -> matrix
   { POLY_CMD,        MATRIX_CMD,     D(iiP2Id) , NULL_VAL },
// the same for polyBucket:
   { BUCKET_CMD,      IDEAL_CMD,      D(iiBu2Id) , NULL_VAL },
   { BUCKET_CMD,      VECTOR_CMD,     D(iiBu2V) , NULL_VAL },
   { BUCKET_CMD,      MATRIX_CMD,     D(iiBu2Id) , NULL_VAL },
//  vector -> module
   { VECTOR_CMD,      MODUL_CMD,      D(iiP2Id) , NULL_VAL },
//  vector -> matrix
   { VECTOR_CMD,      MATRIX_CMD,     D(iiV2Ma) , NULL_VAL },
//  ideal -> module
   { IDEAL_CMD,       MODUL_CMD,      D(iiMa2Mo) , NULL_VAL },
//  ideal -> matrix
   { IDEAL_CMD,       MATRIX_CMD,     D(iiDummy) , NULL_VAL },
//  module -> matrix
   { MODUL_CMD,       MATRIX_CMD,     D(iiMo2Ma) , NULL_VAL },
//  matrix -> ideal
//  matrix -> module
   { MATRIX_CMD,      MODUL_CMD,      D(iiMa2Mo) , NULL_VAL },
//  intvec
//  string -> link
   { STRING_CMD,      LINK_CMD,       D(iiS2Link) , NULL_VAL },
// resolution -> list
   { RESOLUTION_CMD,  LIST_CMD,       NULL_VAL , D(iiR2L_l) },
// list -> resolution
   { LIST_CMD,        RESOLUTION_CMD, NULL_VAL ,  D(iiL2R) },
// matrix -> smatrix
   { MATRIX_CMD,      SMATRIX_CMD,    D(iiMa2Mo), NULL_VAL },
// module -> smatrix
   { MODUL_CMD,       SMATRIX_CMD,    D(iiDummy), NULL_VAL },
// smatrix -> matrix
   { SMATRIX_CMD,     MATRIX_CMD,     D(iiMo2Ma) , NULL_VAL },
// smatrix -> module
   { SMATRIX_CMD,     MODUL_CMD,      D(iiDummy) , NULL_VAL },
#ifdef SINGULAR_4_2
   { INT_CMD,         CNUMBER_CMD,    D(iiI2NN) , NULL_VAL },
   { BIGINT_CMD,      CNUMBER_CMD,    D(iiBI2NN) , NULL_VAL },
   { CNUMBER_CMD,     NUMBER_CMD,     D(iiNN2N) , NULL_VAL },
   { CNUMBER_CMD,     POLY_CMD,       D(iiNN2P) , NULL_VAL },
   { INT_CMD,         CPOLY_CMD,      D(iiI2CP), NULL_VAL },
   { BIGINT_CMD,      CPOLY_CMD,      D(iiBI2CP), NULL_VAL },
//   { NUMBER_CMD,      CPOLY_CMD,      D(iiN2CP), NULL_VAL },
   { POLY_CMD,        CPOLY_CMD,      D(iiP2CP), NULL_VAL },
#endif
//  end of list
   { 0,               0,              NULL_VAL , NULL_VAL }
};
#else
extern const struct sConvertTypes dConvertTypes[];
#endif
#ifdef IPASSIGN
const struct sValAssign dAssign[]=
{
// same res types must be grouped together
// proc         res             arg
 {D(jiA_BIGINT),   BIGINT_CMD,     BIGINT_CMD }
,{D(jiA_BIGINTMAT),BIGINTMAT_CMD,  BIGINTMAT_CMD}
,{D(jiA_CRING),    CRING_CMD,      CRING_CMD }
,{D(jiA_IDEAL),    IDEAL_CMD,      IDEAL_CMD }
,{D(jiA_IDEAL_Mo), IDEAL_CMD,      MODUL_CMD }
,{D(jiA_IDEAL_M),  IDEAL_CMD,      MATRIX_CMD }
,{D(jiA_INT),      INT_CMD,        INT_CMD }
,{D(jiA_1x1INTMAT), INT_CMD,       INTMAT_CMD }
,{D(jiA_INTVEC),   INTVEC_CMD,     INTVEC_CMD }
,{D(jiA_INTVEC),   INTMAT_CMD,     INTMAT_CMD }
,{D(jiA_LINK),     LINK_CMD,       STRING_CMD }
,{D(jiA_LINK),     LINK_CMD,       LINK_CMD }
,{D(jiA_LIST_RES), LIST_CMD,       RESOLUTION_CMD }
,{D(jiA_LIST),     LIST_CMD,       LIST_CMD }
,{D(jiA_MAP_ID),   MAP_CMD,        IDEAL_CMD }
,{D(jiA_MAP),      MAP_CMD,        MAP_CMD }
,{D(jiA_IDEAL),    MATRIX_CMD,     MATRIX_CMD }
,{D(jiA_IDEAL),    MODUL_CMD,      MODUL_CMD }
,{D(jiA_MODUL_P),  MODUL_CMD,      POLY_CMD }
,{D(jiA_NUMBER),   NUMBER_CMD,     NUMBER_CMD }
,{D(jiA_PACKAGE),  PACKAGE_CMD,    PACKAGE_CMD }
,{D(jiA_POLY),     POLY_CMD,       POLY_CMD }
,{D(jiA_1x1MATRIX),POLY_CMD,       MATRIX_CMD }
,{D(jiA_BUCKET),   POLY_CMD,       BUCKET_CMD}
,{D(jiA_PROC),     PROC_CMD,       STRING_CMD }
,{D(jiA_PROC),     PROC_CMD,       PROC_CMD }
,{D(jiA_RESOLUTION),RESOLUTION_CMD,RESOLUTION_CMD }
,{D(jiA_RING),     RING_CMD,       RING_CMD }
,{D(jiA_QRING),    RING_CMD,       IDEAL_CMD }
,{D(jiA_IDEAL),    SMATRIX_CMD,    SMATRIX_CMD }
,{D(jiA_IDEAL),    SMATRIX_CMD,    MODUL_CMD }
,{D(jiA_STRING),   STRING_CMD,     STRING_CMD }
,{D(jiA_POLY),     VECTOR_CMD,     VECTOR_CMD }
#ifdef SINGULAR_4_2
,{D(jiA_BIGINTMAT),CMATRIX_CMD,    CMATRIX_CMD}
,{D(jiA_NUMBER2),  CNUMBER_CMD,    CNUMBER_CMD }
,{D(jiA_NUMBER2_I),CNUMBER_CMD,    INT_CMD }
,{D(jiA_NUMBER2_N),CNUMBER_CMD,    NUMBER_CMD }
,{D(jiA_POLY2),    CPOLY_CMD,      CPOLY_CMD }
//,{D(jiA_POLY2_P),  CPOLY_CMD,      POLY_CMD }
,{D(jiA_CRING),    CRING_CMD,      CRING_CMD }
#endif
,{D(jiA_DEF),      DEF_CMD,        DEF_CMD }
,{NULL_VAL,        0,              0 }
};
const struct sValAssign_sys dAssign_sys[]=
{
// sysvars:
 {D(jjECHO),       VECHO,          INT_CMD }
,{D(jjPRINTLEVEL), VPRINTLEVEL,    INT_CMD }
,{D(jjCOLMAX),     VCOLMAX,        INT_CMD }
,{D(jjTIMER),      VTIMER,         INT_CMD }
#ifdef HAVE_GETTIMEOFDAY
,{D(jjRTIMER),     VRTIMER,        INT_CMD }
#endif
,{D(jjMAXDEG),     VMAXDEG,        INT_CMD }
,{D(jjMAXMULT),    VMAXMULT,       INT_CMD }
,{D(jjTRACE),      TRACE,          INT_CMD }
,{D(jjSHORTOUT),   VSHORTOUT,      INT_CMD }
,{D(jjMINPOLY),    VMINPOLY,       NUMBER_CMD }
,{D(jjNOETHER),    VNOETHER,       POLY_CMD }
,{NULL_VAL,        0,              0 }
};
#endif
