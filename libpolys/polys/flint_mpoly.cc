// emacs edit mode for this file is -*- C++ -*-
/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/*
* ABSTRACT: flint mpoly
*/

#include "misc/auxiliary.h"
#include "flintconv.h"
#include "flint_mpoly.h"

#ifdef HAVE_FLINT
#if __FLINT_RELEASE >= 20503
#include "coeffs/coeffs.h"
#include "coeffs/longrat.h"
#include "polys/monomials/p_polys.h"

#include <vector>

/****** ring conversion ******/

BOOLEAN convSingRFlintR(fmpq_mpoly_ctx_t ctx, const ring r)
{
  if (rRing_ord_pure_dp(r))
  {
    fmpq_mpoly_ctx_init(ctx,r->N,ORD_DEGREVLEX);
    return FALSE;
  }
  else if (rRing_ord_pure_Dp(r))
  {
    fmpq_mpoly_ctx_init(ctx,r->N,ORD_DEGLEX);
    return FALSE;
  }
  else if (rRing_ord_pure_lp(r))
  {
    fmpq_mpoly_ctx_init(ctx,r->N,ORD_LEX);
    return FALSE;
  }
  return TRUE;
}

BOOLEAN convSingRFlintR(nmod_mpoly_ctx_t ctx, const ring r)
{
  if (rRing_ord_pure_dp(r))
  {
    nmod_mpoly_ctx_init(ctx,r->N,ORD_DEGREVLEX,r->cf->ch);
    return FALSE;
  }
  else if (rRing_ord_pure_Dp(r))
  {
    nmod_mpoly_ctx_init(ctx,r->N,ORD_DEGLEX,r->cf->ch);
    return FALSE;
  }
  else if (rRing_ord_pure_lp(r))
  {
    nmod_mpoly_ctx_init(ctx,r->N,ORD_LEX,r->cf->ch);
    return FALSE;
  }
  return TRUE;
}

/******** polynomial conversion ***********/

#if HAVE_OMALLOC
// memory allocation is not thread safe; singular polynomials must be constructed in serial

/*
    We agree the that result of a singular -> fmpq_mpoly conversion is
    readonly. This restricts the usage of the result in flint functions to
    const arguments. However, the real readonly conversion is currently only
    implemented in the threaded conversion below since it requires a scan of
    all coefficients anyways. The _fmpq_mpoly_clear_readonly_sing needs to
    be provided for a consistent interface in the polynomial operations.
*/
static void _fmpq_mpoly_clear_readonly_sing(fmpq_mpoly_t a, fmpq_mpoly_ctx_t ctx)
{
    fmpq_mpoly_clear(a, ctx);
}

void convSingPFlintMP(fmpq_mpoly_t res, fmpq_mpoly_ctx_t ctx, poly p, int lp, const ring r)
{
  fmpq_mpoly_init2(res, lp, ctx);
  ulong* exp=(ulong*)omAlloc((r->N+1)*sizeof(ulong));
  while(p!=NULL)
  {
    number n=pGetCoeff(p);
    fmpq_t c;
    convSingNFlintN_QQ(c,n);
    #if SIZEOF_LONG==8
    p_GetExpVL(p,(int64*)exp,r);
    fmpq_mpoly_push_term_fmpq_ui(res, c, exp, ctx);
    #else
    p_GetExpV(p,(int*)exp,r);
    fmpq_mpoly_push_term_fmpq_ui(res, c, &(exp[1]), ctx);
    #endif
    fmpq_clear(c);
    pIter(p);
  }
  fmpq_mpoly_reduce(res, ctx); // extra step for QQ ensures res has content canonically factored out
  omFreeSize(exp,(r->N+1)*sizeof(ulong));
}

poly convFlintMPSingP(fmpq_mpoly_t f, fmpq_mpoly_ctx_t ctx, const ring r)
{
  int d=fmpq_mpoly_length(f,ctx)-1;
  poly p=NULL;
  ulong* exp=(ulong*)omAlloc((r->N+1)*sizeof(ulong));
  fmpq_t c;
  fmpq_init(c);
  for(int i=d; i>=0; i--)
  {
    fmpq_mpoly_get_term_coeff_fmpq(c,f,i,ctx);
    poly pp=p_Init(r);
    #if SIZEOF_LONG==8
    fmpq_mpoly_get_term_exp_ui(exp,f,i,ctx);
    p_SetExpVL(pp,(int64*)exp,r);
    #else
    fmpq_mpoly_get_term_exp_ui(&(exp[1]),f,i,ctx);
    p_SetExpV(pp,(int*)exp,r);
    #endif
    p_Setm(pp,r);
    number n=convFlintNSingN_QQ(c,r->cf);
    pSetCoeff0(pp,n);
    pNext(pp)=p;
    p=pp;
  }
  fmpq_clear(c);
  omFreeSize(exp,(r->N+1)*sizeof(ulong));
  p_Test(p,r);
  return p;
}

poly convFlintMPSingP(nmod_mpoly_t f, nmod_mpoly_ctx_t ctx, const ring r)
{
  int d=nmod_mpoly_length(f,ctx)-1;
  poly p=NULL;
  ulong* exp=(ulong*)omAlloc((r->N+1)*sizeof(ulong));
  for(int i=d; i>=0; i--)
  {
    ulong c=nmod_mpoly_get_term_coeff_ui(f,i,ctx);
    poly pp=p_Init(r);
    #if SIZEOF_LONG==8
    nmod_mpoly_get_term_exp_ui(exp,f,i,ctx);
    p_SetExpVL(pp,(int64*)exp,r);
    #else
    nmod_mpoly_get_term_exp_ui(&(exp[1]),f,i,ctx);
    p_SetExpV(pp,(int*)exp,r);
    #endif
    p_Setm(pp,r);
    pSetCoeff0(pp,(number)c);
    pNext(pp)=p;
    p=pp;
  }
  omFreeSize(exp,(r->N+1)*sizeof(ulong));
  p_Test(p,r);
  return p;
}

void convSingPFlintMP(nmod_mpoly_t res, nmod_mpoly_ctx_t ctx, poly p, int lp,const ring r)
{
  nmod_mpoly_init2(res, lp, ctx);
  ulong* exp=(ulong*)omAlloc((r->N+1)*sizeof(ulong));
  while(p!=NULL)
  {
    number n=pGetCoeff(p);
    #if SIZEOF_LONG==8
    p_GetExpVL(p,(int64*)exp,r);
    nmod_mpoly_push_term_ui_ui(res, (ulong)n, exp, ctx);
    #else
    p_GetExpV(p,(int*)exp,r);
    nmod_mpoly_push_term_ui_ui(res, (ulong)n, &(exp[1]), ctx);
    #endif
    pIter(p);
  }
  omFreeSize(exp,(r->N+1)*sizeof(ulong));
}

#else
// memory allocator is thread safe; singular polynomials may be constructed in parallel

// like convSingNFlintN_QQ but it takes an initialized fmpq_t f
static void my_convSingNFlintN_QQ(fmpq_t f, number n)
{
    if (SR_HDL(n)&SR_INT)
    {
        fmpq_set_si(f,SR_TO_INT(n),1);
    }
    else if (n->s<3)
    {
        fmpz_set_mpz(fmpq_numref(f), n->z);
        fmpz_set_mpz(fmpq_denref(f), n->n);
    }
    else
    {
        fmpz_set_mpz(fmpq_numref(f), n->z);
        fmpz_one(fmpq_denref(f));
    }
}


/*
    In order that flint may sometimes borrow the large integer coeffs of
    polynomials over QQ (borrow means: simply point to the same GMP structs
    that singular has already allocated), we define the result of a
    singular -> fmpq_mpoly conversion to be readonly. This means we agree
    that
        - it can only be used as an const argument to a flint function
        - singular must not mutate the original coeffs while the readonly object is in use
*/

static void _fmpq_mpoly_clear_readonly_sing(fmpq_mpoly_t a, fmpq_mpoly_ctx_t ctx)
{
    if (fmpq_is_one(a->content))
    {
        if (a->zpoly->alloc > 0)
        {
            flint_free(a->zpoly->coeffs);
            flint_free(a->zpoly->exps);
        }

        fmpq_clear(a->content);
    }
    else
    {
        fmpq_mpoly_clear(a, ctx);
    }
}

/* singular -> fmpq_mpoly conversion */

class convert_sing_to_fmpq_mpoly_base
{
public:
    slong num_threads;
    slong length;
    fmpq_mpoly_struct * res;
    const fmpq_mpoly_ctx_struct * ctx;
    std::vector<poly> markers;
    ring r;
    fmpq_t content;

    convert_sing_to_fmpq_mpoly_base(fmpq_mpoly_struct * res_,
                     const fmpq_mpoly_ctx_struct * ctx_, const ring r_, poly p)
      : num_threads(0),
        res(res_),
        ctx(ctx_),
        r(r_)
    {
        fmpq_t c;
        fmpq_init(c);
        fmpq_init(content);
        fmpq_zero(content);

        length = 0;
        while (1)
        {
            if ((length % 4096) == 0)
            {
                my_convSingNFlintN_QQ(c, number(pGetCoeff(p)));
                fmpq_gcd(content, content, c);
                markers.push_back(p);
            }
            if (p == NULL)
                return;
            length++;
            pIter(p);
        }

        fmpq_clear(c);
    }

    ~convert_sing_to_fmpq_mpoly_base()
    {
        fmpq_clear(content);
    }
};

class convert_sing_to_fmpq_mpoly_arg
{
public:
    fmpq_t content;
    slong start_idx, end_idx;
    convert_sing_to_fmpq_mpoly_base* base;
    flint_bitcnt_t required_bits;

    convert_sing_to_fmpq_mpoly_arg() {fmpq_init(content);}
    ~convert_sing_to_fmpq_mpoly_arg() {fmpq_clear(content);}
};

static void convert_sing_to_fmpq_mpoly_content_bits(void * varg)
{
    convert_sing_to_fmpq_mpoly_arg* arg = (convert_sing_to_fmpq_mpoly_arg*) varg;
    convert_sing_to_fmpq_mpoly_base * base = arg->base;
    ulong * exp = (ulong*) flint_malloc((base->r->N + 1)*sizeof(ulong));
    fmpq_t c;
    fmpq_init(c);

    slong idx = arg->start_idx/4096;
    poly p = arg->base->markers[idx];
    idx *= 4096;
    while (idx < arg->start_idx)
    {
        pIter(p);
        idx++;
    }

    flint_bitcnt_t required_bits = MPOLY_MIN_BITS;
    fmpq_set(arg->content, base->content);

    while (idx < arg->end_idx)
    {
        number n = number(pGetCoeff(p));

        if (fmpq_is_one(arg->content) && (SR_HDL(n)&SR_INT || n->s >= 3))
        {
            /* content is 1 and n is an integer, nothing to do */
        }
        else
        {
            my_convSingNFlintN_QQ(c, n);
            fmpq_gcd(arg->content, arg->content, c);
        }

        #if SIZEOF_LONG==8
        p_GetExpVL(p, (int64*)exp, base->r);
        flint_bitcnt_t exp_bits = mpoly_exp_bits_required_ui(exp, base->ctx->zctx->minfo);
        #else
        p_GetExpV(p, (int*)exp, base->r);
        flint_bitcnt_t exp_bits = mpoly_exp_bits_required_ui(&(exp[1]), base->ctx->zctx->minfo);
        #endif

        required_bits = FLINT_MAX(required_bits, exp_bits);

        pIter(p);
        idx++;
    }

    arg->required_bits = required_bits;

    fmpq_clear(c);
    flint_free(exp);
}

static void convert_sing_to_fmpq_mpoly_zpoly_worker(void * varg)
{
    convert_sing_to_fmpq_mpoly_arg * arg = (convert_sing_to_fmpq_mpoly_arg *) varg;
    convert_sing_to_fmpq_mpoly_base * base = arg->base;
    ulong * exp = (ulong*) flint_malloc((base->r->N + 1)*sizeof(ulong));
    fmpq_t c, t;
    fmpq_init(c);
    fmpq_init(t);

    slong idx = arg->start_idx/4096;
    poly p = base->markers[idx];
    idx *= 4096;
    while (idx < arg->start_idx)
    {
        pIter(p);
        idx++;
    }

    slong N = mpoly_words_per_exp(base->res->zpoly->bits, base->ctx->zctx->minfo);
    fmpz * res_coeffs = base->res->zpoly->coeffs;
    ulong * res_exps = base->res->zpoly->exps;
    flint_bitcnt_t res_bits = base->res->zpoly->bits;

    while (idx < arg->end_idx)
    {
        if (fmpq_is_one(base->res->content))
        {
            // borrowing singular integers
            // the entry res_coeffs[idx] is junk, we should just overwrite it

            number n = number(pGetCoeff(p));

            if (SR_HDL(n)&SR_INT)
            {
                // n is a singular-small integer
                res_coeffs[idx] = SR_TO_INT(n);
            }
            else if (n->s<3)
            {
                // n is an element of QQ \ ZZ, should not happen
                assume(false);
            }
            else
            {
                // n is a singular-large integer, n may be flint-small
                res_coeffs[idx] = PTR_TO_COEFF(n->z);
                if (fmpz_fits_si(res_coeffs + idx))
                {
                    slong val = fmpz_get_si(res_coeffs + idx);
                    if (val >= COEFF_MIN && val <= COEFF_MAX)
                        res_coeffs[idx] = val;
                }
            }
        }
        else
        {
            my_convSingNFlintN_QQ(c, number(pGetCoeff(p)));
            FLINT_ASSERT(!fmpq_is_zero(base->res->content));
            fmpq_div(t, c, base->res->content);
            FLINT_ASSERT(fmpz_is_one(fmpq_denref(t)));
            fmpz_swap(res_coeffs + idx, fmpq_numref(t));
        }

        #if SIZEOF_LONG==8
        p_GetExpVL(p, (int64*)exp, base->r);
        mpoly_set_monomial_ui(res_exps + N*idx, exp, res_bits, base->ctx->zctx->minfo);
        #else
        p_GetExpV(p, (int*)exp, base->r);
        mpoly_set_monomial_ui(res_exps + N*idx, &(exp[1]), res_bits, base->ctx->minfo);
        #endif

        pIter(p);
        idx++;
    }

    flint_free(exp);
    fmpq_clear(c);
    fmpq_clear(t);
}

/* res comes in unitialized!!!! */
void convSingPFlintMP(fmpq_mpoly_t res, fmpq_mpoly_ctx_t ctx, poly p, int lp, const ring r)
{
    thread_pool_handle * handles;
    slong num_handles;
    slong thread_limit = 1000; // TODO: should be paramter to this function

    /* the constructor works out the length of p and sets some markers */
    convert_sing_to_fmpq_mpoly_base base(res, ctx, r, p);

    /* sensibly limit thread count and get workers */
    thread_limit = FLINT_MIN(thread_limit, base.length/1024);
    handles = NULL;
    num_handles = 0;
    if (thread_limit > 1 && global_thread_pool_initialized)
    {
        slong max_num_handles = thread_pool_get_size(global_thread_pool);
        max_num_handles = FLINT_MIN(thread_limit - 1, max_num_handles);
        if (max_num_handles > 0)
        {
            handles = (thread_pool_handle *) flint_malloc(max_num_handles*sizeof(thread_pool_handle));
            num_handles = thread_pool_request(global_thread_pool, handles, max_num_handles);
        }
    }

    /* fill in thread division points */
    base.num_threads = 1 + num_handles;
    convert_sing_to_fmpq_mpoly_arg * args = new convert_sing_to_fmpq_mpoly_arg[base.num_threads];
    slong cur_idx = 0;
    for (slong i = 0; i < base.num_threads; i++)
    {
        slong next_idx = i + 1 < base.num_threads ? (i + 1)*base.length/base.num_threads : base.length;
        FLINT_ASSERT(cur_idx <= base.length);
        next_idx = FLINT_MAX(next_idx, cur_idx);
        next_idx = FLINT_MIN(next_idx, base.length);
        args[i].base = &base;
        args[i].start_idx = cur_idx;
        args[i].end_idx   = next_idx;
        cur_idx = next_idx;
    }

    /* get content and bits */
    for (slong i = 0; i < num_handles; i++)
        thread_pool_wake(global_thread_pool, handles[i], convert_sing_to_fmpq_mpoly_content_bits, args + i);
    convert_sing_to_fmpq_mpoly_content_bits(args + num_handles);
    for (slong i = 0; i < num_handles; i++)
        thread_pool_wait(global_thread_pool, handles[i]);

    flint_bitcnt_t required_bits = MPOLY_MIN_BITS;
    for (slong i = 0; i <= num_handles; i++)
        required_bits = FLINT_MAX(required_bits, args[i].required_bits);

    /* sign of content should match sign of first coeff */
    fmpq_t content;
    fmpq_init(content);
    fmpq_zero(content);
    for (slong i = 0; i < base.num_threads; i++)
        fmpq_gcd(content, content, args[i].content);
    if (p != NULL)
    {
        fmpq_t c;
        fmpq_init(c);
        my_convSingNFlintN_QQ(c, number(pGetCoeff(p)));
        if (fmpq_sgn(c) < 0)
            fmpq_neg(content, content);
        fmpq_clear(c);
    }

    /* initialize res with optimal bits */
    required_bits = mpoly_fix_bits(required_bits, ctx->zctx->minfo);
    if (fmpq_is_one(content))
    {
        /* initialize borrowed coeffs */
        slong N = mpoly_words_per_exp(required_bits, ctx->zctx->minfo);
        slong alloc = base.length;
        if (alloc != 0)
        {
            res->zpoly->coeffs = (fmpz *) flint_malloc(alloc*sizeof(fmpz));
            res->zpoly->exps   = (ulong *) flint_malloc(alloc*N*sizeof(ulong));
        }
        else
        {
            res->zpoly->coeffs = NULL;
            res->zpoly->exps = NULL;
        }
        res->zpoly->alloc = alloc;
        res->zpoly->length = 0;
        res->zpoly->bits = required_bits;

        fmpq_init(res->content);
        fmpq_one(res->content);
    }
    else
    {
        /* initialize coeffs that will be created and destroyed */
        fmpq_mpoly_init3(res, base.length, required_bits, ctx);
        fmpq_swap(res->content, content);
    }

    fmpq_clear(content);

    /* fill in res->zpoly */
    for (slong i = 0; i < num_handles; i++)
        thread_pool_wake(global_thread_pool, handles[i], convert_sing_to_fmpq_mpoly_zpoly_worker, args + i);
    convert_sing_to_fmpq_mpoly_zpoly_worker(args + num_handles);
    for (slong i = 0; i < num_handles; i++)
        thread_pool_wait(global_thread_pool, handles[i]);

    base.res->zpoly->length = base.length;

    delete[] args;

    for (slong i = 0; i < num_handles; i++)
        thread_pool_give_back(global_thread_pool, handles[i]);
    if (handles)
        flint_free(handles);
}

/* fmpq_mpoly -> singular conversion */

class convert_fmpq_mpoly_to_sing_base
{
public:
    slong num_threads;
    fmpq_mpoly_struct * f;
    const fmpq_mpoly_ctx_struct * ctx;
    ring r;

    convert_fmpq_mpoly_to_sing_base(slong num_threads_, fmpq_mpoly_struct * f_,
                            const fmpq_mpoly_ctx_struct * ctx_, const ring r_)
      : num_threads(num_threads_),
        f(f_),
        ctx(ctx_),
        r(r_)
    {
    }
};

class convert_fmpq_mpoly_to_sing_arg
{
public:
    poly poly_start, poly_end;
    slong start_idx, end_idx;
    convert_fmpq_mpoly_to_sing_base* base;
};

static void convert_fmpq_mpoly_to_sing_worker(void * varg)
{
    convert_fmpq_mpoly_to_sing_arg * arg = (convert_fmpq_mpoly_to_sing_arg *) varg;
    convert_fmpq_mpoly_to_sing_base * base = arg->base;
    ulong* exp = (ulong*) flint_malloc((base->r->N + 1)*sizeof(ulong));
    fmpq_t c;
    fmpq_init(c);

    for (slong idx = arg->end_idx - 1; idx >= arg->start_idx; idx--)
    {
        poly pp = p_Init(base->r);

        #if SIZEOF_LONG==8
        fmpq_mpoly_get_term_exp_ui(exp, base->f, idx, base->ctx);
        p_SetExpVL(pp, (int64*)exp, base->r);
        #else
        fmpq_mpoly_get_term_exp_ui(&(exp[1]), base->f, idx, base->ctx);
        p_SetExpV(pp, (int*)exp, base->r);
        #endif
        p_Setm(pp, base->r);

        fmpq_mpoly_get_term_coeff_fmpq(c, base->f, idx, base->ctx);
        pSetCoeff0(pp, number(convFlintNSingN_QQ(c, base->r->cf)));

        if (idx == arg->end_idx - 1)
            arg->poly_end = pp;

        pNext(pp) = arg->poly_start;
        arg->poly_start = pp;
    }

    flint_free(exp);
    fmpq_clear(c);
}

poly convFlintMPSingP(fmpq_mpoly_t f, fmpq_mpoly_ctx_t ctx, const ring r)
{
    thread_pool_handle * handles;
    slong num_handles;
    slong thread_limit = 1000;// TODO: should be paramter to this function

    /* sensibly limit threads and get workers */
    thread_limit = FLINT_MIN(thread_limit, f->zpoly->length/1024);
    handles = NULL;
    num_handles = 0;
    if (thread_limit > 1 && global_thread_pool_initialized)
    {
        slong max_num_handles = thread_pool_get_size(global_thread_pool);
        max_num_handles = FLINT_MIN(thread_limit - 1, max_num_handles);
        if (max_num_handles > 0)
        {
            handles = (thread_pool_handle *) flint_malloc(max_num_handles*sizeof(thread_pool_handle));
            num_handles = thread_pool_request(global_thread_pool, handles, max_num_handles);
        }
    }

    convert_fmpq_mpoly_to_sing_base base(num_handles + 1, f, ctx, r);
    convert_fmpq_mpoly_to_sing_arg * args = new convert_fmpq_mpoly_to_sing_arg[base.num_threads];
    slong cur_idx = 0;
    for (slong i = 0; i < base.num_threads; i++)
    {
        slong next_idx = i + 1 < base.num_threads ? (i + 1)*base.f->zpoly->length/base.num_threads
                                                  : base.f->zpoly->length;
        FLINT_ASSERT(cur_idx <= base.f->zpoly->length);
        next_idx = FLINT_MAX(next_idx, cur_idx);
        next_idx = FLINT_MIN(next_idx, base.f->zpoly->length);
        args[i].base = &base;
        args[i].start_idx = cur_idx;
        args[i].end_idx   = next_idx;
        cur_idx = next_idx;
        args[i].poly_end = NULL;
        args[i].poly_start = NULL;
    }

    /* construct pieces */
    for (slong i = 0; i < num_handles; i++)
        thread_pool_wake(global_thread_pool, handles[i], convert_fmpq_mpoly_to_sing_worker, args + i);
    convert_fmpq_mpoly_to_sing_worker(args + num_handles);
    for (slong i = 0; i < num_handles; i++)
        thread_pool_wait(global_thread_pool, handles[i]);

    /* join pieces */
    poly p = NULL;
    for (slong i = num_handles; i >= 0; i--)
    {
        if (args[i].poly_end != NULL)
        {
            pNext(args[i].poly_end) = p;
            p = args[i].poly_start;
        }
    }

    for (slong i = 0; i < num_handles; i++)
        thread_pool_give_back(global_thread_pool, handles[i]);
    if (handles)
        flint_free(handles);

    p_Test(p,r);
    return p;
}


/* singular -> nmod_mpoly conversion */

class convert_sing_to_nmod_mpoly_base
{
public:
    slong num_threads;
    slong length;
    nmod_mpoly_struct * res;
    const nmod_mpoly_ctx_struct * ctx;
    std::vector<poly> markers;
    ring r;

    convert_sing_to_nmod_mpoly_base(nmod_mpoly_struct * res_,
                     const nmod_mpoly_ctx_struct * ctx_, const ring r_, poly p)
      : num_threads(0),
        res(res_),
        ctx(ctx_),
        r(r_)
    {
        length = 0;
        while (1)
        {
            if ((length % 4096) == 0)
                markers.push_back(p);
            if (p == NULL)
                return;
            length++;
            pIter(p);
        }
    }
};

class convert_sing_to_nmod_mpoly_arg
{
public:
    slong start_idx, end_idx;
    convert_sing_to_nmod_mpoly_base* base;
    flint_bitcnt_t required_bits;
};

static void convert_sing_to_nmod_mpoly_bits(void * varg)
{
    convert_sing_to_nmod_mpoly_arg * arg = (convert_sing_to_nmod_mpoly_arg *) varg;
    convert_sing_to_nmod_mpoly_base * base = arg->base;
    ulong * exp = (ulong*) flint_malloc((base->r->N + 1)*sizeof(ulong));

    slong idx = arg->start_idx/4096;
    poly p = base->markers[idx];
    idx *= 4096;
    while (idx < arg->start_idx)
    {
        pIter(p);
        idx++;
    }

    flint_bitcnt_t required_bits = MPOLY_MIN_BITS;

    while (idx < arg->end_idx)
    {
        #if SIZEOF_LONG==8
        p_GetExpVL(p, (int64*)exp, base->r);
        flint_bitcnt_t exp_bits = mpoly_exp_bits_required_ui(exp, base->ctx->minfo);
        #else
        p_GetExpV(p, (int*)exp, base->r);
        flint_bitcnt_t exp_bits = mpoly_exp_bits_required_ui(&(exp[1]), base->ctx->minfo);
        #endif

        required_bits = FLINT_MAX(required_bits, exp_bits);

        pIter(p);
        idx++;
    }

    arg->required_bits = required_bits;

    flint_free(exp);
}


static void convert_sing_to_nmod_mpoly_worker(void * varg)
{
    convert_sing_to_nmod_mpoly_arg * arg = (convert_sing_to_nmod_mpoly_arg *) varg;
    convert_sing_to_nmod_mpoly_base * base = arg->base;
    ulong * exp = (ulong*) flint_malloc((base->r->N + 1)*sizeof(ulong));

    slong idx = arg->start_idx/4096;
    poly p = base->markers[idx];
    idx *= 4096;
    while (idx < arg->start_idx)
    {
        pIter(p);
        idx++;
    }

    slong N = mpoly_words_per_exp(base->res->bits, base->ctx->minfo);
    ulong * res_coeffs = base->res->coeffs;
    ulong * res_exps = base->res->exps;
    flint_bitcnt_t res_bits = base->res->bits;

    while (idx < arg->end_idx)
    {
        #if SIZEOF_LONG==8
        p_GetExpVL(p, (int64*)exp, base->r);
        mpoly_set_monomial_ui(res_exps + N*idx, exp, res_bits, base->ctx->minfo);
        #else
        p_GetExpV(p, (int*)exp, base->r);
        mpoly_set_monomial_ui(res_exps + N*idx, &(exp[1]), res_bits, base->ctx->minfo);
        #endif

        res_coeffs[idx] = (ulong)(number(pGetCoeff(p)));

        pIter(p);
        idx++;
    }

    flint_free(exp);
}

/* res comes in unitialized!!!! */
void convSingPFlintMP(nmod_mpoly_t res, nmod_mpoly_ctx_t ctx, poly p, int lp, const ring r)
{
    thread_pool_handle * handles;
    slong num_handles;
    slong thread_limit = 1000; // TODO: should be paramter to this function

    /* the constructor works out the length of p and sets some markers */
    convert_sing_to_nmod_mpoly_base base(res, ctx, r, p);

    /* sensibly limit thread count and get workers */
    thread_limit = FLINT_MIN(thread_limit, base.length/1024);
    handles = NULL;
    num_handles = 0;
    if (thread_limit > 1 && global_thread_pool_initialized)
    {
        slong max_num_handles = thread_pool_get_size(global_thread_pool);
        max_num_handles = FLINT_MIN(thread_limit - 1, max_num_handles);
        if (max_num_handles > 0)
        {
            handles = (thread_pool_handle *) flint_malloc(max_num_handles*sizeof(thread_pool_handle));
            num_handles = thread_pool_request(global_thread_pool, handles, max_num_handles);
        }
    }

    /* fill in thread division points */
    base.num_threads = 1 + num_handles;
    convert_sing_to_nmod_mpoly_arg * args = new convert_sing_to_nmod_mpoly_arg[base.num_threads];
    slong cur_idx = 0;
    for (slong i = 0; i < base.num_threads; i++)
    {
        slong next_idx = i + 1 < base.num_threads ? (i + 1)*base.length/base.num_threads
                                                  : base.length;
        FLINT_ASSERT(cur_idx <= base.length);
        next_idx = FLINT_MAX(next_idx, cur_idx);
        next_idx = FLINT_MIN(next_idx, base.length);
        args[i].base = &base;
        args[i].start_idx = cur_idx;
        args[i].end_idx   = next_idx;
        cur_idx = next_idx;
    }

    /* find required bits */
    for (slong i = 0; i < num_handles; i++)
        thread_pool_wake(global_thread_pool, handles[i], convert_sing_to_nmod_mpoly_bits, args + i);
    convert_sing_to_nmod_mpoly_bits(args + num_handles);
    for (slong i = 0; i < num_handles; i++)
        thread_pool_wait(global_thread_pool, handles[i]);

    flint_bitcnt_t required_bits = MPOLY_MIN_BITS;
    for (slong i = 0; i <= num_handles; i++)
        required_bits = FLINT_MAX(required_bits, args[i].required_bits);

    /* initialize res with optimal bits */
    nmod_mpoly_init3(res, base.length, mpoly_fix_bits(required_bits, ctx->minfo), ctx);

    /* fill in res */
    for (slong i = 0; i < num_handles; i++)
        thread_pool_wake(global_thread_pool, handles[i], convert_sing_to_nmod_mpoly_worker, args + i);
    convert_sing_to_nmod_mpoly_worker(args + num_handles);
    for (slong i = 0; i < num_handles; i++)
        thread_pool_wait(global_thread_pool, handles[i]);

    base.res->length = base.length;

    delete[] args;

    for (slong i = 0; i < num_handles; i++)
        thread_pool_give_back(global_thread_pool, handles[i]);
    if (handles)
        flint_free(handles);
}


/* nmod_mpoly -> singular conversion */

class convert_nmod_mpoly_to_sing_base
{
public:
    slong num_threads;
    nmod_mpoly_struct * f;
    const nmod_mpoly_ctx_struct * ctx;
    ring r;

    convert_nmod_mpoly_to_sing_base(slong num_threads_, nmod_mpoly_struct * f_,
                             const nmod_mpoly_ctx_struct * ctx_, const ring r_)
      : num_threads(num_threads_),
        f(f_),
        ctx(ctx_),
        r(r_)
    {
    }
};

class convert_nmod_mpoly_to_sing_arg
{
public:
    poly poly_start, poly_end;
    slong start_idx, end_idx;
    convert_nmod_mpoly_to_sing_base* base;
};

static void convert_nmod_mpoly_to_sing_worker(void * varg)
{
    convert_nmod_mpoly_to_sing_arg * arg = (convert_nmod_mpoly_to_sing_arg *) varg;
    convert_nmod_mpoly_to_sing_base * base = arg->base;
    ulong* exp = (ulong*) flint_malloc((base->r->N + 1)*sizeof(ulong));

    for (slong idx = arg->end_idx - 1; idx >= arg->start_idx; idx--)
    {
        poly pp = p_Init(base->r);

        #if SIZEOF_LONG==8
        nmod_mpoly_get_term_exp_ui(exp, base->f, idx, base->ctx);
        p_SetExpVL(pp, (int64*)exp, base->r);
        #else
        nmod_mpoly_get_term_exp_ui(&(exp[1]), base->f, idx, base->ctx);
        p_SetExpV(pp, (int*)exp, base->r);
        #endif
        p_Setm(pp, base->r);

        pSetCoeff0(pp, number(nmod_mpoly_get_term_coeff_ui(base->f, idx, base->ctx)));

        if (idx == arg->end_idx - 1)
            arg->poly_end = pp;

        pNext(pp) = arg->poly_start;
        arg->poly_start = pp;
    }

    flint_free(exp);
}

poly convFlintMPSingP(nmod_mpoly_t f, nmod_mpoly_ctx_t ctx, const ring r)
{
    thread_pool_handle * handles;
    slong num_handles;
    slong thread_limit = 1000; // TODO: should be paramter to this function

    /* sensibly limit threads and get workers */
    thread_limit = FLINT_MIN(thread_limit, f->length/1024);
    handles = NULL;
    num_handles = 0;
    if (thread_limit > 1 && global_thread_pool_initialized)
    {
        slong max_num_handles = thread_pool_get_size(global_thread_pool);
        max_num_handles = FLINT_MIN(thread_limit - 1, max_num_handles);
        if (max_num_handles > 0)
        {
            handles = (thread_pool_handle *) flint_malloc(max_num_handles*sizeof(thread_pool_handle));
            num_handles = thread_pool_request(global_thread_pool, handles, max_num_handles);
        }
    }

    convert_nmod_mpoly_to_sing_base base(num_handles + 1, f, ctx, r);
    convert_nmod_mpoly_to_sing_arg * args = new convert_nmod_mpoly_to_sing_arg[base.num_threads];
    slong cur_idx = 0;
    for (slong i = 0; i < base.num_threads; i++)
    {
        slong next_idx = i + 1 < base.num_threads ? (i + 1)*base.f->length/base.num_threads
                                                  : base.f->length;
        FLINT_ASSERT(cur_idx <= base.f->length);
        next_idx = FLINT_MAX(next_idx, cur_idx);
        next_idx = FLINT_MIN(next_idx, base.f->length);
        args[i].base = &base;
        args[i].start_idx = cur_idx;
        args[i].end_idx   = next_idx;
        cur_idx = next_idx;
        args[i].poly_end = NULL;
        args[i].poly_start = NULL;
    }

    /* construct pieces */
    for (slong i = 0; i < num_handles; i++)
        thread_pool_wake(global_thread_pool, handles[i], convert_nmod_mpoly_to_sing_worker, args + i);
    convert_nmod_mpoly_to_sing_worker(args + num_handles);
    for (slong i = 0; i < num_handles; i++)
        thread_pool_wait(global_thread_pool, handles[i]);

    /* join pieces */
    poly p = NULL;
    for (slong i = num_handles; i >= 0; i--)
    {
        if (args[i].poly_end != NULL)
        {
            pNext(args[i].poly_end) = p;
            p = args[i].poly_start;
        }
    }

    for (slong i = 0; i < num_handles; i++)
        thread_pool_give_back(global_thread_pool, handles[i]);
    if (handles)
        flint_free(handles);

    p_Test(p,r);
    return p;
}

#endif

/****** polynomial operations ***********/

poly Flint_Mult_MP(poly p,int lp, poly q, int lq, fmpq_mpoly_ctx_t ctx, const ring r)
{
  fmpq_mpoly_t pp,qq,res;
  convSingPFlintMP(pp,ctx,p,lp,r); // pp read only
  convSingPFlintMP(qq,ctx,q,lq,r); // qq read only
  fmpq_mpoly_init(res,ctx);
  fmpq_mpoly_mul(res,pp,qq,ctx);
  poly pres=convFlintMPSingP(res,ctx,r);
  fmpq_mpoly_clear(res,ctx);
  _fmpq_mpoly_clear_readonly_sing(pp,ctx);
  _fmpq_mpoly_clear_readonly_sing(qq,ctx);
  fmpq_mpoly_ctx_clear(ctx);
  p_Test(pres,r);
  return pres;
}

poly Flint_Mult_MP(poly p,int lp, poly q, int lq, nmod_mpoly_ctx_t ctx, const ring r)
{
  nmod_mpoly_t pp,qq,res;
  convSingPFlintMP(pp,ctx,p,lp,r);
  convSingPFlintMP(qq,ctx,q,lq,r);
  nmod_mpoly_init(res,ctx);
  nmod_mpoly_mul(res,pp,qq,ctx);
  poly pres=convFlintMPSingP(res,ctx,r);
  nmod_mpoly_clear(res,ctx);
  nmod_mpoly_clear(pp,ctx);
  nmod_mpoly_clear(qq,ctx);
  nmod_mpoly_ctx_clear(ctx);
  p_Test(pres,r);
  return pres;
}

// Zero will be returned if the division is not exact
poly Flint_Divide_MP(poly p,int lp, poly q, int lq, fmpq_mpoly_ctx_t ctx, const ring r)
{
  fmpq_mpoly_t pp,qq,res;
  convSingPFlintMP(pp,ctx,p,lp,r); // pp read only
  convSingPFlintMP(qq,ctx,q,lq,r); // qq read only
  fmpq_mpoly_init(res,ctx);
  fmpq_mpoly_divides(res,pp,qq,ctx);
  poly pres = convFlintMPSingP(res,ctx,r);
  fmpq_mpoly_clear(res,ctx);
  _fmpq_mpoly_clear_readonly_sing(pp,ctx);
  _fmpq_mpoly_clear_readonly_sing(qq,ctx);
  fmpq_mpoly_ctx_clear(ctx);
  p_Test(pres,r);
  return pres;
}

poly Flint_Divide_MP(poly p,int lp, poly q, int lq, nmod_mpoly_ctx_t ctx, const ring r)
{
  nmod_mpoly_t pp,qq,res;
  convSingPFlintMP(pp,ctx,p,lp,r);
  convSingPFlintMP(qq,ctx,q,lq,r);
  nmod_mpoly_init(res,ctx);
  nmod_mpoly_divides(res,pp,qq,ctx);
  poly pres=convFlintMPSingP(res,ctx,r);
  nmod_mpoly_clear(res,ctx);
  nmod_mpoly_clear(pp,ctx);
  nmod_mpoly_clear(qq,ctx);
  nmod_mpoly_ctx_clear(ctx);
  p_Test(pres,r);
  return pres;
}

poly Flint_GCD_MP(poly p,int lp,poly q,int lq,nmod_mpoly_ctx_t ctx,const ring r)
{
  nmod_mpoly_t pp,qq,res;
  convSingPFlintMP(pp,ctx,p,lp,r);
  convSingPFlintMP(qq,ctx,q,lq,r);
  nmod_mpoly_init(res,ctx);
  int ok=nmod_mpoly_gcd(res,pp,qq,ctx);
  poly pres;
  if (ok)
  {
    pres=convFlintMPSingP(res,ctx,r);
    p_Test(pres,r);
  }
  else
  {
    pres=p_One(r);
  }
  nmod_mpoly_clear(res,ctx);
  nmod_mpoly_clear(pp,ctx);
  nmod_mpoly_clear(qq,ctx);
  nmod_mpoly_ctx_clear(ctx);
  return pres;
}

poly Flint_GCD_MP(poly p,int lp,poly q,int lq,fmpq_mpoly_ctx_t ctx,const ring r)
{
  fmpq_mpoly_t pp,qq,res;
  convSingPFlintMP(pp,ctx,p,lp,r); // pp read only
  convSingPFlintMP(qq,ctx,q,lq,r); // qq read only
  fmpq_mpoly_init(res,ctx);
  int ok=fmpq_mpoly_gcd(res,pp,qq,ctx);
  poly pres;
  if (ok)
  {
    // Flint normalizes the gcd to be monic.
    // Singular wants a gcd defined over ZZ that is primitive and has a positive leading coeff.
    if (!fmpq_mpoly_is_zero(res, ctx))
    {
      fmpq_t content;
      fmpq_init(content);
      fmpq_mpoly_content(content, res, ctx);
      fmpq_mpoly_scalar_div_fmpq(res, res, content, ctx);
      fmpq_clear(content);
    }
    pres=convFlintMPSingP(res,ctx,r);
    p_Test(pres,r);
  }
  else
  {
    pres=p_One(r);
  }
  fmpq_mpoly_clear(res,ctx);
  _fmpq_mpoly_clear_readonly_sing(pp,ctx);
  _fmpq_mpoly_clear_readonly_sing(qq,ctx);
  fmpq_mpoly_ctx_clear(ctx);
  return pres;
}

#endif
#endif
