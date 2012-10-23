/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/***************************************************************
 *  File:    p_MemAdd.h
 *  Purpose: macros for memory addition
 *  Author:  obachman (Olaf Bachmann)
 *  Created: 8/00
 *******************************************************************/
#ifndef P_MEM_ADD_H
#define P_MEM_ADD_H

/***************************************************************
 *
 * MemSum
 *
 ***************************************************************/

#define _p_MemSum(i, r, s1, s2) r[i] = s1[i] + s2[i]

#define _p_MemSum_LengthOne(r, s1, s2) _p_MemSum(0, r, s1, s2)
#define _p_MemSum_LengthTwo(r, s1, s2) do{_p_MemSum_LengthOne(r, s1, s2); _p_MemSum(1, r, s1, s2);} while(0)
#define _p_MemSum_LengthThree(r, s1, s2) do{_p_MemSum_LengthTwo(r, s1, s2); _p_MemSum(2, r, s1, s2);} while(0)
#define _p_MemSum_LengthFour(r, s1, s2) do{_p_MemSum_LengthThree(r, s1, s2); _p_MemSum(3, r, s1, s2);} while(0)
#define _p_MemSum_LengthFive(r, s1, s2) do{_p_MemSum_LengthFour(r, s1, s2); _p_MemSum(4, r, s1, s2);} while(0)
#define _p_MemSum_LengthSix(r, s1, s2) do{_p_MemSum_LengthFive(r, s1, s2); _p_MemSum(5, r, s1, s2);} while(0)
#define _p_MemSum_LengthSeven(r, s1, s2) do{_p_MemSum_LengthSix(r, s1, s2); _p_MemSum(6, r, s1, s2);} while(0)
#define _p_MemSum_LengthEight(r, s1, s2) do{_p_MemSum_LengthSeven(r, s1, s2); _p_MemSum(7, r, s1, s2);} while(0)

#define _p_MemSum_Declare(r, s1, s2)                \
  const unsigned long* _s1 = ((unsigned long*) s1); \
  const unsigned long* _s2 = ((unsigned long*) s2); \
  unsigned long* _r = ((unsigned long*) r)

#define p_MemSum_LengthOne(r, s1, s2, length) _p_MemSum_LengthOne(r, s1, s2)
#define p_MemSum_LengthTwo(r, s1, s2, length)   \
do                                              \
{                                               \
  _p_MemSum_Declare(r,s1,s2);                    \
  _p_MemSum_LengthTwo(_r, _s1, _s2);            \
}                                               \
while (0)
#define p_MemSum_LengthThree(r, s1, s2, length) \
do                                              \
{                                               \
  _p_MemSum_Declare(r,s1,s2);                    \
  _p_MemSum_LengthThree(_r, _s1, _s2);          \
}                                               \
while (0)
#define p_MemSum_LengthFour(r, s1, s2, length)  \
do                                              \
{                                               \
  _p_MemSum_Declare(r,s1,s2);                    \
  _p_MemSum_LengthFour(_r, _s1, _s2);           \
}                                               \
while (0)
#define p_MemSum_LengthFive(r, s1, s2, length)  \
do                                              \
{                                               \
  _p_MemSum_Declare(r,s1,s2);                    \
  _p_MemSum_LengthFive(_r, _s1, _s2);           \
}                                               \
while (0)
#define p_MemSum_LengthSix(r, s1, s2, length)   \
do                                              \
{                                               \
  _p_MemSum_Declare(r,s1,s2);                    \
  _p_MemSum_LengthSix(_r, _s1, _s2);            \
}                                               \
while (0)
#define p_MemSum_LengthSeven(r, s1, s2, length) \
do                                              \
{                                               \
  _p_MemSum_Declare(r,s1,s2);                    \
  _p_MemSum_LengthSeven(_r, _s1, _s2);          \
}                                               \
while (0)
#define p_MemSum_LengthEight(r, s1, s2, length) \
do                                              \
{                                               \
  _p_MemSum_Declare(r,s1,s2);                    \
  _p_MemSum_LengthEight(_r, _s1, _s2);          \
}                                               \
while (0)

#define p_MemSum_LengthGeneral(r, s1, s2, length)   \
do                                                  \
{                                                   \
  _p_MemSum_Declare(r,s1,s2);                       \
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

/***************************************************************
 *
 * MemAdd
 *
 ***************************************************************/
#define _p_MemAdd(i, r, s) r[i] += s[i]

#define _p_MemAdd_LengthOne(r, s) _p_MemAdd(0, r, s)
#define _p_MemAdd_LengthTwo(r, s) do{_p_MemAdd_LengthOne(r, s); _p_MemAdd(1, r, s);} while(0)
#define _p_MemAdd_LengthThree(r, s) do{_p_MemAdd_LengthTwo(r, s); _p_MemAdd(2, r, s);} while(0)
#define _p_MemAdd_LengthFour(r, s) do{_p_MemAdd_LengthThree(r, s); _p_MemAdd(3, r, s);} while(0)
#define _p_MemAdd_LengthFive(r, s) do{_p_MemAdd_LengthFour(r, s); _p_MemAdd(4, r, s);} while(0)
#define _p_MemAdd_LengthSix(r, s) do{_p_MemAdd_LengthFive(r, s); _p_MemAdd(5, r, s);} while(0)
#define _p_MemAdd_LengthSeven(r, s) do{_p_MemAdd_LengthSix(r, s); _p_MemAdd(6, r, s);} while(0)
#define _p_MemAdd_LengthEight(r, s) do{_p_MemAdd_LengthSeven(r, s); _p_MemAdd(7, r, s);} while(0)

#define _p_MemAdd_Declare(r, s)                \
  const unsigned long* _s = ((unsigned long*) s); \
  unsigned long* _r = ((unsigned long*) r)

#define p_MemAdd_LengthOne(r, s, length) _p_MemAdd_LengthOne(r, s)
#define p_MemAdd_LengthTwo(r, s, length)   \
do                                              \
{                                               \
  _p_MemAdd_Declare(r,s);                    \
  _p_MemAdd_LengthTwo(_r, _s);            \
}                                               \
while (0)
#define p_MemAdd_LengthThree(r, s, length) \
do                                              \
{                                               \
  _p_MemAdd_Declare(r,s);                    \
  _p_MemAdd_LengthThree(_r, _s);          \
}                                               \
while (0)
#define p_MemAdd_LengthFour(r, s, length)  \
do                                              \
{                                               \
  _p_MemAdd_Declare(r,s);                    \
  _p_MemAdd_LengthFour(_r, _s);           \
}                                               \
while (0)
#define p_MemAdd_LengthFive(r, s, length)  \
do                                              \
{                                               \
  _p_MemAdd_Declare(r,s);                    \
  _p_MemAdd_LengthFive(_r, _s);           \
}                                               \
while (0)
#define p_MemAdd_LengthSix(r, s, length)   \
do                                              \
{                                               \
  _p_MemAdd_Declare(r,s);                    \
  _p_MemAdd_LengthSix(_r, _s);            \
}                                               \
while (0)
#define p_MemAdd_LengthSeven(r, s, length) \
do                                              \
{                                               \
  _p_MemAdd_Declare(r,s);                    \
  _p_MemAdd_LengthSeven(_r, _s);          \
}                                               \
while (0)
#define p_MemAdd_LengthEight(r, s, length) \
do                                              \
{                                               \
  _p_MemAdd_Declare(r,s);                    \
  _p_MemAdd_LengthEight(_r, _s);          \
}                                               \
while (0)

#define p_MemAdd_LengthGeneral(r, s, length)       \
do                                                  \
{                                                   \
  _p_MemAdd_Declare(r,s);                          \
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


/***************************************************************
 *
 * MemDiff
 *
 ***************************************************************/
#define _p_MemDiff(i, r, s1, s2) r[i] = s1[i] - s2[i]

#define _p_MemDiff_LengthOne(r, s1, s2) _p_MemDiff(0, r, s1, s2)
#define _p_MemDiff_LengthTwo(r, s1, s2) do{_p_MemDiff_LengthOne(r, s1, s2); _p_MemDiff(1, r, s1, s2);} while(0)
#define _p_MemDiff_LengthThree(r, s1, s2) do{_p_MemDiff_LengthTwo(r, s1, s2); _p_MemDiff(2, r, s1, s2);} while(0)
#define _p_MemDiff_LengthFour(r, s1, s2) do{_p_MemDiff_LengthThree(r, s1, s2); _p_MemDiff(3, r, s1, s2);} while(0)
#define _p_MemDiff_LengthFive(r, s1, s2) do{_p_MemDiff_LengthFour(r, s1, s2); _p_MemDiff(4, r, s1, s2);} while(0)
#define _p_MemDiff_LengthSix(r, s1, s2) do{_p_MemDiff_LengthFive(r, s1, s2); _p_MemDiff(5, r, s1, s2);} while(0)
#define _p_MemDiff_LengthSeven(r, s1, s2) do{_p_MemDiff_LengthSix(r, s1, s2); _p_MemDiff(6, r, s1, s2);} while(0)
#define _p_MemDiff_LengthEight(r, s1, s2) do{_p_MemDiff_LengthSeven(r, s1, s2); _p_MemDiff(7, r, s1, s2);} while(0)

#define _p_MemDiff_Declare(r, s1, s2)                \
  const unsigned long* _s1 = ((unsigned long*) s1); \
  const unsigned long* _s2 = ((unsigned long*) s2); \
  unsigned long* _r = ((unsigned long*) r)

#define p_MemDiff_LengthOne(r, s1, s2, length) _p_MemDiff_LengthOne(r, s1, s2)
#define p_MemDiff_LengthTwo(r, s1, s2, length)   \
do                                              \
{                                               \
  _p_MemDiff_Declare(r,s1,s2);                    \
  _p_MemDiff_LengthTwo(_r, _s1, _s2);            \
}                                               \
while (0)
#define p_MemDiff_LengthThree(r, s1, s2, length) \
do                                              \
{                                               \
  _p_MemDiff_Declare(r,s1,s2);                    \
  _p_MemDiff_LengthThree(_r, _s1, _s2);          \
}                                               \
while (0)
#define p_MemDiff_LengthFour(r, s1, s2, length)  \
do                                              \
{                                               \
  _p_MemDiff_Declare(r,s1,s2);                    \
  _p_MemDiff_LengthFour(_r, _s1, _s2);           \
}                                               \
while (0)
#define p_MemDiff_LengthFive(r, s1, s2, length)  \
do                                              \
{                                               \
  _p_MemDiff_Declare(r,s1,s2);                    \
  _p_MemDiff_LengthFive(_r, _s1, _s2);           \
}                                               \
while (0)
#define p_MemDiff_LengthSix(r, s1, s2, length)   \
do                                              \
{                                               \
  _p_MemDiff_Declare(r,s1,s2);                    \
  _p_MemDiff_LengthSix(_r, _s1, _s2);            \
}                                               \
while (0)
#define p_MemDiff_LengthSeven(r, s1, s2, length) \
do                                              \
{                                               \
  _p_MemDiff_Declare(r,s1,s2);                    \
  _p_MemDiff_LengthSeven(_r, _s1, _s2);          \
}                                               \
while (0)
#define p_MemDiff_LengthEight(r, s1, s2, length) \
do                                              \
{                                               \
  _p_MemDiff_Declare(r,s1,s2);                    \
  _p_MemDiff_LengthEight(_r, _s1, _s2);          \
}                                               \
while (0)

#define p_MemDiff_LengthGeneral(r, s1, s2, length)   \
do                                                  \
{                                                   \
  _p_MemDiff_Declare(r,s1,s2);                       \
  const unsigned long _l = (unsigned long) length;       \
  unsigned long _i = 0;                             \
                                                    \
  do                                                \
  {                                                 \
    _r[_i] = _s1[_i] - _s2[_i];                     \
    _i++;                                           \
  }                                                 \
  while (_i != _l);                                 \
}                                                   \
while (0)


/***************************************************************
 *
 * MemSub
 *
 ***************************************************************/

#define _p_MemSub(i, r, s) r[i] -= s[i]

#define _p_MemSub_LengthOne(r, s) _p_MemSub(0, r, s)
#define _p_MemSub_LengthTwo(r, s) do{_p_MemSub_LengthOne(r, s); _p_MemSub(1, r, s);} while(0)
#define _p_MemSub_LengthThree(r, s) do{_p_MemSub_LengthTwo(r, s); _p_MemSub(2, r, s);} while(0)
#define _p_MemSub_LengthFour(r, s) do{_p_MemSub_LengthThree(r, s); _p_MemSub(3, r, s);} while(0)
#define _p_MemSub_LengthFive(r, s) do{_p_MemSub_LengthFour(r, s); _p_MemSub(4, r, s);} while(0)
#define _p_MemSub_LengthSix(r, s) do{_p_MemSub_LengthFive(r, s); _p_MemSub(5, r, s);} while(0)
#define _p_MemSub_LengthSeven(r, s) do{_p_MemSub_LengthSix(r, s); _p_MemSub(6, r, s);} while(0)
#define _p_MemSub_LengthEight(r, s) do{_p_MemSub_LengthSeven(r, s); _p_MemSub(7, r, s);} while(0)

#define _p_MemSub_Declare(r, s)                \
  const unsigned long* _s = ((unsigned long*) s); \
  unsigned long* _r = ((unsigned long*) r)

#define p_MemSub_LengthOne(r, s, length) _p_MemSub_LengthOne(r, s)
#define p_MemSub_LengthTwo(r, s, length)   \
do                                              \
{                                               \
  _p_MemSub_Declare(r,s);                    \
  _p_MemSub_LengthTwo(_r, _s);            \
}                                               \
while (0)
#define p_MemSub_LengthThree(r, s, length) \
do                                              \
{                                               \
  _p_MemSub_Declare(r,s);                    \
  _p_MemSub_LengthThree(_r, _s);          \
}                                               \
while (0)
#define p_MemSub_LengthFour(r, s, length)  \
do                                              \
{                                               \
  _p_MemSub_Declare(r,s);                    \
  _p_MemSub_LengthFour(_r, _s);           \
}                                               \
while (0)
#define p_MemSub_LengthFive(r, s, length)  \
do                                              \
{                                               \
  _p_MemSub_Declare(r,s);                    \
  _p_MemSub_LengthFive(_r, _s);           \
}                                               \
while (0)
#define p_MemSub_LengthSix(r, s, length)   \
do                                              \
{                                               \
  _p_MemSub_Declare(r,s);                    \
  _p_MemSub_LengthSix(_r, _s);            \
}                                               \
while (0)
#define p_MemSub_LengthSeven(r, s, length) \
do                                              \
{                                               \
  _p_MemSub_Declare(r,s);                    \
  _p_MemSub_LengthSeven(_r, _s);          \
}                                               \
while (0)
#define p_MemSub_LengthEight(r, s, length) \
do                                              \
{                                               \
  _p_MemSub_Declare(r,s);                    \
  _p_MemSub_LengthEight(_r, _s);          \
}                                               \
while (0)

#define p_MemSub_LengthGeneral(r, s, length)       \
do                                                  \
{                                                   \
  _p_MemSub_Declare(r,s);                          \
  const unsigned long _l = (unsigned long) length;  \
  unsigned long _i = 0;                             \
                                                    \
  do                                                \
  {                                                 \
    _r[_i] -= _s[_i];                               \
    _i++;                                           \
  }                                                 \
  while (_i != _l);                                 \
}                                                   \
while (0)

#define _p_MemAddSub_Declare(r, s, t)                \
  const unsigned long* _s = ((unsigned long*) s); \
  const unsigned long* _t = ((unsigned long*) t); \
  unsigned long* _r = ((unsigned long*) r)

#define p_MemAddSub_LengthGeneral(r, s, t, length)  \
do                                                  \
{                                                   \
  _p_MemAddSub_Declare(r,s, t);                     \
  const unsigned long _l = (unsigned long) length;  \
  unsigned long _i = 0;                             \
                                                    \
  do                                                \
  {                                                 \
    _r[_i] += _s[_i] - _t[_i];                      \
    _i++;                                           \
  }                                                 \
  while (_i != _l);                                 \
}                                                   \
while (0)



#if 0
//BOCO: Added for SHIFT DVEC case p_ExpSum functions



/* BOCO: This is our letterplace replacement for p_MemSum__T.  *
 * This function will return rt->exp = p->exp + s * q->exp,    *
 * where s is a shift in size of the number of exponents equal *
 * to on in p->exp . rt has to be allocated (for example with  *
 * p_AllocBin?).                                               */
void LPDV__p_ExpSum_slow
  (poly rt, poly p, poly q, ring r)
{
  p_MemCopy_LengthGeneral(rt->exp, p->exp, r->ExpL_Size);

  int lV = r->isLPring.

  //This represents the first index in the currently considered
  //block in rt->exp.
  long index_rt = p_Totaldegree(p, r) * lV + 1;

  long index_q = 1;
  {
    nextblock: ;
    //We will loop, until we found an empty block, or until we
    //considered all variables
    for(long i = 0; i < lV; ++i)
      if( p_GetExp(q, index_q+i, r) )
      {
        p_SetExp(rt, index_rt+i, 1, r);
        index_rt += lV;
        if(index_rt > r->N) break; //looped through all vars
        index_q += lV; //We found a nonzero exponent, thus
        goto nextblock; //we can move on to the next block
      }
  }

  p_Setm(rt,r);  //TODO: Maybe this is not yet nescessary
  return;
}

/* BOCO: This is our letterplace replacement for p_MemSum__T,  *
 * in case we have a dp-ordering on our blocks.                *
 * This function will return rt->exp = p->exp + s * q->exp,    *
 * where s is a shift in size of the number of exponents equal *
 * to on in p->exp . rt has to be allocated (for example with  *
 * p_AllocBin?).                                               */
void LPDV__p_ExpSum_dp
  (poly rt, poly p, poly q, ring r)
{
  p_MemCopy_LengthGeneral(rt->exp, p->exp, r->ExpL_Size);

  //This represents the first index in the currently considered
  //block in rt->exp.
  //long index_rt = p_Totaldegree(p, r) * lV + 1;
  int lV = r->isLPring.
  long index_rt = p->exp[r->omap[0]] * lV + 1;

  long index_q = 1;
  {
    nextblock: ;
    //We will loop, until we found an empty block, or until we
    //considered all variables
    for(long i = 0; i < lV; ++i)
      if( p_GetExp(q, index_q+i, r) )
      {
        rt->exp[r->omap[index_rt+i]] = 1;
        p_SetExp(rt, index_rt+i, 1, r);
        index_rt += lV;
        if(index_rt > r->N) break; //looped through all vars
        index_q += lV; //We found a nonzero exponent, thus
        goto nextblock; //we can move on to the next block
      }
  }
  rt->exp[r->omap[0]] += q->exp[r->omap[0]];

  return;
}
#endif

#endif /* P_MEM_ADD_H */
