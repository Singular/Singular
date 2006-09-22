/***************************************************************************
 *
 *   HEADER FILE:  MP_BasicDict.h
 *
 *      The MP basic dictionary (definitions and declarations)
 *
 *
 *   Change Log:
 *      1/97 obachman  -- this is a first try
 *
 ***************************************************************************/
#ifndef _MP_BasicDict_h
#define _MP_BasicDict_h



/***************************************************************************
 *
 * Common Operator Definitions
 *
 ***************************************************************************/
/* for the time being, no attention was paid to "good" numeric value
   assignments */
#define MP_CopBasicInc              1
#define MP_CopBasicDec              2
#define MP_CopBasicSize             3
#define MP_CopBasicList             4
#define MP_CopBasicAdd              5
#define MP_CopBasicMinus            6
#define MP_CopBasicMult             7
#define MP_CopBasicDiv              8
#define MP_CopBasicMod              9
#define MP_CopBasicPow              10
#define MP_CopBasicGreaterEqual     11
#define MP_CopBasicGreater          12
#define MP_CopBasicLessEqual        13
#define MP_CopBasicLess             14
#define MP_CopBasicAnd              15
#define MP_CopBasicOr               16
#define MP_CopBasicEqual            17
#define MP_CopBasicNotEqual         18
#define MP_CopBasicRange            19
#define MP_CopBasicIndex            20
#define MP_CopBasicDiff             21
#define MP_CopBasicInterSect        22
#define MP_CopBasicSubst            23
#define MP_CopBasicNot              24
#define MP_CopBasicAssign           25
#define MP_CopBasicComplex          26
#define MP_CopBasicPackage          27
#endif /* _MP_BasicDict_h */
