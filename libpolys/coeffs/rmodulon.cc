/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/*
* ABSTRACT: numbers modulo n
*/
#include "misc/auxiliary.h"

#include "misc/mylimits.h"
#include "misc/prime.h" // IsPrime
#include "reporter/reporter.h"

#include "coeffs/si_gmp.h"
#include "coeffs/coeffs.h"
#include "coeffs/modulop.h"
#include "coeffs/rintegers.h"
#include "coeffs/numbers.h"

#include "coeffs/mpr_complex.h"

#include "coeffs/longrat.h"
#include "coeffs/rmodulon.h"

#include <string.h>

#ifdef HAVE_RINGS

void nrnWrite (number a, const coeffs);
#ifdef LDEBUG
BOOLEAN nrnDBTest      (number a, const char *f, const int l, const coeffs r);
#endif

EXTERN_VAR omBin gmp_nrz_bin;

coeffs nrnInitCfByName(char *s,n_coeffType n)
{
  const char start[]="ZZ/bigint(";
  const int start_len=strlen(start);
  if (strncmp(s,start,start_len)==0)
  {
    s+=start_len;
    mpz_t z;
    mpz_init(z);
    s=nEatLong(s,z);
    ZnmInfo info;
    info.base=z;
    info.exp= 1;
    while ((*s!='\0') && (*s!=')')) s++;
    // expect ")" or ")^exp"
    if (*s=='\0') { mpz_clear(z); return NULL; }
    if (((*s)==')') && (*(s+1)=='^'))
    {
      s=s+2;
      s=nEati(s,&(info.exp),0);
      return nInitChar(n_Znm,(void*) &info);
    }
    else
      return nInitChar(n_Zn,(void*) &info);
  }
  else return NULL;
}

STATIC_VAR char* nrnCoeffName_buff=NULL;
static char* nrnCoeffName(const coeffs r)
{
  if(nrnCoeffName_buff!=NULL) omFree(nrnCoeffName_buff);
  size_t l = (size_t)mpz_sizeinbase(r->modBase, 10) + 2;
  char* s = (char*) omAlloc(l);
  l+=24;
  nrnCoeffName_buff=(char*)omAlloc(l);
  s= mpz_get_str (s, 10, r->modBase);
  int ll;
  if (nCoeff_is_Zn(r))
  {
    if (strlen(s)<10)
      ll=snprintf(nrnCoeffName_buff,l,"ZZ/(%s)",s);
    else
      ll=snprintf(nrnCoeffName_buff,l,"ZZ/bigint(%s)",s);
  }
  else if (nCoeff_is_Ring_PtoM(r))
    ll=snprintf(nrnCoeffName_buff,l,"ZZ/(bigint(%s)^%lu)",s,r->modExponent);
  assume(ll<(int)l); // otherwise nrnCoeffName_buff too small
  omFreeSize((ADDRESS)s, l-22);
  return nrnCoeffName_buff;
}

static BOOLEAN nrnCoeffIsEqual(const coeffs r, n_coeffType n, void * parameter)
{
  /* test, if r is an instance of nInitCoeffs(n,parameter) */
  ZnmInfo *info=(ZnmInfo*)parameter;
  return (n==r->type) && (r->modExponent==info->exp)
  && (mpz_cmp(r->modBase,info->base)==0);
}

static void nrnKillChar(coeffs r)
{
  mpz_clear(r->modNumber);
  mpz_clear(r->modBase);
  omFreeBin((void *) r->modBase, gmp_nrz_bin);
  omFreeBin((void *) r->modNumber, gmp_nrz_bin);
}

static coeffs nrnQuot1(number c, const coeffs r)
{
    coeffs rr;
    long ch = r->cfInt(c, r);
    mpz_t a,b;
    mpz_init_set(a, r->modNumber);
    mpz_init_set_ui(b, ch);
    mpz_t gcd;
    mpz_init(gcd);
    mpz_gcd(gcd, a,b);
    if(mpz_cmp_ui(gcd, 1) == 0)
    {
      WerrorS("constant in q-ideal is coprime to modulus in ground ring");
      WerrorS("Unable to create qring!");
      return NULL;
    }
    if(r->modExponent == 1)
    {
      ZnmInfo info;
      info.base = gcd;
      info.exp = (unsigned long) 1;
      rr = nInitChar(n_Zn, (void*)&info);
    }
    else
    {
      ZnmInfo info;
      info.base = r->modBase;
      int kNew = 1;
      mpz_t baseTokNew;
      mpz_init(baseTokNew);
      mpz_set(baseTokNew, r->modBase);
      while(mpz_cmp(gcd, baseTokNew) > 0)
      {
        kNew++;
        mpz_mul(baseTokNew, baseTokNew, r->modBase);
      }
      //printf("\nkNew = %i\n",kNew);
      info.exp = kNew;
      mpz_clear(baseTokNew);
      rr = nInitChar(n_Znm, (void*)&info);
    }
    mpz_clear(gcd);
    return(rr);
}

static number nrnCopy(number a, const coeffs)
{
  mpz_ptr erg = (mpz_ptr) omAllocBin(gmp_nrz_bin);
  mpz_init_set(erg, (mpz_ptr) a);
  return (number) erg;
}

/*
 * create a number from int
 */
static number nrnInit(long i, const coeffs r)
{
  mpz_ptr erg = (mpz_ptr) omAllocBin(gmp_nrz_bin);
  mpz_init_set_si(erg, i);
  mpz_mod(erg, erg, r->modNumber);
  return (number) erg;
}

/*
 * convert a number to int
 */
static long nrnInt(number &n, const coeffs)
{
  return mpz_get_si((mpz_ptr) n);
}

#if SI_INTEGER_VARIANT==2
#define nrnDelete nrzDelete
#define nrnSize   nrzSize
#else
static void nrnDelete(number *a, const coeffs)
{
  if (*a != NULL)
  {
    mpz_clear((mpz_ptr) *a);
    omFreeBin((void *) *a, gmp_nrz_bin);
    *a = NULL;
  }
}
static int nrnSize(number a, const coeffs)
{
  mpz_ptr p=(mpz_ptr)a;
  int s=p->_mp_alloc;
  if (s==1) s=(mpz_cmp_ui(p,0)!=0);
  return s;
}
#endif
/*
 * Multiply two numbers
 */
static number nrnMult(number a, number b, const coeffs r)
{
  mpz_ptr erg = (mpz_ptr)omAllocBin(gmp_nrz_bin);
  mpz_init(erg);
  mpz_mul(erg, (mpz_ptr)a, (mpz_ptr) b);
  mpz_mod(erg, erg, r->modNumber);
  return (number) erg;
}

static void nrnPower(number a, int i, number * result, const coeffs r)
{
  mpz_ptr erg = (mpz_ptr)omAllocBin(gmp_nrz_bin);
  mpz_init(erg);
  mpz_powm_ui(erg, (mpz_ptr)a, i, r->modNumber);
  *result = (number) erg;
}

static number nrnAdd(number a, number b, const coeffs r)
{
  mpz_ptr erg = (mpz_ptr)omAllocBin(gmp_nrz_bin);
  mpz_init(erg);
  mpz_add(erg, (mpz_ptr)a, (mpz_ptr) b);
  mpz_mod(erg, erg, r->modNumber);
  return (number) erg;
}

static number nrnSub(number a, number b, const coeffs r)
{
  mpz_ptr erg = (mpz_ptr)omAllocBin(gmp_nrz_bin);
  mpz_init(erg);
  mpz_sub(erg, (mpz_ptr)a, (mpz_ptr) b);
  mpz_mod(erg, erg, r->modNumber);
  return (number) erg;
}

static BOOLEAN nrnIsZero(number a, const coeffs)
{
  return 0 == mpz_cmpabs_ui((mpz_ptr)a, 0);
}

static number nrnNeg(number c, const coeffs r)
{
  if( !nrnIsZero(c, r) )
    // Attention: This method operates in-place.
    mpz_sub((mpz_ptr)c, r->modNumber, (mpz_ptr)c);
  return c;
}

static number nrnInvers(number c, const coeffs r)
{
  mpz_ptr erg = (mpz_ptr)omAllocBin(gmp_nrz_bin);
  mpz_init(erg);
  mpz_invert(erg, (mpz_ptr)c, r->modNumber);
  return (number) erg;
}

/*
 * Give the largest k, such that a = x * k, b = y * k has
 * a solution.
 * a may be NULL, b not
 */
static number nrnGcd(number a, number b, const coeffs r)
{
  mpz_ptr erg = (mpz_ptr)omAllocBin(gmp_nrz_bin);
  mpz_init_set(erg, r->modNumber);
  if (a != NULL) mpz_gcd(erg, erg, (mpz_ptr)a);
  mpz_gcd(erg, erg, (mpz_ptr)b);
  if(mpz_cmp(erg,r->modNumber)==0)
  {
    mpz_clear(erg);
    omFreeBin((ADDRESS)erg,gmp_nrz_bin);
    return nrnInit(0,r);
  }
  return (number)erg;
}

/*
 * Give the smallest k, such that a * x = k = b * y has a solution
 * TODO: lcm(gcd,gcd) better than gcd(lcm) ?
 */
static number nrnLcm(number a, number b, const coeffs r)
{
  number erg = nrnGcd(NULL, a, r);
  number tmp = nrnGcd(NULL, b, r);
  mpz_lcm((mpz_ptr)erg, (mpz_ptr)erg, (mpz_ptr)tmp);
  nrnDelete(&tmp, r);
  return (number)erg;
}

/* Not needed any more, but may have room for improvement
   number nrnGcd3(number a,number b, number c,ring r)
{
  mpz_ptr erg = (mpz_ptr) omAllocBin(gmp_nrz_bin);
  mpz_init(erg);
  if (a == NULL) a = (number)r->modNumber;
  if (b == NULL) b = (number)r->modNumber;
  if (c == NULL) c = (number)r->modNumber;
  mpz_gcd(erg, (mpz_ptr)a, (mpz_ptr)b);
  mpz_gcd(erg, erg, (mpz_ptr)c);
  mpz_gcd(erg, erg, r->modNumber);
  return (number)erg;
}
*/

/*
 * Give the largest k, such that a = x * k, b = y * k has
 * a solution and r, s, s.t. k = s*a + t*b
 * CF: careful: ExtGcd is wrong as implemented (or at least may not
 * give you what you want:
 * ExtGcd(5, 10 modulo 12):
 * the gcdext will return 5 = 1*5 + 0*10
 * however, mod 12, the gcd should be 1
 */
static number nrnExtGcd(number a, number b, number *s, number *t, const coeffs r)
{
  mpz_ptr erg = (mpz_ptr)omAllocBin(gmp_nrz_bin);
  mpz_ptr bs  = (mpz_ptr)omAllocBin(gmp_nrz_bin);
  mpz_ptr bt  = (mpz_ptr)omAllocBin(gmp_nrz_bin);
  mpz_init(erg);
  mpz_init(bs);
  mpz_init(bt);
  mpz_gcdext(erg, bs, bt, (mpz_ptr)a, (mpz_ptr)b);
  mpz_mod(bs, bs, r->modNumber);
  mpz_mod(bt, bt, r->modNumber);
  *s = (number)bs;
  *t = (number)bt;
  return (number)erg;
}

static BOOLEAN nrnIsOne(number a, const coeffs)
{
  return 0 == mpz_cmp_si((mpz_ptr)a, 1);
}

static BOOLEAN nrnEqual(number a, number b, const coeffs)
{
  return 0 == mpz_cmp((mpz_ptr)a, (mpz_ptr)b);
}

static number nrnGetUnit(number k, const coeffs r)
{
  if (mpz_divisible_p(r->modNumber, (mpz_ptr)k)) return nrnInit(1,r);

  mpz_ptr unit = (mpz_ptr)nrnGcd(NULL, k, r);
  mpz_tdiv_q(unit, (mpz_ptr)k, unit);
  mpz_ptr gcd = (mpz_ptr)nrnGcd(NULL, (number)unit, r);
  if (!nrnIsOne((number)gcd,r))
  {
    mpz_ptr ctmp;
    // tmp := unit^2
    mpz_ptr tmp = (mpz_ptr) nrnMult((number) unit,(number) unit,r);
    // gcd_new := gcd(tmp, 0)
    mpz_ptr gcd_new = (mpz_ptr) nrnGcd(NULL, (number) tmp, r);
    while (!nrnEqual((number) gcd_new,(number) gcd,r))
    {
      // gcd := gcd_new
      ctmp = gcd;
      gcd = gcd_new;
      gcd_new = ctmp;
      // tmp := tmp * unit
      mpz_mul(tmp, tmp, unit);
      mpz_mod(tmp, tmp, r->modNumber);
      // gcd_new := gcd(tmp, 0)
      mpz_gcd(gcd_new, tmp, r->modNumber);
    }
    // unit := unit + modNumber / gcd_new
    mpz_tdiv_q(tmp, r->modNumber, gcd_new);
    mpz_add(unit, unit, tmp);
    mpz_mod(unit, unit, r->modNumber);
    nrnDelete((number*) &gcd_new, r);
    nrnDelete((number*) &tmp, r);
  }
  nrnDelete((number*) &gcd, r);
  return (number)unit;
}

/* XExtGcd  returns a unimodular matrix ((s,t)(u,v)) sth.
 * (a,b)^t ((st)(uv)) = (g,0)^t
 * Beware, the ExtGcd will not necessaairly do this.
 * Problem: if g = as+bt then (in Z/nZ) it follows NOT that
 *             1 = (a/g)s + (b/g) t
 * due to the zero divisors.
 */

//#define CF_DEB;
static number nrnXExtGcd(number a, number b, number *s, number *t, number *u, number *v, const coeffs r)
{
  number xx;
#ifdef CF_DEB
  StringSetS("XExtGcd of ");
  nrnWrite(a, r);
  StringAppendS("\t");
  nrnWrite(b, r);
  StringAppendS(" modulo ");
  nrnWrite(xx = (number)r->modNumber, r);
  Print("%s\n", StringEndS());
#endif

  mpz_ptr one = (mpz_ptr)omAllocBin(gmp_nrz_bin);
  mpz_ptr erg = (mpz_ptr)omAllocBin(gmp_nrz_bin);
  mpz_ptr bs  = (mpz_ptr)omAllocBin(gmp_nrz_bin);
  mpz_ptr bt  = (mpz_ptr)omAllocBin(gmp_nrz_bin);
  mpz_ptr bu  = (mpz_ptr)omAllocBin(gmp_nrz_bin);
  mpz_ptr bv  = (mpz_ptr)omAllocBin(gmp_nrz_bin);
  mpz_init(erg);
  mpz_init(one);
  mpz_init_set(bs, (mpz_ptr) a);
  mpz_init_set(bt, (mpz_ptr) b);
  mpz_init(bu);
  mpz_init(bv);
  mpz_gcd(erg, bs, bt);

#ifdef CF_DEB
  StringSetS("1st gcd:");
  nrnWrite(xx= (number)erg, r);
#endif

  mpz_gcd(erg, erg, r->modNumber);

  mpz_div(bs, bs, erg);
  mpz_div(bt, bt, erg);

#ifdef CF_DEB
  Print("%s\n", StringEndS());
  StringSetS("xgcd: ");
#endif

  mpz_gcdext(one, bu, bv, bs, bt);
  number ui = nrnGetUnit(xx = (number) one, r);
#ifdef CF_DEB
  n_Write(xx, r);
  StringAppendS("\t");
  n_Write(ui, r);
  Print("%s\n", StringEndS());
#endif
  nrnDelete(&xx, r);
  if (!nrnIsOne(ui, r))
  {
#ifdef CF_DEB
    PrintS("Scaling\n");
#endif
    number uii = nrnInvers(ui, r);
    nrnDelete(&ui, r);
    ui = uii;
    mpz_ptr uu = (mpz_ptr)omAllocBin(gmp_nrz_bin);
    mpz_init_set(uu, (mpz_ptr)ui);
    mpz_mul(bu, bu, uu);
    mpz_mul(bv, bv, uu);
    mpz_clear(uu);
    omFreeBin(uu, gmp_nrz_bin);
  }
  nrnDelete(&ui, r);
#ifdef CF_DEB
  StringSetS("xgcd");
  nrnWrite(xx= (number)bs, r);
  StringAppendS("*");
  nrnWrite(xx= (number)bu, r);
  StringAppendS(" + ");
  nrnWrite(xx= (number)bt, r);
  StringAppendS("*");
  nrnWrite(xx= (number)bv, r);
  Print("%s\n", StringEndS());
#endif

  mpz_mod(bs, bs, r->modNumber);
  mpz_mod(bt, bt, r->modNumber);
  mpz_mod(bu, bu, r->modNumber);
  mpz_mod(bv, bv, r->modNumber);
  *s = (number)bu;
  *t = (number)bv;
  *u = (number)bt;
  *u = nrnNeg(*u, r);
  *v = (number)bs;
  return (number)erg;
}

static BOOLEAN nrnIsMOne(number a, const coeffs r)
{
  if((r->ch==2) && (nrnIsOne(a,r))) return FALSE;
  mpz_t t; mpz_init_set(t, (mpz_ptr)a);
  mpz_add_ui(t, t, 1);
  bool erg = (0 == mpz_cmp(t, r->modNumber));
  mpz_clear(t);
  return erg;
}

static BOOLEAN nrnGreater(number a, number b, const coeffs)
{
  return 0 < mpz_cmp((mpz_ptr)a, (mpz_ptr)b);
}

static BOOLEAN nrnGreaterZero(number k, const coeffs cf)
{
  if (cf->is_field)
  {
    if (mpz_cmp_ui(cf->modBase,2)==0)
    {
      return TRUE;
    }
    mpz_t ch2; mpz_init_set(ch2, cf->modBase);
    mpz_sub_ui(ch2,ch2,1);
    mpz_divexact_ui(ch2,ch2,2);
    if (mpz_cmp(ch2,(mpz_ptr)k)<0)
      return FALSE;
    mpz_clear(ch2);
  }
  return 0 < mpz_sgn1((mpz_ptr)k);
}

static BOOLEAN nrnIsUnit(number a, const coeffs r)
{
  number tmp = nrnGcd(a, (number)r->modNumber, r);
  bool res = nrnIsOne(tmp, r);
  nrnDelete(&tmp, r);
  return res;
}

static number nrnAnn(number k, const coeffs r)
{
  mpz_ptr tmp = (mpz_ptr) omAllocBin(gmp_nrz_bin);
  mpz_init(tmp);
  mpz_gcd(tmp, (mpz_ptr) k, r->modNumber);
  if (mpz_cmp_si(tmp, 1)==0)
  {
    mpz_set_ui(tmp, 0);
    return (number) tmp;
  }
  mpz_divexact(tmp, r->modNumber, tmp);
  return (number) tmp;
}

static BOOLEAN nrnDivBy(number a, number b, const coeffs r)
{
  /* b divides a iff b/gcd(a, b) is a unit in the given ring: */
  number n = nrnGcd(a, b, r);
  mpz_tdiv_q((mpz_ptr)n, (mpz_ptr)b, (mpz_ptr)n);
  bool result = nrnIsUnit(n, r);
  nrnDelete(&n, NULL);
  return result;
}

static int nrnDivComp(number a, number b, const coeffs r)
{
  if (nrnEqual(a, b,r)) return 2;
  if (mpz_divisible_p((mpz_ptr) a, (mpz_ptr) b)) return -1;
  if (mpz_divisible_p((mpz_ptr) b, (mpz_ptr) a)) return 1;
  return 0;
}

static number nrnDiv(number a, number b, const coeffs r)
{
  if (r->is_field)
  {
    number inv=nrnInvers(b,r);
    number erg=nrnMult(a,inv,r);
    nrnDelete(&inv,r);
    return erg;
  }
  mpz_ptr erg = (mpz_ptr)omAllocBin(gmp_nrz_bin);
  mpz_init(erg);
  if (mpz_divisible_p((mpz_ptr)a, (mpz_ptr)b))
  {
    mpz_divexact(erg, (mpz_ptr)a, (mpz_ptr)b);
    return (number)erg;
  }
  else
  {
    mpz_ptr gcd = (mpz_ptr)nrnGcd(a, b, r);
    mpz_divexact(erg, (mpz_ptr)b, gcd);
    if (!nrnIsUnit((number)erg, r))
    {
      WerrorS("Division not possible, even by cancelling zero divisors.");
      nrnDelete((number*) &gcd, r);
      nrnDelete((number*) &erg, r);
      return (number)NULL;
    }
    // a / gcd(a,b) * [b / gcd (a,b)]^(-1)
    mpz_ptr tmp = (mpz_ptr)nrnInvers((number) erg,r);
    mpz_divexact(erg, (mpz_ptr)a, gcd);
    mpz_mul(erg, erg, tmp);
    nrnDelete((number*) &gcd, r);
    nrnDelete((number*) &tmp, r);
    mpz_mod(erg, erg, r->modNumber);
    return (number)erg;
  }
}

static number nrnMod(number a, number b, const coeffs r)
{
  /*
    We need to return the number rr which is uniquely determined by the
    following two properties:
      (1) 0 <= rr < |b| (with respect to '<' and '<=' performed in Z x Z)
      (2) There exists some k in the integers Z such that a = k * b + rr.
    Consider g := gcd(n, |b|). Note that then |b|/g is a unit in Z/n.
    Now, there are three cases:
      (a) g = 1
          Then |b| is a unit in Z/n, i.e. |b| (and also b) divides a.
          Thus rr = 0.
      (b) g <> 1 and g divides a
          Then a = (a/g) * (|b|/g)^(-1) * b (up to sign), i.e. again rr = 0.
      (c) g <> 1 and g does not divide a
          Then denote the division with remainder of a by g as this:
          a = s * g + t. Then t = a - s * g = a - s * (|b|/g)^(-1) * |b|
          fulfills (1) and (2), i.e. rr := t is the correct result. Hence
          in this third case, rr is the remainder of division of a by g in Z.
     Remark: according to mpz_mod: a,b are always non-negative
  */
  mpz_ptr g = (mpz_ptr)omAllocBin(gmp_nrz_bin);
  mpz_ptr rr = (mpz_ptr)omAllocBin(gmp_nrz_bin);
  mpz_init(g);
  mpz_init_set_ui(rr, 0);
  mpz_gcd(g, (mpz_ptr)r->modNumber, (mpz_ptr)b); // g is now as above
  if (mpz_cmp_si(g, 1L) != 0) mpz_mod(rr, (mpz_ptr)a, g); // the case g <> 1
  mpz_clear(g);
  omFreeBin(g, gmp_nrz_bin);
  return (number)rr;
}

static number nrnIntDiv(number a, number b, const coeffs r)
{
  mpz_ptr erg = (mpz_ptr)omAllocBin(gmp_nrz_bin);
  mpz_init(erg);
  mpz_tdiv_q(erg, (mpz_ptr)a, (mpz_ptr)b);
  return (number)erg;
}

/* CF: note that Z/nZ has (at least) two distinct euclidean structures
 * 1st phi(a) := (a mod n) which is just the structure directly
 *     inherited from Z
 * 2nd phi(a) := gcd(a, n)
 * The 1st version is probably faster as everything just comes from Z,
 * but the 2nd version behaves nicely wrt. to quotient operations
 * and HNF and such. In agreement with nrnMod we imlement the 2nd here
 *
 * For quotrem note that if b exactly divides a, then
 *   min(v_p(a), v_p(n))  >= min(v_p(b), v_p(n))
 * so if we divide  a and b by g:= gcd(a,b,n), then   b becomes a
 * unit mod n/g.
 * Thus we 1st compute the remainder (similar to nrnMod) and then
 * the exact quotient.
 */
static number nrnQuotRem(number a, number b, number  * rem, const coeffs r)
{
  mpz_t g, aa, bb;
  mpz_ptr qq = (mpz_ptr)omAllocBin(gmp_nrz_bin);
  mpz_ptr rr = (mpz_ptr)omAllocBin(gmp_nrz_bin);
  mpz_init(qq);
  mpz_init(rr);
  mpz_init(g);
  mpz_init_set(aa, (mpz_ptr)a);
  mpz_init_set(bb, (mpz_ptr)b);

  mpz_gcd(g, bb, r->modNumber);
  mpz_mod(rr, aa, g);
  mpz_sub(aa, aa, rr);
  mpz_gcd(g, aa, g);
  mpz_div(aa, aa, g);
  mpz_div(bb, bb, g);
  mpz_div(g, r->modNumber, g);
  mpz_invert(g, bb, g);
  mpz_mul(qq, aa, g);
  if (rem)
    *rem = (number)rr;
  else {
    mpz_clear(rr);
    omFreeBin(rr, gmp_nrz_bin);
  }
  mpz_clear(g);
  mpz_clear(aa);
  mpz_clear(bb);
  return (number) qq;
}

/*
 * Helper function for computing the module
 */

STATIC_VAR mpz_ptr nrnMapCoef = NULL;

static number nrnMapModN(number from, const coeffs /*src*/, const coeffs dst)
{
  return nrnMult(from, (number) nrnMapCoef, dst);
}

static number nrnMap2toM(number from, const coeffs /*src*/, const coeffs dst)
{
  mpz_ptr erg = (mpz_ptr)omAllocBin(gmp_nrz_bin);
  mpz_init(erg);
  mpz_mul_ui(erg, nrnMapCoef, (unsigned long)from);
  mpz_mod(erg, erg, dst->modNumber);
  return (number)erg;
}

static number nrnMapZp(number from, const coeffs /*src*/, const coeffs dst)
{
  mpz_ptr erg = (mpz_ptr)omAllocBin(gmp_nrz_bin);
  mpz_init(erg);
  // TODO: use npInt(...)
  mpz_mul_si(erg, nrnMapCoef, (unsigned long)from);
  mpz_mod(erg, erg, dst->modNumber);
  return (number)erg;
}

number nrnMapGMP(number from, const coeffs /*src*/, const coeffs dst)
{
  mpz_ptr erg = (mpz_ptr)omAllocBin(gmp_nrz_bin);
  mpz_init(erg);
  mpz_mod(erg, (mpz_ptr)from, dst->modNumber);
  return (number)erg;
}

static number nrnMapQ(number from, const coeffs src, const coeffs dst)
{
  mpz_ptr erg = (mpz_ptr)omAllocBin(gmp_nrz_bin);
  nlMPZ(erg, from, src);
  mpz_mod(erg, erg, dst->modNumber);
  return (number)erg;
}

#if SI_INTEGER_VARIANT==3
static number nrnMapZ(number from, const coeffs /*src*/, const coeffs dst)
{
  mpz_ptr erg = (mpz_ptr)omAllocBin(gmp_nrz_bin);
  if (n_Z_IS_SMALL(from))
    mpz_init_set_si(erg, SR_TO_INT(from));
  else
    mpz_init_set(erg, (mpz_ptr) from);
  mpz_mod(erg, erg, dst->modNumber);
  return (number)erg;
}
#elif SI_INTEGER_VARIANT==2

static number nrnMapZ(number from, const coeffs src, const coeffs dst)
{
  if (SR_HDL(from) & SR_INT)
  {
    long f_i=SR_TO_INT(from);
    return nrnInit(f_i,dst);
  }
  return nrnMapGMP(from,src,dst);
}
#elif SI_INTEGER_VARIANT==1
static number nrnMapZ(number from, const coeffs src, const coeffs dst)
{
  return nrnMapQ(from,src,dst);
}
#endif
void nrnWrite (number a, const coeffs cf)
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
    if (cf->is_field)
    {
      mpz_t ch2; mpz_init_set(ch2, cf->modBase);
      mpz_sub_ui(ch2,ch2,1);
      mpz_divexact_ui(ch2,ch2,2);
      if ((mpz_cmp_ui(cf->modBase,2)!=0) && (mpz_cmp(ch2,(mpz_ptr)a)<0))
      {
        mpz_sub(ch2,(mpz_ptr)a,cf->modBase);
        z=mpz_get_str(s,10,ch2);
        StringAppendS(z);
      }
      else
      {
        z=mpz_get_str(s,10,(mpz_ptr) a);
        StringAppendS(z);
      }
      mpz_clear(ch2);
    }
    else
    {
      z=mpz_get_str(s,10,(mpz_ptr) a);
      StringAppendS(z);
    }
    omFreeSize((ADDRESS)s,l);
  }
}

nMapFunc nrnSetMap(const coeffs src, const coeffs dst)
{
  /* dst = nrn */
  if ((src->rep==n_rep_gmp) && nCoeff_is_Z(src))
  {
    return nrnMapZ;
  }
  if ((src->rep==n_rep_gap_gmp) /*&& nCoeff_is_Z(src)*/)
  {
    return nrnMapZ;
  }
  if (src->rep==n_rep_gap_rat) /*&& nCoeff_is_Q(src)) or Z*/
  {
    return nrnMapQ;
  }
  // Some type of Z/n ring / field
  if (nCoeff_is_Zn(src) || nCoeff_is_Ring_PtoM(src) ||
      nCoeff_is_Ring_2toM(src) || nCoeff_is_Zp(src))
  {
    if (   (!nCoeff_is_Zp(src))
        && (mpz_cmp(src->modBase, dst->modBase) == 0)
        && (src->modExponent == dst->modExponent)) return ndCopyMap;
    else
    {
      mpz_ptr nrnMapModul = (mpz_ptr) omAllocBin(gmp_nrz_bin);
      // Computing the n of Z/n
      if (nCoeff_is_Zp(src))
      {
        mpz_init_set_si(nrnMapModul, src->ch);
      }
      else
      {
        mpz_init(nrnMapModul);
        mpz_set(nrnMapModul, src->modNumber);
      }
      // nrnMapCoef = 1 in dst       if dst is a subring of src
      // nrnMapCoef = 0 in dst / src if src is a subring of dst
      if (nrnMapCoef == NULL)
      {
        nrnMapCoef = (mpz_ptr) omAllocBin(gmp_nrz_bin);
        mpz_init(nrnMapCoef);
      }
      if (mpz_divisible_p(nrnMapModul, dst->modNumber))
      {
        mpz_set_ui(nrnMapCoef, 1);
      }
      else
      if (mpz_divisible_p(dst->modNumber,nrnMapModul))
      {
        mpz_divexact(nrnMapCoef, dst->modNumber, nrnMapModul);
        mpz_ptr tmp = dst->modNumber;
        dst->modNumber = nrnMapModul;
        if (!nrnIsUnit((number) nrnMapCoef,dst))
        {
          dst->modNumber = tmp;
          nrnDelete((number*) &nrnMapModul, dst);
          return NULL;
        }
        mpz_ptr inv = (mpz_ptr) nrnInvers((number) nrnMapCoef,dst);
        dst->modNumber = tmp;
        mpz_mul(nrnMapCoef, nrnMapCoef, inv);
        mpz_mod(nrnMapCoef, nrnMapCoef, dst->modNumber);
        nrnDelete((number*) &inv, dst);
      }
      else
      {
        nrnDelete((number*) &nrnMapModul, dst);
        return NULL;
      }
      nrnDelete((number*) &nrnMapModul, dst);
      if (nCoeff_is_Ring_2toM(src))
        return nrnMap2toM;
      else if (nCoeff_is_Zp(src))
        return nrnMapZp;
      else
        return nrnMapModN;
    }
  }
  return NULL;      // default
}

/*
 * set the exponent (allocate and init tables) (TODO)
 */

static void nrnSetExp(unsigned long m, coeffs r)
{
  /* clean up former stuff */
  if (r->modNumber != NULL) mpz_clear(r->modNumber);

  r->modExponent= m;
  r->modNumber = (mpz_ptr)omAllocBin(gmp_nrz_bin);
  mpz_init_set (r->modNumber, r->modBase);
  mpz_pow_ui (r->modNumber, r->modNumber, m);
}

/* We expect this ring to be Z/n^m for some m > 0 and for some n > 2 which is not a prime. */
static void nrnInitExp(unsigned long m, coeffs r)
{
  nrnSetExp(m, r);
  assume (r->modNumber != NULL);
//CF: in general, the modulus is computed somewhere. I don't want to
//  check it's size before I construct the best ring.
//  if (mpz_cmp_ui(r->modNumber,2) <= 0)
//    WarnS("nrnInitExp failed (m in Z/m too small)");
}

#ifdef LDEBUG
BOOLEAN nrnDBTest (number a, const char *f, const int l, const coeffs r)
{
  if ( (mpz_sgn1((mpz_ptr) a) < 0) || (mpz_cmp((mpz_ptr) a, r->modNumber) > 0) )
  {
    Warn("mod-n: out of range at %s:%d\n",f,l);
    return FALSE;
  }
  return TRUE;
}
#endif

/*2
* extracts a long integer from s, returns the rest    (COPY FROM longrat0.cc)
*/
static const char * nlCPEatLongC(char *s, mpz_ptr i)
{
  const char * start=s;
  if (!(*s >= '0' && *s <= '9'))
  {
    mpz_init_set_ui(i, 1);
    return s;
  }
  mpz_init(i);
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

static const char * nrnRead (const char *s, number *a, const coeffs r)
{
  mpz_ptr z = (mpz_ptr) omAllocBin(gmp_nrz_bin);
  {
    s = nlCPEatLongC((char *)s, z);
  }
  mpz_mod(z, z, r->modNumber);
  if ((*s)=='/')
  {
    mpz_ptr n = (mpz_ptr) omAllocBin(gmp_nrz_bin);
    s++;
    s=nlCPEatLongC((char*)s,n);
    if (!nrnIsOne((number)n,r))
    {
      *a=nrnDiv((number)z,(number)n,r);
      mpz_clear(z);
      omFreeBin((void *)z, gmp_nrz_bin);
      mpz_clear(n);
      omFreeBin((void *)n, gmp_nrz_bin);
    }
  }
  else
    *a = (number) z;
  return s;
}

static number nrnConvFactoryNSingN( const CanonicalForm n, const coeffs r)
{
  return nrnInit(n.intval(),r);
}

static CanonicalForm nrnConvSingNFactoryN( number n, BOOLEAN setChar, const coeffs r )
{
  if (setChar) setCharacteristic( r->ch );
   return CanonicalForm(nrnInt( n,r ));
}

/* for initializing function pointers */
BOOLEAN nrnInitChar (coeffs r, void* p)
{
  assume( (getCoeffType(r) == n_Zn) || (getCoeffType (r) == n_Znm) );
  ZnmInfo * info= (ZnmInfo *) p;
  r->modBase= (mpz_ptr)nrnCopy((number)info->base, r); //this circumvents the problem
  //in bigintmat.cc where we cannot create a "legal" nrn that can be freed.
  //If we take a copy, we can do whatever we want.

  nrnInitExp (info->exp, r);

  /* next computation may yield wrong characteristic as r->modNumber
     is a GMP number */
  r->ch = mpz_get_ui(r->modNumber);

  r->is_field=FALSE;
  r->is_domain=FALSE;
  r->rep=n_rep_gmp;

  r->cfInit        = nrnInit;
  r->cfDelete      = nrnDelete;
  r->cfCopy        = nrnCopy;
  r->cfSize        = nrnSize;
  r->cfInt         = nrnInt;
  r->cfAdd         = nrnAdd;
  r->cfSub         = nrnSub;
  r->cfMult        = nrnMult;
  r->cfDiv         = nrnDiv;
  r->cfAnn         = nrnAnn;
  r->cfIntMod      = nrnMod;
  r->cfExactDiv    = nrnDiv;
  r->cfInpNeg      = nrnNeg;
  r->cfInvers      = nrnInvers;
  r->cfDivBy       = nrnDivBy;
  r->cfDivComp     = nrnDivComp;
  r->cfGreater     = nrnGreater;
  r->cfEqual       = nrnEqual;
  r->cfIsZero      = nrnIsZero;
  r->cfIsOne       = nrnIsOne;
  r->cfIsMOne      = nrnIsMOne;
  r->cfGreaterZero = nrnGreaterZero;
  r->cfWriteLong   = nrnWrite;
  r->cfRead        = nrnRead;
  r->cfPower       = nrnPower;
  r->cfSetMap      = nrnSetMap;
  //r->cfNormalize   = ndNormalize;
  r->cfLcm         = nrnLcm;
  r->cfGcd         = nrnGcd;
  r->cfIsUnit      = nrnIsUnit;
  r->cfGetUnit     = nrnGetUnit;
  r->cfExtGcd      = nrnExtGcd;
  r->cfXExtGcd     = nrnXExtGcd;
  r->cfQuotRem     = nrnQuotRem;
  r->cfCoeffName   = nrnCoeffName;
  r->nCoeffIsEqual = nrnCoeffIsEqual;
  r->cfKillChar    = nrnKillChar;
  r->cfQuot1       = nrnQuot1;
#if SI_INTEGER_VARIANT==2
  r->cfWriteFd     = nrzWriteFd;
  r->cfReadFd      = nrzReadFd;
#endif

#ifdef LDEBUG
  r->cfDBTest      = nrnDBTest;
#endif
  if ((r->modExponent==1)&&(mpz_size1(r->modBase)==1))
  {
    long p=mpz_get_si(r->modBase);
    if ((p<=FACTORY_MAX_PRIME)&&(p==IsPrime(p))) /*factory limit: <2^29*/
    {
      r->convFactoryNSingN=nrnConvFactoryNSingN;
      r->convSingNFactoryN=nrnConvSingNFactoryN;
    }
  }
  return FALSE;
}

#endif
/* #ifdef HAVE_RINGS */
