/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/***************************************************************
 *  File:    p_MemCopy.h
 *  Purpose: macros for memory addition
 *  Author:  obachman (Olaf Bachmann)
 *  Created: 8/00
 *  Version: $Id: p_MemCopy.h,v 1.1.1.1 2003-10-06 12:15:59 Singular Exp $
 *******************************************************************/
#ifndef P_MEM_COPY_H
#define P_MEM_COPY_H

#define _p_MemCopy(i, d, s) d[i] = s[i]

#define _p_MemCopy_LengthOne(d, s) _p_MemCopy(0, d, s)
#define _p_MemCopy_LengthTwo(d, s) do{_p_MemCopy_LengthOne(d, s); _p_MemCopy(1, d, s);} while(0)
#define _p_MemCopy_LengthThree(d, s) do{_p_MemCopy_LengthTwo(d, s); _p_MemCopy(2, d, s);} while(0)
#define _p_MemCopy_LengthFour(d, s) do{_p_MemCopy_LengthThree(d, s); _p_MemCopy(3, d, s);} while(0)
#define _p_MemCopy_LengthFive(d, s) do{_p_MemCopy_LengthFour(d, s); _p_MemCopy(4, d, s);} while(0)
#define _p_MemCopy_LengthSix(d, s) do{_p_MemCopy_LengthFive(d, s); _p_MemCopy(5, d, s);} while(0)
#define _p_MemCopy_LengthSeven(d, s) do{_p_MemCopy_LengthSix(d, s); _p_MemCopy(6, d, s);} while(0)
#define _p_MemCopy_LengthEight(d, s) do{_p_MemCopy_LengthSeven(d, s); _p_MemCopy(7, d, s);} while(0)

#define _p_MemCopy_Declare(d, s)                \
  unsigned long* _d = ((unsigned long*) d); \
  const unsigned long* _s = ((unsigned long*) s)

#define p_MemCopy_LengthOne(d, s, length) _p_MemCopy_LengthOne(d, s)
#define p_MemCopy_LengthTwo(d, s, length)   \
do                                              \
{                                               \
  _p_MemCopy_Declare(d,s);                    \
  _p_MemCopy_LengthTwo(_d, _s);            \
}                                               \
while (0)
#define p_MemCopy_LengthThree(d, s, length) \
do                                              \
{                                               \
  _p_MemCopy_Declare(d,s);                    \
  _p_MemCopy_LengthThree(_d, _s);          \
}                                               \
while (0)
#define p_MemCopy_LengthFour(d, s, length)  \
do                                              \
{                                               \
  _p_MemCopy_Declare(d,s);                    \
  _p_MemCopy_LengthFour(_d, _s);           \
}                                               \
while (0)
#define p_MemCopy_LengthFive(d, s, length)  \
do                                              \
{                                               \
  _p_MemCopy_Declare(d,s);                    \
  _p_MemCopy_LengthFive(_d, _s);           \
}                                               \
while (0)
#define p_MemCopy_LengthSix(d, s, length)   \
do                                              \
{                                               \
  _p_MemCopy_Declare(d,s);                    \
  _p_MemCopy_LengthSix(_d, _s);            \
}                                               \
while (0)
#define p_MemCopy_LengthSeven(d, s, length) \
do                                              \
{                                               \
  _p_MemCopy_Declare(d,s);                    \
  _p_MemCopy_LengthSeven(_d, _s);          \
}                                               \
while (0)
#define p_MemCopy_LengthEight(d, s, length) \
do                                              \
{                                               \
  _p_MemCopy_Declare(d,s);                    \
  _p_MemCopy_LengthEight(_d, _s);          \
}                                               \
while (0)

#define p_MemCopy_LengthGeneral(d, s, length)   \
do                                              \
{                                               \
  _p_MemCopy_Declare(d,s);                      \
  const unsigned long _l = (unsigned long) length;   \
  unsigned long _i = 0;                         \
                                                \
  do                                            \
  {                                             \
    _d[_i] = _s[_i];                            \
    _i++;                                       \
  }                                             \
  while (_i != _l);                             \
}                                               \
while (0)

#endif // P_MEM_COPY_H
