// -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*-
/*****************************************************************************\
 * Computer Algebra System SINGULAR    
\*****************************************************************************/
/** @file syzextra.cc
 * 
 * Here we implement the Computation of Syzygies
 *
 * ABSTRACT: Computation of Syzygies due to Schreyer
 *
 * @author Oleksandr Motsak
 *
 **/
/*****************************************************************************/

// include header file
#include <kernel/mod2.h>

#include "syzextra.h"

#include "DebugPrint.h"

#include <omalloc/omalloc.h>

#include <misc/intvec.h>
#include <misc/options.h>

#include <coeffs/coeffs.h>

#include <polys/monomials/p_polys.h>
#include <polys/monomials/ring.h>

#include <kernel/kstd1.h>
#include <kernel/polys.h>
#include <kernel/syz.h>
#include <kernel/ideals.h>



#include <Singular/tok.h>
#include <Singular/ipid.h>
#include <Singular/lists.h>
#include <Singular/attrib.h>

#include <Singular/ipid.h> 
#include <Singular/ipshell.h> // For iiAddCproc

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// USING_NAMESPACE_SINGULARXX;
USING_NAMESPACE( SINGULARXXNAME :: DEBUG )


BEGIN_NAMESPACE_SINGULARXX     BEGIN_NAMESPACE(SYZEXTRA)


BEGIN_NAMESPACE(SORT_c_ds)


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

#ifndef NDEBUG
  const int __DEBUG__ = 0;
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

END_NAMESPACE
/* namespace SORT_c_ds */

/// return a new term: leading coeff * leading monomial of p
/// with 0 leading component!
poly leadmonom(const poly p, const ring r)
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


   
poly p_Tail(const poly p, const ring r)
{
  if( p == NULL)
    return NULL;
  else
    return p_Copy( pNext(p), r );
}


ideal id_Tail(const ideal id, const ring r)
{
  if( id == NULL)
    return NULL;

  const ideal newid = idInit(IDELEMS(id),id->rank);
  
  for (int i=IDELEMS(id) - 1; i >= 0; i--)
    newid->m[i] = p_Tail( id->m[i], r );

  newid->rank = id_RankFreeModule(newid, currRing);

  return newid;  
}



void Sort_c_ds(const ideal id, const ring r)
{
  const int sizeNew = IDELEMS(id);

#ifdef _GNU_SOURCE
#define qsort_my(m, s, ss, r, cmp) qsort_r(m, s, ss, cmp, r)
#else
#define qsort_my(m, s, ss, r, cmp) qsort_r(m, s, ss, cmp)
#endif
  
  if( sizeNew >= 2 )
    qsort_my(id->m, sizeNew, sizeof(poly), r, FROM_NAMESPACE(SORT_c_ds, cmp_c_ds));

#undef qsort_my

  id->rank = id_RankFreeModule(id, r);
}

ideal SchreyerSyzygyComputation::Compute1LeadingSyzygyTerms()
{
  const ideal& id = m_idLeads;
  const ring& r = m_rBaseRing;
//  const SchreyerSyzygyComputationFlags& attributes = m_atttributes;

//   const BOOLEAN __DEBUG__      = attributes.__DEBUG__;
//   const BOOLEAN __SYZCHECK__   = attributes.__SYZCHECK__;
//   const BOOLEAN __LEAD2SYZ__   = attributes.__LEAD2SYZ__;
//   const BOOLEAN __HYBRIDNF__   = attributes.__HYBRIDNF__;
//   const BOOLEAN __TAILREDSYZ__ = attributes.__TAILREDSYZ__;

  assume(!__LEAD2SYZ__);

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

  Sort_c_ds(newid, r);

  return newid;
}

ideal SchreyerSyzygyComputation::Compute2LeadingSyzygyTerms()
{
  const ideal& id = m_idLeads;
  const ring& r = m_rBaseRing;
//  const SchreyerSyzygyComputationFlags& attributes = m_atttributes;
  
//   const BOOLEAN __DEBUG__      = attributes.__DEBUG__;
//   const BOOLEAN __SYZCHECK__   = attributes.__SYZCHECK__;
//   const BOOLEAN __LEAD2SYZ__   = attributes.__LEAD2SYZ__;
//   const BOOLEAN __HYBRIDNF__   = attributes.__HYBRIDNF__;
//  const BOOLEAN __TAILREDSYZ__ = attributes.__TAILREDSYZ__;
  

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

  Sort_c_ds(newid, r);
  
  return newid;
}

poly SchreyerSyzygyComputation::TraverseNF(const poly a, const poly a2) const
{
  const ideal& L = m_idLeads;
  const ideal& T = m_idTails;

  const ring& R = m_rBaseRing;

  const int r = p_GetComp(a, R) - 1; 

  assume( r >= 0 && r < IDELEMS(T) );
  assume( r >= 0 && r < IDELEMS(L) );

  poly aa = leadmonom(a, R); assume( aa != NULL); // :(
  poly t = TraverseTail(aa, r);

  if( a2 != NULL )
  {
    assume( __LEAD2SYZ__ );

    const int r2 = p_GetComp(a2, R) - 1; poly aa2 = leadmonom(a2, R); // :(

    assume( r2 >= 0 && r2 < IDELEMS(T) );

    t = p_Add_q(a2, p_Add_q(t, TraverseTail(aa2, r2), R), R);

    p_Delete(&aa2, R);
  } else
    t = p_Add_q(t, ReduceTerm(aa, L->m[r], a), R);

  p_Delete(&aa, R);

  return t;
}



void SchreyerSyzygyComputation::ComputeSyzygy()
{
  assume( m_idLeads != NULL );
  assume( m_idTails != NULL );
  
  const ideal& L = m_idLeads;
  const ideal& T = m_idTails;

  ideal& TT = m_syzTails;
  const ring& R = m_rBaseRing;

  assume( IDELEMS(L) == IDELEMS(T) );

  if( m_syzLeads == NULL )
    ComputeLeadingSyzygyTerms( __LEAD2SYZ__ && !__IGNORETAILS__ ); // 2 terms OR 1 term!

  assume( m_syzLeads != NULL );

  ideal& LL = m_syzLeads;

  
  const int size = IDELEMS(LL);

  TT = idInit(size, 0);

  if( size == 1 && LL->m[0] == NULL )
    return;


  for( int k = size - 1; k >= 0; k-- )
  {
    const poly a = LL->m[k]; assume( a != NULL );

    poly a2 = pNext(a);    

    // Splitting 2-terms Leading syzygy module
    if( a2 != NULL )
      pNext(a) = NULL;

    if( __IGNORETAILS__ )
    {
      TT->m[k] = NULL;

      assume( a2 != NULL );

      if( a2 != NULL )
        p_Delete(&a2, R);

      continue;
    }
    
//    TT->m[k] = a2;

    if( ! __HYBRIDNF__ )
    {
      TT->m[k] = TraverseNF(a, a2);
    } else
    {
      TT->m[k] = SchreyerSyzygyNF(a, a2);
    }

  }

  TT->rank = id_RankFreeModule(TT, R);
}

void SchreyerSyzygyComputation::ComputeLeadingSyzygyTerms(bool bComputeSecondTerms)
{
//  const SchreyerSyzygyComputationFlags& attributes = m_atttributes;

//  const BOOLEAN __LEAD2SYZ__   = attributes.__LEAD2SYZ__;
//  const BOOLEAN __TAILREDSYZ__ = attributes.__TAILREDSYZ__;

  assume( m_syzLeads == NULL );

  if( bComputeSecondTerms )
  {
    assume( __LEAD2SYZ__ );
//    m_syzLeads = FROM_NAMESPACE(INTERNAL, _Compute2LeadingSyzygyTerms(m_idLeads, m_rBaseRing, m_atttributes));
    m_syzLeads = Compute2LeadingSyzygyTerms();
  }
  else
  {
    assume( !__LEAD2SYZ__ );
    
    m_syzLeads = Compute1LeadingSyzygyTerms();
  }
//    m_syzLeads = FROM_NAMESPACE(INTERNAL, _ComputeLeadingSyzygyTerms(m_idLeads, m_rBaseRing, m_atttributes));
  
  // NOTE: set m_LS if tails are to be reduced!
  assume( m_syzLeads!= NULL );

  if (__TAILREDSYZ__ && !__IGNORETAILS__ && (IDELEMS(m_syzLeads) > 0) && !((IDELEMS(m_syzLeads) == 1) && (m_syzLeads->m[0] == NULL)))
  {
    m_LS = m_syzLeads;
    m_checker.Initialize(m_syzLeads);
#ifndef NDEBUG    
    if( __DEBUG__ )
    {
      const ring& r = m_rBaseRing;
      PrintS("SchreyerSyzygyComputation::ComputeLeadingSyzygyTerms: \n");
      PrintS("m_syzLeads: \n");
      dPrint(m_syzLeads, r, r, 1);
      PrintS("m_checker.Initialize(m_syzLeads) => \n");
      m_checker.DebugPrint();
    }
#endif
    assume( m_checker.IsNonempty() ); // TODO: this always fails... BUG????
  }
}

#define NOPRODUCT 1

poly SchreyerSyzygyComputation::SchreyerSyzygyNF(const poly syz_lead, poly syz_2) const
{
  
  assume( !__IGNORETAILS__ );

  const ideal& L = m_idLeads;
  const ideal& T = m_idTails;
  const ring& r = m_rBaseRing;

  assume( syz_lead != NULL );

  if( syz_2 == NULL )
  {
    const int rr = p_GetComp(syz_lead, r) - 1; 

    assume( rr >= 0 && rr < IDELEMS(T) );
    assume( rr >= 0 && rr < IDELEMS(L) );


#if NOPRODUCT
    syz_2 = m_div.FindReducer(syz_lead, L->m[rr], syz_lead, m_checker);
#else    
    poly aa = leadmonom(syz_lead, r); assume( aa != NULL); // :(
    aa = p_Mult_mm(aa, L->m[rr], r);

    syz_2 = m_div.FindReducer(aa, syz_lead, m_checker);

    p_Delete(&aa, r);
#endif
    
    assume( syz_2 != NULL ); // by construction of S-Polynomial    
  }


  
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

  poly tail = syz_2; // TODO: use bucket!?

  while (spoly != NULL)
  {
    poly t = m_div.FindReducer(spoly, NULL, m_checker);

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

poly SchreyerSyzygyComputation::TraverseTail(poly multiplier, const int tail) const
{
  // TODO: store (multiplier, tail) -.-^-.-^-.--> !
  assume(m_idTails !=  NULL && m_idTails->m != NULL);
  assume( tail >= 0 && tail < IDELEMS(m_idTails) );

  const poly t = m_idTails->m[tail];

  if(t != NULL)
    return TraverseTail(multiplier, t);

  return NULL;
}


poly SchreyerSyzygyComputation::TraverseTail(poly multiplier, poly tail) const
{
  assume( !__IGNORETAILS__ );

  const ideal& L = m_idLeads;
  const ideal& T = m_idTails;
  const ring& r = m_rBaseRing;

  assume( multiplier != NULL );

  assume( L != NULL );
  assume( T != NULL );

  poly s = NULL;

  if( (!__TAILREDSYZ__) || m_lcm.Check(multiplier) )
    for(poly p = tail; p != NULL; p = pNext(p))   // iterate over the tail
      s = p_Add_q(s, ReduceTerm(multiplier, p, NULL), r);  

  return s;
}




poly SchreyerSyzygyComputation::ReduceTerm(poly multiplier, poly term4reduction, poly syztermCheck) const
{
  assume( !__IGNORETAILS__ );

  const ideal& L = m_idLeads;
  const ideal& T = m_idTails;
  const ring& r = m_rBaseRing;

  assume( multiplier != NULL );
  assume( term4reduction != NULL );


  assume( L != NULL );
  assume( T != NULL );

  // simple implementation with FindReducer:
  poly s = NULL;

  if( (!__TAILREDSYZ__) || m_lcm.Check(multiplier) )
  {
#if NOPRODUCT
    s = m_div.FindReducer(multiplier, term4reduction, syztermCheck, m_checker);
#else    
    // NOTE: only LT(term4reduction) should be used in the following:
    poly product = pp_Mult_mm(multiplier, term4reduction, r);
    s = m_div.FindReducer(product, syztermCheck, m_checker);
    p_Delete(&product, r);
#endif
  }

  if( s == NULL ) // No Reducer?
    return s;

  poly b = leadmonom(s, r);

  const int c = p_GetComp(s, r) - 1;
  assume( c >= 0 && c < IDELEMS(T) );

  const poly t = TraverseTail(b, c); // T->m[c];

  if( t != NULL )
    s = p_Add_q(s, t, r);  

  return s;
}





BEGIN_NAMESPACE_NONAME
   
static inline int atGetInt(idhdl rootRingHdl, const char* attribute, long def)
{
  return ((int)(long)(atGet(rootRingHdl, attribute, INT_CMD, (void*)def)));
}

END_NAMESPACE   

SchreyerSyzygyComputationFlags::SchreyerSyzygyComputationFlags(idhdl rootRingHdl):
#ifndef NDEBUG
    __DEBUG__( (BOOLEAN)atGetInt(rootRingHdl,"DEBUG", TRUE) ),
#else
    __DEBUG__( (BOOLEAN)atGetInt(rootRingHdl,"DEBUG", FALSE) ),
#endif
//    __SYZCHECK__( (BOOLEAN)atGetInt(rootRingHdl, "SYZCHECK", __DEBUG__) ),
    __LEAD2SYZ__( (BOOLEAN)atGetInt(rootRingHdl, "LEAD2SYZ", 1) ), 
    __TAILREDSYZ__( (BOOLEAN)atGetInt(rootRingHdl, "TAILREDSYZ", 1) ), 
    __HYBRIDNF__( (BOOLEAN)atGetInt(rootRingHdl, "HYBRIDNF", 0) ),
    __IGNORETAILS__( (BOOLEAN)atGetInt(rootRingHdl, "IGNORETAILS", 0) ),
    m_rBaseRing( rootRingHdl->data.uring )
{    
  if( __DEBUG__ )
  {
    PrintS("SchreyerSyzygyComputationFlags: \n");
    Print("        DEBUG: \t%d\n", __DEBUG__);
//    Print("   SYZCHECK  : \t%d\n", __SYZCHECK__);
    Print("     LEAD2SYZ: \t%d\n", __LEAD2SYZ__);
    Print("   TAILREDSYZ: \t%d\n", __TAILREDSYZ__);
    Print("  IGNORETAILS: \t%d\n", __IGNORETAILS__);
    
  }

  // TODO: just current setting!
  assume( rootRingHdl == currRingHdl );
  assume( rootRingHdl->typ == RING_CMD );
  assume( m_rBaseRing == currRing );
  // move the global ring here inside???
}

   

CReducerFinder::CLeadingTerm::CLeadingTerm(unsigned int _label,  const poly _lt, const ring R):
    m_sev( p_GetShortExpVector(_lt, R) ),  m_label( _label ),  m_lt( _lt )
{ }


CReducerFinder::~CReducerFinder()
{
  for( CReducersHash::const_iterator it = m_hash.begin(); it != m_hash.end(); it++ )
  {
    const TReducers& v = it->second;
    for(TReducers::const_iterator vit = v.begin(); vit != v.end(); vit++ )
      delete const_cast<CLeadingTerm*>(*vit);
  }
}


void CReducerFinder::Initialize(const ideal L)
{
  assume( m_L == NULL || m_L == L );
  if( m_L == NULL )
    m_L = L;

  assume( m_L == L );
  
  if( L != NULL )
  {
    const ring& R = m_rBaseRing;
    assume( R != NULL );
    
    for( int k = IDELEMS(L) - 1; k >= 0; k-- )
    {
      const poly a = L->m[k]; // assume( a != NULL );

      // NOTE: label is k \in 0 ... |L|-1!!!
      if( a != NULL )
        m_hash[p_GetComp(a, R)].push_back( new CLeadingTerm(k, a, R) );
    }
  }
}

CReducerFinder::CReducerFinder(const ideal L, const SchreyerSyzygyComputationFlags& flags):
    SchreyerSyzygyComputationFlags(flags),
    m_L(const_cast<ideal>(L)), // for debug anyway
    m_hash()
{
  assume( flags.m_rBaseRing == m_rBaseRing );
  if( L != NULL )
    Initialize(L);
}


bool CReducerFinder::IsDivisible(const poly product) const
{
  const ring& r = m_rBaseRing;
  
  const long comp = p_GetComp(product, r);
  const unsigned long not_sev = ~p_GetShortExpVector(product, r);

  assume( comp >= 0 );

  CReducersHash::const_iterator it = m_hash.find(comp); // same module component

  if( it == m_hash.end() )
    return false;

  assume( m_L != NULL );  

  const TReducers& reducers = it->second;

  for(TReducers::const_iterator vit = reducers.begin(); vit != reducers.end(); vit++ )
  {
    const poly p = (*vit)->m_lt;

    assume( p_GetComp(p, r) == comp );

    const int k = (*vit)->m_label;

    assume( m_L->m[k] == p );

    const unsigned long p_sev = (*vit)->m_sev;

    assume( p_sev == p_GetShortExpVector(p, r) );     

    if( !p_LmShortDivisibleByNoComp(p, p_sev, product, not_sev, r) )
      continue;

    if( __DEBUG__ )
    {
      Print("_FindReducer::Test LS: q is divisible by LS[%d] !:((, diviser is: ", k+1);
      dPrint(p, r, r, 1);
    }

    return true;
  }

  return false;
}


#ifndef NDEBUG
void CReducerFinder::DebugPrint() const
{
  const ring& r = m_rBaseRing;

  for( CReducersHash::const_iterator it = m_hash.begin(); it != m_hash.end(); it++)
  {
    Print("Hash Key: %d, Values: \n", it->first);
    const TReducers& reducers = it->second;

    for(TReducers::const_iterator vit = reducers.begin(); vit != reducers.end(); vit++ )
    {
      const poly p = (*vit)->m_lt;

      assume( p_GetComp(p, r) == it->first );

      const int k = (*vit)->m_label;

      assume( m_L->m[k] == p );

      const unsigned long p_sev = (*vit)->m_sev;

      assume( p_sev == p_GetShortExpVector(p, r) );

      Print("L[%d]: ", k); dPrint(p, r, r, 0); Print("SEV: %dl\n", p_sev);      
    }
  }
}
#endif


/// _p_LmDivisibleByNoComp for a | b*c
static inline BOOLEAN _p_LmDivisibleByNoComp(const poly a, const poly b, const poly c, const ring r)
{
  int i=r->VarL_Size - 1;
  unsigned long divmask = r->divmask;
  unsigned long la, lb;

  if (r->VarL_LowIndex >= 0)
  {
    i += r->VarL_LowIndex;
    do
    {
      la = a->exp[i];
      lb = b->exp[i] + c->exp[i];
      if ((la > lb) ||
          (((la & divmask) ^ (lb & divmask)) != ((lb - la) & divmask)))
      {
        pDivAssume(p_DebugLmDivisibleByNoComp(a, b, r) == FALSE);
        return FALSE;
      }
      i--;
    }
    while (i>=r->VarL_LowIndex);
  }
  else
  {
    do
    {
      la = a->exp[r->VarL_Offset[i]];
      lb = b->exp[r->VarL_Offset[i]] + c->exp[r->VarL_Offset[i]];
      if ((la > lb) ||
          (((la & divmask) ^ (lb & divmask)) != ((lb - la) & divmask)))
      {
        pDivAssume(p_DebugLmDivisibleByNoComp(a, b, r) == FALSE);
        return FALSE;
      }
      i--;
    }
    while (i>=0);
  }
#ifdef HAVE_RINGS
  assume( !rField_is_Ring(r) ); // not implemented for rings...!
#endif
  return TRUE;
}


poly CReducerFinder::FindReducer(const poly multiplier, const poly t,
                                 const poly syzterm, const CReducerFinder& syz_checker) const
{
  // don't case about the module component of multiplier (as it may be
  // the syzygy term)
  // product = multiplier * t?
  const ring& r = m_rBaseRing;

  assume( multiplier != NULL ); assume( t != NULL );

  const ideal& L = m_L; assume( L != NULL ); // for debug/testing only!

  long c = 0;

  if (syzterm != NULL)
    c = p_GetComp(syzterm, r) - 1;

  assume( c >= 0 && c < IDELEMS(L) );

  if (__DEBUG__ && (syzterm != NULL))
  {
    const poly m = L->m[c];

    assume( m != NULL ); assume( pNext(m) == NULL );

    poly lm = p_Mult_mm(leadmonom(syzterm, r), m, r);

    poly pr = p_Mult_q( p_LmInit(multiplier, r), p_LmInit(t, r), r);
    
    assume( p_EqualPolys(lm, pr, r) );

    //  def @@c = leadcomp(syzterm); int @@r = int(@@c);
    //  def @@product = leadmonomial(syzterm) * L[@@r];

    p_Delete(&lm, r);    
    p_Delete(&pr, r);    
  }

  const long comp = p_GetComp(t, r);
  
  const unsigned long not_sev = ~p_GetShortExpVector(multiplier, t, r); // !

  assume( comp >= 0 );

//   for( int k = IDELEMS(L)-1; k>= 0; k-- )
//   {
//     const poly p = L->m[k];
// 
//     if ( p_GetComp(p, r) != comp )
//       continue;
// 
//     const unsigned long p_sev = p_GetShortExpVector(p, r); // to be stored in m_hash!!!

   // looking for an appropriate diviser p = L[k]...
  CReducersHash::const_iterator it = m_hash.find(comp); // same module component

  if( it == m_hash.end() )
    return NULL;

  assume( m_L != NULL );

  const TReducers& reducers = it->second;

  const BOOLEAN to_check = (syz_checker.IsNonempty()); // __TAILREDSYZ__ && 

  const poly q = p_New(r); pNext(q) = NULL;

  if( __DEBUG__ )
    p_SetCoeff0(q, 0, r); // for printing q

  for(TReducers::const_iterator vit = reducers.begin(); vit != reducers.end(); vit++ )
  {
    const poly p = (*vit)->m_lt;

    assume( p_GetComp(p, r) == comp );

    const int k = (*vit)->m_label;

    assume( L->m[k] == p );

    const unsigned long p_sev = (*vit)->m_sev;

    assume( p_sev == p_GetShortExpVector(p, r) );     

//    if( !p_LmShortDivisibleByNoComp(p, p_sev, product, not_sev, r) )
//      continue;     

    if (p_sev & not_sev)
      continue;

    if( !_p_LmDivisibleByNoComp(p, multiplier, t, r) )
      continue;     


    p_ExpVectorSum(q, multiplier, t, r); // q == product == multiplier * t        
    p_ExpVectorDiff(q, q, p, r); // (LM(product) / LM(L[k]))
    
    p_SetComp(q, k + 1, r);
    p_Setm(q, r);

    // cannot allow something like: a*gen(i) - a*gen(i)
    if (syzterm != NULL && (k == c))
      if (p_ExpVectorEqual(syzterm, q, r))
      {
        if( __DEBUG__ )
        {
          Print("_FindReducer::Test SYZTERM: q == syzterm !:((, syzterm is: ");
          dPrint(syzterm, r, r, 1);
        }    

        continue;
      }

    // while the complement (the fraction) is not reducible by leading syzygies 
    if( to_check && syz_checker.IsDivisible(q) ) 
    {
      if( __DEBUG__ )
      {
        PrintS("_FindReducer::Test LS: q is divisible by LS[?] !:((: ");
      }

      continue;
    }

    number n = n_Mult( p_GetCoeff(multiplier, r), p_GetCoeff(t, r), r);
    p_SetCoeff0(q, n_Neg( n_Div(n, p_GetCoeff(p, r), r), r), r);
    n_Delete(&n, r);
    
    return q;
  }

  p_LmFree(q, r);

  return NULL;
}


poly CReducerFinder::FindReducer(const poly product, const poly syzterm, const CReducerFinder& syz_checker) const
{
  const ring& r = m_rBaseRing;

  assume( product != NULL );

  const ideal& L = m_L; assume( L != NULL ); // for debug/testing only!

  long c = 0;

  if (syzterm != NULL)
    c = p_GetComp(syzterm, r) - 1;

  assume( c >= 0 && c < IDELEMS(L) );

  if (__DEBUG__ && (syzterm != NULL))
  {
    const poly m = L->m[c];

    assume( m != NULL ); assume( pNext(m) == NULL );

    poly lm = p_Mult_mm(leadmonom(syzterm, r), m, r);
    assume( p_EqualPolys(lm, product, r) );

    //  def @@c = leadcomp(syzterm); int @@r = int(@@c);
    //  def @@product = leadmonomial(syzterm) * L[@@r];

    p_Delete(&lm, r);    
  }

  const long comp = p_GetComp(product, r);
  const unsigned long not_sev = ~p_GetShortExpVector(product, r);

  assume( comp >= 0 );

//   for( int k = IDELEMS(L)-1; k>= 0; k-- )
//   {
//     const poly p = L->m[k];
// 
//     if ( p_GetComp(p, r) != comp )
//       continue;
// 
//     const unsigned long p_sev = p_GetShortExpVector(p, r); // to be stored in m_hash!!!
  
   // looking for an appropriate diviser p = L[k]...
  CReducersHash::const_iterator it = m_hash.find(comp); // same module component

  if( it == m_hash.end() )
    return NULL;

  assume( m_L != NULL );

  const TReducers& reducers = it->second;

  const BOOLEAN to_check = (syz_checker.IsNonempty()); // __TAILREDSYZ__ && 

  const poly q = p_New(r); pNext(q) = NULL;

  if( __DEBUG__ )
    p_SetCoeff0(q, 0, r); // for printing q
  
  for(TReducers::const_iterator vit = reducers.begin(); vit != reducers.end(); vit++ )
  {
    const poly p = (*vit)->m_lt;

    assume( p_GetComp(p, r) == comp );

    const int k = (*vit)->m_label;

    assume( L->m[k] == p );

    const unsigned long p_sev = (*vit)->m_sev;

    assume( p_sev == p_GetShortExpVector(p, r) );     

    if( !p_LmShortDivisibleByNoComp(p, p_sev, product, not_sev, r) )
      continue;     

//     // ... which divides the product, looking for the _1st_ appropriate one!
//     if( !p_LmDivisibleByNoComp(p, product, r) ) // included inside  p_LmShortDivisibleBy!
//       continue;

    p_ExpVectorDiff(q, product, p, r); // (LM(product) / LM(L[k]))
    p_SetComp(q, k + 1, r);
    p_Setm(q, r);

    // cannot allow something like: a*gen(i) - a*gen(i)
    if (syzterm != NULL && (k == c))
      if (p_ExpVectorEqual(syzterm, q, r))
      {
        if( __DEBUG__ )
        {
          Print("_FindReducer::Test SYZTERM: q == syzterm !:((, syzterm is: ");
          dPrint(syzterm, r, r, 1);
        }    

        continue;
      }

    // while the complement (the fraction) is not reducible by leading syzygies 
    if( to_check && syz_checker.IsDivisible(q) ) 
    {
      if( __DEBUG__ )
      {
        PrintS("_FindReducer::Test LS: q is divisible by LS[?] !:((: ");
      }
      
      continue;
    }

    p_SetCoeff0(q, n_Neg( n_Div( p_GetCoeff(product, r), p_GetCoeff(p, r), r), r), r);
    return q;
  }

  p_LmFree(q, r);

  return NULL;
}



CLCM::CLCM(const ideal& L, const SchreyerSyzygyComputationFlags& flags):
    SchreyerSyzygyComputationFlags(flags), std::vector<bool>(),
    m_compute(false), m_N(rVar(flags.m_rBaseRing))
{
  const ring& R = m_rBaseRing;
  assume( flags.m_rBaseRing == R );
  assume( R != NULL );

  assume( L != NULL );

  if( __TAILREDSYZ__ && !__HYBRIDNF__ && (L != NULL))
  {
    const int l = IDELEMS(L);

    assume( l > 0 );

    resize(l, false);

    for( int k = l - 1; k >= 0; k-- )
    {
      const poly a = L->m[k]; assume( a != NULL );

      for (unsigned int j = m_N; j > 0; j--)
        if ( !(*this)[j] )
          (*this)[j] = (p_GetExp(a, j, R) > 0);
    }

    m_compute = true;    
  }
}


bool CLCM::Check(const poly m) const
{
  assume( m != NULL );
  if( m_compute && (m != NULL))
  {  
    const ring& R = m_rBaseRing;

    assume( __TAILREDSYZ__ && !__HYBRIDNF__ );

    for (unsigned int j = m_N; j > 0; j--)
      if ( (*this)[j] )
        if(p_GetExp(m, j, R) > 0)
          return true;

    return false;

  } else return true;
}




END_NAMESPACE               END_NAMESPACE_SINGULARXX


// Vi-modeline: vim: filetype=c:syntax:shiftwidth=2:tabstop=8:textwidth=0:expandtab
