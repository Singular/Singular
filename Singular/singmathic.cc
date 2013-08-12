#include "Singular/mod2.h"
#include "kernel/ring.h"
#include "kernel/febase.h"
#include "kernel/ideals.h"
#include "kernel/polys.h"
#include "kernel/numbers.h"
#include "kernel/options.h"
#include "Singular/ipid.h"

#ifdef HAVE_MATHICGB

#include <mathicgb.h>

// Constructs a Singular ideal.
class MathicToSingStream {
public:
  typedef mgb::GroebnerConfiguration::Coefficient Coefficient;
  typedef mgb::GroebnerConfiguration::VarIndex VarIndex;
  typedef mgb::GroebnerConfiguration::Exponent Exponent;

  MathicToSingStream(Coefficient modulus, VarIndex varCount):
    mModulus(modulus),
    mVarCount(varCount),
    mPolyCount(0),
    mTerm(0),
    mIdeal(0)
  {}

  ~MathicToSingStream() {deleteIdeal();}

  // Mathic stream interface

  Coefficient modulus() const {return mModulus;}
  VarIndex varCount() const {return mModulus;}

  void idealBegin(size_t polyCount) {
    deleteIdeal();
    mIdeal = idInit(polyCount);
    mPolyCount = 0;
  }

  void appendPolynomialBegin(size_t termCount) {}

  void appendTermBegin() {
    if (mTerm == 0)
      mTerm = mIdeal->m[mPolyCount] = pInit();
    else
      mTerm = mTerm->next = pInit();
  }

  void appendExponent(VarIndex index, Exponent exponent) {
    pSetExp(mTerm, index + 1, exponent);
  }

  void appendTermDone(Coefficient coefficient) {
    mTerm->coef = reinterpret_cast<number>(coefficient);
    pSetm(mTerm);
  }

  void appendPolynomialDone() {
    ++mPolyCount;
    mTerm = 0;
  }

  void idealDone() {}


  // Singular interface

  ::ideal takeIdeal() {
    ::ideal id = mIdeal;
    mIdeal = 0;
    return id;
  }

private:
  void deleteIdeal() {
    if (mIdeal != 0) {
      idDelete(&mIdeal);
      mIdeal = 0;
    }
  }

  const Coefficient mModulus;
  const VarIndex mVarCount;
  size_t mPolyCount;
  poly mTerm;
  ::ideal mIdeal;
};

BOOLEAN mathicgb(leftv result, leftv arg)
{
  if (arg == NULL || arg->next != NULL || arg->Typ() != IDEAL_CMD) {
    WerrorS("Syntax: mathicgb(<ideal>)");
    return TRUE;
  }
  if (!rField_is_Zp(currRing)) {
    WerrorS("Polynomial ring must be over Zp.");
    return TRUE;
  }

  const int characteristic = n_GetChar(currRing);
  const int varCount = currRing->N;
  mgb::GroebnerConfiguration conf(characteristic, varCount);
  if (TEST_OPT_PROT)
    conf.setLogging("all");

  mgb::GroebnerInputIdealStream toMathic(conf);

  const ideal id = static_cast<const ideal>(arg->Data());
  const int size = IDELEMS(id);
  toMathic.idealBegin(size);
  for (int i = 0; i < size; ++i) {
    const poly origP = id->m[i];
    int termCount = 0;
    for (poly p = origP; p != 0; p = pNext(p))
      ++termCount;
    toMathic.appendPolynomialBegin(termCount);

    for (poly p = origP; p != 0; p = pNext(p)) {
      toMathic.appendTermBegin();
      for (int i = 1; i <= currRing->N; ++i)
        toMathic.appendExponent(i - 1, pGetExp(p, i));
      const long coefLong = reinterpret_cast<long>(pGetCoeff(p));
      toMathic.appendTermDone(static_cast<int>(coefLong));
    }
    toMathic.appendPolynomialDone();
  }
  toMathic.idealDone();

  MathicToSingStream fromMathic(characteristic, varCount);
  mgb::computeGroebnerBasis(toMathic, fromMathic);

  result->rtyp=IDEAL_CMD;
  result->data = fromMathic.takeIdeal();
  return FALSE;
}

template void mgb::computeGroebnerBasis<MathicToSingStream>
  (mgb::GroebnerInputIdealStream&, MathicToSingStream&);

int singmathic_mod_init(SModulFunctions* psModulFunctions)
{
  PrintS("Initializing Singular-Mathic interface Singmathic.\n");
  psModulFunctions->iiAddCproc(
    (currPack->libname ? currPack->libname : ""),
    "mathicgb",
    FALSE,
    mathicgb
  );
  return 1;
}

#else

int singmathic_mod_init(SModulFunctions* psModulFunctions)
{
  PrintS(
    "Cannot initialize the Singular interface to MathicGB "
    "as this Singular executable was built without support "
    "for MathicGB."
  );
}

#endif
