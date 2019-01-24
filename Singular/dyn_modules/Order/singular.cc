#include "kernel/mod2.h" // general settings/macros
#include "Singular/mod_lib.h"
//#include "kernel/febase.h"  // for Print, WerrorS
#include "Singular/ipid.h" // for SModulFunctions, leftv
#include "Singular/number2.h" // for SModulFunctions, leftv
#include "coeffs/numbers.h" // nRegister, coeffs.h
#include "coeffs/coeffs.h"
#include "Singular/blackbox.h" // blackbox type
#include "nforder.h"
#include "nforder_elt.h"
#include "nforder_ideal.h"
#include "coeffs/bigintmat.h"

#ifdef SINGULAR_4_2
STATIC_VAR int nforder_type_id=0;
VAR n_coeffType nforder_type =n_unknown;

// coeffs stuff: -----------------------------------------------------------
STATIC_VAR coeffs nforder_AE=NULL;
static void nforder_Register()
{
  puts("nforder_Register called");
  nforder_type=nRegister(n_unknown,n_nfOrderInit);
  nforder_AE=nInitChar(nforder_type,NULL);
}
// black box stuff: ---------------------------------------------------------
static void * nforder_ideal_Init(blackbox */*b*/)
{
  nforder_AE->ref++;
  return nforder_AE;
}
static char * nforder_ideal_String(blackbox *b, void *d)
{
  StringSetS("");
  if (d) ((nforder_ideal *)d)->Write();
  else StringAppendS("o not defined o");
  return StringEndS();
}
static void * nforder_ideal_Copy(blackbox* /*b*/, void *d)
{ return new nforder_ideal((nforder_ideal*)d, 1);}

static BOOLEAN nforder_ideal_Assign(leftv l, leftv r)
{
  if (l->Typ()==r->Typ())
  {
    if (l->rtyp==IDHDL)
    {
      IDDATA((idhdl)l->data)=(char *)nforder_ideal_Copy((blackbox*)NULL, r->data);
    }
    else
    {
      l->data=(char *)nforder_ideal_Copy((blackbox*)NULL, r->data);
    }
    return FALSE;
  }
  return TRUE;
}
static void nforder_ideal_destroy(blackbox * /*b*/, void *d)
{
  if (d!=NULL)
  {
    delete (nforder_ideal*)d;
  }
}

BOOLEAN checkArgumentIsOrder(leftv arg, nforder * cmp, nforder ** result)
{
  if (arg->Typ() != CRING_CMD) return FALSE;
  coeffs R = (coeffs) arg->Data();
  if (getCoeffType(R) != nforder_type) return FALSE;
  nforder * O = (nforder*) R->data;
  if (cmp && cmp != O) return FALSE;
  *result = O;
  return TRUE;
}

BOOLEAN checkArgumentIsBigintmat(leftv arg, coeffs r, bigintmat ** result)
{
  if (arg->Typ() != BIGINTMAT_CMD) return FALSE;
  bigintmat * b = (bigintmat*) arg->Data();
  if (r && b->basecoeffs() != r) return FALSE;
  *result = b;
  return TRUE;
}

BOOLEAN checkArgumentIsNumber2(leftv arg, coeffs r, number2 * result)
{
  if (arg->Typ() != CNUMBER_CMD) return FALSE;
  number2 b = (number2) arg->Data();
  if (r && b->cf != r) return FALSE;
  *result = b;
  return TRUE;
}


BOOLEAN checkArgumentIsNFOrderIdeal(leftv arg, coeffs r, nforder_ideal ** result)
{
  if (arg->Typ() != nforder_type_id) return FALSE;
  *result = (nforder_ideal *) arg->Data();
  if (r && (*result)->order() != r) return FALSE;
  return TRUE;
}

BOOLEAN checkArgumentIsInt(leftv arg, int* result)
{
  if (arg->Typ() != INT_CMD) return FALSE;
  *result = (long) arg->Data();
  return TRUE;
}

BOOLEAN checkArgumentIsBigint(leftv arg, number* result)
{
  switch (arg->Typ()) {
    case BIGINT_CMD:
      *result = (number)arg->Data();
      return TRUE;
      break;
    case NUMBER_CMD:
      if (currRing->cf == coeffs_BIGINT &&
          getCoeffType(coeffs_BIGINT) == n_Z) {
        *result = (number)arg->Data();
        return TRUE;
      } else
        return FALSE;
      break;
    case CNUMBER_CMD:
      {
        number2 n = (number2)arg->Data();
        if (getCoeffType(n->cf) == n_Z) {
          *result = n->n;
          return TRUE;
        }
        return FALSE;
        break;
      }
    default:
      return FALSE;
  }
}

static BOOLEAN nforder_ideal_Op2(int op,leftv l, leftv r1, leftv r2)
{
  Print("Types are %d %d\n", r1->Typ(), r2->Typ());
  number2 e;
  int f;
  nforder_ideal *I, *J, *H;
  switch (op) {
    case '+':
      {
      if (!checkArgumentIsNFOrderIdeal(r1, NULL, &I))
        return TRUE;
      if (!checkArgumentIsNFOrderIdeal(r2, I->order(), &J))
        return TRUE;
      H = nf_idAdd(I, J);
      break;
      }
    case '*':
      {
      if (!checkArgumentIsNFOrderIdeal(r1, NULL, &I)) {
        leftv r = r1;
        r1 = r2;
        r2 = r; //at least ONE argument has to be an ideal
      }
      if (!checkArgumentIsNFOrderIdeal(r1, NULL, &I))
        return TRUE;
      if (checkArgumentIsNFOrderIdeal(r2, I->order(), &J)) {
        H = nf_idMult(I, J);
      } else if (checkArgumentIsNumber2(r2, I->order(), &e)) {
        H = nf_idMult(I, e->n);
      } else if (checkArgumentIsInt(r2, &f)) {
        H = nf_idMult(I, f);
      } else
        return TRUE;
      break;
      }
    case '^':
      {
        if (!checkArgumentIsNFOrderIdeal(r1, NULL, &I))
          return TRUE;
        if (!checkArgumentIsInt(r2, &f))
          return TRUE;
        H = nf_idPower(I, f);
        break;
      }
    default:
      return TRUE;
  }
  l->rtyp = nforder_type_id;
  l->data = (void*)H;
  return FALSE;
}
static BOOLEAN nforder_ideal_bb_setup()
{
  blackbox *b=(blackbox*)omAlloc0(sizeof(blackbox));
  // all undefined entries will be set to default in setBlackboxStuff
  // the default Print is quite useful,
  // all other are simply error messages
  b->blackbox_destroy=nforder_ideal_destroy;
  b->blackbox_String=nforder_ideal_String;
  //b->blackbox_Print=blackbox_default_Print;
  b->blackbox_Init=nforder_ideal_Init;
  b->blackbox_Copy=nforder_ideal_Copy;
  b->blackbox_Assign=nforder_ideal_Assign;
  //b->blackbox_Op1=blackbox_default_Op1;
  b->blackbox_Op2=nforder_ideal_Op2;
  //b->blackbox_Op3=blackbox_default_Op3;
  //b->blackbox_OpM=blackbox_default_OpM;
  nforder_type_id = setBlackboxStuff(b,"NFOrderIdeal");
  Print("setup: created a blackbox type [%d] '%s'",nforder_type_id, getBlackboxName(nforder_type_id));
  PrintLn();
  return FALSE; // ok, TRUE = error!
}

// module stuff: ------------------------------------------------------------

BOOLEAN checkBigintmatDim(bigintmat * b, int r, int c)
{
  if (b->rows() != r) return FALSE;
  if (b->cols() != c) return FALSE;
  return TRUE;
}

#define returnNumber(_res, _n, _R) \
  do {                                                          \
    number2 _r = (number2)omAlloc(sizeof(struct snumber2));     \
    _r->n = _n;                                                 \
    _r->cf = _R;                                                \
    _res->rtyp = CNUMBER_CMD;                                   \
    _res->data = _r;                                            \
  } while (0)


static BOOLEAN build_ring(leftv result, leftv arg)
{
  nforder *o;
  if (arg->Typ() == LIST_CMD) {
    lists L = (lists)arg->Data();
    int n = lSize(L)+1;
    bigintmat **multtable = (bigintmat**)omAlloc(n*sizeof(bigintmat*));
    for(int i=0; i<n; i++) {
      multtable[i] = (bigintmat*)(L->m[i].Data());
    }
    o = new nforder(n, multtable, nInitChar(n_Z, 0));
    omFree(multtable);
  } else {
    assume(arg->Typ() == INT_CMD);
    int dimension = (int)(long)arg->Data();

    bigintmat **multtable = (bigintmat**)omAlloc(dimension*sizeof(bigintmat*));
    arg = arg->next;
    for (int i=0; i<dimension; i++) {
      multtable[i] = new bigintmat((bigintmat*)arg->Data());
      arg = arg->next;
    }
    o = new nforder(dimension, multtable, nInitChar(n_Z, 0));
    for (int i=0; i<dimension; i++) {
      delete multtable[i];
    }
    omFree(multtable);
  }
  result->rtyp=CRING_CMD; // set the result type
  result->data=(char*)nInitChar(nforder_type, o);// set the result data

  return FALSE;
}

static BOOLEAN ideal_from_mat(leftv result, leftv arg)
{
  nforder * O;
  if (!checkArgumentIsOrder(arg, NULL, &O)) {
    WerrorS("usage: IdealFromMat(order, basis matrix)");
    return TRUE;
  }
  arg = arg->next;
  bigintmat *b;
  if (!checkArgumentIsBigintmat(arg, O->basecoeffs(), &b)) {
    WerrorS("3:usage: IdealFromMat(order, basis matrix)");
    return TRUE;
  }
  result->rtyp = nforder_type_id;
  result->data = new nforder_ideal(b, nInitChar(nforder_type, O));
  return FALSE;
}


static BOOLEAN elt_from_mat(leftv result, leftv arg)
{
  nforder * O;
  if (!checkArgumentIsOrder(arg, NULL, &O)) {
    WerrorS("usage: EltFromMat(order, matrix)");
    return TRUE;
  }
  arg = arg->next;
  bigintmat *b;
  if (!checkArgumentIsBigintmat(arg, O->basecoeffs(), &b)) {
    WerrorS("2:usage: EltFromMat(order, matrix)");
    return TRUE;
  }
  returnNumber(result, (number)EltCreateMat(O, b), nInitChar(nforder_type, O));
  return FALSE;
}

static BOOLEAN discriminant(leftv result, leftv arg)
{
  nforder * O;
  if (!checkArgumentIsOrder(arg, NULL, &O)) {
    WerrorS("usage: Discriminant(order)");
    return TRUE;
  }
  O->calcdisc();

  returnNumber(result, O->getDisc(), O->basecoeffs());
  return FALSE;
}

static BOOLEAN pMaximalOrder(leftv result, leftv arg)
{
  nforder * o;
  if (!checkArgumentIsOrder(arg, NULL, &o)) {
    WerrorS("usage: pMaximalOrder(order, int)");
    return TRUE;
  }
  arg = arg->next;
  long p = (int)(long)arg->Data();
  number P = n_Init(p, o->basecoeffs());

  nforder *op = pmaximal(o, P);

  result->rtyp=CRING_CMD; // set the result type
  result->data=(char*)nInitChar(nforder_type, op);// set the result data
  assume(result->data);

  return FALSE;
}

static BOOLEAN oneStep(leftv result, leftv arg)
{
  assume (arg->Typ()==CRING_CMD);
  coeffs c = (coeffs)arg->Data();
  assume (c->type == nforder_type);
  nforder * o = (nforder*)c->data;
  arg = arg->next;
  long p = (int)(long)arg->Data();
  number P = n_Init(p, o->basecoeffs());

  nforder *op = onestep(o, P, o->basecoeffs());

  result->rtyp=CRING_CMD; // set the result type
  result->data=(char*)nInitChar(nforder_type, op);// set the result data

  return FALSE;
}

static BOOLEAN nforder_simplify(leftv result, leftv arg)
{
  nforder * o;
  if (!checkArgumentIsOrder(arg, NULL, &o)) {
    WerrorS("usage: NFOrderSimplify(order)");
    return TRUE;
  }
  nforder *op = o->simplify();

  result->rtyp=CRING_CMD; // set the result type
  result->data=(char*)nInitChar(nforder_type, op);// set the result data

  return FALSE;
}

static BOOLEAN eltTrace(leftv result, leftv arg)
{
  number2 a;
  if (!checkArgumentIsNumber2(arg, NULL, &a)) {
    WerrorS("EltTrace(elt)");
    return TRUE;
  }
  coeffs  c = a->cf;
  if (getCoeffType(c) != nforder_type) {
    WerrorS("EltTrace(elt in order)");
    return TRUE;
  }
  bigintmat * aa = (bigintmat*)a->n;
  nforder * o = (nforder*)c->data;
  number t = o->elTrace(aa);
  returnNumber(result, t, o->basecoeffs());
  return FALSE;
}

static BOOLEAN eltNorm(leftv result, leftv arg)
{
  number2 a;
  if (!checkArgumentIsNumber2(arg, NULL, &a)) {
    WerrorS("EltNorm(elt)");
    return TRUE;
  }
  coeffs  c = a->cf;
  if (getCoeffType(c) != nforder_type) {
    WerrorS("EltNorm(elt in order)");
    return TRUE;
  }
  bigintmat * aa = (bigintmat*)a->n;
  nforder * o = (nforder*)c->data;
  number t = o->elNorm(aa);
  returnNumber(result, t, o->basecoeffs());
  return FALSE;
}

static BOOLEAN eltRepMat(leftv result, leftv arg)
{
  assume (arg->Typ()==CNUMBER_CMD);
  number2 a = (number2) arg->Data();
  coeffs  c = a->cf;
  bigintmat * aa = (bigintmat*)a->n;
  assume (c->type == nforder_type);
  nforder * o = (nforder*)c->data;
  bigintmat* t = o->elRepMat(aa);
  result->rtyp = BIGINTMAT_CMD;
  result->data = t;
  return FALSE;
}

static BOOLEAN smithtest(leftv result, leftv arg)
{
  assume (arg->Typ()==BIGINTMAT_CMD);
  bigintmat *a = (bigintmat *) arg->Data();
  arg = arg->next;

  long p = (int)(long)arg->Data();
  number P = n_Init(p, a->basecoeffs());

  bigintmat * A, *B;
  diagonalForm(a, &A, &B);


  result->rtyp = NONE;
  return FALSE;
}


extern "C" int SI_MOD_INIT(Order)(SModulFunctions* psModulFunctions)
{
  nforder_Register();
  nforder_ideal_bb_setup();
  psModulFunctions->iiAddCproc(
          (currPack->libname? currPack->libname: ""),// the library name,
          "nfOrder",// the name for the singular interpreter
          FALSE,  // should not be static
          build_ring); // the C/C++ routine

  psModulFunctions->iiAddCproc(
          (currPack->libname? currPack->libname: ""),// the library name,
          "pMaximalOrder",// the name for the singular interpreter
          FALSE,  // should not be static
          pMaximalOrder); // the C/C++ routine

  psModulFunctions->iiAddCproc(
          (currPack->libname? currPack->libname: ""),// the library name,
          "oneStep",// the name for the singular interpreter
          FALSE,  // should not be static
          oneStep); // the C/C++ routine

  psModulFunctions->iiAddCproc(
          (currPack->libname? currPack->libname: ""),
          "Discriminant",
          FALSE,
          discriminant);

  psModulFunctions->iiAddCproc(
          (currPack->libname? currPack->libname: ""),
          "EltFromMat",
          FALSE,
          elt_from_mat);

  psModulFunctions->iiAddCproc(
          (currPack->libname? currPack->libname: ""),
          "NFOrderSimplify",
          FALSE,
          nforder_simplify);

  psModulFunctions->iiAddCproc(
          (currPack->libname? currPack->libname: ""),
          "EltNorm",
          FALSE,
          eltNorm);

  psModulFunctions->iiAddCproc(
          (currPack->libname? currPack->libname: ""),
          "EltTrace",
          FALSE,
          eltTrace);

  psModulFunctions->iiAddCproc(
          (currPack->libname? currPack->libname: ""),
          "EltRepMat",
          FALSE,
          eltRepMat);

  psModulFunctions->iiAddCproc(
          (currPack->libname? currPack->libname: ""),
          "SmithTest",
          FALSE,
          smithtest);

  psModulFunctions->iiAddCproc(
          (currPack->libname? currPack->libname: ""),
          "IdealFromMat",
          FALSE,
          ideal_from_mat);

  module_help_main(
     (currPack->libname? currPack->libname: "NFOrder"),// the library name,
    "nforder: orders in number fields"); // the help string for the module
  VAR return MAX_TOK;
}
#endif
