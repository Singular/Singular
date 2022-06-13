/***************************************
 *  Computer Algebra System SINGULAR   *
 ***************************************/
/*
 * ABSTRACT: resolutions
 * reference: https://arxiv.org/abs/1502.01654
 */

#include "kernel/GBEngine/syz.h"
#include "coeffs/numbers.h"
#include "kernel/polys.h"
#include "kernel/ideals.h"

#include <vector>
#include <map>

/*
 * set variables[i] to false if the i-th variable does not appear among the
 * leading terms of L
 */
static void update_variables(std::vector<bool> &variables, const ideal L)
{
    const ring R = currRing;
    const int l = L->ncols-1;
    int k;
    for (int j = R->N; j > 0; j--)
    {
        if (variables[j-1])
        {
            for (k = l; k >= 0; k--)
            {
                if (p_GetExp(L->m[k], j, R) > 0)
                {
                    break;
                }
            }
            if (k < 0)
            {   // no break
                variables[j-1] = false;
            }
        }
    }
}

/*
 * If the previous step in the resolution is reduced, then this check can be
 * used to determine lower order terms.
 */
static inline bool check_variables(const std::vector<bool> &variables,
        const poly m)
{
    const ring R = currRing;
    // variables[R->N] is true iff index == 1, that is, for the first step in
    // the resolution
    if (UNLIKELY(variables[R->N]))
    {
        return true;
    }
    for (int j = R->N; j > 0; j--)
    {
        if (UNLIKELY(variables[j-1] && p_GetExp(m, j, R) > 0))
        {
            return true;
        }
    }
    return false;
}

/*
 * For each step in the resolution, the following data is saved for each of the
 * induced leading terms: the leading term itself, its short exponent vector,
 * and its position in the ideal/module.
 */
typedef struct {
    poly lt;
    unsigned long sev;
    unsigned long comp;
} lt_struct;

static void initialize_hash(lt_struct **C, const ideal L)
{
    const ring R = currRing;
    const unsigned long n_elems = L->ncols;
    unsigned int *count
        = (unsigned int *)omAlloc0((L->rank+1)*sizeof(unsigned int));
    unsigned long k = 0;
    while (k < n_elems)
    {
        count[__p_GetComp(L->m[k], R)]++;
        k++;
    }
    for (int i = 0; i <= L->rank; i++)
    {
        // do ++count[i] and use C[i][0].comp to save count[i]
        C[i] = (lt_struct *)omalloc0((++count[i])*sizeof(lt_struct));
        C[i][0].comp = count[i];
    }
    k = n_elems;
    // the order of the elements in each C[i] matters if check_variables() is
    // to be used
    while (k > 0)
    {
        const poly a = L->m[k-1];
        const unsigned long comp = __p_GetComp(a, R);
        C[comp][--count[comp]]
            = (lt_struct){a, p_GetShortExpVector(a, R), k};
        k--;
    }
    omFree(count);
}

/*
 * compute a new term in the resolution, that is, compute
 * ( t * multiplier / f ) where f is an induced leading term from the previous
 * module, or return NULL if no such f dividing t * multiplier exists, that is,
 * if multiplier is a lower order term
 */
static poly find_reducer(const poly multiplier, const poly t,
        const lt_struct *const *const hash_previous_module)
{
    const ring r = currRing;
    const lt_struct *v = hash_previous_module[__p_GetComp(t, r)];
    unsigned long count = v[0].comp;
    if (UNLIKELY(count == 1))
    {
        return NULL;
    }
    const poly q = p_New(r);
    pNext(q) = NULL;
    p_MemSum_LengthGeneral(q->exp, multiplier->exp, t->exp, r->ExpL_Size);
    const unsigned long q_not_sev = ~p_GetShortExpVector(q, r);
    for(unsigned long i = 1; i < count; i++)
    {
        if (LIKELY(v[i].sev & q_not_sev)
                || UNLIKELY(!(_p_LmDivisibleByNoComp(v[i].lt, q, r))))
                {
            continue;
        }
        p_MemAdd_NegWeightAdjust(q, r);
        p_ExpVectorDiff(q, q, v[i].lt, r);
        p_SetComp(q, v[i].comp, r);
        p_Setm(q, r);
        number n = n_Div(p_GetCoeff(multiplier, r), p_GetCoeff(v[i].lt, r), r->cf);
        n_InpMult(n, p_GetCoeff(t, r), r->cf);
        p_SetCoeff0(q, n_InpNeg(n, r->cf), r);
        return q;
    }
    p_LmFree(q, r);
    return NULL;
}

static poly traverse_tail(const poly multiplier, const int comp,
        const ideal previous_module, const std::vector<bool> &variables,
        const lt_struct *const *const hash_previous_module);

static poly compute_image(const poly multiplier, const int comp,
        const ideal previous_module, const std::vector<bool> &variables,
        const lt_struct *const *const hash_previous_module,
        const bool use_cache);

/*
 * recursively call traverse_tail() for each new term found by find_reducer()
 */
static poly reduce_term(const poly multiplier, const poly term,
        const ideal previous_module, const std::vector<bool> &variables,
        const lt_struct *const *const hash_previous_module,
        const bool use_cache)
{
    poly s = find_reducer(multiplier, term, hash_previous_module);
    if (s == NULL)
    {
        return NULL;
    }
    const ring r = currRing;
    const int c = __p_GetComp(s, r) - 1;
    poly t;
    if (use_cache)
    {
        t = traverse_tail(s, c, previous_module, variables,
                hash_previous_module);
    } else {
        t = compute_image(s, c, previous_module, variables,
                hash_previous_module, false);
    }
    return p_Add_q(s, t, r);
}

/*
 * iterating over tail, call reduce_term(multiplier, p, ...) for each term p in
 * tail and sum up the results
 */
static poly compute_image(const poly multiplier, const int comp,
        const ideal previous_module, const std::vector<bool> &variables,
        const lt_struct *const *const hash_previous_module,
        const bool use_cache)
{
    const poly tail = previous_module->m[comp]->next;
    if (UNLIKELY(tail == NULL) || !check_variables(variables, multiplier))
    {
        return NULL;
    }
    sBucket_pt sum = sBucketCreate(currRing);
    for (poly p = tail; p != NULL; p = pNext(p))
    {
        const poly rt = reduce_term(multiplier, p, previous_module, variables,
                hash_previous_module, use_cache);
        sBucket_Add_p(sum, rt, pLength(rt));
    }
    poly s;
    int l;
    sBucketClearAdd(sum, &s, &l);
    sBucketDestroy(&sum);
    return s;
}

struct cache_compare
{
    inline bool operator() (const poly& l, const poly& r) const
    {
        return (p_LmCmp(l, r, currRing) == -1);
        /* For expensive orderings, consider:
         * return (memcmp(l->exp, r->exp,
         *         (currRing->CmpL_Size)*sizeof(unsigned long)) < 0);
         */
    }
};

typedef std::map<poly, poly, cache_compare> cache_term;

STATIC_VAR cache_term *Cache;

static void initialize_cache(const int size)
{
    Cache = new cache_term[size];
}

static void delete_cache(const int size)
{
    const ring r = currRing;
    for (int i = 0; i < size; i++)
    {
        cache_term *T = &(Cache[i]);
        for (cache_term::iterator itr = T->begin(); itr != T->end(); ++itr)
        {
            p_Delete(&(itr->second), r);
            p_Delete(const_cast<poly*>(&(itr->first)), r);
        }
        T->clear();
    }
    delete[](Cache);
}

static void insert_into_cache_term(cache_term *T, const poly multiplier,
        const poly p)
{
    const ring r = currRing;
    T->insert(cache_term::value_type(p_Head(multiplier, r), p_Copy(p, r)));
}

static poly get_from_cache_term(const cache_term::const_iterator itr,
        const poly multiplier)
{
    if (LIKELY(itr->second == NULL))
    {
        return NULL;
    }
    const ring r = currRing;
    poly p = p_Copy(itr->second, r);
    if (LIKELY(!n_Equal(pGetCoeff(multiplier), pGetCoeff(itr->first), r->cf)))
    {
        number n = n_Div(pGetCoeff(multiplier), pGetCoeff(itr->first), r->cf);
        p = p_Mult_nn(p, n, r);
        n_Delete(&n, r->cf);
    }
    return p;
}

static poly traverse_tail(const poly multiplier, const int comp,
        const ideal previous_module, const std::vector<bool> &variables,
        const lt_struct *const *const hash_previous_module)
{
    cache_term *T = &(Cache[comp]);
    cache_term::const_iterator itr = T->find(multiplier);
    if (LIKELY(itr != T->end()))
    {
        return get_from_cache_term(itr, multiplier);
    }
    poly p = compute_image(multiplier, comp, previous_module, variables,
            hash_previous_module, true);
    insert_into_cache_term(T, multiplier, p);
    return p;
}

/*
 * lift the extended induced leading term a to a syzygy
 */
static poly lift_ext_LT(const poly a, const ideal previous_module,
        const std::vector<bool> &variables,
        const lt_struct *const *const hash_previous_module,
        const bool use_cache)
{
    const ring R = currRing;
    // the leading term does not need to be cached
    poly t1 = compute_image(a, __p_GetComp(a, R)-1, previous_module, variables,
            hash_previous_module, use_cache);
    poly t2;
    if (use_cache)
    {
        t2 = traverse_tail(a->next, __p_GetComp(a->next, R)-1,
                previous_module, variables, hash_previous_module);
    } else {
        t2 = compute_image(a->next, __p_GetComp(a->next, R)-1,
                previous_module, variables, hash_previous_module, false);
    }
    t1 = p_Add_q(t1, t2, R);
    return t1;
}

/*****************************************************************************/

typedef poly syzHeadFunction(ideal, int, int);

/*
 * compute the induced leading term corresponding to the index pair (i, j)
 */
static poly syzHeadFrame(const ideal G, const int i, const int j)
{
    const ring r = currRing;
    const poly f_i = G->m[i];
    const poly f_j = G->m[j];
    poly head = p_Init(r);
    pSetCoeff0(head, n_Init(1, r->cf));
    long exp_i, exp_j, lcm;
    for (int k = (int)r->N; k > 0; k--)
    {
        exp_i = p_GetExp(f_i, k, r);
        exp_j = p_GetExp(f_j, k, r);
        lcm = si_max(exp_i, exp_j);
        p_SetExp(head, k, lcm-exp_i, r);
    }
    p_SetComp(head, i+1, r);
    p_Setm(head, r);
    return head;
}

/*
 * compute the _extended_ induced leading term corresponding to the index pair
 * (i, j), that is, the first two terms w.r.t. the induced order
 */
static poly syzHeadExtFrame(const ideal G, const int i, const int j)
{
    const ring r = currRing;
    const poly f_i = G->m[i];
    const poly f_j = G->m[j];
    poly head = p_Init(r);
    pSetCoeff0(head, n_Init(1, r->cf));
    poly head_ext = p_Init(r);
    pSetCoeff0(head_ext, n_InpNeg(n_Div(pGetCoeff(f_i), pGetCoeff(f_j), r->cf),
                r->cf));
    long exp_i, exp_j, lcm;
    for (int k = (int)r->N; k > 0; k--)
    {
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

/*
 * compute the monomial ideal M_i, see reference;
 * in the first step, we cannot assume that all leading terms which lie in the
 * component are adjacent to each other
 */
static ideal syzM_i_unsorted(const ideal G, const int i,
        syzHeadFunction *syzHead)
{
    const ring r = currRing;
    ideal M_i = NULL;
    unsigned long comp = __p_GetComp(G->m[i], r);
    int ncols = 0;
    for (int j = i-1; j >= 0; j--)
    {
        if (__p_GetComp(G->m[j], r) == comp) ncols++;
    }
    if (ncols > 0)
    {
        M_i = idInit(ncols, G->ncols);
        int k = ncols-1;
        for (int j = i-1; j >= 0; j--)
        {
            if (__p_GetComp(G->m[j], r) == comp)
            {
                M_i->m[k] = syzHead(G, i, j);
                k--;
            }
        }
        id_DelDiv(M_i, currRing);
        idSkipZeroes(M_i);
    }
    return M_i;
}

/*
 * compute the monomial ideal M_i, see reference;
 * from step two on, we can assume that all leading terms which lie in the same
 * component are adjacent to each other
 */
static ideal syzM_i_sorted(const ideal G, const int i,
        syzHeadFunction *syzHead)
{
    const ring r = currRing;
    ideal M_i = NULL;
    unsigned long comp = __p_GetComp(G->m[i], r);
    int index = i-1;
    while (__p_GetComp(G->m[index], r) == comp) index--;
    index++;
    int ncols = i-index;
    if (ncols > 0)
    {
        M_i = idInit(ncols, G->ncols);
        for (int j = ncols-1; j >= 0; j--)
        {
            M_i->m[j] = syzHead(G, i, j+index);
        }
        id_DelDiv(M_i, currRing);
        idSkipZeroes(M_i);
    }
    return M_i;
}

/*
 * concatenate the ideals in M[]
 */
static ideal idConcat(const ideal *M, const int size, const int rank)
{
    int ncols = 0;
    for (int i = size-1; i >= 0; i--)
    {
        if (M[i] != NULL)
        {
            ncols += M[i]->ncols;
        }
    }
    if (ncols == 0) return idInit(1, rank);
    ideal result = idInit(ncols, rank);
    int k = ncols-1;
    for (int i = size-1; i >= 0; i--)
    {
        if (M[i] != NULL)
        {
            for (int j = M[i]->ncols-1; j >= 0; j--)
            {
                result->m[k] = M[i]->m[j];
                k--;
            }
        }
    }
    return result;
}

static int compare_comp(const poly p_a, const poly p_b)
{
    const ring r = currRing;
    long comp_a = __p_GetComp(p_a, r);
    long comp_b = __p_GetComp(p_b, r);
    return (comp_a > comp_b) - (comp_a < comp_b);
}

static int compare_deg(const poly p_a, const poly p_b)
{
    const ring r = currRing;
    long deg_a = p_Deg(p_a, r);
    long deg_b = p_Deg(p_b, r);
    return (deg_a > deg_b) - (deg_a < deg_b);
}

static int compare_lex(const poly p_a, const poly p_b)
{
    int cmp;
    const ring r = currRing;
    int exp_a[r->N+1];
    int exp_b[r->N+1];
    p_GetExpV(p_a, exp_a, r);
    p_GetExpV(p_b, exp_b, r);
    for (int i = r->N; i > 0; i--)
    {
        cmp = (exp_a[i] > exp_b[i]) - (exp_a[i] < exp_b[i]);
        if (cmp != 0)
        {
            return cmp;
        }
    }
    return 0;
}

static int compare_Mi(const void* a, const void *b)
{
    poly p_a = *((poly *)a);
    poly p_b = *((poly *)b);
    int cmp;
    if ((cmp = compare_comp(p_a, p_b))
            || (cmp = compare_deg(p_a, p_b))
            || (cmp = compare_lex(p_a, p_b)))
            {
        return cmp;
    }
    return 0;
}

/*
 * compute the frame, that is, the induced leading terms for the next step in
 * the resolution
 */
static ideal computeFrame(const ideal G, syzM_i_Function syzM_i,
        syzHeadFunction *syzHead)
{
    ideal *M = (ideal *)omalloc((G->ncols-1)*sizeof(ideal));
    for (int i = G->ncols-2; i >= 0; i--)
    {
        M[i] = syzM_i(G, i+1, syzHead);
    }
    ideal frame = idConcat(M, G->ncols-1, G->ncols);
    for (int i = G->ncols-2; i >= 0; i--)
    {
        if (M[i] != NULL)
        {
            omFreeSize(M[i]->m, M[i]->ncols*sizeof(poly));
            omFreeBin(M[i], sip_sideal_bin);
        }
    }
    omfree(M);
    qsort(frame->m, frame->ncols, sizeof(poly), compare_Mi);
    return frame;
}

/*
 * lift each (extended) induced leading term to a syzygy
 */
static void computeLiftings(const resolvente res, const int index,
        const std::vector<bool> &variables, const bool use_cache)
{
    if (use_cache)
    {
        initialize_cache(res[index-1]->ncols);
    }
    lt_struct **hash_previous_module
        = (lt_struct **)omAlloc((res[index-1]->rank+1)*sizeof(lt_struct *));
    initialize_hash(hash_previous_module, res[index-1]);
    for (int j = res[index]->ncols-1; j >= 0; j--)
    {
        res[index]->m[j]->next->next = lift_ext_LT(res[index]->m[j],
                res[index-1], variables, hash_previous_module, use_cache);
    }
    for (int i = 0; i <= res[index-1]->rank; i++)
    {
        omfree(hash_previous_module[i]);
    }
    omFree(hash_previous_module);
    if (use_cache)
    {
        delete_cache(res[index-1]->ncols);
    }
}

/*
 * check if the monomial m contains any of the variables set to false
 */
static inline bool contains_unused_variable(const poly m,
    const std::vector<bool> &variables)
{
    const ring R = currRing;
    for (int j = R->N; j > 0; j--)
    {
        if (!variables[j-1] && p_GetExp(m, j, R) > 0)
        {
            return true;
        }
    }
    return false;
}

/*
 * delete any term in res[index] which contains any of the variables set to
 * false
 */
static void delete_variables(resolvente res, const int index,
    const std::vector<bool> &variables)
{
    for (int i = 0; i < res[index]->ncols; i++)
    {
        poly p_iter = res[index]->m[i]->next;
        if (p_iter != NULL)
        {
            while (p_iter->next != NULL)
            {
                if (contains_unused_variable(p_iter->next, variables))
                {
                    pLmDelete(&p_iter->next);
                } else {
                    pIter(p_iter);
                }
            }
        }
    }
}

static void delete_tails(resolvente res, const int index)
{
    const ring r = currRing;
    for (int i = 0; i < res[index]->ncols; i++)
    {
        if (res[index]->m[i] != NULL)
        {
            p_Delete(&(res[index]->m[i]->next), r);
        }
    }
}

/*
 * for each step in the resolution, compute the corresponding module until
 * either index == max_index is reached or res[index] is the zero module
 */
static int computeResolution_iteration(resolvente res, const int max_index,
        syzHeadFunction *syzHead, const bool do_lifting,
        const bool single_module, const bool use_cache,
        const bool use_tensor_trick, std::vector<bool> &variables)
{
    int index = 1;
    while (!idIs0(res[index]))
    {
        if (do_lifting)
        {
            computeLiftings(res, index, variables, use_cache);
            if (single_module)
            {
                delete_tails(res, index-1);
            }
            // we don't know if the input is a reduced SB:
            if (index == 1)
            {
                variables[currRing->N] = false;
            }
            update_variables(variables, res[index]);
            if (use_tensor_trick)
            {
                delete_variables(res, index, variables);
            }
        }
        if (index >= max_index) { break; }
        index++;
        res[index] = computeFrame(res[index-1], syzM_i_sorted, syzHead);
    }
    return index;
}

/*
 * compute the frame of the first syzygy module and set variables, then call
 * computeResolution_iteration() for the remaining steps
 */
static int computeResolution(resolvente res, const int max_index,
        syzHeadFunction *syzHead, const bool do_lifting,
        const bool single_module, const bool use_cache,
        const bool use_tensor_trick)
{
    if (idIs0(res[0]))
    {
        return 1;
    }
    std::vector<bool> variables;
    variables.resize(currRing->N+1, true);
    if (do_lifting)
    {
        update_variables(variables, res[0]);
        if (use_tensor_trick)
        {
            delete_variables(res, 0, variables);
        }
    }
    int index = 0;
    if (max_index > 0)
    {
        res[1] = computeFrame(res[0], syzM_i_unsorted, syzHead);
        index = computeResolution_iteration(res, max_index, syzHead,
                do_lifting, single_module, use_cache, use_tensor_trick,
                variables);
    }
    variables.clear();
    return index+1;
}

static void set_options(syzHeadFunction **syzHead_ptr, bool *do_lifting_ptr,
        bool *single_module_ptr, const char *method)
{
    if (strcmp(method, "complete") == 0)
    {   // default
        *syzHead_ptr = syzHeadExtFrame;
        *do_lifting_ptr = true;
        *single_module_ptr = false;
    }
    else if (strcmp(method, "frame") == 0)
    {
        *syzHead_ptr = syzHeadFrame;
        *do_lifting_ptr = false;
        *single_module_ptr = false;
    }
    else if (strcmp(method, "extended frame") == 0)
    {
        *syzHead_ptr = syzHeadExtFrame;
        *do_lifting_ptr = false;
        *single_module_ptr = false;
    }
    else if (strcmp(method, "single module") == 0)
    {
        *syzHead_ptr = syzHeadExtFrame;
        *do_lifting_ptr = true;
        *single_module_ptr = true;
    }
    else {   // "linear strand" (not yet implemented)
        *syzHead_ptr = syzHeadExtFrame;
        *do_lifting_ptr = true;
        *single_module_ptr = false;
    }
}

/*
 * insert the first term of r at the right place
 */
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

/*
 * For each poly in the resolution, insert the first two terms at their right
 * places. If single_module is true, then only consider the last module.
 */
static void insert_ext_induced_LTs(const resolvente res, const int length,
        const bool single_module)
{
    const ring R = currRing;
    poly p, q;
    int index = (single_module ? length-1 : 1);
    while (index < length && !idIs0(res[index]))
    {
        for (int j = res[index]->ncols-1; j >= 0; j--)
        {
            insert_first_term(res[index]->m[j]->next, p, q, R);
            insert_first_term(res[index]->m[j], p, q, R);
        }
        index++;
    }
}

/*
 * Compute the Schreyer resolution of arg, see reference at the beginning of
 * this file.
 *
 * If use_cache == true (default), the result of compute_image() is cached for
 * _every_ term in the current step of the resolution. This corresponds to the
 * subtree attached to the node which represents this term, see reference.
 *
 * If use_tensor_trick == true, the current module is modfied after each
 * lifting step in the resolution: any term which contains a variable which
 * does not appear among the (induced) leading terms is deleted. Note that the
 * resulting object is not necessarily a complex anymore. However, constant
 * entries remain exactly the same. This option does not apply for
 * method == "frame" and method "extended frame".
 *
 * These two options are used in PrymGreen.jl; do not delete!
 */
syStrategy syFrank(const ideal arg, const int length, const char *method,
        const bool use_cache, const bool use_tensor_trick)
{
    syStrategy result = (syStrategy)omAlloc0(sizeof(ssyStrategy));
    resolvente res = (resolvente)omAlloc0((length+1)*sizeof(ideal));
    if (strcmp(method, "frame") != 0)
    {
        res[0] = id_Copy(arg, currRing);
    }
    else
    {
        res[0] = id_Head(arg, currRing);
    }
    syzHeadFunction *syzHead;
    bool do_lifting;
    bool single_module;
    set_options(&syzHead, &do_lifting, &single_module, method);
    int new_length = computeResolution(res, length-1, syzHead, do_lifting,
            single_module, use_cache, use_tensor_trick);
    if (new_length < length)
    {
        res = (resolvente)omReallocSize(res, (length+1)*sizeof(ideal),
                (new_length+1)*sizeof(ideal));
    }
    if (strcmp(method, "frame") != 0)
    {
        insert_ext_induced_LTs(res, new_length, single_module);
    }
    result->fullres = res;
    result->length = new_length;
    result->list_length = new_length;
    return result;
}

