/*! @file
 *
 * This file contains extensions to some structures and
 * functions defined in kutil.h (...Object, ...Strategy, etc.).
 *
 * @brief This file contains extensions to some structures and
 *        declarations of utility functions and some analogs to
 *        functions from kutil.h .
 *
 * This file is part of the Letterplace ShiftDVec Project
 *
 * @author Grischa Studzinski
 * @author Benjamin Schnitzler benjaminschnitzler@googlemail.com
 *
 * @copyright see main copyright notice for Singular
 *
 *
 * \par Object Extensions and Inheritance:
 *
\verbatim
sTObject --- contains ---> ShiftDVec::sTObjectExtension
    ^                                   ^
    |                                   |
sLObject --- contains ---> ShiftDVec::sLObjectExtension
\endverbatim
 * The sTObjectExtension contains the DVec for it corresponding
 * sTObject.
 *
 * If the sLObjectExtension corresponds to an sLObject, which is
 * a pair, from which we want to build an S-Polynomial, this pair
 * theoretically consists of an unshifted and a shifted
 * polynomial; however, we will only store the shift of the
 * second polynomial in the Extension.
 *
 * Note to myself: Better do not use private members in any of
 * the classes, since the standard does not define, how members
 * from the private/public areas are lay out in memory (in
 * which order) and this might be important for the singular
 * memory management.
 *
 *
 * \todo The extensions could be stored in sets paralell to
 *       strat->L and strat->T using Singular memory management.
 * \todo For Release: Tidy up; remove unnessesary procedures;
 *       remove debug stuff (partially and improve what is left)
 * \todo many other things
 */

#include <kernel/kutil.h>


#ifndef SDKUTIL_H
#define SDKUTIL_H

namespace ShiftDVec
{
  namespace SD = ShiftDVec;

  typedef unsigned int uint;

  class skStrategy;
  class sLObjectExtension;
  class sTObjectExtension;
  typedef skStrategy* kStrategy;

  uint CreateDVec(poly p, ring r, uint*& dvec);

  uint getShift
    (poly p, uint numFirstVar, ring r = currRing );

  bool compareDVec
      ( const uint* dvec, poly p, 
        uint offset, uint maxSize, ring r = currRing );

  uint divisibleBy
    ( const uint* dvec1, uint dvSize1, 
      const uint* dvec2, uint dvSize2, int numVars );

  uint divisibleBy
    ( const uint* dvec1, uint dvSize1, 
      const uint* dvec2, uint dvSize2, 
      uint minShift, uint maxShift, int numVars );

  uint divisibleBy
    ( sTObject * t1, sTObject * t2, int numVars );

  uint RdivisibleBy
      ( const uint* dvec1, uint dvSize1, 
        const uint* dvec2, uint dvSize2, int numVars );

  uint findRightOverlaps
    ( TObject * t1, TObject * t2, 
      int numVars, int maxDeg, uint ** overlaps );

  BOOLEAN redViolatesDeg
    ( poly a, poly b, int uptodeg,
      ring aLmRing = currRing, 
      ring bLmRing = currRing, ring bTailRing = currRing );
  BOOLEAN redViolatesDeg
    ( TObject* a, TObject* b, 
      int uptodeg, ring lmRing = currRing );

  bool createSPviolatesDeg
    ( poly a, poly b, uint shift, int uptodeg, 
      ring aLmRing = currRing, ring bLmRing = currRing, 
      ring aTailRing = currRing, ring bTailRing = currRing );
  bool shiftViolatesDeg
    ( poly p, uint shift, int uptodeg, 
      ring pLmRing = currRing, ring pTailRing = currRing );

  void dvecWrite(const poly p, ring r = currRing);

  void dvecWrite(const uint* dvec, uint dvSize);

  void dvecWrite(const TObject* t);

  void lcmDvecWrite(const LObject* t);

  extern int lpDVCase; //defined in kutil2.cc

  uint lcmDivisibleBy
    ( LObject * lcm, TObject * p, int numVars );

  void lcmDvecWrite(LObject* t);

  int cmpDVecProper
      ( uint* dv1, uint begin_dv1,
        uint* dv2, uint begin_dv2, uint size, int lV );

  int cmpDVecProper
      ( sTObject* T1, uint beginT1,
        sTObject* T2, uint beginT2, uint size, int lV );

  long p_lmCopy( poly p1, ring r1,
                 poly p2, ring r2, int lV, long nexp=-1 );

  long p_lmAppend( poly p1, long o1, ring r1,
                   poly p2, long o2, ring r2,
                   int lV, long nexp=-1 );
}


/* classes/class extensions for ShiftDVec case */


/** This extension corresponds to an sTObject
 *
 * It contains different procedures for creating a dvec of the
 * polynomials of the sTObject and to check divisibility of
 * monomials by having divisibility criterions for dvecs and
 * checking them.
 */
class ShiftDVec::sTObjectExtension
{
  public:
    /** We simulate polymorphism by storing the type of the
     *  extension
     */
    enum { TObject_Extension = 1,
           LObject_Extension = 2 } Extension_Type;

    /** It may be possible to have several Objects storing the
     *  same extension. We count how many do, so that we can
     *  delete the Extension, when the last possesor is deleted.
     *
     *  \todo nice idea, but not really necessary! Remove it!
     */
    int number_of_possesors;

    sTObject* T; //!< The sTObject, this object is contained in
    uint * dvec; //!< Distance Vector of lm(p)
    uint dvSize; //!< size of the >>uint * dvec<< array

    //uint pIsInR; //already i_r in ::sTObject

    // constructor
    sTObjectExtension(sTObject* _T) :
      T(_T), dvec(NULL), number_of_possesors(1) {}

    // destructor
    ~sTObjectExtension() { freeDVec(); }

    void Set_Number_Of_Possesors(int num)
    { number_of_possesors = num; }

    void Increase_Number_Of_Possessors(){++number_of_possesors;}

    void dumbInit(sTObject* _T){ T = _T; dvec = NULL; }

    //! Initialize the distance vector of this extension
    void SetDVecIfNULL(poly p, ring r)
    { if(!dvec) SetDVec(p, r); }

    //! Initialize the distance vector of this extension
    void SetDVecIfNULL()
    { if(!dvec) SetDVec(); }
    
    //! Initialize the distance vector of this extension
    void SetDVec(poly p, ring r)
    {
      freeDVec(); 
      dvSize = CreateDVec(p, r, dvec); 
    }

    //! Initialize the distance vector of this extension
    void SetDVec()
    {
      freeDVec();
      if(T->p == NULL)
        dvSize = CreateDVec(T->t_p, T->tailRing, dvec);
      else
        dvSize = CreateDVec(T->p, currRing, dvec);
    }

    //! Initialize the distance vector of this extension
    void SetDVec(uint* dv) {dvec = dv;}

    uint*  GetDVec(); 
    uint  getDVecAtIndex(uint index) {return dvec[index];}
    uint GetDVsize(); 

    //! \todo Delete this function
    int cmpDVec(sTObject* toCompare);

    //! \todo Delete this function
    int cmpDVec
      (sTObject* T1, uint begin, uint beginT1, uint size);

    int cmpDVecProper
      ( sTObject* T1, 
        uint begin, uint beginT1, uint size, int lV );

    void freeDVec(); 

    /** @brief Test if this->dvec is divisible by the polynomial
     *         of Tobj. We will return the first shift.
     *
     *  If fromRight is true, we will only check, if this->dvec
     *  will be divided from the right, d.i. if Tobj dvec
     *  corresponds to a tail of this->dvec .
     *
     *  If the dvecs do not exist (are equal to NULL) they will
     *  be created.
     */
    uint divisibleBy( sTObject * Tobj,
                      int numVars, bool fromRight = false )
    {
      SetDVecIfNULL();
      Tobj->SD_Ext()->SetDVecIfNULL();
      if( fromRight )
      {
        return ShiftDVec::RdivisibleBy
                           ( dvec, dvSize,
                             Tobj->SD_Ext()->dvec,
                             Tobj->SD_Ext()->dvSize, numVars );
      }
      else
      {
        return ShiftDVec::divisibleBy
                           ( dvec, dvSize,
                             Tobj->SD_Ext()->dvec,
                             Tobj->SD_Ext()->dvSize, numVars );
      }
    }

    /** @brief Check first if the component is zero or if the
     *         components of both polynomials equal.
     */
    uint divisibleBy_Comp( sTObject * Tobj,
                           poly sTObject::*tpoff,
                           ring r, int numVars,
                           bool fromRight = false )
    {
      p_LmCheckPolyRing1( Tobj->*tpoff, r );
      pIfThen1( T->p != NULL,
                p_LmCheckPolyRing1(Tobj->*tpoff, r) );
      if ( p_GetComp(T->p, r) == 0 || 
           p_GetComp(T->p, r) == p_GetComp(Tobj->*tpoff,r) )
      { return
          divisibleBy( Tobj, tpoff, r, numVars, fromRight ); }
      return UINT_MAX;
    }

    /** @brief Test if this->dvec is divisible by the polynomial
     *         of Tobj. We will return the first shift.
     *
     *
     *  This procecure is similar to
     *  uint divisibleBy( sTObject * Tobj,
     *                    int numVars, bool fromRight = false )
     *  except for, that we can decide, if we want to use Tobj->p
     *  or Tobj->t_p
     *
     *  \todo This function is complicate to use. Do we really
     *        need it?
     */
    uint divisibleBy( sTObject * Tobj,
                      poly sTObject::* tpoff,
                      ring r, int numVars, bool fromRight )
    {
      SetDVecIfNULL();
      Tobj->SD_Ext()->SetDVecIfNULL( Tobj->*tpoff, r );

      if( fromRight )
      {
        return ShiftDVec::RdivisibleBy
                           ( dvec, dvSize,
                             Tobj->SD_Ext()->dvec,
                             Tobj->SD_Ext()->dvSize, numVars );
      }
      else
      {
        return ShiftDVec::divisibleBy
                           ( dvec, dvSize,
                             Tobj->SD_Ext()->dvec,
                             Tobj->SD_Ext()->dvSize, numVars );
      }
    }
};

/** This extension corresponds to a Pair (an sLObject)
 *
 * It contains different procedures for creating a dvec of an
 * S-Polynomial or comparing the Lcm of different S-Polynomials,
 * which are used for Gebauer-Moeller .
 *
 * \todo are all these lcm procedures really used at the moment?
 *       We should get rid of the stuff, we don't need.
 */
class ShiftDVec::sLObjectExtension :
  public ShiftDVec::sTObjectExtension
{
  public:
    uint shift_p2;

    // constructor
    sLObjectExtension(sLObject* T) : sTObjectExtension(T) {}

    uint getLcmDVSize(ring r = currRing);

    //uses the LObjects lcm or p1, p2 if USE_DVEC_LCM is set.
    void SetLcmDVec(ring r = currRing);

    void SetLcmDvecIfNULL( ring r = currRing )
    { if(!dvec) SetLcmDVec(r); }

    void SetLcmDvecToNULL() { dvec = NULL; dvSize = 0; }

    bool gm3LcmUnEqualToLcm( poly p1, poly p2,
                             int lV, ring r = currRing );

    uint lcmDivisibleBy( sTObject * T, int numVars );

    uint lcmDivisibleBy( sTObject * T, uint minShift,
                         uint maxShift, int numVars   );

    bool compareLcmTo( sLObject* other, ring r = currRing );

    bool compareLcmTo( poly p1, poly p2, ring r = currRing );

    sLObject* get_LObject()
    { return static_cast<sLObject*>(T); }
};

/** Extended skStrategy class for the LPDV project
 *
 * We store some useful additional information here (like uptodeg
 * and lV). We plan also to store some additional information and
 * an additional set for Left-GB calculation.
 *
 * \todo We should have parallel to the T/L sets some extension
 *       sets here to be more Singular conforment. And to have
 *       better an cleaner memory-management.
 */
class ShiftDVec::skStrategy : public ::skStrategy
{
  public:
    /** LeftGB calculation - factored ideal U (in K<X>/U)
     *
     * This is needed for the calculation of a left GB in a
     * factor algebra K<X>/U; so this will be used in the
     * ShiftDVec::LeftGB namespace
     *
     * \todo U is probably subset of R; so either we will keep
     *       references to the Objects in R here, or we will
     *       delete them completely and instead let get_U_at(i)
     *       handle the translation from R to U at index i. We
     *       will decide that later. However: we have to care for
     *       the Initialization somewhere.
     *       Question: Will U be part of T ??? - I don't think
     *       so, but again: What was the difference between T and
     *       R ?
     */
    TObject* U; //first index:0; last index: size_of_U-1
    uint size_of_U; // This is, what it says!
    int lV;
    int uptodeg;

    void init_lV(int lV){this->lV = lV;}
    int get_lV(){return lV;}

    void init_uptodeg(int uptodeg){this->uptodeg = uptodeg;}
    int get_uptodeg(){return uptodeg;}

    void init_U(ideal U){/*TODO*/ assume(0);}
    uint get_size_of_U(){return size_of_U;}
    uint translate_index_U_to_R(uint i){/*TODO*/ assume(0);}
    TObject* get_U_at(uint i){return &(U[i]);}

    bool is_lgb_case() { return false; /*TODO*/ }
};


/* other stuff */


ideal kStdShiftDVec
  ( ideal F, ideal Q, tHomog h,intvec ** w, intvec *hilb,
    int syzComp, int newIdeal, intvec *vw, int uptodeg, int lV );

void initBuchMoraCritShiftDVec( ShiftDVec::kStrategy strat);

ideal bbaShiftDVec
  ( ideal F, ideal Q,intvec *w,intvec *hilb,
    ShiftDVec::kStrategy strat, int uptodeg, int lV );

void initBuchMoraCritShiftDVec( ShiftDVec::kStrategy strat);

ideal freegbdvc(ideal I, int uptodeg, int lVblock);


//BOCO: lpMult(Profiler) stuff
poly lpMult( poly p, poly m, int uptodeg, int lV  );
lists lpMultProfiler
  ( lists L, int uptodeg, int lV, int n );
lists lpMultProfilerR
  ( lists L, int uptodeg, int lV, int n, int resolution );

//LPDV Template Test
poly TemplateTestLPDV( poly p, poly q, int uptodeg, int lV );

#endif  //#ifndef SDKUTIL_H

// vim: set foldmethod=syntax :
// vim: set textwidth=65 :
// vim: set colorcolumn=+1 :
