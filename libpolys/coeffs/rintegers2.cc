/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/*
* ABSTRACT: numbers (integers)
*/


#ifdef HAVE_RINGS
#if SI_INTEGER_VARIANT == 2

#include "coeffs/si_gmp.h"

/*
 * Multiply two numbers
 */
static number nrzMult (number a, number b, const coeffs)
{
  mpz_ptr erg = (mpz_ptr) omAllocBin(gmp_nrz_bin);
  mpz_init(erg);
  mpz_mul(erg, (mpz_ptr) a, (mpz_ptr) b);
  return (number) erg;
}

/*
 * Give the smallest non unit k, such that a * x = k = b * y has a solution
 */
static number nrzLcm (number a,number b,const coeffs)
{
  mpz_ptr erg = (mpz_ptr) omAllocBin(gmp_nrz_bin);
  mpz_init(erg);
  mpz_lcm(erg, (mpz_ptr) a, (mpz_ptr) b);
  return (number) erg;
}

/*
 * Give the largest non unit k, such that a = x * k, b = y * k has
 * a solution.
 */
static number nrzGcd (number a,number b,const coeffs)
{
  mpz_ptr erg = (mpz_ptr) omAllocBin(gmp_nrz_bin);
  mpz_init(erg);
  mpz_gcd(erg, (mpz_ptr) a, (mpz_ptr) b);
  return (number) erg;
}

/*
 * Give the largest non unit k, such that a = x * k, b = y * k has
 * a solution and r, s, s.t. k = s*a + t*b
 */
static number nrzExtGcd (number a, number b, number *s, number *t, const coeffs)
{
  mpz_ptr erg = (mpz_ptr) omAllocBin(gmp_nrz_bin);
  mpz_ptr bs = (mpz_ptr) omAllocBin(gmp_nrz_bin);
  mpz_ptr bt = (mpz_ptr) omAllocBin(gmp_nrz_bin);
  mpz_init(erg);
  mpz_init(bs);
  mpz_init(bt);
  mpz_gcdext(erg, bs, bt, (mpz_ptr) a, (mpz_ptr) b);
  *s = (number) bs;
  *t = (number) bt;
  return (number) erg;
}

static number nrzXExtGcd (number a, number b, number *s, number *t, number *u, number *v, const coeffs )
{
  mpz_ptr erg = (mpz_ptr) omAllocBin(gmp_nrz_bin);
  mpz_ptr bs = (mpz_ptr) omAllocBin(gmp_nrz_bin);
  mpz_ptr bt = (mpz_ptr) omAllocBin(gmp_nrz_bin);
  mpz_init(erg);
  mpz_init(bs);
  mpz_init(bt);

  mpz_gcdext(erg, bs, bt, (mpz_ptr)a, (mpz_ptr)b);

  mpz_ptr bu = (mpz_ptr) omAllocBin(gmp_nrz_bin);
  mpz_ptr bv = (mpz_ptr) omAllocBin(gmp_nrz_bin);

  mpz_init_set(bu, (mpz_ptr) b);
  mpz_init_set(bv, (mpz_ptr) a);

  assume(mpz_cmp_si(erg, 0));

  mpz_div(bu, bu, erg);
  mpz_div(bv, bv, erg);

  mpz_mul_si(bu, bu, -1);
  *u = (number) bu;
  *v = (number) bv;

  *s = (number) bs;
  *t = (number) bt;
  return (number) erg;
}

static void nrzPower (number a, int i, number * result, const coeffs)
{
  mpz_ptr erg = (mpz_ptr) omAllocBin(gmp_nrz_bin);
  mpz_init(erg);
  mpz_pow_ui(erg, (mpz_ptr) a, i);
  *result = (number) erg;
}

/*
 * create a number from int
 */
number nrzInit (long i, const coeffs)
{
  mpz_ptr erg = (mpz_ptr) omAllocBin(gmp_nrz_bin);
  mpz_init_set_si(erg, i);
  return (number) erg;
}

void nrzDelete(number *a, const coeffs)
{
  if (*a != NULL)
  {
    mpz_clear((mpz_ptr) *a);
    omFreeBin((ADDRESS) *a, gmp_nrz_bin);
    *a = NULL;
  }
}

static number nrzCopy(number a, const coeffs)
{
  mpz_ptr erg = (mpz_ptr) omAllocBin(gmp_nrz_bin);
  mpz_init_set(erg, (mpz_ptr) a);
  return (number) erg;
}

#if 0
number nrzCopyMap(number a, const coeffs /*src*/, const coeffs dst)
{
  return nrzCopy(a,dst);
}
#endif

int nrzSize(number a, const coeffs)
{
  mpz_ptr p=(mpz_ptr)a;
  int s=p->_mp_alloc;
  if (s==1) s=(mpz_cmp_ui(p,0)!=0);
  return s;

}

/*
 * convert a number to int
 */
static long nrzInt(number &n, const coeffs)
{
  return mpz_get_si( (mpz_ptr)n);
}

static number nrzAdd (number a, number b, const coeffs)
{
  mpz_ptr erg = (mpz_ptr) omAllocBin(gmp_nrz_bin);
  mpz_init(erg);
  mpz_add(erg, (mpz_ptr) a, (mpz_ptr) b);
  return (number) erg;
}

static number nrzSub (number a, number b, const coeffs)
{
  mpz_ptr erg = (mpz_ptr) omAllocBin(gmp_nrz_bin);
  mpz_init(erg);
  mpz_sub(erg, (mpz_ptr) a, (mpz_ptr) b);
  return (number) erg;
}

static number nrzGetUnit (number, const coeffs r)
{
  return nrzInit(1, r);
}

static BOOLEAN nrzIsUnit (number a, const coeffs)
{
  return 0 == mpz_cmpabs_ui((mpz_ptr) a, 1);
}

static BOOLEAN nrzIsZero (number  a, const coeffs)
{
  return 0 == mpz_cmpabs_ui((mpz_ptr) a, 0);
}

static BOOLEAN nrzIsOne (number a, const coeffs)
{
  return (0 == mpz_cmp_ui((mpz_ptr) a, 1));
}

static BOOLEAN nrzIsMOne (number a, const coeffs)
{
  return (0 == mpz_cmp_si((mpz_ptr) a, -1));
}

static BOOLEAN nrzEqual (number a,number b, const coeffs)
{
  return 0 == mpz_cmp((mpz_ptr) a, (mpz_ptr) b);
}

static BOOLEAN nrzGreater (number a,number b, const coeffs)
{
  return 0 < mpz_cmp((mpz_ptr) a, (mpz_ptr) b);
}

static BOOLEAN nrzGreaterZero (number k, const coeffs)
{
  return 0 < mpz_sgn1((mpz_ptr) k);
}

static BOOLEAN nrzDivBy (number a,number b, const coeffs)
{
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

static number nrzDiv (number a,number b, const coeffs r)
{
  mpz_ptr erg = (mpz_ptr) omAllocBin(gmp_nrz_bin);
  mpz_init(erg);
  if (nrzIsZero(b,r))
  {
    WerrorS(nDivBy0);
  }
  else
  {
    mpz_ptr r = (mpz_ptr) omAllocBin(gmp_nrz_bin);
    mpz_init(r);
    mpz_tdiv_qr(erg, r, (mpz_ptr) a, (mpz_ptr) b);
    mpz_clear(r);
    omFreeBin(r, gmp_nrz_bin);
  }
  return (number) erg;
}

static number nrzExactDiv (number a,number b, const coeffs r)
{
  mpz_ptr erg = (mpz_ptr) omAllocBin(gmp_nrz_bin);
  mpz_init(erg);
  if (nrzIsZero(b,r))
  {
    WerrorS(nDivBy0);
  }
  else
  {
    mpz_tdiv_q(erg, (mpz_ptr) a, (mpz_ptr) b);
  }
  return (number) erg;
}

static number nrzEucNorm (number a, const coeffs )
{
  mpz_ptr abs = (mpz_ptr) omAllocBin(gmp_nrz_bin);
  mpz_init(abs);
  mpz_abs(abs, (mpz_ptr)a);

  return (number) abs;
}

static number nrzSmallestQuotRem (number a, number b, number * r, const coeffs )
{
  mpz_ptr qq = (mpz_ptr) omAllocBin(gmp_nrz_bin);
  mpz_init(qq);
  mpz_ptr rr = (mpz_ptr) omAllocBin(gmp_nrz_bin);
  mpz_init(rr);
  int gsign = mpz_sgn((mpz_ptr) b);
  mpz_t gg, ghalf;
  mpz_init(gg);
  mpz_init(ghalf);
  mpz_abs(gg, (mpz_ptr) b);
  mpz_fdiv_qr(qq, rr, (mpz_ptr) a, gg);
  mpz_tdiv_q_2exp(ghalf, gg, 1);
  if (mpz_cmp(rr, ghalf) > 0)  // r > ghalf
    {
      mpz_sub(rr, rr, gg);
      mpz_add_ui(qq, qq, 1);
    }
  if (gsign < 0) mpz_neg(qq, qq);

  mpz_clear(gg);
  mpz_clear(ghalf);
  if (r==NULL)
  {
    mpz_clear(rr);
    omFreeBin(rr,gmp_nrz_bin);
  }
  else
  {
    *r=(number)rr;
  }
  return (number) qq;
}

/*
static number nrzQuotRem (number a, number b, number * r, const coeffs )
{
  mpz_ptr qq = (mpz_ptr) omAllocBin(gmp_nrz_bin);
  mpz_init(qq);
  mpz_ptr rr = (mpz_ptr) omAllocBin(gmp_nrz_bin);
  mpz_init(rr);
  mpz_tdiv_qr(qq, rr, (mpz_ptr) a, (mpz_ptr) b);
  if (r==NULL)
  {
    mpz_clear(rr);
    omFreeBin(rr,gmp_nrz_bin);
  }
  else
  {
    *r=(number)rr;
  }
  return (number) qq;
}
*/

static number nrzIntMod (number a,number b, const coeffs)
{
  mpz_ptr erg = (mpz_ptr) omAllocBin(gmp_nrz_bin);
  mpz_init(erg);
  mpz_ptr r = (mpz_ptr) omAllocBin(gmp_nrz_bin);
  mpz_init(r);
  mpz_tdiv_qr(erg, r, (mpz_ptr) a, (mpz_ptr) b);
  mpz_clear(erg);
  omFreeBin(erg, gmp_nrz_bin);
  return (number) r;
}

static number  nrzInvers (number c, const coeffs r)
{
  if (!nrzIsUnit((number) c, r))
  {
    WerrorS("Non invertible element.");
    return nrzInit(0,r);
  }
  return nrzCopy(c,r);
}

static number nrzNeg (number c, const coeffs)
{
// nNeg inplace !!!
  mpz_mul_si((mpz_ptr) c, (mpz_ptr) c, -1);
  return c;
}

static number nrzMapMachineInt(number from, const coeffs /*src*/, const coeffs /*dst*/)
{
  mpz_ptr erg = (mpz_ptr) omAllocBin(gmp_nrz_bin);
  mpz_init_set_ui(erg, (unsigned long) from);
  return (number) erg;
}

static number nrzMapZp(number from, const coeffs /*src*/, const coeffs /*dst*/)
{
  mpz_ptr erg = (mpz_ptr) omAllocBin(gmp_nrz_bin);
  mpz_init_set_si(erg, (long) from);
  return (number) erg;
}

static number nrzMapQ(number from, const coeffs src, const coeffs /*dst*/)
{
  mpz_ptr erg = (mpz_ptr) omAllocBin(gmp_nrz_bin);
  mpz_init(erg);
  nlMPZ(erg, from, src);
  return (number) erg;
}

static number nrzMaplongR(number from, const coeffs src, const coeffs dst)
{
  gmp_float *ff=(gmp_float*)from;
  if (mpf_fits_slong_p(ff->t))
  {
    long l=mpf_get_si(ff->t);
    return nrzInit(l,dst);
  }
  char *out=floatToStr(*(gmp_float*)from, src->float_len);
  char *p=strchr(out,'.');
  *p='\0';
  mpz_ptr erg = (mpz_ptr) omAllocBin(gmp_nrz_bin);
  mpz_init(erg);
  if (out[0]=='-')
  {
    mpz_set_str(erg,out+1,10);
    mpz_mul_si(erg, erg, -1);
  }
  else
  {
    mpz_set_str(erg,out,10);
  }
  omFree( (void *)out );
  return (number) erg;
}

static nMapFunc nrzSetMap(const coeffs src, const coeffs /*dst*/)
{
  /* dst = currRing */
  /* dst = nrn */
  if ((src->rep==n_rep_gmp)
  && (nCoeff_is_Z(src) || nCoeff_is_Zn(src) || nCoeff_is_Ring_PtoM(src)))
  {
    return ndCopyMap; //nrzCopyMap;
  }
  if ((src->rep==n_rep_gap_gmp) /*&& nCoeff_is_Z(src)*/)
  {
    return ndCopyMap; //nrzCopyMap;
  }
  if (nCoeff_is_Ring_2toM(src))
  {
    return nrzMapMachineInt;
  }
  if (nCoeff_is_Zp(src))
  {
    return nrzMapZp;
  }
  if (getCoeffType(src)==n_Q /*nCoeff_is_Q(src) or coeffs_BIGINT*/)
  {
    return nrzMapQ;
  }
  if (nCoeff_is_long_R(src))
  {
    return nrzMaplongR;
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
static BOOLEAN nrzDBTest (number, const char *, const int, const coeffs)
{
  return TRUE;//TODO
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
    int l=mpz_sizeinbase((mpz_ptr) a, 10) + 2;
    s=(char*)omAlloc(l);
    z=mpz_get_str(s,10,(mpz_ptr) a);
    StringAppendS(z);
    omFreeSize((ADDRESS)s,l);
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


static CanonicalForm nrzConvSingNFactoryN(number n, BOOLEAN setChar, const coeffs /*r*/)
{
  if (setChar) setCharacteristic( 0 );

  CanonicalForm term;
  mpz_t num;
  mpz_init_set(num, *((mpz_t*)n));
  term = make_cf(num);
  return term;
}

static number nrzConvFactoryNSingN(const CanonicalForm n, const coeffs r)
{
  if (n.isImm())
    return nrzInit(n.intval(),r);
  else
  {
    mpz_ptr m = (mpz_ptr) omAllocBin(gmp_nrz_bin);
    gmp_numerator(n,m);
    if (!n.den().isOne())
    {
      WarnS("denominator is not 1 in factory");
    }
    return (number) m;
  }
}

static const char * nrzRead (const char *s, number *a, const coeffs)
{
  mpz_ptr z = (mpz_ptr) omAllocBin(gmp_nrz_bin);
  {
    mpz_init(z);
    s = nlEatLongC((char *) s, z);
  }
  *a = (number) z;
  return s;
}

static coeffs nrzQuot1(number c, const coeffs r)
{
    long ch = r->cfInt(c, r);
    mpz_t dummy;
    mpz_init_set_ui(dummy, ch);
    ZnmInfo info;
    info.base = dummy;
    info.exp = (unsigned long) 1;
    coeffs rr = nInitChar(n_Zn, (void*)&info);
    mpz_clear(dummy);
    return(rr);
}

static number nrzInitMPZ(mpz_t m, const coeffs)
{
  mpz_ptr z = (mpz_ptr) omAllocBin(gmp_nrz_bin);
  mpz_init_set(z, m);
  return (number)z;
}

static void nrzMPZ(mpz_t res, number &a, const coeffs)
{
  mpz_init_set(res, (mpz_ptr) a);
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

void nrzWriteFd(number n, const ssiInfo* d, const coeffs)
{
  mpz_out_str (d->f_write,SSI_BASE, (mpz_ptr)n);
  fputc(' ',d->f_write);
}

number nrzReadFd(const ssiInfo *d, const coeffs)
{
  mpz_ptr erg = (mpz_ptr) omAllocBin(gmp_nrz_bin);
  mpz_init(erg);
  s_readmpz_base(d->f_read,erg,SSI_BASE);
  return (number)erg;
}

BOOLEAN nrzInitChar(coeffs r,  void *)
{
  assume( getCoeffType(r) == n_Z );

  r->is_field=FALSE;
  r->is_domain=TRUE;
  r->rep=n_rep_gmp;

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
  r->cfMPZ = nrzMPZ;
  r->cfSize  = nrzSize;
  r->cfInt  = nrzInt;
  r->cfDivComp = nrzDivComp;
  r->cfIsUnit = nrzIsUnit;
  r->cfGetUnit = nrzGetUnit;
  r->cfExtGcd = nrzExtGcd;
  r->cfXExtGcd = nrzXExtGcd;
  r->cfDivBy = nrzDivBy;
  r->cfEucNorm = nrzEucNorm;
  r->cfQuotRem = nrzSmallestQuotRem;
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
  r->cfQuot1 = nrzQuot1;
  r->convSingNFactoryN=nrzConvSingNFactoryN;
  r->convFactoryNSingN=nrzConvFactoryNSingN;
  r->cfChineseRemainder=nlChineseRemainderSym;
  r->cfFarey=nrzFarey;
  r->cfWriteFd=nrzWriteFd;
  r->cfReadFd=nrzReadFd;
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
