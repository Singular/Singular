/* Copyright 1996 Michael Messollen. All rights reserved. */
///////////////////////////////////////////////////////////////////////////////
// emacs edit mode for this file is -*- C++ -*-
static char * rcsid = "$Id: SqrFree.cc,v 1.14 2008-01-25 14:19:40 Singular Exp $";
static char * errmsg = "\nYou found a bug!\nPlease inform (Michael Messollen) michael@math.uni-sb.de .\n Please include above information and your input (the ideal/polynomial and characteristic) in your bug-report.\nThank you.";
///////////////////////////////////////////////////////////////////////////////
// FACTORY - Includes
#include<factory.h>
#ifndef NOSTREAMIO
#ifdef HAVE_IOSTREAM
#include <iostream>
#define OSTREAM std::ostream
#define ISTREAM std::istream
#define CERR std::cerr
#define CIN std::cin
#elif defined(HAVE_IOSTREAM_H)
#include <iostream.h>
#define OSTREAM ostream
#define ISTREAM istream
#define CERR cerr
#define CIN cin
#endif
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
// A uni/multivariate SqrFree routine.Works for polynomials  //
// which don\'t have a constant as the content.              //
// Works for charcteristic 0 and q=p^m                       //
// returns a list of polys each of sqrfree, but gcd(f_i,f_j) //
// needs not to be 1 !!!!!                                   //
///////////////////////////////////////////////////////////////

CFFList
SqrFree(const CanonicalForm & r ){
  CFFList outputlist, sqrfreelist = sqrFree(r,0,false);
  CFFListIterator i;
  CanonicalForm elem;
  int n=totaldegree(r);

  DEBINCLEVEL(CERR, "SqrFree");

  if ( sqrfreelist.length() < 2 ){
    DEBDECLEVEL(CERR, "SqrFree");
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

  DEBOUTLN(CERR, "SqrFree returns list:", outputlist);
  DEBDECLEVEL(CERR, "SqrFree");
  return outputlist;
}

/*
$Log: not supported by cvs2svn $
Revision 1.13  2008/01/22 09:51:37  Singular
*hannes: sqrFree/InternalSqrFree -> factory

Revision 1.12  2008/01/07 13:34:56  Singular
*hannes: omse optiomzations(isOne)

Revision 1.11  2007/05/15 14:46:49  Singular
*hannes: factorize in Zp(a)[x...]

Revision 1.10  2006/05/16 14:46:50  Singular
*hannes: gcc 4.1 fixes

Revision 1.9  2006/04/28 13:46:29  Singular
*hannes: better tests for 0, 1

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
