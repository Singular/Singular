#include "kernel/mod2.h"

#include "misc/intvec.h"
#include "misc/options.h"

#include "coeffs/coeffs.h"

#include "polys/PolyEnumerator.h"

#include "polys/monomials/p_polys.h"
#include "polys/monomials/ring.h"
#include "polys/simpleideals.h"

#include "kernel/GBEngine/kstd1.h"

#include "kernel/polys.h"

#include "kernel/GBEngine/syz.h"

#include "Singular/tok.h"
#include "Singular/ipid.h"
#include "Singular/lists.h"
#include "Singular/attrib.h"

#include "Singular/ipid.h"
#include "Singular/ipshell.h" // For iiAddCproc

// extern coeffs coeffs_BIGINT

#include "singularxx_defs.h"

#include "syzextra.h"


#include "Singular/mod_lib.h"


#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "polys/monomials/ring.h"


// returns TRUE, if idRankFreeModule(m) > 0 ???
/// test whether this input has vectors among entries or no enties
/// result must be FALSE for only 0-entries
static BOOLEAN id_IsModule(ideal id, ring r)
{
  id_Test(id, r);

  if( id->rank != 1 ) return TRUE;

  if (rRing_has_Comp(r))
  {
    const int l = IDELEMS(id);

    for (int j=0; j<l; j++)
      if (id->m[j] != NULL && p_GetComp(id->m[j], r) > 0)
        return TRUE;

    return FALSE; // rank: 1, only zero or no entries? can be an ideal OR module... BUT in the use-case should better be an ideal!
  }

  return FALSE;
}




static inline void NoReturn(leftv& res)
{
  res->rtyp = NONE;
  res->data = NULL;
}

/// wrapper around n_ClearContent
static BOOLEAN _ClearContent(leftv res, leftv h)
{
  NoReturn(res);

  const char *usage = "'ClearContent' needs a (non-zero!) poly or vector argument...";

  if( h == NULL )
  {
    WarnS(usage);
    return TRUE;
  }

  assume( h != NULL );

  if( !( h->Typ() == POLY_CMD || h->Typ() == VECTOR_CMD) )
  {
    WarnS(usage);
    return TRUE;
  }

  assume (h->Next() == NULL);

  poly ph = reinterpret_cast<poly>(h->Data());

  if( ph == NULL )
  {
    WarnS(usage);
    return TRUE;
  }

  const ring r =  currRing;
  assume( r != NULL ); assume( r->cf != NULL ); const coeffs C = r->cf;

  number n;

  // experimentall (recursive enumerator treatment) of alg. ext
  CPolyCoeffsEnumerator itr(ph);
  n_ClearContent(itr, n, C);

  res->data = n;
  res->rtyp = NUMBER_CMD;

  return FALSE;
}

/// wrapper around n_ClearDenominators
static BOOLEAN _ClearDenominators(leftv res, leftv h)
{
  NoReturn(res);

  const char *usage = "'ClearDenominators' needs a (non-zero!) poly or vector argument...";

  if( h == NULL )
  {
    WarnS(usage);
    return TRUE;
  }

  assume( h != NULL );

  if( !( h->Typ() == POLY_CMD || h->Typ() == VECTOR_CMD) )
  {
    WarnS(usage);
    return TRUE;
  }

  assume (h->Next() == NULL);

  poly ph = reinterpret_cast<poly>(h->Data());

  if( ph == NULL )
  {
    WarnS(usage);
    return TRUE;
  }

  const ring r =  currRing;
  assume( r != NULL ); assume( r->cf != NULL ); const coeffs C = r->cf;

  number n;

  // experimentall (recursive enumerator treatment) of alg. ext.
  CPolyCoeffsEnumerator itr(ph);
  n_ClearDenominators(itr, n, C);

  res->data = n;
  res->rtyp = NUMBER_CMD;

  return FALSE;
}


/// try to get an optional (simple) integer argument out of h
/// or return the default value
static int getOptionalInteger(const leftv& h, const int _n)
{
  if( h!= NULL && h->Typ() == INT_CMD )
  {
    int n = (int)(long)(h->Data());

    if( n < 0 )
      Warn("Negative (%d) optional integer argument", n);

    return (n);
  }

  return (_n);
}

static inline number jjLONG2N(long d)
{
  return n_Init(d, coeffs_BIGINT);
}

static inline void view(const intvec* v)
{
#ifndef SING_NDEBUG
  v->view();
#else
  // This code duplication is only due to Hannes's #ifndef SING_NDEBUG!
  Print ("intvec: {rows: %d, cols: %d, length: %d, Values: \n", v->rows(), v->cols(), v->length());

  for (int i = 0; i < v->rows(); i++)
  {
    Print ("Row[%3d]:", i);
    for (int j = 0; j < v->cols(); j++)
      Print (" %5d", (*v)[j + i * (v->cols())] );
    PrintLn ();
  }
  PrintS ("}\n");
#endif

}



/// Get leading component
static BOOLEAN leadcomp(leftv res, leftv h)
{
  if ((h!=NULL) && (h->Typ()==VECTOR_CMD || h->Typ()==POLY_CMD))
  {
    const ring r = currRing;

    const poly p = (poly)(h->Data());

    if (p != NULL )
    {
      assume( p != NULL );
      p_LmTest(p, r);

      const unsigned long iComp = p_GetComp(p, r);

  //    assume( iComp > 0 ); // p is a vector

      res->data = reinterpret_cast<void *>(jjLONG2N(iComp));
    }
    else
      res->data = reinterpret_cast<void *>(jjLONG2N(0));


    res->rtyp = BIGINT_CMD;
    return FALSE;
  }

  WerrorS("`leadcomp(<poly/vector>)` expected");
  return TRUE;
}

/// Same for Induced Schreyer ordering (ordering on components is defined by sign!)
static BOOLEAN MakeInducedSchreyerOrdering(leftv res, leftv h)
{
  int sign = 1;
  if ((h!=NULL) && (h->Typ()==INT_CMD))
  {
    const int s = (int)((long)(h->Data()));

    if( s != -1 && s != 1 )
    {
      WerrorS("`MakeInducedSchreyerOrdering(<int>)` called with wrong integer argument (must be +-1)!");
      return TRUE;
    }

    sign = s;
  }

  assume( sign == 1 || sign == -1 );
  res->data = reinterpret_cast<void *>(rAssure_InducedSchreyerOrdering(currRing, TRUE, sign));
  res->rtyp = RING_CMD; // return new ring!
  // QRING_CMD?
  return FALSE;
}


/// ?
static BOOLEAN GetInducedData(leftv res, leftv h)
{
  const ring r = currRing;

  int p = 0; // which IS-block? p^th!

  if ((h!=NULL) && (h->Typ()==INT_CMD))
  {
    p = (int)((long)(h->Data())); h=h->next;
    assume(p >= 0);
  }

  const int pos = rGetISPos(p, r);

  if(  /*(*/ -1 == pos /*)*/  )
  {
    WerrorS("`GetInducedData([int])` called on incompatible ring (not created by 'MakeInducedSchreyerOrdering'!)");
    return TRUE;
  }


  const int iLimit = r->typ[pos].data.is.limit;
  const ideal F = r->typ[pos].data.is.F;

  ideal FF = id_Copy(F, r);

  lists l=(lists)omAllocBin(slists_bin);
  l->Init(2);

  l->m[0].rtyp = INT_CMD;
  l->m[0].data = reinterpret_cast<void *>(iLimit);


  //        l->m[1].rtyp = MODUL_CMD;

  if( id_IsModule(FF, r) ) // ???
  {
    l->m[1].rtyp = MODUL_CMD;

    //          Print("before: %d\n", FF->nrows);
    //          FF->nrows = id_RankFreeModule(FF, r); // ???
    //          Print("after: %d\n", FF->nrows);
  }
  else
    l->m[1].rtyp = IDEAL_CMD;

  l->m[1].data = reinterpret_cast<void *>(FF);

  res->rtyp = LIST_CMD; // list of int/module
  res->data = reinterpret_cast<void *>(l);

  return FALSE;

}

/// Returns old SyzCompLimit, can set new limit
static BOOLEAN SetInducedReferrence(leftv res, leftv h)
{
  res->Init();
  NoReturn(res);

  const ring r = currRing;

  if( !( (h!=NULL) && ( (h->Typ()==IDEAL_CMD) || (h->Typ()==MODUL_CMD))) )
  {
    WerrorS("`SetInducedReferrence(<ideal/module>, [int[, int]])` expected");
    return TRUE;
  }

  const ideal F = (ideal)h->Data(); ; // No copy!
  h=h->next;

  int rank = 0;

  if ((h!=NULL) && (h->Typ()==INT_CMD))
  {
    rank = (int)((long)(h->Data())); h=h->next;
    assume(rank >= 0);
  } else
    rank = id_RankFreeModule(F, r); // Starting syz-comp (1st: i+1)

  int p = 0; // which IS-block? p^th!

  if ((h!=NULL) && (h->Typ()==INT_CMD))
  {
    p = (int)((long)(h->Data())); h=h->next;
    assume(p >= 0);
  }

  const int posIS = rGetISPos(p, r);

  if(  /*(*/ -1 == posIS /*)*/  )
  {
    WerrorS("`SetInducedReferrence(<ideal/module>, [int[, int]])` called on incompatible ring (not created by 'MakeInducedSchreyerOrdering'!)");
    return TRUE;
  }

  // F & componentWeights belong to that ordering block of currRing now:
  rSetISReference(r, F, rank, p); // F will be copied!
  return FALSE;
}


/// Get raw syzygies (idPrepare)
static BOOLEAN idPrepare(leftv res, leftv h)
{
  //        extern int rGetISPos(const int p, const ring r);

  const ring r = currRing;

  const bool isSyz = rIsSyzIndexRing(r);
  const int posIS = rGetISPos(0, r);


  if ( !( (h!=NULL) && (h->Typ()==MODUL_CMD) && (h->Data() != NULL) ) )
  {
    WerrorS("`idPrepare(<module>)` expected");
    return TRUE;
  }

  const ideal I = reinterpret_cast<ideal>(h->Data());

  assume( I != NULL );
  idTest(I);

  int iComp = -1;

  h=h->next;
  if ( (h!=NULL) && (h->Typ()==INT_CMD) )
  {
    iComp = (int)((long)(h->Data()));
  }
  else
  {
      if( (!isSyz) && (-1 == posIS) )
      {
        WerrorS("`idPrepare(<...>)` called on incompatible ring (not created by 'MakeSyzCompOrdering' or 'MakeInducedSchreyerOrdering'!)");
        return TRUE;
      }

    if( isSyz )
      iComp = rGetCurrSyzLimit(r);
    else
      iComp = id_RankFreeModule(r->typ[posIS].data.is.F, r); // ;
  }

  assume(iComp >= 0);


  intvec* w = reinterpret_cast<intvec *>(atGet(h, "isHomog", INTVEC_CMD));
  tHomog hom = testHomog;

  //           int add_row_shift = 0;
  //
  if (w!=NULL)
  {
    w = ivCopy(w);
  //             add_row_shift = ww->min_in();
  //
  //             (*ww) -= add_row_shift;
  //
  //             if (idTestHomModule(I, currRing->qideal, ww))
  //             {
    hom = isHomog;
  //               w = ww;
  //             }
  //             else
  //             {
  //               //WarnS("wrong weights");
  //               delete ww;
  //               w = NULL;
  //               hom=testHomog;
  //             }
  }


  // computes syzygies of h1,
  // works always in a ring with ringorder_s
  // NOTE: rSetSyzComp(syzcomp) should better be called beforehand
  //        ideal idPrepare (ideal  h1, tHomog hom, int syzcomp, intvec **w);

  ideal J = // idPrepare( I, hom, iComp, &w);
           kStd(I, currRing->qideal, hom, &w, NULL, iComp);

  idTest(J);

  if (w!=NULL)
    atSet(res, omStrDup("isHomog"), w, INTVEC_CMD);
  //             if (w!=NULL) delete w;

  res->rtyp = MODUL_CMD;
  res->data = reinterpret_cast<void *>(J);
  return FALSE;
}

extern "C" int SI_MOD_INIT(syzextra)(SModulFunctions* psModulFunctions)
{

#define ADD(C,D,E) \
  psModulFunctions->iiAddCproc((currPack->libname? currPack->libname: ""), (char*)C, D, E);


  ADD("ClearContent", FALSE, _ClearContent);
  ADD("ClearDenominators", FALSE, _ClearDenominators);

  ADD("leadcomp", FALSE, leadcomp);

  ADD("SetInducedReferrence", FALSE, SetInducedReferrence);
  ADD("GetInducedData", FALSE, GetInducedData);
  ADD("MakeInducedSchreyerOrdering", FALSE, MakeInducedSchreyerOrdering);

  ADD("idPrepare", FALSE, idPrepare);

#undef ADD
  return MAX_TOK;
}
