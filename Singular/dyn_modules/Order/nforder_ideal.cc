#include "coeffs/bigintmat.h"
#include "nforder.h"
#include "reporter/reporter.h"
#include "coeffs/numbers.h"
#include "coeffs/coeffs.h"
#include "Singular/ipid.h"
#include "nforder_ideal.h"




////////////////////////////////////
//// Konstruktoren/Destruktoren ////
////////////////////////////////////

/*________________0_______________*/
void nforder_ideal::init() {
  memset(this, 0, sizeof(nforder_ideal));
}

nforder_ideal::nforder_ideal(bigintmat * _basis, const coeffs O) {
  init();
  ord = O;
  basis = new bigintmat(_basis);
}

nforder_ideal::nforder_ideal(nforder_ideal *I, int) {
  init();
  ord = I->ord;
  coeffs C = ((nforder *)ord->data)->basecoeffs();
  basis = new bigintmat(I->basis);
  if (I->den) {
    den = n_Copy(I->den, C);
  }
  if (I->norm) {
    norm = n_Copy(I->norm, C);
    norm_den = n_Copy(I->norm_den, C);
  }
  if (I->min) {
    min = n_Copy(I->min, C);
    min_den = n_Copy(I->min_den, C);
  }
}

void nforder_ideal::Write() {
  coeffs C = ((nforder *)ord->data)->basecoeffs();
  if (isFractional()) {
    StringAppendS("Fractional ");
  }
  StringAppend("Ideal with basis:\n");
  basis->Write();
  if (isFractional()) {
    number d;
    StringAppend(" / ");
    n_Write((d = viewBasisDen()), C);
  }
  StringAppendS("\n");
  if (norm) {
    StringAppendS("and norm ");
    n_Write(norm, C);
    StringAppendS(" / ");
    n_Write(norm_den, C);
    StringAppendS(" ");
  }
  if (min) {
    StringAppendS("and min ");
    n_Write(min, C);
    StringAppendS(" / ");
    n_Write(min_den, C);
    StringAppendS(" ");
  }
}

char * nforder_ideal::String() {
  StringSetS("");
  Write();
  return StringEndS();
}
void nforder_ideal::Print() {
  char * s = String();
  PrintS(s);
  PrintS("\n");
  omFree(s);
}

nforder_ideal::~nforder_ideal() {
  if (basis) delete basis;
  coeffs C = ((nforder *)ord->data)->basecoeffs();
  if (den) n_Delete(&den, C);
  if (norm) {
    n_Delete(&norm, C);
    n_Delete(&norm_den, C);
  }
  if (min) {
    n_Delete(&min, C);
    n_Delete(&min_den, C);
  }
}

#ifdef HAVE_RINGS
nforder_ideal * nf_idAdd(nforder_ideal *A, nforder_ideal *B)
{
  assume(A->order() == B->order());
  nforder * O = (nforder*) A->order()->data;
  coeffs C = O->basecoeffs();
  bigintmat * r = new bigintmat(O->getDim(), 2*O->getDim(), C),
            *s1, *s2;
  number den = NULL;
  if (B->isFractional()) {
    s1 = A->getBasis();
    s1->skalmult(B->viewBasisDen(), C);
    den = n_Copy(B->viewBasisDen(), C);
  } else {
    s1 = A->viewBasis();
  }
  if (A->isFractional()) {
    s2 = B->getBasis();
    s2->skalmult(A->viewBasisDen(), C);
    if (den) {
      number d = n_Mult(den, A->viewBasisDen(), C);
      n_Delete(&den, C);
      den = d;
    } else {
      den = n_Copy(A->viewBasisDen(), C);
    }
  } else {
    s2 = B->viewBasis();
  }
  r->concatcol(s1, s2);

  if (A->isFractional())
    delete s2;
  if (B->isFractional())
    delete s1;

  number modA = NULL, modB = NULL;
  if (!(modA = A->viewMin())) {
    modA = A->viewNorm();
  }
  if (!(modB = B->viewMin())) {
    modB = B->viewNorm();
  }
  bigintmat *t2;
  if (modA && modB) {
    number mod = n_Gcd(modA, modB, C);
    t2 = r->modhnf(mod, C);
    n_Delete(&mod, C);
  } else {
    r->hnf();
    t2 = new bigintmat(O->getDim(), O->getDim(), C);
    t2->copySubmatInto(r, 1, O->getDim()+1, O->getDim(), O->getDim(), 1,1);
  }
  delete r;
  if (den) {
    t2->simplifyContentDen(&den);
  }
  nforder_ideal *D = new nforder_ideal(t2, A->order());
  if (den)
    D->setBasisDenTransfer(den);

  if (O->oneIsOne())
    D->setMinTransfer(t2->get(1,1), den ? n_Copy(den, C) : n_Init(1, C));
  D->setNormTransfer(t2->det(), den ? n_Copy(den, C) : n_Init(1, C));
  delete t2;
  return D;
}
#endif

#ifdef HAVE_RINGS
nforder_ideal * nf_idMult(nforder_ideal *A, nforder_ideal *B)
{
  assume(A->order() == B->order());
  nforder * O = (nforder*) A->order()->data;
  coeffs C = O->basecoeffs();
  number den = NULL;

  bigintmat * r= NULL;
  bigintmat * c = new bigintmat(O->getDim(), 1, C),
            *rep = new bigintmat(O->getDim(), O->getDim(), C);
  for(int i=0; i<O->getDim(); i++) {
    A->viewBasis()->getcol(i+1, c);
    O->multmap(c, rep);
    bigintmat * cc = bimMult(rep, B->viewBasis());
    if (r) {
      bigintmat * s = new bigintmat(O->getDim(), r->cols()+O->getDim(), C);
      s->concatcol(r, cc);
      delete r;
      delete cc;
      r = s;
    } else {
      r = cc;
    }
  }
  delete c;

  number modA = NULL, modB = NULL;
  if (!(modA = A->viewMin())) {
    modA = A->viewNorm();
  }
  if (!(modB = B->viewMin())) {
    modB = B->viewNorm();
  }


  bigintmat * t1;
  if (modA && modB) {
    number mod = n_Mult(modA, modB, C);
    t1 = r->modhnf(mod, C);
    n_Delete(&mod, C);
  } else {
    r->hnf();
    t1 = new bigintmat(O->getDim(), O->getDim(), C);
    r->getColRange(r->cols()-O->getDim()+1, O->getDim(), t1);
  }
  delete r;

  if (A->isFractional()) {
    den = A->viewBasisDen();
  }
  if (B->isFractional()) {
    if (den)
      den = n_Mult(den, B->viewBasisDen(), C);
    else
      den = n_Copy(B->viewBasisDen(), C);
  }
  if (den) {
    t1->simplifyContentDen(&den);
  }
  nforder_ideal *D = new nforder_ideal(t1, A->order());
  if (den)
    D->setBasisDenTransfer(den);

  if (O->oneIsOne())
    D->setMinTransfer(t1->get(1,1), den ? n_Copy(den, C) : n_Init(1, C));
  D->setNormTransfer(t1->det(), den ? n_Copy(den, C) : n_Init(1, C));
  delete t1;
  return D;
}
#endif

nforder_ideal* nf_idMult(nforder_ideal * A, number b)
{
  nforder * O = (nforder*) A->order()->data;
  coeffs C = O->basecoeffs();
  bigintmat * r = O->elRepMat((bigintmat*) b);
  bigintmat * s = bimMult(r, A->viewBasis());
  delete r;
  if (A->isFractional()) {
    number d = n_Copy(A->viewBasisDen(), C);
    s->simplifyContentDen(&d);
    nforder_ideal * res = new nforder_ideal(s, A->order());
    res->setBasisDenTransfer(d);
    return res;
  } else {
    return new nforder_ideal(s, A->order());
  }
}

nforder_ideal * nf_idInit(int i, coeffs O)
{
  nforder *ord = (nforder*) O->data;
  coeffs C = ord->basecoeffs();
  bigintmat * r = new bigintmat(ord->getDim(), ord->getDim(), C);
  r->one();
  number I = n_Init(i, C);
  r->skalmult(I, C);
  nforder_ideal * A = new nforder_ideal(r, O);
  delete r;
  number n;
  n_Power(I, ord->getDim(), &n, C);
  A->setNormTransfer(n, n_Init(1, C));
  A->setMinTransfer(I, n_Init(1, C));
  return A;
}

nforder_ideal * nf_idInit(number I, coeffs O)
{
  nforder *ord = (nforder*) O->data;
  bigintmat * r = ord->elRepMat((bigintmat*)I);
  nforder_ideal * A = new nforder_ideal(r, O);
  delete r;
  return A;
}

nforder_ideal* nf_idMult(nforder_ideal * A, int b)
{
  nforder * O = (nforder*) A->order()->data;
  coeffs C = O->basecoeffs();
  bigintmat * s = new bigintmat(A->viewBasis());
  number bb = n_Init(b, C);
  s->skalmult(bb, C);
  n_Delete(&bb, C);

  if (A->isFractional()) {
    number d = n_Copy(A->viewBasisDen(), C);
    s->simplifyContentDen(&d);
    nforder_ideal * res = new nforder_ideal(s, A->order());
    res->setBasisDenTransfer(d);
    return res;
  } else {
    return new nforder_ideal(s, A->order());
  }
}

nforder_ideal* nf_idPower(nforder_ideal* A, int i)
{
  if (i==0) {
    return nf_idInit(1, A->order());
  } else if (i==1) {
    return new nforder_ideal(A, 1);
  } else if (i<0) {
    Werror("not done yet");
    return NULL;
  } else {
    nforder_ideal *B = nf_idPower(A, i/2);
    nforder_ideal *res = nf_idMult(B, B);
    delete B;
    if (i&1) {
      nforder_ideal * C = nf_idMult(res, B);
      delete res;
      return C;
    } else {
      return res;
    }
  }
}
