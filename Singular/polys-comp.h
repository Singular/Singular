#ifndef POLYS_COMP_H
#define POLYS_COMP_H
/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/* $Id: polys-comp.h,v 1.17 1999-10-22 11:14:16 obachman Exp $ */

/***************************************************************
 *
 * File:       polys-comp.h
 * Purpose:    low-level monomial comparison routines
 *
 ***************************************************************/

#include "polys-impl.h"

// need to undefine this, since longs might be negative
#define u_s

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
  register const u_s long* s1 = &(p1->exp.l[currRing->pCompLowIndex]); \
  register const u_s long* s2 = &(p2->exp.l[currRing->pCompLowIndex]); \
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
#define register

#define _pMonCmp(p1, p2, actionE, actionG, actionS)                           \
do                                                                            \
{                                                                             \
  register const u_s long* __s1 = &(p1->exp.l[currRing->pCompLowIndex]); \
  register const u_s long* __s2 = &(p2->exp.l[currRing->pCompLowIndex]); \
  register int __i = currRing->pCompLSize - 1;                                \
  _memcmp(__s1, __s2, __i, actionE, goto _NotEqual);                          \
                                                                              \
  _NotEqual:                                                                  \
  if (currRing->ordsgn[__i] != 1)                                             \
  {                                                                           \
    if (__s2[__i] > __s1[__i]) actionG;                                       \
    actionS;                                                                  \
  }                                                                           \
  if (__s1[__i] > __s2[__i]) actionG;                                         \
  actionS;                                                                    \
}                                                                             \
while (0)


#endif // WORDS_BIGENDIAN

inline int rComp0(poly p1, poly p2)
{
  _pMonCmp(p1, p2, return 0, return 1, return -1);
}

#endif // POLYS_COMP_H

