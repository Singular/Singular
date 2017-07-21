/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/

/*
* ABSTRACT:
*/



#include "misc/auxiliary.h"
#include "misc/mylimits.h"

#include "reporter/reporter.h"
#include "omalloc/omalloc.h"

#include "coeffs/numbers.h"
#include "coeffs/coeffs.h"
#include "coeffs/mpr_complex.h"

#include "coeffs/juliacf.h"
#include "coeffs/longrat.h"

#ifdef HAVE_JULIA
#include <julia.h>

struct jcf_struct
{
  jl_value_t *val;
  long pos;
}
typedef struct jcf_struct * jcf_number;

// julia helper routines:
//
number jcfNew(jl_value_t* v, const coeffs cf)
{
  jcf_number r=(jcf_number)omAlloc(sizeof(jcf_struct));
  r->val=v;
  // ????
  return (number)r;
}

void jcfFree(jcf_number n,  const coeffs cf)
{
  // ???
}

#ifdef LDEBUG
static BOOLEAN jcfDBTest(number a, const coeffs r, const char *f, const int l);
#endif

static void jcfCoeffWrite (const coeffs r, BOOLEAN /*details*/)
{
  PrintS("juliacf");
}


static BOOLEAN jcfGreaterZero (number k, const coeffs r)
{
}

/*2
* create a number from int
*/
static number jcfInit (long i, const coeffs r)
{
  jl_value_t *ii = jl_box_int64(i);
  return jcfNew(ii,r);
}

/*2
* convert a number to int
*/
static long jcfInt(number &n, const coeffs r)
{
  jcf_number aa=(jcf_number)n;
  if (jl_is_int64(aa->val))
  {
    long l=jl_unbox_int64(aa->val);
    return l;
  }
  return 0;
}

static void jcfDelete)(number * a, const coeffs r)
{
  jcf_number aa=(jcf_number)(*a);
  jcfFree(aa,r);
  *a=NULL;
}
static number jcfAdd (number a, number b, const coeffs r)
{
  jcf_number aa=(jcf_number)a;
  jcf_number bb=(jcf_number)b;
  jl_function_t *func = jl_get_function(jl_base_module, "+");
  return jcfNew(jl_call2(func, aa->val, bb->val),r);
}

static number jcfSub (number a, number b, const coeffs r)
{
  jcf_number aa=(jcf_number)a;
  jcf_number bb=(jcf_number)b;
  jl_function_t *func = jl_get_function(jl_base_module, "-");
  return jcfNew(jl_call2(func, aa->val, bb->val),r);
}

static number jcfMult (number a,number b, const coeffs r)
{
  jcf_number aa=(jcf_number)a;
  jcf_number bb=(jcf_number)b;
  jl_function_t *func = jl_get_function(jl_base_module, "-");
  return jcfNew(jl_call2(func, aa->val, bb->val),r);
}

static number jcfDiv (number a,number b, const coeffs r)
{
  jcf_number aa=(jcf_number)a;
  jcf_number bb=(jcf_number)b;
  jl_function_t *func = jl_get_function(jl_base_module, "/");
  return jcfNew(jl_call2(func, aa->val, bb->val),r);
}

static BOOLEAN jcfIsZero (number  a, const coeffs r)
{
  jcf_number aa=(jcf_number)a;
  jl_value_t *ii = jl_box_int64(0);
  jl_function_t *func = jl_get_function(jl_base_module, "==");
  jl_value_t *res=jl_call2(func, aa->val, ii);
  if (jl_unbox_bool(res)) return TRUE;
  else                    return FALSE;
}

static BOOLEAN jcfIsOne (number a, const coeffs r)
{
  jcf_number aa=(jcf_number)a;
  jl_value_t *ii = jl_box_int64(1);
  jl_function_t *func = jl_get_function(jl_base_module, "==");
  jl_value_t *res=jl_call2(func, aa->val, ii);
  if (jl_unbox_bool(res)) return TRUE;
  else                    return FALSE;
}

static BOOLEAN jcfIsMOne (number a, const coeffs r)
{
  jcf_number aa=(jcf_number)a;
  jl_value_t *ii = jl_box_int64(-1);
  jl_function_t *func = jl_get_function(jl_base_module, "==");
  jl_value_t *res=jl_call2(func, aa->val, ii);
  if (jl_unbox_bool(res)) return TRUE;
  else                    return FALSE;
}

static number jcfInvers (number c, const coeffs r)
{
  jl_value_t *ii = jl_box_int64(1);
  jcf_number cc=(jcf_number)c;
  jl_function_t *func = jl_get_function(jl_base_module, "/");
  return jcfNew(jl_call2(func, ii, cc->val),r);
}

static number jcfNeg (number c, const coeffs r)
{
  // inplace negate:
  jl_value_t *ii = jl_box_int64(-1);
  jcf_number cc=(jcf_number)c;
  jl_function_t *func = jl_get_function(jl_base_module, "*");
  cc->val=jl_call2(func, ii, cc->val);
  return (number)cc;
}

static BOOLEAN jcfGreater (number a,number b, const coeffs r)
{
  jcf_number aa=(jcf_number)a;
  jcf_number bb=(jcf_number)b;
  jl_function_t *func = jl_get_function(jl_base_module, ">");
  jl_value_t *res=jl_call2(func, aa->val, bb->val);
  if (jl_unbox_bool(res)) return TRUE;
  else                    return FALSE;
}

static BOOLEAN jcfEqual (number a,number b, const coeffs r)
{
  jcf_number aa=(jcf_number)a;
  jcf_number bb=(jcf_number)b;
  jl_function_t *func = jl_get_function(jl_base_module, "==");
  jl_value_t *res=jl_call2(func, aa->val, bb->val);
  if (jl_unbox_bool(res)) return TRUE;
  else                    return FALSE;
}

static void jcfWrite (number a, const coeffs r)
{
  jcf_number aa=(jcf_number)a;
  jl_function_t *func = jl_get_function(jl_base_module, "string");
  jl_value_t *res=jl_call1(func, aa->val);
  // ???
  // char *s=jl_unbox_charptr(res);
  // StringAppendS(s);
}

#if 0
static void jcfPower (number a, int i, number * result, const coeffs r)
{
}
#endif

static const char * jcfRead (const char *s, number *a, const coeffs r)
{
  int i=1;
  s=eati(s,&i);
  *a=jcfInit (i,r);
  return s;
}

static void jcfKillChar(coeffs r)
{
}

static number jcfCopy(number a, const coeffs cf)
{
  jcf_number aa=(jcf_number)a;
  return jcfNew(aa->val,r);
}

static BOOLEAN jcfCoeffIsEqual(const coeffs r, n_coeffType n, void * parameter)
{
}

#ifdef LDEBUG
/*2
* test valid numbers: not implemented yet
*/
#pragma GCC diagnostic ignored "-Wunused-parameter"
static BOOLEAN  jcfDBTest(number a, const char *f, const int l, const coeffs r)
{
  return TRUE;
}
#endif

static nMapFunc jcfSetMap(const coeffs src, const coeffs dst)
{
  return NULL;
}

static char* jcfCoeffString(const coeffs r)
{
  return omStrDup("juliacf");
}

static char* jcfCoeffName(const coeffs r)
{
  return (char*)"Float()";
}

BOOLEAN jcfInitChar(coeffs n, void* p)
{
  n->is_field=TRUE;
  n->is_domain=TRUE;
  n->rep=n_rep_float;

  n->cfKillChar = jcfKillChar;
  n->ch = 0;
  n->cfCoeffString = jcfCoeffString;
  n->cfCoeffName = jcfCoeffName;

  n->cfInit = jcfInit;
  n->cfInt  = jcfInt;
  n->cfDelete=jcfDelete;
  n->cfAdd   = jcfAdd;
  n->cfSub   = jcfSub;
  n->cfMult  = jcfMult;
  n->cfDiv   = jcfDiv;
  n->cfExactDiv= jcfDiv;
  n->cfInpNeg   = jcfNeg;
  n->cfInvers= jcfInvers;
  n->cfCopy  = jcfCopy;
  n->cfGreater = jcfGreater;
  n->cfEqual = jcfEqual;
  n->cfIsZero = jcfIsZero;
  n->cfIsOne = jcfIsOne;
  n->cfIsMOne = jcfIsMOne;
  n->cfGreaterZero = jcfGreaterZero;
  n->cfWriteLong = jcfWrite;
  n->cfRead = jcfRead;
  //n->cfPower = jcfPower;
  n->cfSetMap = jcfSetMap;
  n->cfCoeffWrite  = jcfCoeffWrite;

    /*nSize  = ndSize;*/
#ifdef LDEBUG
  //n->cfDBTest=ndDBTest; // not yet implemented: jcfDBTest;
#endif

  n->nCoeffIsEqual = jcfCoeffIsEqual;
  return FALSE;
}
#endif
