/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/***************************************************************
 *  File:    p_MemCmp.h
 *  Purpose: macros for memory comparisons
 *  Author:  obachman (Olaf Bachmann)
 *  Created: 8/00
 *  Version: $Id: p_MemCmp.h,v 1.1.1.1 2003-10-06 12:15:59 Singular Exp $
 *******************************************************************/
#ifndef P_MEM_CMP_H
#define P_MEM_CMP_H

/***************************************************************
 *
 *  auxiallary macros
 *
 *******************************************************************/
#define _p_MemCmp_Declare(s1, s2)                   \
  const unsigned long* _s1 = ((unsigned long*) s1); \
  const unsigned long* _s2 = ((unsigned long*) s2); \
  register unsigned long _v1;                       \
  register unsigned long _v2

#define _p_MemCmp_Equal(i, s1, s2, actionE) \
  do {  _v1=((unsigned long*)s1)[i]; _v2=((unsigned long*)s2)[i]; if (_v1==_v2) actionE; }while (0)
#define _p_MemCmp_NotEqual(actionG, actionS)   \
  do {if (_v1 > _v2) actionG; actionS;} while (0)

#define _p_MemCmp_LengthOne_Equal(i, s1, s2, actionE) \
  _p_MemCmp_Equal(i, s1, s2, actionE)
#define _p_MemCmp_LengthTwo_Equal(i, s1, s2, actionE) \
  _p_MemCmp_Equal(i, s1, s2, _p_MemCmp_Equal(i+1, s1, s2, actionE))
#define _p_MemCmp_LengthThree_Equal(i, s1, s2, actionE) \
  _p_MemCmp_LengthTwo_Equal(i, s1, s2, _p_MemCmp_Equal(i+2, s1, s2, actionE))
#define _p_MemCmp_LengthFour_Equal(i, s1, s2, actionE) \
  _p_MemCmp_LengthThree_Equal(i, s1, s2, _p_MemCmp_Equal(i+3, s1, s2, actionE))
#define _p_MemCmp_LengthFive_Equal(i, s1, s2, actionE) \
  _p_MemCmp_LengthFour_Equal(i, s1, s2, _p_MemCmp_Equal(i+4, s1, s2, actionE))
#define _p_MemCmp_LengthSix_Equal(i, s1, s2, actionE) \
  _p_MemCmp_LengthFive_Equal(i, s1, s2, _p_MemCmp_Equal(i+5, s1, s2, actionE))
#define _p_MemCmp_LengthSeven_Equal(i, s1, s2, actionE) \
  _p_MemCmp_LengthSix_Equal(i, s1, s2, _p_MemCmp_Equal(i+6, s1, s2, actionE))
#define _p_MemCmp_LengthEight_Equal(i, s1, s2, actionE) \
  _p_MemCmp_LengthSeven_Equal(i, s1, s2, _p_MemCmp_Equal(i+7, s1, s2, actionE))
#define _p_MemCmp_LengthGeneral_Equal(i, s1, s2, length, actionE)   \
do                                                                  \
{                                                                   \
                                                                    \
  register unsigned long _i = (unsigned long) i;                    \
  const unsigned long _l =                                          \
         (unsigned long) length + (unsigned long)i ;                \
                                                                    \
  LengthGeneral_LoopTop:                                            \
    _v1 = s1[_i];                                                   \
    _v2 = s2[_i];                                                   \
    if (_v1 == _v2)                                                 \
    {                                                               \
      _i++;                                                         \
      if (_i == _l) actionE;                                        \
      else  goto LengthGeneral_LoopTop;                             \
    }                                                               \
}                                                                   \
while (0)

/***************************************************************
 *
 *  Pomog
 *
 *******************************************************************/
#define p_MemCmp_LengthOne_OrdPomog(s1, s2, length, ordsgn, actionE, actionG, actionS) \
do                                                                          \
{                                                                           \
  _p_MemCmp_Declare(s1,s2);                                                 \
  _p_MemCmp_LengthOne_Equal(0, _s1, _s2, actionE);                          \
  _p_MemCmp_NotEqual(actionG, actionS);                                     \
}                                                                           \
while (0)
#define p_MemCmp_LengthTwo_OrdPomog(s1, s2, length, ordsgn, actionE, actionG, actionS) \
do                                                                          \
{                                                                           \
  _p_MemCmp_Declare(s1,s2);                                                 \
  _p_MemCmp_LengthTwo_Equal(0, _s1, _s2, actionE);                 \
  _p_MemCmp_NotEqual(actionG, actionS);                                      \
}                                                                           \
while (0)
#define p_MemCmp_LengthThree_OrdPomog(s1, s2, length, ordsgn, actionE, actionG, actionS)   \
do                                                                              \
{                                                                               \
  _p_MemCmp_Declare(s1,s2);                                                     \
  _p_MemCmp_LengthThree_Equal(0, _s1, _s2, actionE);                   \
  _p_MemCmp_NotEqual(actionG, actionS);                                      \
}                                                                               \
while (0)
#define p_MemCmp_LengthFour_OrdPomog(s1, s2, length, ordsgn, actionE, actionG, actionS)    \
do                                                                              \
{                                                                               \
  _p_MemCmp_Declare(s1,s2);                                                     \
  _p_MemCmp_LengthFour_Equal(0, _s1, _s2, actionE);                    \
  _p_MemCmp_NotEqual(actionG, actionS);                                      \
}                                                                               \
while (0)
#define p_MemCmp_LengthFive_OrdPomog(s1, s2, length, ordsgn, actionE, actionG, actionS)    \
do                                                                              \
{                                                                               \
  _p_MemCmp_Declare(s1,s2);                                                     \
  _p_MemCmp_LengthFive_Equal(0, _s1, _s2, actionE);                    \
  _p_MemCmp_NotEqual(actionG, actionS);                                      \
}                                                                               \
while (0)
#define p_MemCmp_LengthSix_OrdPomog(s1, s2, length, ordsgn, actionE, actionG, actionS) \
do                                                                          \
{                                                                           \
  _p_MemCmp_Declare(s1,s2);                                                 \
  _p_MemCmp_LengthSix_Equal(0, _s1, _s2, actionE);                 \
  _p_MemCmp_NotEqual(actionG, actionS);                                      \
}                                                                           \
while (0)
#define p_MemCmp_LengthSeven_OrdPomog(s1, s2, length, ordsgn, actionE, actionG, actionS)   \
do                                                                              \
{                                                                               \
  _p_MemCmp_Declare(s1,s2);                                                     \
  _p_MemCmp_LengthSeven_Equal(0, _s1, _s2, actionE);                   \
  _p_MemCmp_NotEqual(actionG, actionS);;                                      \
}                                                                               \
while (0)

#define p_MemCmp_LengthEight_OrdPomog(s1, s2, length, ordsgn, actionE, actionG, actionS)   \
do                                                                              \
{                                                                               \
  _p_MemCmp_Declare(s1,s2);                                                     \
  _p_MemCmp_LengthEight_Equal(0, _s1, _s2, actionE);                   \
  _p_MemCmp_NotEqual(actionG, actionS);                                      \
}                                                                               \
while (0)
#define p_MemCmp_LengthGeneral_OrdPomog(s1, s2, length, ordsgn, actionE, actionG, actionS) \
do                                                                              \
{                                                                               \
  _p_MemCmp_Declare(s1,s2);                                                     \
  _p_MemCmp_LengthGeneral_Equal(0, _s1, _s2, length, actionE);      \
  _p_MemCmp_NotEqual(actionG, actionS);                                          \
}                                                                               \
while (0)

/***************************************************************
 *
 *  Nomog
 *
 *******************************************************************/
#define p_MemCmp_LengthOne_OrdNomog(s1, s2, length, ordsgn, actionE, actionG, actionS)     \
  p_MemCmp_LengthOne_OrdPomog(s1, s2, length, ordsgn, actionE, actionS, actionG)
#define p_MemCmp_LengthTwo_OrdNomog(s1, s2, length, ordsgn, actionE, actionG, actionS)     \
  p_MemCmp_LengthTwo_OrdPomog(s1, s2, length, ordsgn, actionE, actionS, actionG)
#define p_MemCmp_LengthThree_OrdNomog(s1, s2, length, ordsgn, actionE, actionG, actionS)   \
  p_MemCmp_LengthThree_OrdPomog(s1, s2, length, ordsgn, actionE, actionS, actionG)
#define p_MemCmp_LengthFour_OrdNomog(s1, s2, length, ordsgn, actionE, actionG, actionS)    \
  p_MemCmp_LengthFour_OrdPomog(s1, s2, length, ordsgn, actionE, actionS, actionG)
#define p_MemCmp_LengthFive_OrdNomog(s1, s2, length, ordsgn, actionE, actionG, actionS)    \
  p_MemCmp_LengthFive_OrdPomog(s1, s2, length, ordsgn, actionE, actionS, actionG)
#define p_MemCmp_LengthSix_OrdNomog(s1, s2, length, ordsgn, actionE, actionG, actionS)     \
  p_MemCmp_LengthSix_OrdPomog(s1, s2, length, ordsgn, actionE, actionS, actionG)
#define p_MemCmp_LengthSeven_OrdNomog(s1, s2, length, ordsgn, actionE, actionG, actionS)   \
  p_MemCmp_LengthSeven_OrdPomog(s1, s2, length, ordsgn, actionE, actionS, actionG)
#define p_MemCmp_LengthEight_OrdNomog(s1, s2, length, ordsgn, actionE, actionG, actionS)   \
  p_MemCmp_LengthEight_OrdPomog(s1, s2, length, ordsgn, actionE, actionS, actionG)
#define p_MemCmp_LengthGeneral_OrdNomog(s1, s2, length, ordsgn, actionE, actionG, actionS) \
  p_MemCmp_LengthGeneral_OrdPomog(s1, s2, length, ordsgn, actionE, actionS, actionG)

/***************************************************************
 *
 *  PomogZero
 *
 *******************************************************************/
#define p_MemCmp_LengthTwo_OrdPomogZero(s1, s2, length, ordsgn, actionE, actionG, actionS)     \
  p_MemCmp_LengthOne_OrdPomog(s1, s2, length, ordsgn, actionE, actionG, actionS)
#define p_MemCmp_LengthThree_OrdPomogZero(s1, s2, length, ordsgn, actionE, actionG, actionS)   \
  p_MemCmp_LengthTwo_OrdPomog(s1, s2, length, ordsgn, actionE, actionG, actionS)
#define p_MemCmp_LengthFour_OrdPomogZero(s1, s2, length, ordsgn, actionE, actionG, actionS)    \
  p_MemCmp_LengthThree_OrdPomog(s1, s2, length, ordsgn, actionE, actionG, actionS)
#define p_MemCmp_LengthFive_OrdPomogZero(s1, s2, length, ordsgn, actionE, actionG, actionS)    \
  p_MemCmp_LengthFour_OrdPomog(s1, s2, length, ordsgn, actionE, actionG, actionS)
#define p_MemCmp_LengthSix_OrdPomogZero(s1, s2, length, ordsgn, actionE, actionG, actionS)     \
  p_MemCmp_LengthFive_OrdPomog(s1, s2, length, ordsgn, actionE, actionG, actionS)
#define p_MemCmp_LengthSeven_OrdPomogZero(s1, s2, length, ordsgn, actionE, actionG, actionS)   \
  p_MemCmp_LengthSix_OrdPomog(s1, s2, length, ordsgn, actionE, actionG, actionS)
#define p_MemCmp_LengthEight_OrdPomogZero(s1, s2, length, ordsgn, actionE, actionG, actionS)   \
  p_MemCmp_LengthSeven_OrdPomog(s1, s2, length, ordsgn, actionE, actionG, actionS)
#define p_MemCmp_LengthGeneral_OrdPomogZero(s1, s2, length, ordsgn, actionE, actionG, actionS) \
  p_MemCmp_LengthGeneral_OrdPomog(s1, s2, (length) -1, ordsgn, actionE, actionG, actionS)

/***************************************************************
 *
 *  NomogZero
 *
 *******************************************************************/
#define p_MemCmp_LengthTwo_OrdNomogZero(s1, s2, length, ordsgn, actionE, actionG, actionS)     \
  p_MemCmp_LengthOne_OrdNomog(s1, s2, length, ordsgn, actionE, actionG, actionS)
#define p_MemCmp_LengthThree_OrdNomogZero(s1, s2, length, ordsgn, actionE, actionG, actionS)   \
  p_MemCmp_LengthTwo_OrdNomog(s1, s2, length, ordsgn, actionE, actionG, actionS)
#define p_MemCmp_LengthFour_OrdNomogZero(s1, s2, length, ordsgn, actionE, actionG, actionS)    \
  p_MemCmp_LengthThree_OrdNomog(s1, s2, length, ordsgn, actionE, actionG, actionS)
#define p_MemCmp_LengthFive_OrdNomogZero(s1, s2, length, ordsgn, actionE, actionG, actionS)    \
  p_MemCmp_LengthFour_OrdNomog(s1, s2, length, ordsgn, actionE, actionG, actionS)
#define p_MemCmp_LengthSix_OrdNomogZero(s1, s2, length, ordsgn, actionE, actionG, actionS)     \
  p_MemCmp_LengthFive_OrdNomog(s1, s2, length, ordsgn, actionE, actionG, actionS)
#define p_MemCmp_LengthSeven_OrdNomogZero(s1, s2, length, ordsgn, actionE, actionG, actionS)   \
  p_MemCmp_LengthSix_OrdNomog(s1, s2, length, ordsgn, actionE, actionG, actionS)
#define p_MemCmp_LengthEight_OrdNomogZero(s1, s2, length, ordsgn, actionE, actionG, actionS)   \
  p_MemCmp_LengthSeven_OrdNomog(s1, s2, length, ordsgn, actionE, actionG, actionS)
#define p_MemCmp_LengthGeneral_OrdNomogZero(s1, s2, length, ordsgn, actionE, actionG, actionS) \
  p_MemCmp_LengthGeneral_OrdNomog(s1, s2, (length) -1, ordsgn, actionE, actionG, actionS)

/***************************************************************
 *
 *  NegPomog
 *
 *******************************************************************/
#define p_MemCmp_LengthTwo_OrdNegPomog(s1, s2, length, ordsgn, actionE, actionG, actionS)  \
  p_MemCmp_LengthOne_OrdPomog(s1, s2, length, ordsgn,                                      \
                           _p_MemCmp_LengthOne_Equal(1, _s2, _s1,actionE),      \
                           actionS, actionG)
#define p_MemCmp_LengthThree_OrdNegPomog(s1, s2, length, ordsgn, actionE, actionG, actionS)    \
  p_MemCmp_LengthOne_OrdPomog(s1, s2, length, ordsgn,                                          \
                           _p_MemCmp_LengthTwo_Equal(1, _s2, _s1, actionE),              \
                           actionS, actionG)
#define p_MemCmp_LengthFour_OrdNegPomog(s1, s2, length, ordsgn, actionE, actionG, actionS) \
  p_MemCmp_LengthOne_OrdPomog(s1, s2, length, ordsgn,                                      \
                           _p_MemCmp_LengthThree_Equal(1, _s2, _s1, actionE),        \
                           actionS, actionG)
#define p_MemCmp_LengthFive_OrdNegPomog(s1, s2, length, ordsgn, actionE, actionG, actionS) \
  p_MemCmp_LengthOne_OrdPomog(s1, s2, length, ordsgn,                                      \
                           _p_MemCmp_LengthFour_Equal(1, _s2, _s1, actionE),         \
                           actionS, actionG)
#define p_MemCmp_LengthSix_OrdNegPomog(s1, s2, length, ordsgn, actionE, actionG, actionS)  \
  p_MemCmp_LengthOne_OrdPomog(s1, s2, length, ordsgn,                                      \
                           _p_MemCmp_LengthFive_Equal(1, _s2, _s1, actionE),         \
                           actionS, actionG)
#define p_MemCmp_LengthSeven_OrdNegPomog(s1, s2, length, ordsgn, actionE, actionG, actionS)    \
  p_MemCmp_LengthOne_OrdPomog(s1, s2, length, ordsgn,                                          \
                           _p_MemCmp_LengthSix_Equal(1, _s2, _s1, actionE),              \
                           actionS, actionG)
#define p_MemCmp_LengthEight_OrdNegPomog(s1, s2, length, ordsgn, actionE, actionG, actionS)    \
  p_MemCmp_LengthOne_OrdPomog(s1, s2, length, ordsgn,                                          \
                           _p_MemCmp_LengthSeven_Equal(1, _s2, _s1, actionE),            \
                           actionS, actionG)
#define p_MemCmp_LengthGeneral_OrdNegPomog(s1, s2, length, ordsgn, actionE, actionG, actionS)      \
  p_MemCmp_LengthOne_OrdPomog(s1, s2, length, ordsgn,                                              \
                           _p_MemCmp_LengthGeneral_Equal(1, _s2, _s1, (length) -1, actionE), \
                           actionS, actionG)

/***************************************************************
 *
 *  PomogNeg
 *
 *******************************************************************/
#define p_MemCmp_LengthTwo_OrdPomogNeg(s1, s2, length, ordsgn, actionE, actionG, actionS)  \
  p_MemCmp_LengthOne_OrdPomog(s1, s2, length, ordsgn,                                      \
                           _p_MemCmp_Equal(1, _s2, _s1, actionE),               \
                           actionG, actionS)
#define p_MemCmp_LengthThree_OrdPomogNeg(s1, s2, length, ordsgn, actionE, actionG, actionS)    \
  p_MemCmp_LengthTwo_OrdPomog(s1, s2, length, ordsgn,                                          \
                           _p_MemCmp_Equal(2, _s2, _s1, actionE),                   \
                           actionG, actionS)
#define p_MemCmp_LengthFour_OrdPomogNeg(s1, s2, length, ordsgn, actionE, actionG, actionS) \
  p_MemCmp_LengthThree_OrdPomog(s1, s2, length, ordsgn,                                    \
                           _p_MemCmp_Equal(3, _s2, _s1, actionE),              \
                           actionG, actionS)
#define p_MemCmp_LengthFive_OrdPomogNeg(s1, s2, length, ordsgn, actionE, actionG, actionS) \
  p_MemCmp_LengthFour_OrdPomog(s1, s2, length, ordsgn,                                     \
                           _p_MemCmp_Equal(4, _s2, _s1, actionE),               \
                           actionG, actionS)
#define p_MemCmp_LengthSix_OrdPomogNeg(s1, s2, length, ordsgn, actionE, actionG, actionS)  \
  p_MemCmp_LengthFive_OrdPomog(s1, s2, length, ordsgn,                                     \
                           _p_MemCmp_Equal(5, _s2, _s1, actionE),               \
                           actionG, actionS)
#define p_MemCmp_LengthSeven_OrdPomogNeg(s1, s2, length, ordsgn, actionE, actionG, actionS)    \
  p_MemCmp_LengthSix_OrdPomog(s1, s2, length, ordsgn,                                          \
                           _p_MemCmp_Equal(6, _s2, _s1, actionE),                   \
                           actionG, actionS)
#define p_MemCmp_LengthEight_OrdPomogNeg(s1, s2, length, ordsgn, actionE, actionG, actionS)    \
  p_MemCmp_LengthSeven_OrdPomog(s1, s2, length, ordsgn,                                        \
                           _p_MemCmp_Equal(7, _s2, _s1, actionE),                   \
                           actionG, actionS)
#define p_MemCmp_LengthGeneral_OrdPomogNeg(s1, s2, length, ordsgn, actionE, actionG, actionS)  \
  p_MemCmp_LengthGeneral_OrdPomog(s1, s2, (length-1), ordsgn,                                  \
                           _p_MemCmp_Equal(_i, _s2, _s1, actionE),                  \
                           actionG, actionS)

/***************************************************************
 *
 *  PosNomog
 *
 *******************************************************************/
#define p_MemCmp_LengthThree_OrdPosNomog(s1, s2, length, ordsgn, actionE, actionG, actionS)    \
  p_MemCmp_LengthThree_OrdNegPomog(s1, s2, length, ordsgn, actionE, actionS, actionG)
#define p_MemCmp_LengthFour_OrdPosNomog(s1, s2, length, ordsgn, actionE, actionG, actionS) \
  p_MemCmp_LengthFour_OrdNegPomog(s1, s2, length, ordsgn, actionE, actionS, actionG)
#define p_MemCmp_LengthFive_OrdPosNomog(s1, s2, length, ordsgn, actionE, actionG, actionS) \
  p_MemCmp_LengthFive_OrdNegPomog(s1, s2, length, ordsgn, actionE, actionS, actionG)
#define p_MemCmp_LengthSix_OrdPosNomog(s1, s2, length, ordsgn, actionE, actionG, actionS)  \
  p_MemCmp_LengthSix_OrdNegPomog(s1, s2, length, ordsgn, actionE, actionS, actionG)
#define p_MemCmp_LengthSeven_OrdPosNomog(s1, s2, length, ordsgn, actionE, actionG, actionS)    \
  p_MemCmp_LengthSeven_OrdNegPomog(s1, s2, length, ordsgn, actionE, actionS, actionG)
#define p_MemCmp_LengthEight_OrdPosNomog(s1, s2, length, ordsgn, actionE, actionG, actionS)    \
  p_MemCmp_LengthEight_OrdNegPomog(s1, s2, length, ordsgn, actionE, actionS, actionG)
#define p_MemCmp_LengthGeneral_OrdPosNomog(s1, s2, length, ordsgn, actionE, actionG, actionS)  \
  p_MemCmp_LengthGeneral_OrdNegPomog(s1, s2, length, ordsgn, actionE, actionS, actionG)

/***************************************************************
 *
 *  NomogPos
 *
 *******************************************************************/
#define p_MemCmp_LengthThree_OrdNomogPos(s1, s2, length, ordsgn, actionE, actionG, actionS)    \
  p_MemCmp_LengthThree_OrdPomogNeg(s1, s2, length, ordsgn, actionE, actionS, actionG)
#define p_MemCmp_LengthFour_OrdNomogPos(s1, s2, length, ordsgn, actionE, actionG, actionS) \
  p_MemCmp_LengthFour_OrdPomogNeg(s1, s2, length, ordsgn, actionE, actionS, actionG)
#define p_MemCmp_LengthFive_OrdNomogPos(s1, s2, length, ordsgn, actionE, actionG, actionS) \
  p_MemCmp_LengthFive_OrdPomogNeg(s1, s2, length, ordsgn, actionE, actionS, actionG)
#define p_MemCmp_LengthSix_OrdNomogPos(s1, s2, length, ordsgn, actionE, actionG, actionS)  \
  p_MemCmp_LengthSix_OrdPomogNeg(s1, s2, length, ordsgn, actionE, actionS, actionG)
#define p_MemCmp_LengthSeven_OrdNomogPos(s1, s2, length, ordsgn, actionE, actionG, actionS)    \
  p_MemCmp_LengthSeven_OrdPomogNeg(s1, s2, length, ordsgn, actionE, actionS, actionG)
#define p_MemCmp_LengthEight_OrdNomogPos(s1, s2, length, ordsgn, actionE, actionG, actionS)    \
  p_MemCmp_LengthEight_OrdPomogNeg(s1, s2, length, ordsgn, actionE, actionS, actionG)
#define p_MemCmp_LengthGeneral_OrdNomogPos(s1, s2, length, ordsgn, actionE, actionG, actionS)  \
  p_MemCmp_LengthGeneral_OrdPomogNeg(s1, s2, length, ordsgn, actionE, actionS, actionG)


/***************************************************************
 *
 *  PomogNegZero
 *
 *******************************************************************/
#define p_MemCmp_LengthThree_OrdPomogNegZero(s1, s2, length, ordsgn, actionE, actionG, actionS)   \
  p_MemCmp_LengthTwo_OrdPomogNeg(s1, s2, length, ordsgn, actionE, actionG, actionS)
#define p_MemCmp_LengthFour_OrdPomogNegZero(s1, s2, length, ordsgn, actionE, actionG, actionS)    \
  p_MemCmp_LengthThree_OrdPomogNeg(s1, s2, length, ordsgn, actionE, actionG, actionS)
#define p_MemCmp_LengthFive_OrdPomogNegZero(s1, s2, length, ordsgn, actionE, actionG, actionS)    \
  p_MemCmp_LengthFour_OrdPomogNeg(s1, s2, length, ordsgn, actionE, actionG, actionS)
#define p_MemCmp_LengthSix_OrdPomogNegZero(s1, s2, length, ordsgn, actionE, actionG, actionS)     \
  p_MemCmp_LengthFive_OrdPomogNeg(s1, s2, length, ordsgn, actionE, actionG, actionS)
#define p_MemCmp_LengthSeven_OrdPomogNegZero(s1, s2, length, ordsgn, actionE, actionG, actionS)   \
  p_MemCmp_LengthSix_OrdPomogNeg(s1, s2, length, ordsgn, actionE, actionG, actionS)
#define p_MemCmp_LengthEight_OrdPomogNegZero(s1, s2, length, ordsgn, actionE, actionG, actionS)   \
  p_MemCmp_LengthSeven_OrdPomogNeg(s1, s2, length, ordsgn, actionE, actionG, actionS)
#define p_MemCmp_LengthGeneral_OrdPomogNegZero(s1, s2, length, ordsgn, actionE, actionG, actionS) \
  p_MemCmp_LengthGeneral_OrdPomogNeg(s1, s2, (length) -1, ordsgn, actionE, actionG, actionS)

/***************************************************************
 *
 *  NegPomogZero
 *
 *******************************************************************/
#define p_MemCmp_LengthThree_OrdNegPomogZero(s1, s2, length, ordsgn, actionE, actionG, actionS)   \
  p_MemCmp_LengthTwo_OrdNegPomog(s1, s2, length, ordsgn, actionE, actionG, actionS)
#define p_MemCmp_LengthFour_OrdNegPomogZero(s1, s2, length, ordsgn, actionE, actionG, actionS)    \
  p_MemCmp_LengthThree_OrdNegPomog(s1, s2, length, ordsgn, actionE, actionG, actionS)
#define p_MemCmp_LengthFive_OrdNegPomogZero(s1, s2, length, ordsgn, actionE, actionG, actionS)    \
  p_MemCmp_LengthFour_OrdNegPomog(s1, s2, length, ordsgn, actionE, actionG, actionS)
#define p_MemCmp_LengthSix_OrdNegPomogZero(s1, s2, length, ordsgn, actionE, actionG, actionS)     \
  p_MemCmp_LengthFive_OrdNegPomog(s1, s2, length, ordsgn, actionE, actionG, actionS)
#define p_MemCmp_LengthSeven_OrdNegPomogZero(s1, s2, length, ordsgn, actionE, actionG, actionS)   \
  p_MemCmp_LengthSix_OrdNegPomog(s1, s2, length, ordsgn, actionE, actionG, actionS)
#define p_MemCmp_LengthEight_OrdNegPomogZero(s1, s2, length, ordsgn, actionE, actionG, actionS)   \
  p_MemCmp_LengthSeven_OrdNegPomog(s1, s2, length, ordsgn, actionE, actionG, actionS)
#define p_MemCmp_LengthGeneral_OrdNegPomogZero(s1, s2, length, ordsgn, actionE, actionG, actionS) \
  p_MemCmp_LengthGeneral_OrdNegPomog(s1, s2, (length) -1, ordsgn, actionE, actionG, actionS)

/***************************************************************
 *
 *  NomogPosZero
 *
 *******************************************************************/
#define p_MemCmp_LengthFour_OrdNomogPosZero(s1, s2, length, ordsgn, actionE, actionG, actionS)    \
  p_MemCmp_LengthThree_OrdNomogPos(s1, s2, length, ordsgn, actionE, actionG, actionS)
#define p_MemCmp_LengthFive_OrdNomogPosZero(s1, s2, length, ordsgn, actionE, actionG, actionS)    \
  p_MemCmp_LengthFour_OrdNomogPos(s1, s2, length, ordsgn, actionE, actionG, actionS)
#define p_MemCmp_LengthSix_OrdNomogPosZero(s1, s2, length, ordsgn, actionE, actionG, actionS)     \
  p_MemCmp_LengthFive_OrdNomogPos(s1, s2, length, ordsgn, actionE, actionG, actionS)
#define p_MemCmp_LengthSeven_OrdNomogPosZero(s1, s2, length, ordsgn, actionE, actionG, actionS)   \
  p_MemCmp_LengthSix_OrdNomogPos(s1, s2, length, ordsgn, actionE, actionG, actionS)
#define p_MemCmp_LengthEight_OrdNomogPosZero(s1, s2, length, ordsgn, actionE, actionG, actionS)   \
  p_MemCmp_LengthSeven_OrdNomogPos(s1, s2, length, ordsgn, actionE, actionG, actionS)
#define p_MemCmp_LengthGeneral_OrdNomogPosZero(s1, s2, length, ordsgn, actionE, actionG, actionS) \
  p_MemCmp_LengthGeneral_OrdNomogPos(s1, s2, (length) -1, ordsgn, actionE, actionG, actionS)

/***************************************************************
 *
 *  PosNomogZero
 *
 *******************************************************************/
#define p_MemCmp_LengthFour_OrdPosNomogZero(s1, s2, length, ordsgn, actionE, actionG, actionS)    \
  p_MemCmp_LengthThree_OrdPosNomog(s1, s2, length, ordsgn, actionE, actionG, actionS)
#define p_MemCmp_LengthFive_OrdPosNomogZero(s1, s2, length, ordsgn, actionE, actionG, actionS)    \
  p_MemCmp_LengthFour_OrdPosNomog(s1, s2, length, ordsgn, actionE, actionG, actionS)
#define p_MemCmp_LengthSix_OrdPosNomogZero(s1, s2, length, ordsgn, actionE, actionG, actionS)     \
  p_MemCmp_LengthFive_OrdPosNomog(s1, s2, length, ordsgn, actionE, actionG, actionS)
#define p_MemCmp_LengthSeven_OrdPosNomogZero(s1, s2, length, ordsgn, actionE, actionG, actionS)   \
  p_MemCmp_LengthSix_OrdPosNomog(s1, s2, length, ordsgn, actionE, actionG, actionS)
#define p_MemCmp_LengthEight_OrdPosNomogZero(s1, s2, length, ordsgn, actionE, actionG, actionS)   \
  p_MemCmp_LengthSeven_OrdPosNomog(s1, s2, length, ordsgn, actionE, actionG, actionS)
#define p_MemCmp_LengthGeneral_OrdPosNomogZero(s1, s2, length, ordsgn, actionE, actionG, actionS) \
  p_MemCmp_LengthGeneral_OrdPosNomog(s1, s2, (length) -1, ordsgn, actionE, actionG, actionS)

/***************************************************************
 *
 * PosPosNomog
 *
 *******************************************************************/
#define p_MemCmp_LengthThree_OrdPosPosNomog(s1, s2, length, ordsgn, actionE, actionG, actionS)             \
  p_MemCmp_LengthOne_OrdPomog(s1, s2, length, ordsgn,                                                      \
                           _p_MemCmp_Equal(1, _s1, _s2,                                         \
                                           _p_MemCmp_LengthOne_Equal(2, _s2, _s1, actionE)),    \
                           actionG, actionS)
#define p_MemCmp_LengthFour_OrdPosPosNomog(s1, s2, length, ordsgn, actionE, actionG, actionS) \
  p_MemCmp_LengthOne_OrdPomog(s1, s2, length, ordsgn,                                                      \
                           _p_MemCmp_Equal(1, _s1, _s2,                                         \
                                           _p_MemCmp_LengthTwo_Equal(2, _s2, _s1, actionE)),    \
                           actionG, actionS)
#define p_MemCmp_LengthFive_OrdPosPosNomog(s1, s2, length, ordsgn, actionE, actionG, actionS) \
  p_MemCmp_LengthOne_OrdPomog(s1, s2, length, ordsgn,                                                      \
                           _p_MemCmp_Equal(1, _s1, _s2,                                         \
                                           _p_MemCmp_LengthThree_Equal(2, _s2, _s1, actionE)),    \
                           actionG, actionS)
#define p_MemCmp_LengthSix_OrdPosPosNomog(s1, s2, length, ordsgn, actionE, actionG, actionS)  \
  p_MemCmp_LengthOne_OrdPomog(s1, s2, length, ordsgn,                                                      \
                           _p_MemCmp_Equal(1, _s1, _s2,                                         \
                                           _p_MemCmp_LengthFour_Equal(2, _s2, _s1, actionE)),    \
                           actionG, actionS)
#define p_MemCmp_LengthSeven_OrdPosPosNomog(s1, s2, length, ordsgn, actionE, actionG, actionS)    \
  p_MemCmp_LengthOne_OrdPomog(s1, s2, length, ordsgn,                                                      \
                           _p_MemCmp_Equal(1, _s1, _s2,                                         \
                                           _p_MemCmp_LengthFive_Equal(2, _s2, _s1, actionE)),    \
                           actionG, actionS)
#define p_MemCmp_LengthEight_OrdPosPosNomog(s1, s2, length, ordsgn, actionE, actionG, actionS)    \
  p_MemCmp_LengthOne_OrdPomog(s1, s2, length, ordsgn,                                                      \
                           _p_MemCmp_Equal(1, _s1, _s2,                                         \
                                           _p_MemCmp_LengthSix_Equal(2, _s2, _s1, actionE)),    \
                           actionG, actionS)
#define p_MemCmp_LengthGeneral_OrdPosPosNomog(s1, s2, length, ordsgn, actionE, actionG, actionS)          \
  p_MemCmp_LengthOne_OrdPomog(s1, s2, length, ordsgn,                                                     \
                           _p_MemCmp_Equal(1, _s1, _s2,                                                   \
                                           _p_MemCmp_LengthGeneral_Equal(2,_s2,_s1,(length-2), actionE)), \
                           actionG, actionS)

/***************************************************************
 *
 * NegPosNomog
 *
 *******************************************************************/
#define p_MemCmp_LengthThree_OrdNegPosNomog(s1, s2, length, ordsgn, actionE, actionG, actionS)             \
  p_MemCmp_LengthOne_OrdPomog(s1, s2, length, ordsgn,                                                      \
                           _p_MemCmp_Equal(1, _s2, _s1,                                         \
                                           _p_MemCmp_LengthOne_Equal(2, _s1, _s2, actionE)),    \
                           actionS, actionG)
#define p_MemCmp_LengthFour_OrdNegPosNomog(s1, s2, length, ordsgn, actionE, actionG, actionS)              \
  p_MemCmp_LengthOne_OrdPomog(s1, s2, length, ordsgn,                                                      \
                           _p_MemCmp_Equal(1, _s2, _s1,                                         \
                                           _p_MemCmp_LengthTwo_Equal(2, _s1, _s2, actionE)),    \
                           actionS, actionG)
#define p_MemCmp_LengthFive_OrdNegPosNomog(s1, s2, length, ordsgn, actionE, actionG, actionS)              \
  p_MemCmp_LengthOne_OrdPomog(s1, s2, length, ordsgn,                                                      \
                           _p_MemCmp_Equal(1, _s2, _s1,                                         \
                                           _p_MemCmp_LengthThree_Equal(2, _s1, _s2, actionE)),  \
                           actionS, actionG)
#define p_MemCmp_LengthSix_OrdNegPosNomog(s1, s2, length, ordsgn, actionE, actionG, actionS)               \
  p_MemCmp_LengthOne_OrdPomog(s1, s2, length, ordsgn,                                                      \
                           _p_MemCmp_Equal(1, _s2, _s1,                                         \
                                           _p_MemCmp_LengthFour_Equal(2, _s1, _s2, actionE)),   \
                           actionS, actionG)
#define p_MemCmp_LengthSeven_OrdNegPosNomog(s1, s2, length, ordsgn, actionE, actionG, actionS)             \
  p_MemCmp_LengthOne_OrdPomog(s1, s2, length, ordsgn,                                                      \
                           _p_MemCmp_Equal(1, _s2, _s1,                                         \
                                           _p_MemCmp_LengthFive_Equal(2, _s1, _s2, actionE)),   \
                           actionS, actionG)
#define p_MemCmp_LengthEight_OrdNegPosNomog(s1, s2, length, ordsgn, actionE, actionG, actionS)             \
  p_MemCmp_LengthOne_OrdPomog(s1, s2, length, ordsgn,                                                      \
                           _p_MemCmp_Equal(1, _s2, _s1,                                         \
                                           _p_MemCmp_LengthSix_Equal(2, _s1, _s2, actionE)),    \
                           actionS, actionG)
#define p_MemCmp_LengthGeneral_OrdNegPosNomog(s1, s2, length, ordsgn, actionE, actionG, actionS)               \
  p_MemCmp_LengthOne_OrdPomog(s1, s2, length, ordsgn,                                                          \
                           _p_MemCmp_Equal(1, _s2, _s1,                                             \
                                           _p_MemCmp_LengthGeneral_Equal(2, _s1,_s2,                \
                                                                         (length-2) , actionE)),    \
                           actionS, actionG)

/***************************************************************
 *
 * PosNomogPos
 *
 *******************************************************************/
#define p_MemCmp_LengthThree_OrdPosNomogPos(s1, s2, length, ordsgn, actionE, actionG, actionS)             \
  p_MemCmp_LengthOne_OrdPomog(s1, s2, length, ordsgn,                                                      \
                           _p_MemCmp_LengthOne_Equal(1, _s2, _s1,                               \
                                           _p_MemCmp_LengthOne_Equal(2, _s1, _s2, actionE)),    \
                           actionG, actionS)
#define p_MemCmp_LengthFour_OrdPosNomogPos(s1, s2, length, ordsgn, actionE, actionG, actionS)  \
  p_MemCmp_LengthOne_OrdPomog(s1, s2, length, ordsgn,                                          \
                           _p_MemCmp_LengthTwo_Equal(1, _s2, _s1,                  \
                                           _p_MemCmp_Equal(3, _s1, _s2, actionE)),  \
                           actionG, actionS)
#define p_MemCmp_LengthFive_OrdPosNomogPos(s1, s2, length, ordsgn, actionE, actionG, actionS)  \
  p_MemCmp_LengthOne_OrdPomog(s1, s2, length, ordsgn,                                          \
                           _p_MemCmp_LengthThree_Equal(1, _s2, _s1,                 \
                                           _p_MemCmp_Equal(4, _s1, _s2, actionE)),  \
                           actionG, actionS)
#define p_MemCmp_LengthSix_OrdPosNomogPos(s1, s2, length, ordsgn, actionE, actionG, actionS)   \
  p_MemCmp_LengthOne_OrdPomog(s1, s2, length, ordsgn,                                          \
                           _p_MemCmp_LengthFour_Equal(1, _s2, _s1,                  \
                                           _p_MemCmp_Equal(5, _s1, _s2, actionE)),  \
                           actionG, actionS)
#define p_MemCmp_LengthSeven_OrdPosNomogPos(s1, s2, length, ordsgn, actionE, actionG, actionS) \
  p_MemCmp_LengthOne_OrdPomog(s1, s2, length, ordsgn,                                          \
                           _p_MemCmp_LengthFive_Equal(1, _s2, _s1,                  \
                                           _p_MemCmp_Equal(6, _s1, _s2, actionE)),  \
                           actionG, actionS)
#define p_MemCmp_LengthEight_OrdPosNomogPos(s1, s2, length, ordsgn, actionE, actionG, actionS) \
  p_MemCmp_LengthOne_OrdPomog(s1, s2, length, ordsgn,                                          \
                           _p_MemCmp_LengthSix_Equal(1, _s2, _s1,                   \
                                           _p_MemCmp_Equal(7, _s1, _s2, actionE)),  \
                           actionG, actionS)
#define p_MemCmp_LengthGeneral_OrdPosNomogPos(s1, s2, length, ordsgn, actionE, actionG, actionS)       \
  p_MemCmp_LengthOne_OrdPomog(s1, s2, length, ordsgn,                                                  \
                           _p_MemCmp_LengthGeneral_Equal(1, _s2, _s1, (length - 2),         \
                                                   _p_MemCmp_Equal(_i, _s1, _s2, actionE)), \
                           actionG, actionS)


/***************************************************************
 *
 *  PosPosNomogZero
 *
 *******************************************************************/
#define p_MemCmp_LengthFour_OrdPosPosNomogZero(s1, s2, length, ordsgn, actionE, actionG, actionS)    \
  p_MemCmp_LengthThree_OrdPosPosNomog(s1, s2, length, ordsgn, actionE, actionG, actionS)
#define p_MemCmp_LengthFive_OrdPosPosNomogZero(s1, s2, length, ordsgn, actionE, actionG, actionS)    \
  p_MemCmp_LengthFour_OrdPosPosNomog(s1, s2, length, ordsgn, actionE, actionG, actionS)
#define p_MemCmp_LengthSix_OrdPosPosNomogZero(s1, s2, length, ordsgn, actionE, actionG, actionS)     \
  p_MemCmp_LengthFive_OrdPosPosNomog(s1, s2, length, ordsgn, actionE, actionG, actionS)
#define p_MemCmp_LengthSeven_OrdPosPosNomogZero(s1, s2, length, ordsgn, actionE, actionG, actionS)   \
  p_MemCmp_LengthSix_OrdPosPosNomog(s1, s2, length, ordsgn, actionE, actionG, actionS)
#define p_MemCmp_LengthEight_OrdPosPosNomogZero(s1, s2, length, ordsgn, actionE, actionG, actionS)   \
  p_MemCmp_LengthSeven_OrdPosPosNomog(s1, s2, length, ordsgn, actionE, actionG, actionS)
#define p_MemCmp_LengthGeneral_OrdPosPosNomogZero(s1, s2, length, ordsgn, actionE, actionG, actionS) \
  p_MemCmp_LengthGeneral_OrdPosPosNomog(s1, s2, (length) -1, ordsgn, actionE, actionG, actionS)

/***************************************************************
 *
 *  PosNomogPosZero
 *
 *******************************************************************/
#define p_MemCmp_LengthFour_OrdPosNomogPosZero(s1, s2, length, ordsgn, actionE, actionG, actionS)    \
  p_MemCmp_LengthThree_OrdPosNomogPos(s1, s2, length, ordsgn, actionE, actionG, actionS)
#define p_MemCmp_LengthFive_OrdPosNomogPosZero(s1, s2, length, ordsgn, actionE, actionG, actionS)    \
  p_MemCmp_LengthFour_OrdPosNomogPos(s1, s2, length, ordsgn, actionE, actionG, actionS)
#define p_MemCmp_LengthSix_OrdPosNomogPosZero(s1, s2, length, ordsgn, actionE, actionG, actionS)     \
  p_MemCmp_LengthFive_OrdPosNomogPos(s1, s2, length, ordsgn, actionE, actionG, actionS)
#define p_MemCmp_LengthSeven_OrdPosNomogPosZero(s1, s2, length, ordsgn, actionE, actionG, actionS)   \
  p_MemCmp_LengthSix_OrdPosNomogPos(s1, s2, length, ordsgn, actionE, actionG, actionS)
#define p_MemCmp_LengthEight_OrdPosNomogPosZero(s1, s2, length, ordsgn, actionE, actionG, actionS)   \
  p_MemCmp_LengthSeven_OrdPosNomogPos(s1, s2, length, ordsgn, actionE, actionG, actionS)
#define p_MemCmp_LengthGeneral_OrdPosNomogPosZero(s1, s2, length, ordsgn, actionE, actionG, actionS) \
  p_MemCmp_LengthGeneral_OrdPosNomogPos(s1, s2, (length) -1, ordsgn, actionE, actionG, actionS)

/***************************************************************
 *
 *  NegPosNomogZero
 *
 *******************************************************************/
#define p_MemCmp_LengthFour_OrdNegPosNomogZero(s1, s2, length, ordsgn, actionE, actionG, actionS)    \
  p_MemCmp_LengthThree_OrdNegPosNomog(s1, s2, length, ordsgn, actionE, actionG, actionS)
#define p_MemCmp_LengthFive_OrdNegPosNomogZero(s1, s2, length, ordsgn, actionE, actionG, actionS)    \
  p_MemCmp_LengthFour_OrdNegPosNomog(s1, s2, length, ordsgn, actionE, actionG, actionS)
#define p_MemCmp_LengthSix_OrdNegPosNomogZero(s1, s2, length, ordsgn, actionE, actionG, actionS)     \
  p_MemCmp_LengthFive_OrdNegPosNomog(s1, s2, length, ordsgn, actionE, actionG, actionS)
#define p_MemCmp_LengthSeven_OrdNegPosNomogZero(s1, s2, length, ordsgn, actionE, actionG, actionS)   \
  p_MemCmp_LengthSix_OrdNegPosNomog(s1, s2, length, ordsgn, actionE, actionG, actionS)
#define p_MemCmp_LengthEight_OrdNegPosNomogZero(s1, s2, length, ordsgn, actionE, actionG, actionS)   \
  p_MemCmp_LengthSeven_OrdNegPosNomog(s1, s2, length, ordsgn, actionE, actionG, actionS)
#define p_MemCmp_LengthGeneral_OrdNegPosNomogZero(s1, s2, length, ordsgn, actionE, actionG, actionS) \
  p_MemCmp_LengthGeneral_OrdNegPosNomog(s1, s2, (length) -1, ordsgn, actionE, actionG, actionS)


/***************************************************************
 *
 *  OrdGeneral
 *
 *******************************************************************/
#define _p_MemCmp_OrdGeneral_Declare(s1, s2)    \
  const unsigned long* _s1 = ((unsigned long*) s1);     \
  const unsigned long* _s2 = ((unsigned long*) s2);     \
  register unsigned long _v1;                           \
  register unsigned long _v2;                           \
  register unsigned long _i

#define _p_MemCmp_OrdGeneral_NotEqual(ordsgn, actionG, actionS) \
do                                                              \
{                                                               \
  const long* _ordsgn = (long*) ordsgn;                        \
  if (_v1 > _v2)                                                \
  {                                                             \
    if (_ordsgn[_i] == 1) actionG;                              \
    actionS;                                                    \
  }                                                             \
  if (_ordsgn[_i] == 1) actionS;                                \
  actionG;                                                      \
}                                                               \
while (0)

#define _p_MemCmp_OrdGeneral(i, actionE)         \
do                                              \
{                                               \
  _i = i;                                       \
  _v1 = _s1[i];                                 \
  _v2 = _s2[i];                                 \
  if (_v1 == _v2) actionE;                      \
}                                               \
while(0)

#define _p_MemCmp_LengthTwo_OrdGeneral(actionE)          \
  _p_MemCmp_OrdGeneral(0, _p_MemCmp_OrdGeneral(1, actionE))

#define _p_MemCmp_LengthThree_OrdGeneral(actionE)          \
  _p_MemCmp_LengthTwo_OrdGeneral(_p_MemCmp_OrdGeneral(2, actionE))

#define _p_MemCmp_LengthFour_OrdGeneral(actionE)          \
  _p_MemCmp_LengthThree_OrdGeneral(_p_MemCmp_OrdGeneral(3, actionE))

#define _p_MemCmp_LengthFive_OrdGeneral(actionE)          \
  _p_MemCmp_LengthFour_OrdGeneral(_p_MemCmp_OrdGeneral(4, actionE))

#define _p_MemCmp_LengthSix_OrdGeneral(actionE)          \
  _p_MemCmp_LengthFive_OrdGeneral(_p_MemCmp_OrdGeneral(5, actionE))

#define _p_MemCmp_LengthSeven_OrdGeneral(actionE)          \
  _p_MemCmp_LengthSix_OrdGeneral(_p_MemCmp_OrdGeneral(6, actionE))

#define _p_MemCmp_LengthEight_OrdGeneral(actionE)          \
  _p_MemCmp_LengthSeven_OrdGeneral(_p_MemCmp_OrdGeneral(7, actionE))

#define p_MemCmp_LengthOne_OrdGeneral(s1, s2, length, ordsgn, actionE, actionG, actionS)    \
do                                                                                          \
{                                                                                           \
  _p_MemCmp_OrdGeneral_Declare(s1, s2);                                                     \
  _p_MemCmp_OrdGeneral(0, actionE);                                                         \
  _p_MemCmp_OrdGeneral_NotEqual(ordsgn, actionG, actionS);                                  \
}                                                                                           \
while (0)

#define p_MemCmp_LengthTwo_OrdGeneral(s1, s2, length, ordsgn, actionE, actionG, actionS)    \
do                                                                                          \
{                                                                                           \
  _p_MemCmp_OrdGeneral_Declare(s1, s2);                                                     \
  _p_MemCmp_LengthTwo_OrdGeneral(actionE);                                               \
  _p_MemCmp_OrdGeneral_NotEqual(ordsgn, actionG, actionS);                                  \
}                                                                                           \
while (0)

#define p_MemCmp_LengthThree_OrdGeneral(s1, s2, length, ordsgn, actionE, actionG, actionS)  \
do                                                                                          \
{                                                                                           \
  _p_MemCmp_OrdGeneral_Declare(s1, s2);                                                     \
  _p_MemCmp_LengthThree_OrdGeneral(actionE);                                             \
  _p_MemCmp_OrdGeneral_NotEqual(ordsgn, actionG, actionS);                                  \
}                                                                                           \
while (0)

#define p_MemCmp_LengthFour_OrdGeneral(s1, s2, length, ordsgn, actionE, actionG, actionS)   \
do                                                                                          \
{                                                                                           \
  _p_MemCmp_OrdGeneral_Declare(s1, s2);                                                     \
  _p_MemCmp_LengthFour_OrdGeneral(actionE);                                              \
  _p_MemCmp_OrdGeneral_NotEqual(ordsgn, actionG, actionS);                                  \
}                                                                                           \
while (0)

#define p_MemCmp_LengthFive_OrdGeneral(s1, s2, length, ordsgn, actionE, actionG, actionS)   \
do                                                                                          \
{                                                                                           \
  _p_MemCmp_OrdGeneral_Declare(s1, s2);                                                     \
  _p_MemCmp_LengthFive_OrdGeneral(actionE);                                              \
  _p_MemCmp_OrdGeneral_NotEqual(ordsgn, actionG, actionS);                                  \
}                                                                                           \
while (0)

#define p_MemCmp_LengthSix_OrdGeneral(s1, s2, length, ordsgn, actionE, actionG, actionS)    \
do                                                                                          \
{                                                                                           \
  _p_MemCmp_OrdGeneral_Declare(s1, s2);                                                     \
  _p_MemCmp_LengthSix_OrdGeneral(actionE);                                               \
  _p_MemCmp_OrdGeneral_NotEqual(ordsgn, actionG, actionS);                                  \
}                                                                                           \
while (0)

#define p_MemCmp_LengthSeven_OrdGeneral(s1, s2, length, ordsgn, actionE, actionG, actionS)  \
do                                                                                          \
{                                                                                           \
  _p_MemCmp_OrdGeneral_Declare(s1, s2);                                                     \
  _p_MemCmp_LengthSeven_OrdGeneral(actionE);                                             \
  _p_MemCmp_OrdGeneral_NotEqual(ordsgn, actionG, actionS);                                  \
}                                                                                           \
while (0)

#define p_MemCmp_LengthEight_OrdGeneral(s1, s2, length, ordsgn, actionE, actionG, actionS)  \
do                                                                                          \
{                                                                                           \
  _p_MemCmp_OrdGeneral_Declare(s1, s2);                                                     \
  _p_MemCmp_LengthEight_OrdGeneral(actionE);                                                \
  _p_MemCmp_OrdGeneral_NotEqual(ordsgn, actionG, actionS);                                  \
}                                                                                           \
while (0)

/***************************************************************
 *
 *  Last but not least LengthGeneral_OrdGeneral
 *
 *******************************************************************/
#define p_MemCmp_LengthGeneral_OrdGeneral(s1, s2, length, ordsgn, actionE, actionG, actionS)    \
do                                                                                              \
{                                                                                               \
  _p_MemCmp_OrdGeneral_Declare(s1, s2);                                                         \
  const unsigned long _l = (unsigned long) length;                                              \
                                                                                                \
  _i=0;                                                                                         \
                                                                                                \
  LengthGeneral_OrdGeneral_LoopTop:                                                             \
  _v1 = _s1[_i];                                                                                \
  _v2 = _s2[_i];                                                                                \
  if (_v1 == _v2)                                                                               \
  {                                                                                             \
    _i++;                                                                                       \
    if (_i == _l) actionE;                                                                      \
    goto LengthGeneral_OrdGeneral_LoopTop;                                                      \
  }                                                                                             \
                                                                                                \
  _p_MemCmp_OrdGeneral_NotEqual(ordsgn, actionG, actionS);                                      \
}                                                                                               \
while (0)

// or, in other (less efficient) words
#define p_MemCmp_LengthGeneral_OrdGeneral2(s1, s2, length, ordsgn, actionE, actionG, actionS)   \
do                                                                                              \
{                                                                                               \
  int _i;                                                                                       \
                                                                                                \
  for (_i=0; _i<length;_i++)                                                                    \
  {                                                                                             \
    if (s1[_i] != s2[_i])                                                                       \
    {                                                                                           \
      if (s1[_i] > s2[_i])                                                                      \
      {                                                                                         \
        if (ordsgn[_i] == 1)                                                                    \
          actionG;                                                                              \
        actionS;                                                                                \
      }                                                                                         \
      if (ordsgn[_i] == 1)                                                                      \
        actionS;                                                                                \
      actionG;                                                                                  \
    }                                                                                           \
  }                                                                                             \
  actionE;                                                                                      \
}                                                                                               \
while (0)

/***************************************************************
 *
 *  Bitmask
 *
 *******************************************************************/
#define _p_MemCmp_Bitmask_Declare(s1, s2, bitmask)  \
  const unsigned long* _s1 = ((unsigned long*) s1); \
  const unsigned long* _s2 = ((unsigned long*) s2); \
  register const unsigned long _bitmask = bitmask;  \
  register unsigned long _v1;                       \
  register unsigned long _v2;                       \
  register unsigned long _i                         \


#define p_MemCmp_Bitmask_LengthGeneral(s1, s2, bitmask, length, actionG, actionS)   \
do                                                                                  \
{                                                                                   \
  _p_MemCmp_Bitmask_Declare(s1, s2, bitmask);                                             \
  const unsigned long _l = (unsigned long) length;                                  \
                                                                                    \
  _i=0;                                                                             \
                                                                                    \
  while (_i < _l)                                                                   \
  {                                                                                 \
    _v1 = _s1[_i];                                                                  \
    _v2 = _s2[_i];                                                                  \
                                                                                    \
    if ((_v1 > _v2) ||                                                              \
        (((_v1 & _bitmask) ^ (_v2 & _bitmask)) != ((_v2 - _v1) & _bitmask)))           \
      actionS;                                                                      \
    _i++;                                                                           \
  }                                                                                 \
  actionG;                                                                          \
}                                                                                   \
while (0)


#define _p_MemCmp_Bitmask(i, actionS)                                       \
do                                                                          \
{                                                                           \
  _i = i;                                                                   \
  _v1 = _s1[i];                                                             \
  _v2 = _s2[i];                                                             \
  if ((_v1 > _v2) ||                                                        \
      (((_v1 & _bitmask) ^ (_v2 & _bitmask)) != ((_v2 - _v1) & _bitmask)))     \
    actionS;                                                               \
}                                                                           \
while (0)

#define _p_MemCmp_Bitmask_LengthTwo(actionS)    \
  do                                            \
{                                               \
  _p_MemCmp_Bitmask(0, actionS);                \
  _p_MemCmp_Bitmask(1, actionS);                \
}                                               \
while (0)

#define _p_MemCmp_Bitmask_LengthThree(actionS)  \
  do                                            \
{                                               \
  _p_MemCmp_Bitmask_LengthTwo(actionS);         \
  _p_MemCmp_Bitmask(2, actionS);                \
}                                               \
while (0)

#define _p_MemCmp_Bitmask_LengthFour(actionS)   \
  do                                            \
{                                               \
  _p_MemCmp_Bitmask_LengthThree(actionS);    \
  _p_MemCmp_Bitmask(3, actionS);                \
}                                               \
while (0)


#define _p_MemCmp_Bitmask_LengthFive(actionS)   \
  do                                            \
{                                               \
  _p_MemCmp_Bitmask_LengthFour(actionS);     \
  _p_MemCmp_Bitmask(4, actionS);                \
}                                               \
while (0)


#define _p_MemCmp_Bitmask_LengthSix(actionS)    \
  do                                            \
{                                               \
  _p_MemCmp_Bitmask_LengthFive(actionS);     \
  _p_MemCmp_Bitmask(5, actionS);                \
}                                               \
while (0)

#define _p_MemCmp_Bitmask_LengthSeven(actionS)  \
  do                                            \
{                                               \
  _p_MemCmp_Bitmask_LengthSix(actionS);      \
  _p_MemCmp_Bitmask(6, actionS);                \
}                                               \
while (0)

#define _p_MemCmp_Bitmask_LengthEight(actionS)  \
do                                              \
{                                               \
  _p_MemCmp_Bitmask_LengthSeven(actionS);    \
  _p_MemCmp_Bitmask(7, actionS);                \
}                                               \
while (0)

#define p_MemCmp_Bitmask_LengthZero(s1, s2, bitmask, length, actionG, actionS) actionG

#define p_MemCmp_Bitmask_LengthOne(s1, s2, bitmask, length, actionG, actionS)   \
do                                                                              \
{                                                                               \
  _p_MemCmp_Bitmask_Declare(s1, s2, bitmask);                                         \
  _p_MemCmp_Bitmask(0, actionS);                                                \
  actionG;                                                                      \
}                                                                               \
while (0)

#define p_MemCmp_Bitmask_LengthTwo(s1, s2, bitmask, length, actionG, actionS)   \
do                                                                              \
{                                                                               \
  _p_MemCmp_Bitmask_Declare(s1, s2, bitmask);                                         \
  _p_MemCmp_Bitmask_LengthTwo(actionS);                                         \
  actionG;                                                                      \
}                                                                               \
while (0)

#define p_MemCmp_Bitmask_LengthThree(s1, s2, bitmask, length, actionG, actionS)   \
do                                                                              \
{                                                                               \
  _p_MemCmp_Bitmask_Declare(s1, s2, bitmask);                                         \
  _p_MemCmp_Bitmask_LengthThree(actionS);                                         \
  actionG;                                                                      \
}                                                                               \
while (0)

#define p_MemCmp_Bitmask_LengthFour(s1, s2, bitmask, length, actionG, actionS)   \
do                                                                              \
{                                                                               \
  _p_MemCmp_Bitmask_Declare(s1, s2, bitmask);                                         \
  _p_MemCmp_Bitmask_LengthFour(actionS);                                         \
  actionG;                                                                      \
}                                                                               \
while (0)

#define p_MemCmp_Bitmask_LengthFive(s1, s2, bitmask, length, actionG, actionS)   \
do                                                                              \
{                                                                               \
  _p_MemCmp_Bitmask_Declare(s1, s2, bitmask);                                         \
  _p_MemCmp_Bitmask_LengthFive(actionS);                                         \
  actionG;                                                                      \
}                                                                               \
while (0)

#define p_MemCmp_Bitmask_LengthSix(s1, s2, bitmask, length, actionG, actionS)   \
do                                                                              \
{                                                                               \
  _p_MemCmp_Bitmask_Declare(s1, s2, bitmask);                                         \
  _p_MemCmp_Bitmask_LengthSix(actionS);                                         \
  actionG;                                                                      \
}                                                                               \
while (0)

#define p_MemCmp_Bitmask_LengthSeven(s1, s2, bitmask, length, actionG, actionS) \
do                                                                              \
{                                                                               \
  _p_MemCmp_Bitmask_Declare(s1, s2, bitmask);                                         \
  _p_MemCmp_Bitmask_LengthSeven(actionS);                                       \
  actionG;                                                                      \
}                                                                               \
while (0)

#define p_MemCmp_Bitmask_LengthEigth(s1, s2, bitmask, length, actionG, actionS) \
do                                                                              \
{                                                                               \
  _p_MemCmp_Bitmask_Declare(s1, s2, bitmask);                                         \
  _p_MemCmp_Bitmask_LengthEigth(actionS);                                       \
  actionG;                                                                      \
}                                                                               \
while (0)


#endif // P_MEM_CMP
