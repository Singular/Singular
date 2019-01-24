#include "kernel/mod2.h" // general settings/macros
//#include "kernel/febase.h"  // for Print, WerrorS
#include "Singular/ipid.h" // for SModulFunctions, leftv
#include "Singular/number2.h" // for SModulFunctions, leftv
#include "coeffs/numbers.h" // nRegister, coeffs.h
#include "coeffs/coeffs.h"
#include "Singular/blackbox.h" // blackbox type
#include "nforder.h"
#include "coeffs/bigintmat.h"


#ifdef SINGULAR_4_2
EXTERN_VAR n_coeffType nforder_type;

static void WriteRing(const coeffs r, BOOLEAN details)
{
  ((nforder *)r->data)->Print();
}

static char* CoeffString(const coeffs r)
{
  return ((nforder *)r->data)->String();
}
static void EltWrite(number a, const coeffs r)
{
  bigintmat * b = (bigintmat*)a;
  if (a) {
    bigintmat * c = b->transpose();
    c->Write();
    StringAppendS("^t ");
  } else {
    StringAppendS("(Null)\n");
  }
}

number EltCreateMat(nforder *a, bigintmat *b)
{
  number xx;
  if (b->rows()==1) {
    assume(b->cols()==a->getDim());
    xx = (number) b->transpose();
  } else {
    assume(b->rows() == a->getDim());
    assume(b->cols() == 1);
    xx = (number) new bigintmat((bigintmat*)b);
  }
//  Print("Created new element %lx from %lx\n", xx, b);
  return (number) xx;
}


static BOOLEAN order_cmp(coeffs n, n_coeffType t, void*parameter)
{
  return (t==nforder_type) && (n->data == parameter);
}

static void KillChar(coeffs r) {
  Print("KillChar %lx\n", r);
}
#ifdef LDEBUG
  BOOLEAN EltDBTest(number, const char *, const int, const coeffs)
{
    return TRUE;
}
#endif

static void SetChar(const coeffs r)
{
  Print("%s called\n", __func__);
}
                                // or NULL
   // general stuff
static number EltMult(number a, number b, const coeffs r)
{
  nforder *O = (nforder*) (r->data);
  bigintmat *c = new bigintmat((bigintmat*)a);
  O->elMult(c, (bigintmat*) b);
  return (number) c;
}
static number EltSub(number a, number b, const coeffs r)
{
  nforder *O = (nforder*) (r->data);
  bigintmat *c = new bigintmat((bigintmat*)a);
  O->elSub(c, (bigintmat*) b);
  return (number) c;
}
static number EltAdd(number a, number b, const coeffs r)
{
  nforder *O = (nforder*) (r->data);
  bigintmat *c = new bigintmat((bigintmat*)a);
  O->elAdd(c, (bigintmat*) b);
  return (number) c;
}
static number EltDiv(number a, number b, const coeffs r)
{
  Werror("%s called\n", __func__, a, b, r);
  return NULL;
}
static number EltIntDiv(number a, number b, const coeffs r)
{
  Werror("IntDiv called on order elts", a, b, r);
  return NULL;
}
static number EltIntMod(number a, number b, const coeffs r)
{
  Werror("IntMod called on order elts", a, b, r);
  return NULL;
}
static number EltExactDiv(number a, number b, const coeffs r)
{
  Werror("%s called\n", __func__, a, b, r);
  return NULL;
}
   /// init with an integer
static number  EltInit(long i,const coeffs r)

{
  nforder * O = (nforder*) r->data;
  if (!O) return NULL; //during init, this seems to be called with O==NULL
  coeffs C = O->basecoeffs();
  bigintmat * b = new bigintmat(O->getDim(), 1, C);
  if (O->oneIsOne()) {
    basis_elt(b, 1);
    number I = n_Init(i, C);
    b->skalmult(I, C);
    n_Delete(&I, C);
    return (number) b;
  } else
    return NULL;
}

   /// init with a GMP integer
static number  EltInitMPZ(mpz_t i, const coeffs r)

{
  Werror("%s called\n", __func__);
  return NULL;
}
   /// how complicated, (0) => 0, or positive
static int EltSize(number n, const coeffs r)

{
  Werror("%s called\n", __func__);
  return NULL;
}
   /// convertion to int, 0 if impossible
static long EltInt(number &n, const coeffs r)

{
  Werror("%s called\n", __func__);
  return NULL;
}
   /// Converts a non-negative number n into a GMP number, 0 if impossible
static void EltMPZ(mpz_t result, number &n, const coeffs r)

{
  Werror("%s called\n", __func__);
}
   /// changes argument  inline: a:= -a
   /// return -a! (no copy is returned)
   /// the result should be assigned to the original argument: e.g. a = n_Neg(a,r)
static number  EltNeg(number a, const coeffs r)
   /// return -a
{
  Werror("%s called\n", __func__);
  return NULL;
}
static number  EltInvers(number a, const coeffs r)
   /// return 1/a
{
  Werror("%s called\n", __func__);
  return NULL;
}
static number  EltCopy(number a, const coeffs r)
{
  return EltCreateMat((nforder*)r->data, (bigintmat*)a);
}

static const char * EltRead(const char * s, number * a, const coeffs r)
{
//  Print("%s called with ->%s-<\n", __func__, s);
  return s;
}

static BOOLEAN EltEqual(number a,number b, const coeffs r)
{
  Print("%s called\n", __func__, a, b, r);
  return 0;
}
static BOOLEAN EltGreater(number a,number b, const coeffs r)
{
  Print("%s called\n", __func__, a, b, r);
  return 0;
}
static BOOLEAN EltIsOne(number a, const coeffs r)
{
//  Print("%s called\n", __func__, a, r);
  return 0;
}
static BOOLEAN EltIsMOne(number a, const coeffs r)
{
//  Print("%s called\n", __func__, a, r);
  return 0;
}
static BOOLEAN EltGreaterZero(number a, const coeffs r)
{
//  Print("%s called\n", __func__, a, r);
  return 1;
}
static BOOLEAN EltIsZero(number a, const coeffs r)
{
  return (a==NULL) || ((bigintmat*)a)->isZero();
}

static nMapFunc EltSetMap(const coeffs src, const coeffs dst)
{
  Print("%s called\n", __func__, src, dst);
  return NULL;
}

static void EltDelete(number * a, const coeffs r)
{
//  Print("Deleting %lx\n%s\n", *a, (((bigintmat*)(*a))->String()));

  delete (bigintmat*)(*a);
  *a = NULL;
}

BOOLEAN n_nfOrderInit(coeffs r,  void * parameter)
{
  assume( getCoeffType(r) == nforder_type );
  r->nCoeffIsEqual=order_cmp;
  r->cfKillChar = KillChar;
  r->cfSetChar = SetChar;
  r->cfCoeffString=CoeffString;
  r->cfCoeffWrite=WriteRing;
  r->cfWriteShort=EltWrite;
  r->cfInit = EltInit;
  r->cfMult = EltMult;
  r->cfSub = EltSub;
  r->cfAdd = EltAdd;
  r->cfDiv = EltDiv;
  r->cfExactDiv = EltExactDiv;
  r->cfInitMPZ = EltInitMPZ;
  r->cfSize = EltSize;
  r->cfInt = EltInt;
  r->cfMPZ = EltMPZ;
  r->cfInpNeg = EltNeg;
  r->cfInvers = EltInvers;
  r->cfCopy = EltCopy;
  r->data = parameter;

  r->cfWriteLong = EltWrite;
  r->cfRead =EltRead;
  r->cfGreater = EltGreater;
  r->cfEqual = EltEqual;
  r->cfIsZero = EltIsZero;
  r->cfIsOne = EltIsOne;
  r->cfIsMOne = EltIsMOne;
  r->cfGreaterZero = EltGreaterZero;
  r->cfDelete = EltDelete;
  r->cfSetMap = EltSetMap;
#ifdef LDEBUG
  r->cfDBTest = EltDBTest;
#endif
  return FALSE;
}
#endif /* SINGULAR_4_2 */


