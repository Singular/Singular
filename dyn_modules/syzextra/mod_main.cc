#include <Singular/mod2.h>

#include <omalloc/omalloc.h>

#include <kernel/syz.h>
#include <kernel/intvec.h>
#include <kernel/p_polys.h>
#include <kernel/longrat.h>
#include <kernel/kstd1.h>

#include <Singular/tok.h>
#include <Singular/ipid.h>
#include <Singular/lists.h>
#include <Singular/attrib.h>

#include "singularxx_defs.h"
#include "DebugPrint.h"
#include "myNF.h"



extern void rSetISReference(const ideal F, const int rank, const int p, const intvec * componentWeights, const ring r);
extern void pISUpdateComponents(ideal F, const intvec *const V, const int MIN, const ring r);
extern ring rCurrRingAssure_SyzComp();
extern ring rAssure_InducedSchreyerOrdering(const ring r, BOOLEAN complete, int sign);
extern int rGetISPos(const int p, const ring r);


USING_NAMESPACE( SINGULARXXNAME :: DEBUG )
USING_NAMESPACE( SINGULARXXNAME :: NF )

BEGIN_NAMESPACE_NONAME
static inline void NoReturn(leftv& res)
{
  res->rtyp = NONE;
  res->data = NULL;
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

BOOLEAN noop(leftv __res, leftv /*__v*/)
{
  NoReturn(__res);
  return FALSE;
}



static inline number jjLONG2N(long d)
{
#if SIZEOF_LONG == 8
  int i=(int)d;
  if ((long)i == d)
    return nlInit(i, NULL);
  else
  {
# if !defined(OM_NDEBUG) && !defined(NDEBUG)
    omCheckBin(rnumber_bin);
# endif
    number z=(number)omAllocBin(rnumber_bin);
# if defined(LDEBUG)
    z->debug=123456;
# endif
    z->s=3;
    mpz_init_set_si(z->z,d);
    return z;
  }
#else
  return nlInit((int)d, NULL);
#endif
}

static inline void view(const intvec* v)
{
#ifndef NDEBUG
  v->view();
#else
  // This code duplication is only due to Hannes's #ifndef NDEBUG!
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

                   

static BOOLEAN DetailedPrint(leftv __res, leftv h)
{
  NoReturn(__res);

  if( h == NULL )
  {
    WarnS("DetailedPrint needs an argument...");
    return TRUE;
  }

  if( h->Typ() == RING_CMD)
  {
    const ring r = (const ring)h->Data();
    rWrite(r);
    PrintLn();
#ifdef RDEBUG
    rDebugPrint(r);
#endif
    return FALSE;
  }

  if( h->Typ() == POLY_CMD || h->Typ() == VECTOR_CMD)
  {
    const poly p = (const poly)h->Data(); h = h->Next();

    dPrint(p, currRing, currRing, getOptionalInteger(h, 3));

    return FALSE;
  }

  if( h->Typ() == IDEAL_CMD || h->Typ() == MODUL_CMD)
  {
    const ideal id = (const ideal)h->Data(); h = h->Next(); 

    dPrint(id, currRing, currRing, getOptionalInteger(h, 3));
    
    return FALSE;            
  }

  if( h->Typ() == RESOLUTION_CMD )
  {
    const syStrategy syzstr = reinterpret_cast<const syStrategy>(h->Data());

    h = h->Next();

    int nTerms = getOptionalInteger(h, 1);


    Print("RESOLUTION_CMD(%p): ", reinterpret_cast<const void*>(syzstr)); PrintLn();

    const ring save = currRing;
    const ring r = syzstr->syRing;
    const ring rr = (r != NULL) ? r: save;


    const int iLength = syzstr->length;

    Print("int 'length': %d", iLength); PrintLn();
    Print("int 'regularity': %d", syzstr->regularity); PrintLn();
    Print("short 'list_length': %hd", syzstr->list_length); PrintLn();
    Print("short 'references': %hd", syzstr->references); PrintLn();


#define PRINT_pINTVECTOR(s, v) Print("intvec '%10s'(%p)", #v, reinterpret_cast<const void*>((s)->v)); \
if( (s)->v != NULL ){ PrintS(": "); view((s)->v); }; \
PrintLn();

    PRINT_pINTVECTOR(syzstr, resolution);
    PRINT_pINTVECTOR(syzstr, betti);
    PRINT_pINTVECTOR(syzstr, Tl);
    PRINT_pINTVECTOR(syzstr, cw);
#undef PRINT_pINTVECTOR

    if (r == NULL)
      Print("ring '%10s': NULL", "syRing");
    else 
      if (r == currRing)
        Print("ring '%10s': currRing", "syRing");
      else
        if (r != NULL && r != save)
        {
          Print("ring '%10s': ", "syRing");
          rWrite(r);
#ifdef RDEBUG
          //              rDebugPrint(r);
#endif
          // rChangeCurrRing(r);
        }            
    PrintLn();

    const SRes rP = syzstr->resPairs;
    Print("SRes 'resPairs': %p", reinterpret_cast<const void*>(rP)); PrintLn();

    if (rP != NULL)
      for (int iLevel = 0; (iLevel < iLength) && (rP[iLevel] != NULL) && ((*syzstr->Tl)[iLevel] >= 0); iLevel++)
      {
        int n = 0;
        const int iTl = (*syzstr->Tl)[iLevel];
        for (int j = 0; (j < iTl) && ((rP[iLevel][j].lcm!=NULL) || (rP[iLevel][j].syz!=NULL)); j++)
        {
          if (rP[iLevel][j].isNotMinimal==NULL)
            n++;
        }
        Print("minimal-resPairs-Size[1+%d]: %d", iLevel, n); PrintLn();
      }


    //  const ring rrr = (iLevel > 0) ? rr : save; ?
#define PRINT_RESOLUTION(s, v) Print("resolution '%12s': %p", #v, reinterpret_cast<const void*>((s)->v)); PrintLn(); \
if ((s)->v != NULL) \
  for (int iLevel = 0; (iLevel < iLength) && ( ((s)->v)[iLevel] != NULL ); iLevel++) \
  { \
    /* const ring rrr = (iLevel > 0) ? save : save; */ \
    Print("id '%10s'[%d]: (%p) ncols = %d / size: %d; nrows = %d, rank = %ld / rk: %ld", #v, iLevel, reinterpret_cast<const void*>(((s)->v)[iLevel]), ((s)->v)[iLevel]->ncols, idSize(((s)->v)[iLevel]), ((s)->v)[iLevel]->nrows, ((s)->v)[iLevel]->rank, -1L/*idRankFreeModule(((s)->v)[iLevel], rrr)*/ ); \
    PrintLn(); \
  } \
  PrintLn();

    // resolvente:
    PRINT_RESOLUTION(syzstr, minres);
    PRINT_RESOLUTION(syzstr, fullres);

    assume (idRankFreeModule (syzstr->res[1], rr) == syzstr->res[1]->rank);

    PRINT_RESOLUTION(syzstr, res);
    PRINT_RESOLUTION(syzstr, orderedRes);
#undef PRINT_RESOLUTION

#define PRINT_POINTER(s, v) Print("pointer '%17s': %p", #v, reinterpret_cast<const void*>((s)->v)); PrintLn();
    // 2d arrays:
    PRINT_POINTER(syzstr, truecomponents);
    PRINT_POINTER(syzstr, ShiftedComponents);
    PRINT_POINTER(syzstr, backcomponents);
    PRINT_POINTER(syzstr, Howmuch);
    PRINT_POINTER(syzstr, Firstelem);
    PRINT_POINTER(syzstr, elemLength);
    PRINT_POINTER(syzstr, sev);

    // arrays of intvects:
    PRINT_POINTER(syzstr, weights);
    PRINT_POINTER(syzstr, hilb_coeffs);
#undef PRINT_POINTER


    if (syzstr->fullres==NULL)
    {
      PrintS("resolution 'fullres': (NULL) => resolution not computed yet");
      PrintLn();
    } else
    {
      Print("resolution 'fullres': (%p) => resolution seems to be computed already", reinterpret_cast<const void*>(syzstr->fullres));
      PrintLn();
      dPrint(*syzstr->fullres, save, save, nTerms);
    }




    if (syzstr->minres==NULL)
    {
      PrintS("resolution 'minres': (NULL) => resolution not minimized yet");
      PrintLn();
    } else
    {
      Print("resolution 'minres': (%p) => resolution seems to be minimized already", reinterpret_cast<const void*>(syzstr->minres));
      PrintLn();
      dPrint(*syzstr->minres, save, save, nTerms);
    }




    /*
    int ** truecomponents;
    long** ShiftedComponents;
    int ** backcomponents;
    int ** Howmuch;
    int ** Firstelem;
    int ** elemLength;
    unsigned long ** sev;

    intvec ** weights;
    intvec ** hilb_coeffs;

    SRes resPairs;               //polynomial data for internal use only

    resolvente fullres;
    resolvente minres;
    resolvente res;              //polynomial data for internal use only
    resolvente orderedRes;       //polynomial data for internal use only
*/

    //            if( currRing != save ) rChangeCurrRing(save);
  }


  return FALSE;
}



/// Get leading monom (no module component)
static BOOLEAN leadmonom(leftv res, leftv h)
{
  NoReturn(res);

  if ((h!=NULL) && (h->Typ()==VECTOR_CMD || h->Typ()==POLY_CMD) && (h->Data() != NULL))
  {
    const ring r = currRing;
    const poly p = (poly)(h->Data());

    poly m = NULL;

    if( p != NULL )
    {
      assume( p != NULL );
      assume( p_LmTest(p, r) );

      m = p_LmInit(p, r);
      p_SetCoeff0(m, n_Init(1, r), r);

      //            if( p_GetComp(m, r) != 0 )
      //            {
      p_SetComp(m, 0, r);
      p_Setm(m, r);
      //            }

      assume( p_GetComp(m, r) == 0 );
      assume( m != NULL );
      assume( p_LmTest(m, r) );
    }

    res->rtyp = POLY_CMD;
    res->data = reinterpret_cast<void *>(m);

    return FALSE;
  }

  WerrorS("`system(\"leadmonom\",<poly/vector>)` expected");
  return TRUE;
}

/// Get leading component
static BOOLEAN leadcomp(leftv res, leftv h)
{
  NoReturn(res);

  if ((h!=NULL) && (h->Typ()==VECTOR_CMD || h->Typ()==POLY_CMD) && (h->Data() != NULL))
  {
    const ring r = currRing;

    const poly p = (poly)(h->Data());

    assume( p != NULL );
    assume( p_LmTest(p, r) );

    const unsigned long iComp = p_GetComp(p, r);

    assume( iComp > 0 ); // p is a vector

    res->rtyp = BIGINT_CMD;
    res->data = reinterpret_cast<void *>(jjLONG2N(iComp));

    return FALSE;
  }

  WerrorS("`system(\"leadcomp\",<poly/vector>)` expected");
  return TRUE;
}




/// Get raw leading exponent vector
static BOOLEAN leadrawexp(leftv res, leftv h)
{
  NoReturn(res);

  if ((h!=NULL) && (h->Typ()==VECTOR_CMD || h->Typ()==POLY_CMD) && (h->Data() != NULL))
  {
    const ring r = currRing;
    const poly p = (poly)(h->Data());

    assume( p != NULL );
    assume( p_LmTest(p, r) );

    const int iExpSize = r->ExpL_Size;

//    intvec *iv = new intvec(iExpSize);

    lists l=(lists)omAllocBin(slists_bin);
    l->Init(iExpSize);

    for(int i = iExpSize-1; i >= 0; i--)
    {
      l->m[i].rtyp = BIGINT_CMD;
      l->m[i].data = reinterpret_cast<void *>(jjLONG2N(p->exp[i])); // longs...
    }

    res->rtyp = LIST_CMD; // list of bigints
    res->data = reinterpret_cast<void *>(l);
    return FALSE;
  }

  WerrorS("`system(\"leadrawexp\",<poly/vector>)` expected");
  return TRUE;
}


/// Endowe the current ring with additional (leading) Syz-component ordering
static BOOLEAN MakeSyzCompOrdering(leftv res, leftv /*h*/)
{

  NoReturn(res);

  //    res->data = rCurrRingAssure_SyzComp(); // changes current ring! :(
  res->data = reinterpret_cast<void *>(rCurrRingAssure_SyzComp());
  res->rtyp = RING_CMD; // return new ring!
  // QRING_CMD? 

  return FALSE;
}


/// Same for Induced Schreyer ordering (ordering on components is defined by sign!)
static BOOLEAN MakeInducedSchreyerOrdering(leftv res, leftv h)
{

  NoReturn(res);

  int sign = 1;
  if ((h!=NULL) && (h->Typ()==INT_CMD))
  {
    const int s = (int)((long)(h->Data()));

    if( s != -1 && s != 1 )
    {
      WerrorS("`system(\"MakeInducedSchreyerOrdering\",<int>)` called with wrong integer argument (must be +-1)!");
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


/// Returns old SyzCompLimit, can set new limit
static BOOLEAN SetSyzComp(leftv res, leftv h)
{
  NoReturn(res);

  const ring r = currRing;

  if( !rIsSyzIndexRing(r) )
  {
    WerrorS("`system(\"SetSyzComp\",<int>)` called on incompatible ring (not created by 'MakeSyzCompOrdering'!)");
    return TRUE;
  }

  res->rtyp = INT_CMD;
  res->data = reinterpret_cast<void *>(rGetCurrSyzLimit(r)); // return old syz limit

  if ((h!=NULL) && (h->Typ()==INT_CMD))
  {
    const int iSyzComp = (int)reinterpret_cast<long>(h->Data());
    assume( iSyzComp > 0 );
    rSetSyzComp( iSyzComp );
  }

  return FALSE;
}

/// ?
static BOOLEAN GetInducedData(leftv res, leftv h)
{
  NoReturn(res);

  const ring r = currRing;

  int p = 0; // which IS-block? p^th!

  if ((h!=NULL) && (h->Typ()==INT_CMD))
  {
    p = (int)((long)(h->Data())); h=h->next;
    assume(p >= 0);
  }

  const int pos = rGetISPos(p, r);

  if( (-1 == pos) )
  {
    WerrorS("`system(\"GetInducedData\",[int])` called on incompatible ring (not created by 'MakeInducedSchreyerOrdering'!)");
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

  if( idIsModule(FF, r) )
  {
    l->m[1].rtyp = MODUL_CMD;

    //          Print("before: %d\n", FF->nrows);
    //          FF->nrows = idRankFreeModule(FF, r); // ???
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
  NoReturn(res);

  const ring r = currRing;

  if( !( (h!=NULL) && ( (h->Typ()==IDEAL_CMD) || (h->Typ()==MODUL_CMD))) )
  {
    WerrorS("`system(\"SetInducedReferrence\",<ideal/module>, [int[, int]])` expected");
    return TRUE;
  }

  intvec * componentWeights = (intvec *)atGet(h,"isHomog",INTVEC_CMD); // No copy!

  const ideal F = (ideal)h->Data(); ; // No copy!
  h=h->next;

  int rank = 0;

  if ((h!=NULL) && (h->Typ()==INT_CMD))
  {
    rank = (int)((long)(h->Data())); h=h->next;
    assume(rank >= 0);
  } else
    rank = idRankFreeModule(F, r); // Starting syz-comp (1st: i+1)

  int p = 0; // which IS-block? p^th!

  if ((h!=NULL) && (h->Typ()==INT_CMD))
  {
    p = (int)((long)(h->Data())); h=h->next;
    assume(p >= 0);
  }

  const int posIS = rGetISPos(p, r);

  if( (-1 == posIS) )
  {
    WerrorS("`system(\"SetInducedReferrence\",<ideal/module>, [int[, int]])` called on incompatible ring (not created by 'MakeInducedSchreyerOrdering'!)");
    return TRUE;
  }



  // F & componentWeights belong to that ordering block of currRing now:
  rSetISReference(F, rank, p, componentWeights, r); // F and componentWeights will be copied!
  return FALSE;
}


//    F = system("ISUpdateComponents", F, V, MIN );
//    // replace gen(i) -> gen(MIN + V[i-MIN]) for all i > MIN in all terms from F!
static BOOLEAN ISUpdateComponents(leftv res, leftv h)
{
  NoReturn(res);

  PrintS("ISUpdateComponents:.... \n");

  if ((h!=NULL) && (h->Typ()==MODUL_CMD))
  {
    ideal F = (ideal)h->Data(); ; // No copy!
    h=h->next;

    if ((h!=NULL) && (h->Typ()==INTVEC_CMD))
    {
      const intvec* const V = (const intvec* const) h->Data();
      h=h->next;

      if ((h!=NULL) && (h->Typ()==INT_CMD))
      {
        const int MIN = (int)((long)(h->Data()));

        pISUpdateComponents(F, V, MIN, currRing);
        return FALSE;
      }
    }
  }

  WerrorS("`system(\"ISUpdateComponents\",<module>, intvec, int)` expected");
  return TRUE;
}


/// NF using length
static BOOLEAN reduce_syz(leftv res, leftv h)
{
  const ring r = currRing;

  if ( !( (h!=NULL) && (h->Typ()==VECTOR_CMD || h->Typ()==POLY_CMD) ) )
  {
    WerrorS("`system(\"reduce_syz\",<poly/vector>!, <ideal/module>, <int>, [int])` expected");
    return TRUE;
  }

  res->rtyp = h->Typ();
  const poly v = reinterpret_cast<poly>(h->Data());
  h=h->next;

  if ( !( (h!=NULL) && (h->Typ()==MODUL_CMD || h->Typ()==IDEAL_CMD ) ) )
  {
    WerrorS("`system(\"reduce_syz\",<poly/vector>, <ideal/module>!, <int>, [int])` expected");
    return TRUE;
  }

  assumeStdFlag(h);
  const ideal M = reinterpret_cast<ideal>(h->Data()); h=h->next;


  if ( !( (h!=NULL) && (h->Typ()== INT_CMD)  ) )
  {
    WerrorS("`system(\"reduce_syz\",<poly/vector>, <ideal/module>, <int>!, [int])` expected");
    return TRUE;
  }

  const int iSyzComp = (int)((long)(h->Data())); h=h->next;

  int iLazyReduce = 0;

  if ( ( (h!=NULL) && (h->Typ()== INT_CMD)  ) )
    iLazyReduce = (int)((long)(h->Data())); 

  res->data = (void *)kNFLength(M, currQuotient, v, iSyzComp, iLazyReduce);
  return FALSE;
}


/// Get raw syzygies (idPrepare)
static BOOLEAN idPrepare(leftv res, leftv h)
{
  //        extern int rGetISPos(const int p, const ring r);

  const ring r = currRing;

  const bool isSyz = rIsSyzIndexRing(r);
  const int posIS = rGetISPos(0, r);

  if( (!isSyz) && (-1 == posIS) )
  {
    WerrorS("`system(\"idPrepare\",<...>)` called on incompatible ring (not created by 'MakeSyzCompOrdering' or 'MakeInducedSchreyerOrdering'!)");
    return TRUE;
  }

  if ( !( (h!=NULL) && (h->Typ()==MODUL_CMD) && (h->Data() != NULL) ) )
  {
    WerrorS("`system(\"idPrepare\",<module>)` expected");
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
  } else
  {
    if( isSyz )
      iComp = rGetCurrSyzLimit(r);
    else
      iComp = idRankFreeModule(r->typ[posIS].data.is.F, r); // ;
  }


  tHomog hom=testHomog;
  intvec *w; //  = reinterpret_cast<intvec *>(atGet(h, "isHomog", INTVEC_CMD));

  //           int add_row_shift = 0;
  // 
  //           if (w!=NULL)
  //           {
  //             intvec * ww = ivCopy(w);
  // 
  //             add_row_shift = ww->min_in();
  // 
  //             (*ww) -= add_row_shift;
  //             
  //             if (idTestHomModule(I, currQuotient, ww))
  //             {
  //               hom = isHomog;
  //               w = ww;
  //             }
  //             else
  //             {
  //               //WarnS("wrong weights");
  //               delete ww;
  //               w = NULL;
  //               hom=testHomog;
  //             }
  //           }


  // computes syzygies of h1,
  // works always in a ring with ringorder_s
  // NOTE: rSetSyzComp(syzcomp) should better be called beforehand
  //        ideal idPrepare (ideal  h1, tHomog hom, int syzcomp, intvec **w);

  ideal J = // idPrepare( I, hom, iComp, &w);
           kStd(I, currQuotient, hom, &w, NULL, iComp);

  idTest(J);

  if (w!=NULL) delete w;
  //          if (w!=NULL)
  //            atSet(res, omStrDup("isHomog"), w, INTVEC_CMD);

  res->rtyp = MODUL_CMD;
  res->data = reinterpret_cast<void *>(J);
  return FALSE;
}
      
END_NAMESPACE

extern "C"
{

int mod_init(SModulFunctions* psModulFunctions) 
{
  
  psModulFunctions->iiAddCproc(currPack->libname,(char*)"DetailedPrint",FALSE, DetailedPrint);
  psModulFunctions->iiAddCproc(currPack->libname,(char*)"leadmonom",FALSE, leadmonom);
  psModulFunctions->iiAddCproc(currPack->libname,(char*)"leadcomp",FALSE, leadcomp);
  psModulFunctions->iiAddCproc(currPack->libname,(char*)"leadrawexp",FALSE, leadrawexp);


  psModulFunctions->iiAddCproc(currPack->libname,(char*)"ISUpdateComponents",FALSE, ISUpdateComponents);
  psModulFunctions->iiAddCproc(currPack->libname,(char*)"SetInducedReferrence",FALSE, SetInducedReferrence);
  psModulFunctions->iiAddCproc(currPack->libname,(char*)"GetInducedData",FALSE, GetInducedData);
  psModulFunctions->iiAddCproc(currPack->libname,(char*)"SetSyzComp",FALSE, SetSyzComp);
  psModulFunctions->iiAddCproc(currPack->libname,(char*)"MakeInducedSchreyerOrdering",FALSE, MakeInducedSchreyerOrdering);
  psModulFunctions->iiAddCproc(currPack->libname,(char*)"MakeSyzCompOrdering",FALSE, MakeSyzCompOrdering);
  
  psModulFunctions->iiAddCproc(currPack->libname,(char*)"noop",FALSE, noop);
 
  psModulFunctions->iiAddCproc(currPack->libname,(char*)"idPrepare",FALSE, idPrepare);
  psModulFunctions->iiAddCproc(currPack->libname,(char*)"reduce_syz",FALSE, reduce_syz);

//  psModulFunctions->iiAddCproc(currPack->libname,(char*)"",FALSE, );
  
  return 0;
}
}
