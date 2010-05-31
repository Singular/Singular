#undef PR_DELETE_MONOM
#if PR_DELETE_SRC > 0
#define PR_DELETE_MONOM(src, r_src)             \
do                                              \
{                                               \
  PR_NDELETE(&pGetCoeff(src), r_src);           \
  p_LmFree(src, r_src);            \
}                                               \
while (0)
#else
#define PR_DELETE_MONOM(src, r_src) ((void)0)
#endif

#undef PR_INIT_EVECTOR_COPY
#undef PR_CPY_EVECTOR
#undef PR_ALLOC_MONOM
#if PR_RING_EQUAL > 0
#undef PR_NO_SORT
#define PR_NO_SORT 1
#define PR_INIT_EVECTOR_COPY(r_src, r_dest) ((void)0)
#define PR_CPY_EVECTOR(dest, dest_r, src, src_r) \
  omMemcpyW(dest->exp, src->exp, dest_r->ExpL_Size)
#define PR_ALLOC_MONOM(r) p_New(r) 
#undef PR_ALLOC_MONOM
#define PR_ALLOC_MONOM(r) p_Init(r) 
#else
#define  PR_INIT_EVECTOR_COPY(r_src, r_dest) int _min = si_min(r_dest->N, r_src->N)
#define  PR_CPY_EVECTOR(dest, dest_r, src, src_r) \
  prCopyEvector(dest, dest_r, src, src_r, _min)
#define PR_ALLOC_MONOM(r) p_Init(r) 
#endif

#undef PR_NCOPY
#undef PR_NDELETE
#undef PR_NUMBER_SIMPLE_NAME
#if PR_NUMBER_SIMPLE > 0
#define PR_NCOPY(n, r) n
#define PR_NDELETE(n, r) ((void)0)
#define PR_NUMBER_SIMPLE_NAME NSimple
#else
#define PR_NCOPY(n, r) n_Copy(n,r)
#define PR_NDELETE(n, r) n_Delete(n,r)
#define PR_NUMBER_SIMPLE_NAME NoNSimple
#endif

#undef PR_SORT_POLY
#if PR_NO_SORT > 0
#define PR_SORT_POLY(p, d_r, s_r) ((void)0)
#else
#define PR_SORT_POLY(p, d_r, s_r) p = p_SortMerge(p, d_r, d_r->OrdSgn == s_r->OrdSgn)
#endif

