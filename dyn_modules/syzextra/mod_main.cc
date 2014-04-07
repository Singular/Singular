



#include <kernel/mod2.h>

#include <omalloc/omalloc.h>

#include <misc/intvec.h>

#include <coeffs/coeffs.h>

#include <polys/PolyEnumerator.h>

#include <polys/monomials/p_polys.h>
#include <polys/monomials/ring.h>
// #include <kernel/longrat.h>
#include <kernel/GBEngine/kstd1.h>

#include <kernel/polys.h>

#include <kernel/GBEngine/syz.h>

#include <Singular/tok.h>
#include <Singular/ipid.h>
#include <Singular/lists.h>
#include <Singular/attrib.h>

#include <Singular/ipid.h> 
#include <Singular/ipshell.h> // For iiAddCproc

// extern coeffs coeffs_BIGINT

#include "singularxx_defs.h"
#include "DebugPrint.h"
#include "myNF.h"


#include <Singular/mod_lib.h>


#if GOOGLE_PROFILE_ENABLED 
#include <google/profiler.h>
#endif // #if GOOGLE_PROFILE_ENABLED 


#include <stdio.h>
#include <stdlib.h>
#include <string.h>


extern void pISUpdateComponents(ideal F, const intvec *const V, const int MIN, const ring r);
// extern ring rCurrRingAssure_SyzComp();
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

static BOOLEAN noop(leftv __res, leftv /*__v*/)
{
  NoReturn(__res);
  return FALSE;
}

static BOOLEAN _ProfilerStart(leftv __res, leftv h)
{
  NoReturn(__res);
#if GOOGLE_PROFILE_ENABLED
  if( h!= NULL && h->Typ() == STRING_CMD )
  {
    const char* name = (char*)(h->Data());
    assume( name != NULL );   
    ProfilerStart(name);
  } else
    WerrorS("ProfilerStart requires a string [name] argument"); 
#else
  WarnS("Sorry no google profiler support (GOOGLE_PROFILE_ENABLE!=1)...");
//  return TRUE; // ?
#endif // #if GOOGLE_PROFILE_ENABLED
  return FALSE;
  (void)h;
}
static BOOLEAN _ProfilerStop(leftv __res, leftv /*__v*/)
{
  NoReturn(__res);
#if GOOGLE_PROFILE_ENABLED
  ProfilerStop();
#else
  WarnS("Sorry no google profiler support (GOOGLE_PROFILE_ENABLED!=1)...");
//  return TRUE; // ?
#endif // #if GOOGLE_PROFILE_ENABLED
  return FALSE;
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
    rWrite(r, TRUE);
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
    Print("id '%10s'[%d]: (%p) ncols = %d / size: %d; nrows = %d, rank = %ld / rk: %ld", #v, iLevel, reinterpret_cast<const void*>(((s)->v)[iLevel]), ((s)->v)[iLevel]->ncols, idSize(((s)->v)[iLevel]), ((s)->v)[iLevel]->nrows, ((s)->v)[iLevel]->rank, -1L/*id_RankFreeModule(((s)->v)[iLevel], rrr)*/ ); \
    PrintLn(); \
  } \
  PrintLn();

    // resolvente:
    PRINT_RESOLUTION(syzstr, minres);
    PRINT_RESOLUTION(syzstr, fullres);

    assume (id_RankFreeModule (syzstr->res[1], rr) == syzstr->res[1]->rank);

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






static BOOLEAN Tail(leftv res, leftv h)
{
  NoReturn(res);

  if( h == NULL )
  {
    WarnS("Tail needs an argument...");
    return TRUE;
  }

  assume( h != NULL );

  if( h->Typ() == POLY_CMD || h->Typ() == VECTOR_CMD)
  {
    const poly p = (const poly)h->Data();

    res->rtyp = h->Typ();

    if( p == NULL)
      res->data = NULL;
    else
      res->data = p_Copy( pNext(p), currRing );

    h = h->Next(); assume (h == NULL);
    
    return FALSE;
  }

  if( h->Typ() == IDEAL_CMD || h->Typ() == MODUL_CMD)
  {
    const ideal id = (const ideal)h->Data();

    res->rtyp = h->Typ();

    if( id == NULL)
      res->data = NULL;
    else
    {
      const ideal newid = idInit(IDELEMS(id),id->rank);
      for (int i=IDELEMS(id) - 1; i >= 0; i--)
        if( id->m[i] != NULL )
          newid->m[i] = p_Copy(pNext(id->m[i]), currRing);
        else
          newid->m[i] = NULL;
      
      res->data = newid; 
    }
    
    h = h->Next(); assume (h == NULL);
    
    return FALSE;
  }

  WarnS("Tail needs a single poly/vector/ideal/module argument...");
  return TRUE;
}



/// Get leading term without a module component
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
      p_SetCoeff0(m, n_Copy(p_GetCoeff(p, r), r), r);

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

  WerrorS("`leadmonom(<poly/vector>)` expected");
  return TRUE;
}

/// Get leading component
static BOOLEAN leadcomp(leftv res, leftv h)
{
  NoReturn(res);

  if ((h!=NULL) && (h->Typ()==VECTOR_CMD || h->Typ()==POLY_CMD))
  {
    const ring r = currRing;

    const poly p = (poly)(h->Data());

    if (p != NULL )
    {
      assume( p != NULL );
      assume( p_LmTest(p, r) );

      const unsigned long iComp = p_GetComp(p, r);

  //    assume( iComp > 0 ); // p is a vector

      res->data = reinterpret_cast<void *>(jjLONG2N(iComp));
    } else
      res->data = reinterpret_cast<void *>(jjLONG2N(0));
      

    res->rtyp = BIGINT_CMD;
    return FALSE;
  }

  WerrorS("`leadcomp(<poly/vector>)` expected");
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

  WerrorS("`leadrawexp(<poly/vector>)` expected");
  return TRUE;
}


/// Endowe the current ring with additional (leading) Syz-component ordering
static BOOLEAN MakeSyzCompOrdering(leftv res, leftv /*h*/)
{

  NoReturn(res);

  //    res->data = rCurrRingAssure_SyzComp(); // changes current ring! :(
  res->data = reinterpret_cast<void *>(rAssure_SyzComp(currRing, TRUE));
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


/// Returns old SyzCompLimit, can set new limit
static BOOLEAN SetSyzComp(leftv res, leftv h)
{
  NoReturn(res);

  const ring r = currRing;

  if( !rIsSyzIndexRing(r) )
  {
    WerrorS("`SetSyzComp(<int>)` called on incompatible ring (not created by 'MakeSyzCompOrdering'!)");
    return TRUE;
  }

  res->rtyp = INT_CMD;
  res->data = reinterpret_cast<void *>(rGetCurrSyzLimit(r)); // return old syz limit

  if ((h!=NULL) && (h->Typ()==INT_CMD))
  {
    const int iSyzComp = (int)reinterpret_cast<long>(h->Data());
    assume( iSyzComp > 0 );
    rSetSyzComp(iSyzComp, currRing);
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

  if( idIsModule(FF, r) )
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


//    F = ISUpdateComponents( F, V, MIN );
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

  WerrorS("`ISUpdateComponents(<module>, intvec, int)` expected");
  return TRUE;
}


/// NF using length
static BOOLEAN reduce_syz(leftv res, leftv h)
{
  // const ring r = currRing;

  if ( !( (h!=NULL) && (h->Typ()==VECTOR_CMD || h->Typ()==POLY_CMD) ) )
  {
    WerrorS("`reduce_syz(<poly/vector>!, <ideal/module>, <int>, [int])` expected");
    return TRUE;
  }

  res->rtyp = h->Typ();
  const poly v = reinterpret_cast<poly>(h->Data());
  h=h->next;

  if ( !( (h!=NULL) && (h->Typ()==MODUL_CMD || h->Typ()==IDEAL_CMD ) ) )
  {
    WerrorS("`reduce_syz(<poly/vector>, <ideal/module>!, <int>, [int])` expected");
    return TRUE;
  }

  assumeStdFlag(h);
  const ideal M = reinterpret_cast<ideal>(h->Data()); h=h->next;


  if ( !( (h!=NULL) && (h->Typ()== INT_CMD)  ) )
  {
    WerrorS("`reduce_syz(<poly/vector>, <ideal/module>, <int>!, [int])` expected");
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
  } else
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

/// Get raw syzygies (idPrepare)
static BOOLEAN _p_Content(leftv res, leftv h)
{
  if ( !( (h!=NULL) && (h->Typ()==POLY_CMD) && (h->Data() != NULL) ) )
  {
    WerrorS("`p_Content(<poly-var>)` expected");
    return TRUE;
  }


  const poly p = reinterpret_cast<poly>(h->Data());

  
  pTest(p);  pWrite(p); PrintLn();

  
  p_Content( p, currRing);      

  pTest(p);
  pWrite(p); PrintLn();
  
  NoReturn(res);
  return false;
}

static BOOLEAN _m2_end(leftv res, leftv h)
{
  int ret = 0;
  
  if ( (h!=NULL) && (h->Typ()!=INT_CMD) )
  {
    WerrorS("`m2_end([<int>])` expected");
    return TRUE;
  }
  ret = (int)(long)(h->Data());

  m2_end( ret );

  NoReturn(res);
  return FALSE;
}

   

END_NAMESPACE


int SI_MOD_INIT(syzextra)(SModulFunctions* psModulFunctions) 
{
#define ADD0(A,B,C,D,E) A(B, (char*)C, D, E)
// #define ADD(A,B,C,D,E) ADD0(iiAddCproc, "", C, D, E)
  #define ADD(A,B,C,D,E) ADD0(A->iiAddCproc, B, C, D, E)
  ADD(psModulFunctions, currPack->libname, "ClearContent", FALSE, _ClearContent);
  ADD(psModulFunctions, currPack->libname, "ClearDenominators", FALSE, _ClearDenominators);

  ADD(psModulFunctions, currPack->libname, "DetailedPrint", FALSE, DetailedPrint);
  ADD(psModulFunctions, currPack->libname, "leadmonomial", FALSE, leadmonom);
  ADD(psModulFunctions, currPack->libname, "leadcomp", FALSE, leadcomp);
  ADD(psModulFunctions, currPack->libname, "leadrawexp", FALSE, leadrawexp);

  ADD(psModulFunctions, currPack->libname, "Tail", FALSE, Tail);

  ADD(psModulFunctions, currPack->libname, "ISUpdateComponents", FALSE, ISUpdateComponents);
  ADD(psModulFunctions, currPack->libname, "SetInducedReferrence", FALSE, SetInducedReferrence);
  ADD(psModulFunctions, currPack->libname, "GetInducedData", FALSE, GetInducedData);
  ADD(psModulFunctions, currPack->libname, "SetSyzComp", FALSE, SetSyzComp);
  ADD(psModulFunctions, currPack->libname, "MakeInducedSchreyerOrdering", FALSE, MakeInducedSchreyerOrdering);
  ADD(psModulFunctions, currPack->libname, "MakeSyzCompOrdering", FALSE, MakeSyzCompOrdering);

  ADD(psModulFunctions, currPack->libname, "ProfilerStart", FALSE, _ProfilerStart); ADD(psModulFunctions, currPack->libname, "ProfilerStop",  FALSE, _ProfilerStop );
  
  ADD(psModulFunctions, currPack->libname, "noop", FALSE, noop);
  ADD(psModulFunctions, currPack->libname, "idPrepare", FALSE, idPrepare);
  ADD(psModulFunctions, currPack->libname, "reduce_syz", FALSE, reduce_syz);

  ADD(psModulFunctions, currPack->libname, "p_Content", FALSE, _p_Content);

  ADD(psModulFunctions, currPack->libname, "m2_end", FALSE, _m2_end);
  //  ADD(psModulFunctions, currPack->libname, "", FALSE, );
#undef ADD  
  return 0;
}

#ifndef EMBED_PYTHON
extern "C" { 
int mod_init(SModulFunctions* psModulFunctions)
{ 
  return SI_MOD_INIT(syzextra)(psModulFunctions); 
}
}
#endif
