#ifndef POLYS_COMP_H
#define POLYS_COMP_H
/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/* $Id: polys-comp.h,v 1.7 1998-04-30 15:29:34 obachman Exp $ */

/***************************************************************
 *
 * File:       polys-comp.h
 * Purpose:    low-level monomial comparison routines
 *
 ***************************************************************/

#include "polys-impl.h"

#ifdef WORDS_BIGENDIAN

#define _pMonComp_otEXP_nwONE(p1, p2, d, actionD, actionE)  \
do                                                          \
{                                                           \
  d = *((long*) &(p1->exp[0])) - *((long*) &(p2->exp[0]));  \
  if (d) actionD;                                           \
  actionE;                                                  \
}                                                           \
while(0)
  
#define _pMonComp_otEXPCOMP_nwONE(p1, p2, d, actionD, actionE)  \
do                                                              \
{                                                               \
  d = *((long*) &(p1->exp[0])) - *((long*) &(p2->exp[0]));      \
  if (d)                                                        \
  {                                                             \
    if (((long) (pGetComp(p1) - pGetComp(p2))) == d)            \
      d = -d;                                                   \
    actionD;                                                    \
  }                                                             \
  actionE;                                                      \
}                                                               \
while(0)

#define _pMonComp_otEXP_nwTWO(p1, p2, d, actionD, actionE)  \
do                                                          \
{                                                           \
  const long* s1 = (long*) &(p1->exp[0]);                   \
  const long* s2 = (long*) &(p2->exp[0]);                   \
  d = *s1 - *s2;                                            \
  if (d) actionD;                                           \
  d = *(s1 + 1) - *(s2 + 1);                                \
  if (d) actionD;                                           \
  actionE;                                                  \
}                                                           \
while(0)
  
#define _pMonComp_otEXPCOMP_nwTWO(p1, p2, d, actionD, actionE)  \
do                                                              \
{                                                               \
  const long* s1 = (long*) &(p1->exp[0]);                       \
  const long* s2 = (long*) &(p2->exp[0]);                       \
  d = *s1 - *s2;                                                \
  if (d) actionD;                                               \
  d = *(s1 + 1) - *(s2 + 1);                                    \
  if (d)                                                        \
  {                                                             \
    if (((long) (pGetComp(p1) - pGetComp(p2))) == d)            \
      d = -d;                                                   \
    actionD;                                                    \
  }                                                             \
  actionE;                                                      \
}                                                               \
while(0)
  
#define _pMonComp_otEXP_nwEVEN(p1, p2, length, d, actionD, actionE) \
do                                                                  \
{                                                                   \
  const long* s1 = (long*) &(p1->exp[0]);                           \
  const long* s2 = (long*) &(p2->exp[0]);                           \
  const long* const lb = s1 + length;                               \
                                                                    \
  for (;;)                                                          \
  {                                                                 \
    d = *s1 - *s2;                                                  \
    if (d) actionD;                                                 \
    s1++;                                                           \
    s2++;                                                           \
    d = *s1 - *s2;                                                  \
    if (d) actionD;                                                 \
    s1++;                                                           \
    if (s1 == lb) actionE;                                          \
    s2++;                                                           \
  }                                                                 \
}                                                                   \
while(0)

#define _pMonComp_otEXP_nwODD(p1, p2, length, d, actionD, actionE)  \
do                                                                  \
{                                                                   \
  const long* s1 = (long*) &(p1->exp[0]);                           \
  const long* s2 = (long*) &(p2->exp[0]);                           \
  const long* const lb = s1 + length;                               \
                                                                    \
  for (;;)                                                          \
  {                                                                 \
    d = *s1 - *s2;                                                  \
    if (d) actionD;                                                 \
    s1++;                                                           \
    if (s1 == lb) actionE;                                          \
    s2++;                                                           \
    d = *s1 - *s2;                                                  \
    if (d) actionD;                                                 \
    s1++;                                                           \
    s2++;                                                           \
  }                                                                 \
}                                                                   \
while(0)
  
#define _pMonComp_otEXPCOMP_nwEVEN(p1, p2, length, d, actionD, actionE) \
do                                                                      \
{                                                                       \
  const long* s1 = (long*) &(p1->exp[0]);                               \
  const long* s2 = (long*) &(p2->exp[0]);                               \
  const long* const lb = s1 + length-1;                                 \
                                                                        \
  for (;;)                                                              \
  {                                                                     \
    d = *s1 - *s2;                                                      \
    if (d) actionD;                                                     \
    s1++;                                                               \
    if (s1 == lb) break;                                                \
    s2++;                                                               \
    d = *s1 - *s2;                                                      \
    if (d) actionD;                                                     \
    s1++;                                                               \
    s2++;                                                               \
  }                                                                     \
                                                                        \
  d = *s1 - *(s2 + 1);                                                  \
  if (d)                                                                \
  {                                                                     \
    if (((long) (pGetComp(p1) - pGetComp(p2))) == d)                    \
      d = -d;                                                           \
    actionD;                                                            \
  }                                                                     \
  actionE;                                                              \
}                                                                       \
while(0)
  
#define _pMonComp_otEXPCOMP_nwODD(p1, p2, length, d, actionD, actionE)  \
do                                                                      \
{                                                                       \
  const long* s1 = (long*) &(p1->exp[0]);                               \
  const long* s2 = (long*) &(p2->exp[0]);                               \
  const long* const lb = s1 + length      -1;                           \
                                                                        \
  for (;;)                                                              \
  {                                                                     \
    d = *s1 - *s2;                                                      \
    if (d) actionD;                                                     \
    s1++;                                                               \
    s2++;                                                               \
    d = *s1 - *s2;                                                      \
    if (d) actionD;                                                     \
    s1++;                                                               \
    if (s1 == lb) break;                                                \
    s2++;                                                               \
  }                                                                     \
                                                                        \
  d = *s1 - *(s2 + 1);                                                  \
  if (d)                                                                \
  {                                                                     \
    if (((long) (pGetComp(p1) - pGetComp(p2))) == d)                    \
      d = -d;                                                           \
    actionD;                                                            \
  }                                                                     \
  actionE;                                                              \
}                                                                       \
while(0)

#define _pMonComp_otEXP_nwGEN(p1, p2, length, d, actionD, actionE)  \
do                                                                  \
{                                                                   \
  const long* s1 = (long*) &(p1->exp[0]);                           \
  const long* s2 = (long*) &(p2->exp[0]);                           \
  const long* const lb = s1 + length;                               \
                                                                    \
  for (;;)                                                          \
  {                                                                 \
    d = *s1 - *s2;                                                  \
    if (d) actionD;                                                 \
    s1++;                                                           \
    if (s1 == lb) actionE;                                          \
    s2++;                                                           \
  }                                                                 \
}                                                                   \
while(0)

#define _pMonComp_otEXPCOMP_nwGEN(p1, p2, length, d, actionD, actionE)  \
do                                                                      \
{                                                                       \
  const long* s1 = (long*) &(p1->exp[0]);                               \
  const long* s2 = (long*) &(p2->exp[0]);                               \
  const long* const lb = s1 + length      -1;                           \
                                                                        \
  for (;;)                                                              \
  {                                                                     \
    d = *s1 - *s2;                                                      \
    if (d) actionD;                                                     \
    s1++;                                                               \
    if (s1 == lb) break;                                                \
    s2++;                                                               \
  }                                                                     \
                                                                        \
  d = *s1 - *(s2 + 1);                                                  \
  if (d)                                                                \
  {                                                                     \
    if (((long) (pGetComp(p1) - pGetComp(p2))) == d)                    \
      d = -d;                                                           \
    actionD;                                                            \
  }                                                                     \
  actionE;                                                              \
}                                                                       \
while(0)

  
#else //  ! WORDS_BIGENDIAN

#define _pMonComp_otEXP_nwONE(p1, p2, d, actionD, actionE)                  \
do                                                                          \
{                                                                           \
  d = *(((long*) p1) + pMonomSizeW-1) - *(((long*) p2)  + pMonomSizeW-1);   \
  if (d) actionD;                                                           \
  actionE;                                                                  \
}                                                                           \
while(0)
  
#define _pMonComp_otEXPCOMP_nwONE(p1, p2, d, actionD, actionE)              \
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

#define _pMonComp_otEXP_nwTWO(p1, p2, d, actionD, actionE)  \
do                                                          \
{                                                           \
  const long* s1 = ((long*) p1) + pMonomSizeW-1;            \
  const long* s2 = ((long*) p2)  + pMonomSizeW-1;           \
  d = *s1 - *s2;                                            \
  if (d) actionD;                                           \
  d = *(s1 - 1) - *(s2 - 1);                                \
  if (d) actionD;                                           \
  actionE;                                                  \
}                                                           \
while(0)
  
#define _pMonComp_otEXPCOMP_nwTWO(p1, p2, d, actionD, actionE)  \
do                                                              \
{                                                               \
  const long* s1 = ((long*) p1) + pMonomSizeW-1;                \
  const long* s2 = ((long*) p2)  + pMonomSizeW-1;               \
  d = *s1 - *s2;                                                \
  if (d) actionD;                                               \
  d = *s1 - *s2;                                                \
  if (d) actionD;                                               \
  d = *(s1 -1) - *(s2 -1);                                      \
  if (d)                                                        \
  {                                                             \
    if (((long) (pGetComp(p1) - pGetComp(p2))) == d)            \
      d = -d;                                                   \
    actionD;                                                    \
  }                                                             \
  actionE;                                                      \
}                                                               \
while(0)
  
#define _pMonComp_otEXP_nwEVEN(p1, p2, length, d, actionD, actionE) \
do                                                                  \
{                                                                   \
  const long* s1 = ((long*) p1) + pMonomSizeW-1;                    \
  const long* s2 = ((long*) p2)  + pMonomSizeW-1;                   \
  const long* const lb = s1 - length;                               \
  for (;;)                                                          \
  {                                                                 \
    d = *s1 - *s2;                                                  \
    if (d) actionD;                                                 \
    s1--;                                                           \
    s2--;                                                           \
    d = *s1 - *s2;                                                  \
    if (d) actionD;                                                 \
    s1--;                                                           \
    if (s1 == lb) actionE;                                          \
    s2--;                                                           \
  }                                                                 \
}                                                                   \
while(0)

#define _pMonComp_otEXP_nwODD(p1, p2, length, d, actionD, actionE)  \
do                                                                  \
{                                                                   \
  const long* s1 = ((long*) p1) + pMonomSizeW-1;                    \
  const long* s2 = ((long*) p2)  + pMonomSizeW-1;                   \
  const long* const lb = s1 - length;                               \
                                                                    \
  for (;;)                                                          \
  {                                                                 \
    d = *s1 - *s2;                                                  \
    if (d) actionD;                                                 \
    s1--;                                                           \
    if (s1 == lb) actionE;                                          \
    s2--;                                                           \
    d = *s1 - *s2;                                                  \
    if (d) actionD;                                                 \
    s1--;                                                           \
    s2--;                                                           \
  }                                                                 \
}                                                                   \
while(0)
  
#define _pMonComp_otEXPCOMP_nwEVEN(p1, p2, length, d, actionD, actionE) \
do                                                                      \
{                                                                       \
  const long* s1 = ((long*) p1) + pMonomSizeW-1;                        \
  const long* s2 = ((long*) p2)  + pMonomSizeW-1;                       \
  const long* const lb = s1 - length +1;                                \
                                                                        \
  for (;;)                                                              \
  {                                                                     \
    d = *s1 - *s2;                                                      \
    if (d) actionD;                                                     \
    s1--;                                                               \
    if (s1 == lb) break;                                                \
    s2--;                                                               \
    d = *s1 - *s2;                                                      \
    if (d) actionD;                                                     \
    s1--;                                                               \
    s2--;                                                               \
  }                                                                     \
                                                                        \
  d = *s1 - *(s2 - 1);                                                  \
  if (d)                                                                \
  {                                                                     \
    if (((long) (pGetComp(p1) - pGetComp(p2))) == d)                    \
      d = -d;                                                           \
    actionD;                                                            \
  }                                                                     \
  actionE;                                                              \
}                                                                       \
while(0)
  
#define _pMonComp_otEXPCOMP_nwODD(p1, p2, length, d, actionD, actionE)  \
do                                                                      \
{                                                                       \
  const long* s1 = ((long*) p1) + pMonomSizeW-1;                        \
  const long* s2 = ((long*) p2)  + pMonomSizeW-1;                       \
  const long* const lb = s1 - length +1;                                \
                                                                        \
  for (;;)                                                              \
  {                                                                     \
    d = *s1 - *s2;                                                      \
    if (d) actionD;                                                     \
    s1--;                                                               \
    s2--;                                                               \
    d = *s1 - *s2;                                                      \
    if (d) actionD;                                                     \
    s1--;                                                               \
    if (s1 == lb) break;                                                \
    s2--;                                                               \
  }                                                                     \
                                                                        \
  d = *s1 - *(s2 - 1);                                                  \
  if (d)                                                                \
  {                                                                     \
    if (((long) (pGetComp(p1) - pGetComp(p2))) == d)                    \
      d = -d;                                                           \
    actionD;                                                            \
  }                                                                     \
  actionE;                                                              \
}                                                                       \
while(0)

#define _pMonComp_otEXP_nwGEN(p1, p2, length, d, actionD, actionE)  \
do                                                                  \
{                                                                   \
  const long* s1 = ((long*) p1) + pMonomSizeW-1;                    \
  const long* s2 = ((long*) p2)  + pMonomSizeW-1;                   \
  const long* const lb = s1 - length;                               \
                                                                    \
  for (;;)                                                          \
  {                                                                 \
    d = *s1 - *s2;                                                  \
    if (d) actionD;                                                 \
    s1--;                                                           \
    if (s1 == lb) actionE;                                          \
    s2--;                                                           \
  }                                                                 \
}                                                                   \
while(0)
  
#define _pMonComp_otEXPCOMP_nwGEN(p1, p2, length, d, actionD, actionE) \
do                                                                      \
{                                                                       \
  const long* s1 = ((long*) p1) + pMonomSizeW-1;                        \
  const long* s2 = ((long*) p2)  + pMonomSizeW-1;                       \
  const long* const lb = s1 - length +1;                                \
                                                                        \
  for (;;)                                                              \
  {                                                                     \
    d = *s1 - *s2;                                                      \
    if (d) actionD;                                                     \
    s1--;                                                               \
    if (s1 == lb) break;                                                \
    s2--;                                                               \
  }                                                                     \
                                                                        \
  d = *s1 - *(s2 - 1);                                                  \
  if (d)                                                                \
  {                                                                     \
    if (((long) (pGetComp(p1) - pGetComp(p2))) == d)                    \
      d = -d;                                                           \
    actionD;                                                            \
  }                                                                     \
  actionE;                                                              \
}                                                                       \
while(0)

#endif // WORDS_BIGENDIAN

#endif // POLYS_COMP_H

