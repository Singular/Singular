#include <polys/monomials/ring.h>
#include <kernel/polys.h>
#include <kernel/shiftgb.h>


poly p_LPshift(poly p, int sh, int uptodeg, int lV,const ring r);
poly p_Shrink(poly p, int lV, const ring r);

inline void printExp
  (long unsigned int* exp, int numberOf16BitParts);

int osize;
int* omap;
/* Creates a Mapping: i -> pos, where i is an index of a
 * variable in a block of the letterplace polynomial and pos the
 * position of the order Field for the block. Does yet not work
 * properly; May only work for dp-Orderings... TODO!           */
inline int* GetOrderMapping( ring r );

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

inline poly lp_pp_Mult_mm_v0_a
  ( poly p, const ring r_lm_p, const ring r_t_p,
    poly m, const ring r_m, int uptodeg, int lV, 
    uint* mDVec, uint dvSize );

inline poly lp_pp_Mult_mm_v0_b
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

inline poly lp_pp_Mult_mm_v1_b
  ( poly p, const ring r_lm_p, const ring r_t_p,
    poly m, const ring r_m, int uptodeg, int lV, 
    uint* mDVec, uint dvSize );

inline poly lp_pp_Mult_mm_v1_c
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

inline poly lp_pp_Mult_mm_v2_c
  ( poly p, const ring r_lm_p, const ring r_t_p,
    poly m, const ring r_m, int uptodeg, int lV, 
    uint* mDVec, uint dvSize );

inline poly lp_pp_Mult_mm_v2_d
  ( poly p, const ring r_lm_p, const ring r_t_p,
    poly m, const ring r_m, int uptodeg, int lV, 
    uint* mDVec, uint dvSize );

inline poly lp_pp_Mult_mm_v2_e
  ( poly p, const ring r_lm_p, const ring r_t_p,
    poly m, const ring r_m, int uptodeg, int lV, 
    uint* mDVec, uint dvSize );

inline poly lp_pp_Mult_mm_v2_f
  ( poly p, const ring r_lm_p, const ring r_t_p,
    poly m, const ring r_m, int uptodeg, int lV, 
    uint* mDVec, uint dvSize );

inline poly lp_pp_Mult_mm_v2_a_with_mDVec_neu
  ( poly p, const ring r_lm_p, const ring r_t_p,
    poly m, const ring r_m, int uptodeg, int lV, 
    uint* mDVec, uint dvSize );

inline poly lp_pp_Mult_mm_v2_a_with_mDVec_neu2
  ( poly p, const ring r_lm_p, const ring r_t_p,
    poly m, const ring r_m, int uptodeg, int lV, 
    uint* mDVec, uint dvSize );

inline poly lp_pp_Mult_mm_v3
  ( poly p, const ring r_lm_p, const ring r_t_p,
    poly m, const ring r_m, int uptodeg, int lV, 
    uint* mDVec, uint dvSize );



inline poly mm_Mult_pp_normal
  ( poly p, const ring r_lm_p, const ring r_t_p,
    poly m, const ring r_m, int uptodeg, int lV, 
    uint* mDVec, uint dvSize );

inline poly lp_mm_Mult_pp_shift
  ( poly p, const ring r_lm_p, const ring r_t_p,
    poly m, const ring r_m, int uptodeg, int lV, 
    uint* mDVec, uint dvSize );

inline poly lp_mm_Mult_pp_slow
  ( poly p, const ring r_lm_p, const ring r_t_p,
    poly m, const ring r_m, int uptodeg, int lV, 
    uint* mDVec, uint dvSize );

inline poly lp_mm_Mult_pp_direct
  ( poly p, const ring r_lm_p, const ring r_t_p,
    poly m, const ring r_m, int uptodeg, int lV, 
    uint* mDVec, uint dvSize );

inline poly lp_mm_Mult_pp_direct2
  ( poly p, const ring r_lm_p, const ring r_t_p,
    poly m, const ring r_m, int uptodeg, int lV, 
    uint* mDVec, uint dvSize );

inline poly lp_mm_Mult_pp_direct3
  ( poly p, const ring r_lm_p, const ring r_t_p,
    poly m, const ring r_m, int uptodeg, int lV, 
    uint* mDVec, uint dvSize );


struct letterplace_multiplication_functions
{
  poly (*pp_Mult_mm[11])
    ( poly, const ring, const ring, poly, const ring, 
      int uptodeg, int lV, uint* mDVec, uint dvSize   );
  int size;
   
}; 

#if 1 //p * m testing case
//struct letterplace_multiplication_functions lpMultFunctions = { { &pp_Mult_mm_normal, &lp_pp_Mult_mm_shift_n_shrink, &lp_pp_Mult_mm_v0, &lp_pp_Mult_mm_v1, &lp_pp_Mult_mm_v2, &lp_pp_Mult_mm_v1_a, &lp_pp_Mult_mm_v2_a, &lp_pp_Mult_mm_v2_a_with_mDVec }, 8, };
struct letterplace_multiplication_functions lpMultFunctions = { 
  { &pp_Mult_mm_normal, 
    &lp_pp_Mult_mm_shift_n_shrink, 
    &lp_pp_Mult_mm_v0, 
    &lp_pp_Mult_mm_v0_a, 
    &lp_pp_Mult_mm_v0_b, 
    &lp_pp_Mult_mm_v1_a, 
    &lp_pp_Mult_mm_v1_c, 
    &lp_pp_Mult_mm_v2_c, 
    &lp_pp_Mult_mm_v2_e, 
    &lp_pp_Mult_mm_v2_f, 
    &lp_pp_Mult_mm_v2_a_with_mDVec_neu
  }, 11
};
#endif

#if 0 //m * p testing case
struct letterplace_multiplication_functions lpMultFunctions = { 
  { &mm_Mult_pp_normal,
    &lp_mm_Mult_pp_shift,
    &lp_mm_Mult_pp_slow,
    &lp_mm_Mult_pp_direct,
    &lp_mm_Mult_pp_direct2
  }, 5
};
#endif

//Winners for p*m:
//
//Winners in the dp-optimized Group:
//lp_pp_Mult_mm_v2_a_with_mDVec_neu
//lp_pp_Mult_mm_v2_c
//
//Winner of the slower ones:
//lp_pp_Mult_mm_v1_a (with the use of the dvec of m)
//
//Winner of the slower ones, which does not use m's dvec
//lp_pp_Mult_mm_v0_b
//
//Winners for m*p (no use of dvecs so far)
//
//Winner in the dp-optimized Group:
//lp_mm_Mult_pp_direct
//
//Winner of the slower ones:
//lp_mm_Mult_pp_slow


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

inline void printExp
  (long unsigned int* exp, int numberOf16BitParts)
{
  int i = 1;
  int j, k;
  long unsigned int bitmask;
  while(1)
  {
    bitmask = 1;
    bitmask = bitmask << (8 * sizeof(long) - 1);
    for(k = 0; k < sizeof(long)/2; ++k, ++i)
    {
      if(i > numberOf16BitParts){ PrintLn(); return; }
      for(j = 0; j < 16; ++j)
      {
        if(bitmask & *exp) PrintS("1");
        else PrintS("0");
        bitmask >>= 1;
      }
      PrintS(" ");
    }
    PrintLn();
    if(i > numberOf16BitParts){ PrintLn(); return; }
    ++exp;
  }
}

/* Creates a Mapping: i -> pos, where i is an index of a
 * variable in a block of the letterplace polynomial and pos the
 * position of the order Field for the block. Does yet not work
 * properly; May only work for dp-Orderings... TODO!           
 * Returns Mapping in oindex and size as return value.
 * oindex has to be freed with omFreeSize. TODO: Care for ring!*/
inline int GetOrderMapping( ring r, int** oindex )
{
  *oindex = (int *) omAlloc( (r->N+1) * sizeof(int) );

  for(int i = 1; i < r->OrdSize; ++i)
  {
    sro_ord* o=&(r->typ[i]);
    assume(o->ord_typ == ro_dp);
    int a,e;
    a=o->data.dp.start;
    e=o->data.dp.end;
    for(int i=a;i<=e;i++) (*oindex)[i] = o->data.dp.place;
  }

  (*oindex)[0] = currRing->pOrdIndex;

  return r->N+1;
}

inline long max(long a, long b)
{ return a > b ? a : b; }

#if 0
//This was loosely derived from p_Head; It will return the
//total degree of lm(p) and return a copy of p in q. The ring of
//q can be different from the ring of p (At least I hope so).
//After this function you may have to invoke p_Setm.
static inline poly p_CpyExp
  (poly p, poly* q, const ring r_p, const ring r_q)
{
  if (p == NULL) return NULL;
  p_LmCheckPolyRing1(p, r_p);
  omTypeAllocBin(poly, *q, r->PolyBin);
  p_SetRingOfLm(*q, r_q);
  pNext(*q) = NULL;

  //This represents the first index in the currently considered
  //block in q and p.
  long it = 1;
  
  {
    nextblock: ;
    //We will loop, until we found an empty block, or until we
    //considered all variables
    for(long i = 0; i < lV; ++i)
      if( p_GetExp(p, it+i, r_p) )
      {
        p_SetExp(*q, it+i, 1, r_q);
        it += lV;
        if(it > r->N) return it - lV + i; //looped through all vars
        goto nextblock;
        
      }
  }

  p_SetCoeff( *q, n_Copy(p_GetCoeff(p, r_p), r_p), r_q );
  return np;
}
#endif

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
inline poly lp_pp_Mult_mm_v0_a
  ( poly p, const ring r_lm_p, const ring r_t_p,
    poly m, const ring r_m, int uptodeg, int lV, 
    uint* mDVec = NULL, uint dvSize = 0          )
{
  if( p == NULL || m == NULL ) return NULL;

  //BOCO TODO: Care for Rings and free the allocated space !!!

  long degM = p_Totaldegree(m, r_m);

  //BOCO: Not to nice: Who frees the space at the end?
  static int N = currRing->N;
  static int* exp_m_double = 
    (int*) omAlloc0( 2*(N+1) * sizeof(int) );
  static int* exp_m = exp_m_double+N+1;
  if(N != currRing->N)
  {
    omFreeSize( (ADDRESS)exp_m_double, 2*(N+1) * sizeof(int) );
    N = currRing->N;
    exp_m_double = (int*) omAlloc0( 2*(N+1) * sizeof(int) );
    exp_m = exp_m_double+N+1;
  }

  p_GetExpV(m, exp_m, currRing);
  spolyrec rt;
  poly rt_it = &rt;

  while(p != NULL)
  {
    rt_it = rt_it->next = p_Init(currRing);

    p_SetCoeff
      ( rt_it, n_Copy(p_GetCoeff(p, currRing), currRing), currRing );

    long degP = p_Totaldegree(p, currRing);
    p_SetExpV( rt_it, exp_m-degP*lV, currRing );
     
    p_ExpVectorAdd(rt_it, p, currRing);

    pIter(p);
  }

  number coeffM = p_GetCoeff(m, currRing);
  return p_Mult_nn(rt.next, coeffM, currRing, currRing);
}

// returns Copy(m)*Copy(p), does neither destroy p nor m Version
// for non-homog, non-shifted letterplace poly-/monomials
// m should be from r_m, lm(p) from r_lm_p, tail(p) from r_t_p
// output will have lm in r_lm_p and tail in r_t_p
inline poly lp_pp_Mult_mm_v0_b
  ( poly p, const ring r_lm_p, const ring r_t_p,
    poly m, const ring r_m, int uptodeg, int lV, 
    uint* mDVec = NULL, uint dvSize = 0          )
{
  if( p == NULL || m == NULL ) return NULL;


  poly rt = p_Head(p, r_lm_p);
  poly rt_it = rt;
  long deg_rt_it = p_Totaldegree(p, r_lm_p) * lV + 1;
  long max_deg_rt_it = lV * uptodeg;
  long m_ind_it = 1;

  {
    nextblock: ;
    for(int j = m_ind_it, i = 0; i < lV; ++i, ++j)
      if( p_GetExp(m, j, r_m) )
      {
        p_SetExp(rt_it, deg_rt_it+i, 1, r_lm_p);
        deg_rt_it += lV;
        m_ind_it += lV;
        if(deg_rt_it > max_deg_rt_it) break;
        goto nextblock;
      }
  }
  p_Setm(rt_it, r_lm_p);

  pIter(p);
  while(p != NULL)
  {
    rt_it = rt_it->next = p_Head(p, r_t_p);
    deg_rt_it = p_Totaldegree(p, r_t_p) * lV + 1;
    long m_ind_it = 1;

    {
      nextblock2: ;
      for(int i = 0, j = m_ind_it; i < lV; ++j, ++i)
        if( p_GetExp(m, j, r_m) )
        {
          p_SetExp(rt_it, deg_rt_it+i, 1, r_t_p);
          deg_rt_it += lV;
          m_ind_it += lV;
          if(deg_rt_it > max_deg_rt_it) break;
          goto nextblock2;
        }
    }

    p_Setm(rt_it, r_t_p);
    pIter(p);
  }

  number coeffM = p_GetCoeff(m, r_m);
  return p_Mult_nn(rt, coeffM, r_lm_p, r_t_p);
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

    p_Setm(rt_it, r_t_p);

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

//Does not work properly (Why? -> TODO)
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
    //p_Setm(rt_it, r_lm_p);
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
    //p_Setm(rt_it, r_t_p);

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
  p_Setm(rt, r_lm_p);

  poly rt_it = rt;
  pIter(p);
  while(p != NULL)
  {
    rt_it = rt_it->next = p_Head(p, r_t_p);
    indexP = p_Totaldegree(p, r_t_p) * lV;
    indexP += dvec[0];
    for(dvIt = dvec; dvIt != dvLast; ++dvIt, indexP += *dvIt)
      p_SetExp( rt_it, indexP, 1, r_t_p );
    p_Setm(rt_it, r_t_p);
    pIter(p);
  }

  number coeffM = p_GetCoeff(m, r_m);
  return p_Mult_nn(rt, coeffM, r_lm_p, r_t_p);
}

// returns Copy(m)*Copy(p), does neither destroy p nor m Version
// for non-homog, non-shifted letterplace poly-/monomials
// m should be from r_m, lm(p) from r_lm_p, tail(p) from r_t_p
// output will have lm in r_lm_p and tail in r_t_p
inline poly lp_pp_Mult_mm_v1_b
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
  //p_Setm(rt, r_lm_p);

  poly rt_it = rt;
  pIter(p);
  while(p != NULL)
  {
    rt_it = rt_it->next = p_Head(p, r_t_p);
    indexP = p_Totaldegree(p, r_t_p) * lV;
    indexP += dvec[0];
    for(dvIt = dvec; dvIt != dvLast; ++dvIt, indexP += *dvIt)
      p_SetExp( rt_it, indexP, 1, r_t_p );
    //p_Setm(rt_it, r_t_p);
    pIter(p);
  }

  number coeffM = p_GetCoeff(m, r_m);
  return p_Mult_nn(rt, coeffM, r_lm_p, r_t_p);
}

// returns Copy(m)*Copy(p), does neither destroy p nor m Version
// for non-homog, non-shifted letterplace poly-/monomials
// m should be from r_m, lm(p) from r_lm_p, tail(p) from r_t_p
// output will have lm in r_lm_p and tail in r_t_p
inline poly lp_pp_Mult_mm_v1_c
  ( poly p, const ring r_lm_p, const ring r_t_p,
    poly m, const ring r_m, int uptodeg, int lV, 
    uint* mDVec = NULL, uint dvSize = 0          )
{
  if( p == NULL || m == NULL ) return NULL;

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
        rt_it->exp[omap[indexP]] = 1;
        j = j + 1;
      }
    rt_it->exp[omap[0]] += m->exp[omap[0]];

    pIter(p);
    rt_it = rt_it->next = p_Head(p, r_t_p);
    if(p == NULL) break;
    indexP = p_Totaldegree(p, r_t_p) * lV + 1;
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
  p_Setm(rt, r_lm_p);

  poly rt_it = rt;
  pIter(p);
  while(p != NULL)
  {
    rt_it = rt_it->next = p_Head(p, r_t_p);
    indexP = p_Totaldegree(p, r_t_p) * lV;
    VOffPtr = VOffset + indexP + dvec[0];
    for(dvIt = dvec; dvIt != dvLast; ++dvIt, VOffPtr += *dvIt)
      p_SetOneBit(rt_it, *VOffPtr);
    p_Setm(rt_it, r_t_p);
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
  p_Setm(rt, r_lm_p);

  poly rt_it = rt;
  pIter(p);
  while(p != NULL)
  {
    rt_it = rt_it->next = p_Head(p, r_t_p);
    indexP = p_Totaldegree(p, r_t_p) * lV;
    VOffPtr = VOffset + indexP + mDVec[0];
    for(dvIt = mDVec; dvIt != dvLast; ++dvIt, VOffPtr += *dvIt)
      p_SetOneBit(rt_it, *VOffPtr);
    p_Setm(rt_it, r_t_p);
    pIter(p);
  }

  number coeffM = p_GetCoeff(m, r_m);
  return p_Mult_nn(rt, coeffM, r_lm_p, r_t_p);
}

// returns Copy(m)*Copy(p), does neither destroy p nor m Version
// for non-homog, non-shifted letterplace poly-/monomials
// m should be from r_m, lm(p) from r_lm_p, tail(p) from r_t_p
// output will have lm in r_lm_p and tail in r_t_p
inline poly lp_pp_Mult_mm_v2_c
  ( poly p, const ring r_lm_p, const ring r_t_p,
    poly m, const ring r_m, int uptodeg, int lV, 
    uint* mDVec = NULL, uint dvSize = 0          )
{
  if( p == NULL || m == NULL ) return NULL;


  poly rt = p_Head(p, r_lm_p);
  poly rt_it = rt;
  long deg_rt_it = p_Totaldegree(p, r_lm_p) * lV + 1;
  long max_deg_rt_it = lV * uptodeg;
  long m_ind_it = 1;

  {
    nextblock: ;
    for(int j = m_ind_it, i = 0; i < lV; ++i, ++j)
      if( p_GetExp(m, j, r_m) )
      {
        p_SetExp(rt_it, deg_rt_it+i, 1, r_lm_p);
        rt_it->exp[omap[deg_rt_it+i]] = 1;
        deg_rt_it += lV;
        m_ind_it += lV;
        if(deg_rt_it > max_deg_rt_it) break;
        goto nextblock;
      }
  }
  rt_it->exp[omap[0]] += m->exp[omap[0]];

  pIter(p);
  while(p != NULL)
  {
    rt_it = rt_it->next = p_Head(p, r_t_p);
    deg_rt_it = p_Totaldegree(p, r_t_p) * lV + 1;
    long m_ind_it = 1;

    {
      nextblock2: ;
      for(int i = 0, j = m_ind_it; i < lV; ++j, ++i)
        if( p_GetExp(m, j, r_m) )
        {
          p_SetExp(rt_it, deg_rt_it+i, 1, r_t_p);
          rt_it->exp[omap[deg_rt_it+i]] = 1;
          deg_rt_it += lV;
          m_ind_it += lV;
          if(deg_rt_it > max_deg_rt_it) break;
          goto nextblock2;
        }
    }
    rt_it->exp[omap[0]] += m->exp[omap[0]];

    pIter(p);
  }

  number coeffM = p_GetCoeff(m, r_m);
  return p_Mult_nn(rt, coeffM, r_lm_p, r_t_p);
}

// returns Copy(m)*Copy(p), does neither destroy p nor m Version
// for non-homog, non-shifted letterplace poly-/monomials
// m should be from r_m, lm(p) from r_lm_p, tail(p) from r_t_p
// output will have lm in r_lm_p and tail in r_t_p
inline poly lp_pp_Mult_mm_v2_d
  ( poly p, const ring r_lm_p, const ring r_t_p,
    poly m, const ring r_m, int uptodeg, int lV, 
    uint* mDVec = NULL, uint dvSize = 0          )
{
  if( p == NULL || m == NULL ) return NULL;


  poly rt = p_Head(p, r_lm_p);
  poly rt_it = rt;
  long deg_rt_it = p_Totaldegree(p, r_lm_p) * lV + 1;
  long max_deg_rt_it = lV * uptodeg;
  long m_ind_it = 1;

  {
    nextblock: ;
    for(int j = m_ind_it, i = 0; i < lV; ++i, ++j)
      if( p_GetExp(m, j, r_m) )
      {
        p_SetExp(rt_it, deg_rt_it+i, 1, r_lm_p);
        rt_it->exp[omap[deg_rt_it+i]] = 1;
        deg_rt_it += lV;
        m_ind_it += lV;
        if(deg_rt_it > max_deg_rt_it) break;
        goto nextblock;
      }
  }
  rt_it->exp[omap[0]] += m->exp[omap[0]];

  pIter(p);
  while(p != NULL)
  {
    rt_it = rt_it->next = p_Head(p, r_t_p);
    deg_rt_it = p_Totaldegree(p, r_t_p) * lV + 1;
    long m_ind_it = 1;

    {
      nextblock2: ;
      for(int i = 0, j = m_ind_it; i < lV; ++j, ++i)
        if( p_GetExp(m, j, r_m) )
        {
          p_SetExp(rt_it, deg_rt_it+i, 1, r_t_p);
          rt_it->exp[omap[deg_rt_it+i]] = 1;
          deg_rt_it += lV;
          m_ind_it += lV;
          if(deg_rt_it > max_deg_rt_it) break;
          goto nextblock2;
        }
    }
    rt_it->exp[omap[0]] += m->exp[omap[0]];

    pIter(p);
  }

  number coeffM = p_GetCoeff(m, r_m);
  return p_Mult_nn(rt, coeffM, r_lm_p, r_t_p);
}

void p_ExpSum_dp
  (poly rt, poly p, poly q, int lV, ring r)
{
  //This represents the first index in the currently considered
  //block in exprt.
  long index_rt = 1;
  
  //This represents the first index in the currently considered
  //block in p.exp or q.exp respectively.
  long index_exp = 1; //first we will iterate through exp1
  {
    nextblock: ;
    //We will loop, until we found an empty block, or until we
    //considered all variables
    for(long i = 0; i < lV; ++i)
      if( p_GetExp(p, index_exp+i, r) )
      {
        p_SetExp(rt, index_rt+i, 1, r);
        rt->exp[omap[index_rt+i]] = 1;
        index_rt += lV; //We found a nonzero exponent, thus
        index_exp += lV; //we can move on to the next block
        if(index_rt > r->N) 
        { 
          rt->exp[omap[0]] = p->exp[omap[0]];
          return;  //looped through all vars
        }
        goto nextblock;
      }
  }

  index_exp = 1; //Now we will iterate through exp2
  {
    nextblock2: ;
    for(long i = 0; i < lV; ++i)
      if( p_GetExp(q, index_exp+i, r) )
      {
        p_SetExp(rt, index_rt+i, 1, r);
        rt->exp[omap[index_rt+i]] = 1;
        index_rt += lV;
        index_exp += lV;
        if(index_rt > r->N) break;
        goto nextblock2;
      }
  }

  rt->exp[omap[0]] = 
    p->exp[omap[0]] + q->exp[omap[0]];

  return; //BOCO: could also return Totaldeg(p+q) ...
}

inline poly lp_pp_Mult_mm_v2_e
  ( poly p, const ring r_lm_p, const ring r_t_p,
    poly m, const ring r_m, int uptodeg, int lV, 
    uint* mDVec = NULL, uint dvSize = 0          )
{
  if( p == NULL || m == NULL ) return NULL;


  poly rt = p_Init(r_lm_p);
  poly rt_it = rt;
  p_ExpSum_dp(rt, p, m, lV, r_m);
  p_SetCoeff
    ( rt, n_Copy(p_GetCoeff(p, r_lm_p), r_lm_p), r_lm_p );

  pIter(p);
  while(p != NULL)
  {
    rt_it = rt_it->next = p_Init(r_t_p);
    p_ExpSum_dp(rt_it, p, m, lV, r_m);
    p_SetCoeff
      ( rt_it, n_Copy(p_GetCoeff(p, r_t_p), r_t_p), r_t_p );
    pIter(p);
  }

  rt_it->next = NULL;
  //Or better do that above...
  return p_Mult_nn
    (rt, p_GetCoeff(m, r_m), r_lm_p, r_t_p);
}

void p_ExpSum_dp_f
  (poly rt, poly p, poly q, int lV, ring r)
{
  int i, j;
  for(i=1, j=0; i <= r->N && j < lV; ++i, ++j)
  {
    if( p_GetExp(p, i, r) )
    {
      p_SetExp(rt, i, 1, r);
      rt->exp[omap[i]] = 1;
      j = 0;
    }
    else
      p_SetExp(rt, i, 0, r);
  }

  for(int k = 1; i <= r->N; ++i, ++k)
  {
    if( p_GetExp(q, k, r) )
    {
      p_SetExp(rt, i, 1, r);
      rt->exp[omap[i]] = 1;
    }
    else
      p_SetExp(rt, i, 0, r);
  }
}

inline poly lp_pp_Mult_mm_v2_f
  ( poly p, const ring r_lm_p, const ring r_t_p,
    poly m, const ring r_m, int uptodeg, int lV, 
    uint* mDVec = NULL, uint dvSize = 0          )
{
  if( p == NULL || m == NULL ) return NULL;


  poly rt = p_New(r_lm_p);
  p_ExpSum_dp_f(rt, p, m, lV, r_m);
  p_SetRingOfLm(rt, r_m);
  p_SetCoeff
    ( rt, n_Copy(p_GetCoeff(p, r_lm_p), r_lm_p), r_lm_p );

  poly rt_it = rt;

  pIter(p);
  while(p != NULL)
  {
    rt_it = rt_it->next = p_Init(r_t_p);
    p_ExpSum_dp_f(rt_it, p, m, lV, r_m);
    p_SetRingOfLm(rt_it, r_m);
    p_SetCoeff
      ( rt_it, n_Copy(p_GetCoeff(p, r_t_p), r_t_p), r_t_p );
    pIter(p);
  }

  rt->next = NULL;
  //Or better do that above...
  return p_Mult_nn
    (rt, p_GetCoeff(m, r_m), r_lm_p, r_t_p);
}

// returns Copy(m)*Copy(p), does neither destroy p nor m Version
// for non-homog, non-shifted letterplace poly-/monomials
// m should be from r_m, lm(p) from r_lm_p, tail(p) from r_t_p
// output will have lm in r_lm_p and tail in r_t_p
inline poly lp_pp_Mult_mm_v2_a_with_mDVec_neu
  ( poly p, const ring r_lm_p, const ring r_t_p,
    poly m, const ring r_m, int uptodeg, int lV, 
    uint* mDVec = NULL, uint dvSize = 0          )
{
  if( p == NULL || m == NULL || mDVec == NULL ) return NULL;

  uint* dvIt;
  uint* dvLast = &(mDVec[dvSize]);

  poly rt = p_Head(p, r_lm_p);
  long indexP = p_Totaldegree(p, r_lm_p)*lV;
  indexP += mDVec[0];
  for(dvIt = mDVec; dvIt != dvLast; ++dvIt, indexP += *dvIt)
  {
    p_SetExp(rt, indexP, 1, r_lm_p);
    rt->exp[omap[indexP]] = 1;
  }
  rt->exp[omap[0]] += m->exp[omap[0]];

  poly rt_it = rt;
  pIter(p);
  while(p != NULL)
  {
    rt_it = rt_it->next = p_Head(p, r_t_p);
    indexP = p_Totaldegree(p, r_t_p) * lV;
    indexP += mDVec[0];
    for(dvIt = mDVec; dvIt != dvLast; ++dvIt, indexP += *dvIt)
    {
      p_SetExp(rt_it, indexP, 1, r_t_p);
      rt_it->exp[omap[indexP]] = 1;
    }
    rt_it->exp[omap[0]] += m->exp[omap[0]];
    pIter(p);
  }

  number coeffM = p_GetCoeff(m, r_m);
  return p_Mult_nn(rt, coeffM, r_lm_p, r_t_p);
}

// returns Copy(m)*Copy(p), does neither destroy p nor m Version
// for non-homog, non-shifted letterplace poly-/monomials
// m should be from r_m, lm(p) from r_lm_p, tail(p) from r_t_p
// output will have lm in r_lm_p and tail in r_t_p
// forget this function!
inline poly lp_pp_Mult_mm_v2_a_with_mDVec_neu2
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
  {
    p_SetOneBit(rt, r_lm_p->VarOffset[indexP]);
    rt->exp[omap[indexP]] = 1;
  }
  rt->exp[omap[0]] += m->exp[omap[0]];

  poly rt_it = rt;
  pIter(p);
  while(p != NULL)
  {
    rt_it = rt_it->next = p_Head(p, r_t_p);
    indexP = p_Totaldegree(p, r_t_p) * lV;
    VOffPtr = VOffset + indexP + mDVec[0];
    for(dvIt = mDVec; dvIt != dvLast; ++dvIt, VOffPtr += *dvIt)
    {
      p_SetOneBit(rt_it, *VOffPtr);
      rt_it->exp[omap[indexP]] = 1;
    }
    rt_it->exp[omap[0]] += m->exp[omap[0]];
    pIter(p);
  }

  number coeffM = p_GetCoeff(m, r_m);
  return p_Mult_nn(rt, coeffM, r_lm_p, r_t_p);
}


// returns Copy(m)*Copy(p), does neither destroy p nor m Version
// for non-homog, non-shifted letterplace poly-/monomials
// m should be from r_m, lm(p) from r_lm_p, tail(p) from r_t_p
// output will have lm in r_lm_p and tail in r_t_p
// BOCO: WARNING: This function is likely to fail if ordering is
// not dp... (-> TODO)
// BOCO: Does it work with the tail ring? Likely not... (TODO:
// get to know more Theory)
// BOCO: TODO/WARNING: May only work for certain word sizes and
// partitions...
// BOCO: Forget this function at the moment
inline poly lp_pp_Mult_mm_v3
  ( poly p, const ring r_lm_p, const ring r_t_p,
    poly m, const ring r_m, int uptodeg, int lV, 
    uint* mDVec = NULL, uint dvSize = 0          )
{
  if( p == NULL || m == NULL ) return NULL;

  //BOCO: TODO: Care for Rings etc.!!!
  //BOCO: TODO: A lot... (other word sizes, partitions...)

  long degM = p_Totaldegree(m, currRing);

  spolyrec rt;
  poly rt_it = &rt;

  while(p != NULL)
  {
    //PrintLn();

    rt_it = rt_it->next = p_Head(p, currRing);
    //printExp(rt_it->exp, 4*8);
    //printExp(m->exp, 4*8);
    long degP = p_Totaldegree(p, currRing);

    //Print("degP: %d\n", degP);
    //Print("degM: %d\n", degP);

    int sizeP = (2 + degP * (lV+2)) * (sizeof(long)/2);
    int sizeM = degM * (lV+2) * (sizeof(long)/2);
    int startM = sizeof(long);

    //Print("sizeP: %d\n", sizeP);
    //Print("sizeM: %d\n", sizeM);

    //memcpy(rt_it->exp + sizeP, m->exp + startM, sizeM);
    //for(int i = 0; i < degP)
    rt_it->exp[0] *= 2;
    for(int i = 1; i <= 4; ++i)
      rt_it->exp[i+4] = m->exp[i];

    //printExp(rt_it->exp, 4*9);

    pIter(p);
  }

  number coeffM = p_GetCoeff(m, r_m);
  return p_Mult_nn(rt.next, coeffM, r_lm_p, r_t_p);
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



//mm_Mult_pp Part



// returns Copy(m)*Copy(p), does neither destroy p nor m Version
// for non-homog, non-shifted letterplace poly-/monomials
// m should be from r_m, lm(p) from r_lm_p, tail(p) from r_t_p
// output will have lm in r_lm_p and tail in r_t_p
inline poly mm_Mult_pp_normal
  ( poly p, const ring r_lm_p, const ring r_t_p,
    poly m, const ring r_m, int uptodeg, int lV, 
    uint* mDVec = NULL, uint dvSize = 0          )
{
  if( p == NULL || m == NULL ) return NULL;

  //BOCO: TODO: does not care for rings !!!
  //Not very efficient -> I think this is not really used.
  return pp_Mult_qq( p, m, r_t_p );
}

// returns Copy(m)*Copy(p), does neither destroy p nor m Version
// for non-homog, non-shifted letterplace poly-/monomials
// m should be from r_m, lm(p) from r_lm_p, tail(p) from r_t_p
// output will have lm in r_lm_p and tail in r_t_p
inline poly lp_mm_Mult_pp_shift
  ( poly p, const ring r_lm_p, const ring r_t_p,
    poly m, const ring r_m, int uptodeg, int lV, 
    uint* mDVec = NULL, uint dvSize = 0          )
{
  if( p == NULL || m == NULL ) return NULL;

  long degM = p_Totaldegree(m, r_m);

  poly pTemp = p_LPshift( p, degM, uptodeg, lV, r_m );

  //BOCO: pp_Mult_qq seems not to be very efficient:
  //I think for GB-Computations other functions are used.
  poly res = pp_Mult_qq( m, pTemp, currRing );

  p_Delete(&pTemp, currRing);

  return res;
}

// returns Copy(m)*Copy(p), does neither destroy p nor m Version
// for non-homog, non-shifted letterplace poly-/monomials
// m should be from r_m, lm(p) from r_lm_p, tail(p) from r_t_p
// output will have lm in r_lm_p and tail in r_t_p
inline poly lp_mm_Mult_pp_slow
  ( poly p, const ring r_lm_p, const ring r_t_p,
    poly m, const ring r_m, int uptodeg, int lV, 
    uint* mDVec = NULL, uint dvSize = 0          )
{
  if( p == NULL || m == NULL ) return NULL;


  poly rt = p_Head(m, r_m);
  poly rt_it = rt;
  long deg_rt_it = p_Totaldegree(m, r_m) * lV + 1;
  long max_deg_rt_it = lV * uptodeg;
  long m_ind_it = 1;

  {
    nextblock: ;
    for(int j = m_ind_it, i = 0; i < lV; ++i, ++j)
      if( p_GetExp(p, j, r_lm_p) )
      {
        p_SetExp(rt_it, deg_rt_it+i, 1, r_m);
        deg_rt_it += lV;
        m_ind_it += lV;
        if(deg_rt_it > max_deg_rt_it) break;
        goto nextblock;
      }
  }
  p_Setm(rt_it, r_m);
  rt_it->next = NULL;
  number coeffP = p_GetCoeff(p, r_lm_p);
  rt_it = p_Mult_nn(rt_it, coeffP, r_lm_p, r_t_p);

  pIter(p);
  while(p != NULL)
  {
    rt_it = rt_it->next = p_Head(m, r_m);
    deg_rt_it = p_Totaldegree(m, r_m) * lV + 1;
    long m_ind_it = 1;

    {
      nextblock2: ;
      for(int i = 0, j = m_ind_it; i < lV; ++j, ++i)
        if( p_GetExp(p, j, r_t_p) )
        {
          p_SetExp(rt_it, deg_rt_it+i, 1, r_m);
          deg_rt_it += lV;
          m_ind_it += lV;
          if(deg_rt_it > max_deg_rt_it) break;
          goto nextblock2;
        }
    }

    p_Setm(rt_it, r_m);
    rt_it->next = NULL;
    number coeffP = p_GetCoeff(p, r_lm_p);
    rt_it = p_Mult_nn(rt_it, coeffP, r_lm_p, r_t_p);
    pIter(p);
  }

  return rt;
}

// returns Copy(m)*Copy(p), does neither destroy p nor m Version
// for non-homog, non-shifted letterplace poly-/monomials
// m should be from r_m, lm(p) from r_lm_p, tail(p) from r_t_p
// output will have lm in r_lm_p and tail in r_t_p
inline poly lp_mm_Mult_pp_direct
  ( poly p, const ring r_lm_p, const ring r_t_p,
    poly m, const ring r_m, int uptodeg, int lV, 
    uint* mDVec = NULL, uint dvSize = 0          )
{
  //TODO: 
  //- We could use the DVec of the lm of p for a little
  //  improvement.
  //- if we have the totaldegree of m (the size of the dvec),
  //  then we can also improve this a little bit.
  //- As usual we have yet to care for rings.
 
  if( p == NULL || m == NULL ) return NULL;

  poly rt = p_Head(m, r_lm_p);  //Problem 1: r_m == r_lm_p ???
  poly rt_it = rt;
  long deg_rt_it0 = p_Totaldegree(m, r_m) * lV + 1;
  long deg_rt_it = deg_rt_it0;
  long max_deg_rt_it = lV * uptodeg;
  long m_ind_it = 1;

  {
    nextblock: ;
    for(int j = m_ind_it, i = 0; i < lV; ++i, ++j)
      if( p_GetExp(p, j, r_lm_p) )
      {
        p_SetExp(rt_it, deg_rt_it+i, 1, r_lm_p);
        rt_it->exp[omap[deg_rt_it+i]] = 1;
        deg_rt_it += lV;
        m_ind_it += lV;
        if(deg_rt_it > max_deg_rt_it) break;
        goto nextblock;
      }
  }
  rt_it->exp[omap[0]] += p->exp[omap[0]];

  rt_it->next = NULL;
  number coeffP = p_GetCoeff(p, r_lm_p);
  rt_it = p_Mult_nn(rt_it, coeffP, r_lm_p, r_t_p);

  pIter(p);
  while(p != NULL)
  {
    rt_it = rt_it->next = p_Head(m, r_t_p); //Problem 2: ...

    m_ind_it = 1;
    deg_rt_it = deg_rt_it0;

    {
      nextblock2: ;
      for(int i = 0, j = m_ind_it; i < lV; ++j, ++i)
        if( p_GetExp(p, j, r_t_p) )
        {
          p_SetExp(rt_it, deg_rt_it+i, 1, r_t_p);
          rt_it->exp[omap[deg_rt_it+i]] = 1;
          deg_rt_it += lV;
          m_ind_it += lV;
          if(deg_rt_it > max_deg_rt_it) break;
          goto nextblock2;
        }
    }
    rt_it->exp[omap[0]] += p->exp[omap[0]];

    rt_it->next = NULL;
    number coeffP = p_GetCoeff(p, r_t_p);
    rt_it = p_Mult_nn(rt_it, coeffP, r_t_p, r_t_p);

    pIter(p);
  }

  rt_it->next = NULL;
  return rt;
}

// returns Copy(m)*Copy(p), does neither destroy p nor m Version
// for non-homog, non-shifted letterplace poly-/monomials
// m should be from r_m, lm(p) from r_lm_p, tail(p) from r_t_p
// output will have lm in r_lm_p and tail in r_t_p
inline poly lp_mm_Mult_pp_direct2
  ( poly p, const ring r_lm_p, const ring r_t_p,
    poly m, const ring r_m, int uptodeg, int lV, 
    uint* mDVec = NULL, uint dvSize = 0          )
{
  //TODO: 
  //- We could use the DVec of the lm of p for a little
  //  improvement.
  //- As usual we have yet to care for rings, 
  //  see Problem 1 and Problem 2 below.
  //  UPDATE: I think the algorithm needed for the bba is a bit
  //  different - I will have to rewrite everything. So this is
  //  just some test function, which will only work, if all
  //  Rings are the same.

  //We begin with multiplying m * lm(p) (to be more exact we
  //should say: multiplying m with an apropriate shift of lm(p))
 
  if( p == NULL || m == NULL ) return NULL;

  poly rt = p_Head(m, r_lm_p);  //Problem 1: r_m == r_lm_p ???
  poly rt_it = rt;

  //This is the first index in p in the current block
  long m_ind_it = 1;

  //This is the first index of m after the last used block in m
  long deg_rt_it0 = p_Totaldegree(m, r_m) * lV + 1;
  long deg_rt_it = deg_rt_it0;
  long degP = p_Totaldegree(p, r_lm_p);
  {
    nextblock: ;
    for(int j = m_ind_it, i = 0; i < lV; ++i, ++j)
      if( p_GetExp(p, j, r_lm_p) )
      { //Found the exponent in this block!
        p_SetExp(rt_it, deg_rt_it+i, 1, r_lm_p);
        rt_it->exp[omap[deg_rt_it+i]] = 1; //ord field of block
        deg_rt_it += lV; //only one exponent in each block
        m_ind_it += lV; //so we can jump to the next block
        if(!--degP) break; //no more variables in p
        goto nextblock;
      }
  }

  //Multiply with the coefficient of p
  rt_it->next = NULL;
  number coeffP = p_GetCoeff(p, r_lm_p);
  rt_it = p_Mult_nn(rt_it, coeffP, r_lm_p, r_t_p);

  //We still have to update the global ord field:
  rt_it->exp[omap[0]] += p->exp[omap[0]];

  //Now we will multiply m * tail(p) in the same manner

  pIter(p);
  while(p != NULL)
  {
    rt_it = rt_it->next = p_Head(m, r_t_p); //Problem 2: ...

    m_ind_it = 1;
    deg_rt_it = deg_rt_it0;
    degP = p_Totaldegree(p, r_t_p);
    {
      nextblock2: ;
      for(int i = 0, j = m_ind_it; i < lV; ++j, ++i)
        if( p_GetExp(p, j, r_t_p) )
        {
          p_SetExp(rt_it, deg_rt_it+i, 1, r_t_p);
          rt_it->exp[omap[deg_rt_it+i]] = 1;
          deg_rt_it += lV;
          m_ind_it += lV;
          if(!--degP) break;
          goto nextblock2;
        }
    }
    rt_it->exp[omap[0]] += p->exp[omap[0]];

    rt_it->next = NULL;
    number coeffP = p_GetCoeff(p, r_t_p);
    rt_it = p_Mult_nn(rt_it, coeffP, r_t_p, r_t_p);

    pIter(p);
  }

  return rt;
}

// returns Copy(m)*Copy(p), does neither destroy p nor m Version
// for non-homog, non-shifted letterplace poly-/monomials
// m should be from r_m, lm(p) from r_lm_p, tail(p) from r_t_p
// output will have lm in r_lm_p and tail in r_t_p
// This one does not work!!!
inline poly lp_mm_Mult_pp_direct3
  ( poly p, const ring r_lm_p, const ring r_t_p,
    poly m, const ring r_m, int uptodeg, int lV, 
    uint* mDVec = NULL, uint dvSize = 0          )
{
  //TODO: 
  //- We could use the DVec of the lm of p for a little
  //  improvement.
  //- As usual we have yet to care for rings, 
  //  see Problem 1 and Problem 2 below.
  //  UPDATE: I think the algorithm needed for the bba is a bit
  //  different - I will have to rewrite everything. So this is
  //  just some test function, which will only work, if all
  //  Rings are the same.

  //We begin with multiplying m * lm(p) (to be more exact we
  //should say: multiplying m with an apropriate shift of lm(p))
 
  if( p == NULL || m == NULL ) return NULL;

  poly rt = p_Head(m, r_lm_p);  //Problem 1: r_m == r_lm_p ???
  poly rt_it = rt;

  //This is the first index in p in the current block
  long m_ind_it = 1;

  //This is the first index of m after the last used block in m
  //long deg_rt_it0 = p_Totaldegree(m, r_m) * lV + 1;
  long deg_rt_it0 = (p->exp[omap[0]]) * lV + 1;
  long deg_rt_it = deg_rt_it0;
  //long degP = p_Totaldegree(p, r_lm_p);
  long degP = p->exp[omap[0]];
  {
    nextblock: ;
    for(int j = m_ind_it, i = 0; i < lV; ++i, ++j)
      if( p_GetExp(p, j, r_lm_p) )
      { //Found the exponent in this block!
        p_SetExp(rt_it, deg_rt_it+i, 1, r_lm_p);
        rt_it->exp[omap[deg_rt_it+i]] = 1; //ord field of block
        deg_rt_it += lV; //only one exponent in each block
        m_ind_it += lV; //so we can jump to the next block
        if(!--degP) break; //no more variables in p
        goto nextblock;
      }
  }

  //Multiply with the coefficient of p
  rt_it->next = NULL;
  number coeffP = p_GetCoeff(p, r_lm_p);
  rt_it = p_Mult_nn(rt_it, coeffP, r_lm_p, r_t_p);

  //We still have to update the global ord field:
  rt_it->exp[omap[0]] += p->exp[omap[0]];

  //Now we will multiply m * tail(p) in the same manner

  pIter(p);
  while(p != NULL)
  {
    rt_it = rt_it->next = p_Head(m, r_t_p); //Problem 2: ...

    m_ind_it = 1;
    deg_rt_it = deg_rt_it0;
    //degP = p_Totaldegree(p, r_t_p);
    degP = p->exp[omap[0]];
    {
      nextblock2: ;
      for(int i = 0, j = m_ind_it; i < lV; ++j, ++i)
        if( p_GetExp(p, j, r_t_p) )
        {
          p_SetExp(rt_it, deg_rt_it+i, 1, r_t_p);
          rt_it->exp[omap[deg_rt_it+i]] = 1;
          deg_rt_it += lV;
          m_ind_it += lV;
          if(!--degP) break;
          goto nextblock2;
        }
    }
    rt_it->exp[omap[0]] += p->exp[omap[0]];

    rt_it->next = NULL;
    number coeffP = p_GetCoeff(p, r_t_p);
    rt_it = p_Mult_nn(rt_it, coeffP, r_t_p, r_t_p);

    pIter(p);
  }

  return rt;
}
