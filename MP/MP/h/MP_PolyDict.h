/***************************************************************************
 *
 *   HEADER FILE:  MP_PolyDict.h
 *
 *      The Polytype dictionary (definitions and declarations)
 *
 *
 *   Change Log:
 *
 *
 ***************************************************************************/
#ifndef _MP_PolyDict_h
#define _MP_PolyDict_h

/* Lets give the Poly Dictionary the number 10 */

#define MP_PolyDict 10

/***************************************************************************
 *
 * Annotations
 *
 ***************************************************************************/
#define MP_AnnotPolyModuleVector        1
#define MP_AnnotPolyVarNames            2
#define MP_AnnotPolyVarNumber           3
#define MP_AnnotPolyOrdering            4
#define MP_AnnotPolyDefRel              5
#define MP_AnnotPolyIrreducible         6
#define MP_AnnotPolyWeights             8
#define MP_AnnotPolyModuleRank          9
#define MP_AnnotShouldHavePolyOrdering  10

/***************************************************************************
 *
 * (Common) Constants
 *
 ***************************************************************************/
#define MP_CcPolyOrdering_Unknown       0
#define MP_CcPolyOrdering_Vector        1 /* a */
#define MP_CcPolyOrdering_Matrix        2 /* M */
#define MP_CcPolyOrdering_IncComp       3 /* c */
#define MP_CcPolyOrdering_DecComp       4 /* C */
#define MP_CcPolyOrdering_Lex           5 /* lp */
#define MP_CcPolyOrdering_DegRevLex     6 /* dp */
#define MP_CcPolyOrdering_DegLex        7 /* Dp */
#define MP_CcPolyOrdering_NegLex        8 /* ls */
#define MP_CcPolyOrdering_NegDegRevLex  9 /* ds */
#define MP_CcPolyOrdering_NegDegLex     10 /* Ds */
#define MP_CcPolyOrdering_NegRevLex     11
#define MP_CcPolyOrdering_RevLex        12

#if 0 /* Here is what it used to be */
#define MP_CcPolyOrdering_No    0
#define MP_CcPolyOrdering_a     1
#define MP_CcPolyOrdering_c     2
#define MP_CcPolyOrdering_C     3
#define MP_CcPolyOrdering_M     4
#define MP_CcPolyOrdering_lp    5
#define MP_CcPolyOrdering_Dp    6
#define MP_CcPolyOrdering_wp    7
#define MP_CcPolyOrdering_Wp    8
#define MP_CcPolyOrdering_ls    9
#define MP_CcPolyOrdering_ds    10
#define MP_CcPolyOrdering_Ds    11
#define MP_CcPolyOrdering_ws    12
#define MP_CcPolyOrdering_Ws    13
#define MP_CcPolyOrdering_Unspec 14
#endif


/***************************************************************************
 *
 * Common Operator Definitions
 *
 ***************************************************************************/

#define MP_CopPolyBetti                 1
#define MP_CopPolyChar                  2
#define MP_CopPolyCharSeries            3
#define MP_CopPolyDeg                   4
#define MP_CopPolyDim                   5
#define MP_CopPolyFactorize             6
#define MP_CopPolyHilb                  7
#define MP_CopPolyHomog                 8
#define MP_CopPolyInDepSet              9
#define MP_CopPolyIdeal                 10
#define MP_CopPolyKbase                 11
#define MP_CopPolyLead                  12
#define MP_CopPolyLeadCoef              13
#define MP_CopPolyLeadExp               14
#define MP_CopPolyMaxIdeal              15
#define MP_CopPolyMinBase               16
#define MP_CopPolyMinRes                17
#define MP_CopPolyModule                18
#define MP_CopPolyMultiplicity          19
#define MP_CopPolyOrder                 20
#define MP_CopPolyPrune                 21
#define MP_CopPolyQHWeight              22
#define MP_CopPolyRegularity            23
#define MP_CopPolyStd                   24
#define MP_CopPolySyz                   25
#define MP_CopPolyVdim                  26
#define MP_CopPolyCoeffs                27
#define MP_CopPolyContract              28
#define MP_CopPolyEliminate             29
#define MP_CopPolyJet                   30
#define MP_CopPolyLift                  31
#define MP_CopPolyLiftstd               32
#define MP_CopPolyModulo                33
#define MP_CopPolyMres                  34
#define MP_CopPolyQuotient              35
#define MP_CopPolyReduce                36
#define MP_CopPolyPreimage              37
#define MP_CopPolyRes                   38
#define MP_CopPolyRing                  39
#define MP_CopPolyDenseDistPoly         40
#define MP_CopPolyDenseDistPolyFrac     41
#define MP_CopPolyClearDenom            42
#define MP_CopPolyFacStd                43
#define MP_CopPolyResultant             44
#define MP_CopPolyFreeModule            45

/***************************************************************************
 *
 * Common Metat Types
 *
 ***************************************************************************/

#define MP_CmtPolyExpTreePoly       1

#endif /* _MP_PolyDict_h */
