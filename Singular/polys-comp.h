#ifndef POLYS_COMP_H
#define POLYS_COMP_H
/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/* $Id: polys-comp.h,v 1.13 1999-10-01 16:24:38 obachman Exp $ */

/***************************************************************
 *
 * File:       polys-comp.h
 * Purpose:    low-level monomial comparison routines
 *
 ***************************************************************/

#include "polys-impl.h"
#include "syz.h"

#ifdef WORDS_BIGENDIAN

#define _memcmp(p1, p2, i, l, actionE, actionD) \
do                                              \
{                                               \
  i = 0;                                        \
  while (i < l)                                 \
  {                                             \
    if (p1[i] != p2[i]) actionD;                \
    i++;                                        \
  }                                             \
  actionE;                                      \
}                                               \
while (0)

#define _pMonCmp(p1, p2, actionE, actionG, actionS)                         \
do                                                                          \
{                                                                           \
  register const unsigned long* s1 = &(p1->exp.l[currRing->pCompLowIndex]); \
  register const unsigned long* s2 = &(p2->exp.l[currRing->pCompLowIndex]); \
  int _l = currRing->pCompLSize;                                            \
  register int _i;                                                          \
  _memcmp(s1, s2, _i, _l, actionE, goto _NotEqual);                         \
                                                                            \
  _NotEqual:                                                                \
  if (currRing->ordsgn[_i] != 1)                                            \
  {                                                                         \
    if (s2[_i] > s1[_i]) actionG;                                           \
    actionS;                                                                \
  }                                                                         \
  if (s1[_i] > s2[_i]) actionG;                                             \
  actionS;                                                                  \
}                                                                           \
while (0)

#else //  ! WORDS_BIGENDIAN


#define _memcmp(p1, p2, i, actionE, actionD)    \
do                                              \
{                                               \
  for (;;)                                      \
  {                                             \
    if (p1[i] != p2[i]) actionD;                \
    if (i == 0) actionE;                        \
    i--;                                        \
  }                                             \
}                                               \
while (0)

#define _pMonCmp(p1, p2, actionE, actionG, actionS)                          \
do                                                                           \
{                                                                            \
  register const unsigned long* s1 = &(p1->exp.l[currRing->pCompLowIndex]); \
  register const unsigned long* s2 = &(p2->exp.l[currRing->pCompLowIndex]); \
  register int _i = currRing->pCompLSize - 1;                                \
  _memcmp(s1, s2, _i, actionE, goto _NotEqual);                              \
                                                                             \
  _NotEqual:                                                                 \
  if (currRing->ordsgn[_i] != 1)                                             \
  {                                                                          \
    if (s2[_i] > s1[_i]) actionG;                                            \
    actionS;                                                                 \
  }                                                                          \
  if (s1[_i] > s2[_i]) actionG;                                              \
  actionS;                                                                   \
}                                                                            \
while (0)


#endif // WORDS_BIGENDIAN

#endif // POLYS_COMP_H

