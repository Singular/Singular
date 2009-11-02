/*******************************************************************
 *  File:    omMemOps.h
 *  Purpose: low-level Macros for memory operations
 *  Author:  obachman (Olaf Bachmann)
 *  Created: 11/99
 *  Version: $Id$
 *******************************************************************/

#ifndef OM_MEM_OPS_H
#define OM_MEM_OPS_H

#ifdef DO_DEEP_PROFILE
extern void _omMemcpyW(long* p1, long* p2, long l);
#define omMemcpy_nwEVEN(p1, p2, l)    _omMemcpyW((long*) p1, (long*) p2, (long) l)
#define omMemcpy_nwODD(p1, p2, l)     _omMemcpyW((long*) p1, (long*) p2, (long) l)
#define omMemcpyW(p1, p2, l)          _omMemcpyW((long*) p1, (long*) p2, (long) l)

extern void _omMemaddW(long* p1, long* p2, long* p3, long l);
#define omMemaddW(p1, p2, p3, l)          _omMemaddW(p1, p2, p3, l)
#define omMemadd_nwODD(p1, p2, p3, l)     _omMemaddW(p1, p2, p3, l)
#define omMemadd_nwEVEN(p1, p2, p3, l)    _omMemaddW(p1, p2, p3, l)
#define omMemadd_nwONE(p1, p2, p3)        _omMemaddW(p1, p2, p3, 1)
#define omMemadd_nwTWO(p1, p2, p3)        _omMemaddW(p1, p2, p3, 2)

extern void _omMemsetW(long* p1, long w, long l);
#define omMemsetW(p1, w, l) _omMemsetW(p1, w, l)

#else /* ! DO_DEEP_PROFILE */

#define omMemcpyW(p1, p2, l)                    \
do                                              \
{                                               \
  long _i = l;                                  \
  long* _s1 = (long*) (p1);                       \
  const long* _s2 = (long*) (p2);                 \
                                                \
  for (;;)                                      \
  {                                             \
    *_s1 = *_s2;                                \
    _i--;                                       \
    if (_i == 0) break;                         \
    _s1++;                                      \
    _s2++;                                      \
  }                                             \
}                                               \
while(0)

#define omMemcpy_nwODD(p1, p2, l)               \
do                                              \
{                                               \
  long _i = (l) - 1;                              \
  long* _s1 = (long*) (p1);                       \
  const long* _s2 = (long*) (p2);                 \
                                                \
  *_s1++ = *_s2++;                              \
  for (;;)                                      \
  {                                             \
    *_s1++ = *_s2++;                            \
    *_s1++ = *_s2++;                            \
    _i -= 2;                                    \
    if (_i == 0) break;                         \
  }                                             \
}                                               \
while(0)

#define omMemcpy_nwEVEN(p1, p2, l)              \
do                                              \
{                                               \
  long _i = l;                                  \
  long* _s1 = (long*) (p1);                       \
  const long* _s2 = (long*) (p2);                 \
                                                \
  for (;;)                                      \
  {                                             \
    *_s1++ = *_s2++;                            \
    *_s1++ = *_s2++;                            \
    _i -= 2;                                    \
    if (_i == 0) break;                         \
  }                                             \
}                                               \
while(0)

#define omMemaddW(P1, P2, P3, L)                \
do                                              \
{                                               \
  unsigned long* _p1 = P1;                      \
  const unsigned long* _p2 = P2;                \
  const unsigned long* _p3 = P3;                \
  unsigned long l = L;                          \
                                                \
  do                                            \
  {                                             \
    *_p1++ = *_p2++ + *_p3++;                   \
    l--;                                        \
  }                                             \
  while(l);                                     \
}                                               \
while(0)

#define omMemadd_nwODD(P1, P2, P3, L)           \
do                                              \
{                                               \
  unsigned long* _p1 = P1;                      \
  const unsigned long* _p2 = P2;                \
  const unsigned long* _p3 = P3;                \
  unsigned long l = L;                          \
                                                \
 *_p1++ = *_p2++ + *_p3++;                      \
  l--;                                          \
                                                \
  do                                            \
  {                                             \
     *_p1++ = *_p2++ + *_p3++;                  \
     *_p1++ = *_p2++ + *_p3++;                  \
     l -=2;                                     \
  }                                             \
  while(l);                                     \
}                                               \
while(0)

#define omMemadd_nwEVEN(P1, P2, P3, L)          \
do                                              \
{                                               \
  unsigned long* _p1 = P1;                      \
  const unsigned long* _p2 = P2;                \
  const unsigned long* _p3 = P3;                \
  unsigned long l = L;                          \
                                                \
  do                                            \
  {                                             \
     *_p1++ = *_p2++ + *_p3++;                  \
     *_p1++ = *_p2++ + *_p3++;                  \
     l -=2;                                     \
  }                                             \
  while(l);                                     \
}                                               \
while(0)

#define omMemadd_nwONE(P1, P2, P3)              \
do                                              \
{                                               \
  unsigned long* _p1 = P1;                      \
  const unsigned long* _p2 = P2;                \
  const unsigned long* _p3 = P3;                \
                                                \
 *_p1 = *_p2 + *_p3;                            \
}                                               \
while(0)

#define omMemadd_nwTWO(P1, P2, P3)              \
do                                              \
{                                               \
  unsigned long* _p1 = P1;                      \
  const unsigned long* _p2 = P2;                \
  const unsigned long* _p3 = P3;                \
                                                \
 *_p1++ = *_p2++ + *_p3++;                      \
 *_p1 = *_p2 + *_p3;                            \
}                                               \
while(0)

#define omMemsetW(P1, W, L)                     \
do                                              \
{                                               \
  long* _p1 = (long*) (P1);                     \
  unsigned long _l = L;                         \
  unsigned long _w = W;                         \
  while(_l)                                     \
  {                                             \
    *_p1++ = _w;                                \
    _l--;                                       \
  }                                             \
}                                               \
while(0)

#endif /* DO_DEEP_PROFILE */

#endif /* OM_LIST_H */
