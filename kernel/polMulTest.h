#include <polys/monomials/ring.h>
#include <kernel/polys.h>
#include <kernel/shiftgb.h>

poly p_LPshift(poly p, int sh, int uptodeg, int lV,const ring r);
poly p_Shrink(poly p, int lV, const ring r);

inline poly pp_Mult_mm_normal
  ( poly p, const ring r_lm_p, const ring r_t_p,
    poly m, const ring r_m, int uptodeg, int lV, 
    uint* mDVec, uint dvSize );

inline poly lp_pp_Mult_mm_shift_n_shrink
  ( poly p, const ring r_lm_p, const ring r_t_p,
    poly m, const ring r_m, int uptodeg, int lV, 
    uint* mDVec, uint dvSize );

inline poly lp_pp_Mult_mm_v0
  ( poly p, const ring r_lm_p, const ring r_t_p,
    poly m, const ring r_m, int uptodeg, int lV, 
    uint* mDVec, uint dvSize );

inline poly lp_pp_Mult_mm_v1
  ( poly p, const ring r_lm_p, const ring r_t_p,
    poly m, const ring r_m, int uptodeg, int lV, 
    uint* mDVec, uint dvSize );

inline poly lp_pp_Mult_mm_v1_a
  ( poly p, const ring r_lm_p, const ring r_t_p,
    poly m, const ring r_m, int uptodeg, int lV, 
    uint* mDVec, uint dvSize );

inline poly lp_pp_Mult_mm_v2
  ( poly p, const ring r_lm_p, const ring r_t_p,
    poly m, const ring r_m, int uptodeg, int lV, 
    uint* mDVec, uint dvSize );

inline poly lp_pp_Mult_mm_v2_a
  ( poly p, const ring r_lm_p, const ring r_t_p,
    poly m, const ring r_m, int uptodeg, int lV, 
    uint* mDVec, uint dvSize );

inline poly lp_pp_Mult_mm_v2_a_with_mDVec
  ( poly p, const ring r_lm_p, const ring r_t_p,
    poly m, const ring r_m, int uptodeg, int lV, 
    uint* mDVec, uint dvSize );

struct letterplace_multiplication_functions
{
  poly (*pp_Mult_mm[8])
    ( poly, const ring, const ring, poly, const ring, 
      int uptodeg, int lV, uint* mDVec, uint dvSize   );
  int size;
   
}; 

struct letterplace_multiplication_functions lpMultFunctions = { { &pp_Mult_mm_normal, &lp_pp_Mult_mm_shift_n_shrink, &lp_pp_Mult_mm_v0, &lp_pp_Mult_mm_v1, &lp_pp_Mult_mm_v2, &lp_pp_Mult_mm_v1_a, &lp_pp_Mult_mm_v2_a, &lp_pp_Mult_mm_v2_a_with_mDVec }, 8, };

/*
struct letterplace_multiplication_functions lpMultFunctions = 
{ { &pp_Mult_mm_normal, 
    &lp_pp_Mult_mm_shift_n_shrink, 
    &lp_pp_Mult_mm_v0, 
    &lp_pp_Mult_mm_v1, 
    &lp_pp_Mult_mm_v2, 
    &lp_pp_Mult_mm_v1_a, 
    &lp_pp_Mult_mm_v2_a, 
    &lp_pp_Mult_mm_v2_a_with_mDVec }, 8, };
*/

//Note to me: How to use lists
#if 2>3
//lists is a pointer to slists class (Singular/lists.h):
//typedef slists *           lists;
lists L;
L->nr; //Number of Elements minus 1
L->m[i].rtyp = INT_CMD;
L->m[i].data = (void*) intPtr;
//Append, Insert ... functions exist, see Singular/lists.h

/* other notes
lpMult for debugging multiplications in kstd2.cc
lpMultProfiler for profiling of our multiplications in kstd2.cc
lpMultProfiler has to be tested and debugged !!!

in extra.cc:
pMultM
lpMultTest
*/
#endif


typedef unsigned int uint;

#if 0
inline uint CreateDVec
  (poly p, ring r, uint*& dvec)
{
  if(p == NULL){dvec = NULL; return 0;}

  //We should test here, if p is in r. (see sTObject::Set ?)

  //BOCO: TODO: to calculate the total degree of a polynomial
  //each time, we need it seems to be inefficient; if we would
  //store it for each monmial, and if we would update it for
  //each time we multiply, we would have fast access to it!!!
  uint dvSize = p_Totaldegree(p, r);
  assume(dvSize < 1000);
  if(!dvSize){dvec = NULL; return 0;}
  dvec = (uint *)omAlloc(dvSize*sizeof(uint));

  uint * it = dvec;

  /* transform lm(p) to shift invariant distance vector
   * j is the distance to the next entry in the block
   * representation of the letterplace monomial lm(p)   
   * "it" is a pointer to the next free entry in our distance
   * vector                                                   
   * TODO: Do that by getting the exponent vector (see mail from
   * H. Schoenemann)
   * REMARK TO MYSELF: ??? But getting the exponent vector isnt
   * better, as it internally uses p_GetExp itself and does even
   * allocate additional space for the exponent vector ??? Or do
   * I miss the point? I will test that some other time.
   */
  for(uint j=1, i=1, l=0; l < dvSize; ++i)
    if(p_GetExp(p,i,r)){*it=j;++it;j=1;++l;} else{++j;}

  return dvSize;
}
#else
namespace ShiftDVec
{ uint CreateDVec(poly p, ring r, uint*& dvec); }
#endif


#if 0 //Neither used, nor tested

//Sets p2[v2] = p1[v1]
//assumes p2[v2] == 0
//TODO:
//May not work, if r1 != r2
//May not work at all
inline void p_CopyExpFromToZero
  ( poly p1, poly p2, 
    const int v1, const int v2, const ring r1, const ring r2 )
{
  pAssume2(r1 == r2);
  p_LmCheckPolyRing2(p1, r);
  p_LmCheckPolyRing2(p2, r);
  pAssume2(v1>0 && v1 <= r1->N);
  pAssume2(v2>0 && v2 <= r2->N);
  pAssume2(r->VarOffset[v1] != -1);
  pAssume2(r->VarOffset[v2] != -1);

  unsigned long iBitmask1 = r1->bitmask;
  //unsigned long iBitmask2 = r2->bitmask;
  int VarOffset1 = r1->VarOffset[v1];
  int VarOffset2 = r2->VarOffset[v2];
  int pos1 = VarOffset1 & 0xffffff;
  int pos2 = VarOffset2 & 0xffffff;
  int shift1 = VarOffset1 >> 24;
  int shift2 = VarOffset2 >> 24;

  pAssume2((p2->exp[pos2] >> (shift2) & iBitmask1) == 0);

  p2->exp[pos2] |= 
    (((p1->exp[pos1] >> shift1) & iBitmask1) << shift2);
}

//Sets p2[v2] = p1[v1]
//assumes p2[v2] == 0
//TODO:
//May not work, if r1 != r2
//May not work at all
inline long p_CopyExpFromToZeroAndReturn
  ( poly p1, poly p2, 
    const int v1, const int v2, const ring r1, const ring r2 )
{
  pAssume2(r1 == r2);
  p_LmCheckPolyRing2(p1, r);
  p_LmCheckPolyRing2(p2, r);
  pAssume2(v1>0 && v1 <= r1->N);
  pAssume2(v2>0 && v2 <= r2->N);
  pAssume2(r->VarOffset[v1] != -1);
  pAssume2(r->VarOffset[v2] != -1);

  unsigned long iBitmask = r1->bitmask;
  int VarOffset1 = r1->VarOffset[v1];
  int VarOffset2 = r2->VarOffset[v2];
  int pos1 = VarOffset1 & 0xffffff;
  int pos2 = VarOffset2 & 0xffffff;
  int shift1 = VarOffset1 >> 24;
  int shift2 = VarOffset2 >> 24;

  pAssume2(((p2->exp[pos2] >> shift2) & iBitmask) == 0);

  long e1 = (p1->exp[pos1] >> shift1) & iBitmask;
  p2->exp[pos2] |= (e1 << shift2);
  return e1;
}

// returns Copy(p)*Copy(m), does neither destroy p nor m Version
// for non-homog, non-shifted letterplace poly-/monomials
// should be from r_m, lm(p) from r_lm_p, tail(p) from r_t_p
// output will have lm in r_lm_p and tail in r_t_p
inline poly lp_mm_Mult_pp_v1
  ( poly m, const ring r_m, 
    poly p, const ring r_lm_p, const ring r_t_p )
{
  //BOCO: TODO: to calculate the total degree of a polynomial
  //each time, we need it seems to be inefficient; if we would
  //store it for each monmial, and if we would update it for
  //each time we multiply, we would have fast access to it!!!
  long degM = p_Totaldegree(m, r_m);

  //first let rt = m * lm(p)
  poly rt;
  if( r_m == r_lm_p )
    rt = p_Head(m, r_lm_p);
  else
  {
    omBin bin = r_lm_p->PolyBin;
    p_AllocBin( rt, bin, r_lm_p);
    for(long i = 0; i < degM; ++i)
      p_SetExp( rt, i, p_GetExp(m, i, r_m), r_lm_p);
  }
  long sumDeg = p_Totaldegree(p, r_lm_p) + degM;
  for(long i = degM, j = 0; i < sumDeg; ++i, ++j)
    p_SetExp( rt, i, p_GetExp(p, j, r_lm_p), r_lm_p);

  if(p->next == NULL) return rt;

  //create a copy of m in the tail ring of p
  poly mCpy;
  if( r_m == r_t_p )
    mCpy = p_Head(m, r_t_p);
  else
  {
    omBin bin = r_t_p->PolyBin;
    p_AllocBin( rt, bin, r_t_p);
    for(long i = 0; i < degM; ++i)
      p_SetExp( mCpy, i, p_GetExp(m, i, r_m), r_t_p);
  }

  //now let tail(rt) = mCpy * tail(p)
  poly rt_it = rt;
  pIter(p);
  while(p->next != NULL)
  {
    rt_it = rt_it->next = p_Head(mCpy, r_t_p);
    sumDeg = p_Totaldegree(p, r_t_p) + degM;
    for(long i = degM, j = 0; i < sumDeg; ++i, ++j)
      p_SetExp( rt_it, i, p_GetExp(p, j, r_t_p), r_t_p);
    pIter(p);
  }
  rt_it = rt_it->next = mCpy;
  sumDeg = p_Totaldegree(p->next, r_t_p) + degM;
  for(long i = degM, j = 0; i < sumDeg; ++i, ++j)
    p_SetExp( rt_it, i, p_GetExp(p->next, j, r_t_p), r_t_p);
  rt_it->next = NULL;

  return rt;
}
#endif

inline long max(long a, long b)
{ return a > b ? a : b; }

// returns Copy(m)*Copy(p), does neither destroy p nor m Version
// for non-homog, non-shifted letterplace poly-/monomials
// m should be from r_m, lm(p) from r_lm_p, tail(p) from r_t_p
// output will have lm in r_lm_p and tail in r_t_p
inline poly pp_Mult_mm_normal
  ( poly p, const ring r_lm_p, const ring r_t_p,
    poly m, const ring r_m, int uptodeg, int lV, 
    uint* mDVec = NULL, uint dvSize = 0          )
{
  if( p == NULL || m == NULL ) return NULL;

  //BOCO: TODO: does not care for rings !!!
  return pp_Mult_mm( p, m, r_t_p );
}

// returns Copy(m)*Copy(p), does neither destroy p nor m Version
// for non-homog, non-shifted letterplace poly-/monomials
// m should be from r_m, lm(p) from r_lm_p, tail(p) from r_t_p
// output will have lm in r_lm_p and tail in r_t_p
inline poly lp_pp_Mult_mm_shift_n_shrink
  ( poly p, const ring r_lm_p, const ring r_t_p,
    poly m, const ring r_m, int uptodeg, int lV, 
    uint* mDVec = NULL, uint dvSize = 0          )
{
  if( p == NULL || m == NULL ) return NULL;

  long degP = p_Totaldegree(p, r_lm_p);
  poly p_it = p;
  while((pIter(p_it)) != NULL)
    degP = max(p_Totaldegree(p_it, r_t_p), degP);

  poly mTemp = p_LPshift( m, degP, uptodeg, lV, r_m );

  //BOCO: TODO: do both not care for rings !!!
  poly res = pp_Mult_mm( p, mTemp, r_t_p );
  poly shr = p_Shrink( res, lV, r_t_p );

  p_Delete(&mTemp, r_m);
  p_Delete(&res, r_m);

  return shr;
}

// returns Copy(m)*Copy(p), does neither destroy p nor m Version
// for non-homog, non-shifted letterplace poly-/monomials
// m should be from r_m, lm(p) from r_lm_p, tail(p) from r_t_p
// output will have lm in r_lm_p and tail in r_t_p
inline poly lp_pp_Mult_mm_v0
  ( poly p, const ring r_lm_p, const ring r_t_p,
    poly m, const ring r_m, int uptodeg, int lV, 
    uint* mDVec = NULL, uint dvSize = 0          )
{
  if( p == NULL || m == NULL ) return NULL;

  //BOCO: TODO: to calculate the total degree of a polynomial
  //each time, we need it seems to be inefficient; if we would
  //store it for each monmial, and if we would update it for
  //each time we multiply, we would have fast access to it!!!
  long degM = p_Totaldegree(m, r_m);

  int* exp_m = (int*) omAlloc0( (r_m->N+1) * sizeof(int) );
  int* exp_p = (int*) 
    omAlloc0( (max(r_lm_p->N, r_t_p->N)+1) * sizeof(int) );
  p_GetExpV(m, exp_m, r_m);

  poly rt;
  poly rt_it = rt = p_Init(r_lm_p);
  p_SetCoeff
    ( rt_it, n_Copy(p_GetCoeff(p, r_lm_p), r_lm_p), r_lm_p );
  p_GetExpV(p, exp_p, r_lm_p);
  long degP = p_Totaldegree(p, r_lm_p);
  memcpy( exp_p+degP*lV+1, exp_m+1, degM*lV*sizeof(int) );
  p_SetExpV( rt_it, exp_p, r_lm_p );

  pIter(p);
  while(p != NULL)
  {
    rt_it = rt_it->next = p_Init(r_t_p);
    p_SetCoeff
      ( rt_it, n_Copy(p_GetCoeff(p, r_t_p), r_t_p), r_t_p );
    degP = p_Totaldegree(p, r_t_p);
    p_GetExpV(p, exp_p, r_t_p);
    memcpy( exp_p+degP*lV+1, exp_m+1, degM*lV*sizeof(int) );
    p_SetExpV( rt_it, exp_p, r_t_p );

    pIter(p);
  }
  rt_it->next = NULL;

  omFreeSize( (ADDRESS)exp_m, (r_m->N+1) * sizeof(int) );
  omFreeSize( (ADDRESS)exp_p, 
              (max(r_lm_p->N, r_t_p->N)+1) * sizeof(int) );

  number coeffM = p_GetCoeff(m, r_m);
  return p_Mult_nn(rt, coeffM, r_lm_p, r_t_p);
  return rt;
}

// returns Copy(m)*Copy(p), does neither destroy p nor m Version
// for non-homog, non-shifted letterplace poly-/monomials
// m should be from r_m, lm(p) from r_lm_p, tail(p) from r_t_p
// output will have lm in r_lm_p and tail in r_t_p
inline poly lp_pp_Mult_mm_v1
  ( poly p, const ring r_lm_p, const ring r_t_p,
    poly m, const ring r_m, int uptodeg, int lV, 
    uint* mDVec = NULL, uint dvSize = 0          )
{
  if( p == NULL || m == NULL ) return NULL;

  //BOCO: TODO: to calculate the total degree of a polynomial
  //each time, we need it seems to be inefficient; if we would
  //store it for each monmial, and if we would update it for
  //each time we multiply, we would have fast access to it!!!
  long degM = p_Totaldegree(m, r_m);

  poly rt = p_Head(p, r_lm_p);
  poly rt_it = rt;
  long indexP = p_Totaldegree(p, r_lm_p) * lV + 1;
  while(1)
  {
    for(long i = 1, j = 0; j < degM; ++i, ++indexP)
      if( p_GetExp(m, i, r_m) )
      {
        p_SetExp( rt_it, indexP, 1, r_t_p);
        j = j + 1;
      }

    pIter(p);
    rt_it = rt_it->next = p_Head(p, r_t_p);
    if(p == NULL) break;
    indexP = p_Totaldegree(p, r_t_p) * lV + 1;
  }

  number coeffM = p_GetCoeff(m, r_m);
  return p_Mult_nn(rt, coeffM, r_lm_p, r_t_p);
}

inline bool p_GetOneBit(const poly p, const int VarOffset)
{
  //BOCO: TODO IDEA: We could create an Array containing
  //the VarOffset >> 24 and VarOffset & 0xffffff for each Varoffset.
  pAssume2((VarOffset >> (24 + 6)) == 0);
  return 
    ((p->exp[(VarOffset & 0xffffff)] >> (VarOffset >> 24)) & 1);
}

inline void p_SetOneBit(const poly p, const int VarOffset)
{
  p->exp[(VarOffset & 0xffffff)] |= (1 << (VarOffset >> 24));
}

// returns Copy(m)*Copy(p), does neither destroy p nor m Version
// for non-homog, non-shifted letterplace poly-/monomials
// m should be from r_m, lm(p) from r_lm_p, tail(p) from r_t_p
// output will have lm in r_lm_p and tail in r_t_p
inline poly lp_pp_Mult_mm_v2
  ( poly p, const ring r_lm_p, const ring r_t_p,
    poly m, const ring r_m, int uptodeg, int lV, 
    uint* mDVec = NULL, uint dvSize = 0          )
{
  if( p == NULL || m == NULL ) return NULL;

  //BOCO: TODO: to calculate the total degree of a polynomial
  //each time, we need it seems to be inefficient; if we would
  //store it for each monmial, and if we would update it for
  //each time we multiply, we would have fast access to it!!!
  long degM = p_Totaldegree(m, r_m);

  poly rt = p_Head(p, r_lm_p);
  poly rt_it = rt;
  long deg_rt_it = p_Totaldegree(p, r_lm_p) * lV + 1;
  long max_deg_rt_it = lV * uptodeg;
  long m_ind_it = 1;
  while(true)
  {
    nextblock: ;
    for(int j = m_ind_it, i = 0; i < lV; ++i, ++j)
      if(p_GetOneBit(m, r_lm_p->VarOffset[j]))
      {
        p_SetOneBit(rt_it, r_lm_p->VarOffset[deg_rt_it+i]);
        deg_rt_it += lV;
        m_ind_it += lV;
        if(deg_rt_it > max_deg_rt_it) break;
        goto nextblock;
      }
    break; //no more Variables in m
  }

  pIter(p);
  int* VOffset = r_t_p->VarOffset;
  while(p != NULL)
  {
    rt_it = rt_it->next = p_Head(p, r_t_p);
    deg_rt_it = p_Totaldegree(p, r_t_p) * lV + 1;
    long m_ind_it = 1;
    while(true)
    {
      nextblock2: ;
      for(int i = 0, j = m_ind_it; i < lV; ++j, ++i)
        if(p_GetOneBit(m, VOffset[j])) 
        {
          p_SetOneBit(rt_it, VOffset[deg_rt_it+i]);
          deg_rt_it += lV;
          m_ind_it += lV;
          goto nextblock2;
        }
      break; //no more Variables in m
    }

    pIter(p);
  }

  number coeffM = p_GetCoeff(m, r_m);
  return p_Mult_nn(rt, coeffM, r_lm_p, r_t_p);
}

// returns Copy(m)*Copy(p), does neither destroy p nor m Version
// for non-homog, non-shifted letterplace poly-/monomials
// m should be from r_m, lm(p) from r_lm_p, tail(p) from r_t_p
// output will have lm in r_lm_p and tail in r_t_p
inline poly lp_pp_Mult_mm_v1_a
  ( poly p, const ring r_lm_p, const ring r_t_p,
    poly m, const ring r_m, int uptodeg, int lV, 
    uint* mDVec = NULL, uint dvSize = 0          )
{
  if( p == NULL || m == NULL ) return NULL;

  uint* dvec, * dvIt;
  dvSize = ShiftDVec::CreateDVec(m, r_m, dvec);
  uint* dvLast = &(dvec[dvSize]);

  poly rt = p_Head(p, r_lm_p);
  long indexP = p_Totaldegree(p, r_lm_p) * lV;
  indexP += dvec[0];
  for(dvIt = dvec; dvIt != dvLast; ++dvIt, indexP += *dvIt)
    p_SetExp( rt, indexP, 1, r_lm_p );

  poly rt_it = rt;
  pIter(p);
  while(p != NULL)
  {
    rt_it = rt_it->next = p_Head(p, r_t_p);
    indexP = p_Totaldegree(p, r_t_p) * lV;
    indexP += dvec[0];
    for(dvIt = dvec; dvIt != dvLast; ++dvIt, indexP += *dvIt)
      p_SetExp( rt_it, indexP, 1, r_t_p );
    pIter(p);
  }

  number coeffM = p_GetCoeff(m, r_m);
  return p_Mult_nn(rt, coeffM, r_lm_p, r_t_p);
}

// returns Copy(m)*Copy(p), does neither destroy p nor m Version
// for non-homog, non-shifted letterplace poly-/monomials
// m should be from r_m, lm(p) from r_lm_p, tail(p) from r_t_p
// output will have lm in r_lm_p and tail in r_t_p
inline poly lp_pp_Mult_mm_v2_a
  ( poly p, const ring r_lm_p, const ring r_t_p,
    poly m, const ring r_m, int uptodeg, int lV, 
    uint* mDVec = NULL, uint dvSize = 0          )
{
  if( p == NULL || m == NULL ) return NULL;

  int numVars = r_t_p->N;
  int* VOffset = r_t_p->VarOffset;
  int* VOffPtr = VOffset;

  uint* dvec, * dvIt;
  dvSize = ShiftDVec::CreateDVec(m, r_m, dvec);
  uint* dvLast = &(dvec[dvSize]);

  poly rt = p_Head(p, r_lm_p);
  long indexP = p_Totaldegree(p, r_lm_p)*lV;
  indexP += dvec[0];
  for(dvIt = dvec; dvIt != dvLast; ++dvIt, indexP += *dvIt)
    p_SetOneBit(rt, r_lm_p->VarOffset[indexP]);

  poly rt_it = rt;
  pIter(p);
  while(p != NULL)
  {
    rt_it = rt_it->next = p_Head(p, r_t_p);
    indexP = p_Totaldegree(p, r_t_p) * lV;
    VOffPtr = VOffset + indexP + dvec[0];
    for(dvIt = dvec; dvIt != dvLast; ++dvIt, VOffPtr += *dvIt)
      p_SetOneBit(rt_it, *VOffPtr);
    pIter(p);
  }

  omFreeSize( (ADDRESS)dvec, (dvSize) * sizeof(uint) );

  number coeffM = p_GetCoeff(m, r_m);
  return p_Mult_nn(rt, coeffM, r_lm_p, r_t_p);
}

// returns Copy(m)*Copy(p), does neither destroy p nor m Version
// for non-homog, non-shifted letterplace poly-/monomials
// m should be from r_m, lm(p) from r_lm_p, tail(p) from r_t_p
// output will have lm in r_lm_p and tail in r_t_p
inline poly lp_pp_Mult_mm_v2_a_with_mDVec
  ( poly p, const ring r_lm_p, const ring r_t_p,
    poly m, const ring r_m, int uptodeg, int lV, 
    uint* mDVec = NULL, uint dvSize = 0          )
{
  if( p == NULL || m == NULL || mDVec == NULL ) return NULL;

  int numVars = r_t_p->N;
  int* VOffset = r_t_p->VarOffset;
  int* VOffPtr = VOffset;

  uint* dvIt;
  uint* dvLast = &(mDVec[dvSize]);

  poly rt = p_Head(p, r_lm_p);
  long indexP = p_Totaldegree(p, r_lm_p)*lV;
  indexP += mDVec[0];
  for(dvIt = mDVec; dvIt != dvLast; ++dvIt, indexP += *dvIt)
    p_SetOneBit(rt, r_lm_p->VarOffset[indexP]);

  poly rt_it = rt;
  pIter(p);
  while(p != NULL)
  {
    rt_it = rt_it->next = p_Head(p, r_t_p);
    indexP = p_Totaldegree(p, r_t_p) * lV;
    VOffPtr = VOffset + indexP + mDVec[0];
    for(dvIt = mDVec; dvIt != dvLast; ++dvIt, VOffPtr += *dvIt)
      p_SetOneBit(rt_it, *VOffPtr);
    pIter(p);
  }

  number coeffM = p_GetCoeff(m, r_m);
  return p_Mult_nn(rt, coeffM, r_lm_p, r_t_p);
}

#if 0
// returns p=Copy(p1)*Copy(p2), with lm(p) in lm_ring and
// tail(p) in tail_ring, does neither destroy p1 nor p2 
// assumes lm(pi) in lm_ring and tail(pi) in tail_ring (i=1,2)
PINLINE2 poly lp_pp_Mult_pp_v1
  ( poly p1, poly p2, const ring lm_ring, const ring tail_ring )
{
  while(p1->next != NULL)
  {
    lp_mm_Mult_pp_v1(p1, p2)
  }
  //yet to implement
  return NULL;
}
#endif //#if 0
