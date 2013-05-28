/* file:        SDTObjectExtension.h
 * authors:     Grischa Studzinski & Benjamin Schnitzler
 * created:     ask git
 * last change: ask git
 *
 * DESCRIPTION:
 * This is a file on your computer. Ok, it may not be your
 * computer. So this is a file on a computer. Hm, maybe the
 * storage device containing this file is not located in a
 * computer. Well at least, I am pretty sure that this is a
 * file... Ha! As You are reading this file, it is contained
 * in the RAM on a computer You are using! Hm, what is the
 * definition of "computer"? Maybe not the whole file is
 * contained in the RAM... . It is to complicated, I give up.
 */


#ifndef SDKUTIL_H
#define SDKUTIL_H

namespace ShiftDVec
{
  namespace SD = ShiftDVec;

  typedef unsigned int uint;

  class sTObjectExtension;

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


/* classes/class extensions for ShiftDVec case */


/* Inheritance for the ShiftDVec case explained
 *
 * sTObject --- contains ---> ShiftDVec::sTObjectExtension
 *     ^
 *     |
 * sLObject
 *     ^
 *     |
 * ShiftDVec::sLObject
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
