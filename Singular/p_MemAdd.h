/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/***************************************************************
 *  File:    p_MemAdd.h
 *  Purpose: macros for memory addition
 *  Author:  obachman (Olaf Bachmann)
 *  Created: 8/00
 *  Version: $Id: p_MemAdd.h,v 1.1 2000-09-04 14:06:58 obachman Exp $
 *******************************************************************/
#ifndef P_MEM_ADD_H
#define P_MEM_ADD_H

#define _p_MemAdd(i, r, s1, s2) r[i] = s1[i] + s2[i]

#define _p_MemAdd_LengthOne(r, s1, s2) _p_MemAdd(0, r, s1, s2)
#define _p_MemAdd_LengthTwo(r, s1, s2) do{_p_MemAdd_LengthOne(r, s1, s2); _p_MemAdd(1, r, s1, s2);} while(0)
#define _p_MemAdd_LengthThree(r, s1, s2) do{_p_MemAdd_LengthTwo(r, s1, s2); _p_MemAdd(2, r, s1, s2);} while(0)
#define _p_MemAdd_LengthFour(r, s1, s2) do{_p_MemAdd_LengthThree(r, s1, s2); _p_MemAdd(3, r, s1, s2);} while(0)
#define _p_MemAdd_LengthFive(r, s1, s2) do{_p_MemAdd_LengthFour(r, s1, s2); _p_MemAdd(4, r, s1, s2);} while(0)
#define _p_MemAdd_LengthSix(r, s1, s2) do{_p_MemAdd_LengthFive(r, s1, s2); _p_MemAdd(5, r, s1, s2);} while(0)
#define _p_MemAdd_LengthSeven(r, s1, s2) do{_p_MemAdd_LengthSix(r, s1, s2); _p_MemAdd(6, r, s1, s2);} while(0)
#define _p_MemAdd_LengthEight(r, s1, s2) do{_p_MemAdd_LengthSeven(r, s1, s2); _p_MemAdd(7, r, s1, s2);} while(0)

#define _p_MemAdd_Declare(r, s1, s2)                \
  const unsigned long* _s1 = ((unsigned long*) s1); \
  const unsigned long* _s2 = ((unsigned long*) s2); \
  unsigned long* _r = ((unsigned long*) r)

#define p_MemAdd_LengthOne(r, s1, s2, length) _p_MemAdd_LengthOne(r, s1, s2)
#define p_MemAdd_LengthTwo(r, s1, s2, length)   \
do                                              \
{                                               \
  _p_MemAdd_Declare(r,s1,s2);                    \
  _p_MemAdd_LengthTwo(_r, _s1, _s2);            \
}                                               \
while (0)
#define p_MemAdd_LengthThree(r, s1, s2, length) \
do                                              \
{                                               \
  _p_MemAdd_Declare(r,s1,s2);                    \
  _p_MemAdd_LengthThree(_r, _s1, _s2);          \
}                                               \
while (0)
#define p_MemAdd_LengthFour(r, s1, s2, length)  \
do                                              \
{                                               \
  _p_MemAdd_Declare(r,s1,s2);                    \
  _p_MemAdd_LengthFour(_r, _s1, _s2);           \
}                                               \
while (0)
#define p_MemAdd_LengthFive(r, s1, s2, length)  \
do                                              \
{                                               \
  _p_MemAdd_Declare(r,s1,s2);                    \
  _p_MemAdd_LengthFive(_r, _s1, _s2);           \
}                                               \
while (0)
#define p_MemAdd_LengthSix(r, s1, s2, length)   \
do                                              \
{                                               \
  _p_MemAdd_Declare(r,s1,s2);                    \
  _p_MemAdd_LengthSix(_r, _s1, _s2);            \
}                                               \
while (0)
#define p_MemAdd_LengthSeven(r, s1, s2, length) \
do                                              \
{                                               \
  _p_MemAdd_Declare(r,s1,s2);                    \
  _p_MemAdd_LengthSeven(_r, _s1, _s2);          \
}                                               \
while (0)
#define p_MemAdd_LengthEight(r, s1, s2, length) \
do                                              \
{                                               \
  _p_MemAdd_Declare(r,s1,s2);                    \
  _p_MemAdd_LengthEight(_r, _s1, _s2);          \
}                                               \
while (0)

#define p_MemAdd_LengthGeneral(r, s1, s2, length)   \
do                                                  \
{                                                   \
  _p_MemAdd_Declare(r,s1,s2);                       \
  const unsigned long _l = (unsigned long) length;       \
  unsigned long _i = 0;                             \
                                                    \
  do                                                \
  {                                                 \
    _r[_i] = _s1[_i] + _s2[_i];                     \
    _i++;                                           \
  }                                                 \
  while (_i != _l);                                 \
}                                                   \
while (0)


/* Incrementing memory */

#define _p_MemIncr(i, r, s) r[i] += s[i]

#define _p_MemIncr_LengthOne(r, s) _p_MemIncr(0, r, s)
#define _p_MemIncr_LengthTwo(r, s) do{_p_MemIncr_LengthOne(r, s); _p_MemIncr(1, r, s);} while(0)
#define _p_MemIncr_LengthThree(r, s) do{_p_MemIncr_LengthTwo(r, s); _p_MemIncr(2, r, s);} while(0)
#define _p_MemIncr_LengthFour(r, s) do{_p_MemIncr_LengthThree(r, s); _p_MemIncr(3, r, s);} while(0)
#define _p_MemIncr_LengthFive(r, s) do{_p_MemIncr_LengthFour(r, s); _p_MemIncr(4, r, s);} while(0)
#define _p_MemIncr_LengthSix(r, s) do{_p_MemIncr_LengthFive(r, s); _p_MemIncr(5, r, s);} while(0)
#define _p_MemIncr_LengthSeven(r, s) do{_p_MemIncr_LengthSix(r, s); _p_MemIncr(6, r, s);} while(0)
#define _p_MemIncr_LengthEight(r, s) do{_p_MemIncr_LengthSeven(r, s); _p_MemIncr(7, r, s);} while(0)

#define _p_MemIncr_Declare(r, s)                \
  const unsigned long* _s = ((unsigned long*) s); \
  unsigned long* _r = ((unsigned long*) r)

#define p_MemIncr_LengthOne(r, s, length) _p_MemIncr_LengthOne(r, s)
#define p_MemIncr_LengthTwo(r, s, length)   \
do                                              \
{                                               \
  _p_MemIncr_Declare(r,s);                    \
  _p_MemIncr_LengthTwo(_r, _s);            \
}                                               \
while (0)
#define p_MemIncr_LengthThree(r, s, length) \
do                                              \
{                                               \
  _p_MemIncr_Declare(r,s);                    \
  _p_MemIncr_LengthThree(_r, _s);          \
}                                               \
while (0)
#define p_MemIncr_LengthFour(r, s, length)  \
do                                              \
{                                               \
  _p_MemIncr_Declare(r,s);                    \
  _p_MemIncr_LengthFour(_r, _s);           \
}                                               \
while (0)
#define p_MemIncr_LengthFive(r, s, length)  \
do                                              \
{                                               \
  _p_MemIncr_Declare(r,s);                    \
  _p_MemIncr_LengthFive(_r, _s);           \
}                                               \
while (0)
#define p_MemIncr_LengthSix(r, s, length)   \
do                                              \
{                                               \
  _p_MemIncr_Declare(r,s);                    \
  _p_MemIncr_LengthSix(_r, _s);            \
}                                               \
while (0)
#define p_MemIncr_LengthSeven(r, s, length) \
do                                              \
{                                               \
  _p_MemIncr_Declare(r,s);                    \
  _p_MemIncr_LengthSeven(_r, _s);          \
}                                               \
while (0)
#define p_MemIncr_LengthEight(r, s, length) \
do                                              \
{                                               \
  _p_MemIncr_Declare(r,s);                    \
  _p_MemIncr_LengthEight(_r, _s);          \
}                                               \
while (0)

#define p_MemIncr_LengthGeneral(r, s, length)       \
do                                                  \
{                                                   \
  _p_MemIncr_Declare(r,s);                          \
  const unsigned long _l = (unsigned long) length;  \
  unsigned long _i = 0;                             \
                                                    \
  do                                                \
  {                                                 \
    _r[_i] += _s[_i];                               \
    _i++;                                           \
  }                                                 \
  while (_i != _l);                                 \
}                                                   \
while (0)

#endif P_MEM_ADD_H
