/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/*
* ABSTRACT: numbers (integers)
*/
#ifdef HAVE_RINGS
#if SI_INTEGER_VARIANT == 3

//make sure that a small number is an immediate integer
//bascially coped from longrat.cc nlShort3
//TODO: is there any point in checking 0 first???
//TODO: it is not clear that this works in 32/64 bit everywhere.
//      too many hacks.
#ifdef LDEBUG
#define nrzTest(A) nrzDBTest(A,__FILE__,__LINE__,NULL)
BOOLEAN nrzDBTest (number x, const char *f, const int l, const coeffs);
#else
#define nrzTest(A)
#endif

#undef CF_DEBUG
static inline number nrz_short(number x)
{
#if CF_DEBUG
  StringAppendS("short(");
  nrzWrite(x, NULL);
#endif
  if (mpz_sgn1((mpz_ptr) x)==0)
  {
    mpz_clear((mpz_ptr)x);
    omFreeBin(x, gmp_nrz_bin);
#if CF_DEBUG
    StringAppendS(")=0");
#endif
    return INT_TO_SR(0);
  }
  if (mpz_size1((mpz_ptr)x)<=MP_SMALL)
  {
    long ui=mpz_get_si((mpz_ptr)x);
    if ((((ui<<3)>>3)==ui)
    && (mpz_cmp_si((mpz_ptr)x,ui)==0))
    {
      mpz_clear((mpz_ptr)x);
      omFreeBin(x, gmp_nrz_bin);
#if CF_DEBUG
    StringAppendS(")=imm");
#endif
      return INT_TO_SR(ui);
    }
  }
#if CF_DEBUG
  StringAppendS(")");
#endif
  return x;
}


static int nrzSize(number a, const coeffs)
{
  if (a==INT_TO_SR(0)) return 0;
  if (n_Z_IS_SMALL(a)) return 1;
  return ((mpz_ptr)a)->_mp_alloc;
}


/*
 * Multiply two numbers
 * check for 0, 1, -1 maybe
 */
#if CF_DEBUG
number _nrzMult(number, number, const coeffs);
number nrzMult(number a, number b, const coeffs R)
{
  StringSetS("Mult: ");
  nrzWrite(a, R);
  StringAppendS(" by ");
  nrzWrite(b, R);
  number c = _nrzMult(a, b, R);
  StringAppendS(" is ");
  nrzWrite(c, R);
  char * s = StringEndS();
  Print("%s\n", s);
  omFree(s);
  return c;
}
number _nrzMult (number a, number b, const coeffs R)
#else
number nrzMult (number a, number b, const coeffs R)
#endif
{
  if (n_Z_IS_SMALL(a) && n_Z_IS_SMALL(b))
  {
  //from longrat.cc
    if (SR_TO_INT(a)==0)
      return a;
    if (SR_TO_INT(b)==0)
      return b;
    long r=(long)((unsigned long)(SR_HDL(a)-1L))*((unsigned long)(SR_HDL(b)>>1));
    if ((r/(SR_HDL(b)>>1))==(SR_HDL(a)-1L))
    {
      number u=((number) ((r>>1)+SR_INT));
    //  if (((((long)SR_HDL(u))<<1)>>1)==SR_HDL(u)) return (u);
      return nrzInit(SR_HDL(u)>>2, R);
    }
    mpz_ptr erg = (mpz_ptr) omAllocBin(gmp_nrz_bin);
    mpz_init(erg);
    mpz_set_si(erg, SR_TO_INT(a));
    mpz_mul_si(erg, erg, SR_TO_INT(b));
    nrzTest((number)erg);
    return (number) erg;
  }
  else if (n_Z_IS_SMALL(a))
  {
    if (SR_TO_INT(a)==0)
      return a;
    mpz_ptr erg = (mpz_ptr) omAllocBin(gmp_nrz_bin);
    mpz_init_set(erg, (mpz_ptr) b);
    mpz_mul_si(erg, erg, SR_TO_INT(a));
    nrzTest((number)erg);
    return (number) erg;
  }
  else if (n_Z_IS_SMALL(b))
  {
    if (SR_TO_INT(b)==0)
      return b;
    mpz_ptr erg = (mpz_ptr) omAllocBin(gmp_nrz_bin);
    mpz_init_set(erg, (mpz_ptr) a);
    mpz_mul_si(erg, erg, SR_TO_INT(b));
    nrzTest((number)erg);
    return (number) erg;
  }
  else
  {
    mpz_ptr erg = (mpz_ptr) omAllocBin(gmp_nrz_bin);
    mpz_init(erg);
    mpz_mul(erg, (mpz_ptr) a, (mpz_ptr) b);
    nrzTest((number)erg);
    return (number) erg;
  }
}


static long int_gcd(long a, long b)
{
  long r;
  a = ABS(a);
  b = ABS(b);
  if (!a) return b;
  if (!b) return a;
  do
  {
    r = a % b;
    a = b;
    b = r;
  } while (b);
  return ABS(a); // % in c doeas not imply a signn
                 // it would be unlikely to see a negative here
                 // but who knows
}

/*
 * Give the smallest non unit k, such that a * x = k = b * y has a solution
 */
static number nrzLcm (number a, number b, const coeffs R)
{
  #ifdef CF_DEBUG
  PrintS("nrzLcm\n");
  #endif
  mpz_ptr erg;
  if (n_Z_IS_SMALL(a) && n_Z_IS_SMALL(b))
  {
    long g = int_gcd(SR_TO_INT(a), SR_TO_INT(b));
    return nrzMult(a, INT_TO_SR(SR_TO_INT(b)/g), R);
  }
  else
  {
    erg = (mpz_ptr) omAllocBin(gmp_nrz_bin);
    if (n_Z_IS_SMALL(a))
    {
      mpz_init_set(erg, (mpz_ptr) b);
      unsigned long g = mpz_gcd_ui(NULL, erg, (unsigned long) ABS(SR_TO_INT(a)));
      mpz_mul_si(erg, erg, SR_TO_INT(a)/g);
    }
    else if (n_Z_IS_SMALL(b))
    {
      mpz_init_set(erg, (mpz_ptr) a);
      unsigned long g = mpz_gcd_ui(NULL, erg, (unsigned long) ABS(SR_TO_INT(b)));
      mpz_mul_si(erg, erg, SR_TO_INT(b)/g);
    }
    else
    {
      mpz_init(erg);
      mpz_lcm(erg, (mpz_ptr) a, (mpz_ptr) b);
    }
  }
  return (number) erg;
}

static number nrzCopy(number a, const coeffs)
{
  if (n_Z_IS_SMALL(a)) return a;
  mpz_ptr erg = (mpz_ptr) omAllocBin(gmp_nrz_bin);
  mpz_init_set(erg, (mpz_ptr) a);
  return (number) erg;
}

/*
 * Give the largest non unit k, such that a = x * k, b = y * k has
 * a solution.
 */
static number nrzGcd (number a,number b,const coeffs R)
{
  if (n_Z_IS_SMALL(a) && n_Z_IS_SMALL(b))
  {
    long g = int_gcd(SR_TO_INT(a), SR_TO_INT(b));
    return INT_TO_SR(g);
  }
  else if (n_Z_IS_SMALL(a))
  {
    if (a==INT_TO_SR(0))
      return nrzCopy(b, R);
    unsigned long g = mpz_gcd_ui(NULL, (mpz_ptr)b, (unsigned long) ABS(SR_TO_INT(a)));
    return INT_TO_SR( g);
  }
  else if (n_Z_IS_SMALL(b))
  {
    if (b==INT_TO_SR(0))
      return nrzCopy(a, R);
    unsigned long g = mpz_gcd_ui(NULL, (mpz_ptr)a, (unsigned long) ABS(SR_TO_INT(b)));
    return INT_TO_SR(g);
  }
  else
  {
    mpz_ptr erg = (mpz_ptr) omAllocBin(gmp_nrz_bin);
    mpz_init(erg);
    mpz_gcd(erg, (mpz_ptr) a, (mpz_ptr) b);
    return (number) erg;
  }
}

/*
 * Give the largest non unit k, such that a = x * k, b = y * k has
 * a solution and r, s, s.t. k = s*a + t*b
 */
static long int_extgcd(long a, long b, long * u, long* x, long * v, long* y)
{
  long q, r;
  if (!a)
  {
    *u = 0;
    *v = 1;
    *x = -1;
    *y = 0;
    return b;
  }
  if (!b)
  {
    *u = 1;
    *v = 0;
    *x = 0;
    *y = 1;
    return a;
  }
  *u=1;
  *v=0;
  *x=0;
  *y=1;
  do
  {
    q = a/b;
    r = a%b;
    assume (q*b+r == a);
    a = b;
    b = r;

    r = -(*v)*q+(*u);
    (*u) =(*v);
    (*v) = r;

    r = -(*y)*q+(*x);
    (*x) = (*y);
    (*y) = r;
  } while (b);

  return a;
}

static number  nrzExtGcd (number a, number b, number *s, number *t, const coeffs)
{
  if (n_Z_IS_SMALL(a) && n_Z_IS_SMALL(b))
  {
    long u, v, x, y;
    long g = int_extgcd(SR_TO_INT(a), SR_TO_INT(b), &u, &v, &x, &y);
    *s = INT_TO_SR(u);
    *t = INT_TO_SR(v);
    return INT_TO_SR(g);
  }
  else
  {
    mpz_t aa, bb;
    if (n_Z_IS_SMALL(a))
    {
      mpz_init_set_si(aa, SR_TO_INT(a));
    }
    else
    {
      mpz_init_set(aa, (mpz_ptr) a);
    }
    if (n_Z_IS_SMALL(b))
    {
      mpz_init_set_si(bb, SR_TO_INT(b));
    }
    else
    {
      mpz_init_set(bb, (mpz_ptr) b);
    }
    mpz_ptr erg = (mpz_ptr) omAllocBin(gmp_nrz_bin);
    mpz_ptr bs = (mpz_ptr) omAllocBin(gmp_nrz_bin);
    mpz_ptr bt = (mpz_ptr) omAllocBin(gmp_nrz_bin);
    mpz_init(erg);
    mpz_init(bs);
    mpz_init(bt);
    mpz_gcdext(erg, bs, bt, aa, bb);
    *s = nrz_short((number) bs);
    *t = nrz_short((number) bt);
    mpz_clear(aa);
    mpz_clear(bb);
    return nrz_short((number) erg);
  }
}
#if CF_DEBUG
static number _nrzXExtGcd(number, number, number *, number *, number *, number *, const coeffs);
static number nrzXExtGcd(number a, number b, number *x, number * y, number * u, number * v, const coeffs R)
{
  char * s;
  StringSetS("XExtGcd: ");
  nrzWrite(a, R);
  StringAppendS(" by ");
  nrzWrite(b, R);
  number c = _nrzXExtGcd(a, b, x, y, u, v, R);
  StringAppendS(" is ");
  nrzWrite(c, R);
  StringAppendS("[[");
  nrzWrite(*x, R);
  StringAppendS(", ");
  nrzWrite(*y, R);
  StringAppendS("], ");
  nrzWrite(*u, R);
  StringAppendS(", ");
  nrzWrite(*v, R);
  s=StringEndS();
  Print("%s]]\n", s);
  omFree(s);
  return c;
}
static number  _nrzXExtGcd (number a, number b, number *s, number *t, number *u, number *v, const coeffs )
#else
static number  nrzXExtGcd (number a, number b, number *s, number *t, number *u, number *v, const coeffs )
#endif
{
  if (n_Z_IS_SMALL(a) && n_Z_IS_SMALL(b))
  {
    long uu, vv, x, y;
    long g = int_extgcd(SR_TO_INT(a), SR_TO_INT(b), &uu, &vv, &x, &y);
    *s = INT_TO_SR(uu);
    *t = INT_TO_SR(vv);
    *u = INT_TO_SR(x);
    *v = INT_TO_SR(y);
    return INT_TO_SR(g);
  }
  else
  {
    mpz_t aa, bb;
    if (n_Z_IS_SMALL(a))
    {
      mpz_init_set_si(aa, SR_TO_INT(a));
    }
    else
    {
      mpz_init_set(aa, (mpz_ptr) a);
    }
    if (n_Z_IS_SMALL(b))
    {
      mpz_init_set_si(bb, SR_TO_INT(b));
    }
    else
    {
      mpz_init_set(bb, (mpz_ptr) b);
    }
    mpz_ptr erg = (mpz_ptr) omAllocBin(gmp_nrz_bin);
    mpz_ptr bs = (mpz_ptr) omAllocBin(gmp_nrz_bin);
    mpz_ptr bt = (mpz_ptr) omAllocBin(gmp_nrz_bin);
    mpz_init(erg);
    mpz_init(bs);
    mpz_init(bt);

    mpz_gcdext(erg, bs, bt, aa, bb);

    mpz_ptr bu = (mpz_ptr) omAllocBin(gmp_nrz_bin);
    mpz_ptr bv = (mpz_ptr) omAllocBin(gmp_nrz_bin);

    mpz_init_set(bu, (mpz_ptr) bb);
    mpz_init_set(bv, (mpz_ptr) aa);

    mpz_clear(aa);
    mpz_clear(bb);
    assume(mpz_cmp_si(erg, 0));

    mpz_div(bu, bu, erg);
    mpz_div(bv, bv, erg);

    mpz_mul_si(bu, bu, -1);
    *u = nrz_short((number) bu);
    *v = nrz_short((number) bv);

    *s = nrz_short((number) bs);
    *t = nrz_short((number) bt);
    return nrz_short((number) erg);
  }
}
#if CF_DEBUG
static number _nrzQuotRem(number, number, number *, const coeffs);
static number nrzQuotRem(number a, number b, number * r, const coeffs R)
{
  StringSetS("QuotRem: ");
  nrzWrite(a, R);
  StringAppendS(" by ");
  nrzWrite(b, R);
  number c = _nrzQuotRem(a, b, r, R);
  StringAppendS(" is ");
  nrzWrite(c, R);
  if (r) {
    StringAppendS("+R(");
    nrzWrite(*r, R);
    StringAppendS(")");
  }
  char * s = StringEndS();
  Print("%s\n", s);
  omFree(s);
  return c;
}
static number _nrzQuotRem (number a, number b, number * r, const coeffs )
#else
static number nrzQuotRem (number a, number b, number * r, const coeffs )
#endif
{
  assume(SR_TO_INT(b));
  if (n_Z_IS_SMALL(a) && n_Z_IS_SMALL(b))
  {
    if (r)
      *r = INT_TO_SR(SR_TO_INT(a) % SR_TO_INT(b));
    return INT_TO_SR(SR_TO_INT(a)/SR_TO_INT(b));
  }
  else if (n_Z_IS_SMALL(a))
  {
    //a is small, b is not, so q=0, r=a
    if (r)
      *r = a;
    return INT_TO_SR(0);
  }
  else if (n_Z_IS_SMALL(b))
  {
    unsigned long rr;
    mpz_ptr qq = (mpz_ptr) omAllocBin(gmp_nrz_bin);
    mpz_init(qq);
    mpz_t rrr;
    mpz_init(rrr);
    rr = mpz_divmod_ui(qq, rrr, (mpz_ptr) a, (unsigned long)ABS(SR_TO_INT(b)));
    mpz_clear(rrr);

    if (r)
      *r = INT_TO_SR(rr);
    if (SR_TO_INT(b)<0)
    {
      mpz_mul_si(qq, qq, -1);
    }
    return nrz_short((number)qq);
  }
  mpz_ptr qq = (mpz_ptr) omAllocBin(gmp_nrz_bin),
             rr = (mpz_ptr) omAllocBin(gmp_nrz_bin);
  mpz_init(qq);
  mpz_init(rr);
  mpz_divmod(qq, rr, (mpz_ptr)a, (mpz_ptr)b);
  if (r)
    *r = (number) rr;
  else
  {
    mpz_clear(rr);
  }
  nrzTest((number)qq);
  return (number) qq;
}

static void nrzPower (number a, int i, number * result, const coeffs)
{
  mpz_ptr erg = (mpz_ptr) omAllocBin(gmp_nrz_bin);
  mpz_init(erg);
  mpz_t aa;
  if (n_Z_IS_SMALL(a))
    mpz_init_set_si(aa, SR_TO_INT(a));
  else
    mpz_init_set(aa, (mpz_ptr) a);
  mpz_pow_ui(erg, aa, i);
  *result = nrz_short((number) erg);
}

/*
 * create a number from int
 * TODO: do not create an mpz if not necessary
 */
number nrzInit (long i, const coeffs)
{
  mpz_ptr erg = (mpz_ptr) omAllocBin(gmp_nrz_bin);
  mpz_init_set_si(erg, i);
  return nrz_short((number) erg);
}

static number nrzInitMPZ(mpz_t m, const coeffs)
{
  mpz_ptr erg = (mpz_ptr) omAllocBin(gmp_nrz_bin);
  mpz_init_set(erg, m);
  return nrz_short((number) erg);
}


static void nrzDelete(number *a, const coeffs)
{
  if (*a == NULL) return;
  if (n_Z_IS_SMALL(*a)==0)
  {
    mpz_clear((mpz_ptr) *a);
    omFreeBin((ADDRESS) *a, gmp_nrz_bin);
  }
  *a = NULL;
}

/*
 * convert a number to int
 */
static long nrzInt(number &n, const coeffs)
{
  if (n_Z_IS_SMALL(n)) return SR_TO_INT(n);
  return mpz_get_si( (mpz_ptr)n);
}
#if CF_DEBUG
static number _nrzAdd(number, number, const coeffs);
static number nrzAdd(number a, number b, const coeffs R)
{
  StringSetS("Add: ");
  nrzWrite(a, R);
  StringAppendS(" to ");
  nrzWrite(b, R);
  number c = _nrzAdd(a, b, R);
  StringAppendS(" is ");
  nrzWrite(c, R);
  char * s = StringEndS();
  Print("%s\n", s);
  omFree(s);
  return c;
}
static number _nrzAdd (number a, number b, const coeffs )
#else
static number nrzAdd (number a, number b, const coeffs )
#endif
{
  if (n_Z_IS_SMALL(a) && n_Z_IS_SMALL(b))
  {
    long c = SR_TO_INT(a) + SR_TO_INT(b);
    if (INT_IS_SMALL(c))
      return INT_TO_SR(c);
    mpz_ptr erg = (mpz_ptr) omAllocBin(gmp_nrz_bin);
    mpz_init_set_si(erg, c);

    nrzTest((number)erg);
    return (number) erg;
  }
  else if (n_Z_IS_SMALL(a))
  {
    mpz_ptr erg = (mpz_ptr) omAllocBin(gmp_nrz_bin);
    mpz_init(erg);
    if (SR_TO_INT(a)>0)
      mpz_add_ui(erg, (mpz_ptr) b, (unsigned long)SR_TO_INT(a));
    else
      mpz_sub_ui(erg, (mpz_ptr) b, (unsigned long)-(SR_TO_INT(a)));
    return nrz_short((number) erg);
  }
  else if (n_Z_IS_SMALL(b))
  {
    mpz_ptr erg = (mpz_ptr) omAllocBin(gmp_nrz_bin);
    mpz_init(erg);
    if (SR_TO_INT(b)>0)
      mpz_add_ui(erg, (mpz_ptr) a, (unsigned long)SR_TO_INT(b));
    else
      mpz_sub_ui(erg, (mpz_ptr) a, (unsigned long)-(SR_TO_INT(b)));
    return nrz_short((number) erg);
  }
  else
  {
    mpz_ptr erg = (mpz_ptr) omAllocBin(gmp_nrz_bin);
    mpz_init(erg);
    mpz_add(erg, (mpz_ptr) a, (mpz_ptr) b);
    return nrz_short((number) erg);
  }
}

static number nrzSub (number a, number b,  const coeffs )
{
  if (n_Z_IS_SMALL(a) && n_Z_IS_SMALL(b))
  {
    long c = SR_TO_INT(a) - SR_TO_INT(b);
    if (INT_IS_SMALL(c))
      return INT_TO_SR(c);
    mpz_ptr erg = (mpz_ptr) omAllocBin(gmp_nrz_bin);
    mpz_init_set_si(erg, c);
    nrzTest((number)erg);
    return (number) erg;
  }
  else if (n_Z_IS_SMALL(a))
  {
    mpz_ptr erg = (mpz_ptr) omAllocBin(gmp_nrz_bin);
    mpz_init(erg);

    if (SR_TO_INT(a)>0)
      mpz_ui_sub(erg, (unsigned long)SR_TO_INT(a), (mpz_ptr) b);
    else
    {
      mpz_add_ui(erg, (mpz_ptr) b, (unsigned long)-SR_TO_INT(a));
      mpz_neg(erg, erg);
    }
    return nrz_short((number) erg);
  }
  else if (n_Z_IS_SMALL(b))
  {
    mpz_ptr erg = (mpz_ptr) omAllocBin(gmp_nrz_bin);
    mpz_init(erg);
    if (SR_TO_INT(b)>0)
      mpz_sub_ui(erg, (mpz_ptr) a, (unsigned long)SR_TO_INT(b));
    else
      mpz_add_ui(erg, (mpz_ptr) a, (unsigned long)-SR_TO_INT(b));
    return nrz_short((number) erg);
  }
  else
  {
    mpz_ptr erg = (mpz_ptr) omAllocBin(gmp_nrz_bin);
    mpz_init(erg);
    mpz_sub(erg, (mpz_ptr) a, (mpz_ptr) b);
    return nrz_short((number) erg);
  }
}

static BOOLEAN nrzGreater (number a,number b, const coeffs)
{
  if (n_Z_IS_SMALL(a) && n_Z_IS_SMALL(b))
    return ((long)a)>((long)b);
  else if (n_Z_IS_SMALL(a))
    return 0 > mpz_cmp_si((mpz_ptr)b,SR_TO_INT(a));
  else if (n_Z_IS_SMALL(b))
    return 0 < mpz_cmp_si((mpz_ptr)a,SR_TO_INT(b));
  return 0 < mpz_cmp((mpz_ptr) a, (mpz_ptr) b);
}

static BOOLEAN nrzGreaterZero (number k, const coeffs C)
{
  return nrzGreater(k, INT_TO_SR(0), C);
}

static number  nrzGetUnit (number n, const coeffs r)
{
  if (nrzGreaterZero(n, r))
    return INT_TO_SR(1);
  /*else*/
    return INT_TO_SR(-1);
}

static number nrzAnn(number n, const coeffs)
{
  if (SR_TO_INT(n))  // in Z: the annihilator of !=0 is 0
    return INT_TO_SR(0);
  else
    return INT_TO_SR(1);
}

static BOOLEAN nrzIsUnit (number a, const coeffs)
{
  return ABS(SR_TO_INT(a))==1;
}

static BOOLEAN nrzIsZero (number  a, const coeffs)
{
  return (a==INT_TO_SR(0));
}

static BOOLEAN nrzIsOne (number a, const coeffs)
{
  return a==INT_TO_SR(1);
}

static BOOLEAN nrzIsMOne (number a, const coeffs)
{
  return a==INT_TO_SR(-1);
}

static BOOLEAN nrzEqual (number a,number b, const coeffs)
{
  if (n_Z_IS_SMALL(a) && n_Z_IS_SMALL(b))
    return a==b;
  else if (n_Z_IS_SMALL(a) || n_Z_IS_SMALL(b))
    return FALSE;
  else
    return 0 == mpz_cmp((mpz_ptr) a, (mpz_ptr) b);
}

static BOOLEAN nrzDivBy (number a,number b, const coeffs)
{
  if (n_Z_IS_SMALL(a) && n_Z_IS_SMALL(b))
  {
    return SR_TO_INT(a) %SR_TO_INT(b) ==0;
  }
  else if (n_Z_IS_SMALL(a))
  {
    return a==INT_TO_SR(0);
  }
  else if (n_Z_IS_SMALL(b))
  {
    return mpz_divisible_ui_p((mpz_ptr)a, (unsigned long)ABS(SR_TO_INT(b))) != 0;
  }
  else
    return mpz_divisible_p((mpz_ptr) a, (mpz_ptr) b) != 0;
}

static int nrzDivComp(number a, number b, const coeffs r)
{
  if (nrzDivBy(a, b, r))
  {
    if (nrzDivBy(b, a, r)) return 2;
    return -1;
  }
  if (nrzDivBy(b, a, r)) return 1;
  return 0;
}

static number nrzDiv (number a,number b, const coeffs)
{
  assume(SR_TO_INT(b));
  if (n_Z_IS_SMALL(a) && n_Z_IS_SMALL(b))
  {
    //if (SR_TO_INT(a) % SR_TO_INT(b))
    //{
    //  WerrorS("1:Division by non divisible element.");
    //  WerrorS("Result is without remainder.");
    //}
    return INT_TO_SR(SR_TO_INT(a)/SR_TO_INT(b));
  }
  else if (n_Z_IS_SMALL(a))
  {
    //if (SR_TO_INT(a))
    //{
    //  WerrorS("2:Division by non divisible element.");
    //  WerrorS("Result is without remainder.");
    //}
    return INT_TO_SR(0);
  }
  else if (n_Z_IS_SMALL(b))
  {
    mpz_ptr erg = (mpz_ptr) omAllocBin(gmp_nrz_bin);
    mpz_t r;
    mpz_init(r);
    mpz_init(erg);
    if (mpz_divmod_ui(erg, r, (mpz_ptr) a, (unsigned long)ABS(SR_TO_INT(b)))) {
    //  WerrorS("3:Division by non divisible element.");
    //  WerrorS("Result is without remainder.");
    }
    mpz_clear(r);
    if (SR_TO_INT(b)<0)
      mpz_neg(erg, erg);
    return nrz_short((number) erg);
  }
  mpz_ptr erg = (mpz_ptr) omAllocBin(gmp_nrz_bin);
  mpz_init(erg);
  mpz_t r;
  mpz_init(r);
  mpz_tdiv_qr(erg, r, (mpz_ptr) a, (mpz_ptr) b);
#if CF_DEBUG
  StringSetS("division of");
  nrzWrite(a, R);
  StringAppendS(" by ");
  nrzWrite(b, R);
  StringAppendS(" is ");
  number du;
  nrzWrite(du = (number)erg, R);
  StringAppendS(" rest ");
  nrzWrite(du = (number)r, R);
  char * s = StringEndS();
  Print("%s\n", s);
  omFree(s);
#endif

  if (mpz_sgn1(r)!=0)
  {
    //WerrorS("4:Division by non divisible element.");
    //WerrorS("Result is without remainder.");
  }
  mpz_clear(r);
  return nrz_short((number) erg);
}

static number nrzExactDiv (number a,number b, const coeffs)
{
  assume(SR_TO_INT(b));
  mpz_t aa, bb;
  if (n_Z_IS_SMALL(a))
    mpz_init_set_si(aa, SR_TO_INT(a));
  else
    mpz_init_set(aa, (mpz_ptr) a);
  if (n_Z_IS_SMALL(b))
    mpz_init_set_si(bb, SR_TO_INT(b));
  else
    mpz_init_set(bb, (mpz_ptr) b);
  mpz_ptr erg = (mpz_ptr) omAllocBin(gmp_nrz_bin);
  mpz_init(erg);
  mpz_tdiv_q(erg, (mpz_ptr) aa, (mpz_ptr) bb);
  mpz_clear(aa);
  mpz_clear(bb);
  nrzTest((number)erg);
  return (number) erg;
}

static number nrzIntMod (number a,number b, const coeffs)
{
  mpz_t aa, bb;
  assume(SR_TO_INT(b));
  if (n_Z_IS_SMALL(a))
    mpz_init_set_si(aa, SR_TO_INT(a));
  else
    mpz_init_set(aa, (mpz_ptr) a);
  if (n_Z_IS_SMALL(b))
    mpz_init_set_si(bb, SR_TO_INT(b));
  else
    mpz_init_set(bb, (mpz_ptr) b);

  mpz_t erg;
  mpz_init(erg);
  mpz_ptr r = (mpz_ptr) omAllocBin(gmp_nrz_bin);
  mpz_init(r);
  mpz_tdiv_qr(erg, r, (mpz_ptr) aa, (mpz_ptr) bb);
  mpz_clear(erg);
  mpz_clear(aa);
  mpz_clear(bb);

  return nrz_short((number) r);
}

static number  nrzInvers (number c, const coeffs r)
{
  if (!nrzIsUnit((number) c, r))
  {
    WerrorS("Non invertible element.");
    return (number)NULL;
  }
  return c; // has to be 1 or -1....
}

static number nrzNeg (number c, const coeffs)
{
// nNeg inplace !!!
  if (n_Z_IS_SMALL(c))
    return INT_TO_SR(-SR_TO_INT(c));
  mpz_mul_si((mpz_ptr) c, (mpz_ptr) c, -1);
  return c;
}

static number nrzFarey(number r, number N, const coeffs R)
{
  number a0 = nrzCopy(N, R);
  number b0 = nrzInit(0, R);
  number a1 = nrzCopy(r, R);
  number b1 = nrzInit(1, R);
  number two = nrzInit(2, R);
#if 0
  PrintS("Farey start with ");
  n_Print(r, R);
  PrintS(" mod ");
  n_Print(N, R);
  PrintLn();
#endif
  while (1)
  {
    number as = nrzMult(a1, a1, R);
    n_InpMult(as, two, R);
    if (nrzGreater(N, as, R))
    {
      nrzDelete(&as, R);
      break;
    }
    nrzDelete(&as, R);
    number q = nrzDiv(a0, a1, R);
    number t = nrzMult(a1, q, R),
           s = nrzSub(a0, t, R);
    nrzDelete(&a0, R);
    a0 = a1;
    a1 = s;
    nrzDelete(&t, R);

    t = nrzMult(b1, q, R);
    s = nrzSub(b0, t, R);
    nrzDelete(&b0, R);
    b0 = b1;
    b1 = s;
    nrzDelete(&t, R);
    nrzDelete(&q, R);
  }
  number as = nrzMult(b1, b1, R);
  n_InpMult(as, two, R);
  nrzDelete(&two, R);
  if (nrzGreater(as, N, R))
  {
    nrzDelete(&a0, R);
    nrzDelete(&a1, R);
    nrzDelete(&b0, R);
    nrzDelete(&b1, R);
    nrzDelete(&as, R);
    return NULL;
  }
  nrzDelete(&as, R);
  nrzDelete(&a0, R);
  nrzDelete(&b0, R);

  number a, b, ab;
  coeffs Q = nInitChar(n_Q, 0);
  nMapFunc f = n_SetMap(R, Q);
  a = f(a1, R, Q);
  b = f(b1, R, Q);
  ab = n_Div(a, b, Q);
  n_Delete(&a, Q);
  n_Delete(&b, Q);
  nKillChar(Q);

  nrzDelete(&a1, R);
  nrzDelete(&b1, R);
  return ab;
}

static number nrzMapMachineInt(number from, const coeffs /*src*/, const coeffs /*dst*/)
{
  mpz_ptr erg = (mpz_ptr) omAllocBin(gmp_nrz_bin);
  mpz_init_set_ui(erg, (unsigned long) from);
  return nrz_short((number) erg);
}

static number nrzMapZp(number from, const coeffs /*src*/, const coeffs /*dst*/)
{
  mpz_ptr erg = (mpz_ptr) omAllocBin(gmp_nrz_bin);
  mpz_init_set_si(erg, (long) from);
  return nrz_short((number) erg);
}

static number nrzModNMap(number from, const coeffs /* src */, const coeffs /*dst*/)
{
  mpz_ptr erg = (mpz_ptr) omAllocBin(gmp_nrz_bin);
  mpz_init_set(erg, (mpz_ptr) from);
  return nrz_short((number) erg);
}

static number nrzMapQ(number from, const coeffs /* src */, const coeffs dst)
{
  if (SR_HDL(from) & SR_INT)
    return nrzInit(SR_TO_INT(from),dst);
  if (from->s!=3)
  {
    WerrorS("rational in map to integer");
    return NULL;
  }
  mpz_ptr erg = (mpz_ptr) omAllocBin(gmp_nrz_bin);
  mpz_init_set(erg, from->z);
  return nrz_short((number) erg);
}

static nMapFunc nrzSetMap(const coeffs src, const coeffs /*dst*/)
{
  /* dst = rintegers */
  if (src->rep==n_rep_gmp) //nCoeff_is_Zn(src) || nCoeff_is_Ring_PtoM(src))
    return nrzModNMap;

  if ((src->rep==n_rep_gap_gmp) && nCoeff_is_Z(src))
  {
    return ndCopyMap; //nrzCopyMap;
  }
  if (src->rep==n_rep_gap_rat) /*&& nCoeff_is_Z(src)) Q, bigint*/
  {
    return nrzMapQ;
  }
  if ((src->rep==n_rep_int) && nCoeff_is_Ring_2toM(src))
  {
    return nrzMapMachineInt;
  }
  if ((src->rep==n_rep_int) && nCoeff_is_Zp(src))
  {
    return nrzMapZp;
  }
  return NULL;      // default
}


/*
 * set the exponent (allocate and init tables) (TODO)
 */

void nrzSetExp(int, coeffs)
{
}

void nrzInitExp(int, coeffs)
{
}

#ifdef LDEBUG
BOOLEAN nrzDBTest (number x, const char *f, const int l, const coeffs)
{
  if (SR_HDL(x) & SR_INT) return TRUE;
  if (mpz_sgn1((mpz_ptr) x)==0)
  {
    Print("gmp-0 %s:%d\n",f,l);
    return FALSE;
  }
  if (mpz_size1((mpz_ptr)x)<=MP_SMALL)
  {
    long ui=mpz_get_si((mpz_ptr)x);
    if ((((ui<<3)>>3)==ui)
    && (mpz_cmp_si((mpz_ptr)x,ui)==0))
    {
      Print("gmp-small %s:%d\n",f,l);
      return FALSE;
    }
  }
  return TRUE;
}
#endif

void nrzWrite (number a, const coeffs)
{
  char *s,*z;
  if (a==NULL)
  {
    StringAppendS("o");
  }
  else
  {
    if (n_Z_IS_SMALL(a))
    {
      StringAppend("%d", SR_TO_INT(a));
    }
    else
    {
      int l=mpz_sizeinbase((mpz_ptr) a, 10) + 2;
      s=(char*)omAlloc(l);
      z=mpz_get_str(s,10,(mpz_ptr) a);
      StringAppendS(z);
      omFreeSize((ADDRESS)s,l);
    }
  }
}

/*2
* extracts a long integer from s, returns the rest    (COPY FROM longrat0.cc)
*/
static const char * nlEatLongC(char *s, mpz_ptr i)
{
  const char * start=s;

  if (*s<'0' || *s>'9')
  {
    mpz_set_ui(i,1);
    return s;
  }
  while (*s >= '0' && *s <= '9') s++;
  if (*s=='\0')
  {
    mpz_set_str(i,start,10);
  }
  else
  {
    char c=*s;
    *s='\0';
    mpz_set_str(i,start,10);
    *s=c;
  }
  return s;
}

static const char * nrzRead (const char *s, number *a, const coeffs)
{
  mpz_ptr z = (mpz_ptr) omAllocBin(gmp_nrz_bin);
  {
    mpz_init(z);
    s = nlEatLongC((char *) s, z);
  }
  *a = nrz_short((number) z);
  return s;
}

static CanonicalForm nrzConvSingNFactoryN( number n, BOOLEAN setChar, const coeffs /*r*/ )
{
  if (setChar) setCharacteristic( 0 );

  CanonicalForm term;
  if ( n_Z_IS_SMALL(n))
  {
    term = SR_TO_INT(n);
  }
  else
  {
    mpz_t dummy;
    mpz_init_set( dummy,n->z );
    term = make_cf( dummy );
  }
  return term;
}

static number nrzConvFactoryNSingN( const CanonicalForm n, const coeffs r)
{
  if (n.isImm())
  {
    return nrzInit(n.intval(),r);
  }
  else
  {
    if ( !n.den().isOne() )
    {
     WerrorS("rational in conversion to integer");
     return NULL;
    }
    mpz_ptr z = (mpz_ptr) omAlloc0Bin(gmp_nrz_bin);
    gmp_numerator( n,z);
    return nrz_short((number)z);
  }
}

static void nrzMPZ(mpz_t res, number &a, const coeffs)
{
  if (n_Z_IS_SMALL(a))
    mpz_init_set_si(res, SR_TO_INT(a));
  else
    mpz_init_set(res, (mpz_ptr) a);
}

static coeffs nrzQuot1(number c, const coeffs r)
{
    mpz_t dummy;
    if(n_Z_IS_SMALL(c))
    {
      long ch = r->cfInt(c, r);
      mpz_init_set_ui(dummy, ch);
    }
    else
    {
      mpz_init_set(dummy, (mpz_ptr)c);
    }
    ZnmInfo info;
    info.base = dummy;
    info.exp = (unsigned long) 1;
    coeffs rr = nInitChar(n_Zn, (void*)&info);
    mpz_clear(dummy);
    return(rr);
}

number nlReadFd(const ssiInfo *d, const coeffs);
void nlWriteFd(number n, const ssiInfo* d, const coeffs);

BOOLEAN nrzInitChar(coeffs r,  void *)
{
  assume( getCoeffType(r) == n_Z );

  r->is_field=FALSE;
  r->is_domain=TRUE;
  r->rep=n_rep_gap_gmp;

  //r->nCoeffIsEqual = ndCoeffIsEqual;
  r->cfCoeffName = nrzCoeffName;
  //r->cfKillChar = ndKillChar;
  r->cfMult  = nrzMult;
  r->cfSub   = nrzSub;
  r->cfAdd   = nrzAdd;
  r->cfDiv   = nrzDiv;
  r->cfIntMod= nrzIntMod;
  r->cfExactDiv= nrzExactDiv;
  r->cfInit = nrzInit;
  r->cfInitMPZ = nrzInitMPZ;
  r->cfSize  = nrzSize;
  r->cfInt  = nrzInt;
  //#ifdef HAVE_RINGS
  r->cfDivComp = nrzDivComp; // only for ring stuff
  r->cfIsUnit = nrzIsUnit; // only for ring stuff
  r->cfGetUnit = nrzGetUnit; // only for ring stuff
  r->cfAnn = nrzAnn;
  r->cfExtGcd = nrzExtGcd; // only for ring stuff
  r->cfXExtGcd = nrzXExtGcd; // only for ring stuff
  r->cfEucNorm = nrzEucNorm;
  r->cfQuotRem = nrzSmallestQuotRem;
  r->cfDivBy = nrzDivBy; // only for ring stuff
  //#endif
  r->cfInpNeg   = nrzNeg;
  r->cfInvers= nrzInvers;
  r->cfCopy  = nrzCopy;
  r->cfWriteLong = nrzWrite;
  r->cfRead = nrzRead;
  r->cfGreater = nrzGreater;
  r->cfEqual = nrzEqual;
  r->cfIsZero = nrzIsZero;
  r->cfIsOne = nrzIsOne;
  r->cfIsMOne = nrzIsMOne;
  r->cfGreaterZero = nrzGreaterZero;
  r->cfPower = nrzPower;
  r->cfGcd  = nrzGcd;
  r->cfLcm  = nrzLcm;
  r->cfDelete= nrzDelete;
  r->cfSetMap = nrzSetMap;
  r->convSingNFactoryN = nrzConvSingNFactoryN;
  r->convFactoryNSingN = nrzConvFactoryNSingN;
  r->cfMPZ = nrzMPZ;
  r->cfFarey = nrzFarey;
  r->cfWriteFd=nlWriteFd;
  r->cfReadFd=nlReadFd;

  r->cfQuot1 = nrzQuot1;
  // requires conversion to factory:
  r->cfChineseRemainder=nlChineseRemainderSym;
  // debug stuff

#ifdef LDEBUG
  r->cfDBTest=nrzDBTest;
#endif

  r->ch = 0;
  r->has_simple_Alloc=FALSE;
  r->has_simple_Inverse=FALSE;
  return FALSE;
}
#endif
#endif
