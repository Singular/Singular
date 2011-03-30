#ifndef POLYS_NC_H
#define POLYS_NC_H

#ifdef HAVE_PLURAL



// TODO: the following is a part of ring.h... would be nice to have a
// clear public NC interface defined here!

#include <polys/monomials/ring.h>
#include <polys/kbuckets.h>


class ip_smatrix;
typedef ip_smatrix *       matrix;

class skStrategy;
typedef skStrategy * kStrategy;



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

// NC pProcs:
typedef poly (*mm_Mult_p_Proc_Ptr)(const poly m, poly p, const ring r);
typedef poly (*mm_Mult_pp_Proc_Ptr)(const poly m, const poly p, const ring r);



typedef ideal (*GB_Proc_Ptr)(const ideal F, const ideal Q, const intvec *w, const intvec *hilb, kStrategy strat, const ring r);

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

  GB_Proc_Ptr                           GB;
//                                         GlobalGB, // BBA
//                                         LocalGB;  // MORA
};

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
/*
  private:

   CGlobalMultiplier* m_Multiplier;
    CFormulaPowerMultiplier* m_PowerMultiplier;

  public:
 
    inline CGlobalMultiplier* GetGlobalMultiplier() const
        { assume(ncRingType() != nc_exterior); return (m_Multiplier); };

    inline CGlobalMultiplier*& GetGlobalMultiplier()
        { assume(ncRingType() != nc_exterior); return (m_Multiplier); };


    inline CFormulaPowerMultiplier* GetFormulaPowerMultiplier() const
        { assume(ncRingType() != nc_exterior); return (m_PowerMultiplier); };

    inline CFormulaPowerMultiplier*& GetFormulaPowerMultiplier()
        { assume(ncRingType() != nc_exterior); return (m_PowerMultiplier); };
*/
   
  public:
    nc_pProcs p_Procs; // NC procedures.

};




// //////////////////////////////////////////////////////////////////////// //
// NC inlines

inline nc_struct*& GetNC(ring r)
{
  return r->GetNC();
}; 

inline nc_type& ncRingType(nc_struct* p)
{
  assume(p!=NULL);
  return (p->ncRingType());
};

inline nc_type ncRingType(ring r) // Get
{
  if(rIsPluralRing(r))
    return (ncRingType(r->GetNC()));
  else
    return (nc_error);
};

inline void ncRingType(ring r, nc_type t) // Set
{
  assume((r != NULL) && (r->GetNC() != NULL));
  ncRingType(r->GetNC()) = t;
};


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
inline poly nc_mm_Mult_pp(const poly m, const poly p, const ring r)
{
  assume(rIsPluralRing(r));
  assume(r->GetNC()->p_Procs.mm_Mult_pp!=NULL);
  return r->GetNC()->p_Procs.mm_Mult_pp(m, p, r);
//  return pp_Mult_mm( p, m, r);
}


// returns m*p, does destroy p, preserves m
inline poly nc_mm_Mult_p(const poly m, poly p, const ring r)
{
  assume(rIsPluralRing(r));
  assume(r->GetNC()->p_Procs.mm_Mult_p!=NULL);
  return r->GetNC()->p_Procs.mm_Mult_p(m, p, r);
//   return p_Mult_mm( p, m, r);
}

inline poly nc_CreateSpoly(const poly p1, const poly p2, const ring r)
{
  assume(rIsPluralRing(r));
  assume(r->GetNC()->p_Procs.SPoly!=NULL);
  return r->GetNC()->p_Procs.SPoly(p1, p2, r);
}

inline poly nc_ReduceSpoly(const poly p1, poly p2, const ring r)
{
  assume(rIsPluralRing(r));
  assume(r->GetNC()->p_Procs.ReduceSPoly!=NULL);
#ifdef PDEBUG
//  assume(p_LmDivisibleBy(p1, p2, r));
#endif
  return r->GetNC()->p_Procs.ReduceSPoly(p1, p2, r);
}

/*
inline void nc_PolyReduce(poly &b, const poly p, number *c, const ring r) // nc_PolyPolyRed
{
  assume(rIsPluralRing(r));
//  assume(r->GetNC()->p_Procs.PolyReduce!=NULL);
//  r->GetNC()->p_Procs.PolyReduce(b, p, c, r);
}
*/

inline void nc_kBucketPolyRed(kBucket_pt b, poly p, number *c)
{
  const ring r = b->bucket_ring;
  assume(rIsPluralRing(r));

//   return gnc_kBucketPolyRedNew(b, p, c);

  assume(r->GetNC()->p_Procs.BucketPolyRed!=NULL);
  return r->GetNC()->p_Procs.BucketPolyRed(b, p, c);
}

inline void nc_BucketPolyRed_Z(kBucket_pt b, poly p, number *c)
{
  const ring r = b->bucket_ring;
  assume(rIsPluralRing(r));

//   return gnc_kBucketPolyRed_ZNew(b, p, c);

  assume(r->GetNC()->p_Procs.BucketPolyRed_Z!=NULL);
  return r->GetNC()->p_Procs.BucketPolyRed_Z(b, p, c);

}

inline ideal nc_GB(const ideal F, const ideal Q, const intvec *w, const intvec *hilb, kStrategy strat, const ring r)
{
  assume(rIsPluralRing(r));

  assume(r->GetNC()->p_Procs.GB!=NULL);
  return r->GetNC()->p_Procs.GB(F, Q, w, hilb, strat, r);
}

#endif /* HAVE_PLURAL */

#endif /* POLYS_NC_H */
