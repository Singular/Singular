#ifndef POLYS_COMP_H
#define POLYS_COMP_H
/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/* $Id: polys-comp.h,v 1.23 2000-08-18 15:42:06 Singular Exp $ */

/***************************************************************
 *
 * File:       polys-comp.h
 * Purpose:    low-level monomial comparison routines
 *
 ***************************************************************/

#include "polys-impl.h"


#ifndef HAVE_SHIFTED_EXPONENTS

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

#define _prMonCmp(p1, p2, r, actionE, actionG, actionS)         \
do                                                              \
{                                                               \
  register const u_s long* s1 = &(p1->exp.l[r->pCompLowIndex]); \
  register const u_s long* s2 = &(p2->exp.l[r->pCompLowIndex]); \
  int _l = r->pCompLSize;                                       \
  register int _i;                                              \
  _memcmp(s1, s2, _i, _l, actionE, goto _NotEqual);             \
                                                                \
  _NotEqual:                                                    \
  if (r->ordsgn[_i] != 1)                                       \
  {                                                             \
    if (s2[_i] > s1[_i]) actionG;                               \
    actionS;                                                    \
  }                                                             \
  if (s1[_i] > s2[_i]) actionG;                                 \
  actionS;                                                      \
}                                                               \
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

#define _prMonCmp(p1, p2, r, actionE, actionG, actionS)             \
do                                                                  \
{                                                                   \
  register const u_s long* __s1 =                                   \
                    (const long *)&(p1->exp.l[r->pCompLowIndex]);   \
  register const u_s long* __s2 =                                   \
                    (const long *)&(p2->exp.l[r->pCompLowIndex]);   \
  register int __i = r->pCompLSize - 1;                             \
  _memcmp(__s1, __s2, __i, actionE, goto _NotEqual);                \
                                                                    \
  _NotEqual:                                                        \
  if (r->ordsgn[__i] != 1)                                          \
  {                                                                 \
    if (__s2[__i] > __s1[__i]) actionG;                             \
    actionS;                                                        \
  }                                                                 \
  if (__s1[__i] > __s2[__i]) actionG;                               \
  actionS;                                                          \
}                                                                   \
while (0)


#endif // WORDS_BIGENDIAN
#else // ndef HAVE_SHIFTED_EXPONENTS

// copied from BIGENDIAN case, with modification: pCompLowIndex=0

// need to undefine this, since longs might be negative
#define u_s

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

#define _prMonCmp(p1, p2, r, actionE, actionG, actionS)   \
do                                                        \
{                                                         \
  register const u_s long* s1 = &(p1->exp.l[0]);          \
  register const u_s long* s2 = &(p2->exp.l[0]);          \
  int _l = r->pCompLSize;                                 \
  register int _i;                                        \
  _memcmp(s1, s2, _i, _l, actionE, goto _NotEqual);       \
                                                          \
  _NotEqual:                                              \
  if (r->ordsgn[_i] != 1)                                 \
  {                                                       \
    if (s2[_i] > s1[_i]) actionG;                         \
    actionS;                                              \
  }                                                       \
  if (s1[_i] > s2[_i]) actionG;                           \
  actionS;                                                \
}                                                         \
while (0)

#endif

#if defined(PDEBUG) && defined(HAVE_SHIFTED_EXPONENTS)
extern int pDBsyzComp;
int rComp0(poly p1, poly p2);
#else
inline int rComp0(poly p1, poly p2)
{
  _prMonCmp(p1, p2, currRing, return 0, return 1, return -1);
}
#endif

inline int prComp0(poly p1, poly p2, ring r)
{
  _prMonCmp(p1, p2, r, return 0, return 1, return -1);
}

#endif // POLYS_COMP_H

