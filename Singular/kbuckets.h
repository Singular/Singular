#ifndef KBUCKETS_H
#define KBUCKETS_H

/////////////////////////////////////////////////////////////////////////
// configuration
//

// define to enable bucket reductions
#define HAVE_BUCKETS

// define to enable deep profiles
// #define DO_DEEP_PROFILE

#ifdef HAVE_BUCKETS
// define to enable fast, specialized bucket procedures
// #define FAST_BUCKETS

#include "mmheap.h"

#if defined(HEAP_DEBUG) || ((! defined(PDEBUG)) && (! defined(MDEBUG)))
// define to enable heap memory management
// #define BUCKETS_USE_HEAPS
#endif

#if (! defined(PDEBUG)) && (! defined(MDEBUG))
// define to enable shallow memory management
// define special memory management of leading terms
#define HAVE_LM_MM
#endif

// define to use Spolyloops with buckets
#define BUCKETS_USE_SPOLYLOOP

// define to enable kbassume
// #define BUCKETS_DEBUG

// Do not use fast buckets for deep profiles
#ifdef DO_DEEP_PROFILE
#undef FAST_BUCKETS
#endif

#ifndef NDEBUG
#undef BUCKETS_DEBUG
#define BUCKETS_DEBUG
#endif


/////////////////////////////////////////////////////////////////////////
// declarations
//

// Creation of buckets
void kBucketDestroy(kBucket_pt *bucket);
kBucket_pt kBucketCreate(ring r, rOrderType_t ot, BOOLEAN homog = FALSE);
inline kBucket_pt kBucketCreate(ring r, int modrank, int syzComp, 
                                BOOLEAN homog)
{
  return kBucketCreate(r, spGetOrderType(r, modrank, syzComp), homog);
}

inline kBucket_pt kBucketCreate(ring r, kStrategy strat, int syzComp)
{
  if (strat->bucket != NULL)
  {
    kBucketDestroy(&(strat->bucket));
  }
  return kBucketCreate(r, spGetOrderType(r, strat->ak, syzComp),
                       (strat->homog && strat->kModW==NULL &&
                         ! rHasSimpleLexOrder(r)));
}

inline kBucket_pt kBucketCreate(ring r, kStrategy strat)
{
  if (strat->bucket != NULL)
  {
    kBucketDestroy(&(strat->bucket));
  }
  return kBucketCreate(r, spGetOrderType(r, strat->ak, strat->syzComp),
                       ((strat->homog && strat->kModW==NULL &&
                         ! rHasSimpleLexOrder(r))));
}

void kBucketInit(kBucket_pt bucket, poly p, BOOLEAN del = TRUE);
void kBucketClear();
poly kBucketExtractLm(kBucket_pt bucket);
void kBucketCanonicalize(kBucket_pt bucket, 
                         poly *p, unsigned int length,
                         poly lm = NULL);
void kBucketSpolyRed(kBucket_pt bucket,
                     poly p1, unsigned int l1, 
                     poly lm, poly spNoether);

extern void redHomog (LObject* h,kStrategy strat);
extern  void redHoney (LObject*  h,kStrategy strat);

/* Addition procedure buisness */
typedef poly (*kbAddProc)(poly a1, const unsigned int l1, 
                          poly a2, const unsigned int l2, 
                          unsigned int *length);
extern poly kbAdd_General(poly a1, const unsigned int l1, 
                          poly a2, const unsigned int l2, 
                          unsigned int *length);
extern kbAddProc GetkbAddProc(ring r, rOrderType_t ot, BOOLEAN homog);
extern void kbSetAddProc(ring r, rOrderType_t rot, BOOLEAN homog);

/* Multiplication procedure business */
typedef poly (*kbMultProc)(poly a1, poly a2);
extern poly  kbMult_General(poly a1, poly lm);
extern kbMultProc GetkbMultProc(ring r, rOrderType_t ot, BOOLEAN homog);
extern kbMultProc kbmult;

#ifdef BUCKETS_USE_SPOLYLOOP
/* spolyloop procedure business */
typedef unsigned int (*kbSpolyLoopProc)(poly a1, const unsigned int l1, 
                                        poly a2, const unsigned int l2, 
                                        poly monom, poly spNoether);
extern unsigned int kbSpolyLoop_General(poly a1, const unsigned int l1, 
                                        poly a2, const unsigned int l2,
                                        poly monom, poly spNoether);
extern kbSpolyLoopProc GetkbSpolyLoopProc(ring r, rOrderType_t ot, BOOLEAN homog);
#endif // BUCKETS_USE_SPOLYLOOP

/////////////////////////////////////////////////////////////////////////
// Memory management
//
#ifdef BUCKETS_USE_HEAPS

#define kb_pNew(p)                  mmh_alloc_monom(p)
#define kb_pFree1(p)                mmh_free_monom(p)

#else // ! BUCKETS_USE_HEAPS

#define kb_pNew(p)                  p = pNew()
#define kb_pFree1(p)                pFree1(p)

#endif // BUCKETS_USE_HEAPS

#define kb_pFree1AndAdvance(p)                  \
do                                              \
{                                               \
  poly __p = p;                                 \
  p = pNext(__p);                               \
  kb_pFree1(__p);                               \
}                                               \
while(0)

#define kb_pDelete1AndAdvance(p)                \
do                                              \
{                                               \
  nDelete(&(pGetCoeff(p)));                     \
  kb_pFree1AndAdvance(p);                       \
}                                               \
while (0)

#define kb_pDelete1(p)                \
do                                              \
{                                               \
  nDelete(&(pGetCoeff(p)));                     \
  kb_pFree1(p);                       \
}                                               \
while (0)

#define memadd(P1, P2, P3, L)                   \
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

#define memadd_nwODD(P1, P2, P3, L)             \
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
          
#define memadd_nwEVEN(P1, P2, P3, L)            \
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
  
#define memadd_nwONE(P1, P2, P3)                \
do                                              \
{                                               \
  unsigned long* _p1 = P1;                      \
  const unsigned long* _p2 = P2;                \
  const unsigned long* _p3 = P3;                \
                                                \
 *_p1 = *_p2 + *_p3;                            \
}                                               \
while(0)

#define memadd_nwTWO(P1, P2, P3)                \
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


#ifndef BUCKETS_DEBUG
/* empty macro, if NDEBUG */
#define kbassume(x) ((void*) 0)
#else /* BUCKETS_DEBUG */
#define kbassume(x) assume(x)
#if 0
#define kbassume(x) _kbassume(x, __FILE__, __LINE__)
#define _kbassume(x, f, l)                                        \
do                                                              \
{                                                               \
  if (! (x))                                                    \
  {                                                             \
    Warn("Internal assume violation: file %s line %d\n", f, l); \
  }                                                             \
}                                                               \
while (0)
#endif
#endif /* ! BUCKETS_DEBUG*/

#endif /* HAVE_BUCKETS */

#endif /* KBUCKETS_H */
