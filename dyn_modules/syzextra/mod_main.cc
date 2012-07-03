



#include <kernel/mod2.h>

#include <omalloc/omalloc.h>

#include <misc/intvec.h>
#include <misc/options.h>

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
#include "syzextra.h"


#include <Singular/mod_lib.h>


#if GOOGLE_PROFILE_ENABLED 
#include <google/profiler.h>
#endif // #if GOOGLE_PROFILE_ENABLED 


#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#ifdef _GNU_SOURCE
#define qsort_my(m, s, ss, r, cmp) qsort_r(m, s, ss, cmp, r)
#else
#define qsort_my(m, s, ss, r, cmp) qsort_r(m, s, ss, cmp)
#endif



extern void pISUpdateComponents(ideal F, const intvec *const V, const int MIN, const ring r);
// extern ring rCurrRingAssure_SyzComp();
extern ring rAssure_InducedSchreyerOrdering(const ring r, BOOLEAN complete, int sign);
extern int rGetISPos(const int p, const ring r);

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

#ifdef LDEBUG
    r->cf->cfDBTest(n,__FILE__,__LINE__,r->cf);
#endif

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


#ifdef _GNU_SOURCE
static int cmp_c_ds(const void *p1, const void *p2, void *R)
{
#else
static int cmp_c_ds(const void *p1, const void *p2)
{
  void *R = currRing; 
#endif

  const int YES = 1;
  const int NO = -1;

  const ring r =  (const ring) R; // TODO/NOTE: the structure is known: C, lp!!!

  assume( r == currRing );

  const poly a = *(const poly*)p1;
  const poly b = *(const poly*)p2;

  assume( a != NULL );
  assume( b != NULL );
  
  assume( p_LmTest(a, r) );
  assume( p_LmTest(b, r) );


  const signed long iCompDiff = p_GetComp(a, r) - p_GetComp(b, r);

  // TODO: test this!!!!!!!!!!!!!!!!

  //return -( compare (c, qsorts) )

  const int __DEBUG__ = 0;

#ifndef NDEBUG
  if( __DEBUG__ )
  {
    PrintS("cmp_c_ds: a, b: \np1: "); dPrint(a, r, r, 2);
    PrintS("b: "); dPrint(b, r, r, 2);
    PrintLn();
  }
#endif


  if( iCompDiff > 0 )
    return YES;

  if( iCompDiff < 0 )
    return  NO;

  assume( iCompDiff == 0 );

  const signed long iDegDiff = p_Totaldegree(a, r) - p_Totaldegree(b, r);

  if( iDegDiff > 0 )
    return YES;

  if( iDegDiff < 0 )
    return  NO;

  assume( iDegDiff == 0 );

#ifndef NDEBUG
  if( __DEBUG__ )
  {
    PrintS("cmp_c_ds: a & b have the same comp & deg! "); PrintLn();
  }
#endif

  for (int v = rVar(r); v > 0; v--)
  {
    assume( v > 0 );
    assume( v <= rVar(r) );

    const signed int d = p_GetExp(a, v, r) - p_GetExp(b, v, r);

    if( d > 0 )
      return YES;

    if( d < 0 )
      return NO;

    assume( d == 0 );
  }

  return 0;  
}



static ideal ComputeLeadingSyzygyTerms(const ideal& id, const ring r)
{
  // 1. set of components S?
  // 2. for each component c from S: set of indices of leading terms
  // with this component?
  // 3. short exp. vectors for each leading term?

  const int size = IDELEMS(id);

  if( size < 2 )
  {
    const ideal newid = idInit(1, 0); newid->m[0] = NULL; // zero ideal...        
    return newid;
  }


  // TODO/NOTE: input is supposed to be (reverse-) sorted wrt "(c,ds)"!??

  // components should come in groups: count elements in each group
  // && estimate the real size!!!


  // use just a vector instead???
  const ideal newid = idInit( (size * (size-1))/2, size); // maximal size: ideal case!

  int k = 0;

  for (int j = 0; j < size; j++)
  {
    const poly p = id->m[j];
    assume( p != NULL );
    const int  c = p_GetComp(p, r);

    for (int i = j - 1; i >= 0; i--)
    {
      const poly pp = id->m[i];
      assume( pp != NULL );
      const int  cc = p_GetComp(pp, r);

      if( c != cc )
        continue;

      const poly m = p_Init(r); // p_New???

      // m = LCM(p, pp) / p! // TODO: optimize: knowing the ring structure: (C/lp)!
      for (int v = rVar(r); v > 0; v--)
      {
        assume( v > 0 );
        assume( v <= rVar(r) );

        const short e1 = p_GetExp(p , v, r);
        const short e2 = p_GetExp(pp, v, r);

        if( e1 >= e2 )
          p_SetExp(m, v, 0, r);
        else
          p_SetExp(m, v, e2 - e1, r);

      }

      assume( (j > i) && (i >= 0) );

      p_SetComp(m, j + 1, r);
      pNext(m) = NULL;
      p_SetCoeff0(m, n_Init(1, r->cf), r); // for later...

      p_Setm(m, r); // should not do anything!!!

      newid->m[k++] = m;
    }
  }

//   if( __DEBUG__ && FALSE )
//   {
//     PrintS("ComputeLeadingSyzygyTerms::Temp0: \n");
//     dPrint(newid, r, r, 1);
//   }

  // the rest of newid is assumed to be zeroes...

  // simplify(newid, 2 + 32)??
  // sort(newid, "C,ds")[1]???
  id_DelDiv(newid, r); // #define SIMPL_LMDIV 32

//   if( __DEBUG__ && FALSE )
//   {
//     PrintS("ComputeLeadingSyzygyTerms::Temp1: \n");
//     dPrint(newid, r, r, 1);
//   }

  idSkipZeroes(newid); // #define SIMPL_NULL 2

//   if( __DEBUG__ )
//   {
//     PrintS("ComputeLeadingSyzygyTerms::Output: \n");
//     dPrint(newid, r, r, 1);
//   }

  const int sizeNew = IDELEMS(newid);

  if( sizeNew >= 2 )
    qsort_my(newid->m, sizeNew, sizeof(poly), r, cmp_c_ds);

  newid->rank = id_RankFreeModule(newid, r);

  return newid;
}


static BOOLEAN ComputeLeadingSyzygyTerms(leftv res, leftv h)
{
  const ring r = currRing;
  NoReturn(res);

  if( h == NULL )
  {
    WarnS("ComputeLeadingSyzygyTerms needs an argument...");
    return TRUE;
  }

  assume( h != NULL );  

#ifndef NDEBUG
  const BOOLEAN __DEBUG__ = (BOOLEAN)((long)(atGet(currRingHdl,"DEBUG",INT_CMD, (void*)TRUE)));
#else
  const BOOLEAN __DEBUG__ = (BOOLEAN)((long)(atGet(currRingHdl,"DEBUG",INT_CMD, (void*)FALSE)));
#endif

  if( h->Typ() == IDEAL_CMD || h->Typ() == MODUL_CMD)
  {
    const ideal id = (const ideal)h->Data();

    assume(id != NULL);

    if( __DEBUG__ )
    {
      PrintS("ComputeLeadingSyzygyTerms::Input: \n");
      
      const BOOLEAN __LEAD2SYZ__ = (BOOLEAN)((long)(atGet(currRingHdl,"LEAD2SYZ",INT_CMD, (void*)0)));
      const BOOLEAN __TAILREDSYZ__ = (BOOLEAN)((long)(atGet(currRingHdl,"TAILREDSYZ",INT_CMD, (void*)0)));
      const BOOLEAN __SYZCHECK__ = (BOOLEAN)((long)(atGet(currRingHdl,"SYZCHECK",INT_CMD, (void*)0)));


      Print("\nSYZCHECK: \t%d", __SYZCHECK__);
      Print(", DEBUG: \t%d", __DEBUG__);
      Print(", LEAD2SYZ: \t%d", __LEAD2SYZ__);
      Print(", TAILREDSYZ: \t%d\n", __TAILREDSYZ__);

      dPrint(id, r, r, 1);

      assume( !__LEAD2SYZ__ );
    }

    h = h->Next(); assume (h == NULL);

    const ideal newid = ComputeLeadingSyzygyTerms(id, r);
    
    res->data = newid; res->rtyp = MODUL_CMD;
    return FALSE;
  }

  WarnS("ComputeLeadingSyzygyTerms needs a single ideal/module argument...");
  return TRUE;
}

///  sorting wrt <c,ds> & reversing...
/// change the input inplace!!!
// TODO: use a ring with >_{c, ds}!???
static BOOLEAN Sort_c_ds(leftv res, leftv h)
{
  NoReturn(res);

  const ring r = currRing;
  NoReturn(res);

  if( h == NULL )
  {
    WarnS("Sort_c_ds needs an argument...");
    return TRUE;
  }

  assume( h != NULL );  

#ifndef NDEBUG
  const BOOLEAN __DEBUG__ = (BOOLEAN)((long)(atGet(currRingHdl,"DEBUG",INT_CMD, (void*)FALSE)));
#else
  const BOOLEAN __DEBUG__ = (BOOLEAN)((long)(atGet(currRingHdl,"DEBUG",INT_CMD, (void*)FALSE)));
#endif

  if(    (h->Typ() == IDEAL_CMD || h->Typ() == MODUL_CMD)
      && (h->rtyp  == IDHDL) // must be a variable!
      && (h->e == NULL) // not a list element
      ) 
  {
    const ideal id = (const ideal)h->Data();

    assume(id != NULL);

    if( __DEBUG__ )
    {
      PrintS("Sort_c_ds::Input: \n");

      const BOOLEAN __LEAD2SYZ__ = (BOOLEAN)((long)(atGet(currRingHdl,"LEAD2SYZ",INT_CMD, (void*)0)));
      const BOOLEAN __TAILREDSYZ__ = (BOOLEAN)((long)(atGet(currRingHdl,"TAILREDSYZ",INT_CMD, (void*)0)));
      const BOOLEAN __SYZCHECK__ = (BOOLEAN)((long)(atGet(currRingHdl,"SYZCHECK",INT_CMD, (void*)0)));   

      Print("\nSYZCHECK: \t%d", __SYZCHECK__);
      Print(", DEBUG: \t%d", __DEBUG__);
      Print(", LEAD2SYZ: \t%d", __LEAD2SYZ__);
      Print(", TAILREDSYZ: \t%d\n", __TAILREDSYZ__);

      dPrint(id, r, r, 1);      
    }

    assume (h->Next() == NULL);

    id_Test(id, r);

    const int size = IDELEMS(id);

    const ideal newid = id; // id_Copy(id, r); // copy???

    if( size >= 2 )
      qsort_my(newid->m, size, sizeof(poly), r, cmp_c_ds);
    
//    res->data = newid;
//    res->rtyp = h->Typ();
    
    if( __DEBUG__ )
    {
      PrintS("Sort_c_ds::Output: \n");
      dPrint(newid, r, r, 1);
    }

    return FALSE;
  }

  WarnS("ComputeLeadingSyzygyTerms needs a single ideal/module argument (must be a variable!)...");
  return TRUE;  
}



static ideal Compute2LeadingSyzygyTerms(const ideal& id, const ring r)
{
#ifndef NDEBUG
  const BOOLEAN __DEBUG__ = (BOOLEAN)((long)(atGet(currRingHdl,"DEBUG",INT_CMD, (void*)TRUE)));
#else
  const BOOLEAN __DEBUG__ = (BOOLEAN)((long)(atGet(currRingHdl,"DEBUG",INT_CMD, (void*)FALSE)));
#endif

  const BOOLEAN __TAILREDSYZ__ = (BOOLEAN)((long)(atGet(currRingHdl,"TAILREDSYZ",INT_CMD, (void*)0)));
  const BOOLEAN __SYZCHECK__ = (BOOLEAN)((long)(atGet(currRingHdl,"SYZCHECK",INT_CMD, (void*)0)));   



  // 1. set of components S?
  // 2. for each component c from S: set of indices of leading terms
  // with this component?
  // 3. short exp. vectors for each leading term?

  const int size = IDELEMS(id);

  if( size < 2 )
  {
    const ideal newid = idInit(1, 1); newid->m[0] = NULL; // zero module...   
    return newid;
  }


    // TODO/NOTE: input is supposed to be sorted wrt "C,ds"!??

    // components should come in groups: count elements in each group
    // && estimate the real size!!!


    // use just a vector instead???
  ideal newid = idInit( (size * (size-1))/2, size); // maximal size: ideal case!

  int k = 0;

  for (int j = 0; j < size; j++)
  {
    const poly p = id->m[j];
    assume( p != NULL );
    const int  c = p_GetComp(p, r);

    for (int i = j - 1; i >= 0; i--)
    {
      const poly pp = id->m[i];
      assume( pp != NULL );
      const int  cc = p_GetComp(pp, r);

      if( c != cc )
        continue;

        // allocate memory & zero it out!
      const poly m = p_Init(r); const poly mm = p_Init(r);


        // m = LCM(p, pp) / p! mm = LCM(p, pp) / pp!
        // TODO: optimize: knowing the ring structure: (C/lp)!

      for (int v = rVar(r); v > 0; v--)
      {
        assume( v > 0 );
        assume( v <= rVar(r) );

        const short e1 = p_GetExp(p , v, r);
        const short e2 = p_GetExp(pp, v, r);

        if( e1 >= e2 )
          p_SetExp(mm, v, e1 - e2, r); //            p_SetExp(m, v, 0, r);
        else
          p_SetExp(m, v, e2 - e1, r); //            p_SetExp(mm, v, 0, r);

      }

      assume( (j > i) && (i >= 0) );

      p_SetComp(m, j + 1, r);
      p_SetComp(mm, i + 1, r);

      const number& lc1 = p_GetCoeff(p , r);
      const number& lc2 = p_GetCoeff(pp, r);

      number g = n_Lcm( lc1, lc2, r );

      p_SetCoeff0(m ,       n_Div(g, lc1, r), r);
      p_SetCoeff0(mm, n_Neg(n_Div(g, lc2, r), r), r);

      n_Delete(&g, r);

      p_Setm(m, r); // should not do anything!!!
      p_Setm(mm, r); // should not do anything!!!

      pNext(m) = mm; //        pNext(mm) = NULL;

      newid->m[k++] = m;
    }
  }

//   if( __DEBUG__ && FALSE )
//   {
//     PrintS("Compute2LeadingSyzygyTerms::Temp0: \n");
//     dPrint(newid, r, r, 1);
//   }

  if( !__TAILREDSYZ__ )
  {
      // simplify(newid, 2 + 32)??
      // sort(newid, "C,ds")[1]???
    id_DelDiv(newid, r); // #define SIMPL_LMDIV 32

//     if( __DEBUG__ && FALSE )
//     {
//       PrintS("Compute2LeadingSyzygyTerms::Temp1 (deldiv): \n");
//       dPrint(newid, r, r, 1);
//     }
  }
  else
  {
      //      option(redSB); option(redTail);
      //      TEST_OPT_REDSB
      //      TEST_OPT_REDTAIL
    assume( r == currRing );
    BITSET _save_test = test;
    test |= (Sy_bit(OPT_REDTAIL) | Sy_bit(OPT_REDSB));

    intvec* w=new intvec(IDELEMS(newid));
    ideal tmp = kStd(newid, currQuotient, isHomog, &w);
    delete w;

    test = _save_test;

    id_Delete(&newid, r);
    newid = tmp;

//     if( __DEBUG__ && FALSE )
//     {
//       PrintS("Compute2LeadingSyzygyTerms::Temp1 (std): \n");
//       dPrint(newid, r, r, 1);
//     }

  }

  idSkipZeroes(newid);

  const int sizeNew = IDELEMS(newid);

  if( sizeNew >= 2 )
    qsort_my(newid->m, sizeNew, sizeof(poly), r, cmp_c_ds);

  newid->rank = id_RankFreeModule(newid, r);
  
  return newid;
}



static BOOLEAN Compute2LeadingSyzygyTerms(leftv res, leftv h)
{
  const ring r = currRing;
  NoReturn(res);

  if( h == NULL )
  {
    WarnS("Compute2LeadingSyzygyTerms needs an argument...");
    return TRUE;
  }

  assume( h != NULL );  

#ifndef NDEBUG
  const BOOLEAN __DEBUG__ = (BOOLEAN)((long)(atGet(currRingHdl,"DEBUG",INT_CMD, (void*)TRUE)));
#else
  const BOOLEAN __DEBUG__ = (BOOLEAN)((long)(atGet(currRingHdl,"DEBUG",INT_CMD, (void*)FALSE)));
#endif

  const BOOLEAN __LEAD2SYZ__ = (BOOLEAN)((long)(atGet(currRingHdl,"LEAD2SYZ",INT_CMD, (void*)0)));
  
  assume( __LEAD2SYZ__ );
  
  if( h->Typ() == IDEAL_CMD || h->Typ() == MODUL_CMD)
  {
    const ideal id = (const ideal)h->Data();

    assume(id != NULL);

    if( __DEBUG__ )
    {
      PrintS("Compute2LeadingSyzygyTerms::Input: \n");
      dPrint(id, r, r, 0);
    }

    h = h->Next(); assume (h == NULL);

    res->data = Compute2LeadingSyzygyTerms(id, r);
    res->rtyp = MODUL_CMD;

    return FALSE;
  }

  WarnS("Compute2LeadingSyzygyTerms needs a single ideal/module argument...");
  return TRUE;
}


/// return a new term: leading coeff * leading monomial of p
/// with 0 leading component!
static inline poly leadmonom(const poly p, const ring r)
{
  poly m = NULL;

  if( p != NULL )
  {
    assume( p != NULL );
    assume( p_LmTest(p, r) );

    m = p_LmInit(p, r);
    p_SetCoeff0(m, n_Copy(p_GetCoeff(p, r), r), r);

    p_SetComp(m, 0, r);
    p_Setm(m, r);

    assume( p_GetComp(m, r) == 0 );
    assume( m != NULL );
    assume( pNext(m) == NULL );
    assume( p_LmTest(m, r) );
  }

  return m;
}



static poly FindReducer(poly product, poly syzterm,
                        ideal L, ideal LS,
                        const ring r)
{
  assume( product != NULL );
  assume( L != NULL );

  int c = 0;

  if (syzterm != NULL)
    c = p_GetComp(syzterm, r) - 1;

  assume( c >= 0 && c < IDELEMS(L) );
  
#ifndef NDEBUG
  const BOOLEAN __DEBUG__ = (BOOLEAN)((long)(atGet(currRingHdl,"DEBUG",INT_CMD, (void*)TRUE)));
#else
  const BOOLEAN __DEBUG__ = (BOOLEAN)((long)(atGet(currRingHdl,"DEBUG",INT_CMD, (void*)FALSE)));
#endif

  long debug = __DEBUG__;
  const BOOLEAN __SYZCHECK__ = (BOOLEAN)((long)(atGet(currRingHdl,"SYZCHECK",INT_CMD, (void*)debug)));   
  
  if (__SYZCHECK__ && syzterm != NULL)
  {
    const poly m = L->m[c];

    assume( m != NULL ); assume( pNext(m) == NULL );

    poly lm = p_Mult_mm(leadmonom(syzterm, r), m, r);
    assume( p_EqualPolys(lm, product, r) );

    //  def @@c = leadcomp(syzterm); int @@r = int(@@c);
    //  def @@product = leadmonomial(syzterm) * L[@@r];

    p_Delete(&lm, r);    
  }
  
  // looking for an appropriate diviser q = L[k]...
  for( int k = IDELEMS(L)-1; k>= 0; k-- )
  {
    const poly p = L->m[k];    

    // ... which divides the product, looking for the _1st_ appropriate one!
    if( !p_LmDivisibleBy(p, product, r) )
      continue;


    const poly q = p_New(r);
    pNext(q) = NULL;
    p_ExpVectorDiff(q, product, p, r); // (LM(product) / LM(L[k]))
    p_SetComp(q, k + 1, r);
    p_Setm(q, r);

    // cannot allow something like: a*gen(i) - a*gen(i)
    if (syzterm != NULL && (k == c))
      if (p_ExpVectorEqual(syzterm, q, r))
      {
        if( __DEBUG__ )
        {
          Print("FindReducer::Test SYZTERM: q == syzterm !:((, syzterm is: ");
          dPrint(syzterm, r, r, 1);
        }    

        p_LmFree(q, r);
        continue;
      }

    // while the complement (the fraction) is not reducible by leading syzygies 
    if( LS != NULL )
    {
      BOOLEAN ok = TRUE;

      for(int kk = IDELEMS(LS)-1; kk>= 0; kk-- )
      {
        const poly pp = LS->m[kk];

        if( p_LmDivisibleBy(pp, q, r) )
        {
          
          if( __DEBUG__ )
          {
            Print("FindReducer::Test LS: q is divisible by LS[%d] !:((, diviser is: ", kk+1);
            dPrint(pp, r, r, 1);
          }    

          ok = FALSE; // q in <LS> :((
          break;                 
        }
      }

      if(!ok)
      {
        p_LmFree(q, r);
        continue;
      }
    }

    p_SetCoeff0(q, n_Neg( n_Div( p_GetCoeff(product, r), p_GetCoeff(p, r), r), r), r);
    return q;

  }

  
  return NULL;
}


/// TODO: save shortcut (syz: |-.->) LM(LM(m) * "t") -> syz?
/// proc SSFindReducer(def product, def syzterm, def L, def T, list #)
static BOOLEAN FindReducer(leftv res, leftv h)
{
  const char* usage = "`FindReducer(<poly/vector>, <vector/0>, <ideal/module>[,<module>])` expected";
  const ring r = currRing;

  NoReturn(res);


#ifndef NDEBUG
  const BOOLEAN __DEBUG__ = (BOOLEAN)((long)(atGet(currRingHdl,"DEBUG",INT_CMD, (void*)TRUE)));
#else
  const BOOLEAN __DEBUG__ = (BOOLEAN)((long)(atGet(currRingHdl,"DEBUG",INT_CMD, (void*)FALSE)));
#endif

  const BOOLEAN __TAILREDSYZ__ = (BOOLEAN)((long)(atGet(currRingHdl,"TAILREDSYZ",INT_CMD, (void*)0)));

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

  if( __TAILREDSYZ__ )
    assume (LS != NULL);

  assume( h == NULL );

  if( __DEBUG__ )
  {
    PrintS("FindReducer(product, syzterm, L, T, #)::Input: \n");

    PrintS("product: "); dPrint(product, r, r, 2);
    PrintS("syzterm: "); dPrint(syzterm, r, r, 2);
    PrintS("L: "); dPrint(L, r, r, 0);
//    PrintS("T: "); dPrint(T, r, r, 4);

    if( LS == NULL )
      PrintS("LS: NULL\n");
    else
    {
      PrintS("LS: "); dPrint(LS, r, r, 0);
    }
  }

  res->rtyp = VECTOR_CMD;
  res->data = FindReducer(product, syzterm, L, LS, r);

  if( __DEBUG__ )
  {
    PrintS("FindReducer::Output: \n");
    dPrint((poly)res->data, r, r, 2);
  }    
  
  return FALSE;   
  
}

static poly ReduceTerm(poly multiplier, poly term4reduction, poly syztermCheck,
                       ideal L, ideal T, ideal LS, const ring r);

static poly TraverseTail(poly multiplier, poly tail, 
                         ideal L, ideal T, ideal LS,
                         const ring r)
{
  assume( multiplier != NULL );

  assume( L != NULL );
  assume( T != NULL );

  poly s = NULL;

  // iterate over the tail
  for(poly p = tail; p != NULL; p = pNext(p))
    s = p_Add_q(s, ReduceTerm(multiplier, p, NULL, L, T, LS, r), r);  
    
  return s;
}


static poly ReduceTerm(poly multiplier, poly term4reduction, poly syztermCheck,
                       ideal L, ideal T, ideal LS, const ring r)
{
  assume( multiplier != NULL );
  assume( term4reduction != NULL );


  assume( L != NULL );
  assume( T != NULL );
  
  // assume(r == currRing); // ?

  // simple implementation with FindReducer:
  poly s = NULL;
  
  if(1)
  {
    // NOTE: only LT(term4reduction) should be used in the following:
    poly product = pp_Mult_mm(multiplier, term4reduction, r);
    s = FindReducer(product, syztermCheck, L, LS, r);
    p_Delete(&product, r);
  }

  if( s == NULL ) // No Reducer?
    return s;

  poly b = leadmonom(s, r);

  const int c = p_GetComp(s, r) - 1;
  assume( c >= 0 && c < IDELEMS(T) );

  const poly tail = T->m[c];

  if( tail != NULL )
    s = p_Add_q(s, TraverseTail(b, tail, L, T, LS, r), r);  
  
  return s;
}


static poly SchreyerSyzygyNF(poly syz_lead, poly syz_2, ideal L, ideal T, ideal LS, const ring r)
{
  assume( syz_lead != NULL );
  assume( syz_2 != NULL );

  assume( L != NULL );
  assume( T != NULL );

  assume( IDELEMS(L) == IDELEMS(T) );

  int  c = p_GetComp(syz_lead, r) - 1;

  assume( c >= 0 && c < IDELEMS(T) );

  poly p = leadmonom(syz_lead, r); // :(  
  poly spoly = pp_Mult_qq(p, T->m[c], r);
  p_Delete(&p, r);

  
  c = p_GetComp(syz_2, r) - 1;
  assume( c >= 0 && c < IDELEMS(T) );

  p = leadmonom(syz_2, r); // :(
  spoly = p_Add_q(spoly, pp_Mult_qq(p, T->m[c], r), r);
  p_Delete(&p, r);

  poly tail = p_Copy(syz_2, r); // TODO: use bucket!?

  while (spoly != NULL)
  {
    poly t = FindReducer(spoly, NULL, L, LS, r);

    p_LmDelete(&spoly, r);
    
    if( t != NULL )
    {
      p = leadmonom(t, r); // :(
      c = p_GetComp(t, r) - 1;

      assume( c >= 0 && c < IDELEMS(T) );
      
      spoly = p_Add_q(spoly, pp_Mult_qq(p, T->m[c], r), r);

      p_Delete(&p, r);
      
      tail = p_Add_q(tail, t, r);
    }    
  }
  
  return tail;
}

// proc SchreyerSyzygyNF(vector syz_lead, vector syz_2, def L, def T, list #)
static BOOLEAN SchreyerSyzygyNF(leftv res, leftv h)
{
  const char* usage = "`SchreyerSyzygyNF(<vector>, <vector>, <ideal/module>, <ideal/module>[,<module>])` expected";
  const ring r = currRing;

  NoReturn(res);

#ifndef NDEBUG
  const BOOLEAN __DEBUG__ = (BOOLEAN)((long)(atGet(currRingHdl,"DEBUG",INT_CMD, (void*)TRUE)));
#else
  const BOOLEAN __DEBUG__ = (BOOLEAN)((long)(atGet(currRingHdl,"DEBUG",INT_CMD, (void*)FALSE)));
#endif

  const BOOLEAN __TAILREDSYZ__ = (BOOLEAN)((long)(atGet(currRingHdl,"TAILREDSYZ",INT_CMD, (void*)0)));
//  const BOOLEAN __LEAD2SYZ__ = (BOOLEAN)((long)(atGet(currRingHdl,"LEAD2SYZ",INT_CMD, (void*)0)));
  const BOOLEAN __SYZCHECK__ = (BOOLEAN)((long)(atGet(currRingHdl,"SYZCHECK",INT_CMD, (void*)0)));   

  const BOOLEAN __HYBRIDNF__ = (BOOLEAN)((long)(atGet(currRingHdl,"HYBRIDNF",INT_CMD, (void*)0)));

  assume( __HYBRIDNF__ );
  
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

  if( __TAILREDSYZ__ )
    assume (LS != NULL);

  assume( h == NULL );

  if( __DEBUG__ )
  {
    PrintS("SchreyerSyzygyNF(syz_lead, syz_2, L, T, #)::Input: \n");

    PrintS("syz_lead: "); dPrint(syz_lead, r, r, 2);
    PrintS("syz_2: "); dPrint(syz_2, r, r, 2);

    PrintS("L: "); dPrint(L, r, r, 0);
    PrintS("T: "); dPrint(T, r, r, 0);

    if( LS == NULL )
      PrintS("LS: NULL\n");
    else
    {
      PrintS("LS: "); dPrint(LS, r, r, 0);
    }
  }
  
  res->rtyp = VECTOR_CMD;
  res->data = SchreyerSyzygyNF(syz_lead, syz_2, L, T, LS, r);

  if( __DEBUG__ )
  {
    PrintS("SchreyerSyzygyNF::Output: ");

    dPrint((poly)res->data, r, r, 2);
  }


  return FALSE;
}



/// TODO: save shortcut (syz: |-.->) LM(m) * "t" -> ?
/// proc SSReduceTerm(poly m, def t, def syzterm, def L, def T, list #)
static BOOLEAN ReduceTerm(leftv res, leftv h)
{
  const char* usage = "`ReduceTerm(<poly>, <poly/vector>, <vector/0>, <ideal/module>, <ideal/module>[,<module>])` expected";
  const ring r = currRing;

  NoReturn(res);

#ifndef NDEBUG
  const BOOLEAN __DEBUG__ = (BOOLEAN)((long)(atGet(currRingHdl,"DEBUG",INT_CMD, (void*)TRUE)));
#else
  const BOOLEAN __DEBUG__ = (BOOLEAN)((long)(atGet(currRingHdl,"DEBUG",INT_CMD, (void*)FALSE)));
#endif

  const BOOLEAN __TAILREDSYZ__ = (BOOLEAN)((long)(atGet(currRingHdl,"TAILREDSYZ",INT_CMD, (void*)0)));
//  const BOOLEAN __LEAD2SYZ__ = (BOOLEAN)((long)(atGet(currRingHdl,"LEAD2SYZ",INT_CMD, (void*)0)));
  const BOOLEAN __SYZCHECK__ = (BOOLEAN)((long)(atGet(currRingHdl,"SYZCHECK",INT_CMD, (void*)0)));   

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

  if( __TAILREDSYZ__ )
    assume (LS != NULL);

  assume( h == NULL );

  if( __DEBUG__ )
  {
    PrintS("ReduceTerm(m, t, syzterm, L, T, #)::Input: \n");

    PrintS("m: "); dPrint(multiplier, r, r, 2);
    PrintS("t: "); dPrint(term4reduction, r, r, 2);
    PrintS("syzterm: "); dPrint(syztermCheck, r, r, 2);
    
    PrintS("L: "); dPrint(L, r, r, 0);
    PrintS("T: "); dPrint(T, r, r, 0);

    if( LS == NULL )
      PrintS("LS: NULL\n");
    else
    {
      PrintS("LS: "); dPrint(LS, r, r, 0);
    }
  }


  if (__SYZCHECK__ && syztermCheck != NULL)
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
  res->data = ReduceTerm(multiplier, term4reduction, syztermCheck, L, T, LS, r);


  if( __DEBUG__ )
  {
    PrintS("ReduceTerm::Output: ");

    dPrint((poly)res->data, r, r, 2);
  }
  
  
  return FALSE;
}




// TODO: store m * @tail -.-^-.-^-.--> ?
// proc SSTraverseTail(poly m, def @tail, def L, def T, list #)
static BOOLEAN TraverseTail(leftv res, leftv h)
{
  const char* usage = "`TraverseTail(<poly>, <poly/vector>, <ideal/module>, <ideal/module>[,<module>])` expected";
  const ring r = currRing;

  NoReturn(res);

#ifndef NDEBUG
  const BOOLEAN __DEBUG__ = (BOOLEAN)((long)(atGet(currRingHdl,"DEBUG",INT_CMD, (void*)TRUE)));
#else
  const BOOLEAN __DEBUG__ = (BOOLEAN)((long)(atGet(currRingHdl,"DEBUG",INT_CMD, (void*)FALSE)));
#endif

  const BOOLEAN __TAILREDSYZ__ = (BOOLEAN)((long)(atGet(currRingHdl,"TAILREDSYZ",INT_CMD, (void*)1)));

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

  if( __TAILREDSYZ__ )
    assume (LS != NULL);

  assume( h == NULL );

  if( __DEBUG__ )
  {
    PrintS("TraverseTail(m, t, L, T, #)::Input: \n");

    PrintS("m: "); dPrint(multiplier, r, r, 2);
    PrintS("t: "); dPrint(tail, r, r, 10);

    PrintS("L: "); dPrint(L, r, r, 0);
    PrintS("T: "); dPrint(T, r, r, 0);

    if( LS == NULL )
      PrintS("LS: NULL\n");
    else
    {
      PrintS("LS: "); dPrint(LS, r, r, 0);
    }
  }

  res->rtyp = VECTOR_CMD;
  res->data = TraverseTail(multiplier, tail, L, T, LS, r);


  if( __DEBUG__ )
  {
    PrintS("TraverseTail::Output: ");
    dPrint((poly)res->data, r, r, 2);
  }

  return FALSE;
}



static void ComputeSyzygy(const ideal L, const ideal T, ideal& LL, ideal& TT, const ring R)
{
  assume( R == currRing ); // For attributes :-/

  assume( IDELEMS(L) == IDELEMS(T) );
  
#ifndef NDEBUG
  const BOOLEAN __DEBUG__ = (BOOLEAN)((long)(atGet(currRingHdl,"DEBUG",INT_CMD, (void*)TRUE)));
#else
  const BOOLEAN __DEBUG__ = (BOOLEAN)((long)(atGet(currRingHdl,"DEBUG",INT_CMD, (void*)FALSE)));
#endif

  const BOOLEAN __LEAD2SYZ__ = (BOOLEAN)((long)(atGet(currRingHdl,"LEAD2SYZ",INT_CMD, (void*)1)));
  const BOOLEAN __TAILREDSYZ__ = (BOOLEAN)((long)(atGet(currRingHdl,"TAILREDSYZ",INT_CMD, (void*)1)));
  const BOOLEAN __SYZCHECK__ = (BOOLEAN)((long)(atGet(currRingHdl,"SYZCHECK",INT_CMD, (void*)__DEBUG__)));

  const BOOLEAN __HYBRIDNF__ = (BOOLEAN)((long)(atGet(currRingHdl,"HYBRIDNF",INT_CMD, (void*)0)));


  if( __LEAD2SYZ__ )
    LL = Compute2LeadingSyzygyTerms(L, R); // 2 terms!
  else
    LL = ComputeLeadingSyzygyTerms(L, R); // 1 term!

  const int size = IDELEMS(LL);

  TT = idInit(size, 0);

  if( size == 1 && LL->m[0] == NULL )
    return;
  

  ideal LS = NULL;
  
  if( __TAILREDSYZ__ )
    LS = LL;

  for( int k = size - 1; k >= 0; k-- )
  {
    const poly a = LL->m[k]; assume( a != NULL );
    
    const int r = p_GetComp(a, R) - 1; 
    
    assume( r >= 0 && r < IDELEMS(T) );
    assume( r >= 0 && r < IDELEMS(L) );

    poly aa = leadmonom(a, R); assume( aa != NULL); // :(    
    
    poly a2 = pNext(a);    

    if( a2 != NULL )
    {
      TT->m[k] = a2; pNext(a) = NULL;
    }

    if( ! __HYBRIDNF__ )
    {
      poly t = TraverseTail(aa, T->m[r], L, T, LS, R);

      if( a2 != NULL )
      {
        assume( __LEAD2SYZ__ );

        const int r2 = p_GetComp(a2, R) - 1; poly aa2 = leadmonom(a2, R); // :(

        assume( r2 >= 0 && r2 < IDELEMS(T) );
        
        TT->m[k] = p_Add_q(a2, p_Add_q(t, TraverseTail(aa2, T->m[r2], L, T, LS, R), R), R);

        p_Delete(&aa2, R);
      } else
        TT->m[k] = p_Add_q(t, ReduceTerm(aa, L->m[r], a, L, T, LS, R), R);

    } else
    {
      if( a2 == NULL )
      {
        aa = p_Mult_mm(aa, L->m[r], R);
        a2 = FindReducer(aa, a, L, LS, R); 
      }
      assume( a2 != NULL );
        
      TT->m[k] = SchreyerSyzygyNF(a, a2, L, T, LS, R); // will copy a2 :(
      
      p_Delete(&a2, R);
    }
    p_Delete(&aa, R);    
  }

  TT->rank = id_RankFreeModule(TT, R);
}



// module (N, LL, TT) = SSComputeSyzygy(L, T);
// Compute Syz(L ++ T) = N = LL ++ TT
// proc SSComputeSyzygy(def L, def T)
static BOOLEAN ComputeSyzygy(leftv res, leftv h)
{
  const char* usage = "`ComputeSyzygy(<ideal/module>, <ideal/module>])` expected";
  const ring r = currRing;

  NoReturn(res);

#ifndef NDEBUG
  const BOOLEAN __DEBUG__ = (BOOLEAN)((long)(atGet(currRingHdl,"DEBUG",INT_CMD, (void*)TRUE)));
#else
  const BOOLEAN __DEBUG__ = (BOOLEAN)((long)(atGet(currRingHdl,"DEBUG",INT_CMD, (void*)FALSE)));
#endif

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

  if( __DEBUG__ )
  {
    PrintS("ComputeSyzygy(L, T)::Input: \n");

    PrintS("L: "); dPrint(L, r, r, 0);
    PrintS("T: "); dPrint(T, r, r, 0);
  }

  ideal LL, TT;

  ComputeSyzygy(L, T, LL, TT, r);

  lists l = (lists)omAllocBin(slists_bin); l->Init(2);

  l->m[0].rtyp = MODUL_CMD; l->m[0].data = reinterpret_cast<void *>(LL);

  l->m[1].rtyp = MODUL_CMD; l->m[1].data = reinterpret_cast<void *>(TT);
  
  res->data = l; res->rtyp = LIST_CMD;
  
  if( __DEBUG__ )
  {
    PrintS("ComputeSyzygy::Output: ");
    dPrint(LL, r, r, 0);
    dPrint(TT, r, r, 0);
  }

  return FALSE;

}







/// Get leading term without a module component
static BOOLEAN leadmonom(leftv res, leftv h)
{
  NoReturn(res);

  if ((h!=NULL) && (h->Typ()==VECTOR_CMD || h->Typ()==POLY_CMD) && (h->Data() != NULL))
  {
    const ring r = currRing;
    const poly p = (poly)(h->Data());

    const poly m = leadmonom(p, r);

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

  res->data = (void *)kNFLength(M, currQuotient, v, iSyzComp, iLazyReduce); // NOTE: currRing :(
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
  //             if (idTestHomModule(I, currQuotient, ww))
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
           kStd(I, currQuotient, hom, &w, NULL, iComp);

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

   

END_NAMESPACE


int SI_MOD_INIT(syzextra)(SModulFunctions* psModulFunctions) 
{
#define ADD0(A,B,C,D,E) A(B, (char*)C, D, E)
// #define ADD(A,B,C,D,E) ADD0(iiAddCproc, "", C, D, E)
  #define ADD(A,B,C,D,E) ADD0(A->iiAddCproc, B, C, D, E)
  ADD(psModulFunctions, currPack->libname, "ClearContent", FALSE, _ClearContent);
  ADD(psModulFunctions, currPack->libname, "ClearDenominators", FALSE, _ClearDenominators);

  ADD(psModulFunctions, currPack->libname, "m2_end", FALSE, _m2_end);

  ADD(psModulFunctions, currPack->libname, "DetailedPrint", FALSE, DetailedPrint);
  ADD(psModulFunctions, currPack->libname, "leadmonomial", FALSE, leadmonom);
  ADD(psModulFunctions, currPack->libname, "leadcomp", FALSE, leadcomp);
  ADD(psModulFunctions, currPack->libname, "leadrawexp", FALSE, leadrawexp);

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

  ADD(psModulFunctions, currPack->libname, "Tail", FALSE, Tail);
  
  ADD(psModulFunctions, currPack->libname, "ComputeLeadingSyzygyTerms", FALSE, ComputeLeadingSyzygyTerms);
  ADD(psModulFunctions, currPack->libname, "Compute2LeadingSyzygyTerms", FALSE, Compute2LeadingSyzygyTerms);
  
  ADD(psModulFunctions, currPack->libname, "Sort_c_ds", FALSE, Sort_c_ds);
  ADD(psModulFunctions, currPack->libname, "FindReducer", FALSE, FindReducer);


  ADD(psModulFunctions, currPack->libname, "ReduceTerm", FALSE, ReduceTerm);
  ADD(psModulFunctions, currPack->libname, "TraverseTail", FALSE, TraverseTail);

    
  ADD(psModulFunctions, currPack->libname, "SchreyerSyzygyNF", FALSE, SchreyerSyzygyNF);
  ADD(psModulFunctions, currPack->libname, "ComputeSyzygy", FALSE, ComputeSyzygy);
  
  //  ADD(psModulFunctions, currPack->libname, "GetAMData", FALSE, GetAMData);

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
