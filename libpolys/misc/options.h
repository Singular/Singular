#ifndef OPTIONS_H
#define OPTIONS_H
/*****************************************
 * *  Computer Algebra System SINGULAR      *
 * *****************************************/
/*
 * ABSTRACT: macros for global options
 */
#include "misc/auxiliary.h" /*for BOOLEAN*/
/*the general set of std-options : si_opt_1(test) */
/*the general set of verbose-options : si_opt_2(verbose) */
#ifdef __cplusplus
extern "C" VAR unsigned si_opt_1; //< NOTE: Original option variable name: test
extern "C" VAR unsigned si_opt_2; //< NOTE: Original option variable name: verbose
extern "C" VAR BOOLEAN siCntrlc;
#else
EXTERN_VAR unsigned si_opt_1;
EXTERN_VAR unsigned si_opt_2;
EXTERN_VAR BOOLEAN siCntrlc;
#endif
#define SI_SAVE_OPT(A,B) { A=si_opt_1; B=si_opt_2; }
#define SI_SAVE_OPT1(A) { A=si_opt_1; }
#define SI_SAVE_OPT2(A) { A=si_opt_2; }
#define SI_RESTORE_OPT(A,B) { si_opt_1=A; si_opt_2=B; }
#define SI_RESTORE_OPT1(A) { si_opt_1=A; }
#define SI_RESTORE_OPT2(A) { si_opt_2=A; }

/*
**  Set operations (small sets only)
*/

#define Sy_bit(x)     ((unsigned)1<<(x))
#define Sy_inset(x,s) ((Sy_bit(x)&(s))?TRUE:FALSE)
#define BTEST1(a)     Sy_inset((a), si_opt_1)
#define BVERBOSE(a)   Sy_inset((a), si_opt_2)

/*
** defines for BITSETs
*/

#define V_QUIET       0
#define V_QRING       1
#define V_SHOW_MEM    2
#define V_YACC        3
#define V_REDEFINE    4
#define V_READING     5
#define V_LOAD_LIB    6
#define V_DEBUG_LIB   7
#define V_LOAD_PROC   8
#define V_DEF_RES     9

#define V_SHOW_USE   11
#define V_IMAP       12
#define V_PROMPT     13
#define V_NSB        14
#define V_CONTENTSB  15
#define V_CANCELUNIT 16
#define V_MODPSOLVSB 17
#define V_UPTORADICAL 18
#define V_FINDMONOM  19
#define V_COEFSTRAT  20
#define V_IDLIFT     21
#define V_LENGTH     22
//#define V_REPORT_STD 23
/*23: kDebugPrint */
#define V_ALLWARN    24
#define V_INTERSECT_ELIM 25
#define V_INTERSECT_SYZ 26
#define V_ASSIGN_NONE 27
/* for tests: 28-30 */
#define V_DEG_STOP   31


#define OPT_PROT           0
#define OPT_REDSB          1
#define OPT_NOT_BUCKETS    2
#define OPT_NOT_SUGAR      3
#define OPT_INTERRUPT      4
#define OPT_SUGARCRIT      5
#define OPT_DEBUG          6
#define OPT_REDTHROUGH     7
#define OPT_NO_SYZ_MINIM   8
#define OPT_RETURN_SB      9
#define OPT_FASTHC        10
#define OPT_OLDSTD        20

#define OPT_STAIRCASEBOUND 22
#define OPT_MULTBOUND     23
#define OPT_DEGBOUND      24
#define OPT_REDTAIL       25
#define OPT_INTSTRATEGY   26
#define OPT_FINDET        27
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
#define TEST_OPT_FINDET            BTEST1(OPT_FINDET)
#define TEST_OPT_RETURN_SB         BTEST1(OPT_RETURN_SB)
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
#define TEST_OPT_NO_SYZ_MINIM      BTEST1(OPT_NO_SYZ_MINIM)


#define TEST_OPT_CONTENTSB         BVERBOSE(V_CONTENTSB)
#define TEST_OPT_CANCELUNIT        BVERBOSE(V_CANCELUNIT)
#define TEST_OPT_IDLIFT            BVERBOSE(V_IDLIFT)
#define TEST_OPT_LENGTH            BVERBOSE(V_LENGTH)
#define TEST_V_QRING               BVERBOSE(V_QRING)
#define TEST_V_NSB                 BVERBOSE(V_NSB)
#define TEST_V_QUIET               BVERBOSE(V_QUIET)
#define TEST_V_ASSIGN_NONE         BVERBOSE(V_ASSIGN_NONE)

#define TEST_VERB_NSB              BVERBOSE(V_NSB)
#define TEST_V_DEG_STOP            BVERBOSE(V_DEG_STOP)
#define TEST_V_MODPSOLVSB          BVERBOSE(V_MODPSOLVSB)
#define TEST_V_COEFSTRAT           BVERBOSE(V_COEFSTRAT)
#define TEST_V_UPTORADICAL         BVERBOSE(V_UPTORADICAL)
#define TEST_V_FINDMONOM           BVERBOSE(V_FINDMONOM)
#define TEST_V_ALLWARN             BVERBOSE(V_ALLWARN)
#define TEST_V_INTERSECT_ELIM      BVERBOSE(V_INTERSECT_ELIM)
#define TEST_V_INTERSECT_SYZ       BVERBOSE(V_INTERSECT_SYZ)


#endif
