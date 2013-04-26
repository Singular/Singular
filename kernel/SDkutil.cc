/* file:        SDkutil.cc
 * authors:     Grischa Studzinski & Benjamin Schnitzler
 * created:     ask git
 * last change: ask git
 *
 * TODO:
 * a lot (see SDkutil.h)
 * - loop
 *     search for (semantic and syntax) bugs
 *   until all bugs are cleared
 * (unfortunatly the above statement does not compile)
 */

#include <kernel/kutil.h>
//#include <kernel/SDkutil.h>//do this via kutil.h otherwise...
#include <kernel/SDDebug.h>

#include <climits>

//Needed because of include order; should rather be included in
//ring.h
#include <kernel/mod2.h>

#include <polys/monomials/p_polys.h> //For p_Totaldegree and the like
#include <kernel/polys.h> //For pTotaldegree and the like
#include <kernel/febase.h> //For Print stuff

typedef skStrategy* kStrategy;



//member functions of ShiftDVec::sTobject



uint ShiftDVec::CreateDVec
  (poly p, ring r, uint*& dvec)
{
  initDeBoGri
    ( SD::indent, 
      "Entering CreateDVec", "Leaving CreateDVec", 512 );
  deBoGriPrint(++SD::debugCounter, "debugCounter: ", 512);

  if(p == NULL){dvec = NULL; return 0;}

  //We should test here, if p is in r. (see sTObject::Set ?)

  //I hope this is the right way to get the total deg. of lm(p)
  uint dvSize = p_Totaldegree(p, r);
  assume(dvSize < 1000);
  if(!dvSize){dvec = NULL; return 0;}
  dvec = (uint *)omAlloc0(dvSize*sizeof(uint));
  loGriToFile("omAlloc0 for dvec in CreateDVec ",dvSize*sizeof(uint),1024, (void*)dvec);

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


uint* ShiftDVec::sTObject::GetDVec()
{
  return dvec;
}


uint ShiftDVec::sTObject::GetDVsize()
{
  if(!dvec) SetDVec();
  if(!dvec) return 0; //Be careful! p does not exist!
  return dvSize;
}


int ShiftDVec::sTObject::cmpDVec(SD::sTObject* T)
{
  if(GetDVsize() == T->GetDVsize())
    return memcmp(dvec,T->dvec,dvSize*sizeof(uint));
  return -1;
}

/* Compares part of this object's DVec, beginning at begin, to 
 * part of T1's DVec, beginning at beginT1. Comparison ends
 * after comparing size entries. This function does not check, 
 * if begin + size, respectivly beginT1 + size is larger than
 * this object's DVec size, respectivly the size of T1's DVec.
 * Returns 0 if these parts match, or non-zero value if not
 * (see memcmp for more informations).                        */
int ShiftDVec::sTObject::cmpDVec
  (SD::sTObject* T1, uint begin, uint beginT1, uint size)
{
  assume(begin + size <= dvSize && beginT1 + size <= T1->dvSize);
  initDeBoGri
    ( SD::indent, 
      "Entering cmpDVec", "Leaving cmpDVec", 128 );
  deBoGriPrint(begin + size, "begin + size: ", 128);
  deBoGriPrint(dvSize, "dvSize: ", 128);
  deBoGriPrint(beginT1 + size, "beginT1 + size: ", 128);
  deBoGriPrint(T1->dvSize, "T1->dvSize: ", 128);

    return 
      memcmp( dvec+begin, T1->dvec+beginT1, size*sizeof(uint) );
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
  ( SD::sTObject* T1, uint beginT1,
    SD::sTObject* T2, uint beginT2, uint size, int lV )
{
  assume(beginT2+size<=T2->dvSize && beginT1+size<=T1->dvSize);

  return
    cmpDVecProper
      ( T1->dvec, beginT1, T2->dvec, beginT2, size, lV );
}

/* Does what cmpDVec should do. size > 0 should hold.*/
int ShiftDVec::sTObject::cmpDVecProper
  ( SD::sTObject* T1, 
    uint begin, uint beginT1, uint size, int lV )
{
  return 
    SD::cmpDVecProper
      (T1, beginT1, this, begin, size, lV);
}

void ShiftDVec::sTObject::freeDVec()
{
  if(dvec){ 
    loGriToFile("omFreeSize of (ADDRESS)dvec in freeDVec ", sizeof(uint)*dvSize, 1024, (void*)dvec ); 
    omFreeSize((ADDRESS)dvec, sizeof(uint)*dvSize); 
    dvec = NULL;
    dvSize = 0;
  }
}

ShiftDVec::sTObject&
ShiftDVec::sTObject::operator=(const SD::sTObject& t)
{
  this->::sTObject::operator=(t);

  //dvec = NULL; //or should we copy, or deep copy??? - only user knows
  //dvSize = 0;
  //Be careful: we copy the dvec shallow!
  dvec = t.dvec;
  dvSize = t.dvSize;
}



//member functions of SD::sLobject



//Get the size of the lcms dvec. Initialize it, if not set - the
//latter requires p1 and p2 to be set and shifted correctly.
uint ShiftDVec::sLObject::getLcmDVSize(ring r)
{
  assume( p1 != NULL && p2 != NULL );

  if( !lcmDvSize ) {
    //I hope, this will get me the letterplace degree of lm(p1) 
    //(totaldeg(x(4)*y(5)*z(6)) for example should be 6).
    //BOCO: IMPORTANT: TODO: This was a wrong assuption - maybe
    //use getShift somehow.
    int degP1 = p_Totaldegree(p1, r);
    int degP2 = p_Totaldegree(p2, r);
    lcmDvSize = degP1 > degP2 ? degP1 : degP2; 
  }

  return lcmDvSize;
}


void ShiftDVec::sLObject::SetLcmDVec(ring r)
{
  initDeBoGri
    ( SD::indent, 
      "Entering SetLcmDVec", "Leaving SetLcmDVec", 16 );

#ifdef USE_DVEC_LCM
  //We want to create the lcm directly from p1, p2 .
  //This is an adapted version of the CreateDVec function.
  //TODO: This has to be testet!
  deBoGriPrint("USE_DVEC_LCM is set.", 16);
  deBoGriPrint
    ("Using p1, p2 to create the lcm's DVec directly.", 16);
  
  assume(p1 != NULL && p2 != NULL);

  deBoGriPrint(p1, "p1 is: ", 16);
  deBoGriPrint(p2, "p2 is: ", 16);

  //I hope, this will get me the letterplace degree of lm(p1) 
  //(totaldeg(x(4)*y(5)*z(6)) for example should be 6).
  //BOCO: IMPORTANT: TODO: This was a wrong assuption - maybe
  //use getShift somehow.
  uint dvSize1 = p_Totaldegree(p1, r);
  uint dvSize2 = p_Totaldegree(p2, r);

  //One of the polys should be shifted in letterplace case. This
  //one overlaps the other one.
  poly shifted; poly notShifted;
  if(dvSize1 > dvSize2)
    {poly shifted=p1; poly notShifted=p2; 
      lcmDvSize=dvSize1; dvSize1 = dvSize2;}
  else
    {poly shifted=p2; poly notShifted=p1; lcmDvSize=dvSize2;}

  assume(lcmDvSize > 0);

 loGriToFile("omAlloc0 for lcmDvec in SetLcmDVec ",lcmDvSize*sizeof(uint),1024);
  lcmDvec = (uint *)omAlloc0(lcmDvSize*sizeof(uint));

  uint * it = lcmDvec;

  /* The creation of the shift invariant distance vector is
   * similar to its creation in CreateDVec; we will begin with
   * the not shifted poly. After we have created it's dvec we
   * will continue to finish the lcmDvec by beginning at the
   * right postion of the second polynomial.                   */
  int j = 1, i = 1, l = 0;
  for(;l < dvSize1; ++i)
    if(p_GetExp(notShifted,i,r)){*it=j;++it;j=1;++l;} else{++j;}
  for(;l < lcmDvSize; ++i)
    if(p_GetExp(shifted,i,r)){*it=j;++it;j=1;++l;} else{++j;}
#else
  deBoGriPrint(lcm, "The lcm is: ", 16);
  SetLcmDVec(lcm, r);
#endif
}


void ShiftDVec::sLObject::freeLcmDVec()
{
  if(lcmDVec){ 
    loGriToFile("omFreeSize for (ADDRESS)lcmDvec in freeLcmDVec",sizeof(uint)*dvSize,1024);
    omFreeSize((ADDRESS)lcmDVec, sizeof(uint)*dvSize); 
    dvec = NULL;
    dvSize = 0;
    lcmDVec = NULL;
  }
}


/* returns true, if the DVec of this objects lcm is equal to the
 * DVec of the other objects lcm. Thus, if both lcm do not have
 * the same shift, but would otherwise be equal, true will be
 * returned, too.  In every other case, false is returned.    */
bool ShiftDVec::sLObject::compareLcmTo( SD::sLObject* other, ring r )
{
  if( this->getLcmDVSize(r) != other->getLcmDVSize(r) ) 
    return false;

  assume(this->p1 != NULL && this->p2 != NULL);
  assume(other->p1 != NULL && other->p2 != NULL);

  //now we need to create the dvecs
  if(!this->lcmDVec) this->SetLcmDVec();
  if(!other->lcmDVec) other->SetLcmDVec();

  if(memcmp((void*)(lcmDVec),(void*)other->lcmDVec,lcmDvSize))
    return false;

  return true;
}


/* This function returns true, if the objects lcm is equal to
 * a shift of lcm(p1, p2).                                    */
bool ShiftDVec::sLObject::compareLcmTo(poly p1, poly p2, ring r)
{
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
    {poly shifted=p1; poly notShifted=p2; 
      lcmDvSize=dvSize1; dvSize1 = dvSize2;}
  else
    {poly hiDeg=p2; poly lowDeg=p1; pLcmDvSize=dvSize2;}

  if(pLcmDvSize != lcmDvSize){return false;}

  if( compareDVec(lcmDVec, lowDeg, 0, dvSize1, r) &&
      compareDVec(lcmDVec, lowDeg, dvSize1, pLcmDvSize, r) ) 
    return true;

  return false;
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
bool ShiftDVec::sLObject::gm3LcmUnEqualToLcm
  (poly p1, poly p2, int lV, ring r /* = currRing */ )
{
  bool unequal;

  //Number of variables in our letterplace ring
  int numVars = r->N;

  //p2 is shifted, we will loop, until we discover the first
  //variable not equal to 0.
  int j;
  for (j=1; j <= numVars && !pGetExp(p2, j); ++j)
    unequal = unequal || pGetExp(p1, j) != pGetExp(this->lcm,j);

  //We looped, until pGetExp(p2, j) != 0. From here on, p1 and
  //p2 should be equal, until the end of p1.
  assume(lV < j);
  for (; j <= numVars; ++j){
    unequal = unequal || pGetExp(p2, j) != pGetExp(this->lcm,j);
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
    if( pGetExp(p2, j) != pGetExp(this->lcm,j) ) return true;

  //The lcms were equal
  return false;
}

uint ShiftDVec::sLObject::lcmDivisibleBy
  ( SD::sTObject * T, int numVars )
{
  initDeBoGri
    ( SD::indent, 
      "Entering lcmDivisibleBy", "Leaving lcmDivisibleBy", 16 );

  this->SetLcmDvecIfNULL();
  T->SetDVecIfNULL();

  deBoGriPrint(this->lcm, "this->lcm: ", 16);
  deBoGriPrint
    (this->lcmDVec, this->lcmDvSize, "this->lcmDvec: ", 16);
  deBoGriPrint(this->lcmDvSize, "this->lcmDvSize: ", 16);
  deBoGriPrint(T->p, "T->p: ", 16);
  deBoGriPrint(T->dvec, T->dvSize, "T->dvec: ", 16);
  deBoGriPrint(T->dvSize, "T->dvSize: ", 16);

  return ShiftDVec::divisibleBy
    ( lcmDVec, lcmDvSize, T->dvec, T->dvSize, numVars );
}

uint ShiftDVec::sLObject::lcmDivisibleBy
  ( SD::sTObject * T, 
    uint minShift, uint maxShift, int numVars )
{
  initDeBoGri
    ( SD::indent, 
      "Entering lcmDivisibleBy(sTObject, int, uint, uint, int)", 
      "Leaving lcmDivisibleBy", 16                              );

  this->SetLcmDvecIfNULL();
  T->SetDVecIfNULL();

  deBoGriPrint(this->lcm, "this->lcm: ", 16);
  deBoGriPrint
    (this->lcmDVec, this->lcmDvSize, "this->lcmDvec: ", 16);
  deBoGriPrint(this->lcmDvSize, "this->lcmDvSize: ", 16);
  deBoGriPrint(T->p, "T->p: ", 16);
  deBoGriPrint(T->dvec, T->dvSize, "T->dvec: ", 16);
  deBoGriPrint(T->dvSize, "T->dvSize: ", 16);

  return ShiftDVec::divisibleBy
    ( lcmDVec, lcmDvSize, 
      T->dvec, T->dvSize, minShift, maxShift, numVars );
}



//member functions of SD::skStrategy



ShiftDVec::TObject* ShiftDVec::skStrategy::s_2_t(int i)
{
  if (i >= 0 && i <= sl)
  {
    int sri= S_2_R[i];
    if ((sri >= 0) && (sri <= tl))
    {
      SD::TObject* t = R[sri];
      if ((t != NULL) && (t->p == S[i]))
        return t;
    }
    // last but not least, try kFindInT
    sri = kFindInT(S[i], T, tl);
    if (sri >= 0)
      return &(T[sri]);
  }
  return NULL;
}

ShiftDVec::TObject* ShiftDVec::skStrategy::S_2_T(int i)
{
  assume(i>= 0 && i<=sl);
  assume(S_2_R[i] >= 0 && S_2_R[i] <= tl);
  SD::TObject* TT = R[S_2_R[i]];
  assume(TT != NULL && TT->p == S[i]);
  return TT;
}

ShiftDVec::sLObject&
ShiftDVec::sLObject::operator=(const SD::sTObject& t)
{
  memset(this, 0, sizeof(*this));
  memcpy(this, &t, sizeof(SD::sTObject));
  return *this;
}

void ShiftDVec::skStrategy::initT()
{
  T = (SD::TSet)omAlloc0(setmaxT*sizeof(SD::TObject));
  for (int i=setmaxT-1; i>=0; i--)
  {
    T[i].tailRing = currRing;
    T[i].i_r = -1;
  }
}

void ShiftDVec::skStrategy::initR()
{
  R = (SD::TObject**)omAlloc0(setmaxT*sizeof(SD::TObject*));
}


//other functions, which do not have no counterpart in normal bba



//This should get the shift of a letterplace polynomial.
//numFirstVar should be the index of the variable in the first
//visual block of p (first index should be 1!). TODO: It would
//be more efficient to store the shifts for all shifted
//polynomials somehow. p should not be NULL!
uint ShiftDVec::getShift
  ( poly p, uint numFirstVar, ring r )
{
  for( uint i = numFirstVar, sh=0; ; i += r->N, ++sh )
    if( p_GetExp(p, i, r) ) return sh;
}


bool ShiftDVec::compareDVec
  ( const uint* dvec, poly p, uint offset, uint maxSize, ring r )
{

#if DEBOGRI > 0 
if( deBoGri == 4 )
{
  PrintS("\nIn compareDVec\nComparing\n");
  dvecWrite(dvec, maxSize);
  PrintS("\nWith\n");
  pWrite(p);
  PrintS("Which has dvec:\n");
  dvecWrite(p);
  Print("\nUp to size %d\n", maxSize);
}
#endif

  for(uint j = 1, i = 1, l = offset; l < maxSize; ++i)
  {
    if( p_GetExp(p,i,r) )
      if( dvec[l] != j ){ return false; }
      else { j = 1; ++l; }
    else{ ++j; }
  }

#if DEBOGRI > 0
if( deBoGri == 4 )
  PrintS("compareDVec returns true!\n");
#endif

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
  initDeBoGri
    ( SD::indent, 
      "Entering divisibleBy", "Leaving divisibleBy", 256 );
  deBoGriPrint(dvec1, dvSize1, "dvec1: ", 256);
  deBoGriPrint(dvec2, dvSize2, "dvec2: ", 256);
  //assume(dvSize2 > 0);
  //assume(dvSize1 > 0);
  if(dvSize1 < dvSize2) return UINT_MAX;
  if(!dvSize2) return UINT_MAX-1;

  loGriToFile("omAlloc0 for tmpVec in divBy ",dvSize1*sizeof(uint),1024);
  uint * tmpVec = (uint *)omAlloc0(dvSize1*sizeof(uint));
  memcpy((void*)tmpVec,(void*)dvec1,dvSize1*sizeof(uint));

  //overflow can not happen since dvSize1 <= dvSize2
  uint maxSh = dvSize1 - dvSize2;
  size_t cmpLen = dvSize2 * sizeof(uint);
  int shift=0;
  for(; shift < maxSh; ++shift)
  {
    if(!memcmp((void*)(tmpVec+shift),(void*)dvec2,cmpLen))
    {
      loGriToFile("omFreeSize for (ADDRESS)tmpVec in divBy ",dvSize1*sizeof(uint),1024);
      omFreeSize((ADDRESS)tmpVec, sizeof(uint)*dvSize1);
      return shift;
    }
    tmpVec[shift+1] = tmpVec[shift] + tmpVec[shift+1] - numVars;
  }
  if(!memcmp((void*)(tmpVec+shift),(void*)dvec2,cmpLen))
  {
    loGriToFile("omFreeSize for (ADDRESS)tmpVec in divBy ",dvSize1*sizeof(uint),1024);
    omFreeSize((ADDRESS)tmpVec, sizeof(uint)*dvSize1);
    return shift;
  }
  loGriToFile("omFreeSize for (ADDRESS)tmpVec in divBy ",dvSize1*sizeof(uint),1024);
  omFreeSize((ADDRESS)tmpVec, sizeof(uint)*dvSize1);
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
 loGriToFile("omAlloc0 in divBy for tmpVec ",tmpSize*sizeof(uint),1024);
  uint * tmpVec = (uint *) omAlloc0( tmpSize*sizeof(uint) );

  memcpy
    ((void*)tmpVec,(void*)(dvec1+minShift),tmpSize*sizeof(uint));

  for(int i = 0; i < minShift; ++i) tmpVec[0] += dvec1[i];
  tmpVec[0] -= (numVars * minShift);

  size_t cmpLen = dvSize2 * sizeof(uint);
  for(int shift=0; shift <= maxShift - minShift; ++shift){
    if(!memcmp((void*)(tmpVec+shift),(void*)dvec2,cmpLen)){
      loGriToFile("omFreeSize for (ADDRESS)tmpVec in divBy ",tmpSize*sizeof(uint),1024);
      omFreeSize((ADDRESS)tmpVec, sizeof(uint)*tmpSize);
      return shift;
    }
    tmpVec[shift+1] = tmpVec[shift] + tmpVec[shift+1] - numVars;
  }

  loGriToFile("omFreeSize for (ADDRESS)tmpVec in divBy ",tmpSize*sizeof(uint),1024);
  omFreeSize((ADDRESS)tmpVec, sizeof(uint)*tmpSize);
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
  ( SD::sTObject * p1, 
    SD::sTObject * p2, int numVars )
{
  p1->SetDVecIfNULL();
  p2->SetDVecIfNULL();

  return divisibleBy
    (p1->dvec, p1->dvSize, p2->dvec, p2->dvSize, numVars );
}


#if 0 //in progress
/* Stores the first shift s greater than s0 into s0, where
 * lm(p1) is divided by the shifted monomial s*lm(p2) (,that is
 * the dvec of lm(p1) has a central overlap with the dvec of 
 * lm(p2)). If no such shift exists, false is returned,
 * otherwise true.
 *
 * Has to be tested!
 */
bool ShiftDVec::divisibleBy
  ( SD::sTObject * p1, 
    SD::sTObject * p2, int& s0, int numVars )
{
  p1->SetDVecIfNULL();
  p2->SetDVecIfNULL();

  return divisibleBy
    (p1->dvec, p1->dvSize, p2->dvec, p2->dvSize, numVars );
}
#endif


/* Tests if the LObject's polynomial lcm is divisible by a
 * shift s of TObject's polynomial p. Returns this shift s.
 * Sets the dvecs, if they are NULL.
 *
 * Has to be tested!
 */
uint ShiftDVec::lcmDivisibleBy
  ( LObject * lcm, SD::sTObject * p, 
    int numVars                             )
{
  initDeBoGri
    ( SD::indent, 
      "Entering lcmDivisibleBy", "Leaving lcmDivisibleBy", 16 );
  lcm->SetLcmDvecIfNULL();
  p->SetDVecIfNULL();

  deBoGriPrint(lcm->dvec, lcm->dvSize, "lcm->dvec: ", 16);
  deBoGriPrint(lcm->dvSize, "lcm->lcmdvSize: ", 16);
  deBoGriPrint(p->dvec, p->dvSize, "p->dvec: ", 16);
  deBoGriPrint(p->dvSize, "p->dvSize: ", 16);

  return divisibleBy
    ( lcm->getLcmDVec(),
      lcm->getLcmDVSize(), p->dvec, p->dvSize, numVars );
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
  initDeBoGri
    ( SD::indent, 
      "Entering redViolatesDeg.", "Leaving redViolatesDeg.", 1 );
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

  deBoGriPrint
    ( "Reduction viol. deg. bound.", 1, 
      !(tg_b + tg_lm_a - tg_lm_b > uptodeg) );
  deBoGriPrint
    ( a, "poly a: ", 1, 
      !(tg_b + tg_lm_a - tg_lm_b > uptodeg) );
  deBoGriPrint
    ( b, "poly b: ", 1, 
      !(tg_b + tg_lm_a - tg_lm_b > uptodeg) );

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
  ( SD::TObject* a, SD::TObject* b, int uptodeg, ring lmRing )
{
  initDeBoGri
    ( SD::indent, 
      "Entering redViolatesDeg v2.", 
      "Leaving redViolatesDeg v2.", 1 );

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


/* Returns true, if the creation of the s-poly of poly a and
 * poly b would violate the degree bound, false otherwise.
 * shift has to be the shift of a, such that b and shift a
 * have overlap, example with shift = 3 (x^2yz and y^3x^2)
 *    xxyz | poly a
 * yyyxx   | poly b
 * Will always return false, if uptodeg == 0.
 * WARNING/TODO:
 * Recently changed! Has to be tested again. Is it right, how we
 * handle Rings?
 */
bool ShiftDVec::createSPviolatesDeg
  ( poly a, poly b, uint shifta, int uptodeg,
    ring aLmRing, ring bLmRing, ring aTailRing, ring bTailRing )
{
  initDeBoGri
    ( SD::indent, 
      "Entering createSPviolatesDeg", 
      "Leaving createSPviolatesDeg", 1 );
  if(!uptodeg) return false;

  int tg_lm_b = p_Totaldegree(b, bLmRing);
  uint degOverlap = tg_lm_b - shifta + 1;

#if 0 //replaced
  poly it_a = a;
#else //replacement
  poly it_a = pNext(a); //TODO: Does that work with Singular?
#endif

  while(it_a){
    int tg_lm_it_a = p_Totaldegree(it_a, aTailRing);
    if(tg_lm_b + tg_lm_it_a - degOverlap > uptodeg) 
    {
      deBoGriPrint("Creation of s-poly violates deg. bound.", 1);
      deBoGriPrint(a, "poly a: ", 1);
      deBoGriPrint(shifta, "Shift for poly a: ", 1);
      deBoGriPrint(b, "poly b: ", 1);
      deBoGriPrint(uptodeg, "Uptodeg: ", 1);
      deBoGriPrint(tg_lm_b, "Totaldeg of lm(b): ", 1);
      deBoGriPrint(degOverlap, "Degree of overlap: ", 1);
      deBoGriPrint(tg_lm_it_a, "Degree of lm(it_a): ", 1);
      return true;
    }
    pIter(it_a);
  }

  int tg_lm_a = p_Totaldegree(a, aLmRing);

#if 0 //replaced
  poly it_b = b;
#else //replacement
  poly it_b = pNext(b); //TODO: Does that work with Singular?
#endif

  while(it_b){
    int tg_lm_it_b = p_Totaldegree(it_b, bTailRing);
    if(tg_lm_a + tg_lm_it_b - degOverlap > uptodeg)
    {
      deBoGriPrint("Creation of s-poly violates deg. bound.", 1);
      deBoGriPrint(a, "poly a: ", 1);
      deBoGriPrint(shifta, "Shift for poly a: ", 1);
      deBoGriPrint(b, "poly b: ", 1);
      deBoGriPrint(uptodeg, "Uptodeg: ", 1);
      deBoGriPrint(tg_lm_a, "Totaldeg of lm(a): ", 1);
      deBoGriPrint(degOverlap, "Degree of overlap: ", 1);
      deBoGriPrint(tg_lm_it_b, "Degree of lm(it_b): ", 1);
      return true;
    }
    pIter(it_b);
  }

  return false;
}


bool ShiftDVec::shiftViolatesDeg
  (poly p, uint shift, int uptodeg, ring pLmRing, ring pTailRing)
{
  assume(p != NULL);
  int tg_lm_p = p_Totaldegree(p, pLmRing);
  loop{
    if(tg_lm_p + shift > uptodeg) return true;
    if( pIter(p) == NULL ) break;
    tg_lm_p = p_Totaldegree(p, pTailRing);
  }

  return false;
}

/* vim: set foldmethod=syntax : */
