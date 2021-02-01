/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/*
* ABSTRACT: flint: rational functions over Q (using fmpq_mpoly)
*/
#include <ctype.h> /* isdigit*/

#include "misc/auxiliary.h"

#include "coeffs/coeffs.h"

#ifdef HAVE_FLINT
#include "flint/flint.h"
#if __FLINT_RELEASE >= 20503
#include "factory/factory.h"

#include "coeffs/numbers.h"
#include "coeffs/longrat.h"
#include "coeffs/flintcf_Qrat.h"
#include "polys/flint_mpoly.h"
#ifdef QA_DEBUG
#define TRANSEXT_PRIVATES
#include "polys/ext_fields/transext.h"
#include "polys/monomials/p_polys.h"
#endif

typedef fmpq_rat_struct *fmpq_rat_ptr;
typedef fmpq_mpoly_struct *fmpq_mpoly_ptr;
typedef fmpq_mpoly_ctx_struct *fmpq_ctx_ptr;
typedef fmpz *fmpz_ptr;
typedef fmpq_rat_data_struct *data_ptr;

/******************************************************************************
* Helper functions
******************************************************************************/

/*2
* extracts a long integer from s, returns the rest
*/
static char * nlEatLong(char *s, fmpz_ptr i)
{
  const char * start = s;

  while (*s >= '0' && *s <= '9') s++;
  if (*s == '\0')
  {
    fmpz_set_str(i, start, 10);
  }
  else
  {
    char c = *s;
    *s = '\0';
    fmpz_set_str(i, start, 10);
    *s = c;
  }
  return s;
}

static void fmpq_rat_init(fmpq_rat_ptr a, const coeffs r)
{
  fmpq_mpoly_init(a->num, ((data_ptr)r->data)->ctx);
  fmpq_mpoly_init(a->den, ((data_ptr)r->data)->ctx);
}

static void fmpq_rat_clear(fmpq_rat_ptr a, const coeffs r)
{
  fmpq_mpoly_clear(a->num, ((data_ptr)r->data)->ctx);
  fmpq_mpoly_clear(a->den, ((data_ptr)r->data)->ctx);
}

static void fmpq_rat_canonicalise(fmpq_rat_ptr a, const coeffs r)
{
  fmpz_t n, d;
  fmpz_init(n);
  fmpz_init(d);
  fmpz_gcd(n, fmpq_numref(a->num->content), fmpq_numref(a->den->content));
  fmpz_lcm(d, fmpq_denref(a->num->content), fmpq_denref(a->den->content));
  if (!fmpz_is_one(d))
  {
     fmpq_mul_fmpz(a->num->content, a->num->content, d);
     fmpq_mul_fmpz(a->den->content, a->den->content, d);
  }
  if (!fmpz_is_one(n))
  {
     fmpq_div_fmpz(a->num->content, a->num->content, n);
     fmpq_div_fmpz(a->den->content, a->den->content, n);
  }
  fmpz_clear(n);
  fmpz_clear(d);
}

/******************************************************************************
* Main interface
******************************************************************************/

static BOOLEAN CoeffIsEqual(const coeffs c, n_coeffType n, void * parameter)
{
  if (c->type == n)
  {
    const fmpq_ctx_ptr ctx = (fmpq_ctx_ptr) ((data_ptr)c->data)->ctx;
    const QaInfo *par=(QaInfo*)parameter;
    if (par->N != c->iNumberOfParameters) return FALSE;
    // compare parameter names
    for(int i=0;i<par->N;i++)
    {
      if (strcmp(par->names[i],c->pParameterNames[i])!=0) return FALSE;
    }
    return TRUE;
  }
  return FALSE;
}

static number Mult(number a, number b, const coeffs c)
{
  n_Test(a,c);
  n_Test(b,c);
  fmpq_rat_ptr res = (fmpq_rat_ptr) omAlloc(sizeof(fmpq_rat_struct));
  fmpq_rat_init(res, c);
  const fmpq_rat_ptr x = (fmpq_rat_ptr) a;
  const fmpq_rat_ptr y = (fmpq_rat_ptr) b;
  const fmpq_ctx_ptr ctx = (fmpq_ctx_ptr) ((data_ptr)c->data)->ctx;
  if (fmpq_mpoly_equal(x->den, y->den, ctx)) /* denominators equal */
  {
    fmpq_mpoly_mul(res->num, x->num, y->num, ctx);
    fmpq_mpoly_mul(res->den, x->den, y->den, ctx);
  }
  else if (fmpq_mpoly_is_one(x->den, ctx)) /* first denominator 1 */
  {
    fmpq_mpoly_t gd;
    fmpq_mpoly_init(gd, ctx);
    fmpq_mpoly_gcd(gd, x->num, y->den, ctx);
    if (fmpq_mpoly_is_one(gd, ctx))
    {
      fmpq_mpoly_mul(res->num, x->num, y->num, ctx);
      fmpq_mpoly_set(res->den, y->den, ctx);
    }
    else
    {
      fmpq_mpoly_div(res->num, x->num, gd, ctx);
      fmpq_mpoly_mul(res->num, res->num, y->num, ctx);
      fmpq_mpoly_div(res->den, y->den, gd, ctx);
    }
    fmpq_mpoly_clear(gd, ctx);
  }
  else if (fmpq_mpoly_is_one(y->den, ctx)) /* second denominator 1 */
  {
    fmpq_mpoly_t gd;
    fmpq_mpoly_init(gd, ctx);
    fmpq_mpoly_gcd(gd, y->num, x->den, ctx);
    if (fmpq_mpoly_is_one(gd, ctx))
    {
      fmpq_mpoly_mul(res->num, x->num, y->num, ctx);
      fmpq_mpoly_set(res->den, x->den, ctx);
    }
    else
    {
      fmpq_mpoly_div(res->num, y->num, gd, ctx);
      fmpq_mpoly_mul(res->num, res->num, x->num, ctx);
      fmpq_mpoly_div(res->den, x->den, gd, ctx);
    }
    fmpq_mpoly_clear(gd, ctx);
  }
  else /* general case */
  {
    fmpq_mpoly_t g1, g2;
    fmpq_mpoly_ptr n1, n2, d1, d2;
    fmpq_mpoly_init(g1, ctx);
    fmpq_mpoly_init(g2, ctx);
    fmpq_mpoly_gcd(g1, x->num, y->den, ctx);
    fmpq_mpoly_gcd(g2, y->num, x->den, ctx);
    n1 = x->num; d2 = y->den;
    d1 = x->den; n2 = y->num;
    if (!fmpq_mpoly_is_one(g1, ctx))
    {
      fmpq_mpoly_div(res->num, x->num, g1, ctx);
      fmpq_mpoly_div(g1, y->den, g1, ctx);
      n1 = res->num; d2 = g1;
    }
    if (!fmpq_mpoly_is_one(g2, ctx))
    {
      fmpq_mpoly_div(res->den, y->num, g2, ctx);
      fmpq_mpoly_div(g2, x->den, g2, ctx);
      n2 = res->den; d1 = g2;
    }
    fmpq_mpoly_mul(res->num, n1, n2, ctx);
    fmpq_mpoly_mul(res->den, d1, d2, ctx);
    fmpq_mpoly_clear(g1, ctx);
    fmpq_mpoly_clear(g2, ctx);
  }
  fmpq_rat_canonicalise(res, c);
  #ifdef QA_DEBUG
  res->p=n_Mult(x->p,y->p, ((data_ptr)c->data)->C);
  #endif
  n_Test((number)res, c);
  return (number) res;
}

static number Sub(number a, number b, const coeffs c)
{
  fmpq_rat_ptr res = (fmpq_rat_ptr) omAlloc(sizeof(fmpq_rat_struct));
  fmpq_rat_init(res, c);
  const fmpq_rat_ptr x = (fmpq_rat_ptr) a;
  const fmpq_rat_ptr y = (fmpq_rat_ptr) b;
  const fmpq_ctx_ptr ctx = (fmpq_ctx_ptr) ((data_ptr)c->data)->ctx;
  if (fmpq_mpoly_equal(x->den, y->den, ctx)) /* denominators equal */
  {
    fmpq_mpoly_sub(res->num, x->num, y->num, ctx);
    if (fmpq_mpoly_is_zero(res->num, ctx))
    {
      fmpq_mpoly_one(res->den, ctx);
      n_Test((number)res,c);
      return (number)res;
    }
    else
    if (fmpq_mpoly_is_one(x->den, ctx))
    {
      fmpq_mpoly_set(res->den, x->den, ctx);
      n_Test((number)res,c);
      return (number)res;
    }
    else
    {
      fmpq_mpoly_t gd;
      fmpq_mpoly_init(gd, ctx);
      fmpq_mpoly_gcd(gd, res->num, x->den, ctx);
      if (fmpq_mpoly_is_one(gd, ctx))
      {
        fmpq_mpoly_set(res->den, x->den, ctx);
      }
      else
      {
        fmpq_mpoly_div(res->den, x->den, gd, ctx);
        fmpq_mpoly_div(res->num, res->num, gd, ctx);
      }
      fmpq_mpoly_clear(gd, ctx);
    }
  }
  else if (fmpq_mpoly_is_one(x->den, ctx)) /* first denominator 1 */
  {
    fmpq_mpoly_mul(res->num, x->num, y->den, ctx);
    fmpq_mpoly_sub(res->num, res->num, y->num, ctx);
    if (fmpq_mpoly_is_zero(res->num, ctx))
    {
      fmpq_mpoly_one(res->den, ctx);
      n_Test((number)res,c);
      return (number)res;
    }
    else
    {
      fmpq_mpoly_set(res->den, y->den, ctx);
    }
  }
  else if (fmpq_mpoly_is_one(y->den, ctx)) /* second denominator 1 */
  {
    fmpq_mpoly_mul(res->num, y->num, x->den, ctx);
    fmpq_mpoly_sub(res->num, x->num, res->num, ctx);
    if (fmpq_mpoly_is_zero(res->num,ctx))
    {
      fmpq_mpoly_one(res->den, ctx);
      n_Test((number)res,c);
      return (number)res;
    }
    else
    {
      fmpq_mpoly_set(res->den, x->den, ctx);
    }
  }
  else /* general case */
  {
    fmpq_mpoly_t gd;
    fmpq_mpoly_init(gd, ctx);
    fmpq_mpoly_gcd(gd, x->den, y->den, ctx);
    if (fmpq_mpoly_is_one(gd, ctx))
    {
      fmpq_mpoly_mul(res->num, x->num, y->den, ctx);
      fmpq_mpoly_mul(gd, y->num, x->den, ctx);
      fmpq_mpoly_sub(res->num, res->num, gd, ctx);
      if (fmpq_mpoly_is_zero(res->num,ctx))
      {
        fmpq_mpoly_one(res->den, ctx);
        n_Test((number)res,c);
        return (number)res;
      }
      else
      {
        fmpq_mpoly_mul(res->den, x->den, y->den, ctx);
      }
    }
    else
    {
      fmpq_mpoly_t q2;
      fmpq_mpoly_init(q2, ctx);
      fmpq_mpoly_div(res->den, x->den, gd, ctx);
      fmpq_mpoly_div(q2, y->den, gd, ctx);
      fmpq_mpoly_mul(res->num, q2, x->num, ctx);
      fmpq_mpoly_mul(res->den, res->den, y->num, ctx);
      fmpq_mpoly_sub(res->num, res->num, res->den, ctx);
      fmpq_mpoly_gcd(res->den, res->num, gd, ctx);
      if (fmpq_mpoly_is_one(res->den, ctx))
      {
        fmpq_mpoly_mul(res->den, q2, x->den, ctx);
      }
      else
      {
        fmpq_mpoly_div(res->num, res->num, res->den, ctx);
        fmpq_mpoly_div(gd, x->den, res->den, ctx);
        fmpq_mpoly_mul(res->den, gd, q2, ctx);
      }
      fmpq_mpoly_clear(q2, ctx);
    }
    fmpq_mpoly_clear(gd, ctx);
  }
  #ifdef QA_DEBUG
  res->p=n_Sub(x->p,y->p, ((data_ptr)c->data)->C);
  #endif
  n_Test((number)res, c);
  return (number) res;
}

static number Add(number a, number b, const coeffs c)
{
  fmpq_rat_ptr res = (fmpq_rat_ptr) omAlloc(sizeof(fmpq_rat_struct));
  fmpq_rat_init(res, c);
  const fmpq_rat_ptr x = (fmpq_rat_ptr) a;
  const fmpq_rat_ptr y = (fmpq_rat_ptr) b;
  const fmpq_ctx_ptr ctx = (fmpq_ctx_ptr) ((data_ptr)c->data)->ctx;
  if (fmpq_mpoly_equal(x->den, y->den, ctx)) /* denominators equal */
  {
    fmpq_mpoly_add(res->num, x->num, y->num, ctx);
    if (fmpq_mpoly_is_zero(res->num, ctx))
    {
      fmpq_mpoly_one(res->den, ctx);
      n_Test((number)res,c);
      return (number)res;
    }
    else
    if (fmpq_mpoly_is_one(x->den, ctx))
    {
      fmpq_mpoly_set(res->den, x->den, ctx);
      n_Test((number)res,c);
      return (number)res;
    }
    else
    {
      fmpq_mpoly_t gd;
      fmpq_mpoly_init(gd, ctx);
      fmpq_mpoly_gcd(gd, res->num, x->den, ctx);
      if (fmpq_mpoly_is_one(gd, ctx))
      {
        fmpq_mpoly_set(res->den, x->den, ctx);
      }
      else
      {
        fmpq_mpoly_div(res->den, x->den, gd, ctx);
        fmpq_mpoly_div(res->num, res->num, gd, ctx);
      }
      fmpq_mpoly_clear(gd, ctx);
    }
  }
  else if (fmpq_mpoly_is_one(x->den, ctx)) /* first denominator 1 */
  {
    fmpq_mpoly_mul(res->num, x->num, y->den, ctx);
    fmpq_mpoly_add(res->num, res->num, y->num, ctx);
    if (fmpq_mpoly_is_zero(res->num, ctx))
    {
      fmpq_mpoly_one(res->den, ctx);
      n_Test((number)res,c);
      return (number)res;
    }
    else
    {
      fmpq_mpoly_set(res->den, y->den, ctx);
    }
  }
  else if (fmpq_mpoly_is_one(y->den, ctx)) /* second denominator 1 */
  {
    fmpq_mpoly_mul(res->num, y->num, x->den, ctx);
    fmpq_mpoly_add(res->num, x->num, res->num, ctx);
    if (fmpq_mpoly_is_zero(res->num, ctx))
    {
      fmpq_mpoly_one(res->den, ctx);
      n_Test((number)res,c);
      return (number)res;
    }
    else
    {
      fmpq_mpoly_set(res->den, x->den, ctx);
    }
  }
  else /* general case */
  {
    fmpq_mpoly_t gd;
    fmpq_mpoly_init(gd, ctx);
    fmpq_mpoly_gcd(gd, x->den, y->den, ctx);
    if (fmpq_mpoly_is_one(gd, ctx))
    {
      fmpq_mpoly_mul(res->num, x->num, y->den, ctx);
      fmpq_mpoly_mul(gd, y->num, x->den, ctx);
      fmpq_mpoly_add(res->num, res->num, gd, ctx);
      if (fmpq_mpoly_is_zero(res->num,ctx))
      {
        fmpq_mpoly_one(res->den, ctx);
        n_Test((number)res,c);
        return (number)res;
      }
      else
      {
        fmpq_mpoly_mul(res->den, x->den, y->den, ctx);
      }
    }
    else
    {
      fmpq_mpoly_t q2;
      fmpq_mpoly_init(q2, ctx);
      fmpq_mpoly_div(res->den, x->den, gd, ctx);
      fmpq_mpoly_div(q2, y->den, gd, ctx);
      fmpq_mpoly_mul(res->num, q2, x->num, ctx);
      fmpq_mpoly_mul(res->den, res->den, y->num, ctx);
      fmpq_mpoly_add(res->num, res->num, res->den, ctx);
      fmpq_mpoly_gcd(res->den, res->num, gd, ctx);
      if (fmpq_mpoly_is_one(res->den, ctx))
      {
        fmpq_mpoly_mul(res->den, q2, x->den, ctx);
      }
      else
      {
        fmpq_mpoly_div(res->num, res->num, res->den, ctx);
        fmpq_mpoly_div(gd, x->den, res->den, ctx);
        fmpq_mpoly_mul(res->den, gd, q2, ctx);
      }
      fmpq_mpoly_clear(q2, ctx);
    }
    fmpq_mpoly_clear(gd, ctx);
  }
  #ifdef QA_DEBUG
  res->p=n_Add(x->p,y->p, ((data_ptr)c->data)->C);
  #endif
  n_Test((number)res, c);
  return (number) res;
}

static number Div(number a, number b, const coeffs c)
{
  const fmpq_rat_ptr x = (fmpq_rat_ptr) a;
  const fmpq_rat_ptr y = (fmpq_rat_ptr) b;
  const fmpq_ctx_ptr ctx = (fmpq_ctx_ptr) ((data_ptr)c->data)->ctx;
  fmpq_rat_ptr res = (fmpq_rat_ptr) omAlloc(sizeof(fmpq_rat_struct));
  fmpq_rat_init(res, c);
  if (fmpq_mpoly_is_zero(y->num, ctx))
  {
    WerrorS(nDivBy0);
    return (number)res;
  }
  if (fmpq_mpoly_equal(x->den, y->num, ctx)) /* denominators equal */
  {
    fmpq_mpoly_mul(res->num, x->num, y->den, ctx);
    fmpq_mpoly_mul(res->den, x->den, y->num, ctx);
  }
  else if (fmpq_mpoly_is_one(x->den, ctx)) /* first denominator 1 */
  {
    fmpq_mpoly_t gd;
    fmpq_mpoly_init(gd, ctx);
    fmpq_mpoly_gcd(gd, x->num, y->num, ctx);
    if (fmpq_mpoly_is_one(gd, ctx))
    {
      fmpq_mpoly_mul(res->num, x->num, y->den, ctx);
      fmpq_mpoly_set(res->den, y->num, ctx);
    }
    else
    {
      fmpq_mpoly_div(res->num, x->num, gd, ctx);
      fmpq_mpoly_mul(res->num, res->num, y->den, ctx);
      fmpq_mpoly_div(res->den, y->num, gd, ctx);
    }
    fmpq_mpoly_clear(gd, ctx);
  }
  else if (fmpq_mpoly_is_one(y->num, ctx)) /* second denominator 1 */
  {
    fmpq_mpoly_t gd;
    fmpq_mpoly_init(gd, ctx);
    fmpq_mpoly_gcd(gd, y->den, x->den, ctx);
    if (fmpq_mpoly_is_one(gd, ctx))
    {
      fmpq_mpoly_mul(res->num, y->den, x->num, ctx);
      fmpq_mpoly_set(res->den, x->den, ctx);
    }
    else
    {
      fmpq_mpoly_div(res->num, y->den, gd, ctx);
      fmpq_mpoly_mul(res->num, res->num, x->num, ctx);
      fmpq_mpoly_div(res->den, x->den, gd, ctx);
    }
    fmpq_mpoly_clear(gd, ctx);
  }
  else /* general case */
  {
    fmpq_mpoly_t g1, g2;
    fmpq_mpoly_ptr n1, n2, d1, d2;
    fmpq_mpoly_init(g1, ctx);
    fmpq_mpoly_init(g2, ctx);
    fmpq_mpoly_gcd(g1, x->num, y->num, ctx);
    fmpq_mpoly_gcd(g2, y->den, x->den, ctx);
    n1 = x->num; d2 = y->num;
    d1 = x->den; n2 = y->den;
    if (!fmpq_mpoly_is_one(g1, ctx))
    {
      fmpq_mpoly_div(res->num, x->num, g1, ctx);
      fmpq_mpoly_div(g1, y->num, g1, ctx);
      n1 = res->num; d2 = g1;
    }
    if (!fmpq_mpoly_is_one(g2, ctx))
    {
      fmpq_mpoly_div(res->den, y->den, g2, ctx);
      fmpq_mpoly_div(g2, x->den, g2, ctx);
      n2 = res->den; d1 = g2;
    }
    fmpq_mpoly_mul(res->num, n1, n2, ctx);
    fmpq_mpoly_mul(res->den, d1, d2, ctx);
    fmpq_mpoly_clear(g1, ctx);
    fmpq_mpoly_clear(g2, ctx);
  }
  fmpq_rat_canonicalise(res, c);
  #ifdef QA_DEBUG
  res->p=n_Div(x->p,y->p, ((data_ptr)c->data)->C);
  #endif
  n_Test((number)res, c);
  return (number) res;
}

static number ExactDiv(number a, number b, const coeffs c)
{
  const fmpq_rat_ptr x = (fmpq_rat_ptr) a;
  const fmpq_rat_ptr y = (fmpq_rat_ptr) b;
  const fmpq_ctx_ptr ctx = (fmpq_ctx_ptr) ((data_ptr)c->data)->ctx;
  fmpq_rat_ptr res = (fmpq_rat_ptr) omAlloc(sizeof(fmpq_rat_struct));
  fmpq_rat_init(res, c);
  if (fmpq_mpoly_is_zero(y->num, ctx))
  {
     WerrorS(nDivBy0);
     return (number)res;
  }
  fmpq_mpoly_div(res->num, x->num, y->num, ctx);
  assume(fmpq_mpoly_is_one(x->den, ctx));
  assume(fmpq_mpoly_is_one(y->den, ctx));
  #ifdef QA_DEBUG
  res->p=n_ExactDiv(x->p,y->p, ((data_ptr)c->data)->C);
  #endif
  n_Test((number)res,c);
  return (number) res;
}

// static number IntMod(number a, number b, const coeffs c);
// {
// }

static number Init(long i, const coeffs c)
{
  fmpq_rat_ptr res = (fmpq_rat_ptr) omAlloc(sizeof(fmpq_rat_struct));
  const fmpq_ctx_ptr ctx = (fmpq_ctx_ptr) ((data_ptr)c->data)->ctx;
  fmpq_rat_init(res, c);
  fmpq_mpoly_set_si(res->num, (slong) i, ctx);
  fmpq_mpoly_set_si(res->den, (slong) 1, ctx);
  #ifdef QA_DEBUG
  res->p=n_Init(i, ((data_ptr)c->data)->C);
  #endif
  n_Test((number)res,c);
  return (number) res;
}

static number InitMPZ(mpz_t i, const coeffs c)
{
  fmpq_rat_ptr res = (fmpq_rat_ptr) omAlloc(sizeof(fmpq_rat_struct));
  const fmpq_ctx_ptr ctx = (fmpq_ctx_ptr) ((data_ptr)c->data)->ctx;
  fmpz_t t;
  fmpz_init(t);
  fmpz_set_mpz(t, i);
  fmpq_rat_init(res, c);
  fmpq_mpoly_set_fmpz(res->num, t, ctx);
  fmpq_mpoly_set_si(res->den, (slong) 1, ctx);
  fmpz_clear(t);
  #ifdef QA_DEBUG
  res->p=n_InitMPZ(i, ((data_ptr)c->data)->C);
  #endif
  n_Test((number)res,c);
  return (number) res;
}

static int Size(number n, const coeffs c)
{
  const fmpq_rat_ptr x = (fmpq_rat_ptr) n;
  const fmpq_ctx_ptr ctx = (fmpq_ctx_ptr) ((data_ptr)c->data)->ctx;
  if (fmpq_mpoly_is_zero(x->num, ctx))
    return 0;
  unsigned long len=fmpq_mpoly_length(x->num, ctx) +
         fmpq_mpoly_length(x->den, ctx)-fmpq_mpoly_is_one(x->den, ctx);
  unsigned long numDegree=fmpq_mpoly_total_degree_si(x->num, ctx);
  unsigned long denDegree=fmpq_mpoly_total_degree_si(x->den, ctx);
  unsigned long t= ((numDegree + denDegree)*(numDegree + denDegree) + 1) * len;
  if (t>INT_MAX) return INT_MAX;
  else return (int)t;
}

static long Int(number &n, const coeffs c)
{
  const fmpq_rat_ptr x = (fmpq_rat_ptr) n;
  const fmpq_ctx_ptr ctx = (fmpq_ctx_ptr) ((data_ptr)c->data)->ctx;
  if (fmpq_mpoly_is_fmpq(x->den, ctx) && fmpq_mpoly_is_fmpq(x->num, ctx))
  {
    long nl = 0;
    fmpq_t r;
    fmpq_init(r);
    fmpq_div(r, x->num->content, x->den->content);
    if (fmpz_is_one(fmpq_denref(r)))
    {
      if (fmpz_fits_si(fmpq_numref(r)))
        nl = fmpz_get_si(fmpq_numref(r));
    }
    fmpq_clear(r);
    return nl;
  }
  return 0;
}

static void MPZ(mpz_t result, number &n, const coeffs c)
{
  mpz_init(result);
  const fmpq_rat_ptr x = (fmpq_rat_ptr) n;
  const fmpq_ctx_ptr ctx = (fmpq_ctx_ptr) ((data_ptr)c->data)->ctx;
  if (fmpq_mpoly_is_fmpq(x->den, ctx) && fmpq_mpoly_is_fmpq(x->num, ctx))
  {
    long nl = 0;
    fmpq_t r;
    fmpq_init(r);
    fmpq_div(r, x->num->content, x->den->content);
    if (fmpz_is_one(fmpq_denref(r)))
    {
      fmpz_get_mpz(result, fmpq_numref(r));
    }
    fmpq_clear(r);
  }
}

static number Neg(number a, const coeffs c)
{
  const fmpq_rat_ptr x = (fmpq_rat_ptr) a;
  const fmpq_ctx_ptr ctx = (fmpq_ctx_ptr) ((data_ptr)c->data)->ctx;
  fmpq_mpoly_neg(x->num, x->num, ctx);
  #ifdef QA_DEBUG
  x->p=n_InpNeg(x->p, ((data_ptr)c->data)->C);
  #endif
  n_Test((number)a, c);
  return a;
}

static number Invers(number a, const coeffs c)
{
  const fmpq_rat_ptr x = (fmpq_rat_ptr) a;
  const fmpq_ctx_ptr ctx = (fmpq_ctx_ptr) ((data_ptr)c->data)->ctx;
  if (fmpq_mpoly_is_zero(x->num, ctx))
  {
    WerrorS(nDivBy0);
    return NULL;
  }
  else
  {
    fmpq_rat_ptr res = (fmpq_rat_ptr) omAlloc(sizeof(fmpq_rat_struct));
    fmpq_rat_init(res, c);
    fmpq_mpoly_set(res->num, x->den, ctx);
    fmpq_mpoly_set(res->den, x->num, ctx);
  #ifdef QA_DEBUG
  res->p=n_Invers(x->p, ((data_ptr)c->data)->C);
  #endif
  n_Test((number)res, c);
    return (number) res;
  }
}

static number Copy(number a, const coeffs c)
{
  fmpq_rat_ptr res = (fmpq_rat_ptr) omAlloc(sizeof(fmpq_rat_struct));
  fmpq_rat_init(res, c);
  const fmpq_rat_ptr x = (fmpq_rat_ptr) a;
  const fmpq_ctx_ptr ctx = (fmpq_ctx_ptr) ((data_ptr)c->data)->ctx;
  fmpq_mpoly_set(res->num, x->num, ctx);
  fmpq_mpoly_set(res->den, x->den, ctx);
  #ifdef QA_DEBUG
  res->p=n_Copy(x->p, ((data_ptr)c->data)->C);
  #endif
  n_Test((number)res, c);
  return (number) res;
}

//static number RePart(number a, const coeffs c)
//{
//}

//static number ImPart(number a, const coeffs c)
//{
//}

static BOOLEAN IsOne(number a, const coeffs c)
{
  if (a==NULL) return FALSE;
  const fmpq_rat_ptr x = (fmpq_rat_ptr) a;
  const fmpq_ctx_ptr ctx = (fmpq_ctx_ptr) ((data_ptr)c->data)->ctx;
  if (!fmpq_mpoly_is_fmpq(x->num, ctx))
    return FALSE;
  if (!fmpq_mpoly_is_fmpq(x->den, ctx))
    return FALSE;
  return fmpq_equal(x->num->content, x->den->content);
}

static BOOLEAN IsZero(number a, const coeffs c)
{
  const fmpq_rat_ptr x = (fmpq_rat_ptr) a;
  const fmpq_ctx_ptr ctx = (fmpq_ctx_ptr) ((data_ptr)c->data)->ctx;
  return fmpq_mpoly_is_zero(x->num, ctx);
}

static void WriteLong(number a, const coeffs c)
{
  if (a==NULL)
  {
    StringAppendS("o");
    return;
  }
  n_Test(a,c);
  const fmpq_rat_ptr x = (fmpq_rat_ptr) a;
  const fmpq_ctx_ptr ctx = (fmpq_ctx_ptr) ((data_ptr)c->data)->ctx;
  if (fmpq_mpoly_is_zero(x->den, ctx))
  {
    StringAppendS("?/o");
    return;
  }
  fmpz_t t;
  long int i, j, k, nmax_i, dmax_i, max_digits;
  fmpq_rat_canonicalise(x, c);
  if (fmpq_mpoly_is_zero(x->num, ctx))
    StringAppendS("0");
  else
  {
    BOOLEAN num_is_const = fmpq_mpoly_is_fmpq(x->num, ctx);
    BOOLEAN den_is_const = fmpq_mpoly_is_fmpq(x->den, ctx);
    BOOLEAN need_times;
    fmpq_mpoly_struct * znum = x->num;
    fmpq_mpoly_struct * zden = x->den;
    slong nvars = fmpq_mpoly_ctx_nvars(ctx);
    fmpz_init(t);
    nmax_i = 0;
    dmax_i = 0;
    for (i = 1; i < fmpq_mpoly_length(znum, ctx); i++)
    {
      if (fmpz_cmpabs(fmpq_mpoly_zpoly_term_coeff_ref(znum, i, ctx),
                      fmpq_mpoly_zpoly_term_coeff_ref(znum, nmax_i, ctx)) > 0)
      {
         nmax_i = i;
      }
    }
    for (i = 1; i < fmpq_mpoly_length(zden, ctx); i++)
    {
      if (fmpz_cmpabs(fmpq_mpoly_zpoly_term_coeff_ref(zden, i, ctx),
                      fmpq_mpoly_zpoly_term_coeff_ref(zden, dmax_i, ctx)) > 0)
      {
        dmax_i = i;
      }
    }
    if (fmpz_cmpabs(fmpq_mpoly_zpoly_term_coeff_ref(znum, nmax_i, ctx),
                       fmpq_mpoly_zpoly_term_coeff_ref(zden, dmax_i, ctx)) > 0)
    {
      fmpz_mul(t, fmpq_numref(x->num->content),
                  fmpq_mpoly_zpoly_term_coeff_ref(znum, nmax_i, ctx));
      max_digits = fmpz_sizeinbase(t, 10);
    }
    else
    {
      fmpz_mul(t, fmpq_numref(x->den->content),
                  fmpq_mpoly_zpoly_term_coeff_ref(zden, dmax_i, ctx));
      max_digits =fmpz_sizeinbase(t, 10);
    }
    char *s = (char*) omAlloc(max_digits + 5);
    if (!num_is_const)
      StringAppendS("(");
    if (fmpq_mpoly_is_one(x->num, ctx))
      StringAppendS("1");
    else
    {
      for (i = 0; i < fmpq_mpoly_length(x->num, ctx); i++)
      {
        need_times = TRUE;
        fmpz_mul(t, fmpq_mpoly_zpoly_term_coeff_ref(znum, i, ctx),
                                       fmpq_numref(x->num->content));
        if (i != 0 && fmpz_sgn(t) > 0)
          StringAppendS("+");
        BOOLEAN need_1=FALSE;
        if (!fmpz_is_one(t))
        {
          fmpz_get_str(s, 10, t);
          {
            int l=strlen(s);
            while((l>0)&&(!isdigit(s[l]))) l--;
            s[l+1]='\0';
          }
          if (strcmp(s,"-1")==0)
          {
            StringAppendS("-");
            need_1 = TRUE;
            need_times = FALSE;
          }
          else
            StringAppendS(s);
        }
        else
        {
          need_1 = TRUE;
          need_times = FALSE;
        }
        for (j = 0; j < c->iNumberOfParameters; j++)
        {
          k = fmpq_mpoly_get_term_var_exp_ui(x->num, i, j, ctx);
          if (k != 0)
          {
            need_1 = FALSE;
            if (need_times)
              StringAppendS("*");
            if (k != 1)
              StringAppend("%s^%d", c->pParameterNames[j], k);
            else
              StringAppendS(c->pParameterNames[j]);
            need_times = TRUE;
          }
        }
        if (need_1)  StringAppendS("1");
      }
    }
    if (!num_is_const)
       StringAppendS(")");
    if (!fmpq_mpoly_is_one(x->den, ctx))
    {
      BOOLEAN closing_paren=FALSE;
      StringAppendS("/");
      if (!den_is_const)
      {
        StringAppendS("(");
        closing_paren = TRUE;
      }
      for (i = 0; i < fmpq_mpoly_length(x->den, ctx); i++)
      {
        need_times = TRUE;
        fmpz_mul(t, fmpq_mpoly_zpoly_term_coeff_ref(zden, i, ctx),
                                       fmpq_numref(x->den->content));
        if (i == 0)
        {
          if ((fmpz_sgn(t) < 0) && den_is_const)
          {
            StringAppendS("(");
            closing_paren = TRUE;
          }
        }
        else if (fmpz_sgn(t) > 0)
          StringAppendS("+");
        if (!fmpz_is_one(t))
        {
          fmpz_get_str(s, 10, t);
          {
            int l=strlen(s);
            while((l>0)&&(!isdigit(s[l]))) l--;
            s[l+1]='\0';
          }
          StringAppendS(s);
        }
        else
        {
          need_times = FALSE;
        }
        for (j = 0; j < nvars; j++)
        {
          k = fmpq_mpoly_get_term_var_exp_ui(x->den, i, j, ctx);
          if (k != 0)
          {
            if (need_times)
              StringAppendS("*");
            if (k != 1)
              StringAppend("%s^%d", c->pParameterNames[j], k);
            else
              StringAppendS(c->pParameterNames[j]);
            need_times = TRUE;
          }
        }
      }
      if (closing_paren)
        StringAppendS(")");
    }
    fmpz_clear(t);
    omFree(s);
  }
}

static void WriteShort(number a, const coeffs c)
{
  const fmpq_rat_ptr x = (fmpq_rat_ptr) a;
  const fmpq_ctx_ptr ctx = (fmpq_ctx_ptr) ((data_ptr)c->data)->ctx;
  fmpz_t t;
  char *s;
  long int i, j, k, nmax_i, dmax_i, max_digits;
  fmpq_rat_canonicalise(x, c);
  if (fmpq_mpoly_is_zero(x->num, ctx))
    StringAppendS("0");
  else
  {
    BOOLEAN num_is_const = fmpq_mpoly_is_fmpq(x->num, ctx);
    BOOLEAN den_is_const = fmpq_mpoly_is_fmpq(x->den, ctx);
    fmpq_mpoly_struct * znum = x->num;
    fmpq_mpoly_struct * zden = x->den;
    slong nvars = fmpq_mpoly_ctx_nvars(ctx);
    fmpz_init(t);
    nmax_i = 0;
    dmax_i = 0;
    for (i = 1; i < fmpq_mpoly_length(znum, ctx); i++)
    {
      if (fmpz_cmpabs(fmpq_mpoly_zpoly_term_coeff_ref(znum, i, ctx),
                      fmpq_mpoly_zpoly_term_coeff_ref(znum, nmax_i, ctx)) > 0)
      {
         nmax_i = i;
      }
    }
    for (i = 1; i < fmpq_mpoly_length(zden, ctx); i++)
    {
      if (fmpz_cmpabs(fmpq_mpoly_zpoly_term_coeff_ref(zden, i, ctx),
                      fmpq_mpoly_zpoly_term_coeff_ref(zden, dmax_i, ctx)) > 0)
      {
        dmax_i = i;
      }
    }
    if (fmpz_cmpabs(fmpq_mpoly_zpoly_term_coeff_ref(znum, nmax_i, ctx),
                       fmpq_mpoly_zpoly_term_coeff_ref(zden, dmax_i, ctx)) > 0)
    {
      fmpz_mul(t, fmpq_numref(x->num->content),
                  fmpq_mpoly_zpoly_term_coeff_ref(znum, nmax_i, ctx));
      max_digits = fmpz_sizeinbase(t, 10);
    } else
    {
      fmpz_mul(t, fmpq_numref(x->den->content),
                  fmpq_mpoly_zpoly_term_coeff_ref(zden, dmax_i, ctx));
      max_digits = fmpz_sizeinbase(t, 10);
    }
    s = (char*) omAlloc(max_digits + 2);
    if (!num_is_const)
      StringAppendS("(");
    if (fmpq_mpoly_is_one(x->num, ctx))
      StringAppendS("1");
    else
    {
      for (i = 0; i < fmpq_mpoly_length(x->num, ctx); i++)
      {
        fmpz_mul(t, fmpq_mpoly_zpoly_term_coeff_ref(znum, i, ctx),
                                       fmpq_numref(x->num->content));
        if (i != 0 && fmpz_sgn(t) > 0)
          StringAppendS("+");
        if (!fmpz_is_one(t))
        {
          fmpz_get_str(s, 10, t);
          StringAppendS(s);
        }
        for (j = 0; j < nvars; j++)
        {
          k = fmpq_mpoly_get_term_var_exp_ui(x->num, i, j, ctx);
          if (k != 0)
          {
            if (k != 1)
              StringAppend("%s%d", c->pParameterNames[j], k);
            else
              StringAppendS(c->pParameterNames[j]);
          }
        }
      }
    }
    if (!num_is_const)
       StringAppendS(")");
    if (!fmpq_mpoly_is_one(x->den, ctx))
    {
      StringAppendS("/");
      if (!den_is_const)
        StringAppendS("(");
      for (i = 0; i < fmpq_mpoly_length(x->den, ctx); i++)
      {
        fmpz_mul(t, fmpq_mpoly_zpoly_term_coeff_ref(zden, i, ctx),
                                       fmpq_numref(x->den->content));
        if (i != 0 && fmpz_sgn(t) > 0)
          StringAppendS("+");
        if (!fmpz_is_one(t))
        {
          fmpz_get_str(s, 10, t);
          StringAppendS(s);
        }
        for (j = 0; j < nvars; j++)
        {
          k = fmpq_mpoly_get_term_var_exp_ui(x->num, i, j, ctx);
          if (k != 0)
          {
            if (k != 1)
              StringAppend("%s%d", c->pParameterNames[j], k);
            else
              StringAppendS(c->pParameterNames[j]);
          }
        }
      }
      if (!den_is_const)
        StringAppendS(")");
    }
    fmpz_clear(t);
  }
}

static const char* Read(const char * st, number * a, const coeffs c)
{
  // we only read "monomials" (i.e. [-][digits][parameter]),
  // everything else (+,*,^,()) is left to the singular interpreter
  long int j;
  char *s = (char *) st;
  const fmpq_ctx_ptr ctx = (fmpq_ctx_ptr) ((data_ptr)c->data)->ctx;
  slong nvars = fmpq_mpoly_ctx_nvars(ctx);
  *a = (number) omAlloc(sizeof(fmpq_rat_struct));
  fmpq_rat_init((fmpq_rat_ptr)(*a), c);
  BOOLEAN neg = FALSE;
  if (*s=='-')
  {
    neg = TRUE;
    s++;
  }
  if (isdigit(*s))
  {
    fmpz_t z;
    fmpz_init(z);
    s = nlEatLong((char *) s, z);
    fmpq_mpoly_set_fmpz(((fmpq_rat_ptr)(*a))->num, z, ctx);
    fmpq_mpoly_one(((fmpq_rat_ptr)(*a))->den, ctx);
    if (*s == '/')
    {
      s++;
      s = nlEatLong((char *) s, z);
      fmpq_mpoly_scalar_div_fmpz(((fmpq_rat_ptr)(*a))->num,
                                    ((fmpq_rat_ptr)(*a))->num, z, ctx);
    }
    fmpz_clear(z);
  }
  else
  {
    BOOLEAN found=FALSE;
    for (j = 0; j < nvars; j++)
    {
      if (strncmp(s, c->pParameterNames[j],
                                  strlen(c->pParameterNames[j])) == 0)
      {
        found=TRUE;
        fmpq_mpoly_gen(((fmpq_rat_ptr)(*a))->num, j, ctx);
        s += strlen(c->pParameterNames[j]);
        if (isdigit(*s))
        {
          int i = 1;
          s = nEati(s, &i, 0);
          if (i != 1)
          {
            fmpq_mpoly_pow_ui(((fmpq_rat_ptr)(*a))->num,
                         ((fmpq_rat_ptr)(*a))->num, (long int) i, ctx);
          }
        }
      }
    }
    if (!found) fmpq_mpoly_one(((fmpq_rat_ptr)(*a))->num, ctx);
    fmpq_mpoly_one(((fmpq_rat_ptr)(*a))->den, ctx);
  }
  if (neg)
    fmpq_mpoly_neg(((fmpq_rat_ptr)(*a))->num, ((fmpq_rat_ptr)(*a))->num, ctx);
  #ifdef QA_DEBUG
  poly pp=convFlintMPSingP(((fmpq_rat_ptr)(*a))->num,ctx,((data_ptr)c->data)->C->extRing);
  fraction f=(fraction)n_Init(1,((data_ptr)c->data)->C); /*leak*/
  NUM(f)=pp;
  ((fmpq_rat_ptr)(*a))->p=(number)f;
  #endif
  n_Test((*a),c);
  return s;
}

static BOOLEAN Greater(number a, number b, const coeffs c)
{
  return Size(a, c) > Size(b, c);
}

static void Delete(number * a, const coeffs c)
{
  if ((*a) != NULL)
  {
    const fmpq_rat_ptr x = (fmpq_rat_ptr) *a;
    fmpq_rat_clear(x, c);
    #ifdef QA_DEBUG
    n_Delete(&(x->p),((data_ptr)c->data)->C);
    #endif
    omFree(*a);
    *a = NULL;
  }
}

static BOOLEAN Equal(number a, number b, const coeffs c)
{
  const fmpq_rat_ptr x = (fmpq_rat_ptr) a;
  const fmpq_rat_ptr y = (fmpq_rat_ptr) b;
  const fmpq_ctx_ptr ctx = (fmpq_ctx_ptr) ((data_ptr)c->data)->ctx;
  if (!fmpz_mpoly_equal(x->num->zpoly, y->num->zpoly, ctx->zctx))
  {
    return FALSE;
  }
  if (!fmpz_mpoly_equal(x->den->zpoly, y->den->zpoly, ctx->zctx))
  {
    return FALSE;
  }
  fmpz_t t1, t2;
  fmpz_init(t1);
  fmpz_init(t2);
  fmpz_mul(t1, fmpq_numref(x->num->content), fmpq_denref(x->den->content));
  fmpz_mul(t1, t1, fmpq_denref(y->num->content));
  fmpz_mul(t1, t1, fmpq_numref(y->den->content));
  fmpz_mul(t2, fmpq_numref(y->num->content), fmpq_denref(y->den->content));
  fmpz_mul(t2, t2, fmpq_denref(x->num->content));
  fmpz_mul(t2, t2, fmpq_numref(x->den->content));
  int eq = fmpz_equal(t1, t2);
  fmpz_clear(t1);
  fmpz_clear(t2);
  return eq;
}

static BOOLEAN IsMOne(number a, const coeffs c)
{
  if (a==NULL) return FALSE;
  fmpq_t content;
  const fmpq_rat_ptr x = (fmpq_rat_ptr) a;
  const fmpq_ctx_ptr ctx = (fmpq_ctx_ptr) ((data_ptr)c->data)->ctx;
  if (!fmpq_mpoly_is_fmpq(x->num, ctx))
    return FALSE;
  if (!fmpq_mpoly_is_fmpq(x->den, ctx))
    return FALSE;
  fmpq_init(content);
  fmpq_neg(content, x->num->content);
  int eq = fmpq_equal(content, x->den->content);
  fmpq_clear(content);
  return eq;
}

static BOOLEAN GreaterZero(number, const coeffs)
{
  return TRUE; /* everything in parens for now so need + sign */
}

static void Power(number a, int i, number * result, const coeffs c)
{
  *result= (number) omAlloc(sizeof(fmpq_rat_struct));
  fmpq_rat_init((fmpq_rat_ptr) (*result), c);
  const fmpq_rat_ptr x = (fmpq_rat_ptr) a;
  const fmpq_ctx_ptr ctx = (fmpq_ctx_ptr) ((data_ptr)c->data)->ctx;
  fmpq_mpoly_pow_ui(((fmpq_rat_ptr)(*result))->num, x->num, (slong) i, ctx);
  fmpq_mpoly_pow_ui(((fmpq_rat_ptr)(*result))->den, x->den, (slong) i, ctx);
}

static number GetDenom(number &n, const coeffs c)
{
  const fmpq_rat_ptr x = (fmpq_rat_ptr) n;
  const fmpq_ctx_ptr ctx = (fmpq_ctx_ptr) ((data_ptr)c->data)->ctx;
  fmpq_rat_ptr res = (fmpq_rat_ptr) omAlloc(sizeof(fmpq_rat_struct));
  fmpq_rat_init(res, c);
  fmpq_mpoly_set(res->num, x->den, ctx);
  fmpq_mpoly_one(res->den, ctx);
  return (number) res;
}

static number GetNumerator(number &n, const coeffs c)
{
  const fmpq_rat_ptr x = (fmpq_rat_ptr) n;
  const fmpq_ctx_ptr ctx = (fmpq_ctx_ptr) ((data_ptr)c->data)->ctx;
  fmpq_rat_ptr res = (fmpq_rat_ptr) omAlloc(sizeof(fmpq_rat_struct));
  fmpq_rat_init(res, c);
  fmpq_mpoly_set(res->num, x->num, ctx);
  fmpq_mpoly_one(res->den, ctx);
  return (number) res;
}

static number ExtGcd(number a, number b, number *s, number *t, const coeffs c)
{
  WerrorS("not a Euclidean ring: ExtGcd");
  return NULL;
}

static number Lcm(number a, number b, const coeffs c)
{
  WerrorS("not yet: Lcm");
  return NULL;
}

static number Q2Frac(number a, const coeffs src, const coeffs dst)
{
  number res;
  if (SR_HDL(a) & SR_INT)
  {
     res=Init(SR_TO_INT(a),dst);
     n_Test(res,dst);
  }
  else if (a->s==3)
  {
    res=InitMPZ(a->z,dst);
    n_Test(res,dst);
  }
  else
  {
    number z=InitMPZ(a->z,dst);
    number n=InitMPZ(a->n,dst);
    res=Div(z,n,dst);
    Delete(&z,dst);
    Delete(&n,dst);
    n_Test(res,dst);
    return res;
  }
  return res;
}

static number Z2Frac(number a, const coeffs src, const coeffs dst)
{
  return InitMPZ((mpz_ptr)a,dst);
}

static number Zp2Frac(number a, const coeffs src, const coeffs dst)
{
  return Init(n_Int(a,src),dst);
}

static nMapFunc SetMap(const coeffs src, const coeffs dst)
{
  if (src == dst) return ndCopyMap;
  if (nCoeff_is_Q_or_BI(src) && (src->rep==n_rep_gap_rat))  /*Q, coeffs_BIGINT */
    return Q2Frac;
  if(src->rep==n_rep_gap_gmp) /*Z */
    return Z2Frac;
  if(nCoeff_is_Zp(src))
    return Zp2Frac;

  return NULL;
}

//static void InpMult(number &a, number b, const coeffs c)
//{
//}

//static void InpAdd(number &a, number b, const coeffs c)
//{
//}

#if 0
static number Init_bigint(number i, const coeffs dummy, const coeffs dst)
{
  const fmpq_ctx_ptr ctx = (fmpq_ctx_ptr) ((data_ptr)(dst->data))->ctx;
  fmpq_rat_ptr res = (fmpq_rat_ptr) omAlloc(sizeof(fmpq_rat_struct));
  fmpz_t f;
  fmpq_rat_init(res, dst);
  if (SR_HDL(i) & SR_INT)
  {
    fmpq_mpoly_set_si(res->num, SR_TO_INT(i), ctx);
  }
  else
  {
    fmpz_init(f);
    fmpz_set_mpz(f, i->z);
    fmpq_mpoly_set_fmpz(res->num, f, ctx);
    fmpz_clear(f);
  }
  fmpq_mpoly_set_si(res->den, 1, ctx);
  return (number) res;
}
#endif

#if 0
static number Farey(number p, number n, const coeffs c)
{
  WerrorS("not yet: Farey");
  return NULL;
}
#endif

#if 0
static number ChineseRemainder(number *x, number *q, int rl,
                    BOOLEAN sym, CFArray &inv_cache, const coeffs c)
{
  WerrorS("not yet: ChineseRemainder");
  return NULL;
}
#endif

static int ParDeg(number a, const coeffs c)
{
    const fmpq_rat_ptr x = (fmpq_rat_ptr) a;
    const fmpq_ctx_ptr ctx = (fmpq_ctx_ptr) ((data_ptr)c->data)->ctx;
    return (int) (fmpq_mpoly_total_degree_si(x->num, ctx) -
                  fmpq_mpoly_total_degree_si(x->den, ctx));
}

static number Parameter(const int i, const coeffs c)
{
  const fmpq_ctx_ptr ctx = (fmpq_ctx_ptr) ((data_ptr)c->data)->ctx;
  fmpq_rat_ptr res = (fmpq_rat_ptr) omAlloc(sizeof(fmpq_rat_struct));
  fmpq_rat_init(res, c);
  fmpq_mpoly_gen(res->num, (slong) i, ctx);
  fmpq_mpoly_one(res->den, ctx);
  return (number) res;
}

static number SubringGcd(number a, number b, const coeffs c)
{
  fmpq_rat_ptr res = (fmpq_rat_ptr) omAlloc(sizeof(fmpq_rat_struct));
  fmpq_rat_init(res, c);
  const fmpq_rat_ptr x = (fmpq_rat_ptr) a;
  const fmpq_rat_ptr y = (fmpq_rat_ptr) b;
  const fmpq_ctx_ptr ctx = (fmpq_ctx_ptr) ((data_ptr)c->data)->ctx;
  fmpq_mpoly_gcd(res->num, x->num, y->num, ctx);
  // handle content:
  fmpz_t cont;
  fmpz_init(cont);
  fmpz_gcd(cont, fmpq_numref(x->num->content), fmpq_numref(y->num->content));
  if (!fmpz_is_one(cont))
  {
     fmpq_mul_fmpz(res->num->content, res->num->content, cont);
  }
  fmpz_gcd(cont, fmpq_denref(x->num->content), fmpq_denref(y->num->content));
  if (!fmpz_is_one(cont))
  {
     fmpq_div_fmpz(res->num->content, res->num->content, cont);
  }
  fmpz_clear(cont);
  fmpq_mpoly_one(res->den, ctx);
  fmpq_rat_canonicalise(res, c);
  #ifdef QA_DEBUG
  res->p=n_SubringGcd(x->p,y->p, ((data_ptr)c->data)->C);
  #endif
  n_Test((number)res, c);
  return (number) res;
}

static number NormalizeHelper(number a, number b, const coeffs c)
{
  fmpq_rat_ptr res = (fmpq_rat_ptr) omAlloc(sizeof(fmpq_rat_struct));
  fmpq_rat_init(res, c);
  const fmpq_rat_ptr x = (fmpq_rat_ptr) a;
  const fmpq_rat_ptr y = (fmpq_rat_ptr) b;
  const fmpq_ctx_ptr ctx = (fmpq_ctx_ptr) ((data_ptr)c->data)->ctx;
  fmpq_mpoly_t gd;
  fmpq_mpoly_init(gd,ctx);
  fmpq_mpoly_one(gd,ctx); // value for gd, if fmpq_mpoly_gcd fails
  fmpq_mpoly_gcd(gd, x->num, y->den, ctx);
  fmpq_mpoly_mul(res->num,  x->num, y->den, ctx);
  if (!fmpq_mpoly_is_one(gd, ctx))// &&(!fmpq_mpoly_is_zero(gd, ctx)))
    fmpq_mpoly_div(res->num, res->num, gd, ctx);
  fmpq_mpoly_one(res->den, ctx);
  #ifdef QA_DEBUG
  res->p=n_NormalizeHelper(x->p,y->p, ((data_ptr)c->data)->C);
  #endif
  n_Test((number)res, c);
  return (number) res;
}

#if 0
static void WriteFd(number a, const ssiInfo *d, const coeffs c)
{
  // format: len a_len(num den) .. a_0
/*  Currently not implemented
  const fmpq_ctx_ptr ctx = (fmpq_ctx_ptr) ((data_ptr)c->data)->ctx;
  const fmpq_rat_ptr aa = (fmpq_rat_ptr) a;
  int l = fmpq_mpoly_length(aa->num, ctx);
  fprintf(d->f_write, "%d ", l);
  mpq_t m;
  mpq_init(m);
  mpz_t num, den;
  mpz_init(num);
  mpz_init(den);
  for(int i = l; i >= 0; i--)
  {
    fmpq_mpoly_get_coeff_mpq(m, aa->num, i);
    mpq_get_num(num, m);
    mpq_get_den(den, m);
    mpz_out_str(d->f_write, SSI_BASE, num);
    fputc(' ', d->f_write);
    mpz_out_str(d->f_write, SSI_BASE, den);
    fputc(' ', d->f_write);
  }
  mpz_clear(den);
  mpz_clear(num);
  mpq_clear(m);
*/
}
#endif

#if 0
static number ReadFd(const ssiInfo *d, const coeffs c)
{
  // format: len a_len .. a_0
/* Currently not implemented
  fmpq_mpoly_ptr aa = (fmpq_mpoly_ptr) omAlloc(sizeof(fmpq_mpoly_t));
  fmpq_mpoly_init(aa);
  int l = s_readint(d->f_read);
  mpz_t nm;
  mpz_init(nm);
  mpq_t m;
  mpq_init(m);
  for (int i = l; i >= 0; i--)
  {
    s_readmpz_base(d->f_read, nm, SSI_BASE);
    mpq_set_num(m, nm);
    s_readmpz_base(d->f_read, nm, SSI_BASE);
    mpq_set_den(m, nm);
    fmpq_mpoly_set_coeff_mpq(aa, i, m);
  }
  mpz_clear(nm);
  mpq_clear(m);
  return (number)aa;
*/
  return NULL;
}
#endif

// cfClearContent

// cfClearDenominators

#if 0
static number ConvFactoryNSingN(const CanonicalForm n, const coeffs c)
{
  WerrorS("not yet: ConvFactoryNSingN");
  return NULL;
}
#endif

#if 0
static CanonicalForm ConvSingNFactoryN(number n, BOOLEAN setChar, const coeffs c)
{
  WerrorS("not yet: ConvSingNFactoryN");
  return CanonicalForm(0);
}
#endif

char * QratCoeffName(const coeffs c)
{
  STATIC_VAR char CoeffName_flint_Qrat[200];
  sprintf(CoeffName_flint_Qrat, "flintQQ(%s",c->pParameterNames[0]);
  for(int i=1; i<c->iNumberOfParameters;i++)
  {
    strcat(CoeffName_flint_Qrat,",");
    strcat(CoeffName_flint_Qrat,c->pParameterNames[i]);
  }
  strcat(CoeffName_flint_Qrat,")");
  return (char*) CoeffName_flint_Qrat;

}

coeffs flintQratInitCfByName(char *s, n_coeffType n)
{
  const char start[] = "flintQ(";
  const int start_len = strlen(start);
  if (strncmp(s, start, start_len) == 0)
  {
    s += start_len;
    // count ,
    char *p=s;
    int N=0;
    loop
    {
      while((*p!=',')&&(*p!=')')&&(*p!='\0')) p++;
      if (*p==',')       { p++; N++;}
      else if (*p==')')  { p++; N++; break;}
      else if (*p=='\0') { break;}
    }
    // get names
    char *names[N];
    int i=0;
    p=s;
    loop
    {
      while((*p!=',')&&(*p!=')')&&(*p!='\0')) p++;
      if ((*p==',')||(*p=')'))
      {
        char c=*p;
        *p='\0';
        names[i]=omStrDup(s);
        *p=c;
        i++;
        p++;
        s=p;
        if (c==')') break;
      }
      if (*p=='\0') break;
    }
    QaInfo pp;
    pp.N=N;
    pp.names=names;
    coeffs cf=nInitChar(n,&pp);
    for(i=0;i<N;i++) omFree(names[i]);
    return cf;
  }
  return NULL;
}

#ifdef LDEBUG
static BOOLEAN DBTest(number c, const char *, const int, const coeffs cf)
{
  const fmpq_rat_ptr x = (fmpq_rat_ptr) c;
  if ((x!=NULL)
  && ((x->num==NULL)||(x->den==NULL)))
  {
    dReportError("NULL num., or den.\n");
    return FALSE;
  }
  const fmpq_ctx_ptr ctx = (fmpq_ctx_ptr) ((data_ptr)cf->data)->ctx;
  fmpq_mpoly_assert_canonical(x->num,ctx);
  fmpq_mpoly_assert_canonical(x->den,ctx);
  if (fmpq_mpoly_is_zero(x->den, ctx))
  {
    dReportError("den.==0\n");
    return FALSE;
  }
  fmpz_t n, d;
  fmpz_init(n);
  fmpz_init(d);
  fmpz_gcd(n, fmpq_numref(x->num->content), fmpq_numref(x->den->content));
  fmpz_lcm(d, fmpq_denref(x->num->content), fmpq_denref(x->den->content));
  if (!fmpz_is_one(d))
  {
     dReportError("canon needed (1)");
     return TRUE;
  }
  if (!fmpz_is_one(n))
  {
     dReportError("canon needed (2)");
     return TRUE;
  }
  fmpz_clear(n);
  fmpz_clear(d);
  #ifdef QA_DEBUG
  poly pp=convFlintMPSingP(x->num,ctx,((data_ptr)cf->data)->C->extRing);
  fraction f=(fraction)x->p;
  if (f==NULL)
  {
    dReportError("x->p==NULL\n");
    return FALSE;
  }
  else
  {
    if (!p_EqualPolys(pp,NUM(f),((data_ptr)cf->data)->C->extRing))
    {
      p_Write(pp,((data_ptr)cf->data)->C->extRing);
      PrintS("num, p=");
      p_Write(NUM(f),((data_ptr)cf->data)->C->extRing);
      dReportError("num wrong.\n");
      return FALSE;
    }
    if (DEN(f)!=NULL)
    {
      pp=convFlintMPSingP(x->den,ctx,((data_ptr)cf->data)->C->extRing);
      if (!p_EqualPolys(pp,DEN(f),((data_ptr)cf->data)->C->extRing))
      {
        p_Write(pp,((data_ptr)cf->data)->C->extRing);
        PrintS("den, p=");
        p_Write(NUM(f),((data_ptr)cf->data)->C->extRing);
        dReportError("den wrong.\n");
        return FALSE;
      }
    }
  }
  #endif
  return TRUE;
}

#endif
static void KillChar(coeffs cf)
{
  for(int i=0;i<cf->iNumberOfParameters;i++)
    omFree((ADDRESS)(cf->pParameterNames[i]));
  omFreeSize(cf->pParameterNames,sizeof(char*));
  const fmpq_ctx_ptr ctx = (fmpq_ctx_ptr) ((data_ptr)cf->data)->ctx;
  fmpq_mpoly_ctx_clear(ctx);
  omFree(cf->data);
}

BOOLEAN flintQrat_InitChar(coeffs cf, void * infoStruct)
{
  QaInfo *pp=(QaInfo*)infoStruct;
  cf->cfCoeffName    = QratCoeffName;
  cf->nCoeffIsEqual  = CoeffIsEqual;
  cf->cfKillChar     = KillChar;
  cf->ch             = 0; //char 0
  cf->cfMult         = Mult;
  cf->cfSub          = Sub;
  cf->cfAdd          = Add;
  cf->cfDiv          = Div;
  cf->cfExactDiv     = Div; // ???
  cf->cfInit         = Init;
  cf->cfInitMPZ      = InitMPZ;
  cf->cfSize         = Size;
  cf->cfInt          = Int;
  cf->cfMPZ          = MPZ;
  cf->cfInpNeg       = Neg;
  cf->cfInvers       = Invers;
  cf->cfCopy         = Copy;
  cf->cfRePart       = Copy;
  // default: cf->cfImPart       = ndReturn0;
  cf->cfWriteLong    = WriteLong;
  cf->cfWriteShort   = WriteLong;
  cf->cfRead         = Read;
  //cf->cfNormalize    = Normalize;

  //cf->cfDivComp=
  //cf->cfIsUnit=
  //cf->cfGetUnit=
  //cf->cfDivBy=

  cf->cfGreater      = Greater;
  cf->cfEqual        = Equal;
  cf->cfIsZero       = IsZero;
  cf->cfIsOne        = IsOne;
  cf->cfIsMOne       = IsMOne;
  cf->cfGreaterZero  = GreaterZero;

  cf->cfPower        = Power;
  cf->cfGetDenom     = GetDenom;
  cf->cfGetNumerator = GetNumerator;
  cf->cfExtGcd       = ExtGcd;
  cf->cfSubringGcd   = SubringGcd;
  cf->cfNormalizeHelper= NormalizeHelper;
  cf->cfLcm          = Lcm;
  cf->cfDelete       = Delete;
  cf->cfSetMap       = SetMap;
  // default: cf->cfInpMult
  // default: cf->cfInpAdd
  //cf->cfFarey        =Farey;
  //cf->cfChineseRemainder = ChineseRemainder;
  cf->cfParDeg       = ParDeg;
  cf->cfParameter    = Parameter;
  //  cf->cfClearContent = ClearContent;
  //  cf->cfClearDenominators = ClearDenominators;
  //cf->convFactoryNSingN = ConvFactoryNSingN;
  //cf->convSingNFactoryN = ConvSingNFactoryN;
  //cf->cfWriteFd      = WriteFd;
  //cf->cfReadFd       = ReadFd;
#ifdef LDEBUG
  cf->cfDBTest       = DBTest;
#endif

  cf->iNumberOfParameters = pp->N;
  char **pn = (char**) omAlloc0(pp->N*sizeof(char*));
  for(int i=0;i<pp->N;i++)
  {
    pn[i] = omStrDup(pp->names[i]);
  }
  cf->pParameterNames = (const char **) pn;
  cf->has_simple_Inverse = FALSE;
  cf->has_simple_Alloc = FALSE;
  cf->is_field = TRUE;
  cf->is_domain = TRUE;

  fmpq_rat_data_struct *ps=(fmpq_rat_data_struct*)omAlloc(sizeof(fmpq_rat_data_struct));
  ps->ctx=(fmpq_mpoly_ctx_struct*)omAlloc(sizeof(fmpq_mpoly_ctx_struct));
  #ifdef QA_DEBUG
  ps->C=pp->C;
  #endif
  fmpq_mpoly_ctx_init(ps->ctx,pp->N,ORD_LEX);
  cf->data=ps;
  return FALSE;
}
#else
BOOLEAN flintQrat_InitChar(coeffs cf, void * infoStruct)
{ return TRUE; }
#endif
#else
BOOLEAN flintQrat_InitChar(coeffs cf, void * infoStruct)
{ return TRUE; }
#endif
