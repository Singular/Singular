/*! @file
 *
 * @brief Various Helper Functions and Definitions for the
 *        Classes used in the Letterplace ShiftDVec
 *        Implementation
 *
 * This file is part of the Letterplace ShiftDVec Project
 *
 * @author Grischa Studzinski
 * @author Benjamin Schnitzler benjaminschnitzler@googlemail.com
 *
 * @copyright see main copyright notice for Singular
 *
 * \todo Reflect about todo
 * \todo Adaption for the new 'shift free' version of the algorithm
 * \todo good (doxygen) Documentation is yet to be created for the
 *       functions in this file
 * \todo Which functions do need a change for Spielwiese
 */

#include <kernel/kutil.h>
#include <kernel/SDkutil.h>
#include <kernel/SDDebug.h>

#include <climits>

//Needed because of include order; should rather be included in
//ring.h
#include <kernel/mod2.h>

#include <polys/monomials/p_polys.h> //For p_Totaldegree and the like
#include <kernel/polys.h> //For pTotaldegree and the like
#include <kernel/febase.h> //For Print stuff

#include <kernel/SDDebug/SDDebug.h>

typedef skStrategy* kStrategy;
typedef class ShiftDVec::sTObjectExtension TExt;
typedef class ShiftDVec::sLObjectExtension LExt;


//additional member functions of sTObject


/* BOCO:
 * This initializes the TObject extensions for our
 * non-commutative DVec algorithms. Initialized extensions
 * have to be freed with SD_Ext_Delete, if not needed any
 * longer. If the destructor of the corresponding TObject is
 * invoced correctly, SD_Ext_Delete() will be called
 * automatically, and may or may not be called again. Beware
 * however, that this is in Singular not always the case, since
 * TObjects are sometimes allocated with Singulars internal
 * omalloc functions (and are then freed with omfree).
 */
ShiftDVec::sTObjectExtension* sTObject::SD_Ext_Init()
{
  if( SD_Object_Extension ) SD_Ext_Delete();

  SD_Object_Extension = new TExt(this);
  MEMORY_LOG("new TExt", SD_Object_Extension);

  SD_Ext()->Extension_Type = TExt::TObject_Extension;
  SD_Ext()->Set_Number_Of_Possesors(1);

  return SD_Object_Extension;
}

ShiftDVec::sTObjectExtension* sTObject::SD_Ext_Init_If_NULL()
{
  if( SD_Object_Extension == NULL ) return SD_Ext_Init();
  else                              return SD_Object_Extension;
}

/* BOCO:
 * Get possesion of an extension from an other sTObject. The
 * other object will retain possession of the extension.
 */
ShiftDVec::sTObjectExtension*
sTObject::Own_Extension_From(const sTObject* Other_Possessor)
{
  if( SD_Ext() )
  {
    if( SD_Ext() == Other_Possessor->SD_Ext() ) return SD_Ext();
    SD_Ext_Delete();
  }

  if( Other_Possessor->SD_Ext() == NULL )
    return SD_Object_Extension = NULL;

  SD_Object_Extension = Other_Possessor->SD_Ext();
  SD_Ext()->number_of_possesors += 1;

  return SD_Ext();
}

/* BOCO:
 * Get possesion of an extension from an other sTObject. The
 * other object will loose the extension.
 */
ShiftDVec::sTObjectExtension*
sTObject::Acquire_Extension_From(sTObject* Other_Possessor)
{
  if( SD_Ext() )
  {
    if( SD_Ext() == Other_Possessor->SD_Ext() )
    {
      Other_Possessor->SD_Ext_Delete();
      SD_Ext()->T = this;
      return SD_Ext();
    }
    SD_Ext_Delete();
  }

  if( Other_Possessor->SD_Ext() == NULL )
    return SD_Object_Extension = NULL;

  SD_Object_Extension = Other_Possessor->SD_Ext();
  SD_Ext()->number_of_possesors += 1;
  Other_Possessor->SD_Ext_Delete();

  SD_Ext()->T = this;

  return SD_Ext();
}

/** Copy the extension from another object
 *
 *  Copy everything (but set number_of_possessors to 1)!
 *  Deepcopy of everything, EXCEPT the DVec, whose pointer is
 *  just acquired!
 *
 *  \todo this is not a good idea...
 *  \todo optionally make it possible to deepcopy DVec!
 */
ShiftDVec::sTObjectExtension*
sLObject::Copy_Extension_From(const sTObject* from_this)
{
  assume( SD_Object_Extension == NULL );

  if( !from_this->SD_Ext() ) return (SD_Object_Extension=NULL);

  switch( from_this->SD_Ext()->Extension_Type )
  {
    case TExt::TObject_Extension:
      SD_Ext_Init();
      break;
    case TExt::LObject_Extension:
      SD_LExt_Init()->shift_p2 = from_this->SD_LExt()->shift_p2;
      break;
    default:
      assume(0);
  }

  SD_Ext()->dvec   = from_this->SD_Ext()->dvec;
  SD_Ext()->dvSize = from_this->SD_Ext()->dvSize;

  return SD_Object_Extension;
}

/* BOCO:
 * This frees the memory reserved for the T/Lobject extensions.
 * If multiple T/LObjects possess the extension, the extension
 * will not be deleted (no memory is freed), but the possession
 * of it will just be removed from this Object.
 */
void sTObject::SD_Ext_Delete()
{
  if( SD_Object_Extension == NULL ) return;

  SD_Ext()->number_of_possesors -= 1;
  assume(SD_Ext()->number_of_possesors >= 0);
  assume(SD_Ext()->number_of_possesors < 9999);

  if( SD_Ext()->number_of_possesors == 0)
  {
    switch( SD_Ext()->Extension_Type )
    {
      case TExt::TObject_Extension:
        MEMORY_LOG("delete TExt", SD_Object_Extension);
        delete SD_Object_Extension;  break;
      case TExt::LObject_Extension:
        MEMORY_LOG("delete LExt", SD_Object_Extension);
        delete static_cast<ShiftDVec::sLObjectExtension*>
              (SD_Object_Extension); break;
      default:
        assume(0);
    }
  }

  SD_Object_Extension = NULL;
}

ShiftDVec::sLObjectExtension* sTObject::SD_LExt() const
{
  assume(SD_Ext()->Extension_Type == TExt::LObject_Extension);
  return static_cast<ShiftDVec::sLObjectExtension*>(SD_Ext());
}


//additional member functions of sLObject


/* BOCO:
 * Like SD_Ext_Init(), but extension will be of type
 * sLObjectExtension.
 */
ShiftDVec::sLObjectExtension* sLObject::SD_LExt_Init()
{
  if( SD_Object_Extension ) { SD_Ext_Delete(); }
    
  LExt* ext = new LExt(this);
  MEMORY_LOG("new LExt", ext);

  SD_Object_Extension = ext;
  SD_Ext()->Extension_Type = TExt::LObject_Extension;
  SD_Ext()->Set_Number_Of_Possesors(1);

  return ext;
}

ShiftDVec::sLObjectExtension* sLObject::SD_LExt_Init_If_NULL()
{
  if( SD_Object_Extension == NULL ) return SD_LExt_Init();
  else                              return SD_LExt();
}


//member functions of ShiftDVec::sTObjectExtension


uint* ShiftDVec::sTObjectExtension::GetDVec()
{
  return dvec;
}

uint ShiftDVec::sTObjectExtension::GetDVsize()
{
  if(!dvec) SetDVec();
  if(!dvec) return 0; //Be careful! p does not exist!
  return dvSize;
}

int ShiftDVec::sTObjectExtension::cmpDVec(sTObject* T)
{
  if(GetDVsize() == T->SD_Ext()->GetDVsize())
    return memcmp( dvec,
                   T->SD_Ext()->dvec, dvSize * sizeof(uint) );
  return -1;
}

/* Compares part of this object's DVec, beginning at begin, to 
 * part of T1's DVec, beginning at beginT1. Comparison ends
 * after comparing size entries. This function does not check, 
 * if begin + size, respectivly beginT1 + size is larger than
 * this object's DVec size, respectivly the size of T1's DVec.
 * Returns 0 if these parts match, or non-zero value if not
 * (see memcmp for more informations).                        */
int ShiftDVec::sTObjectExtension::cmpDVec
  ( sTObject* T1, uint begin, uint beginT1, uint size )
{
  assume( begin + size <= dvSize &&
          beginT1 + size <= T1->SD_Ext()->dvSize );

    return 
      memcmp( dvec + begin,
              T1->SD_Ext()->dvec+beginT1, size*sizeof(uint) );
}

/* Does what cmpDVec should do. size > 0 should hold.*/
int ShiftDVec::cmpDVecProper
  ( uint* dv1, uint begin_dv1,
    uint* dv2, uint begin_dv2, uint size, int lV )
{
  assume(size > 0);
  assume(begin_dv2 >= 0 && begin_dv1 >= 0);

  if(begin_dv2 == 0 && begin_dv1 != 0)
  {
    int first = 0, i = 0;
    for(; i <= begin_dv1; ++i) first += dv1[i];
    if(first - begin_dv1*lV != dv2[begin_dv2]) return -1;
    size -= 1; begin_dv2 += 1; begin_dv1 += 1;
  }
  else if(begin_dv1 == 0)
  {
    int first = 0, i = 0;
    for(; i <= begin_dv2; ++i) first += dv2[i];
    if(first - begin_dv2*lV != dv1[begin_dv1]) return -1;
    size -= 1; begin_dv2 += 1; begin_dv1 += 1;
  }

  return 
    memcmp( dv2+begin_dv2, dv1+begin_dv1, size*sizeof(uint) );
}

/* Does what cmpDVec should do. size > 0 should hold.*/
int ShiftDVec::cmpDVecProper
  ( sTObject* T1, uint beginT1,
    sTObject* T2, uint beginT2, uint size, int lV )
{
  assume( beginT2+size <= T2->SD_Ext()->dvSize &&
          beginT1+size <= T1->SD_Ext()->dvSize    );

  return
    cmpDVecProper
      ( T1->SD_Ext()->dvec, beginT1,
        T2->SD_Ext()->dvec, beginT2, size, lV );
}

/* Does what cmpDVec should do. size > 0 should hold.*/
int ShiftDVec::sTObjectExtension::cmpDVecProper
  ( sTObject* T1, 
    uint begin, uint beginT1, uint size, int lV )
{
  return 
    SD::cmpDVecProper
      ( T1, beginT1, this->T, begin, size, lV );
}

void ShiftDVec::sTObjectExtension::freeDVec()
{
  if(this && dvec)
  {
    OMFREES( dvec, dvSize, uint );
    dvec = NULL;
    dvSize = 0;
  }
}


//member functions of SD::sLObjectExtension

//Get the size of the lcms dvec. Initialize it, if not set - the
//latter requires p1 and p2 to be set and shifted correctly.
uint ShiftDVec::sLObjectExtension::getLcmDVSize(ring r)
{
  LObject * L = this->get_LObject();
  assume( L->p1 != NULL && L->p2 != NULL );

  if( !dvSize )
  {
    //I hope, this will get me the letterplace degree of lm(p1) 
    //(totaldeg(x(4)*y(5)*z(6)) for example should be 6).
    //BOCO: IMPORTANT: TODO: This was a wrong assuption - maybe
    //use getShift somehow.
    int degP1 = p_Totaldegree(L->p1, r);
    int degP2 = p_Totaldegree(L->p2, r);
    dvSize = degP1 > degP2 ? degP1 : degP2; 
  }

  return dvSize;
}

void ShiftDVec::sLObjectExtension::SetLcmDVec(ring r)
{
#ifdef USE_DVEC_LCM
  //We want to create the lcm directly from p1, p2 .
  //This is an adapted version of the CreateDVec function.
  //TODO: This has to be testet!
  
  assume( get_LObject()->p1 != NULL &&
          get_LObject()->p2 != NULL    );

  //I hope, this will get me the letterplace degree of lm(p1) 
  //(totaldeg(x(4)*y(5)*z(6)) for example should be 6).
  //BOCO: IMPORTANT: TODO: This was a wrong assuption - maybe
  //use getShift somehow.
  uint dvSize1 = p_Totaldegree(get_LObject()->p1, r);
  uint dvSize2 = p_Totaldegree(get_LObject()->p2, r);

  //One of the polys should be shifted in letterplace case.
  //This one overlaps the other one.
  poly shifted; poly notShifted;
  if(dvSize1 > dvSize2)
  {
    poly shifted    = get_LObject()->p1;
    poly notShifted = get_LObject()->p2;
    this->dvSize    = dvSize1;
    dvSize1 = dvSize2;
  }
  else
  {
    poly shifted    = get_LObject()->p2;
    poly notShifted = get_LObject()->p1;
    this->dvSize    = dvSize2;
  }

  assume(dvSize > 0);

  dvec = (uint *)OMALLOC_0( dvSize, uint );

  uint * it = dvec;

  /* The creation of the shift invariant distance vector is
   * similar to its creation in CreateDVec; we will begin with
   * the not shifted poly. After we have created it's dvec we
   * will continue to finish the lcmDvec by beginning at the
   * right postion of the second polynomial.                   */
  int j = 1, i = 1, l = 0;
  for(;l < dvSize1; ++i)
    if(p_GetExp(notShifted,i,r)){*it=j;++it;j=1;++l;} else{++j;}
  for(;l < dvSize; ++i)
    if(p_GetExp(shifted,i,r)){*it=j;++it;j=1;++l;} else{++j;}
#else
  SetDVec(get_LObject()->lcm, r);
#endif
}

/* returns true, if the DVec of this objects lcm is equal to the
 * DVec of the other objects lcm. Thus, if both lcm do not have
 * the same shift, but would otherwise be equal, true will be
 * returned, too.  In every other case, false is returned.    */
bool ShiftDVec::sLObjectExtension::compareLcmTo
  ( sLObject* other, ring r )
{
  if( this->getLcmDVSize(r) !=
      other->SD_LExt()->getLcmDVSize(r) )
  { return false; }

  assume( get_LObject()->p1 != NULL &&
          get_LObject()->p2 != NULL    );
  assume( other->p1 != NULL && other->p2 != NULL );

  //now we need to create the dvecs
  if(!this->dvec) this->SetLcmDVec();
  if(!other->SD_Ext()->dvec) other->SD_LExt()->SetLcmDVec();

  if( memcmp( (void*)(dvec),
              (void*)other->SD_Ext()->dvec, dvSize) )
  { return false; }
  
  return true;
}

/* This function returns true, if the objects lcm is equal to
 * a shift of lcm(p1, p2).                                   
 * WARNING: This function is garbage!                        */
bool ShiftDVec::sLObjectExtension::compareLcmTo
  ( poly p1, poly p2, ring r )
{
  assume(0); //TODO: This function is garbage

  return false;
#if 0
  //We want to obtain the lcm directly from p1, p2 .
  //This is an adapted version of the SetLcmDVec function.
  //TODO: This has to be testet!
  
  assume(p1 != NULL && p2 != NULL);

  //I hope, this will get me the letterplace degree of lm(p1) 
  //(totaldeg(x(4)*y(5)*z(6)) for example should be 6).
  uint dvSize1 = p_Totaldegree(p1, r);
  uint dvSize2 = p_Totaldegree(p2, r);

  //We need to sort out, which poly has the higher degree
  poly hiDeg; poly lowDeg; uint pLcmDvSize;
  if(dvSize1 > dvSize2)
  {
    poly shifted    = p1;
    poly notShifted = p2; 
    dvSize          = dvSize1;
    dvSize1         = dvSize2;
  }
  else
  { poly hiDeg = p2; poly lowDeg = p1; pLcmDvSize = dvSize2; }

  if(pLcmDvSize != dvSize){ return false; }

  if( compareDVec(dvec, lowDeg, 0, dvSize1, r) &&
      compareDVec(dvec, lowDeg, dvSize1, pLcmDvSize, r) ) 
    return true;

  return false;
#endif
}

/* This function expects two letterplace polynomials and will
 * return true, if p1 and p2 correspond to a valid overlap and
 * their lcm is equal to this objects lcm. p1 is assumed to be
 * not shifted, p2 is assumed to be shifted. The lcm is assumed
 * to be not shifted. All polynomials are assumed to be
 * correctly formatted letterplace polynomials (no gaps, no
 * exponents greater than 1).
 * Examples:
 *  Ex 1 :  x(1)y(2) and y(2)z(3) correspond to a valid overlap 
 *  their lcm is x(1)y(2)z(3)
 *  Ex 2 :  x(1)y(2) and y(3)z(4) do not correspond to a valid 
 *  overlap
 *  Ex 3 :  x(1)y(2)z(3) and y(2)x(3)z(4) do not correspond to 
 *  a valid overlap, though they have indeed gcd != 1 .
 *  Ex 4 :  x(1)y(2)z(3) and y(2)z(3) do not correspond to a 
 *  valid overlap, since we do only enter pairs, such that p2
 *  does not divide p1.
 * This function was somehow inspired by the pCompareChain
 * function. See that function for more info.                 */
bool ShiftDVec::sLObjectExtension::gm3LcmUnEqualToLcm
  ( poly p1, poly p2, int lV, ring r /* = currRing */ )
{
  bool unequal;

  LObject* L = this->get_LObject();

  //Number of variables in our letterplace ring
  int numVars = r->N;

  //p2 is shifted, we will loop, until we discover the first
  //variable not equal to 0.
  int j;
  for (j=1; j <= numVars && !pGetExp(p2, j); ++j)
    unequal = unequal || pGetExp(p1, j) != pGetExp(L->lcm,j);

  //We looped, until pGetExp(p2, j) != 0. From here on, p1 and
  //p2 should be equal, until the end of p1.
  assume(lV < j);
  for (; j <= numVars; ++j)
  {
    unequal = unequal || pGetExp(p2, j) != pGetExp(L->lcm,j);
    if( pGetExp(p1, j) != pGetExp(p2, j) ) break;
  }

  if( j > numVars - lV ) return false;
  //Everything was equal up to the last letterplace variable,
  //this is like in Ex 4, p2 divides p1 - not a valid pair.

  //position right before current pos. in letterplace polynomial
  int lpPos = j / lV;
  /* p1 and p2 were equal up to the previous position in the
   * letterplace ring. Now p1 should be 0, if not, this would
   * either mean, that p2 is 0 and this implies that p2 divides
   * p1, or p1 != p2 at the current letterplace position, which
   * means, that we have something like Ex 3 . Both cases imply,
   * that we have not a valid pair.                           */
  for(j = lpPos * lV + 1; j <= lpPos * (lV + 1); ++j)
    if( pGetExp(p1, j) != 0 ) return false;

  //Now our polynomials correspond to a valid overlap, we can
  //stop, as soon, as we know, that the lcms are unequal

  if(unequal) return true;

  for (; j <= numVars; ++j)
    if( pGetExp(p2, j) != pGetExp(L->lcm,j) ) return true;

  //The lcms were equal
  return false;
}

uint ShiftDVec::sLObjectExtension::lcmDivisibleBy
  ( sTObject * T, int numVars )
{
  SetLcmDvecIfNULL();
  T->SD_Ext()->SetDVecIfNULL();

  return ShiftDVec::divisibleBy
    ( dvec, dvSize,
      T->SD_Ext()->dvec, T->SD_Ext()->dvSize, numVars );
}

uint ShiftDVec::sLObjectExtension::lcmDivisibleBy
  ( sTObject * T, uint minShift, uint maxShift, int numVars )
{
  SetLcmDvecIfNULL();
  T->SD_Ext()->SetDVecIfNULL();

  return ShiftDVec::divisibleBy
    ( dvec, dvSize, T->SD_Ext()->dvec,
      T->SD_Ext()->dvSize, minShift, maxShift, numVars );
}


//other functions, which do not have no counterpart in normal bba


uint ShiftDVec::CreateDVec (poly p, ring r, uint*& dvec)
{
  if(p == NULL){dvec = NULL; return 0;}

  //We should test here, if p is in r. (see sTObject::Set ?)

  //I hope this is the right way to get the total deg. of lm(p)
  uint dvSize = p_Totaldegree(p, r);
  if(dvSize == 0){dvec = NULL; return 0;}
  assume(dvSize < 1000); // this is funny, but often recued me
  if(!dvSize){dvec = NULL; return 0;}
  dvec = (uint *) OMALLOC_0( dvSize, uint );

  uint * it = dvec;

  /* transform lm(p) to shift invariant distance vector
   * j is the distance to the next entry in the block
   * representation of the letterplace monomial lm(p)   
   * "it" is a pointer to the next free entry in our distance
   * vector                                                   
   */
  for(int j=1, i=1, l=0; l < dvSize; ++i)
    if(p_GetExp(p,i,r)){*it=j;++it;j=1;++l;} else{++j;}

  return dvSize;
}

/* This should get the shift of a letterplace polynomial.
 * numFirstVar should be the index of the variable in the first
 * visual block of p (first index should be 1!).
 *
 * TODO:
 * It would be more efficient to store the shifts for all
 * shifted polynomials somehow. p should not be NULL!
 */
uint ShiftDVec::getShift
  ( poly p, uint numFirstVar, ring r )
{
  for( uint i = numFirstVar, sh=0; ; i += r->N, ++sh )
    if( p_GetExp(p, i, r) ) return sh;
}

bool ShiftDVec::compareDVec
  ( const uint* dvec, poly p,
    uint offset, uint maxSize, ring r )
{
  for(uint j = 1, i = 1, l = offset; l < maxSize; ++i)
  {
    if( p_GetExp(p,i,r) )
      if( dvec[l] != j ){ return false; }
      else { j = 1; ++l; }
    else{ ++j; }
  }

  return true;
}

/* Returns the first shift s, where the polynomial corresponding
 * to dvec1, is divisible by the shifted polynomial corresponding
 * to dvec2. If no such shift exists, UINT_MAX is returned. If
 * dvSize2 == 0, UINT_MAX-1 is returned.
 *
 * Has to be tested!
 */
uint ShiftDVec::divisibleBy
  ( const uint* dvec1, uint dvSize1, 
    const uint* dvec2, uint dvSize2, int numVars )
{
  if(dvSize1 < dvSize2) return UINT_MAX;
  if(!dvSize2) return UINT_MAX-1;

  uint * tmpVec = (uint *)OMALLOC_0( dvSize1, uint );
  memcpy((void*)tmpVec,(void*)dvec1,dvSize1*sizeof(uint));

  //overflow can not happen since dvSize1 <= dvSize2
  uint maxSh = dvSize1 - dvSize2;
  size_t cmpLen = dvSize2 * sizeof(uint);
  int shift=0;
  for(; shift < maxSh; ++shift)
  {
    if(!memcmp((void*)(tmpVec+shift),(void*)dvec2,cmpLen))
    {
      OMFREES( tmpVec, dvSize1, uint );
      return shift;
    }
    tmpVec[shift+1] = tmpVec[shift] + tmpVec[shift+1] - numVars;
  }
  if(!memcmp((void*)(tmpVec+shift),(void*)dvec2,cmpLen))
  {
    OMFREES( tmpVec, dvSize1, uint );
    return shift;
  }
  OMFREES( tmpVec, dvSize1, uint );
  return UINT_MAX;
}

/* Returns the first shift s >= minShift, with s <= maxShift
 * where the polynomial corresponding to dvec1, is divisible by 
 * the shifted polynomial corresponding to dvec2. If no such 
 * shift exists, UINT_MAX is returned.  
 *
 * Assumes: 
 * minShift <= maxShift
 * and
 * maxShift + dvSize2 < dvSize1
 * and
 * dvSize2 > 0
 * and
 * dvSize1 > 0 ,
 * but does not test these!
 *
 * TODO:
 * - Has to be tested!
 * - Maybe Code could be commented a bit more.
 */
uint ShiftDVec::divisibleBy
  ( const uint* dvec1, uint dvSize1, 
    const uint* dvec2, uint dvSize2, 
    uint minShift, uint maxShift, int numVars )
{
  assume(dvSize2 > 0);
  assume(dvSize1 > 0);
  assume(minShift <= maxShift);
  assume(maxShift + dvSize2 < dvSize1);

  uint tmpSize = (maxShift - minShift + dvSize2);
  uint * tmpVec = (uint *) OMALLOC_0( tmpSize, uint );

  memcpy
    ((void*)tmpVec,(void*)(dvec1+minShift),tmpSize*sizeof(uint));

  for(int i = 0; i < minShift; ++i) tmpVec[0] += dvec1[i];
  tmpVec[0] -= (numVars * minShift);

  size_t cmpLen = dvSize2 * sizeof(uint);
  for(int shift=0; shift <= maxShift - minShift; ++shift){
    if(!memcmp((void*)(tmpVec+shift),(void*)dvec2,cmpLen)){
      OMFREES( tmpVec, tmpSize, uint );
      return shift;
    }
    tmpVec[shift+1] = tmpVec[shift] + tmpVec[shift+1] - numVars;
  }

  OMFREES( tmpVec, tmpSize, uint );
  return UINT_MAX;
}

/* Like divisibleBy, but with the difference, that it only
 * returns a shift, if dvec1 is divisible by dvec2 from the
 * right.
 *
 * Has to be tested!
 */
uint ShiftDVec::RdivisibleBy
  ( const uint* dvec1, uint dvSize1, 
    const uint* dvec2, uint dvSize2, int numVars )
{
  if(dvSize1 < dvSize2) return UINT_MAX;
  assume(dvSize2);
  if(!dvSize2) return UINT_MAX-1;

  if( dvSize2 == 1 ) if( dvec1[0] == dvec2[0] ) return 0;
                     else                return UINT_MAX;

  do
  {
    --dvSize2;
    --dvSize1;
    if( dvec2[dvSize2] != dvec1[dvSize1] ) return UINT_MAX;
  }while( dvSize2 > 1 );

  uint dist_cum = 0;
  for( uint i = 0; i < dvSize1; ++i ) dist_cum += dvec1[i];
  dist_cum %= numVars;
  if( dist_cum == dvec2[0] ) return dvSize1-1;

  return UINT_MAX;
}

/* Returns the first shift s, where lm(p1) is divided by the 
 * shifted monomial s*lm(p2) (,that is the dvec of lm(p1) has a
 * central overlap with the dvec of lm(p2)). If no such shift
 * exists, UINT_MAX is returned. If lm(p2) is a Field Element,
 * UINT_MAX-1 is returned (In this case the dvec of p2 should be 
 * a NULL-Vector). Sets the dvecs, if they are NULL.
 *
 * Has to be tested!
 */
uint ShiftDVec::divisibleBy
  ( sTObject * p1, sTObject * p2, int numVars )
{
  p1->SD_Ext()->SetDVecIfNULL();
  p2->SD_Ext()->SetDVecIfNULL();

  return divisibleBy
    ( p1->SD_Ext()->dvec, p1->SD_Ext()->dvSize,
      p2->SD_Ext()->dvec, p2->SD_Ext()->dvSize, numVars );
}

/* Tests if the LObject's polynomial lcm is divisible by a
 * shift s of TObject's polynomial p. Returns this shift s.
 * Sets the dvecs, if they are NULL.
 *
 * Has to be tested!
 */
uint ShiftDVec::lcmDivisibleBy
  ( LObject * lcm, sTObject * p, int numVars )
{
  lcm->SD_LExt()->SetLcmDvecIfNULL();
  p->SD_Ext()->SetDVecIfNULL();

  return divisibleBy( lcm->SD_LExt()->GetDVec(),
                      lcm->SD_LExt()->getLcmDVSize(),
                      p->SD_Ext()->dvec,
                      p->SD_Ext()->dvSize, numVars    );
}

/* Returns true, if reduction of poly a with poly b would
 * violate the the degree bound. (to be more exact: We test, if
 * the total degree of b*lm(a)/lm(b) is greater than uptodeg.)
 * Will always return false, if uptodeg == 0.
 * WARNING/TODO:
 * - Recently changed! Has to be tested again. Is it right, how
 *   we handle Rings?
 * - This is not very perfect; Even if redViolatesDeg returns
 *   true, the polynomial after reduction may not violate the
 *   degree bound.
 */
BOOLEAN ShiftDVec::redViolatesDeg
  ( poly a, poly b, int uptodeg,
    ring aLmRing, ring bLmRing, ring bTailRing )
{
  if(!uptodeg || !b) return FALSE;
  //or should we return true, if !b ?
  
  int tg_lm_a = p_Totaldegree(a, aLmRing);
  int tg_lm_b = p_Totaldegree(b, bLmRing);

  int tg_b    = tg_lm_b;
  while(pIter(b))
  {
    int tg_lm_itb = p_Totaldegree(b, bTailRing);
    tg_b = tg_b > tg_lm_itb ? tg_b : tg_lm_itb;
  }

  return tg_b + tg_lm_a - tg_lm_b > uptodeg;
}

/* Returns true, if reduction of poly a with poly b would
 * violate the the degree bound. (to be more exact: We test, if
 * the total degree of b*lm(a)/lm(b) is greater than uptodeg.)
 * Will always return false, if uptodeg == 0.
 * WARNING/TODO:
 * - Recently changed! Has to be tested again. Is it right, how
 *   we handle Rings?
 * - This is not very perfect; Even if redViolatesDeg returns
 *   true, the polynomial after reduction may not violate the
 *   degree bound.
 */
BOOLEAN ShiftDVec::redViolatesDeg
  ( TObject* a, TObject* b, int uptodeg, ring lmRing )
{
  int tg_a, tg_b;
  poly it_b;
  ring r_a, r_b;

  if( a->p == NULL )
  {
    if( a->t_p == NULL ) return FALSE;
    tg_a = p_Totaldegree(a->t_p, a->tailRing);
  }
  else
    tg_a = p_Totaldegree(a->p, lmRing);

  if( b->p == NULL)
  {
    if( b->t_p == NULL ) return FALSE;
    tg_b = p_Totaldegree(b->t_p, b->tailRing);
    it_b = b->t_p;
    r_b = b->tailRing;
  }
  else
  {
    tg_b = p_Totaldegree(b->p, lmRing);
    it_b = b->p;
    r_b = b->t_p == NULL ? lmRing : b->tailRing;
  }

  while(pIter(it_b))
  {
    int tg_lm_it_b = p_Totaldegree(it_b, r_b);
    tg_b = tg_b > tg_lm_it_b ? tg_b : tg_lm_it_b;
  }

  return tg_b + tg_a - tg_b > uptodeg;
}

/** Test for Degree Bound Violation before entering Pair
 *
 *  Returns true, if the creation of the s-poly of poly a and
 *  poly b would violate the degree bound, false otherwise.
 *  shift has to be the shift of a, such that b and shift a
 *  have overlap, example with shift = 3 (x^2yz and y^3x^2)
 *     xxyz | poly a
 *  yyyxx   | poly b
 *  Will always return false, if uptodeg == 0.
 *  WARNING/TODO:
 *  Recently changed! Has to be tested again. Is it right, how we
 *  handle Rings?
 *
 *  \todo doxygen explaination of parameters
 */
bool ShiftDVec::createSPviolatesDeg
  ( poly a, poly b, uint shifta, int uptodeg,
    ring aLmRing, ring bLmRing, ring aTailRing, ring bTailRing )
{
  if(!uptodeg) return false;

  int tg_lm_b = p_Totaldegree(b, bLmRing);
  uint degOverlap = tg_lm_b - shifta + 1;

  poly it_a = pNext(a); //TODO: Does that work with Singular?

  while(it_a)
  {
    int tg_lm_it_a = p_Totaldegree(it_a, aTailRing);
    if(tg_lm_b + tg_lm_it_a - degOverlap > uptodeg)return true;
    pIter(it_a);
  }

  int tg_lm_a = p_Totaldegree(a, aLmRing);

  poly it_b = pNext(b); //TODO: Does that work with Singular?

  while(it_b)
  {
    int tg_lm_it_b = p_Totaldegree(it_b, bTailRing);
    if(tg_lm_a + tg_lm_it_b - degOverlap > uptodeg)return true;
    pIter(it_b);
  }

  return false;
}

bool ShiftDVec::shiftViolatesDeg
  (poly p, uint shift, int uptodeg, ring pLmRing, ring pTailRing)
{
  assume(p != NULL);
  int tg_lm_p = p_Totaldegree(p, pLmRing);
  loop
  {
    if(tg_lm_p + shift > uptodeg) return true;
    if( pIter(p) == NULL ) break;
    tg_lm_p = p_Totaldegree(p, pTailRing);
  }

  return false;
}

/** return a copy of a (letterplace) monomial in another ring
 *
 *  the number of exponents copied (equal to zero/one) is
 *  determined by nexp, which defaults to the largest possible,
 *  if negative.
 *
 *  copies lm(p1) while switching from r1 to r2 to lm(p2)
 *  p2 should be initialized correctly
 *  ( e.g. by setting p2 = p_init(r2) )
 *
 *  \remarks
 *    - It's not very efficient and p_Setm has to be invoked on
 *      the output. ( I could have done it slightly more
 *      efficient, but I liked to do it like this... . I
 *      shouldn't do that... .)
 *    - The input polynomial has to be a letterplace polynomial
 *      without gaps.
 *    - returns the unweighted Totaldegree of the polynomial
 *    - We could just return i/lV-1, as long as our ring has at
 *      least two variables ;)
 *
 *  \todo test this function
 */
long ShiftDVec::p_lmCopy( poly p1, ring r1,
                          poly p2, ring r2,
                          int lV, long nexp )
{
  long i = 1;
  long maxi = nexp > 0 ? nexp : r1->N;
  do{
    // We will loop, until we found an empty block, or until we
    // considered all variables
    do{
      if( p_GetExp(p1, i, r1) )     // found a nonzero exponent
      {                             // in this block, thus ...
        p_SetExp(p2, i, 1, r2);
        i = i + (lV - (i-1) % lV);  // ... we can move
        goto nextblock;             // to the next block
      }
    }while( ++i % lV != 1 );        // found a block of zeros, so
    return (i-1)/lV - 1;            // we may return (end of p1)
    nextblock: ;
  }while(i <= maxi);

  return maxi/lV;
}

/** copy p2 into p1 beginning after offset o1 in p1 and o2 in p2
 *
 *  If this sounds confusing: A part of the exponent of p2 will
 *  be copied into the exponet of p1 starting at exp[o1+1] in p1.
 *  The part to be copied is everything in p2->exp starting from
 *  o2+1. If this sound still confusing: The reason is probable,
 *  that I drank lots of tea (Viktor allways makes very tasty
 *  green tea - I luve it :) ).
 *  \remarks
 *    - This function was just created, when I somewhere was in
 *      need of it. I love the symmetrie in it, which, funnily,
 *      was a complete and utter coincidence
 *    - returns the unweighted Totaldegree of the resulting
 *      polynomial
 *    - possibly this could be changed to be slightly more
 *      efficient
 *    - nexp ??? completely unused !!!
 *
 *  \todo test this function
 */
long ShiftDVec::p_lmAppend( poly p1, long o1, ring r1,
                            poly p2, long o2, ring r2,
                            int lV,  long nexp )
{
  long i = 1;
  long maxi = o2 > o1 ? r1->N - o2 : r1->N - o1;  // maxi :)
  do{
    do{
      // We will loop, until we found an empty block, or until we
      // considered all variables
      if( p_GetExp(p2, o2+i, r2) )  // found a nonzero exponent
      {                             // in this block, thus ...
        p_SetExp(p1, o1+i, 1, r1);
        i = i + (lV - (i-1) % lV);  // ... we can move
        goto nextblock;             // to the next block
      }
    }while( ++i % lV != 1 );        // found a block of zeros, so
    return (o1+i-1)/lV - 1;         // we may return (end of p2)
    nextblock: ;
  }while( i <= maxi );

  return maxi/lV;
}

// vim: set foldmethod=syntax :
// vim: set textwidth=65 :
// vim: set colorcolumn=+1 :
