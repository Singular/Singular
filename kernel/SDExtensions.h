/* file:        SDkutil.h
 * authors:     Grischa Studzinski & Benjamin Schnitzler
 * created:     ask git
 * last change: ask git
 *
 * DESCRIPTION:
 * This is a file on your computer. Ok, maybe it is not your
 * computer; it's a file on a computer. But what if the storage
 * device, this file is located on is not located in a
 * computer? Well, at least, as you are reading this, this file
 * is located in the RAM of a computer You are using! Or at
 * least part of the file. Hopefully... Maybe not. But I am
 * pretty sure, that this is a file at least.
 *
 * REMARK:
 * This file is strongly related to kutil.h and SDkutil.h .
 *
 * TODO: bidubidu
 */

#ifndef SDEXTENSIONS_H
#define SDEXTENSIONS_H

namespace ShiftDVec
{
  namespace SD = ShiftDVec;

  typedef unsigned int uint;

  class sLObject;
  class skStrategy;
  class sTObjectExtension;
  typedef sLObject LObject;
  typedef sLObject* LSet;
  typedef skStrategy* kStrategy;

  uint CreateDVec(poly p, ring r, uint*& dvec);
}

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

class sTObject;

class ShiftDVec::sTObjectExtension
{
  public:
    sTObject* T; //The sTObject, this object is contained in
    uint * dvec; //Distance Vector of lm(p)
    uint dvSize; //size of the >>uint * dvec<< array

    uint  shift; //shift of polynomial
    //TODO: this shift shall replace the necessity of storing
    //      shifts of polynomials, when creating pairs

    //uint pIsInR; //already i_r in ::sTObject

    // constructor
    sTObjectExtension(sTObject* _T) : dvec(NULL), T(_T) {}

    // TODO: destructor - shall we define one?

    void dumbInit(sTObject* _T){ T = _T; dvec = NULL; }

    // hack for enterL
    void Increase_Number_Of_Possessors(){ ++number_of_possesors; }

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

    uint divisibleBy_Comp( sTObject * Tobj,
                           sTObject::* tpoff,
                           ring r, int numVars )
    {
      p_LmCheckPolyRing1( Tobj->*tpoff, r );
      pIfThen1( this->p != NULL,
                p_LmCheckPolyRing1(Tobj->*tpoff, r) );
      if ( p_GetComp(this->p, r) == 0 || 
           p_GetComp(this->p, r) == p_GetComp(Tobj->*tpoff,r) )
      { return divisibleBy( Tobj, tpoff, r, numVars ); }
      return UINT_MAX;
    }

    uint divisibleBy( sTObject * Tobj,
                      sTObject::* tpoff, ring r, int numVars )
    {
      SetDVecIfNULL();
      Tobj->SD_Ext()->SetDVecIfNULL( Tobj->*tpoff, r );
      return ShiftDVec::divisibleBy
                          ( dvec, dvSize,
                            Tobj->SD_Ext()->dvec,
                            Tobj->SD_Ext()->dvSize, numVars );
    }
};

#endif //ifndef SDEXTENSIONS_H
