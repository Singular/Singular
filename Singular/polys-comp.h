#ifndef POLYS_COMP_H
#define POLYS_COMP_H
/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/* $Id: polys-comp.h,v 1.3 1998-01-15 16:15:26 obachman Exp $ */

/***************************************************************
 *
 * File:       polys-comp.h
 * Purpose:    low-level monomial comparison routines
 *
 ***************************************************************/

#include "polys-impl.h"

#ifdef COMP_FAST

  

#ifdef WORDS_BIGENDIAN

#ifndef COMP_NO_EXP_VECTOR_OPS
#define _pMonCmp_1(p1, p2, d, actionD, actionE)              \
do                                                          \
{                                                           \
  d = *((long*) &(p1->exp[0])) - *((long*) &(p2->exp[0]));  \
  if (d) actionD;                                           \
  actionE;                                                  \
}                                                           \
while(0)
  
#define _pMonCmp_1_c(p1, p2, d, actionD, actionE)            \
do                                                          \
{                                                           \
  d = *((long*) &(p1->exp[0])) - *((long*) &(p2->exp[0]));  \
  if (d)                                                    \
  {                                                         \
    if (((long) (pGetComp(p1) - pGetComp(p2))) == d)        \
      d = -d;                                               \
    actionD;                                                \
  }                                                         \
  actionE;                                                  \
}                                                           \
while(0)

#define _pMonCmp_2(p1, p2, d, actionD, actionE)  \
do                                              \
{                                               \
  const long* s1 = (long*) &(p1->exp[0]);             \
  const long* s2 = (long*) &(p2->exp[0]);             \
  d = *s1 - *s2;                                \
  if (d) actionD;                               \
  d = *(s1 + 1) - *(s2 + 1);                    \
  if (d) actionD;                               \
  actionE;                                      \
}                                               \
while(0)
  
#define _pMonCmp_2_c(p1, p2, d, actionD, actionE)        \
do                                                      \
{                                                       \
  const long* s1 = (long*) &(p1->exp[0]);                     \
  const long* s2 = (long*) &(p2->exp[0]);                     \
  d = *s1 - *s2;                                        \
  if (d) actionD;                                       \
  d = *(s1 + 1) - *(s2 + 1);                            \
  if (d)                                                \
  {                                                     \
    if (((long) (pGetComp(p1) - pGetComp(p2))) == d)    \
      d = -d;                                           \
    actionD;                                            \
  }                                                     \
  actionE;                                              \
}                                                       \
while(0)
  
#define _pMonCmp_2i(p1, p2, length, d, actionD, actionE) \
do                                                      \
{                                                       \
  const long* s1 = (long*) &(p1->exp[0]);                     \
  const long* s2 = (long*) &(p2->exp[0]);                     \
  const long* const lb = s1 + length;                               \
                                                        \
  for (;;)                                              \
  {                                                     \
    d = *s1 - *s2;                                      \
    if (d) actionD;                                     \
    s1++;                                               \
    s2++;                                               \
    d = *s1 - *s2;                                      \
    if (d) actionD;                                     \
    s1++;                                               \
    if (s1 == lb) actionE;                              \
    s2++;                                               \
  }                                                     \
}                                                       \
while(0)

#define _pMonCmp_2i_1(p1, p2, length, d, actionD, actionE)   \
do                                                          \
{                                                           \
  const long* s1 = (long*) &(p1->exp[0]);                         \
  const long* s2 = (long*) &(p2->exp[0]);                         \
  const long* const lb = s1 + length;                                   \
                                                            \
  for (;;)                                                  \
  {                                                         \
    d = *s1 - *s2;                                          \
    if (d) actionD;                                         \
    s1++;                                                   \
    if (s1 == lb) actionE;                                  \
    s2++;                                                   \
    d = *s1 - *s2;                                          \
    if (d) actionD;                                         \
    s1++;                                                   \
    s2++;                                                   \
  }                                                         \
}                                                           \
while(0)
  
#define _pMonCmp_2i_c(p1, p2, length, d, actionD, actionE)  \
do                                                          \
{                                                           \
  const long* s1 = (long*) &(p1->exp[0]);                   \
  const long* s2 = (long*) &(p2->exp[0]);                   \
  const long* const lb = s1 + length-1;                     \
                                                            \
  for (;;)                                                  \
  {                                                         \
    d = *s1 - *s2;                                          \
    if (d) actionD;                                         \
    s1++;                                                   \
    if (s1 == lb) break;                                    \
    s2++;                                                   \
    d = *s1 - *s2;                                          \
    if (d) actionD;                                         \
    s1++;                                                   \
    s2++;                                                   \
  }                                                         \
                                                            \
  d = *s1 - *(s2 + 1);                                      \
  if (d)                                                    \
  {                                                         \
    if (((long) (pGetComp(p1) - pGetComp(p2))) == d)        \
      d = -d;                                               \
    actionD;                                                \
  }                                                         \
  actionE;                                                  \
}                                                           \
while(0)
  
#define _pMonCmp_2i_1_c(p1, p2, length, d, actionD, actionE)    \
do                                                              \
{                                                               \
  const long* s1 = (long*) &(p1->exp[0]);                       \
  const long* s2 = (long*) &(p2->exp[0]);                       \
  const long* const lb = s1 + length      -1;                   \
                                                                \
  for (;;)                                                      \
  {                                                             \
    d = *s1 - *s2;                                              \
    if (d) actionD;                                             \
    s1++;                                                       \
    s2++;                                                       \
    d = *s1 - *s2;                                              \
    if (d) actionD;                                             \
    s1++;                                                       \
    if (s1 == lb) break;                                        \
    s2++;                                                       \
  }                                                             \
                                                                \
  d = *s1 - *(s2 + 1);                                          \
  if (d)                                                        \
  {                                                             \
    if (((long) (pGetComp(p1) - pGetComp(p2))) == d)            \
      d = -d;                                                   \
    actionD;                                                    \
  }                                                             \
  actionE;                                                      \
}                                                               \
while(0)

#else // COMP_NO_EXP_VECTOR_OPS

#define _pMonCmp(p1, p2, d, actionD, actionE)   \
do                                              \
{                                               \
  Exponent_pt e1 = &(p1->exp[0]);         \
  Exponent_pt e2 = &(p2->exp[0]);         \
  const Exponent_pt ub = e1 + pVariables; \
                                                \
  for (;;)                                      \
  {                                             \
    d = *e1 - *e2;                              \
    if (d) actionD;                             \
    e1++;                                       \
    if (e1 == ub)                               \
    {                                           \
      d = pGetComp(p1) - pGetComp(p2);          \
      if (d) actionD;                           \
      actionE;                                  \
    }                                           \
    e2++;                                       \
  }                                             \
}                                               \
while(0)

#define _pMonCmp_c(p1, p2, d, actionD, actionE) \
do                                              \
{                                               \
  Exponent_pt e1 = &(p1->exp[0]);         \
  Exponent_pt e2 = &(p2->exp[0]);         \
  const Exponent_pt ub = e1 + pVariables; \
                                                \
  for (;;)                                      \
  {                                             \
    d = *e1 - *e2;                              \
    if (d) actionD;                             \
    e1++;                                       \
    if (e1 == ub)                               \
    {                                           \
      d = pGetComp(p2) - pGetComp(p1);          \
      if (d) actionD;                           \
      actionE;                                  \
    }                                           \
    e2++;                                       \
  }                                             \
}                                               \
while(0)

#define _pMonCmp_1(p1, p2, d, actionD, actionE)  \
  _pMonCmp(p1, p2, d, actionD, actionE)

#define _pMonCmp_1_c(p1, p2, d, actionD, actionE)  \
  _pMonCmp_c(p1, p2, d, actionD, actionE)
  
#define _pMonCmp_2(p1, p2, d, actionD, actionE)  \
  _pMonCmp(p1, p2, d, actionD, actionE)

#define _pMonCmp_2_c(p1, p2, d, actionD, actionE)  \
  _pMonCmp_c(p1, p2, d, actionD, actionE)
  
#define _pMonCmp_2i(p1, p2, length, d, actionD, actionE) \
  _pMonCmp(p1, p2, d, actionD, actionE)

#define _pMonCmp_2i_1(p1, p2, length, d, actionD, actionE) \
  _pMonCmp(p1, p2, d, actionD, actionE)

#define _pMonCmp_2i_c(p1, p2, length, d, actionD, actionE)   \
  _pMonCmp_c(p1, p2, d, actionD, actionE)

#define _pMonCmp_2i_1_c(p1, p2, length, d, actionD, actionE) \
  _pMonCmp_c(p1, p2, d, actionD, actionE)

#endif // COMP_NO_EXP_VECTOR_OPS 

#else //  ! WORDS_BIGENDIAN

#ifndef COMP_NO_EXP_VECTOR_OPS  
#define _pMonCmp_1(p1, p2, d, actionD, actionE)                              \
do                                                                          \
{                                                                           \
  d = *(((long*) p1) + pMonomSizeW-1) - *(((long*) p2)  + pMonomSizeW-1);   \
  if (d) actionD;                                                           \
  actionE;                                                                  \
}                                                                           \
while(0)
  
#define _pMonCmp_1_c(p1, p2, d, actionD, actionE)                            \
do                                                                          \
{                                                                           \
  d = *(((long*) p1) + pMonomSizeW-1) - *(((long*) p2)  + pMonomSizeW-1);   \
  if (d)                                                                    \
  {                                                                         \
    if (((long) (pGetComp(p1) - pGetComp(p2))) == d)                        \
      d = -d;                                                               \
    actionD;                                                                \
  }                                                                         \
  actionE;                                                                  \
}                                                                           \
while(0)

#define _pMonCmp_2(p1, p2, d, actionD, actionE)  \
do                                              \
{                                               \
  const long* s1 = ((long*) p1) + pMonomSizeW-1;      \
  const long* s2 = ((long*) p2)  + pMonomSizeW-1;      \
  d = *s1 - *s2;                                \
  if (d) actionD;                               \
  d = *(s1 - 1) - *(s2 - 1);                    \
  if (d) actionD;                               \
  actionE;                                      \
}                                               \
while(0)
  
#define _pMonCmp_2_c(p1, p2, d, actionD, actionE)        \
do                                                      \
{                                                       \
  const long* s1 = ((long*) p1) + pMonomSizeW-1;              \
  const long* s2 = ((long*) p2)  + pMonomSizeW-1;             \
  d = *s1 - *s2;                                        \
  if (d) actionD;                                       \
  d = *s1 - *s2;                                        \
  if (d) actionD;                                       \
  d = *(s1 -1) - *(s2 -1);                              \
  if (d)                                                \
  {                                                     \
    if (((long) (pGetComp(p1) - pGetComp(p2))) == d)    \
      d = -d;                                           \
    actionD;                                            \
  }                                                     \
  actionE;                                              \
}                                                       \
while(0)
  
#define _pMonCmp_2i(p1, p2, length, d, actionD, actionE) \
do                                                      \
{                                                       \
  const long* s1 = ((long*) p1) + pMonomSizeW-1;              \
  const long* s2 = ((long*) p2)  + pMonomSizeW-1;             \
  const long* const lb = s1 - length;                                \
  for (;;)                                              \
  {                                                     \
    d = *s1 - *s2;                                      \
    if (d) actionD;                                     \
    s1--;                                               \
    s2--;                                               \
    d = *s1 - *s2;                                      \
    if (d) actionD;                                     \
    s1--;                                               \
    if (s1 == lb) actionE;                              \
    s2--;                                               \
  }                                                     \
}                                                       \
while(0)

#define _pMonCmp_2i_1(p1, p2, length, d, actionD, actionE)   \
do                                                          \
{                                                           \
  const long* s1 = ((long*) p1) + pMonomSizeW-1;                  \
  const long* s2 = ((long*) p2)  + pMonomSizeW-1;                 \
  const long* const lb = s1 - length;                                    \
                                                            \
  for (;;)                                                  \
  {                                                         \
    d = *s1 - *s2;                                          \
    if (d) actionD;                                         \
    s1--;                                                   \
    if (s1 == lb) actionE;                                  \
    s2--;                                                   \
    d = *s1 - *s2;                                          \
    if (d) actionD;                                         \
    s1--;                                                   \
    s2--;                                                   \
  }                                                         \
}                                                           \
while(0)
  
#define _pMonCmp_2i_c(p1, p2, length, d, actionD, actionE)   \
do                                                          \
{                                                           \
  const long* s1 = ((long*) p1) + pMonomSizeW-1;                  \
  const long* s2 = ((long*) p2)  + pMonomSizeW-1;                 \
  const long* const lb = s1 - length +1;                                 \
                                                            \
  for (;;)                                                  \
  {                                                         \
    d = *s1 - *s2;                                          \
    if (d) actionD;                                         \
    s1--;                                                   \
    if (s1 == lb) break;                                    \
    s2--;                                                   \
    d = *s1 - *s2;                                          \
    if (d) actionD;                                         \
    s1--;                                                   \
    s2--;                                                   \
  }                                                         \
                                                            \
  d = *s1 - *(s2 - 1);                                      \
  if (d)                                                    \
  {                                                         \
    if (((long) (pGetComp(p1) - pGetComp(p2))) == d)        \
      d = -d;                                               \
    actionD;                                                \
  }                                                         \
  actionE;                                                  \
}                                                           \
while(0)
  
#define _pMonCmp_2i_1_c(p1, p2, length, d, actionD, actionE) \
do                                                          \
{                                                           \
  const long* s1 = ((long*) p1) + pMonomSizeW-1;                  \
  const long* s2 = ((long*) p2)  + pMonomSizeW-1;                 \
  const long* const lb = s1 - length +1;                                 \
                                                            \
  for (;;)                                                  \
  {                                                         \
    d = *s1 - *s2;                                          \
    if (d) actionD;                                         \
    s1--;                                                   \
    s2--;                                                   \
    d = *s1 - *s2;                                          \
    if (d) actionD;                                         \
    s1--;                                                   \
    if (s1 == lb) break;                                    \
    s2--;                                                   \
  }                                                         \
                                                            \
  d = *s1 - *(s2 - 1);                                      \
  if (d)                                                    \
  {                                                         \
    if (((long) (pGetComp(p1) - pGetComp(p2))) == d)        \
      d = -d;                                               \
    actionD;                                                \
  }                                                         \
  actionE;                                                  \
}                                                           \
while(0)

#else // COMP_NO_EXP_VECTOR_OPS 

#define _pMonCmp(p1, p2, d, actionD, actionE)   \
do                                              \
{                                               \
  Exponent_pt e1 = &(p1->exp[pVarHighIndex]);   \
  Exponent_pt e2 = &(p2->exp[pVarHighIndex]);   \
  const Exponent_pt ub = e1 - pVariables;       \
                                                \
  for (;;)                                      \
  {                                             \
    d = *e1 - *e2;                              \
    if (d) actionD;                             \
    e1--;                                       \
    if (e1 == ub)                               \
    {                                           \
      d = pGetComp(p1) - pGetComp(p2);          \
      if (d) actionD;                           \
      actionE;                                  \
    }                                           \
    e2--;                                       \
  }                                             \
}                                               \
while(0)

#define _pMonCmp_c(p1, p2, d, actionD, actionE) \
do                                              \
{                                               \
  Exponent_pt e1 = &(p1->exp[pVarHighIndex]);   \
  Exponent_pt e2 = &(p2->exp[pVarHighIndex]);   \
  const Exponent_pt ub = e1 - pVariables;       \
                                                \
  for (;;)                                      \
  {                                             \
    d = *e1 - *e2;                              \
    if (d) actionD;                             \
    e1--;                                       \
    if (e1 == ub)                               \
    {                                           \
      d = pGetComp(p2) - pGetComp(p1);          \
      if (d) actionD;                           \
      actionE;                                  \
    }                                           \
    e2--;                                       \
  }                                             \
}                                               \
while(0)

#define _pMonCmp_1(p1, p2, d, actionD, actionE)  \
  _pMonCmp(p1, p2, d, actionD, actionE)

#define _pMonCmp_1_c(p1, p2, d, actionD, actionE)  \
  _pMonCmp_c(p1, p2, d, actionD, actionE)
  
#define _pMonCmp_2(p1, p2, d, actionD, actionE)  \
  _pMonCmp(p1, p2, d, actionD, actionE)

#define _pMonCmp_2_c(p1, p2, d, actionD, actionE)  \
  _pMonCmp_c(p1, p2, d, actionD, actionE)
  
#define _pMonCmp_2i(p1, p2, length, d, actionD, actionE) \
  _pMonCmp(p1, p2, d, actionD, actionE)

#define _pMonCmp_2i_1(p1, p2, length, d, actionD, actionE) \
  _pMonCmp(p1, p2, d, actionD, actionE)

#define _pMonCmp_2i_c(p1, p2, length, d, actionD, actionE)   \
  _pMonCmp_c(p1, p2, d, actionD, actionE)

#define _pMonCmp_2i_1_c(p1, p2, length, d, actionD, actionE) \
  _pMonCmp_c(p1, p2, d, actionD, actionE)
  
#endif // COMP_NO_EXP_VECTOR_OPS 
  
#endif // WORDS_BIGENDIAN

#endif // COMP_FAST

#endif // POLYS_COMP_H

