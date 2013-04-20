/* file:        SDkutil.h
 * authors:     Grischa Studzinski & Benjamin Schnitzler
 * created:     ask git
 * last change: ask git
 *
 * DESCRIPTION:
 * This file contains extensions to some structures and
 * functions defined in kutil.h (...Object, ...Strategy, etc.).
 *
 * TODO: bidubidu
 */


#ifndef SDKUTIL_H
#define SDKUTIL_H

namespace ShiftDVec
{ uint CreateDVec(poly p, ring r, uint*& dvec); }

#include <kernel/kutil.h>

typedef unsigned int uint;


namespace ShiftDVec
{
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
    ( ShiftDVec::sTObject * t1, 
      ShiftDVec::sTObject * t2, int numVars );

  uint findRightOverlaps
    ( ShiftDVec::sTObject * t1, ShiftDVec::sTObject * t2, 
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

  void dvecWrite(const sTObject* t);

  void lcmDvecWrite(const sLObject* t);

  extern int lpDVCase; //defined in kutil2.cc

  uint lcmDivisibleBy
    ( ShiftDVec::sLObject * lcm, 
      ShiftDVec::sTObject * p, int numVars );

  void lcmDvecWrite(sLObject* t);
};



/* classes for ShiftDVec case: ...Object, ...Strategy */



/* Inheritance for the ShiftDVec case explained
 * 
 *             sTObject
 *            /        \
 *           / (virtual)\
 *          /            \
 *      sLObject    ShiftDVec::sTObject
 *           \          /
 *            \        / 
 *             \      /
 *        ShiftDVec::sLObject
 *
 * sTObject and sLObject are those from kutil.h
 */

class sTObject;
class ShiftDVec::skStrategy;

class ShiftDVec::sTObject : public virtual ::sTObject
{
public:
  uint * dvec; //Distance Vector of lm(p)
  uint dvSize; //size of the >>uint * dvec<< array

  uint  shift; //shift of polynomial
  //TODO: this shift shall replace the necessity of storing
  //      shifts of polynomials, when creating pairs

  //uint pIsInR; //already i_r in ::sTObject

  // constructors
  sTObject(ring r = currRing) : 
    ::sTObject(r), dvec(NULL){ t_p = NULL; }

  sTObject(poly p, ring tailRing = currRing) : 
    ::sTObject(p, tailRing),  dvec(NULL){ t_p = NULL; }

  sTObject(poly p, ring c_r, ring tailRing) : 
    ::sTObject(p, c_r, tailRing),  dvec(NULL){ t_p = NULL; }

  sTObject(sTObject* T, int copy) : 
    ::sTObject(T, copy),  dvec(NULL){ t_p = NULL; }

#if 0
  //destructor - 
  //freeing dvecs automatically was to problematic with singular
  ~sTObject()
  { this->freeDVec(); }
#endif

  void dumbInit(poly p_){ p = p_; dvec = NULL; t_p = NULL; }

  void SetDVecIfNULL(poly p, ring r)
  { if(!dvec) SetDVec(p, r); }

  void SetDVecIfNULL()
  { if(!dvec) SetDVec(); }
  
  // Initialize the distance vector of an ShiftDVec::sTObject
  void SetDVec(poly p, ring r)
  { 
    freeDVec(); 
    dvSize = CreateDVec(p, r, dvec); 
  }

  // Initialize the distance vector of an ShiftDVec::sTObject
  void SetDVec()
  { 
    freeDVec(); 
    if(p == NULL)
      dvSize = CreateDVec(t_p, tailRing, dvec); 
    else
      dvSize = CreateDVec(p, currRing, dvec); 
  }

  void SetDVec(uint* dv) {dvec = dv;}

  uint*  GetDVec(); 
  uint  getDVecAtIndex(uint index) {return dvec[index];}
  uint GetDVsize(); 

  int cmpDVec(ShiftDVec::sTObject* toCompare);

  int cmpDVec
    (ShiftDVec::sTObject* T1, uint begin, uint beginT1, uint size);

  int cmpDVecProper
    ( ShiftDVec::sTObject* T1, 
      uint begin, uint beginT1, uint size, int lV );

  void freeDVec(); 

  // may need adjustments
  ShiftDVec::sTObject& operator=
    (const ShiftDVec::sTObject& t);

  uint divisibleBy( ShiftDVec::sTObject * T, 
                    int numVars              )
  { SetDVecIfNULL();
    T->SetDVecIfNULL();
    return ShiftDVec::divisibleBy
      (dvec, dvSize, T->dvec, T->dvSize, numVars); }
};

class ShiftDVec::skStrategy : public ::skStrategy
{
  private:
    /* BOCO DESCRIPTION:
     * This is needed for the calculation of a left GB in a
     * factor algebra K<X>/I; so this will be used in the
     * ShiftDVec::LeftGB namespace
     *
     * BOCO TODO:
     * I is probably subset of R; so either we will keep
     * references to the Objects in R here, or we will delete
     * them completely and instead let get_I_at(i) handle the
     * translation from R to I at index i. We will decide that
     * later. However: we have to care for the Initialization
     * somewhere.
     *   Question: Will I be part of T ??? - I don't think so,
     * but again: What was the difference between T and R ?
     */
    ShiftDVec::sTObject* I;
    uint size_of_I; // This is, what it says!
    int lV;
    int uptodeg;

  public:
    void init_I(ideal I){/*TODO*/ assume(0);}
    uint get_size_of_I(){return size_of_I;}
    uint translate_index_I_to_R(uint i){/*TODO*/ assume(0);}
    ShiftDVec::sLObject get_I_at(uint i){return I[i];}
    void deleteFromL(uint index){/*TODO*/ assume(0);}

}

class ShiftDVec::sLObject :
  public ShiftDVec::sTObject, public ::sLObject
{
  private:
    /* BOCO: Important
     * The lcmDvec and lcmDvSize need to be set to NULL
     * respectivly 0, every time, the lcm changes and at the
     * creation of an LObject.
     * See kutil2.cc for most definitions.
     * TODO:
     * Do the latter in the constructor.
     */
    uint*   lcmDvec;   /*- the corresponding dvec -*/
    uint  lcmDvSize;

  public:
    void SetLcmDVec(poly p, ring r = currRing)
    { lcmDvSize = ShiftDVec::CreateDVec(p, r, lcmDvec); }

    //uses the LObjects lcm or p1, p2 if USE_DVEC_LCM is set.
    void SetLcmDVec(ring r = currRing);

    void SetLcmDvecIfNULL() {if(!lcmDvec) SetLcmDVec();}

    void SetLcmDvecToNULL() {lcmDvec = NULL; lcmDvSize = 0;}

    uint getLcmDvSize(ring r = currRing);

    bool gm3LcmUnEqualToLcm
      (poly p1, poly p2, int lV, ring r = currRing);

    void freeLcmDVec();

    uint lcmDivisibleBy( ShiftDVec::sTObject * T, int numVars );

    uint lcmDivisibleBy
      ( ShiftDVec::sTObject * T, 
        uint minShift, uint maxShift, int numVars );

    bool compareLcmTo( sLObject* other, ring r = currRing );

    bool compareLcmTo( poly p1, poly p2, ring r = currRing );
}



/* other stuff */



ideal kStdShiftDVec
  ( ideal F, ideal Q, tHomog h,intvec ** w, intvec *hilb,
    int syzComp, int newIdeal, intvec *vw, int uptodeg, int lV );

void initBuchMoraCritShiftDVec(kStrategy strat);

ideal bbaShiftDVec
  ( ideal F, ideal Q,intvec *w,intvec *hilb,kStrategy strat, 
    int uptodeg, int lV                                      );

void initBuchMoraCritShiftDVec(kStrategy strat);

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



/* vim: set foldmethod=syntax : */
