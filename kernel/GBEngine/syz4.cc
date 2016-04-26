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
    const syzHeadFunction *syzHead)
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
        id_DelDiv(M_i, currRing);
        idSkipZeroes(M_i);
    }
    return M_i;
}

static ideal syzM_i_sorted(const ideal G, const int i,
    const syzHeadFunction *syzHead)
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
        id_DelDiv(M_i, currRing);
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
    int deg_a = p_Deg(p_a, r);
    int deg_b = p_Deg(p_b, r);
    cmp = (deg_a > deg_b) - (deg_a < deg_b);
    if (cmp != 0) {
         return cmp;
    }
    int comp_a = p_GetComp(p_a, r);
    int comp_b = p_GetComp(p_b, r);
    cmp = (comp_a > comp_b) - (comp_a < comp_b);
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

static ideal computeFrame(const ideal G, const syzM_i_Function syzM_i,
    const syzHeadFunction *syzHead)
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

static int computeResolution(resolvente &res, const int length,
    const syzHeadFunction *syzHead)
{
    int index = 0;
    if (!idIs0(res[index]) && index < length) {
        res[index+1] = computeFrame(res[index], syzM_i_unsorted, syzHead);
        index++;
    }
    while (!idIs0(res[index]) && index < length) {
        res[index+1] = computeFrame(res[index], syzM_i_sorted, syzHead);
        index++;
    }
    if (index < length) {
        res = (resolvente)omReallocSize(res, (length+1)*sizeof(ideal),
            (index+1)*sizeof(ideal));
    }
    return index;
}

static void sortPolys(const resolvente res, const int length)
{
    const ring r = currRing;
    for (int i = length; i > 0; i--) {
        for (int j = res[i]->ncols-1; j >= 0; j--) {
            res[i]->m[j] = p_SortAdd(res[i]->m[j], r);
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
    sortPolys(res, length);
    result->fullres = res;
    result->length = length+1;
    return result;
}

