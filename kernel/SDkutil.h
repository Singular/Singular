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

//#include <kernel/kutil.h>
//No, we have first to include

namespace ShiftDVec
{
  namespace SD = ShiftDVec;

  typedef unsigned int uint;

  class sLObject;
  class sTObject;
  class skStrategy;
  typedef sLObject LObject;
  typedef sTObject TObject;
  typedef sTObject* TSet;
  typedef sLObject* LSet;
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
    ( SD::TObject * t1, SD::TObject * t2, 
      int numVars, int maxDeg, uint ** overlaps );

  BOOLEAN redViolatesDeg
    ( poly a, poly b, int uptodeg,
      ring aLmRing = currRing, 
      ring bLmRing = currRing, ring bTailRing = currRing );
  BOOLEAN redViolatesDeg
    ( SD::TObject* a, SD::TObject* b, 
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

  void dvecWrite(const SD::TObject* t);

  void lcmDvecWrite(const SD::LObject* t);

  extern int lpDVCase; //defined in kutil2.cc

  uint lcmDivisibleBy
    ( SD::LObject * lcm, SD::TObject * p, int numVars );

  void lcmDvecWrite(SD::LObject* t);

  int cmpDVecProper
      ( uint* dv1, uint begin_dv1,
        uint* dv2, uint begin_dv2, uint size, int lV );

  int cmpDVecProper
      ( SD::sTObject* T1, uint beginT1,
        SD::sTObject* T2, uint beginT2, uint size, int lV );
}

/* classes for ShiftDVec case: ...Object, ...Strategy */



/* Inheritance for the ShiftDVec case explained
 *
 * REMARK:
 * sTObject and sLObject shall not have virtual functions,
 * since it will make them "virtual", d.i. create a virtual
 * table for them. Since in Singular these Objects are often
 * created by the Singular internal memory management (for
 * Performance reasons), their virtual table will not be
 * correctly initialized, making them at best unusable and
 * at worst producing inexplicable segfaults at runtime.
 * Multiple inheritance is for the same reason a "no go!",
 * at least, if one has to avoid the diamond problem, since
 * subclasses inheriting "virtual" will have a virtual table
 * (at least this seems to be right for gcc), which possibly
 * stores the common Base class as a pointer. This is why
 * for inheritance, we had to update our Original Idea, which
 * relied on multiple virtual inheritance, to a simpler model,
 * which duplicates the contents of the sLObject class
 * declaration to the declaration of ShiftDVec::sLObject.
 * To modify ShiftDVec::sLObject we introduced
 * ShiftDVec::sIObject, which inherits from
 * ShiftDVec::sTObject, which in turn inherits from sTObject,
 * see class diagrams below. To see, how the Objects are
 * actually internally represented (their memory layout) use
 * the "-fdump-class-hierarchy" compiler flag, e.g.:
 * make CXXFLAGS='-fdump-class-hierarchy'
 * .
 *
 * case normal bba:
 * 
 * sTObject
 *     ^
 *     |
 *     |
 * sLObject
 *
 *
 * case ShiftDVec::bba updated idea:
 * (this avoids using multiple inheritance)
 * 
 *      sTObject
 *          ^
 *          |
 *          |
 * ShiftDVec::sTObject
 *          ^
 *          |
 *          |
 * ShiftDVec::sIObject (intermediate extensions)
 *          ^
 *          |
 *          |
 * ShiftDVec::sLObject ( which contents consist of an
 *                       exact copy of sLObject's contents. ) 
 * TODO: copy the member functions to the ShiftDVec namespace.
 * Create SDkInline.h for this.
 *
 *
 * case ShiftDVec::bba original Idea:
 * (this is the original idea applying multiple inheritance)
 * 
 * Original Idea:
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
      (SD::sTObject* T1, uint begin, uint beginT1, uint size);

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

class ShiftDVec::sLObject :
  public ShiftDVec::sTObject, public ::sLObject
{
  public:
    /* BOCO: Important
     * The lcmDvec and lcmDvSize need to be set to NULL
     * respectivly 0, every time, the lcm changes and at the
     * creation of an LObject.
     * See kutil2.cc for most definitions.
     * TODO:
     * Do the latter in the constructor.
     */
    uint*   lcmDVec;   /*- the corresponding dvec -*/
    uint  lcmDvSize;

    // constructors
    sLObject(ring r = currRing) : 
      SD::sTObject(r) {/*BOCO: others?*/}

    sLObject(poly p, ring tailRing = currRing) : 
      SD::sTObject(p, tailRing){/*BOCO: others?*/}

    sLObject(poly p, ring c_r, ring tailRing) : 
      SD::sTObject(p, c_r, tailRing){/*BOCO: others?*/}

    void SetLcmDVec(poly p, ring r = currRing)
    { lcmDvSize = ShiftDVec::CreateDVec(p, r, lcmDVec); }

    //uses the LObjects lcm or p1, p2 if USE_DVEC_LCM is set.
    void SetLcmDVec(ring r = currRing);

    void SetLcmDvecIfNULL() {if(!lcmDVec) SetLcmDVec();}

    void SetLcmDvecToNULL() {lcmDVec = NULL; lcmDvSize = 0;}

    uint* getLcmDVec() const { return lcmDVec; }

    uint getLcmDVSize(ring r = currRing);

    bool gm3LcmUnEqualToLcm
      (poly p1, poly p2, int lV, ring r = currRing);

    void freeLcmDVec();

    uint lcmDivisibleBy( ShiftDVec::sTObject * T, int numVars );

    uint lcmDivisibleBy
      ( ShiftDVec::sTObject * T, 
        uint minShift, uint maxShift, int numVars );

    bool compareLcmTo(  ShiftDVec::sLObject* other, ring r = currRing );

    bool compareLcmTo( poly p1, poly p2, ring r = currRing );

    //adapted from original sLObject
    SD::sLObject& operator=(const SD::sTObject& t);
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
    SD::TObject* I; //first index:0; last index: size_of_I-1
    uint size_of_I; // This is, what it says!
    int lV;
    int uptodeg;

    /* We had to overwrite some of the sets/objects from
     * ::skStrategy, since they were generalized for ShiftDVec
     * case.
     */
    SD::TSet T;
    SD::LSet L;
    SD::LSet B;
    SD::LObject P;
    SD::TObject** R;

    /* functions/function pointers we had to overwrite */
    SD::TObject* s_2_t(int i);
    SD::TObject* S_2_T(int i);
    int (*red)(SD::LObject * L,kStrategy strat);
    void initL(ideal F)
    { 
      int nr =
        ((IDELEMS(F)+setmaxLinc-1)/setmaxLinc)*setmaxLinc;
      L = (SD::LSet)omAlloc(nr*sizeof(SD::LObject));
    }
    void initB()
    { B = (SD::LSet)omAlloc(setmaxL*sizeof(SD::LObject)); }
    void initT();
    void initR();


    void init_lV(int lV){this->lV = lV;}
    int get_lV(){return lV;}

    void init_uptodeg(int uptodeg){this->uptodeg = uptodeg;}
    int get_uptodeg(){return uptodeg;}

    void init_I(ideal I){/*TODO*/ assume(0);}
    uint get_size_of_I(){return size_of_I;}
    uint translate_index_I_to_R(uint i){/*TODO*/ assume(0);}
    SD::TObject* get_I_at(uint i){return &(I[i]);}

    void deleteFromL(uint index){/*TODO*/ assume(0);}
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
