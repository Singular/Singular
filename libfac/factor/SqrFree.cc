/* Copyright 1996 Michael Messollen. All rights reserved. */
///////////////////////////////////////////////////////////////////////////////
// emacs edit mode for this file is -*- C++ -*-
static char * rcsid = "$Id: SqrFree.cc,v 1.9 2006-04-28 13:46:29 Singular Exp $";
static char * errmsg = "\nYou found a bug!\nPlease inform (Michael Messollen) michael@math.uni-sb.de .\n Please include above information and your input (the ideal/polynomial and characteristic) in your bug-report.\nThank you.";
///////////////////////////////////////////////////////////////////////////////
// FACTORY - Includes
#include<factory.h>
#ifndef NOSTREAMIO
#include <iostream.h>
#endif
// Factor - Includes
#include "tmpl_inst.h"
#include "helpstuff.h"
// some CC's need this:
#include "SqrFree.h"

#ifdef SINGULAR
#define HAVE_SINGULAR_ERROR
#endif

#ifdef HAVE_SINGULAR_ERROR
   extern "C" { void WerrorS(char *); }
#endif

#ifdef SQRFREEDEBUG
# define DEBUGOUTPUT
#else
# undef DEBUGOUTPUT
#endif

#include "debug.h"
#include "timing.h"
TIMING_DEFINE_PRINT(squarefree_time);
TIMING_DEFINE_PRINT(gcd_time);

static inline CFFactor
Powerup( const CFFactor & F , int exp=1){
  return CFFactor(F.factor(), exp*F.exp()) ;
}

static CFFList
Powerup( const CFFList & Inputlist , int exp=1 ){
  CFFList Outputlist;

  for ( CFFListIterator i=Inputlist; i.hasItem(); i++ )
    Outputlist.append(Powerup(i.getItem(), exp));
  return Outputlist ;
}

///////////////////////////////////////////////////////////////
// Compute the Pth root of a polynomial in characteristic p  //
// f must be a polynomial which we can take the Pth root of. //
// Domain is q=p^m , f a uni/multivariate polynomial         //
///////////////////////////////////////////////////////////////
static CanonicalForm
PthRoot( const CanonicalForm & f ){
  CanonicalForm RES, R = f;
  int n= max(level(R),getNumVars(R)), p= getCharacteristic();

  if (n==0){ // constant
    if (R.inExtension()) // not in prime field; f over |F(q=p^k)
    {
      R = power(R,Powerup(p,getGFDegree() - 1)) ;
    }  
    // if f in prime field, do nothing
    return R;
  }
  // we assume R is a Pth power here
  RES = R.genZero();
  Variable x(n);
  for (int i=0; i<= (int) (degree(R,level(R))/p) ; i++)
    RES += PthRoot( R[i*p] ) * power(x,i);
  return RES;
}

///////////////////////////////////////////////////////////////
// Compute the Pth root of a polynomial in characteristic p  //
// f must be a polynomial which we can take the Pth root of. //
// Domain is q=p^m , f a uni/multivariate polynomial         //
///////////////////////////////////////////////////////////////
static CanonicalForm
PthRoot( const CanonicalForm & f ,const CanonicalForm & mipo){
  CanonicalForm RES, R = f;
  int n= max(level(R),getNumVars(R)), p= getCharacteristic();
  int mipodeg=-1;
  if (f.level()==mipo.level()) mipodeg=mipo.degree();
  else if ((f.level()==1) &&(mipo.level()!=-1000000))
  {
    Variable t;
    CanonicalForm tt=getMipo(mipo.mvar(),t);
    mipodeg=degree(tt,t);
  }

  if ((n==0)
  ||(mipodeg!=-1))
  { // constant
    if (R.inExtension()) // not in prime field; f over |F(q=p^k)
    {
      R = power(R,Powerup(p,getGFDegree() - 1)) ;
    }  
    else if ((f.level()==mipo.level())
    ||((f.level()==1) &&(mipo.level()!=-1000000)))
    {
      R = power(R,Powerup(p,mipodeg - 1)) ;
      R=mod(R, mipo);
    }
    // if f in prime field, do nothing
    return R;
  }
  // we assume R is a Pth power here
  RES = R.genZero();
  Variable x(n);
  for (int i=0; i<= (int) (degree(R,level(R))/p) ; i++)
    RES += PthRoot( R[i*p], mipo ) * power(x,i);
  return RES;
}

///////////////////////////////////////////////////////////////
// A uni/multivariate SqrFreeTest routine.                   //
// Cheaper to run if all you want is a test.                 //
// Works for charcteristic 0 and q=p^m                       //
// Returns 1 if poly r is SqrFree, 0 if SqrFree will do some //
// kind of factorization.                                    //
// Would be much more effcient iff we had *good*             //
//  uni/multivariate gcd's and/or gcdtest's                  //
///////////////////////////////////////////////////////////////
int
SqrFreeTest( const CanonicalForm & r, int opt){
  CanonicalForm f=r, g;
  int n=level(f);

  if (getNumVars(f)==0) return 1 ; // a constant is SqrFree
  if ( f.isUnivariate() ) {
    g= f.deriv();
    if ( getCharacteristic() > 0 && g.isZero() ) return 0 ;
    // Next: it would be best to have a *univariate* gcd-test which returns
    // 0 iff gcdtest(f,g) == 1 or a constant ( for real Polynomials )
    g = mygcd(f,g);
    if ( g.isOne() || (-g).isOne() ) return 1;
    else
      if ( getNumVars(g) == 0 ) return 1;// <- totaldegree!!!
      else return 0 ;
  }
  else { // multivariate case
    for ( int k=1; k<=n; k++ ) {
      g = swapvar(f,k,n); g = content(g);
      // g = 1 || -1 : sqr-free, g poly : not sqr-free, g number : opt helps
      if ( ! (g.isOne() || (-g).isOne() || getNumVars(g)==0 ) ) {
        if ( opt==0 ) return 0;
        else {
          if ( SqrFreeTest(g,1) == 0 ) return 0;
          g = swapvar(g,k,n);
          f /=g ;
        }
      }
    }
    // Now f is primitive
    n = level(f); // maybe less indeterminants
    //    if ( totaldegree(f) <= 1 ) return 1;

    // Let`s look if it is a Pth root
    if ( getCharacteristic() > 0 )
      for (int k=1; k<=n; k++ ) {
        g=swapvar(f,k,n); g=g.deriv();
        if ( ! g.isZero() ) break ;
        else if ( k==n) return 0 ; // really is Pth root
      }
    g = f.deriv() ;
    // Next: it would be best to have a *multivariate* gcd-test which returns
    // 0 iff gcdtest(f,g) == 1 or a constant ( for real Polynomials )
    g= mygcd(f,g);
    if ( g.isOne() || (-g).isOne() || (g==f) || (getNumVars(g)==0) ) return 1 ;
    else return 0 ;
  }
#ifdef HAVE_SINGULAR_ERROR
  WerrorS("libfac: ERROR: SqrFreeTest: we should never fall trough here!");
#else
#ifndef NOSTREAMIO
  cerr << "\nlibfac: ERROR: SqrFreeTest: we should never fall trough here!\n"
       << rcsid << errmsg << endl;
#endif
#endif
  return 0;
}

///////////////////////////////////////////////////////////////
// A uni/multivariate SqrFree routine.Works for polynomials  //
// which don\'t have a constant as the content.              //
// Works for charcteristic 0 and q=p^m                       //
// returns a list of polys each of sqrfree, but gcd(f_i,f_j) //
// needs not to be 1 !!!!!                                   //
///////////////////////////////////////////////////////////////
static CFFList
SqrFreed( const CanonicalForm & r , const CanonicalForm &mipo=0){
  CanonicalForm h, g, f = r;
  CFFList Outputlist;
  int n = level(f);

  DEBINCLEVEL(cout, "SqrFreed");
  DEBOUTLN(cout, "Called with r= ", r);
  if (getNumVars(f)==0 ) { // just a constant; return it
    Outputlist= myappend(Outputlist,CFFactor(f,1));
    return Outputlist ;
  }

// We look if we do have a content; if so, SqrFreed the content
// and continue computations with pp(f)
  for (int k=1; k<=n; k++) {
    if ((mipo.level()==-1000000)||(k!=1))
    {
      g = swapvar(f,k,n); g = content(g);
      if ( ! (g.isOne() || (-g).isOne() || degree(g)==0 )) {
        g = swapvar(g,k,n);
        DEBOUTLN(cout, "We have a content: ", g);
        Outputlist = myUnion(InternalSqrFree(g,mipo),Outputlist); // should we add a
                                                // SqrFreeTest(g) first ?
        DEBOUTLN(cout, "Outputlist is now: ", Outputlist);
        f /=g;
        DEBOUTLN(cout, "f is now: ", f);
      }
    }  
  }

// Now f is primitive; Let`s look if f is univariate
  if ( f.isUnivariate() ) {
    DEBOUTLN(cout, "f is univariate: ", f);
    g = content(g);
    if ( ! (g.isOne() || (-g).isOne() ) ){
      Outputlist= myappend(Outputlist,CFFactor(g,1)) ;
      f /= g;
    }
    Outputlist = Union(sqrFree(f),Outputlist) ;
    DEBOUTLN(cout, "Outputlist after univ. sqrFree(f) = ", Outputlist);
    DEBDECLEVEL(cout, "SqrFreed");
    return Outputlist ;
  }

// Linear?
  if ( totaldegree(f) <= 1 ) {
    Outputlist= myappend(Outputlist,CFFactor(f,1)) ;
    DEBDECLEVEL(cout, "SqrFreed");
    return Outputlist ;
  }

// is it Pth root?
  n=level(f); // maybe less indeterminants
  g= f.deriv();
  if ( getCharacteristic() > 0 && g.isZero() ){  // Pth roots only apply to char > 0
    for (int k=1; k<=n; k++)
    {
      if ((mipo.level()==-1000000)||(k!=1))
      {
        g=swapvar(f,k,n) ;
        g = g.deriv();

        if ( ! g.isZero() )
	{ // can`t be Pth root
          CFFList Outputlist2= SqrFreed(swapvar(f,k,n));
          for (CFFListIterator inter=Outputlist2; inter.hasItem(); inter++){
            Outputlist= myappend(Outputlist, CFFactor(swapvar(inter.getItem().factor(),k,n), inter.getItem().exp()));
          }
          return Outputlist;
        }
      }	
      if ( k==n )
      { // really is Pth power
        DEBOUTLN(cout, "f is a p'th root: ", f);
        CFMap m;
        g = compress(f,m);
	if (mipo==0)
          f = m(PthRoot(g));
	else
          f = m(PthRoot(g,mipo));
        DEBOUTLN(cout, "  that is       : ", f);
        // now : Outputlist union ( SqrFreed(f) )^getCharacteristic()
        Outputlist=myUnion(Powerup(InternalSqrFree(f),getCharacteristic()),Outputlist);
        DEBDECLEVEL(cout, "SqrFreed");
        return Outputlist ;
      }
    }
  }
  g = f.deriv();
  DEBOUTLN(cout, "calculating mygcd of ", f);
  DEBOUTLN(cout, "               and ", g);
  h = mygcd(f,pp(g));  h /= lc(h);
  DEBOUTLN(cout,"mygcd(f,g)= ",h);
  if ( (h.isOne()) || ( h==f) || ((-h).isOne()) || getNumVars(h)==0 ) { // no common factor
    Outputlist= myappend(Outputlist,CFFactor(f,1)) ;
    DEBOUTLN(cout, "Outputlist= ", Outputlist);
    DEBDECLEVEL(cout, "SqrFreed");
    return Outputlist ;
  }
  else { // we can split into two nontrivial pieces
    f /= h; // Now we have split the poly into f and h
    g = lc(f);
    if ( g != f.genOne() && getNumVars(g) == 0 ){
       Outputlist= myappend(Outputlist,CFFactor(g,1)) ;
       f /= g;
    }
    DEBOUTLN(cout, "Split into f= ", f);
    DEBOUTLN(cout, "       and h= ", h);
    // For char > 0 the polys f and h can have Pth roots; so we need a test
    // Test is disabled because timing is the same
    
//    if ( SqrFreeTest(f,0) )
//      Outputlist= myappend(Outputlist,CFFactor(f,1)) ;
//    else
    Outputlist=myUnion(Outputlist, InternalSqrFree(f));
//    if ( SqrFreeTest(h,0) )
//      Outputlist= myappend(Outputlist,CFFactor(h,1)) ;
//    else
    Outputlist=myUnion(Outputlist,InternalSqrFree(h));
    DEBOUTLN(cout, "Returning list ", Outputlist);
    DEBDECLEVEL(cout, "SqrFreed");
    return Outputlist ;
  }
#ifdef HAVE_SINGULAR_ERROR
  WerrorS("libfac: ERROR: SqrFreed: we should never fall trough here!");
#else
#ifndef NOSTREAMIO
  cerr << "\nlibfac: ERROR: SqrFreed: we should never fall trough here!\n"
       << rcsid << errmsg << endl;
#endif
#endif
  DEBDECLEVEL(cout, "SqrFreed");
  return Outputlist; // for safety purpose
}

///////////////////////////////////////////////////////////////
// The user front-end for the SqrFreed routine.              //
// Input can have a constant as content                      //
///////////////////////////////////////////////////////////////
CFFList
InternalSqrFree( const CanonicalForm & r , const CanonicalForm & mipo ){
  CanonicalForm g=icontent(r), f = r;
  CFFList Outputlist, Outputlist2;

  DEBINCLEVEL(cout, "InternalSqrFree");
  DEBOUTMSG(cout, rcsid);
  DEBOUTLN(cout,"Called with f= ", f);

  // Take care of stupid users giving us constants
  if ( getNumVars(f) == 0 ) { // a constant ; Exp==1 even if f==0
      Outputlist= myappend(Outputlist,CFFactor(f,1));
  }
  else{
      // Now we are sure: we have a nonconstant polynomial
      g = lc(f);
      while ( getNumVars(g) != 0 ) g=content(g);
      if ( ! g.isOne() ) Outputlist= myappend(Outputlist,CFFactor(g,1)) ;
      f /= g;
      if ( getNumVars(f) != 0 ) // a real polynomial
      {
        if (mipo!=0)
          Outputlist=myUnion(SqrFreed(f,mipo),Outputlist) ;
        else  
          Outputlist=myUnion(SqrFreed(f),Outputlist) ;
      }
  }
  DEBOUTLN(cout,"Outputlist = ", Outputlist);
  for ( CFFListIterator i=Outputlist; i.hasItem(); i++ )
    if ( getNumVars(i.getItem().factor()) > 0 )
      Outputlist2.append(i.getItem());

  DEBOUTLN(cout,"Outputlist2 = ", Outputlist2);
  DEBDECLEVEL(cout, "InternalSqrFree");
  return Outputlist2 ;
}

CFFList
SqrFree(const CanonicalForm & r ){
  CFFList outputlist, sqrfreelist = InternalSqrFree(r);
  CFFListIterator i;
  CanonicalForm elem;
  int n=totaldegree(r);

  DEBINCLEVEL(cout, "SqrFree");

  if ( sqrfreelist.length() < 2 ){
    DEBDECLEVEL(cout, "SqrFree");
    return sqrfreelist;
  }
  for ( int j=1; j<=n; j++ ){
    elem =1;
    for ( i = sqrfreelist; i.hasItem() ; i++ ){
      if ( i.getItem().exp() == j ) elem *= i.getItem().factor();
    }
    if ( !(elem.isOne()) ) outputlist.append(CFFactor(elem,j));
  }
  elem=1;
  for ( i=outputlist; i.hasItem(); i++ )
    if ( getNumVars(i.getItem().factor()) > 0 )
      elem*= power(i.getItem().factor(),i.getItem().exp());
  elem= r/elem;
  outputlist.insert(CFFactor(elem,1));

  DEBOUTLN(cout, "SqrFree returns list:", outputlist);
  DEBDECLEVEL(cout, "SqrFree");
  return outputlist;
}

/*
$Log: not supported by cvs2svn $
Revision 1.8  2002/08/19 11:11:33  Singular
* hannes/pfister: alg_gcd etc.

Revision 1.7  2001/08/08 14:27:38  Singular
*hannes: Dan's HAVE_SINGULAR_ERROR

Revision 1.6  2001/08/08 14:26:56  Singular
*hannes: Dan's HAVE_SINGULAR_ERROR

Revision 1.5  2001/08/08 11:59:13  Singular
*hannes: Dan's NOSTREAMIO changes

Revision 1.4  1997/11/18 16:39:06  Singular
* hannes: moved WerrorS from C++ to C
     (Factor.cc MVMultiHensel.cc SqrFree.cc Truefactor.cc)

Revision 1.3  1997/09/12 07:19:50  Singular
* hannes/michael: libfac-0.3.0

Revision 1.4  1997/04/25 22:19:46  michael
changed cerr and cout messages for use with Singular
Version for libfac-0.2.1

*/
