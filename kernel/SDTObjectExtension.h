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


#ifndef SDTOBJECTEXTENSION_H
#define SDTOBJECTEXTENSION_H

namespace ShiftDVec
{
  namespace SD = ShiftDVec;

  typedef unsigned int uint;

  class sTObjectExtension;
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

#endif
