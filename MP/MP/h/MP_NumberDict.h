/***************************************************************************
 *
 *   HEADER FILE:  MP_NumberDict.h
 *
 *      The Numbertype dictionary (definitions and declarations)
 *
 *
 *   Change Log:
 *
 *
 ***************************************************************************/
#ifndef _MP_NumberDict_h
#define _MP_NumberDict_h

/* Lets give the Number Dictionary the number 12 */

#define MP_NumberDict 12

/***************************************************************************
 *
 * Meta type definitions
 *
 ***************************************************************************/
#define MP_CmtNumberRational            1
#define MP_CmtNumberInteger             2
#define MP_CmtNumberComplex             3
#define MP_CmtNumberReal                4
#define MP_CmtNumberCharp               5
#define MP_CmtNumberModulo              6


/***************************************************************************
 *
 * Annot definitions
 *
 ***************************************************************************/
#define MP_AnnotNumberIsNormalized          1
#define MP_AnnotNumberIsPrime               2
#define MP_AnnotNumberModulos               3


/***************************************************************************
 *
 * Common Operator Definitions
 *
 ***************************************************************************/
#define MP_CopNumberPrime               1
#define MP_CopNumberExtGcd              2
#define MP_CopNumberGcd                 3
#define MP_CopNumberRandom              4


#endif /* _MP_NumberDict_h */
