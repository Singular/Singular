#ifndef POLYS_COMP_H
#define POLYS_COMP_H
/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/* $Id: polys-comp.h,v 1.12 1999-09-29 10:59:35 obachman Exp $ */

/***************************************************************
 *
 * File:       polys-comp.h
 * Purpose:    low-level monomial comparison routines
 *
 ***************************************************************/

#include "polys-impl.h"
#include "syz.h"

#ifdef WORDS_BIGENDIAN

#if 1

#define _pMonComp(p1, p2, d, actionD, actionE)                      \
do                                                                  \
{                                                                   \
  long _i = currRing->pCompLSize - 1;                               \
  register const long* s1 = &(p1->exp.l[currRing->pCompLowIndex]);  \
  register const long* s2 = &(p2->exp.l[currRing->pCompLowIndex]);  \
                                                                    \
  for (;;)                                                          \
  {                                                                 \
    d = *s1++ - *s2++;                                              \
    if (d)                                                          \
    {                                                               \
      d ^= currRing->ordsgn[_i];                                    \
      actionD;                                                      \
    }                                                               \
    if (_i == 0) actionE;                                           \
    _i--;                                                           \
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
  const long* const lb = s1 + length -1;                                \
                                                                        \
  for (;;)                                                              \
  {                                                                     \
    d = *s1 - *s2;                                                      \
    if (s1 == lb) break;                                                \
    if (d) actionD;                                                     \
    s1++;                                                               \
    s2++;                                                               \
  }                                                                     \
                                                                        \
  if (d)                                                                \
  {                                                                     \
    if (((long) (pGetComp(p1) - pGetComp(p2))) == d)                    \
      d = -d;                                                           \
    actionD;                                                            \
  }                                                                     \
  actionE;                                                              \
}                                                                       \
while(0)


#define _pMonComp_otSYZDPC_nwONE(p1, p2, d, actionD, actionE)   \
do                                                              \
{                                                               \
  d = *((long*) &(p2->exp[0])) - *((long*) &(p1->exp[0]));      \
  if (d)                                                        \
  {                                                             \
    const long c1 = pGetComp(p1);                               \
    const long c2 = pGetComp(p2);                               \
    if (c2 - c1 == d)                                           \
    {                                                           \
      d = currcomponents[c1] - currcomponents[c2];              \
    }                                                           \
    actionD;                                                    \
  }                                                             \
  actionE;                                                      \
}                                                               \
while(0)
#endif

#define _pMonComp_otSYZDPC_nwTWO(p1, p2, d, actionD, actionE)   \
do                                                              \
{                                                               \
  const long* s1 = (long*) &(p1->exp[0]);                       \
  const long* s2 = (long*) &(p2->exp[0]);                       \
  d = *s2 - *s1;                                                \
  if (d) actionD;                                               \
  d = *(s2 + 1) - *(s1 + 1);                                    \
  if (d)                                                        \
  {                                                             \
    const long c1 = pGetComp(p1);                               \
    const long c2 = pGetComp(p2);                               \
    if (c2 - c1 == d)                                           \
    {                                                           \
      d = currcomponents[c1] - currcomponents[c2];              \
    }                                                           \
    actionD;                                                    \
  }                                                             \
  actionE;                                                      \
}                                                               \
while(0)

#define _pMonComp_otSYZDPC_nwEVEN(p1, p2, length, d, actionD, actionE)  \
do                                                                      \
{                                                                       \
  const long* s1 = (long*) &(p1->exp[0]);                               \
  const long* s2 = (long*) &(p2->exp[0]);                               \
  const long* const lb = s1 + length-1;                                 \
                                                                        \
  for (;;)                                                              \
  {                                                                     \
    d = *s2 - *s1;                                                      \
    if (d) actionD;                                                     \
    s1++;                                                               \
    if (s1 == lb) break;                                                \
    s2++;                                                               \
    d = *s2 - *s1;                                                      \
    if (d) actionD;                                                     \
    s1++;                                                               \
    s2++;                                                               \
  }                                                                     \
                                                                        \
  d = *(s2 + 1) - *s1;                                                  \
  if (d)                                                                \
  {                                                                     \
    const long c1 = pGetComp(p1);                                       \
    const long c2 = pGetComp(p2);                                       \
    if (c2 - c1 == d)                                                   \
    {                                                                   \
      d = currcomponents[c1] - currcomponents[c2];                      \
    }                                                                   \
    actionD;                                                            \
  }                                                                     \
  actionE;                                                              \
}                                                                       \
while(0)

#define _pMonComp_otSYZDPC_nwODD(p1, p2, length, d, actionD, actionE)   \
do                                                                      \
{                                                                       \
  const long* s1 = (long*) &(p1->exp[0]);                               \
  const long* s2 = (long*) &(p2->exp[0]);                               \
  const long* const lb = s1 + length      -1;                           \
                                                                        \
  for (;;)                                                              \
  {                                                                     \
    d = *s2 - *s1;                                                      \
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
  d = *(s2 + 1) - *s1;                                                  \
  if (d)                                                                \
  {                                                                     \
    const long c1 = pGetComp(p1);                                       \
    const long c2 = pGetComp(p2);                                       \
    if (c2 - c1 == d)                                                   \
    {                                                                   \
      d = currcomponents[c1] - currcomponents[c2];                      \
    }                                                                   \
    actionD;                                                            \
  }                                                                     \
  actionE;                                                              \
}                                                                       \
while(0)

#define _pMonComp_otSYZDPC_nwGEN(p1, p2, length, d, actionD, actionE)   \
do                                                                      \
{                                                                       \
  const long* s1 = (long*) &(p1->exp[0]);                               \
  const long* s2 = (long*) &(p2->exp[0]);                               \
  const long* const lb = s1 + length -1;                                \
                                                                        \
  for (;;)                                                              \
  {                                                                     \
    d = *s2 - *s1;                                                      \
    if (s1 == lb) break;                                                \
    if (d) actionD;                                                     \
    s1++;                                                               \
    s2++;                                                               \
  }                                                                     \
                                                                        \
  if (d)                                                                \
  {                                                                     \
    const long c1 = pGetComp(p1);                                       \
    const long c2 = pGetComp(p2);                                       \
    if (c2 - c1 == d)                                                   \
    {                                                                   \
      d = currcomponents[c1] - currcomponents[c2];                      \
    }                                                                   \
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

#ifndef NDEBUG
#define _pMonComp(p1, p2, d, actionD, actionE)  \
{                                               \
  d = __pMonComp(p1, p2);                       \
  if (d == 0) actionE;                          \
  actionD;                                      \
}

inline long __pMonComp(poly p1, poly p2)
{
  long d;
  unsigned long _i = currRing->pCompLSize - 1;
  register const unsigned long* s1 = &(p1->exp.l[currRing->pCompHighIndex]);
  register const unsigned long* s2 = &(p2->exp.l[currRing->pCompHighIndex]);

  for (;;)
  {
    d = *s1-- - *s2--;
    if (d)
    {
      d ^= currRing->ordsgn[_i];
      if (d == 0) return 1;
      return d;
    }
    if (_i == 0) return d;
    _i--;
  }
}

#else

#define _pMonComp(p1, p2, d, actionD, actionE)                          \
do                                                                      \
{                                                                       \
  long _i = currRing->pCompLSize - 1;                       \
  register const long* s1 = &(p1->exp.l[currRing->pCompHighIndex]);  \
  register const long* s2 = &(p2->exp.l[currRing->pCompHighIndex]);  \
                                                                        \
  for (;;)                                                              \
  {                                                                     \
    d = *s1-- - *s2--;                                                  \
    if (d)                                                              \
    {                                                                   \
      d ^= currRing->ordsgn[_i];                                        \
      actionD;                                                          \
    }                                                                   \
    if (_i == 0) actionE;                                               \
    _i--;                                                               \
  }                                                                     \
}                                                                       \
while(0)
#endif

#endif // WORDS_BIGENDIAN

#endif // POLYS_COMP_H

