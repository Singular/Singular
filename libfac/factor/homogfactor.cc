/* Copyright 1997 Michael Messollen. All rights reserved. */
////////////////////////////////////////////////////////////
// static char * rcsid = "$Id: homogfactor.cc,v 1.4 2002-08-19 11:11:34 Singular Exp $ ";
////////////////////////////////////////////////////////////
// FACTORY - Includes
#include <factory.h>
// Factor - Includes
#include "tmpl_inst.h"
#include "Factor.h"
// some CC's need it:
#include "homogfactor.h"

#ifdef HFACTORDEBUG
#  define DEBUGOUTPUT
#else
#  undef DEBUGOUTPUT
#endif

#include "debug.h"
#include "timing.h"
TIMING_DEFINE_PRINT(hfactorize_time);

///////////////////////////////////////////////////////////////
// get_Terms: Split the polynomial in the containing terms.  //
// getTerms: the real work is done here.                     // 
///////////////////////////////////////////////////////////////
static void 
getTerms( const CanonicalForm & f, const CanonicalForm & t, CFList & result ){

  if ( getNumVars(f) == 0 ) result.append(f*t);
  else{ 
    Variable x(level(f));
    for ( CFIterator i=f; i.hasTerms(); i++ )
      getTerms( i.coeff(), t*power(x,i.exp()), result);
  }
}

CFList 
get_Terms( const CanonicalForm & f ){
  CFList result,dummy,dummy2;
  CFIterator i;
  CFListIterator j;

  if ( getNumVars(f) == 0 ) result.append(f);
  else{
    Variable _x(level(f));
    for ( i=f; i.hasTerms(); i++ ){
      getTerms(i.coeff(), 1, dummy);
      for ( j=dummy; j.hasItem(); j++ )
	result.append(j.getItem() * power(_x, i.exp()));

      dummy= dummy2; // have to initalize new
    }
  }
  return result;
}

///////////////////////////////////////////////////////////////
// is_homogeneous returns 1 iff f is homogeneous, 0 otherwise//
///////////////////////////////////////////////////////////////
bool
is_homogeneous( const CanonicalForm & f){
  CFList termlist= get_Terms(f);
  CFListIterator i;
  int deg= totaldegree(termlist.getFirst());

  for ( i=termlist; i.hasItem(); i++ )
    if ( totaldegree(i.getItem()) != deg ) return 0;
  return 1;
}

///////////////////////////////////////////////////////////////
// get_max_degree_Variable returns Variable with             //
// highest degree. We assume f is *not* a constant!          //
///////////////////////////////////////////////////////////////
static Variable
get_max_degree_Variable(const CanonicalForm & f){
  ASSERT( ( ! f.inCoeffDomain() ), "no constants" );
  int max=0, maxlevel=0, n=level(f);
  for ( int i=1; i<=n; i++ )
    if (degree(f,Variable(i)) >= max) {
      max= degree(f,Variable(i)); maxlevel= i;
    }
  return Variable(maxlevel);
}

///////////////////////////////////////////////////////////////
// homogenize homogenizes f with Variable x                  //
///////////////////////////////////////////////////////////////
static CanonicalForm
homogenize( const CanonicalForm & f, const Variable & x){
  CFList Newlist, Termlist= get_Terms(f); 
  int maxdeg=totaldegree(f), deg;
  CFListIterator i;
  CanonicalForm elem, result=f.genZero();
  
  for (i=Termlist; i.hasItem(); i++){
    elem= i.getItem();
    deg = totaldegree(elem);
    if ( deg < maxdeg )
      Newlist.append(elem * power(x,maxdeg-deg));
    else
      Newlist.append(elem);
  }
  for (i=Newlist; i.hasItem(); i++) // rebuild 
    result += i.getItem();
    
  return result;
}

// we assume g is square-free
CFFList
HomogFactor( const CanonicalForm & g, const CanonicalForm  & minpoly, const int Mainvar ){
  DEBINCLEVEL(cout, "HomogFactor");
  Variable xn = get_max_degree_Variable(g);
  int d_xn = degree(g,xn);
  CFMap n;
  CanonicalForm F = compress(g(1,xn),n); // must compress F! 

  DEBOUTLN(cout, "xn= ", xn);
  DEBOUTLN(cout, "d_xn=   ", d_xn);
  DEBOUTLN(cout, "F= ", F);  

  // should we do this for low degree polys g ? e.g. quadratic?
  // 
  CFFList Intermediatelist;
  if ( getCharacteristic() > 0 )
     Intermediatelist = Factorized(F, minpoly, Mainvar);
  else
     Intermediatelist = factorize(F); // what support for char==0 ?
  CFFList Homoglist;
  CFFListIterator j;
  for ( j=Intermediatelist; j.hasItem(); j++ )
    Homoglist.append(CFFactor( n(j.getItem().factor()), j.getItem().exp()) );
  // Now we have uncompressed factors in Homoglist
  DEBOUTLN(cout, "F factors as: ", Homoglist);
  CFFList Unhomoglist;
  CanonicalForm unhomogelem;
  for ( j=Homoglist; j.hasItem(); j++ ){
    DEBOUTLN(cout, "Homogenizing ",j.getItem().factor()); 
    unhomogelem= homogenize(j.getItem().factor(),xn);
    DEBOUTLN(cout, "      that is ", unhomogelem);
    Unhomoglist.append(CFFactor(unhomogelem,j.getItem().exp()));
    d_xn -= degree(unhomogelem,xn);
  }
  DEBOUTLN(cout, "Power of xn to append is ", d_xn);
  if ( d_xn != 0 ) // have to append xn^(d_xn)
    Unhomoglist.append(CFFactor(CanonicalForm(xn),d_xn));

  DEBDECLEVEL(cout, "HomogFactor");
  return Unhomoglist;
}

/*
$Log: not supported by cvs2svn $
Revision 1.3  1997/09/12 07:19:57  Singular
* hannes/michael: libfac-0.3.0

Revision 1.2  1997/04/25 22:35:20  michael
changed cerr and cout messages for use with Singular
Version for libfac-0.2.1

Revision 1.1  1997/01/17 05:05:48  michael
Initial revision

*/
