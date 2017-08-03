/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/*
* ABSTRACT: resolutions
*/

#include <kernel/GBEngine/syz.h>
#include <omalloc/omalloc.h>
#include <coeffs/numbers.h>
#include <kernel/polys.h>
#include <kernel/ideals.h>

#include <vector>
#include <map>

#ifdef __GNUC__
#define likely(X)   (__builtin_expect(X, 1))
#define unlikely(X) (__builtin_expect(X, 0))
#else
#define likely(X)   (X)
#define unlikely(X) (X)
#endif

static inline void update_variables(std::vector<bool> &variables,
    const ideal L)
{
    const ring R = currRing;
    const int l = IDELEMS(L)-1;
    int k;
    for (int j = R->N; j > 0; j--) {
        if (variables[j-1]) {
            for (k = l; k >= 0; k--) {
                if (p_GetExp(L->m[k], j, R) > 0) {
                    break;
                }
            }
            if (k < 0) {   // no break
                variables[j-1] = false;
            }
        }
    }
}

static inline bool check_variables(const std::vector<bool> &variables,
    const poly m)
{
    const ring R = currRing;
    for (int j = R->N; j > 0; j--) {
        if (variables[j-1] && p_GetExp(m, j, R) > 0) {
            return true;
        }
    }
    return false;
}

typedef struct {
    poly lt;
    unsigned long sev;
    unsigned int comp;
} lt_struct;

typedef std::vector<lt_struct> lts_vector;
typedef std::map<long, lts_vector> lts_hash;

static void initialize_lts_hash(lts_hash &C, const ideal L)
{
    const ring R = currRing;
    const unsigned int n_elems = IDELEMS(L);
    for (unsigned int k = 0; k < n_elems; k++) {
        const poly a = L->m[k];
        C[p_GetComp(a, R)].push_back((lt_struct){a, p_GetShortExpVector(a, R),
            k});
    }
}

#define delete_lts_hash(C) C->clear()

poly FindReducer(const poly multiplier, const poly t, const lts_hash *m_div)
{
  const ring r = currRing;
  lts_hash::const_iterator m_itr = m_div->find(p_GetComp(t, currRing));
  if (m_itr == m_div->end()) {
    return NULL;
  }
  lts_vector::const_iterator m_current = (m_itr->second).begin();
  lts_vector::const_iterator m_finish  = (m_itr->second).end();
  const poly q = p_New(r);
  pNext(q) = NULL;
  p_MemSum_LengthGeneral(q->exp, multiplier->exp, t->exp, r->ExpL_Size);
  const unsigned long m_not_sev = ~p_GetShortExpVector(q, r);
  for( ; m_current != m_finish; ++m_current) {
    if (m_current->sev & m_not_sev
        || unlikely(!(_p_LmDivisibleByNoComp(m_current->lt, q, r)))) {
        continue;
    }
    const poly p = m_current->lt;
    const int k  = m_current->comp;
    p_MemAdd_NegWeightAdjust(q, r);
    p_ExpVectorDiff(q, q, p, r); // (LM(product) / LM(L[k]))
    p_SetComp(q, k + 1, r);
    p_Setm(q, r);
    number n = n_Mult(p_GetCoeff(multiplier, r), p_GetCoeff(t, r), r);
    p_SetCoeff0(q, n_InpNeg(n, r), r);
    return q;
  }
  p_LmFree(q, r);
  return NULL;
}

static poly TraverseTail_test(poly multiplier, const int tail,
   const ideal previous_module, const std::vector<bool> &variables,
   const lts_hash *m_div);

static inline poly ReduceTerm_test(poly multiplier, poly term4reduction,
    const ideal previous_module, const std::vector<bool> &variables,
    const lts_hash *m_div)
{
  const ring r = currRing;
  poly s = FindReducer(multiplier, term4reduction, m_div);
  if( s == NULL )
  {
    return NULL;
  }
  const int c = p_GetComp(s, r) - 1;
  const poly t
      = TraverseTail_test(s, c, previous_module, variables, m_div);
  if( t != NULL )
    s = p_Add_q(s, t, r);
  return s;
}

static poly ComputeImage_test(poly multiplier, const int t,
    const ideal previous_module, const std::vector<bool> &variables,
    const lts_hash *m_div)
{
  const poly tail = previous_module->m[t]->next;
  if(tail != NULL)
  {
    if (!check_variables(variables, multiplier))
    {
      return NULL;
    }
    sBucket_pt sum = sBucketCreate(currRing);
    for(poly p = tail; p != NULL; p = pNext(p))   // iterate over the tail
    {
      const poly rt = ReduceTerm_test(multiplier, p, previous_module,
          variables, m_div);
      sBucket_Add_p(sum, rt, pLength(rt));
    }
    poly s;
    int l;
    sBucketClearAdd(sum, &s, &l);
    sBucketDestroy(&sum);
    return s;
  }
  return NULL;
}

#define CACHE 1

#if CACHE
struct CCacheCompare_test
{
  inline bool operator() (const poly& l, const poly& r)
    const
  {
    return (p_LmCmp(l, r, currRing) == -1);
  }
};

typedef std::map<poly, poly, CCacheCompare_test>
  TP2PCache_test;
typedef std::map<int, TP2PCache_test> TCache_test;

static TCache_test m_cache_test;

// note: we don't need to keep the coeffs of those terms which are lifted to 0

static void delete_cache()
{
    for (TCache_test::iterator it = m_cache_test.begin();
        it != m_cache_test.end(); it++) {
        TP2PCache_test& T = it->second;
        for (TP2PCache_test::iterator vit = T.begin(); vit != T.end(); vit++) {
            p_Delete((&(vit->second)), currRing);
            p_Delete(const_cast<poly*>(&(vit->first)), currRing);
        }
        T.clear();
    }
    m_cache_test.clear();
}
#endif   // CACHE

static poly TraverseTail_test(poly multiplier, const int tail,
    const ideal previous_module, const std::vector<bool> &variables,
    const lts_hash *m_div)
{
  const ring& r = currRing;
#if CACHE
  TCache_test::iterator top_itr = m_cache_test.find(tail);
  if ( top_itr != m_cache_test.end() )
  {
     TP2PCache_test& T = top_itr->second;
     TP2PCache_test::iterator itr = T.find(multiplier);
     if( itr != T.end() )
     {
       if( itr->second == NULL )
         return (NULL);
       poly p = p_Copy(itr->second, r);
       if( !n_Equal( pGetCoeff(multiplier), pGetCoeff(itr->first), r) )
       {
         number n = n_Div( pGetCoeff(multiplier), pGetCoeff(itr->first), r);
         p = p_Mult_nn(p, n, r);
         n_Delete(&n, r);
       }
       return p;
     }
     const poly p = ComputeImage_test(multiplier, tail, previous_module,
         variables, m_div);
     itr = T.find(multiplier);
     if( itr == T.end() )
     {
       T.insert(TP2PCache_test::value_type(p_Head(multiplier,
         r), p) );
       return p_Copy(p, r);
     }
     return p;
  }
#endif   // CACHE
  const poly p = ComputeImage_test(multiplier, tail, previous_module,
      variables, m_div);
#if CACHE
  top_itr = m_cache_test.find(tail);
  if ( top_itr != m_cache_test.end() )
  {
    TP2PCache_test& T = top_itr->second;
    TP2PCache_test::iterator itr = T.find(multiplier);
    if( itr == T.end() )
    {
      T.insert(TP2PCache_test::value_type(p_Head(multiplier,
          r), p));
      return p_Copy(p, r);
    }
    return p;
  }
  TP2PCache_test T;
  T.insert(TP2PCache_test::value_type(p_Head(multiplier, r),
    p));
  m_cache_test.insert( TCache_test::value_type(tail, T) );
  return p_Copy(p, r);
#else
  return p;
#endif   // CACHE
}

static poly lift_ext_LT(const poly a, const ideal previous_module,
    const std::vector<bool> &variables, const lts_hash *m_div)
{
  const ring R = currRing;
  poly t1 = ComputeImage_test(a, p_GetComp(a, R)-1,
      previous_module, variables, m_div);
  poly t2 = TraverseTail_test(a->next, p_GetComp(a->next, R)-1,
      previous_module, variables, m_div);
  t1 = p_Add_q(t1, t2, R);
  return t1;
}

/*****************************************************************************/

// copied from id_DelDiv(), but without testing and without HAVE_RINGS.
// delete id[j], if LT(j) == coeff*mon*LT(i) and vice versa, i.e.,
// delete id[i], if LT(i) == coeff*mon*LT(j)
static void id_DelDiv_no_test(ideal id, const ring r)
{
    int i, j;
    int k = IDELEMS(id)-1;
    for (i = k; i >= 0; i--) {
        if (id->m[i] != NULL) {
            for (j = k; j > i; j--) {
                if (id->m[j] != NULL) {
                    if (p_DivisibleBy(id->m[i], id->m[j], r)) {
                        p_Delete(&id->m[j], r);
                    }
                    else if (p_DivisibleBy(id->m[j], id->m[i], r)) {
                        p_Delete(&id->m[i], r);
                        break;
                    }
                }
            }
        }
    }
}

typedef poly syzHeadFunction(ideal, int, int);

static poly syzHeadFrame(const ideal G, const int i, const int j)
{
    const ring r = currRing;
    const poly f_i = G->m[i];
    const poly f_j = G->m[j];
    poly head = p_Init(r);
    pSetCoeff0(head, n_Init(1, r->cf));
    long exp_i, exp_j, lcm;
    for (int k = (int)r->N; k > 0; k--) {
        exp_i = p_GetExp(f_i, k, r);
        exp_j = p_GetExp(f_j, k, r);
        lcm = si_max(exp_i, exp_j);
        p_SetExp(head, k, lcm-exp_i, r);
    }
    p_SetComp(head, i+1, r);
    p_Setm(head, r);
    return head;
}

static poly syzHeadExtFrame(const ideal G, const int i, const int j)
{
    const ring r = currRing;
    const poly f_i = G->m[i];
    const poly f_j = G->m[j];
    poly head = p_Init(r);
    pSetCoeff0(head, n_Init(1, r->cf));
    poly head_ext = p_Init(r);
    pSetCoeff0(head_ext, n_InpNeg(n_Div(pGetCoeff(f_j), pGetCoeff(f_i), r->cf),
        r->cf));
    long exp_i, exp_j, lcm;
    for (int k = (int)r->N; k > 0; k--) {
        exp_i = p_GetExp(f_i, k, r);
        exp_j = p_GetExp(f_j, k, r);
        lcm = si_max(exp_i, exp_j);
        p_SetExp(head, k, lcm-exp_i, r);
        p_SetExp(head_ext, k, lcm-exp_j, r);
    }
    p_SetComp(head, i+1, r);
    p_Setm(head, r);
    p_SetComp(head_ext, j+1, r);
    p_Setm(head_ext, r);
    head->next = head_ext;
    return head;
}

typedef ideal syzM_i_Function(ideal, int, syzHeadFunction);

static ideal syzM_i_unsorted(const ideal G, const int i,
    syzHeadFunction *syzHead)
{
    ideal M_i = NULL;
    int comp = pGetComp(G->m[i]);
    int ncols = 0;
    for (int j = i-1; j >= 0; j--) {
        if (pGetComp(G->m[j]) == comp) ncols++;
    }
    if (ncols > 0) {
        M_i = idInit(ncols, G->ncols);
        int k = ncols-1;
        for (int j = i-1; j >= 0; j--) {
            if (pGetComp(G->m[j]) == comp) {
                M_i->m[k] = syzHead(G, i, j);
                k--;
            }
        }
        id_DelDiv_no_test(M_i, currRing);
        idSkipZeroes(M_i);
    }
    return M_i;
}

static ideal syzM_i_sorted(const ideal G, const int i,
    syzHeadFunction *syzHead)
{
    ideal M_i = NULL;
    int comp = pGetComp(G->m[i]);
    int index = i-1;
    while (pGetComp(G->m[index]) == comp) index--;
    index++;
    int ncols = i-index;
    if (ncols > 0) {
        M_i = idInit(ncols, G->ncols);
        for (int j = ncols-1; j >= 0; j--) {
            M_i->m[j] = syzHead(G, i, j+index);
        }
        id_DelDiv_no_test(M_i, currRing);
        idSkipZeroes(M_i);
    }
    return M_i;
}

static ideal idConcat(const ideal* M, const int size, const int rank)
{
    int ncols = 0;
    for (int i = size-1; i >= 0; i--) {
        if (M[i] != NULL) {
            ncols += M[i]->ncols;
        }
    }
    if (ncols == 0) return idInit(1, rank);
    ideal result = idInit(ncols, rank);
    int k = ncols-1;
    for (int i = size-1; i >= 0; i--) {
        if (M[i] != NULL) {
            for (int j = M[i]->ncols-1; j >= 0; j--) {
                result->m[k] = M[i]->m[j];
                k--;
            }
        }
    }
    return result;
}

#define SORT_MI 1

#if SORT_MI
static int compare_Mi(const void* a, const void *b)
{
    const ring r = currRing;
    poly p_a = *((poly *)a);
    poly p_b = *((poly *)b);
    int cmp;
    int comp_a = p_GetComp(p_a, r);
    int comp_b = p_GetComp(p_b, r);
    cmp = (comp_a > comp_b) - (comp_a < comp_b);
    if (cmp != 0) {
         return cmp;
    }
    int deg_a = p_Deg(p_a, r);
    int deg_b = p_Deg(p_b, r);
    cmp = (deg_a > deg_b) - (deg_a < deg_b);
    if (cmp != 0) {
         return cmp;
    }
    int exp_a[r->N+1];
    int exp_b[r->N+1];
    p_GetExpV(p_a, exp_a, r);
    p_GetExpV(p_b, exp_b, r);
    for (int i = r->N; i > 0; i--) {
        cmp = (exp_a[i] > exp_b[i]) - (exp_a[i] < exp_b[i]);
        if (cmp != 0) {
            return cmp;
        }
    }
    return 0;
}
#endif   // SORT_MI

static ideal computeFrame(const ideal G, syzM_i_Function syzM_i,
    syzHeadFunction *syzHead)
{
    ideal* M = (ideal *)omalloc((G->ncols-1)*sizeof(ideal));
    for (int i = G->ncols-2; i >= 0; i--) {
        M[i] = syzM_i(G, i+1, syzHead);
    }
    ideal frame = idConcat(M, G->ncols-1, G->ncols);
    for (int i = G->ncols-2; i >= 0; i--) {
        if (M[i] != NULL) {
            omFreeSize(M[i]->m, IDELEMS(M[i])*sizeof(poly));
            omFreeBin(M[i], sip_sideal_bin);
        }
    }
    omFree(M);
#if SORT_MI
    qsort(frame->m, IDELEMS(frame), sizeof(poly), compare_Mi);
#endif
    return frame;
}

static void computeLiftings(const resolvente res, const int index,
    std::vector<bool> &variables)
{
    if (index > 1) {   // we don't know if the input is a reduced SB
        update_variables(variables, res[index-1]);
    }
    lts_hash *hash_previous_module = new lts_hash();
    initialize_lts_hash(*hash_previous_module, res[index-1]);
    for (int j = res[index]->ncols-1; j >= 0; j--) {
        res[index]->m[j]->next->next = lift_ext_LT(res[index]->m[j],
            res[index-1], variables, hash_previous_module);
    }
    delete_lts_hash(hash_previous_module);
    delete(hash_previous_module);
#if CACHE
    delete_cache();
#endif   // CACHE
}

static int computeResolution(resolvente &res, const int length,
    syzHeadFunction *syzHead)
{
    int max_index = length-1;
    int index = 0;
    if (!idIs0(res[index]) && index < max_index) {
        index++;
        res[index] = computeFrame(res[index-1], syzM_i_unsorted, syzHead);
        std::vector<bool> variables;
        variables.resize(currRing->N, true);
        while (!idIs0(res[index])) {
#if 1
            computeLiftings(res, index, variables);
#endif   // LIFT
            if (index < max_index) {
                index++;
                res[index] = computeFrame(res[index-1], syzM_i_sorted,
                    syzHead);
            }
            else {
                break;
            }
        }
        variables.clear();
    }
    max_index = index+1;
    if (max_index < length) {
        res = (resolvente)omReallocSize(res, (length+1)*sizeof(ideal),
            (max_index+1)*sizeof(ideal));
    }
    return max_index;
}

#define insert_first_term(r, p, q, R)                             \
do                                                                \
{                                                                 \
    p = r;                                                        \
    q = p->next;                                                  \
    if (q != NULL && p_LmCmp(p, q, R) != 1) {                     \
        while (q->next != NULL && p_LmCmp(p, q->next, R) == -1) { \
            pIter(q);                                             \
        }                                                         \
        r = p->next;                                              \
        p->next = q->next;                                        \
        q->next = p;                                              \
    }                                                             \
}                                                                 \
while (0)

static void insert_ext_induced_LTs(const resolvente res, const int length)
{
    const ring R = currRing;
    poly p, q;
    for (int i = length-2; i > 0; i--) {
        for (int j = res[i]->ncols-1; j >= 0; j--) {
            insert_first_term(res[i]->m[j]->next, p, q, R);
            insert_first_term(res[i]->m[j], p, q, R);
        }
    }
}

syStrategy syFrank(const ideal arg, int &length, const char *method)
{
    syStrategy result = (syStrategy)omAlloc0(sizeof(ssyStrategy));
    resolvente res = (resolvente)omAlloc0((length+1)*sizeof(ideal));
    res[0] = id_Copy(arg, currRing);
    syzHeadFunction *syzHead;
    if (strcmp(method, "frame") == 0 || strcmp(method, "linear strand") == 0) {
        syzHead = syzHeadFrame;
    }
    else {
        syzHead = syzHeadExtFrame;
    }
    length = computeResolution(res, length, syzHead);
    insert_ext_induced_LTs(res, length);
    result->fullres = res;
    result->length = length;
    return result;
}

