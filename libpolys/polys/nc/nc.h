#ifndef POLYS_NC_H
#define POLYS_NC_H


#ifdef HAVE_PLURAL



// TODO: the following is a part of ring.h... would be nice to have a
// clear public NC interface defined here!

#include <polys/monomials/ring.h>
#include <polys/kbuckets.h>


class ip_smatrix;
typedef ip_smatrix *       matrix;


matrix nc_PrintMat(int a, int b, ring r, int metric);


enum nc_type
{
  nc_error = -1, // Something's gone wrong!
  nc_general = 0, /* yx=q xy+... */
  nc_skew, /*1*/ /* yx=q xy */
  nc_comm, /*2*/ /* yx= xy */
  nc_lie,  /*3*/ /* yx=xy+... */
  nc_undef, /*4*/  /* for internal reasons */

  nc_exterior /*5*/ // Exterior Algebra(SCA): yx= -xy & (!:) x^2 = 0
};


// //////////////////////////////////////////////////////


/// checks whether rings rBase and rCandidate
/// could be opposite to each other
/// returns TRUE if it is so
BOOLEAN rIsLikeOpposite(ring rBase, ring rCandidate);



// Macros used to access upper triangle matrices C,D... (which are actually ideals) // afaik
#define UPMATELEM(i,j,nVar) ( (nVar * ((i)-1) - ((i) * ((i)-1))/2 + (j)-1)-(i) )

/// complete destructor
void nc_rKill(ring r);


BOOLEAN nc_CheckSubalgebra(poly PolyVar, ring r);

// NC pProcs:
typedef poly (*mm_Mult_p_Proc_Ptr)(const poly m, poly p, const ring r);
typedef poly (*mm_Mult_pp_Proc_Ptr)(const poly m, const poly p, const ring r);



typedef poly (*SPoly_Proc_Ptr)(const poly p1, const poly p2, const ring r);
typedef poly (*SPolyReduce_Proc_Ptr)(const poly p1, poly p2, const ring r);

typedef void (*bucket_Proc_Ptr)(kBucket_pt b, poly p, number *c);

struct nc_pProcs
{
public:
  mm_Mult_p_Proc_Ptr                    mm_Mult_p;
  mm_Mult_pp_Proc_Ptr                   mm_Mult_pp;

  bucket_Proc_Ptr                       BucketPolyRed;
  bucket_Proc_Ptr                       BucketPolyRed_Z;

  SPoly_Proc_Ptr                        SPoly;
  SPolyReduce_Proc_Ptr                  ReduceSPoly;
};

class CGlobalMultiplier;
class CFormulaPowerMultiplier;

struct nc_struct
{
  nc_type type;
  //ring basering; // the ring C,D,.. live in (commutative ring with this NC structure!)

  // initial data: square matrices rVar() x rVar()
  // logically: upper triangular!!!
  // TODO: eliminate this waste of memory!!!!
  matrix C; 
  matrix D;

  // computed data:
  matrix *MT; // size 0.. (rVar()*rVar()-1)/2
  matrix COM;
  int *MTsize; // size 0.. (rVar()*rVar()-1)/2

  // IsSkewConstant indicates whethere coeffs C_ij are all equal, 
  // effective together with nc_type=nc_skew
  int IsSkewConstant;

  private:
    // internal data for different implementations
    // if dynamic => must be deallocated in destructor (nc_rKill!)
    union
    {
      struct
      {
        // treat variables from iAltVarsStart till iAltVarsEnd as alternating vars.
        // these variables should have odd degree, though that will not be checked
        // iAltVarsStart, iAltVarsEnd are only used together with nc_type=nc_exterior
        // 1 <= iAltVarsStart <= iAltVarsEnd <= r->N
        unsigned int iFirstAltVar, iLastAltVar; // = 0 by default

        // for factors of super-commutative algebras we need 
        // the part of general quotient ideal modulo squares!    
        ideal idSCAQuotient; // = NULL by default. // must be deleted in Kill!
      } sca;
    } data;

  public:
    
    inline nc_type& ncRingType() { return (type); };
    inline nc_type ncRingType() const { return (type); };

    inline unsigned int& FirstAltVar() 
        { assume(ncRingType() == nc_exterior); return (data.sca.iFirstAltVar); };
    inline unsigned int& LastAltVar () 
        { assume(ncRingType() == nc_exterior); return (data.sca.iLastAltVar ); };

    inline unsigned int FirstAltVar() const 
        { assume(ncRingType() == nc_exterior); return (data.sca.iFirstAltVar); };
    inline unsigned int LastAltVar () const 
        { assume(ncRingType() == nc_exterior); return (data.sca.iLastAltVar ); };

    inline ideal& SCAQuotient() 
        { assume(ncRingType() == nc_exterior); return (data.sca.idSCAQuotient); };
  private:

    CGlobalMultiplier* m_Multiplier;
    CFormulaPowerMultiplier* m_PowerMultiplier;

  public:
 
    inline CGlobalMultiplier* GetGlobalMultiplier() const
        { return (m_Multiplier); };

    inline CGlobalMultiplier*& GetGlobalMultiplier()
        { return (m_Multiplier); };


    inline CFormulaPowerMultiplier* GetFormulaPowerMultiplier() const
        { return (m_PowerMultiplier); };

    inline CFormulaPowerMultiplier*& GetFormulaPowerMultiplier()
        { return (m_PowerMultiplier); };
   
  public:
    nc_pProcs p_Procs; // NC procedures.

};




// //////////////////////////////////////////////////////////////////////// //
// NC inlines

static inline nc_struct*& GetNC(ring r)
{
  return r->GetNC();
}

static inline nc_type& ncRingType(nc_struct* p)
{
  assume(p!=NULL);
  return (p->ncRingType());
}

static inline nc_type ncRingType(ring r) // Get
{
  if(rIsPluralRing(r))
    return (ncRingType(r->GetNC()));
  else
    return (nc_error);
}

static inline void ncRingType(ring r, nc_type t) // Set
{
  assume((r != NULL) && (r->GetNC() != NULL));
  ncRingType(r->GetNC()) = t;
}

static inline void ncRingType(nc_struct* p, nc_type t) // Set
{
  assume(p!=NULL);
  ncRingType(p) = t;
}




// //////////////////////////////////////////////////////////////////////// //
// we must always have this test!?
static inline bool rIsSCA(const ring r)
{
#ifdef HAVE_PLURAL
  return rIsPluralRing(r) && (ncRingType(r) == nc_exterior);
#else
  return false;
#endif
}

// //////////////////////////////////////////////////////////////////////// //
// NC inlines


/// general NC-multiplication with destruction
poly _nc_p_Mult_q(poly p, poly q, const ring r);

/// general NC-multiplication without destruction
poly _nc_pp_Mult_qq(const poly p, const poly q, const ring r);



/// for p_Minus_mm_Mult_qq in pInline2.h
poly nc_p_Minus_mm_Mult_qq(poly p, const poly m, const poly q, int &lp,
                                    const int, const poly, const ring r);

// // for p_Plus_mm_Mult_qq in pInline2.h
// returns p + m*q destroys p, const: q, m
poly nc_p_Plus_mm_Mult_qq(poly p, const poly m, const poly q, int &lp,
                              const int, const ring r);




// returns m*p, does neither destroy p nor m
static inline poly nc_mm_Mult_pp(const poly m, const poly p, const ring r)
{
  assume(rIsPluralRing(r));
  assume(r->GetNC()->p_Procs.mm_Mult_pp!=NULL);
  return r->GetNC()->p_Procs.mm_Mult_pp(m, p, r);
//  return pp_Mult_mm( p, m, r);
}


// returns m*p, does destroy p, preserves m
static inline poly nc_mm_Mult_p(const poly m, poly p, const ring r)
{
  assume(rIsPluralRing(r));
  assume(r->GetNC()->p_Procs.mm_Mult_p!=NULL);
  return r->GetNC()->p_Procs.mm_Mult_p(m, p, r);
//   return p_Mult_mm( p, m, r);
}

static inline poly nc_CreateSpoly(const poly p1, const poly p2, const ring r)
{
  assume(rIsPluralRing(r));
  assume(r->GetNC()->p_Procs.SPoly!=NULL);
  return r->GetNC()->p_Procs.SPoly(p1, p2, r);
}

// ?
poly nc_CreateShortSpoly(poly p1, poly p2, const ring r);

/* brackets: p will be destroyed... */
poly nc_p_Bracket_qq(poly p, const poly q, const ring r);

static inline poly nc_ReduceSpoly(const poly p1, poly p2, const ring r)
{
  assume(rIsPluralRing(r));
  assume(r->GetNC()->p_Procs.ReduceSPoly!=NULL);
#ifdef PDEBUG
//  assume(p_LmDivisibleBy(p1, p2, r));
#endif
  return r->GetNC()->p_Procs.ReduceSPoly(p1, p2, r);
}

void nc_PolyPolyRed(poly &b, poly p, number *c, const ring r);

/*
static inline void nc_PolyReduce(poly &b, const poly p, number *c, const ring r) // nc_PolyPolyRed
{
  assume(rIsPluralRing(r));
//  assume(r->GetNC()->p_Procs.PolyReduce!=NULL);
//  r->GetNC()->p_Procs.PolyReduce(b, p, c, r);
}
*/

static inline void nc_kBucketPolyRed(kBucket_pt b, poly p, number *c)
{
  const ring r = b->bucket_ring;
  assume(rIsPluralRing(r));

//   return gnc_kBucketPolyRedNew(b, p, c);

  assume(r->GetNC()->p_Procs.BucketPolyRed!=NULL);
  return r->GetNC()->p_Procs.BucketPolyRed(b, p, c);
}

static inline void nc_BucketPolyRed_Z(kBucket_pt b, poly p, number *c)
{
  const ring r = b->bucket_ring;
  assume(rIsPluralRing(r));

//   return gnc_kBucketPolyRed_ZNew(b, p, c);

  assume(r->GetNC()->p_Procs.BucketPolyRed_Z!=NULL);
  return r->GetNC()->p_Procs.BucketPolyRed_Z(b, p, c);

}


/* subst: */
poly nc_pSubst(poly p, int n, poly e, const ring r);

// set pProcs table for rGR and global variable p_Procs
// this should be used by p_ProcsSet in p_Procs_Set.h
void nc_p_ProcsSet(ring rGR, p_Procs_s* p_Procs);


// the part, related to the interface
// Changes r, Assumes that all other input belongs to curr
BOOLEAN nc_CallPlural(matrix cc, matrix dd, poly cn, poly dn, ring r,
                      bool bSetupQuotient, //< false
                      bool bCopyInput, //< true
                      bool bBeQuiet, //< false
                      ring curr,
                      bool dummy_ring = false 
		      /* allow to create a nc-ring with 1 variable*/);


BOOLEAN nc_rComplete(const ring src, ring dest, bool bSetupQuotient = true); // in ring.cc

// this function should be used inside QRing definition!
// we go from rG into factor ring rGR with factor ideal rGR->qideal.
bool nc_SetupQuotient(ring rGR, const ring rG = NULL, bool bCopy = false); // rG == NULL means that there is no base G-algebra


bool nc_rCopy(ring res, const ring r, bool bSetupQuotient);

poly pOppose(ring Rop_src, poly p, const ring Rop_dst);
ideal idOppose(ring Rop_src, ideal I, const ring Rop_dst);



// returns the LCM of the head terms of a and b with the given component 
// NOTE: coeff will be created but remains undefined(zero?) 
poly p_Lcm(const poly a, const poly b, const long lCompM, const ring r);

// returns the LCM of the head terms of a and b with component = max comp. of a & b
// NOTE: coeff will be created but remains undefined(zero?) 
poly p_Lcm(const poly a, const poly b, const ring r);





// const int GRMASK = 1 << 1;
const int SCAMASK = 1; // For backward compatibility
const int TESTSYZSCAMASK = 0x0100 | SCAMASK; //

// NCExtensions Mask Property 
int& getNCExtensions();
int  setNCExtensions(int iMask);

// Test
bool ncExtensions(int iMask); //  = 0x0FFFF



#ifdef PLURAL_INTERNAL_DECLARATIONS

#include <polys/matpol.h>

// read only access to NC matrices C/D:
// get C_{i,j}, 1 <= row = i < j = col <= N
static inline poly GetC( const ring r, int i, int j ) 
{
  assume(r!= NULL && rIsPluralRing(r));
  const matrix C = GetNC(r)->C;
  assume(C != NULL);
  const int ncols = C->ncols;
  assume( (i > 0) && (i < j) && (j <= ncols) );
  return ( C->m[ncols * ((i)-1) + (j)-1] );
}

// get D_{i,j}, 1 <= row = i < j = col <= N
static inline poly GetD( const ring r, int i, int j ) 
{
  assume(r!= NULL && rIsPluralRing(r));
  const matrix D = GetNC(r)->D;
  assume(D != NULL);
  const int ncols = D->ncols;
  assume( (i > 0) && (i < j) && (j <= ncols) );
  return ( D->m[ncols * ((i)-1) + (j)-1] );
}

#endif /* PLURAL_INTERNAL_DECLARATIONS */

#endif /* HAVE_PLURAL */

#endif /* POLYS_NC_H */
