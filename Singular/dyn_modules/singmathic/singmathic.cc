#include "kernel/mod2.h"

#ifdef HAVE_MATHICGB

#include "kernel/mod2.h"

#include "misc/options.h"

#include "kernel/ideals.h"
#include "kernel/polys.h"

#include "Singular/ipid.h"
#include "Singular/feOpt.h"
#include "Singular/mod_lib.h"

#include <mathicgb.h>

typedef mgb::GroebnerConfiguration::Coefficient Coefficient;
typedef mgb::GroebnerConfiguration::VarIndex VarIndex;
typedef mgb::GroebnerConfiguration::Exponent Exponent;
typedef mgb::GroebnerConfiguration::BaseOrder BaseOrder;

// Constructs a Singular ideal.
class MathicToSingStream
{
public:
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

  void idealBegin(size_t polyCount)
  {
    deleteIdeal();
    mIdeal = idInit(polyCount);
    mPolyCount = 0;
  }

  void appendPolynomialBegin(size_t termCount) {}

  void appendTermBegin(const mgb::GroebnerConfiguration::Component c)
  {
    if (mTerm == 0)
      mTerm = mIdeal->m[mPolyCount] = pInit();
    else
      mTerm = mTerm->next = pInit();
    pSetComp(mTerm,c);
  }

  void appendExponent(VarIndex index, Exponent exponent)
  {
    pSetExp(mTerm, index + 1, exponent);
  }

  void appendTermDone(Coefficient coefficient)
  {
    mTerm->coef = reinterpret_cast<number>(coefficient);
    pSetm(mTerm);
  }

  void appendPolynomialDone()
  {
    ++mPolyCount;
    mTerm = 0;
  }

  void idealDone() {}


  // Singular interface

  ::ideal takeIdeal()
  {
    ::ideal id = mIdeal;
    mIdeal = 0;
    return id;
  }

private:
  void deleteIdeal()
  {
    if (mIdeal != 0)
    {
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

#include <iostream>

bool setOrder(ring r, mgb::GroebnerConfiguration& conf)
{
  const VarIndex varCount = conf.varCount();

  bool didSetComponentBefore = false;
  mgb::GroebnerConfiguration::BaseOrder baseOrder =
    mgb::GroebnerConfiguration::RevLexDescendingBaseOrder;

  std::vector<Exponent> gradings;
  for (int block = 0; r->order[block] != ringorder_no; ++block)
  {
    // *** ringorder_no

    const rRingOrder_t type = static_cast<rRingOrder_t>(r->order[block]);
    if (r->block0[block] < 0 || r->block1[block] < 0)
    {
      WerrorS("Unexpected negative block0/block1 in ring.");
      return false;
    }
    const VarIndex block0 = static_cast<VarIndex>(r->block0[block]);
    const VarIndex block1 = static_cast<VarIndex>(r->block1[block]);
    const int* const weights = r->wvhdl[block];
    if (block0 > block1)
    {
      WerrorS("Unexpected block0 > block1 in ring.");
      return false;
    }

    // *** ringorder_c and ringorder_C
    if (type == ringorder_c || type == ringorder_C)
    {
      if (block0 != 0 || block1 != 0 || weights != 0)
      {
        WerrorS("Unexpected non-zero fields on c/C block in ring.");
        return false;
      }
      if (didSetComponentBefore)
      {
        WerrorS("Unexpected two c/C blocks in ring.");
        return false;
      }
      didSetComponentBefore = true;
      if (r->order[block + 1] == ringorder_no)
      {
        conf.setComponentBefore
          (mgb::GroebnerConfiguration::ComponentAfterBaseOrder);
      } else
        conf.setComponentBefore(gradings.size() / varCount);
      conf.setComponentsAscending(type == ringorder_C);
      continue;
    }
    if (block0 == 0 || block1 == 0)
    {
      WerrorS("Expected block0 != 0 and block1 != 0 in ring.");
      return false;
    }
    if (block1 > varCount)
    {
      // todo: first handle any block types where this is not true
      WerrorS("Expected block1 <= #vars in ring.");
      return false;
    }

    // dim is how many variables this block concerns.
    const size_t dim = static_cast<size_t>(block1 - block0 + 1);

    // *** single-graded/ungraded lex/revlex orders
    // a(w): w-graded and that's it
    // a64(w): w-graded with 64-bit weights (not supported here)
    //    lp:               lex from  left (descending)
    //    Dp:  1-graded,    lex from  left (descending)
    //    Ds: -1-graded,    lex from  left (descending)
    // Wp(w):  w-graded,    lex from  left (descending)
    // Ws(w): -w-graded,    lex from  left (descending)
    //    rp:               lex from right (ascending)
    //    rs:            revlex from right (descending)
    //    dp:  1-graded, revlex from right (descending)
    //    ds: -1-graded, revlex from right (descending)
    // wp(w):  w-graded, revlex from right (descending)
    // ws(w): -w-graded, revlex from right (descending)
    //    ls:            revlex from  left (ascending)

    if (type == ringorder_a64)
    {
      WerrorS("Block type a64 not supported for MathicGB interface.");
      return false;
    }

    // * handle the single-grading part
    const bool oneGrading = (type == ringorder_Dp || type == ringorder_dp);
    const bool minusOneGrading = (type == ringorder_Ds || type == ringorder_ds);
    const bool wGrading =
      (type == ringorder_a || type == ringorder_Wp || type == ringorder_wp);
    const bool minusWGrading = (type == ringorder_ws || type == ringorder_Ws);
    if (oneGrading || minusOneGrading || wGrading || minusWGrading)
    {
      const VarIndex begin = gradings.size();
      gradings.resize(begin + varCount);
      if (oneGrading || minusOneGrading)
      {
        if (weights != 0)
        {
          WerrorS("Expect wvhdl == 0 in Dp/dp/Ds/ds-block in ring.");
          return false;
        }
        const Exponent value = oneGrading ? 1 : -1;
        for (int var = block0 - 1; var < block1; ++var)
          gradings[begin + var] = value;
      }
      else
      {
        if (weights == 0)
        {
          WerrorS("Expect wvhdl != 0 in a/Wp/wp/ws/Ws-block in ring.");
          return false;
        }
        if (wGrading)
        {
          for (int var = 0; var < dim; ++var)
            gradings[begin + (block0 - 1) + var] = weights[var];
        }
        else
        {
          for (int var = 0; var < dim; ++var)
            gradings[begin + (block0 - 1) + var] = -weights[var];
        }
      }
    }
    if (type == ringorder_a)
      continue; // a has only the grading, so we are done already

    // * handle the lex/revlex part
    const bool lexFromLeft =
      type == ringorder_lp ||
      type == ringorder_Dp ||
      type == ringorder_Ds ||
      type == ringorder_Wp ||
      type == ringorder_Ws;
    const bool lexFromRight = type == ringorder_rp;
    const bool revlexFromLeft = type == ringorder_ls;
    const bool revlexFromRight =
      type == ringorder_rs ||
      type == ringorder_dp ||
      type == ringorder_ds ||
      type == ringorder_wp ||
      type == ringorder_ws;
    if (lexFromLeft || lexFromRight || revlexFromLeft || revlexFromRight)
    {
      const int next = r->order[block + 1];
      bool final = next == ringorder_no;
      if (!final && r->order[block + 2] == ringorder_no)
        final = next == ringorder_c || next == ringorder_C;
      if (final)
      {
        if (lexFromRight)
          baseOrder = mgb::GroebnerConfiguration::LexAscendingBaseOrder;
        else if (revlexFromRight)
          baseOrder = mgb::GroebnerConfiguration::RevLexDescendingBaseOrder;
        else if (lexFromLeft)
          baseOrder = mgb::GroebnerConfiguration::LexDescendingBaseOrder;
        else
          baseOrder = mgb::GroebnerConfiguration::RevLexAscendingBaseOrder;
        continue;
      }

      const size_t begin = gradings.size();
      gradings.resize(begin + dim * varCount);
      const Exponent value = (lexFromLeft || lexFromRight) ? 1 : -1;
      if (lexFromLeft || revlexFromLeft)
      {
        for (size_t row = 0; row < dim; ++row)
          gradings[begin + row * varCount + (block0 - 1) + row] = value;
      }
      else
      {
        for (size_t row = 0; row < dim; ++row)
          gradings[begin + row * varCount + (block1 - 1) - row] = value;
      }
      continue;
    }

    // *** ringorder_M: a square invertible matrix
    if (type == ringorder_M)
    {
      if (weights == 0)
      {
        WerrorS("Expected wvhdl != 0 in M-block in ring.");
        return false;
      }
      const size_t begin = gradings.size();
      gradings.resize(begin + dim * varCount);
      for (size_t row = 0; row < dim; ++row)
        for (size_t col = block0 - 1; col < block1; ++col)
          gradings[begin + row * varCount + col] = weights[row * dim + col];
      continue;
    }

    // *** Miscellaneous unsupported or invalid block types
    if (
      type == ringorder_s ||
      type == ringorder_S ||
      type == ringorder_IS
    )
    {
      // todo: Consider supporting this later.
      WerrorS("Schreyer order s/S/IS not supported in MathicGB interface.");
      return false;
    }
    if (type == ringorder_am)
    {
      // This block is a Schreyer-like ordering only used in Spielwiese.
      // todo: Consider supporting it later.
      WerrorS("Block type am not supported in MathicGB interface");
      return false;
    }
    if (type == ringorder_L)
    {
      WerrorS("Invalid L-block found in order of ring.");
      return false;
    }
    if (type == ringorder_aa)
    {
      // I don't know what an aa block is supposed to do.
      WerrorS("aa ordering not supported by the MathicGB interface.");
      return false;
    }
    if (type == ringorder_unspec)
    {
      WerrorS("Invalid unspec-block found in order of ring.");
      return false;
    }
    WerrorS("Unknown block type found in order of ring.");
    return false;
  }

  if (!didSetComponentBefore)
  {
    WerrorS("Expected to find a c/C block in ring.");
    return false;
  }

  if (!conf.setMonomialOrder(baseOrder, gradings))
  {
    WerrorS("MathicGB does not support non-global orders.");
    return false;
  }
  return true;
}

bool prOrderMatrix(ring r)
{
  const int varCount = r->N;
  mgb::GroebnerConfiguration conf(101, varCount,0);
  if (!setOrder(r, conf))
    return false;
  const std::vector<Exponent>& gradings = conf.monomialOrder().second;
  if (gradings.size() % varCount != 0)
  {
    WerrorS("Expected matrix to be a multiple of varCount.");
    return false;
  }
  const size_t rowCount = gradings.size() / varCount;
  std::cout << "Order matrix:\n";
  for (size_t row = 0; row < rowCount; ++row)
  {
    for (size_t col = 0; col < varCount; ++col)
      std::cerr << ' ' << gradings[row * varCount + col];
    std::cerr << '\n';
  }
  std::cerr
    << "Base order: "
    << mgb::GroebnerConfiguration::baseOrderName(conf.monomialOrder().first)
    << '\n';
  std::cerr << "Component before: " << conf.componentBefore() << '\n';
  std::cerr << "Components ascending: " << conf.componentsAscending() << '\n';
  std::cerr << "Schreyering: " << conf.schreyering() << '\n';
  return true;
}

void prOrder(ring r)
{
  std::cout << "Printing order of ring.\n";
  for (int block = 0; ; ++block)
  {
    switch (r->order[block])
    {
    case ringorder_no: // end of blocks
      return;

    case ringorder_a:
      std::cout << "a";
      break;

    case ringorder_a64: ///< for int64 weights
      std::cout << "a64";
      break;

    case ringorder_c:
      std::cout << "c";
      break;

    case ringorder_C:
      std::cout << "C";
      break;

    case ringorder_M:
      std::cout << "M";
      break;

    case ringorder_S: ///< S?
      std::cout << "S";
      break;

    case ringorder_s: ///< s?
      std::cout << "s";
      break;

    case ringorder_lp:
      std::cout << "lp";
      break;

    case ringorder_dp:
      std::cout << "dp";
      break;

    case ringorder_rp:
      std::cout << "rp";
      break;

    case ringorder_Dp:
      std::cout << "Dp";
      break;

    case ringorder_wp:
      std::cout << "wp";
      break;

    case ringorder_Wp:
      std::cout << "Wp";
      break;

    case ringorder_ls:
      std::cout << "ls"; // not global
      break;

    case ringorder_ds:
      std::cout << "ds"; // not global
      break;

    case ringorder_Ds:
      std::cout << "Ds"; // not global
      break;

    case ringorder_ws:
      std::cout << "ws"; // not global
      break;

    case ringorder_Ws:
      std::cout << "Ws"; // not global
      break;

    case ringorder_am:
      std::cout << "am";
      break;

    case ringorder_L:
      std::cout << "L";
      break;

    // the following are only used internally
    case ringorder_aa: ///< for idElimination, like a, except pFDeg, pWeigths ignore it
      std::cout << "aa";
      break;

    case ringorder_rs: ///< opposite of ls
      std::cout << "rs";
      break;

    case ringorder_IS: ///< Induced (Schreyer) ordering
      std::cout << "IS";
      break;

    case ringorder_unspec:
      std::cout << "unspec";
      break;
    }
    const int b0 = r->block0[block];
    const int b1 = r->block1[block];
    std::cout << ' ' << b0 << ':' << b1 << " (" << r->wvhdl[block] << ")" << std::flush;
    if (r->wvhdl[block] != 0 && b0 != 0)
    {
      for (int v = 0; v <= b1 - b0; ++v)
        std::cout << ' ' << r->wvhdl[block][v];
    } else
      std::cout << " null";
    std::cout << '\n';
  }
}

BOOLEAN prOrderX(leftv result, leftv arg)
{
  if (currRing == 0)
  {
    WerrorS("There is no current ring.");
    return TRUE;
  }
  prOrder(currRing);
  prOrderMatrix(currRing);
  result->rtyp=NONE;
  return FALSE;
}

BOOLEAN mathicgb(leftv result, leftv arg)
{
  result->rtyp=NONE;

  if (arg == NULL || arg->next != NULL ||
  ((arg->Typ() != IDEAL_CMD) &&(arg->Typ() != MODUL_CMD)))
  {
    WerrorS("Syntax: mathicgb(<ideal>/<module>)");
    return TRUE;
  }
  if (!rField_is_Zp(currRing))
  {
    WerrorS("Polynomial ring must be over Zp.");
    return TRUE;
  }

  const int characteristic = n_GetChar(currRing->cf);
  const int varCount = currRing->N;
  const ideal I=(ideal) arg->Data();
  mgb::GroebnerConfiguration conf(characteristic, varCount,I->rank);
  feOptIndex fno=feGetOptIndex(FE_OPT_THREADS);
  //conf.setMaxThreadCount(0); // default number of cores
  conf.setMaxThreadCount((int)(long)feOptSpec[fno].value);
  if (!setOrder(currRing, conf))
    return TRUE;
  if (TEST_OPT_PROT)
    conf.setLogging("all");

  mgb::GroebnerInputIdealStream toMathic(conf);

  const ideal id = static_cast<const ideal>(arg->Data());
  const int size = IDELEMS(id);
  toMathic.idealBegin(size);
  for (int i = 0; i < size; ++i)
  {
    const poly origP = id->m[i];
    int termCount = 0;
    for (poly p = origP; p != 0; p = pNext(p))
      ++termCount;
    toMathic.appendPolynomialBegin(termCount);

    for (poly p = origP; p != 0; p = pNext(p))
    {
      toMathic.appendTermBegin(pGetComp(p));
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

  result->rtyp = arg->Typ();
  result->data = fromMathic.takeIdeal();
  return FALSE;
}

template class std::vector<Exponent>;
template void mgb::computeGroebnerBasis<MathicToSingStream>
  (mgb::GroebnerInputIdealStream&, MathicToSingStream&);

extern "C" int SI_MOD_INIT(singmathic)(SModulFunctions* psModulFunctions)
{
  psModulFunctions->iiAddCproc(
    (currPack->libname ? currPack->libname : ""),
    "mathicgb",
    FALSE,
    mathicgb
  );
  psModulFunctions->iiAddCproc(
    (currPack->libname ? currPack->libname : ""),
    "mathicgb_prOrder",
    FALSE,
    prOrderX
  );
  return MAX_TOK;
}

/* #else

int SI_MOD_INIT(singmathic)(SModulFunctions* psModulFunctions)
{
  WerrorS(
    "Cannot initialize the Singular interface to MathicGB "
    "as this Singular executable was built without support "
    "for MathicGB."
  );
  return 1;
}
*/

/* ressources: ------------------------------------------------------------

http://stackoverflow.com/questions/3786408/number-of-threads-used-by-intel-tbb
When you create the scheduler, you can specify the number of threads as
tbb::task_scheduler_init init(nthread);

    How do I know how many threads are available?

    Do not ask!

        Not even the scheduler knows how many threads really are available
        There may be other processes running on the machine
        Routine may be nested inside other parallel routines

  conf.setMaxThreadCount(0); // default number of cores
*/
#endif /* HAVE_MATHICGB */
