#include <kernel/mod2.h>

#include <omalloc/omalloc.h>

#include <misc/intvec.h>
#include <misc/options.h>

#include <coeffs/coeffs.h>

#include <polys/PolyEnumerator.h>

#include <polys/monomials/p_polys.h>
#include <polys/monomials/ring.h>
#include <polys/simpleideals.h>

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
#include "syzextra.h"


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

// USING_NAMESPACE_SINGULARXX;

USING_NAMESPACE( SINGULARXXNAME :: DEBUG )
USING_NAMESPACE( SINGULARXXNAME :: NF )
USING_NAMESPACE( SINGULARXXNAME :: SYZEXTRA )


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

  if( h->Typ() == NUMBER_CMD)
  {
    number n = (number)h->Data();

    const ring r = currRing;

    n_Test(n, r->cf);

    StringSetS("");
    n_Write(n, r->cf);
    PrintS(StringEndS());
    PrintLn();

    return FALSE;
  }

  if( h->Typ() == RING_CMD)
  {
    const ring r = (const ring)h->Data();
    rWrite(r, TRUE);
    PrintLn();
#ifdef RDEBUG
    //rDebugPrint(r);
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

//    assume (id_RankFreeModule (syzstr->res[1], rr) == syzstr->res[1]->rank);

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

/// wrapper around p_Tail and id_Tail
static BOOLEAN Tail(leftv res, leftv h)
{
  NoReturn(res);

  if( h == NULL )
  {
    WarnS("Tail needs a poly/vector/ideal/module argument...");
    return TRUE;
  }

  assume( h != NULL );

  const ring r =  currRing;

  if( h->Typ() == POLY_CMD || h->Typ() == VECTOR_CMD)
  {
    res->data = p_Tail( (const poly)h->Data(), r );
    res->rtyp = h->Typ();

    h = h->Next(); assume (h == NULL);

    return FALSE;
  }

  if( h->Typ() == IDEAL_CMD || h->Typ() == MODUL_CMD)
  {
    res->data = id_Tail( (const ideal)h->Data(), r );
    res->rtyp = h->Typ();

    h = h->Next(); assume (h == NULL);

    return FALSE;
  }

  WarnS("Tail needs a single poly/vector/ideal/module argument...");
  return TRUE;
}



static BOOLEAN _ComputeLeadingSyzygyTerms(leftv res, leftv h)
{
  const SchreyerSyzygyComputationFlags attributes(currRingHdl);

  const BOOLEAN OPT__DEBUG      = attributes.OPT__DEBUG;
//  const BOOLEAN OPT__SYZCHECK   = attributes.OPT__SYZCHECK;
  const BOOLEAN OPT__LEAD2SYZ   = attributes.OPT__LEAD2SYZ;
//  const BOOLEAN OPT__HYBRIDNF   = attributes.OPT__HYBRIDNF;
//  const BOOLEAN OPT__TAILREDSYZ = attributes.OPT__TAILREDSYZ;

  const ring r = attributes.m_rBaseRing;
  NoReturn(res);

  if( h == NULL )
  {
    WarnS("ComputeLeadingSyzygyTerms needs an argument...");
    return TRUE;
  }

  assume( h != NULL );

  if( h->Typ() == IDEAL_CMD || h->Typ() == MODUL_CMD)
  {
    const ideal id = (const ideal)h->Data();

    assume(id != NULL);

    if( UNLIKELY( OPT__DEBUG ) )
    {
      PrintS("ComputeLeadingSyzygyTerms::Input: \n");
      dPrint(id, r, r, 0);
    }

    assume( !OPT__LEAD2SYZ );

    h = h->Next(); assume (h == NULL);

    const ideal newid = ComputeLeadingSyzygyTerms(id,  attributes);

    res->data = newid; res->rtyp = MODUL_CMD;
    return FALSE;
  }

  WarnS("ComputeLeadingSyzygyTerms needs a single ideal/module argument...");
  return TRUE;
}

///  sorting wrt <c,ds> & reversing...
/// change the input inplace!!!
// TODO: use a ring with >_{c, ds}!???
static BOOLEAN _Sort_c_ds(leftv res, leftv h)
{
  const SchreyerSyzygyComputationFlags attributes(currRingHdl);

  const BOOLEAN OPT__DEBUG      = FALSE; // attributes.OPT__DEBUG;
//  const BOOLEAN OPT__SYZCHECK   = attributes.OPT__SYZCHECK;
//  const BOOLEAN OPT__LEAD2SYZ   = attributes.OPT__LEAD2SYZ;
//  const BOOLEAN OPT__HYBRIDNF   = attributes.OPT__HYBRIDNF;
//  const BOOLEAN OPT__TAILREDSYZ = attributes.OPT__TAILREDSYZ;

  NoReturn(res);

  const ring r = attributes.m_rBaseRing;
  NoReturn(res);

  if( h == NULL )
  {
    WarnS("Sort_c_ds needs an argument...");
    return TRUE;
  }

  assume( h != NULL );

  if(    (h->Typ() == IDEAL_CMD || h->Typ() == MODUL_CMD)
      && (h->rtyp  == IDHDL) // must be a variable!
      && (h->e == NULL) // not a list element
      )
  {
    const ideal id = (const ideal)h->Data();

    assume(id != NULL);

    if( UNLIKELY( OPT__DEBUG ) )
    {
      PrintS("Sort_c_ds::Input: \n");
      dPrint(id, r, r, 0);
    }

    assume (h->Next() == NULL);

    id_Test(id, r);

    Sort_c_ds(id, r); // NOT A COPY! inplace sorting!!!

//    res->data = id;
//    res->rtyp = h->Typ();

    if( UNLIKELY( OPT__DEBUG ) )
    {
      PrintS("Sort_c_ds::Output: \n");
      dPrint(id, r, r, 0);
    }

    // NOTE: nothing is to be returned!!!
    return FALSE;
  }

  WarnS("ComputeLeadingSyzygyTerms needs a single ideal/module argument (must be a variable!)...");
  return TRUE;
}


static BOOLEAN _Compute2LeadingSyzygyTerms(leftv res, leftv h)
{
  const SchreyerSyzygyComputationFlags attributes(currRingHdl);

  const BOOLEAN OPT__DEBUG      = attributes.OPT__DEBUG;
//  const BOOLEAN OPT__SYZCHECK   = attributes.OPT__SYZCHECK;
  const BOOLEAN OPT__LEAD2SYZ   = attributes.OPT__LEAD2SYZ;
//  const BOOLEAN OPT__HYBRIDNF   = attributes.OPT__HYBRIDNF;
//  const BOOLEAN OPT__TAILREDSYZ = attributes.OPT__TAILREDSYZ;

  const ring r = attributes.m_rBaseRing;
  NoReturn(res);

  if( h == NULL )
  {
    WarnS("Compute2LeadingSyzygyTerms needs an argument...");
    return TRUE;
  }

  assume( h != NULL );

  assume( OPT__LEAD2SYZ ); // ???

  if( h->Typ() == IDEAL_CMD || h->Typ() == MODUL_CMD)
  {
    const ideal id = (const ideal)h->Data();

    assume(id != NULL);

    if( UNLIKELY( OPT__DEBUG ) )
    {
      PrintS("Compute2LeadingSyzygyTerms::Input: \n");
      dPrint(id, r, r, 0);
    }

    h = h->Next(); assume (h == NULL);

    res->data = Compute2LeadingSyzygyTerms(id, attributes);
    res->rtyp = MODUL_CMD;

    return FALSE;
  }

  WarnS("Compute2LeadingSyzygyTerms needs a single ideal/module argument...");
  return TRUE;
}



/// proc SSFindReducer(def product, def syzterm, def L, def T, list #)
static BOOLEAN _FindReducer(leftv res, leftv h)
{
  const SchreyerSyzygyComputationFlags attributes(currRingHdl);

  const BOOLEAN OPT__DEBUG      = attributes.OPT__DEBUG;
//   const BOOLEAN OPT__SYZCHECK   = attributes.OPT__SYZCHECK;
//   const BOOLEAN OPT__LEAD2SYZ   = attributes.OPT__LEAD2SYZ;
//   const BOOLEAN OPT__HYBRIDNF   = attributes.OPT__HYBRIDNF;
  const BOOLEAN OPT__TAILREDSYZ = attributes.OPT__TAILREDSYZ;

  const char* usage = "`FindReducer(<poly/vector>, <vector/0>, <ideal/module>[,<module>])` expected";
  const ring r = attributes.m_rBaseRing;

  NoReturn(res);


  if ((h==NULL) || (h->Typ()!=VECTOR_CMD && h->Typ() !=POLY_CMD) || (h->Data() == NULL))
  {
    WerrorS(usage);
    return TRUE;
  }

  const poly product = (poly) h->Data(); assume (product != NULL);


  h = h->Next();
  if ((h==NULL) || !((h->Typ()==VECTOR_CMD) || (h->Data() == NULL)) )
  {
    WerrorS(usage);
    return TRUE;
  }

  poly syzterm = NULL;

  if(h->Typ()==VECTOR_CMD)
    syzterm = (poly) h->Data();



  h = h->Next();
  if ((h==NULL) || (h->Typ()!=IDEAL_CMD && h->Typ() !=MODUL_CMD) || (h->Data() == NULL))
  {
    WerrorS(usage);
    return TRUE;
  }

  const ideal L = (ideal) h->Data(); h = h->Next();

  assume( IDELEMS(L) > 0 );

  ideal LS = NULL;

  if ((h != NULL) && (h->Typ() ==MODUL_CMD) && (h->Data() != NULL))
  {
    LS = (ideal)h->Data();
    h = h->Next();
  }

#ifndef SING_NDEBUG
  if( LIKELY( OPT__TAILREDSYZ) )
    assume (LS != NULL);
#endif

  assume( h == NULL );

  if( UNLIKELY(OPT__DEBUG) )
  {
    PrintS("FindReducer(product, syzterm, L, T, #)::Input: \n");

    PrintS("product: "); dPrint(product, r, r, 0);
    PrintS("syzterm: "); dPrint(syzterm, r, r, 0);
//    PrintS("L: "); dPrint(L, r, r, 0);
//    PrintS("T: "); dPrint(T, r, r, 0);

    if( LS == NULL )
//      PrintS("LS: NULL\n");
      ;
    else
    {
//      PrintS("LS: "); dPrint(LS, r, r, 0);
    }
  }

  res->rtyp = VECTOR_CMD;
  res->data = FindReducer(product, syzterm, L, LS, attributes);

  if( UNLIKELY( OPT__DEBUG ) )
  {
    PrintS("FindReducer::Output: \n");
    dPrint((poly)res->data, r, r, 0);
  }

  return FALSE;

}

// proc SchreyerSyzygyNF(vector syz_lead, vector syz_2, def L, def T, list #)
static BOOLEAN _SchreyerSyzygyNF(leftv res, leftv h)
{
  const SchreyerSyzygyComputationFlags attributes(currRingHdl);

  const BOOLEAN OPT__DEBUG      = attributes.OPT__DEBUG;
//   const BOOLEAN OPT__SYZCHECK   = attributes.OPT__SYZCHECK;
//   const BOOLEAN OPT__LEAD2SYZ   = attributes.OPT__LEAD2SYZ;
  const BOOLEAN OPT__HYBRIDNF   = attributes.OPT__HYBRIDNF;
  const BOOLEAN OPT__TAILREDSYZ = attributes.OPT__TAILREDSYZ;

  const char* usage = "`SchreyerSyzygyNF(<vector>, <vector>, <ideal/module>, <ideal/module>[,<module>])` expected";
  const ring r = attributes.m_rBaseRing;

  NoReturn(res);

  assume( OPT__HYBRIDNF ); // ???

  if ((h==NULL) || (h->Typ() != VECTOR_CMD) || (h->Data() == NULL))
  {
    WerrorS(usage);
    return TRUE;
  }

  const poly syz_lead = (poly) h->Data(); assume (syz_lead != NULL);


  h = h->Next();
  if ((h==NULL) || (h->Typ() != VECTOR_CMD) || (h->Data() == NULL))
  {
    WerrorS(usage);
    return TRUE;
  }

  const poly syz_2 = (poly) h->Data(); assume (syz_2 != NULL);

  h = h->Next();
  if ((h==NULL) || (h->Typ()!=IDEAL_CMD && h->Typ() !=MODUL_CMD) || (h->Data() == NULL))
  {
    WerrorS(usage);
    return TRUE;
  }

  const ideal L = (ideal) h->Data(); assume( IDELEMS(L) > 0 );


  h = h->Next();
  if ((h==NULL) || (h->Typ()!=IDEAL_CMD && h->Typ() !=MODUL_CMD) || (h->Data() == NULL))
  {
    WerrorS(usage);
    return TRUE;
  }

  const ideal T = (ideal) h->Data();

  assume( IDELEMS(L) == IDELEMS(T) );

  ideal LS = NULL;

  h = h->Next();
  if ((h != NULL) && (h->Typ() ==MODUL_CMD) && (h->Data() != NULL))
  {
    LS = (ideal)h->Data();
    h = h->Next();
  }

#ifndef SING_NDEBUG
  if( LIKELY( OPT__TAILREDSYZ) )
    assume (LS != NULL);
#endif

  assume( h == NULL );

  if( UNLIKELY( OPT__DEBUG ) )
  {
    PrintS("SchreyerSyzygyNF(syz_lead, syz_2, L, T, #)::Input: \n");

    PrintS("syz_lead: "); dPrint(syz_lead, r, r, 0);
    PrintS("syz_2: "); dPrint(syz_2, r, r, 0);

//    PrintS("L: "); dPrint(L, r, r, 0);
//    PrintS("T: "); dPrint(T, r, r, 0);

    if( LS == NULL )
//      PrintS("LS: NULL\n")
          ;
    else
    {
//      PrintS("LS: "); dPrint(LS, r, r, 0);
    }
  }

  res->rtyp = VECTOR_CMD;
  res->data = SchreyerSyzygyNF(syz_lead,
                               (syz_2!=NULL)? p_Copy(syz_2, r): syz_2, L, T, LS, attributes);

  if( UNLIKELY( OPT__DEBUG ) )
  {
    PrintS("SchreyerSyzygyNF::Output: ");

    dPrint((poly)res->data, r, r, 0);
  }


  return FALSE;
}



/// proc SSReduceTerm(poly m, def t, def syzterm, def L, def T, list #)
static BOOLEAN _ReduceTerm(leftv res, leftv h)
{
  const SchreyerSyzygyComputationFlags attributes(currRingHdl);

  const BOOLEAN OPT__DEBUG      = attributes.OPT__DEBUG;
//  const BOOLEAN OPT__SYZCHECK   = attributes.OPT__SYZCHECK;
//   const BOOLEAN OPT__LEAD2SYZ   = attributes.OPT__LEAD2SYZ;
//   const BOOLEAN OPT__HYBRIDNF   = attributes.OPT__HYBRIDNF;
  const BOOLEAN OPT__TAILREDSYZ = attributes.OPT__TAILREDSYZ;

  const char* usage = "`ReduceTerm(<poly>, <poly/vector>, <vector/0>, <ideal/module>, <ideal/module>[,<module>])` expected";
  const ring r = attributes.m_rBaseRing;

  NoReturn(res);

  if ((h==NULL) || (h->Typ() !=POLY_CMD) || (h->Data() == NULL))
  {
    WerrorS(usage);
    return TRUE;
  }

  const poly multiplier = (poly) h->Data(); assume (multiplier != NULL);


  h = h->Next();
  if ((h==NULL) || (h->Typ()!=VECTOR_CMD && h->Typ() !=POLY_CMD) || (h->Data() == NULL))
  {
    WerrorS(usage);
    return TRUE;
  }

  const poly term4reduction = (poly) h->Data(); assume( term4reduction != NULL );


  poly syztermCheck = NULL;

  h = h->Next();
  if ((h==NULL) || !((h->Typ()==VECTOR_CMD) || (h->Data() == NULL)) )
  {
    WerrorS(usage);
    return TRUE;
  }

  if(h->Typ()==VECTOR_CMD)
    syztermCheck = (poly) h->Data();


  h = h->Next();
  if ((h==NULL) || (h->Typ()!=IDEAL_CMD && h->Typ() !=MODUL_CMD) || (h->Data() == NULL))
  {
    WerrorS(usage);
    return TRUE;
  }

  const ideal L = (ideal) h->Data(); assume( IDELEMS(L) > 0 );


  h = h->Next();
  if ((h==NULL) || (h->Typ()!=IDEAL_CMD && h->Typ() !=MODUL_CMD) || (h->Data() == NULL))
  {
    WerrorS(usage);
    return TRUE;
  }

  const ideal T = (ideal) h->Data();

  assume( IDELEMS(L) == IDELEMS(T) );

  ideal LS = NULL;

  h = h->Next();
  if ((h != NULL) && (h->Typ() ==MODUL_CMD) && (h->Data() != NULL))
  {
    LS = (ideal)h->Data();
    h = h->Next();
  }

#ifndef SING_NDEBUG
  if( LIKELY( OPT__TAILREDSYZ) )
    assume (LS != NULL);
#endif

  assume( h == NULL );

  if( UNLIKELY( OPT__DEBUG ) )
  {
    PrintS("ReduceTerm(m, t, syzterm, L, T, #)::Input: \n");

    PrintS("m: "); dPrint(multiplier, r, r, 0);
    PrintS("t: "); dPrint(term4reduction, r, r, 0);
    PrintS("syzterm: "); dPrint(syztermCheck, r, r, 0);

//    PrintS("L: "); dPrint(L, r, r, 0);
//    PrintS("T: "); dPrint(T, r, r, 0);

    if( LS == NULL )
//      PrintS("LS: NULL\n")
          ;
    else
    {
//      PrintS("LS: "); dPrint(LS, r, r, 0);
    }
  }


  if ( UNLIKELY( OPT__DEBUG && syztermCheck != NULL) )
  {
    const int c = p_GetComp(syztermCheck, r) - 1;
    assume( c >= 0 && c < IDELEMS(L) );

    const poly p = L->m[c];
    assume( p != NULL ); assume( pNext(p) == NULL );

    assume( p_EqualPolys(term4reduction, p, r) ); // assume? TODO


    poly m = leadmonom(syztermCheck, r);
    assume( m != NULL ); assume( pNext(m) == NULL );

    assume( p_EqualPolys(multiplier, m, r) ); // assume? TODO

    p_Delete(&m, r);

// NOTE:   leadmonomial(syzterm) == m &&  L[leadcomp(syzterm)] == t
  }

  res->rtyp = VECTOR_CMD;
  res->data = ReduceTerm(multiplier, term4reduction, syztermCheck, L, T, LS, attributes);


  if( UNLIKELY( OPT__DEBUG ) )
  {
    PrintS("ReduceTerm::Output: ");

    dPrint((poly)res->data, r, r, 0);
  }


  return FALSE;
}




// proc SSTraverseTail(poly m, def @tail, def L, def T, list #)
static BOOLEAN _TraverseTail(leftv res, leftv h)
{
  const SchreyerSyzygyComputationFlags attributes(currRingHdl);

  const BOOLEAN OPT__DEBUG      = attributes.OPT__DEBUG;
//   const BOOLEAN OPT__SYZCHECK   = attributes.OPT__SYZCHECK;
//   const BOOLEAN OPT__LEAD2SYZ   = attributes.OPT__LEAD2SYZ;
//   const BOOLEAN OPT__HYBRIDNF   = attributes.OPT__HYBRIDNF;
  const BOOLEAN OPT__TAILREDSYZ = attributes.OPT__TAILREDSYZ;

  const char* usage = "`TraverseTail(<poly>, <poly/vector>, <ideal/module>, <ideal/module>[,<module>])` expected";
  const ring r = attributes.m_rBaseRing;

  NoReturn(res);

  if ((h==NULL) || (h->Typ() !=POLY_CMD) || (h->Data() == NULL))
  {
    WerrorS(usage);
    return TRUE;
  }

  const poly multiplier = (poly) h->Data(); assume (multiplier != NULL);

  h = h->Next();
  if ((h==NULL) || (h->Typ()!=VECTOR_CMD && h->Typ() !=POLY_CMD))
  {
    WerrorS(usage);
    return TRUE;
  }

  const poly tail = (poly) h->Data();

  h = h->Next();

  if ((h==NULL) || (h->Typ()!=IDEAL_CMD && h->Typ() !=MODUL_CMD) || (h->Data() == NULL))
  {
    WerrorS(usage);
    return TRUE;
  }

  const ideal L = (ideal) h->Data();

  assume( IDELEMS(L) > 0 );

  h = h->Next();
  if ((h==NULL) || (h->Typ()!=IDEAL_CMD && h->Typ() !=MODUL_CMD) || (h->Data() == NULL))
  {
    WerrorS(usage);
    return TRUE;
  }

  const ideal T = (ideal) h->Data();

  assume( IDELEMS(L) == IDELEMS(T) );

  h = h->Next();

  ideal LS = NULL;

  if ((h != NULL) && (h->Typ() ==MODUL_CMD) && (h->Data() != NULL))
  {
    LS = (ideal)h->Data();
    h = h->Next();
  }

#ifndef SING_NDEBUG
  if( LIKELY( OPT__TAILREDSYZ) )
    assume (LS != NULL);
#endif

  assume( h == NULL );

  if( UNLIKELY( OPT__DEBUG ) )
  {
    PrintS("TraverseTail(m, t, L, T, #)::Input: \n");

    PrintS("m: "); dPrint(multiplier, r, r, 0);
    PrintS("t: "); dPrint(tail, r, r, 0);

//    PrintS("L: "); dPrint(L, r, r, 0);
//    PrintS("T: "); dPrint(T, r, r, 0);

    if( LS == NULL )
//      PrintS("LS: NULL\n")
          ;
    else
    {
//      PrintS("LS: "); dPrint(LS, r, r, 0);
    }
  }

  res->rtyp = VECTOR_CMD;
  res->data = TraverseTail(multiplier, tail, L, T, LS, attributes);


  if( UNLIKELY( OPT__DEBUG ) )
  {
    PrintS("TraverseTail::Output: ");
    dPrint((poly)res->data, r, r, 0);
  }

  return FALSE;
}


static BOOLEAN _ComputeResolution(leftv res, leftv h)
{
  const SchreyerSyzygyComputationFlags attributes(currRingHdl);

  const BOOLEAN OPT__DEBUG      = attributes.OPT__DEBUG;

  const char* usage = "`ComputeResolution(<ideal/module>, <same as before>, <same as before>[,int])` expected";
  const ring r = attributes.m_rBaseRing;

  NoReturn(res);

  // input
  if ((h==NULL) || (h->Typ()!=IDEAL_CMD && h->Typ() !=MODUL_CMD) || (h->Data() == NULL))
  {
    WerrorS(usage);
    return TRUE;
  }

  const int type = h->Typ();
  ideal M = (ideal)(h->CopyD()); // copy for resolution...!???
  int size = IDELEMS(M);

  assume( size >= 0 );

  h = h->Next();

  // lead
  if ((h==NULL) || (h->Typ()!=type) || (h->Data() == NULL))
  {
    WerrorS(usage);
    return TRUE;
  }

  ideal L = (ideal)(h->CopyD()); // no copy!
  assume( IDELEMS(L) == size );

  h = h->Next();
  if ((h==NULL) || (h->Typ()!=type) || (h->Data() == NULL))
  {
    WerrorS(usage);
    return TRUE;
  }

  ideal T = (ideal)(h->CopyD()); // no copy!
  assume( IDELEMS(T) == size );

  h = h->Next();

  // length..?
  long length = 0;

  if ((h!=NULL) && (h->Typ()==INT_CMD))
  {
    length = (long)(h->Data());
    h = h->Next();
  }

  assume( h == NULL );

  if( length <= 0 )
    length = 1 + rVar(r);

  if( UNLIKELY( OPT__DEBUG ) )
  {
    PrintS("ComputeResolution(M, length)::Input: \n");
    Print( "starting length: %ld\n", length);
    PrintS("M: \n"); dPrint(M, r, r, 0);
    PrintS("L=LEAD(M): \n"); dPrint(L, r, r, 0);
    PrintS("T=TAIL(M): \n"); dPrint(T, r, r, 0);
  }


  syStrategy _res=(syStrategy)omAlloc0(sizeof(ssyStrategy));

//  class ssyStrategy; typedef ssyStrategy * syStrategy;
//  typedef ideal *            resolvente;

  _res->length = length + 1; // index + 1;
  _res->fullres = (resolvente)omAlloc0((_res->length+1)*sizeof(ideal));
  int index = 0;
  _res->fullres[index++] = M;

//  if (UNLIKELY(attributes.OPT__TREEOUTPUT))
//    Print("{ \"RESOLUTION: HYBRIDNF:%d, TAILREDSYZ: %d, LEAD2SYZ: %d, IGNORETAILS: %d\": [\n", attributes.OPT__HYBRIDNF, attributes.OPT__TAILREDSYZ, attributes.OPT__LEAD2SYZ, attributes.OPT__IGNORETAILS);

  while( (!idIs0(L)) && (index < length))
  {
    attributes.nextSyzygyLayer();
    ideal LL, TT;

    ComputeSyzygy(L, T, LL, TT, attributes);

    if( UNLIKELY( OPT__DEBUG ) )
    {
      Print("ComputeResolution()::Separated Syzygy[%d]: \n", index);
//      PrintS("LL: \n"); dPrint(LL, r, r, 0);
//      PrintS("TT: \n"); dPrint(TT, r, r, 0);
    }
    size = IDELEMS(LL);

    assume( size == IDELEMS(TT) );

    id_Delete(&L, r); id_Delete(&T, r);

    L = LL; T = TT;

    // id_Add(T, L, r);
    M = idInit(size, 0);
    for( int i = size-1; i >= 0; i-- )
    {
      M->m[i] = p_Add_q(p_Copy(T->m[i], r), p_Copy(L->m[i], r), r); // TODO: :(((
    }
    M->rank = id_RankFreeModule(M, r);

    if( UNLIKELY( OPT__DEBUG ) )
    {
      Print("ComputeResolution()::Restored Syzygy[%d]: \n", index);
      PrintS("M = LL + TT: \n"); dPrint(M, r, r, 0);
    }

    _res->fullres[index++] = M; // ???
  }
//  if ( UNLIKELY(attributes.OPT__TREEOUTPUT) )
//    PrintS("] }\n");

  id_Delete(&L, r); id_Delete(&T, r);

  res->data = _res;
  res->rtyp = RESOLUTION_CMD;

  if( UNLIKELY(OPT__DEBUG) )
  {
    Print("ComputeResolution::Output (index: %d): ", index);
//    class sleftv; typedef sleftv * leftv;
    sleftv _h;
    DetailedPrint(&_h, res);
  }

//  omFreeSize(_res, sizeof(ssyStrategy));

  return FALSE;

}


/// module (LL, TT) = SSComputeSyzygy(L, T);
/// Compute Syz(L ++ T) = N = LL ++ TT
// proc SSComputeSyzygy(def L, def T)
static BOOLEAN _ComputeSyzygy(leftv res, leftv h)
{
  const SchreyerSyzygyComputationFlags attributes(currRingHdl);

  const BOOLEAN OPT__DEBUG      = attributes.OPT__DEBUG;
//   const BOOLEAN OPT__SYZCHECK   = attributes.OPT__SYZCHECK;
//   const BOOLEAN OPT__LEAD2SYZ   = attributes.OPT__LEAD2SYZ;
//   const BOOLEAN OPT__HYBRIDNF   = attributes.OPT__HYBRIDNF;
//   const BOOLEAN OPT__TAILREDSYZ = attributes.OPT__TAILREDSYZ;

  const char* usage = "`ComputeSyzygy(<ideal/module>, <ideal/module>)` expected";
  const ring r = attributes.m_rBaseRing;

  NoReturn(res);

  if ((h==NULL) || (h->Typ()!=IDEAL_CMD && h->Typ() !=MODUL_CMD) || (h->Data() == NULL))
  {
    WerrorS(usage);
    return TRUE;
  }

  const ideal L = (ideal) h->Data();

  assume( IDELEMS(L) > 0 );

  h = h->Next();
  if ((h==NULL) || (h->Typ()!=IDEAL_CMD && h->Typ() !=MODUL_CMD) || (h->Data() == NULL))
  {
    WerrorS(usage);
    return TRUE;
  }

  const ideal T = (ideal) h->Data();
  assume( IDELEMS(L) == IDELEMS(T) );


  h = h->Next(); assume( h == NULL );

  if( UNLIKELY( OPT__DEBUG ) )
  {
    PrintS("ComputeSyzygy(L, T)::Input: \n");
//    PrintS("L: "); dPrint(L, r, r, 0);
//    PrintS("T: "); dPrint(T, r, r, 0);
  }

  ideal LL, TT;

  ComputeSyzygy(L, T, LL, TT, attributes);

  lists l = (lists)omAllocBin(slists_bin); l->Init(2);

  l->m[0].rtyp = MODUL_CMD; l->m[0].data = reinterpret_cast<void *>(LL);

  l->m[1].rtyp = MODUL_CMD; l->m[1].data = reinterpret_cast<void *>(TT);

  res->data = l; res->rtyp = LIST_CMD;

  if( UNLIKELY( OPT__DEBUG ) )
  {
    PrintS("ComputeSyzygy::Output: \nLL: \n");
    dPrint(LL, r, r, 0);
    PrintS("\nTT: \n");
    dPrint(TT, r, r, 0);
  }

  return FALSE;

}

/// Get leading term without a module component
static BOOLEAN _leadmonom(leftv res, leftv h)
{
  NoReturn(res);

  if ((h!=NULL) && (h->Typ()==VECTOR_CMD || h->Typ()==POLY_CMD) && (h->Data() != NULL))
  {
    const ring r = currRing;
    const poly p = (poly)(h->Data());

    res->data = reinterpret_cast<void *>(  leadmonom(p, r) );
    res->rtyp = POLY_CMD;

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
      p_LmTest(p, r);

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
    p_LmTest(p, r);

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


/* // the following turned out to be unnecessary...
/// Finds p^th AM ordering, and returns its position in r->typ[] AND
/// corresponding &r->wvhdl[]
/// returns FALSE if something went wrong!
/// p - starts with 0!
BOOLEAN rGetAMPos(const ring r, const int p, int &typ_pos, int &wvhdl_pos, const BOOLEAN bSearchWvhdl = FALSE)
{
#if MYTEST
  Print("rGetAMPos(p: %d...)\nF:", p);
  PrintLn();
#endif
  typ_pos = -1;
  wvhdl_pos = -1;

  if (r->typ==NULL)
    return FALSE;


  int j = p; // Which IS record to use...
  for( int pos = 0; pos < r->OrdSize; pos++ )
    if( r->typ[pos].ord_typ == ro_am)
      if( j-- == 0 )
      {
        typ_pos = pos;

        if( bSearchWvhdl )
        {
          const int nblocks = rBlocks(r) - 1;
          const int* w = r->typ[pos].data.am.weights; // ?

          for( pos = 0; pos <= nblocks; pos ++ )
            if (r->order[pos] == ringorder_am)
              if( r->wvhdl[pos] == w )
              {
                wvhdl_pos = pos;
                break;
              }
          if (wvhdl_pos < 0)
            return FALSE;

          assume(wvhdl_pos >= 0);
        }
        assume(typ_pos >= 0);
        return TRUE;
      }

  return FALSE;
}

// // ?
// static BOOLEAN GetAMData(leftv res, leftv h)
// {
//   NoReturn(res);
//
//   const ring r = currRing;
//
//   int p = 0; // which IS-block? p^th!
//
//   if ((h!=NULL) && (h->Typ()==INT_CMD))
//     p = (int)((long)(h->Data())); h=h->next;
//
//   assume(p >= 0);
//
//   int d, w;
//
//   if( !rGetAMPos(r, p, d, w, TRUE) )
//   {
//     Werror("`GetAMData([int])`: no %d^th _am block-ordering!", p);
//     return TRUE;
//   }
//
//   assume( r->typ[d].ord_typ == ro_am );
//   assume( r->order[w] == ringorder_am );
//
//
//   const short start = r->typ[d].data.am.start;  // bounds of ordering (in E)
//   const short end = r->typ[d].data.am.end;
//   const short len_gen = r->typ[d].data.am.len_gen; // i>len_gen: weight(gen(i)):=0
//   const int *weights = r->typ[d].data.am.weights; // pointers into wvhdl field of length (end-start+1) + len_gen
//   // contents w_1,... w_n, len, mod_w_1, .. mod_w_len, 0
//
//   assume( weights == r->wvhdl[w] );
//
//
//   lists l=(lists)omAllocBin(slists_bin);
//   l->Init(2);
//
//   const short V = end-start+1;
//   intvec* ww_vars = new intvec(V);
//   intvec* ww_gens = new intvec(len_gen);
//
//   for (int i = 0; i < V; i++ )
//     (*ww_vars)[i] = weights[i];
//
//   assume( weights[V] == len_gen );
//
//   for (int i = 0; i < len_gen; i++ )
//     (*ww_gens)[i] = weights[i - V - 1];
//
//
//   l->m[0].rtyp = INTVEC_CMD;
//   l->m[0].data = reinterpret_cast<void *>(ww_vars);
//
//   l->m[1].rtyp = INTVEC_CMD;
//   l->m[1].data = reinterpret_cast<void *>(ww_gens);
//
//
//   return FALSE;
//
// }
*/

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

  res->data = (void *)kNFLength(M, currRing->qideal, v, iSyzComp, iLazyReduce); // NOTE: currRing :(
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
  return FALSE;
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

// no args.
// init num stats
static BOOLEAN _NumberStatsInit(leftv res, leftv h)
{
  if ( (h!=NULL) && (h->Typ()!=INT_CMD) )
  {
    WerrorS("`NumberStatsInit([<int>])` expected");
    return TRUE;
  }

  unsigned long v = 0;

  if( h != NULL )
    v = (unsigned long)(h->Data());

  number_stats_Init(v);

  NoReturn(res);
  return FALSE;
}

// maybe one arg.
// print num stats
static BOOLEAN _NumberStatsPrint(leftv res, leftv h)
{
  if ( (h!=NULL) && (h->Typ()!=STRING_CMD) )
  {
    WerrorS("`NumberStatsPrint([<string>])` expected");
    return TRUE;
  }

  const char* msg = NULL;

  if( h != NULL )
    msg = (const char*)(h->Data());

  number_stats_Print(msg);

  NoReturn(res);
  return FALSE;
}

END_NAMESPACE

extern "C" int SI_MOD_INIT(syzextra)(SModulFunctions* psModulFunctions)
{

#define ADD(C,D,E) \
  psModulFunctions->iiAddCproc((currPack->libname? currPack->libname: ""), (char*)C, D, E);


// #define ADD(A,B,C,D,E) ADD0(iiAddCproc, "", C, D, E)

//#define ADD0(A,B,C,D,E) A(B, (char*)C, D, E)
// #define ADD(A,B,C,D,E) ADD0(A->iiAddCproc, B, C, D, E)
  ADD("ClearContent", FALSE, _ClearContent);
  ADD("ClearDenominators", FALSE, _ClearDenominators);

  ADD("m2_end", FALSE, _m2_end);

  ADD("DetailedPrint", FALSE, DetailedPrint);
  ADD("leadmonomial", FALSE, _leadmonom);
  ADD("leadcomp", FALSE, leadcomp);
  ADD("leadrawexp", FALSE, leadrawexp);

  ADD("ISUpdateComponents", FALSE, ISUpdateComponents);
  ADD("SetInducedReferrence", FALSE, SetInducedReferrence);
  ADD("GetInducedData", FALSE, GetInducedData);
  ADD("SetSyzComp", FALSE, SetSyzComp);
  ADD("MakeInducedSchreyerOrdering", FALSE, MakeInducedSchreyerOrdering);
  ADD("MakeSyzCompOrdering", FALSE, MakeSyzCompOrdering);

  ADD("ProfilerStart", FALSE, _ProfilerStart);
  ADD("ProfilerStop",  FALSE, _ProfilerStop );

  ADD("noop", FALSE, noop);
  ADD("idPrepare", FALSE, idPrepare);
  ADD("reduce_syz", FALSE, reduce_syz);

  ADD("p_Content", FALSE, _p_Content);

  ADD("Tail", FALSE, Tail);

  ADD("ComputeLeadingSyzygyTerms", FALSE, _ComputeLeadingSyzygyTerms);
  ADD("Compute2LeadingSyzygyTerms", FALSE, _Compute2LeadingSyzygyTerms);

  ADD("Sort_c_ds", FALSE, _Sort_c_ds);
  ADD("FindReducer", FALSE, _FindReducer);


  ADD("ReduceTerm", FALSE, _ReduceTerm);
  ADD("TraverseTail", FALSE, _TraverseTail);


  ADD("SchreyerSyzygyNF", FALSE, _SchreyerSyzygyNF);
  ADD("ComputeSyzygy", FALSE, _ComputeSyzygy);

  ADD("ComputeResolution", FALSE, _ComputeResolution);
//  ADD("GetAMData", FALSE, GetAMData);

  ADD("NumberStatsInit", FALSE, _NumberStatsInit);
  ADD("NumberStatsPrint", FALSE, _NumberStatsPrint);

  //  ADD("", FALSE, );

#undef ADD
  return MAX_TOK;
}
