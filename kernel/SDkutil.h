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
}


/* classes/class extensions for ShiftDVec case */


/* Object Extensions and Inheritance:
 *
 * sTObject --- contains ---> ShiftDVec::sTObjectExtension
 *     ^                                   ^
 *     |                                   |
 * sLObject --- contains ---> ShiftDVec::sLObjectExtension
 *
 * Note to myself: Better do not use private members in any of
 * the classes, since the standard does not define, how members
 * from the private/public areas are lay out in memory (in
 * which order) and this might be important for the singular
 * memory management.
 */

class ShiftDVec::sTObjectExtension
{
  public:
    // This is used for simulating polymorphism
    enum { TObject_Extension = 1,
           LObject_Extension = 2 } Extension_Type;

    // How many Objects do possess a pointer to this extension
    int number_of_possesors;

    sTObject* T; //The sTObject, this object is contained in
    uint * dvec; //Distance Vector of lm(p)
    uint dvSize; //size of the >>uint * dvec<< array

    uint  shift; //shift of polynomial
    //TODO: this shift shall replace the necessity of storing
    //      shifts of polynomials, when creating pairs

    //uint pIsInR; //already i_r in ::sTObject

    // constructor
    sTObjectExtension(sTObject* _T) :
      T(_T), dvec(NULL), number_of_possesors(1) {}

    // destructor
    ~sTObjectExtension() { freeDVec(); }

    void Set_Number_Of_Possesors(int num)
    { number_of_possesors = num; }

    void dumbInit(sTObject* _T){ T = _T; dvec = NULL; }

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
      if(T->p == NULL)
        dvSize = CreateDVec(T->t_p, T->tailRing, dvec);
      else
        dvSize = CreateDVec(T->p, currRing, dvec);
    }

    void SetDVec(uint* dv) {dvec = dv;}

    uint*  GetDVec(); 
    uint  getDVecAtIndex(uint index) {return dvec[index];}
    uint GetDVsize(); 

    int cmpDVec(sTObject* toCompare);

    int cmpDVec
      (sTObject* T1, uint begin, uint beginT1, uint size);

    int cmpDVecProper
      ( sTObject* T1, 
        uint begin, uint beginT1, uint size, int lV );

    void freeDVec(); 

    uint divisibleBy( sTObject * Tobj, int numVars )
    {
      SetDVecIfNULL();
      Tobj->SD_Ext()->SetDVecIfNULL();
      return ShiftDVec::divisibleBy
                          ( dvec, dvSize,
                            Tobj->SD_Ext()->dvec,
                            Tobj->SD_Ext()->dvSize, numVars );
    }
};

class ShiftDVec::sLObjectExtension :
  public ShiftDVec::sTObjectExtension
{
  public:
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

class ShiftDVec::skStrategy : public ::skStrategy
{
  public:
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
    TObject* I; //first index:0; last index: size_of_I-1
    uint size_of_I; // This is, what it says!
    int lV;
    int uptodeg;

    void init_lV(int lV){this->lV = lV;}
    int get_lV(){return lV;}

    void init_uptodeg(int uptodeg){this->uptodeg = uptodeg;}
    int get_uptodeg(){return uptodeg;}

    void init_I(ideal I){/*TODO*/ assume(0);}
    uint get_size_of_I(){return size_of_I;}
    uint translate_index_I_to_R(uint i){/*TODO*/ assume(0);}
    TObject* get_I_at(uint i){return &(I[i]);}
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

/* vim: set foldmethod=syntax : */
